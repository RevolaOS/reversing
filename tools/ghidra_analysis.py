# SPDX-License-Identifier: GPL-3.0-only
# Adapted from RepriseOS tools/ghidra_analysis.py (8fc840d).
"""Distributable analysis records: trim a SARIF export into sharded JSON lines."""

import json
import shutil
import tempfile
from collections import defaultdict
from pathlib import Path

SHARD_BYTES = 4 << 20
BOOKMARK_KINDS = {"Note", "Info"}
KEPT_SOURCES = {"USER_DEFINED", "IMPORTED"}


def keep(rule, properties):
    """True for records that auto-analysis cannot reproduce."""
    if rule in ("memory_map", "comments", "datatype", "entry_points"):
        return True
    if rule == "functions":
        return (
            not properties["name"].startswith("FUN_")
            or properties.get("sourceType") != "DEFAULT"
            or properties.get("signatureSource") != "DEFAULT"
            or properties.get("isThunk")
            or properties.get("hasCustomStorage")
        )
    if rule == "symbols":
        return properties.get("sourceType") in KEPT_SOURCES
    if rule == "bookmarks":
        return properties.get("kind") in BOOKMARK_KINDS
    return False


def analysis_directory(spec, analysis):
    return analysis / str(Path(spec["path"]).with_suffix(""))


def shards(directory, rule):
    return sorted(directory.glob(f"{rule}.jsonl")) + sorted(
        directory.glob(f"{rule}.[0-9][0-9].jsonl")
    )


def read(directory, rule):
    for path in shards(directory, rule):
        with path.open() as source:
            yield from (json.loads(line) for line in source if line.strip())


def write(directory, rule, records):
    lines = [
        json.dumps(r, separators=(",", ":"), sort_keys=True) + "\n" for r in records
    ]
    if sum(map(len, lines)) <= SHARD_BYTES:
        (directory / f"{rule}.jsonl").write_text("".join(lines))
        return
    index, size, part = 0, 0, []
    for line in lines + [None]:
        if line is None or (part and size + len(line) > SHARD_BYTES):
            (directory / f"{rule}.{index:02d}.jsonl").write_text("".join(part))
            index, size, part = index + 1, 0, []
        if line is not None:
            part.append(line)
            size += len(line)


def trim(results, spec, header, destination):
    """Write the kept records of one program to its analysis directory."""
    run = header["runs"][0]
    run.pop("results", None)
    for artifact in run.get("artifacts", []):
        artifact["location"]["uri"] = spec["input"]
    groups = defaultdict(list)
    for result in results:
        rule = result["ruleId"].lower()
        if keep(rule, result["properties"]["additionalProperties"]):
            groups[rule].append(result)
    destination.mkdir(parents=True, exist_ok=True)
    for old in destination.glob("*.jsonl"):
        old.unlink()
    (destination / "program.json").write_text(json.dumps(header, indent=2) + "\n")
    for rule, records in sorted(groups.items()):
        write(destination, rule, records)


def trim_sarif(source, spec, analysis):
    data = json.loads(source.read_text())
    results = data["runs"][0].pop("results")
    trim(results, spec, data, analysis_directory(spec, analysis))


def trim_directory(source, spec, analysis):
    """Trim an untrimmed RepriseOS analysis directory (one jsonl per rule)."""
    header = json.loads((source / "program.json").read_text())
    results = []
    for path in sorted(source.glob("*.jsonl")):
        with path.open() as lines:
            results.extend(json.loads(line) for line in lines if line.strip())
    trim(results, spec, header, analysis_directory(spec, analysis))


OVERLAY_RULES = ("datatype", "functions", "symbols", "comments", "bookmarks")


def overlay(spec, analysis, destination):
    """Write the SARIF that ApplyAnalysis.java reads onto an analysed program."""
    directory = analysis_directory(spec, analysis)
    data = json.loads((directory / "program.json").read_text())
    results = [r for rule in OVERLAY_RULES for r in read(directory, rule)]
    for result in results:
        if result["ruleId"] == "FUNCTIONS":
            props = result["properties"]["additionalProperties"]
            props["isThunk"] = False
            if "thunkAddress" in props:
                props["thunkTarget"] = props.pop("thunkAddress")
    data["runs"][0]["results"] = results
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text(json.dumps(data, separators=(",", ":")))


def publish(source, destination):
    """Replace destination with source in one rename, keeping a copy until it lands."""
    destination.parent.mkdir(parents=True, exist_ok=True)
    previous = destination.with_name(destination.name + ".previous")
    if previous.exists():
        raise ValueError(f"Recovery directory exists: {previous}")
    with tempfile.TemporaryDirectory(
        prefix="analysis-", dir=destination.parent
    ) as temporary:
        ready = Path(temporary) / "ready"
        shutil.copytree(source, ready)
        if destination.exists():
            destination.rename(previous)
        try:
            ready.rename(destination)
        except OSError:
            if previous.exists():
                previous.rename(destination)
            raise
    if previous.exists():
        shutil.rmtree(previous)
