// SPDX-License-Identifier: GPL-3.0-only
// Look up symbols-file names in the program and write what Ghidra knows about each.
// Arguments: input JSON {"functions": {name: at}, "data": {name: at}}, output JSON path.
// A function still at its symbols-file address wins over others with the same name;
// a thunk shows its target's name.
// Prototypes are written only for functions whose signature is not DEFAULT, in the
// symbols-file style ("struct X *name", stdint names). A thunk gets none, because its
// signature is its target's, and neither does a prototype with a type C cannot name.
// @category Revola

import com.google.gson.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.Symbol;
import java.math.BigInteger;
import java.nio.file.*;
import java.util.*;

public class ReadSymbols extends GhidraScript {
  @Override
  public void run() throws Exception {
    String[] args = getScriptArgs();
    JsonObject names = JsonParser.parseString(Files.readString(Path.of(args[0]))).getAsJsonObject();
    JsonObject out = new JsonObject();
    JsonObject functions = new JsonObject();
    JsonObject data = new JsonObject();
    for (var item : names.getAsJsonObject("functions").entrySet()) {
      String name = item.getKey();
      long expected = item.getValue().getAsLong();
      List<Function> found = new ArrayList<>();
      Function atExpected = getFunctionAt(toAddr(expected));
      if (atExpected != null && atExpected.getName().equals(name)) found.add(atExpected);
      else for (Symbol symbol : currentProgram.getSymbolTable().getGlobalSymbols(name)) {
        Function function = getFunctionAt(symbol.getAddress());
        if (function != null && function.getName().equals(name)) found.add(function);
      }
      if (found.size() != 1) {
        if (found.size() > 1) println("AMBIGUOUS " + name);
        continue;
      }
      Function function = found.get(0);
      JsonObject entry = new JsonObject();
      entry.addProperty("at", function.getEntryPoint().getOffset());
      BigInteger mode =
          currentProgram.getProgramContext()
              .getValue(currentProgram.getRegister("TMode"), function.getEntryPoint(), false);
      entry.addProperty("thumb", BigInteger.ONE.equals(mode));
      String prototype =
          function.isThunk() || function.getSignatureSource() == SourceType.DEFAULT
              ? null
              : prototype(function);
      if (prototype != null) entry.addProperty("c", prototype);
      functions.add(name, entry);
    }
    for (var item : names.getAsJsonObject("data").entrySet()) {
      String name = item.getKey();
      long expected = item.getValue().getAsLong();
      List<Symbol> found = new ArrayList<>(currentProgram.getSymbolTable().getGlobalSymbols(name));
      if (found.size() > 1) found.removeIf(s -> s.getAddress().getOffset() != expected);
      if (found.size() == 1) {
        JsonObject entry = new JsonObject();
        entry.addProperty("at", found.get(0).getAddress().getOffset());
        data.add(name, entry);
      } else if (found.size() > 1) println("AMBIGUOUS " + name);
    }
    out.add("functions", functions);
    out.add("data", data);
    Files.writeString(Path.of(args[1]), new Gson().toJson(out));
    println("READ " + functions.size() + " functions, " + data.size() + " data");
  }

  private static final Map<String, String> BUILTIN =
      Map.ofEntries(
          Map.entry("uint", "uint32_t"),
          Map.entry("ushort", "uint16_t"),
          Map.entry("uchar", "uint8_t"),
          Map.entry("byte", "uint8_t"),
          Map.entry("sbyte", "int8_t"),
          Map.entry("ulonglong", "uint64_t"),
          Map.entry("longlong", "int64_t"),
          Map.entry("short", "int16_t"),
          Map.entry("ulong", "uint32_t"),
          Map.entry("long", "int32_t"));
  private static final Set<String> C_NAMES =
      Set.of("void", "char", "int", "float", "double", "uint8_t", "int8_t", "uint16_t",
          "int16_t", "uint32_t", "int32_t", "uint64_t", "int64_t", "uintptr_t", "intptr_t",
          "size_t");

  private String prototype(Function function) {
    StringBuilder text = new StringBuilder();
    String result = declare(function.getReturnType(), function.getName());
    if (result == null) return null;
    text.append(result).append('(');
    Parameter[] parameters = function.getParameters();
    for (int i = 0; i < parameters.length; i++) {
      String parameter = declare(parameters[i].getDataType(), parameters[i].getName());
      if (parameter == null) return null;
      if (i > 0) text.append(", ");
      text.append(parameter);
    }
    if (function.hasVarArgs()) text.append(parameters.length > 0 ? ", ..." : "...");
    else if (parameters.length == 0) text.append("void");
    return text.append(')').toString();
  }

  private String declare(DataType type, String name) {
    String stars = "";
    while (type instanceof Pointer pointer && pointer.getDataType() != null) {
      stars += "*";
      type = pointer.getDataType();
    }
    if (type instanceof Pointer) {
      stars += "*";
      type = VoidDataType.dataType;
    }
    String base = type.getDisplayName();
    if (type instanceof Structure) base = "struct " + base;
    else if (type instanceof Union) base = "union " + base;
    else if (type instanceof ghidra.program.model.data.Enum) base = "enum " + base;
    else base = BUILTIN.getOrDefault(base, base);
    if (!(type instanceof Composite || type instanceof ghidra.program.model.data.Enum)
        && !C_NAMES.contains(base)) return null;
    return base + " " + stars + name;
  }
}
