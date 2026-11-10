#!/usr/bin/env python3
"""Print Ghidra's decompilation of one function, as a second draft next to draft.py.

Use it when m2c's control flow looks wrong: a switch whose cases fall through
with no break, errors inside branches, or gotos. Tried on five matched
functions (2026-11-10): Ghidra got switches and branch structure right where
m2c dropped every break, but it knows no types or field names, misnames
"symbol + offset" pointer constants (a second vtable pointer came out as
_savegpr_26), passes float arguments as raw doubles, and buries float code in
paired-single quantisation noise (GQR / ldexpf lines). So take the structure
from Ghidra and everything else (fields, types, constants, case order) from
draft.py.

It reads the original split object of the function's unit, so the unit's
objects must exist (any build has made them).

Usage:
    py -3.12 tools/decomp/ghidra_draft.py <function> [--module <Module>] [--raw]

Needs Ghidra 12.1, a JDK 21 and the GameCubeLoader extension (for the
Gekko/Broadway language). Found through GHIDRA_INSTALL_DIR and JAVA_HOME, or
as ~/tools/ghidra_*_PUBLIC and ~/tools/jdk-21*.
"""
import argparse
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
SCRIPTS = Path(__file__).resolve().parent / "ghidra"
LANGUAGE = "PowerPC:BE:32:Gekko_Broadway"


def find_dir(env, pattern, marker):
    if os.environ.get(env):
        return Path(os.environ[env])
    hits = sorted((Path.home() / "tools").glob(pattern), reverse=True)
    hits = [h for h in hits if (h / marker).exists()]
    return hits[0] if hits else None


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("function")
    ap.add_argument("--module")
    ap.add_argument("--version", default="RMKE01")
    ap.add_argument("--raw", action="store_true", help="keep Ghidra's warning comments")
    args = ap.parse_args()

    ghidra = find_dir("GHIDRA_INSTALL_DIR", "ghidra_*_PUBLIC", "support")
    jdk = find_dir("JAVA_HOME", "jdk-21*", "bin")
    if not ghidra or not jdk:
        sys.exit("Ghidra or the JDK was not found: set GHIDRA_INSTALL_DIR and JAVA_HOME "
                 "(Ghidra 12.1 with the GameCubeLoader extension, JDK 21)")
    if not list((ghidra / "Ghidra" / "Extensions").glob("GameCube*")):
        sys.exit(f"the GameCubeLoader extension is missing from {ghidra}\\Ghidra\\Extensions")

    base = ROOT / "build" / args.version
    index = json.loads((ROOT / "build" / "decomp" / f"index_{args.version}.json").read_text(encoding="utf-8"))
    hits = [f for f in index if f["name"] == args.function and args.module in (None, f["module"])]
    if not hits:
        sys.exit(f"{args.function} not found in the index (rebuild it with tools/decomp/index.py)")
    if len({(f["module"], f["unit"]) for f in hits}) > 1:
        sys.exit("found in: " + ", ".join(sorted({f["module"] for f in hits})) + "; pass --module")
    module, unit = hits[0]["module"], hits[0]["unit"]
    obj = (base / "obj" if module == "main" else base / module / "obj") / (unit + ".o")
    if not obj.exists():
        sys.exit(f"{obj} does not exist: run py -3.12 tools/decomp/build.py first")

    work = ROOT / "build" / "decomp" / "ghidra"
    work.mkdir(parents=True, exist_ok=True)
    tmp = Path(tempfile.mkdtemp(dir=work))  # one per call: sessions may run this at once
    env = dict(os.environ, JAVA_HOME=str(jdk), PATH=str(jdk / "bin") + os.pathsep + os.environ.get("PATH", ""))
    headless = ghidra / "support" / ("analyzeHeadless.bat" if os.name == "nt" else "analyzeHeadless")
    cmd = [str(headless), str(tmp), "draft", "-import", str(obj),
           "-processor", LANGUAGE, "-cspec", "default",
           "-scriptPath", str(SCRIPTS), "-postScript", "DecompNamed.java", str(tmp / "out"), args.function,
           "-deleteProject"]
    try:
        res = subprocess.run(cmd, capture_output=True, text=True, env=env, timeout=600)
        out = tmp / "out" / (args.function + ".c")
        if not out.exists():
            log = res.stdout + res.stderr
            found = [l for l in log.splitlines() if "NOT FOUND" in l or "ERROR" in l and "REL" not in l and "RSO" not in l]
            sys.exit("Ghidra produced no draft.\n" + "\n".join(found[-10:]))
        text = out.read_text(encoding="utf-8", errors="replace")
    finally:
        shutil.rmtree(tmp, ignore_errors=True)
    if not args.raw:
        text = "\n".join(l for l in text.splitlines()
                         if not re.match(r"/\* WARNING: (Removing unreachable block|Globals starting)", l)).lstrip()
    print(f"// Ghidra draft of {args.function} ({module} {unit}). Structure only: see the docstring.")
    print(text)
    return 0


if __name__ == "__main__":
    sys.exit(main())
