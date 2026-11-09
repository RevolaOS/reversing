# SPDX-License-Identifier: GPL-3.0-only
"""Generate, check and verify a symbols file, and sync it with a Ghidra project.

    python tools/symbols.py generate symbols/osos-2.0.5.toml
    python tools/symbols.py check symbols/osos-2.0.5.toml
    python tools/symbols.py verify symbols/osos-2.0.5.toml [--analysis <dir>]
    python tools/symbols.py push symbols/osos-2.0.5.toml --ghidra <project>
    python tools/symbols.py refresh symbols/osos-2.0.5.toml --ghidra <project>

generate writes <name>.ld and <name>.h next to the symbols file.
check fails when the written files differ from what generate would produce.
verify compares every function and site with the exported analysis.
push names the project's functions and data after the symbols file and applies the
prototypes; sites become Note bookmarks in category "site".
refresh rewrites the address, thumb flag and prototype of existing entries from the
project. It never adds or removes entries.
"""

import argparse
import bisect
import json
import re
import sys
import tempfile
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ANALYSIS = ROOT / "ghidra/analysis/2.0.5/osos"

IDENT = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
STRUCT = re.compile(r"\bstruct\s+([A-Za-z_][A-Za-z0-9_]*)")
SPDX = "/* SPDX-License-Identifier: GPL-3.0-only */"


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
        SPDX,
        f"/* Generated from the symbols file for OSOS {data['firmware']['version']}. */",
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
        SPDX,
        f"/* Generated from the symbols file for OSOS {firmware['version']}. */",
        "#pragma once",
        "",
        f'#define OSOS_VERSION "{firmware["version"]}"',
        f"#define OSOS_RUNTIME_BASE 0x{firmware['runtime_base']:08x}",
        f"#define OSOS_RUNTIME_FILE_OFFSET 0x{firmware['runtime_file_offset']:08x}",
        f"#define OSOS_NATIVE_HEAP_BASE 0x{firmware['native_heap_base']:08x}",
        "",
    ]
    out.append("#ifndef __ASSEMBLER__")
    out.append("#include <stdint.h>")
    out.append("")
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
    out.append("#endif")
    out.append("")
    for table in ("functions", "data"):
        for name, entry in data.get(table, {}).items():
            out.append(f"#define ADDR_{name} 0x{entry['at']:08x}")
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


def load_functions(analysis):
    """Entry -> (name, body ranges) from the exported functions."""
    sys.path.insert(0, str(ROOT / "tools"))
    from ghidra_analysis import read

    functions = {}
    for record in read(analysis, "functions"):
        properties = record["properties"]["additionalProperties"]
        ranges = [
            (a["absoluteAddress"], a["absoluteAddress"] + a["length"])
            for a in (loc["physicalLocation"]["address"] for loc in record["locations"])
        ]
        functions[int(properties["location"], 16)] = (properties["name"], ranges)
    return functions


def verify(path, analysis):
    """Functions must start a Ghidra function or lie in a gap; sites must match their "in"."""
    data = load(path)
    functions = load_functions(analysis)
    ranges = sorted(
        (start, end, entry)
        for entry, (_, body) in functions.items()
        for start, end in body
    )
    starts = [r[0] for r in ranges]

    def containing(address):
        index = bisect.bisect_right(starts, address) - 1
        if index >= 0 and address < ranges[index][1]:
            return ranges[index][2]
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
        print(f"{name}: not a function in Ghidra yet")
    print(f"verified {len(data['functions'])} functions and {len(data['sites'])} sites")


def run_script(project, script, arguments, phase, marker):
    sys.path.insert(0, str(ROOT / "tools"))
    from ghidra_project import headless, preflight, project_dir, require_closed

    manifest = json.loads((ROOT / "ghidra/programs.json").read_text())
    preflight(manifest["ghidra_version"])
    project = project_dir(project)
    require_closed(project)
    output = headless(
        project,
        manifest["programs"][0]["path"].rsplit("/", 1)[0],
        ["-process", "osos.elf", "-postScript", script, *arguments],
        phase,
        [marker],
    )
    for line in output.splitlines():
        for tag in ("FAILED ", "SPLIT ", "AMBIGUOUS ", "PUSHED ", "READ "):
            if f"> {tag}" in line:
                print(tag + line.split(f"> {tag}", 1)[1].split(" (GhidraScript)")[0])
    return output


