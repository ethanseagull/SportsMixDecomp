#!/usr/bin/env python3
"""Read and edit the unit lists in configure.py.

Each game module (main is main.dol) is one library in configure.py's config.libs, and lists the units it compiles
from source, one per line, in dtk-template's form (decompdecomp reads and edits the same lines):
    Object(<Matching|Equivalent|NonMatching>, "<source path under src/>"[, extra_cflags=<option>]),
Options are names defined in configure.py: PCH_MenuF_g3d (the precompiled header) and NOAUTO (-inline noauto).
For the callers they read as "pch=MenuF_g3d" and "flags=noauto", as in the old objects.txt lists.

Usage:
    python tools/decomp/objects.py list [Module]
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
CONFIGURE = ROOT / "configure.py"
STATES = ("Matching", "Equivalent", "NonMatching")
MODULES = ("main", "SystemF", "TourF", "MenuF", "BossF", "BasketF", "VolleyF", "DodgeF", "HockeyF", "DebugF")
DIRS = {"tour": "TourF", "menu": "MenuF", "boss": "BossF", "basket": "BasketF",
        "volley": "VolleyF", "dodge": "DodgeF", "hockey": "HockeyF", "msw": "SystemF",
        "debug": "DebugF"}
OPTIONS = {"pch=MenuF_g3d": "PCH_MenuF_g3d", "flags=noauto": "NOAUTO"}  # objects.txt word -> configure.py name
NAMES = {v: k for k, v in OPTIONS.items()}
ENTRY = re.compile(r'^(\s*)Object\(\s*(\w+)\s*,\s*"([^"]+)"\s*(?:,\s*extra_cflags=(\[[^\]]*\]|\w+)\s*)?\),?\s*$')
INDENT = " " * 12


def path(module=None, version="RMKE01"):
    """Where the unit lists live (one file for every module now)."""
    return CONFIGURE


def norm(unit):
    unit = unit.replace("\\", "/")
    if unit.startswith("src/"):
        unit = unit[4:]
    return unit if unit.endswith((".c", ".cpp")) else unit + ".cpp"


def _lines():
    return CONFIGURE.read_text(encoding="utf-8").split("\n")


def _block(lines, module):
    """(first, end) line indexes of the module's object entries: lines[first:end] are its Object(...) lines."""
    lib = None
    for i, line in enumerate(lines):
        if re.match(rf'^\s*"lib"\s*:\s*"{re.escape(module)}"\s*,', line):
            lib = i
            break
    if lib is None:
        raise KeyError(f"configure.py has no library for module {module}")
    for i in range(lib, len(lines)):
        if re.match(r'^\s*"objects"\s*:\s*\[\s*$', lines[i]):
            start = i + 1
            depth = 0
            for j in range(start, len(lines)):
                if depth == 0 and re.match(r"^\s*\],?\s*$", lines[j]):
                    return start, j
                depth += lines[j].count("(") - lines[j].count(")")
            break
    raise KeyError(f"configure.py: cannot find the objects list of module {module}")


def _opts(cflags):
    if not cflags:
        return []
    names = [n.strip().lstrip("*") for n in cflags.strip("[]").split(",") if n.strip()] if cflags.startswith("[") else [cflags]
    return [NAMES.get(n, f"extra_cflags={n}") for n in names]


def _entry(status, src, opts):
    names = [OPTIONS[o] if o in OPTIONS else o.split("=", 1)[1] if o.startswith("extra_cflags=") else None for o in opts]
    if None in names:
        raise ValueError(f"unknown option in {opts}; known: {', '.join(OPTIONS)}")
    tail = "" if not names else f", extra_cflags={names[0]}" if len(names) == 1 else \
        ", extra_cflags=[" + ", ".join(f"*{n}" for n in names) + "]"
    return f'{INDENT}Object({status}, "{src}"{tail}),'


def read(module, version="RMKE01"):
    """[(status, source, options)] in file order."""
    lines = _lines()
    try:
        start, end = _block(lines, module)
    except KeyError:
        return []
    out = []
    for line in lines[start:end]:
        m = ENTRY.match(line)
        if m:
            out.append((m.group(2), m.group(3), _opts(m.group(4))))
    return out


def find(unit, version="RMKE01"):
    """(module, status) of a registered unit, or (None, None)."""
    unit = norm(unit)
    for module in MODULES:
        for status, src, _ in read(module, version):
            if src == unit:
                return module, status
    return None, None


def module_of(unit, version="RMKE01"):
    # The unit's own split decides: game/msw/ units live in both the DOL
    # ("main") and SystemF.
    unit = norm(unit)
    for module in MODULES:
        splits = ROOT / "config" / version / ("" if module == "main" else module) / "splits.txt"
        if splits.exists() and any(l.rstrip() == unit + ":" for l in splits.read_text(encoding="utf-8").splitlines()):
            return module
    parts = unit.split("/")
    return DIRS.get(parts[1]) if len(parts) > 2 and parts[0] == "game" else None


def _write(lines):
    CONFIGURE.write_text("\n".join(lines), encoding="utf-8", newline="\n")


def set_status(unit, status, version="RMKE01"):
    if status not in STATES:
        raise ValueError(status)
    unit = norm(unit)
    module, old = find(unit, version)
    if module is None:
        raise KeyError(f"{unit} is not registered in configure.py")
    lines = _lines()
    start, end = _block(lines, module)
    for i in range(start, end):
        m = ENTRY.match(lines[i])
        if m and m.group(3) == unit:
            lines[i] = re.sub(r"Object\(\s*\w+", f"Object({status}", lines[i], count=1)
    _write(lines)
    return module, old


def add(unit, status="NonMatching", module=None, opts=(), version="RMKE01"):
    """Register a unit at the end of its module's list; returns False if it already is."""
    unit = norm(unit)
    if find(unit, version)[0]:
        return False
    module = module or module_of(unit)
    if module is None:
        raise KeyError(f"cannot tell the module of {unit}; pass it")
    lines = _lines()
    start, end = _block(lines, module)
    lines.insert(end, _entry(status, unit, list(opts)))
    _write(lines)
    return True


def remove(unit, version="RMKE01"):
    unit = norm(unit)
    module, _ = find(unit, version)
    if module is None:
        return False
    lines = _lines()
    start, end = _block(lines, module)
    lines = lines[:start] + [l for l in lines[start:end] if not ((m := ENTRY.match(l)) and m.group(3) == unit)] + lines[end:]
    _write(lines)
    return True


def statuses(version="RMKE01"):
    """{(module, unit without extension): status} for every registered unit."""
    return {(module, src.rsplit(".", 1)[0]): status for module in MODULES for status, src, _ in read(module, version)}


if __name__ == "__main__":
    if len(sys.argv) >= 2 and sys.argv[1] == "list":
        for module in ([sys.argv[2]] if len(sys.argv) > 2 else MODULES):
            for status, src, opts in read(module):
                print(f"{module:8s} {status:12s} {src} {' '.join(opts)}")
    else:
        sys.exit(__doc__)
