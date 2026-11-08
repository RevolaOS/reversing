# SPDX-License-Identifier: GPL-3.0-only
# Adapted from RepriseOS tools/ghidra_project.py (8fc840d).
"""Ghidra preflight and headless runs against a project directory."""

import os
import re
import shutil
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
WORK = ROOT / ".build"
SCRIPTS = ROOT / "ghidra/scripts"
NAME = "osos"


def ghidra_dir():
    value = os.environ.get("GHIDRA_INSTALL_DIR")
    if not value:
        raise ValueError("Set GHIDRA_INSTALL_DIR (source env.sh)")
    return Path(value)


def preflight(version):
    """Fail early when Ghidra or the JDK does not match the manifest."""
    ghidra = ghidra_dir()
    if not os.access(ghidra / "support/analyzeHeadless", os.X_OK):
        raise ValueError(f"No executable support/analyzeHeadless in {ghidra}")
    properties = dict(
        line.split("=", 1)
        for line in (ghidra / "Ghidra/application.properties").read_text().splitlines()
        if "=" in line and not line.startswith("#")
    )
    if properties["application.version"] != version:
        raise ValueError(f"Use Ghidra {version}")
    java_home = os.environ.get("JAVA_HOME")
    java = str(Path(java_home) / "bin/java") if java_home else shutil.which("java")
    if not java:
        raise ValueError("Set JAVA_HOME to a supported JDK")
    result = subprocess.run([java, "-version"], capture_output=True, text=True)
    match = re.search(r'version "(\d+)', result.stderr + result.stdout)
    if not match or int(match[1]) < int(properties["application.java.min"]):
        raise ValueError(f"Use JDK {properties['application.java.min']} or newer")


def project_dir(path):
    """Accept a project directory or its .gpr file."""
    path = Path(path).resolve()
    if path.suffix == ".gpr":
        path = path.parent
    if any(part.startswith(".") for part in path.parts):
        raise ValueError("Ghidra project paths cannot contain hidden directories")
    return path


def require_closed(project):
    if not (project / f"{NAME}.gpr").is_file():
        raise ValueError(f"No Ghidra project at {project}")
    if list(project.glob(f"{NAME}.lock*")):
        raise ValueError("Close the project in Ghidra first (lock file present)")


def headless(project, folder, options, phase, markers=()):
    """Run analyzeHeadless on <project>/osos.gpr at folder; every marker must print once."""
    WORK.mkdir(exist_ok=True)
    log = WORK / f"ghidra-{phase}.log"
    command = [
        str(ghidra_dir() / "support/analyzeHeadless"),
        str(project),
        f"{NAME}/{folder}".rstrip("/"),
        "-noanalysis",
        "-max-cpu",
        "8",
        "-scriptPath",
        str(SCRIPTS),
        *options,
    ]
    env = os.environ.copy()
    env.setdefault("GHIDRA_HEADLESS_MAXMEM", "8G")
    print(f"{phase}: {log}", flush=True)
    with log.open("w") as output:
        result = subprocess.run(
            command, stdout=output, stderr=subprocess.STDOUT, env=env
        )
    output = log.read_text()
    missing = [marker for marker in markers if output.count(marker) != 1]
    # Analyzer diagnostics do not fail an import; script and framework errors do.
    errors = [line for line in output.splitlines() if "ERROR " in line]
    arm_context_conflict = (
        "ERROR Unexpected Exception (ArmAnalyzer) "
        "ghidra.program.model.listing.ContextChangeException: "
        "Context register change conflicts with one or more instructions"
    )
    fatal = [
        line
        for line in errors
        if "(ClearFlowAndRepairCmd)" not in line
        and line.strip() != arm_context_conflict
    ]
    if result.returncode or fatal or missing:
        raise RuntimeError(
            f"Ghidra failed; see {log}\n"
            + "\n".join(fatal[-8:])
            + (f"\nMissing or repeated markers: {missing}" if missing else "")
        )
    if errors:
        print(f"{phase}: {len(errors)} analyzer diagnostics in {log}", flush=True)
    return output