def push(path, project):
    data = load(path)
    symbols = {
        "functions": [
            {"name": n, "at": e["at"], "thumb": bool(e.get("thumb"))}
            | ({"c": e["c"]} if "c" in e else {})
            for n, e in data.get("functions", {}).items()
        ],
        "data": [{"name": n, "at": e["at"]} for n, e in data.get("data", {}).items()],
        "sites": [
            {"name": n, "at": e["at"], "kind": e["kind"]}
            | ({"in": e["in"]} if "in" in e else {})
            for n, e in data.get("sites", {}).items()
        ],
    }
    with tempfile.TemporaryDirectory() as temporary:
        request = Path(temporary) / "symbols.json"
        request.write_text(json.dumps(symbols))
        output = run_script(
            project, "PushSymbols.java", [str(request)], "push", "PUSHED "
        )
    if "> FAILED " in output:
        raise SystemExit(1)


ENTRY = re.compile(r"^(?P<name>[A-Za-z_][A-Za-z0-9_]*) = \{ (?P<body>.*) \}$")
FIELD = re.compile(r'(?P<key>[a-z]+) = (?P<value>"(?:[^"\\]|\\.)*"|[^,]+)')


def same_prototype(left, right):
    def normal(text):
        text = re.sub(r"\bconst\b", "", text)
        return re.sub(r"\s+", " ", re.sub(r"\s*([*(),])\s*", r"\1", text)).strip()

    return normal(left) == normal(right)


def format_entry(name, fields):
    parts = []
    for key, value in fields.items():
        if isinstance(value, bool):
            parts.append(f"{key} = {'true' if value else 'false'}")
        elif isinstance(value, int):
            parts.append(f"{key} = 0x{value:08x}")
        elif key in ("c", "kind", "in"):
            parts.append(f"{key} = {json.dumps(value)}")
        else:
            parts.append(f"{key} = {value}")
    return f"{name} = {{ {', '.join(parts)} }}"


def refresh(path, project):
    data = load(path)
    with tempfile.TemporaryDirectory() as temporary:
        request = Path(temporary) / "names.json"
        result = Path(temporary) / "found.json"
        request.write_text(
            json.dumps(
                {
                    table: {n: e["at"] for n, e in data.get(table, {}).items()}
                    for table in ("functions", "data")
                }
            )
        )
        run_script(
            project,
            "ReadSymbols.java",
            [str(request), str(result)],
            "refresh",
            "READ ",
        )
        found = json.loads(result.read_text())
    moved = {}
    lines = path.read_text().splitlines(keepends=True)
    table = None
    changed = 0
    for index, line in enumerate(lines):
        header = re.match(r"^\[([a-z.A-Za-z]+)\]", line)
        if header:
            table = header[1]
            continue
        match = ENTRY.match(line.rstrip("\n"))
        if not match or table not in ("functions", "data", "sites"):
            continue
        name = match["name"]
        entry = dict(data[table][name])
        if table == "sites":
            if "in" not in entry:
                continue
            function, offset = entry["in"].split("+")
            if function not in moved:
                continue
            entry["at"] = moved[function] + int(offset, 16)
        else:
            ghidra = found[table].get(name)
            if ghidra is None:
                print(f"{name}: not in the project")
                continue
            if table == "functions":
                if ghidra["at"] != entry["at"]:
                    moved[name] = ghidra["at"]
                if ghidra["thumb"]:
                    entry["thumb"] = True
                else:
                    entry.pop("thumb", None)
                if "c" in entry and "c" in ghidra:
                    if not same_prototype(entry["c"], ghidra["c"]):
                        entry["c"] = ghidra["c"]
            entry["at"] = ghidra["at"]
        ordered = {"at": entry.pop("at"), **entry}
        if "thumb" in ordered:
            ordered = {
                "at": ordered["at"],
                "thumb": ordered.pop("thumb"),
                **{k: v for k, v in ordered.items() if k != "at"},
            }
        text = format_entry(name, ordered) + "\n"
        if text != line and tomllib.loads(text) != tomllib.loads(line):
            lines[index] = text
            changed += 1
    path.write_text("".join(lines))
    print(f"refreshed {changed} entries in {path.name}")


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    parser.add_argument(
        "command", choices=["generate", "check", "verify", "push", "refresh"]
    )
    parser.add_argument("symbols", type=Path)
    parser.add_argument(
        "--analysis", type=Path, default=ANALYSIS, help="exported analysis for verify"
    )
    parser.add_argument("--ghidra", type=Path, help="Ghidra project for push/refresh")
    args = parser.parse_args()
    if args.command in ("push", "refresh") and not args.ghidra:
        parser.error(f"{args.command} needs --ghidra <project>")
    if args.command == "generate":
        generate(args.symbols)
    elif args.command == "check":
        check(args.symbols)
    elif args.command == "verify":
        verify(args.symbols, args.analysis)
    else:
        try:
            {"push": push, "refresh": refresh}[args.command](args.symbols, args.ghidra)
        except (OSError, ValueError, RuntimeError) as error:
            raise SystemExit(f"error: {error}")


if __name__ == "__main__":
    sys.exit(main())
