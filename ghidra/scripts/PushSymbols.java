// SPDX-License-Identifier: GPL-3.0-only
// Import symbols-file names into the program: functions get their name and prototype,
// data gets a label, sites get a Note bookmark in category "site".
// Argument: JSON written by tools/symbols.py push.
// @category Revola

import com.google.gson.*;
import ghidra.app.cmd.function.ApplyFunctionSignatureCmd;
import ghidra.app.cmd.function.FunctionRenameOption;
import ghidra.app.script.GhidraScript;
import ghidra.app.util.cparser.C.CParser;
import ghidra.program.model.address.Address;
import ghidra.program.model.data.FunctionDefinition;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.Symbol;
import java.math.BigInteger;
import java.nio.file.*;
import java.util.*;

public class PushSymbols extends GhidraScript {
  private static final String STDINT =
      "typedef unsigned char uint8_t; typedef signed char int8_t;"
          + " typedef unsigned short uint16_t; typedef short int16_t;"
          + " typedef unsigned int uint32_t; typedef int int32_t;"
          + " typedef unsigned long long uint64_t; typedef long long int64_t;"
          + " typedef unsigned int uintptr_t; typedef int intptr_t; typedef unsigned int size_t;";

  private final List<String> failed = new ArrayList<>();

  @Override
  public void run() throws Exception {
    JsonObject symbols =
        JsonParser.parseString(Files.readString(Path.of(getScriptArgs()[0]))).getAsJsonObject();
    CParser parser = new CParser(currentProgram.getDataTypeManager(), false, null);
    parser.parse(STDINT);
    int functions = 0, data = 0, sites = 0;
    for (JsonElement element : symbols.getAsJsonArray("functions")) {
      if (pushFunction(element.getAsJsonObject(), parser)) functions++;
    }
    for (JsonElement element : symbols.getAsJsonArray("data")) {
      JsonObject entry = element.getAsJsonObject();
      if (label(toAddr(entry.get("at").getAsLong()), entry.get("name").getAsString())) data++;
    }
    var bookmarks = currentProgram.getBookmarkManager();
    bookmarks.removeBookmarks(BookmarkType.NOTE, "site", monitor);
    for (JsonElement element : symbols.getAsJsonArray("sites")) {
      JsonObject entry = element.getAsJsonObject();
      String comment = entry.get("name").getAsString() + ": " + entry.get("kind").getAsString();
      if (entry.has("in")) comment += " in " + entry.get("in").getAsString();
      bookmarks.setBookmark(toAddr(entry.get("at").getAsLong()), BookmarkType.NOTE, "site", comment);
      sites++;
    }
    failed.forEach(line -> println("FAILED " + line));
    println("PUSHED " + functions + " functions, " + data + " data, " + sites + " sites, "
        + failed.size() + " failed");
  }

  private boolean pushFunction(JsonObject entry, CParser parser) throws Exception {
    String name = entry.get("name").getAsString();
    Address address = toAddr(entry.get("at").getAsLong());
    Function function = getFunctionAt(address);
    Function owner = function == null ? getFunctionContaining(address) : null;
    if (function == null) {
      if (getInstructionAt(address) == null) {
        BigInteger mode = entry.get("thumb").getAsBoolean() ? BigInteger.ONE : BigInteger.ZERO;
        currentProgram.getProgramContext()
            .setValue(currentProgram.getRegister("TMode"), address, address, mode);
        disassemble(address);
      }
      function = createFunction(address, name);
      if (function == null) {
        failed.add(name + ": cannot create a function at " + address);
        return false;
      }
    }
    for (Symbol other : currentProgram.getSymbolTable().getGlobalSymbols(name)) {
      Function holder = getFunctionAt(other.getAddress());
      if (other.getAddress().equals(address) || holder == null) continue;
      if (holder.getThunkedFunction(false) == function
          || function.getThunkedFunction(false) == holder) continue;
      // Auto-analysis can merge a function reached by a tail jump into its caller,
      // and the name then lands on the caller. Splitting gives the name back.
      if (holder == owner) {
        holder.setName(null, SourceType.DEFAULT);
        println("SPLIT " + name + " out of the function at " + holder.getEntryPoint());
      } else {
        failed.add(name + ": already names the function at " + other.getAddress());
        return false;
      }
    }
    function.setName(name, SourceType.USER_DEFINED);
    if (!entry.has("c")) return true;
    String prototype = entry.get("c").getAsString();
    try {
      parser.parse(prototype + ";");
    } catch (Exception error) {
      failed.add(name + ": cannot parse " + prototype + ": " + error.getMessage());
      return false;
    }
    if (!(parser.getLastDataType() instanceof FunctionDefinition signature)) {
      failed.add(name + ": not a function prototype: " + prototype);
      return false;
    }
    var command =
        new ApplyFunctionSignatureCmd(
            address, signature, SourceType.USER_DEFINED, true, FunctionRenameOption.RENAME);
    if (!command.applyTo(currentProgram, monitor)) {
      failed.add(name + ": " + command.getStatusMsg());
      return false;
    }
    return true;
  }

  private boolean label(Address address, String name) throws Exception {
    for (Symbol symbol : currentProgram.getSymbolTable().getSymbols(address)) {
      if (symbol.getName().equals(name)) {
        symbol.setPrimary();
        return true;
      }
    }
    Symbol symbol = createLabel(address, name, true, SourceType.USER_DEFINED);
    if (symbol == null) failed.add(name + ": cannot label " + address);
    return symbol != null;
  }
}
