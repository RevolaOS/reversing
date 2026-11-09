// SPDX-License-Identifier: GPL-3.0-only
// Apply saved annotations (types, functions, symbols, comments, bookmarks) to an
// auto-analysed program, then check that every saved annotation is present.
// Arguments: overlay directory, project folder. The overlay is <dir>/<folder>/<name>.sarif.
// @category Revola

import com.google.gson.*;
import ghidra.app.script.GhidraScript;
import ghidra.app.util.bin.FileByteProvider;
import ghidra.app.util.importer.MessageLog;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.Symbol;
import java.nio.file.*;
import java.nio.file.AccessMode;
import java.util.*;
import sarif.managers.ProgramSarifMgr;

public class ApplyAnalysis extends GhidraScript {
  private final List<String> missing = new ArrayList<>();

  @Override
  public void run() throws Exception {
    String[] args = getScriptArgs();
    String path = args[1] + "/" + currentProgram.getName();
    Path file = Path.of(args[0], path.replaceFirst("\\.elf$", ".sarif"));
    byte[] bytes = Files.readAllBytes(file);
    JsonObject run =
        JsonParser.parseString(new String(bytes)).getAsJsonObject().getAsJsonArray("runs").get(0)
            .getAsJsonObject();
    // New functions can call code auto-analysis never reached; analyse the
    // changes so their callees exist before the annotations are checked.
    if (createFunctions(run)) analyzeChanges(currentProgram);
    try (var provider = new FileByteProvider(file.toFile(), null, AccessMode.READ)) {
      new ProgramSarifMgr(provider, new MessageLog()).read(currentProgram, monitor);
    }

    Map<String, Integer> counts = new TreeMap<>();
    for (JsonElement element : run.getAsJsonArray("results")) {
      JsonObject row = element.getAsJsonObject();
      String rule = row.get("ruleId").getAsString();
      JsonObject p = row.getAsJsonObject("properties").getAsJsonObject("additionalProperties");
      switch (rule) {
        case "FUNCTIONS" -> checkFunction(p);
        case "SYMBOLS" -> checkSymbol(location(row), p);
        case "COMMENTS" -> checkComment(location(row), p);
        case "BOOKMARKS" -> checkBookmark(location(row), p);
        default -> {}
      }
      counts.merge(rule.toLowerCase(), 1, Integer::sum);
    }
    if (!missing.isEmpty()) {
      missing.stream().limit(20).forEach(line -> printerr("MISSING " + line));
      throw new IllegalStateException(missing.size() + " saved annotations did not apply");
    }
    println("APPLIED " + path + ": " + counts);
  }

  // Auto-analysis misses some saved functions, often ones reached only through
  // tables or folded into a neighbour. The SARIF reader does not create them.
  // Saved names win over names already in the program.
  private boolean createFunctions(JsonObject run) throws Exception {
    boolean created = false;
    for (JsonElement element : run.getAsJsonArray("results")) {
      JsonObject row = element.getAsJsonObject();
      if (!row.get("ruleId").getAsString().equals("FUNCTIONS")) continue;
      JsonObject p = row.getAsJsonObject("properties").getAsJsonObject("additionalProperties");
      Address entry = toAddr(p.get("location").getAsString());
      String name = p.get("name").getAsString();
      Function function = getFunctionAt(entry);
      if (function == null) {
        Function owner = getFunctionContaining(entry);
        if (getInstructionAt(entry) == null) disassemble(entry);
        function = createFunction(entry, null);
        created |= function != null;
        if (function == null) {
          printerr("cannot create " + name + " at " + entry
              + (owner == null ? "" : " inside " + owner.getName()));
          continue;
        }
      }
      // The SARIF reader adds a second label instead of renaming a function
      // that already has a user-defined name.
      if (!p.get("sourceType").getAsString().equals("DEFAULT") && !function.getName().equals(name)) {
        for (Symbol label : currentProgram.getSymbolTable().getSymbols(entry))
          if (label.getName().equals(name) && !label.equals(function.getSymbol())) label.delete();
        function.setName(name, SourceType.valueOf(p.get("sourceType").getAsString()));
      }
    }
    return created;
  }

  private Address location(JsonObject row) {
    return toAddr(
        row.getAsJsonArray("locations").get(0).getAsJsonObject()
            .getAsJsonObject("physicalLocation").getAsJsonObject("address")
            .get("absoluteAddress").getAsLong());
  }

  private void checkFunction(JsonObject p) throws Exception {
    Address entry = toAddr(p.get("location").getAsString());
    Function function = getFunctionAt(entry);
    String name = p.get("name").getAsString();
    if (function == null) {
      missing.add("function " + name + " at " + entry);
      return;
    }
    // Thunks are restored here because the SARIF reader resolves them before
    // every target exists.
    if (p.has("thunkTarget") && p.get("thunkTarget").getAsString().matches("[0-9a-fA-F]{8}")) {
      Address target = toAddr(p.get("thunkTarget").getAsString());
      Function thunked = getFunctionAt(target);
      if (thunked == null && currentProgram.getMemory().getExecuteSet().contains(target)) {
        disassemble(target);
        thunked = createFunction(target, null);
      }
      if (thunked != null) function.setThunkedFunction(thunked);
    }
    if (!p.get("sourceType").getAsString().equals("DEFAULT") && !function.getName().equals(name))
      missing.add("name " + name + " at " + entry + " (is " + function.getName() + ")");
    if (!p.get("signatureSource").getAsString().equals("DEFAULT")
        && function.getSignatureSource() == SourceType.DEFAULT)
      missing.add("signature of " + name);
  }

  private void checkSymbol(Address address, JsonObject p) {
    String name = p.get("name").getAsString();
    for (Symbol symbol : currentProgram.getSymbolTable().getSymbols(address))
      if (symbol.getName().equals(name)) return;
    missing.add("symbol " + name + " at " + address);
  }

  private void checkComment(Address address, JsonObject p) {
    CommentType type =
        switch (p.get("kind").getAsString()) {
          case "plate" -> CommentType.PLATE;
          case "pre" -> CommentType.PRE;
          case "post" -> CommentType.POST;
          case "repeatable" -> CommentType.REPEATABLE;
          default -> CommentType.EOL;
        };
    String comment = currentProgram.getListing().getComment(type, address);
    if (comment == null || !comment.contains(p.get("value").getAsString()))
      missing.add("comment at " + address);
  }

  private void checkBookmark(Address address, JsonObject p) {
    var bookmark =
        currentProgram.getBookmarkManager().getBookmark(
            address, p.get("kind").getAsString(), p.get("name").getAsString());
    if (bookmark == null) missing.add("bookmark at " + address);
  }
}
