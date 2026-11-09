#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
# Adapted from RepriseOS tools/ghidra.py (8fc840d).
"""Build a Ghidra project from decrypted inputs and the saved analysis, or export it.

    python tools/ghidra.py import --inputs <decrypted 2.0.5 dir> [--project build/ghidra]
    python tools/ghidra.py export [--project build/ghidra]

import prepares ELF images of OSOS and the NOR loader, runs auto-analysis, then
applies ghidra/analysis on top. export writes the project's annotations back to
ghidra/analysis, keeping only what auto-analysis cannot reproduce.
"""

import argparse
import json
import shutil
import subprocess
import sys
import tempfile
from collections import defaultdict
from pathlib import Path

from ghidra_analysis import overlay, publish, trim_sarif
from ghidra_prepare import prepare
from ghidra_project import (
    ROOT,
    WORK,
    headless,
    preflight,
    project_dir,
    require_closed,
)

ANALYSIS = ROOT / "ghidra/analysis"
MANIFEST = ROOT / "ghidra/programs.json"
VERSION = "2.0.5"


def verify_options(manifest_path):
    return [
        "-process",
        "*",
        "-recursive",
        "-readOnly",
        "-postScript",
        "VerifyAnalysis.java",
        str(manifest_path),
    ]


def import_project(args, manifest):
    project = project_dir(args.project)
    if project.exists():
        raise ValueError(f"Import needs a new project directory: {project}")
    cli = args.cli or shutil.which("revola")
    if not cli:
        raise ValueError("No revola CLI on PATH and no --cli")
    WORK.mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="ghidra-inputs-", dir=WORK) as temporary:
        work = Path(temporary)
        specs = prepare(args.inputs.resolve(), work / "prepared", cli, manifest)
        groups = defaultdict(list)
        for spec in specs:
            overlay(
                spec,
                args.analysis,
                work / "overlay" / Path(spec["path"]).with_suffix(".sarif"),
            )
            groups[Path(spec["path"]).parent.as_posix()].append(spec)
        project.parent.mkdir(parents=True, exist_ok=True)
        # Ghidra rejects hidden path components, so stage beside the destination.
        with tempfile.TemporaryDirectory(
            prefix="ghidra-import-", dir=project.parent
        ) as staging:
            stage = Path(staging)
            for folder, members in groups.items():
                headless(
                    stage,
                    folder,
                    [
                        "-processor",
                        manifest["language"],
                        "-cspec",
                        "default",
                        "-import",
                        *(str(work / "prepared/inputs" / s["input"]) for s in members),
                        "-postScript",
                        "AnalyzePrepared.java",
                        str(MANIFEST),
                        folder,
                        "-postScript",
                        "ApplyAnalysis.java",
                        str(work / "overlay"),
                        folder,
                    ],
                    "import-" + folder.replace("/", "-"),
                    [f"PREPARED {s['path']}" for s in members]
                    + [f"APPLIED {s['path']}" for s in members],
                )
            headless(
                stage,
                VERSION,
                verify_options(MANIFEST),
                "verify",
                [f"VERIFIED {s['path']}:" for s in specs],
            )
            stage.rename(project)
    print(f"Open {project / 'osos.gpr'}")


def export(args, manifest):
    project = project_dir(args.project)
    require_closed(project)
    paths = [spec["path"] for spec in manifest["programs"]]
    WORK.mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="ghidra-export-", dir=WORK) as temporary:
        work = Path(temporary)
        headless(
            project,
            VERSION,
            [
                *verify_options(MANIFEST),
                "-postScript",
                "ExportAnalysis.java",
                str(work / "sarif"),
                *paths,
            ],
            "export",
            [f"VERIFIED {p}:" for p in paths] + [f"Exported {p}" for p in paths],
        )
        trimmed = work / "trimmed"
        for spec in manifest["programs"]:
            trim_sarif(work / "sarif" / (spec["path"] + ".sarif"), spec, trimmed)
        publish(trimmed / VERSION, args.analysis / VERSION)
    print(f"Exported {len(paths)} programs to {args.analysis / VERSION}")


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    parser.add_argument("command", choices=["import", "export"])
    parser.add_argument("--project", type=Path, default=ROOT / "build/ghidra")
    parser.add_argument("--analysis", type=Path, default=ANALYSIS)
    parser.add_argument("--inputs", type=Path, help="decrypted 2.0.5 inputs (import)")
    parser.add_argument("--cli", help="revola CLI for firmware extract-efi (import)")
    args = parser.parse_args()
    if args.command == "import" and not args.inputs:
        parser.error("import needs --inputs")
    try:
        manifest = json.loads(MANIFEST.read_text())
        preflight(manifest["ghidra_version"])
        if args.command == "import":
            import_project(args, manifest)
        else:
            export(args, manifest)
    except (OSError, ValueError, RuntimeError, subprocess.CalledProcessError) as error:
        parser.exit(1, f"error: {error}\n")


if __name__ == "__main__":
    sys.exit(main())
