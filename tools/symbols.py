# SPDX-License-Identifier: GPL-3.0-only
"""Generate the linker script and C header from a symbols file, or check them.

    python tools/symbols.py generate symbols/osos-2.0.5.toml
    python tools/symbols.py check symbols/osos-2.0.5.toml
    python tools/symbols.py verify symbols/osos-2.0.5.toml --ghidra <analysis dir>

generate writes <name>.ld and <name>.h next to the symbols file.
check fails when the written files differ from what generate would produce.
verify compares every function address against the Ghidra export.
"""

import argparse
import json
import re
import sys
import tomllib
from pathlib import Path

IDENT = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
STRUCT = re.compile(r"\bstruct\s+([A-Za-z_][A-Za-z0-9_]*)")


def load(path):
    data = tomllib.loads(path.read_text())
    seen = {}
    for table in ("functions", "data", "sites"):
        for name, entry in data.get(table, {}).items():
            if not IDENT.match(name):
                raise SystemExit(f"{name}: not a C identifier")
            if "at" not in entry:
                raise SystemExit(f"{name}: missing address")
            if name in seen:
                raise SystemExit(f"{name}: defined in {seen[name]} and {table}")
            seen[name] = table
    return data


def linker_script(data):
    lines = [
        f"/* Generated from the symbols file for OSOS {data['firmware']['version']}. */"
    ]
    for name, entry in data.get("functions", {}).items():
        address = entry["at"] | (1 if entry.get("thumb") else 0)
        lines.append(f"{name} = 0x{address:08x};")
    for name, entry in data.get("data", {}).items():
        lines.append(f"{name} = 0x{entry['at']:08x};")
    return "\n".join(lines) + "\n"


def header(data):
    firmware = data["firmware"]
    out = [
        f"/* Generated from the symbols file for OSOS {firmware['version']}. */",
        "#pragma once",
        "#include <stdint.h>",
        "",
        f'#define OSOS_VERSION "{firmware["version"]}"',
        f"#define OSOS_RUNTIME_BASE 0x{firmware['runtime_base']:08x}",
        f"#define OSOS_RUNTIME_FILE_OFFSET 0x{firmware['runtime_file_offset']:08x}",
        f"#define OSOS_NATIVE_HEAP_BASE 0x{firmware['native_heap_base']:08x}",
        "",
    ]
    structs = set()
    for table in ("functions", "data"):
        for entry in data.get(table, {}).values():
            structs.update(STRUCT.findall(entry.get("c", "")))
    for name in sorted(structs):
        out.append(f"struct {name};")
    if structs:
        out.append("")
    for name, entry in data.get("functions", {}).items():
        if "c" in entry:
            out.append(f"{entry['c']};")
    out.append("")
    for name, entry in data.get("data", {}).items():
        out.append(f"extern {entry['c']};")
    out.append("")
    for name, entry in data.get("sites", {}).items():
        out.append(f"#define SITE_{name} 0x{entry['at']:08x}")
    out.append("")
    for struct, fields in data.get("offsets", {}).items():
        for field, value in fields.items():
            out.append(f"#define {struct}_{field} 0x{value:x}")
    out.append("")
    for name, value in data.get("constants", {}).items():
        out.append(f"#define {name} 0x{value:x}")
    out.append("")
    for name, value in data.get("resources", {}).items():
        out.append(f"#define RES_{name} 0x{value:08x}")
    return "\n".join(out) + "\n"


def outputs(path):
    return path.with_suffix(".ld"), path.with_suffix(".h")


def generate(path):
    data = load(path)
    ld, h = outputs(path)
    ld.write_text(linker_script(data))
    h.write_text(header(data))
    print(f"wrote {ld.name} and {h.name}")


def check(path):
    data = load(path)
    ld, h = outputs(path)
    stale = [
        f.name
        for f, text in ((ld, linker_script(data)), (h, header(data)))
        if not f.exists() or f.read_text() != text
    ]
    if stale:
        raise SystemExit(f"out of date: {', '.join(stale)}; run generate")
    print("up to date")


def verify(path, analysis):
    """Functions must start a Ghidra function or lie in a gap; sites must match their "in"."""
    data = load(path)
    functions = {}
    for line in (analysis / "functions.jsonl").open():
        record = json.loads(line)
        address = record["locations"][0]["physicalLocation"]["address"]
        properties = record["properties"]["additionalProperties"]
        functions[address["absoluteAddress"]] = (properties["name"], address["length"])
    starts = sorted(functions)

    def containing(address):
        import bisect

        index = bisect.bisect_right(starts, address) - 1
        if index >= 0 and address < starts[index] + functions[starts[index]][1]:
            return starts[index]
        return None

    problems = []
    undefined = []
    for name, entry in data["functions"].items():
        ghidra = functions.get(entry["at"])
        if ghidra is None:
            if containing(entry["at"]) is None:
                undefined.append(name)
            else:
                problems.append(
                    f"{name}: 0x{entry['at']:08x} is inside another function"
                )
        elif ghidra[0] != name and not ghidra[0].startswith("FUN_"):
            problems.append(f"{name}: Ghidra calls 0x{entry['at']:08x} {ghidra[0]}")
    for name, entry in data["sites"].items():
        if "in" not in entry:
            continue
        function, offset = entry["in"].split("+")
        start = data["functions"].get(function, {}).get("at")
        if start is None:
            problems.append(f"{name}: {function} is not in [functions]")
        elif start + int(offset, 16) != entry["at"]:
            problems.append(f"{name}: {entry['in']} is not 0x{entry['at']:08x}")
        elif containing(entry["at"]) != start:
            problems.append(
                f"{name}: Ghidra puts 0x{entry['at']:08x} outside {function}"
            )
    for problem in problems:
        print(problem)
    if problems:
        raise SystemExit(1)
    for name in undefined:
        print(f"{name}: not a function in Ghidra yet (code in a gap between functions)")
    print(f"verified {len(data['functions'])} functions and {len(data['sites'])} sites")


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    parser.add_argument("command", choices=["generate", "check", "verify"])
    parser.add_argument("symbols", type=Path)
    parser.add_argument(
        "--ghidra", type=Path, help="Ghidra analysis directory for verify"
    )
    args = parser.parse_args()
    if args.command == "generate":
        generate(args.symbols)
    elif args.command == "check":
        check(args.symbols)
    else:
        if not args.ghidra:
            parser.error("verify needs --ghidra")
        verify(args.symbols, args.ghidra)


if __name__ == "__main__":
    sys.exit(main())
