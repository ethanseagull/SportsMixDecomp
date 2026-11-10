#!/usr/bin/env python3
"""Read and edit the per-module unit lists, config/<version>/<module>/objects.txt.

Each game module lists the units it compiles from source, one per line:
    <Matching|Equivalent|NonMatching> <source path under src/> [pch=<name>]
configure.py reads them (and re-runs itself when one changes).

Usage:
    python tools/decomp/objects.py list [Module]
"""
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
STATES = ("Matching", "Equivalent", "NonMatching")
MODULES = ("SystemF", "TourF", "MenuF", "BossF", "BasketF", "VolleyF", "DodgeF", "HockeyF")
DIRS = {"tour": "TourF", "menu": "MenuF", "boss": "BossF", "basket": "BasketF",
        "volley": "VolleyF", "dodge": "DodgeF", "hockey": "HockeyF", "msw": "SystemF"}


def path(module, version="RMKE01"):
    return ROOT / "config" / version / module / "objects.txt"


def norm(unit):
    unit = unit.replace("\\", "/")
    if unit.startswith("src/"):
        unit = unit[4:]
    return unit if unit.endswith((".c", ".cpp")) else unit + ".cpp"


def read(module, version="RMKE01"):
    """[(status, source, options)] in file order."""
    p = path(module, version)
    out = []
    if p.exists():
        for line in p.read_text(encoding="utf-8").splitlines():
            line = line.split("#", 1)[0].strip()
            if line:
                status, src, *opts = line.split()
                out.append((status, src, opts))
    return out


def find(unit, version="RMKE01"):
    """(module, status) of a registered unit, or (None, None)."""
    unit = norm(unit)
    for module in MODULES:
        for status, src, _ in read(module, version):
            if src == unit:
                return module, status
    return None, None


def module_of(unit):
    parts = norm(unit).split("/")
    return DIRS.get(parts[1]) if len(parts) > 2 and parts[0] == "game" else None


def _rewrite(module, fn, version="RMKE01"):
    p = path(module, version)
    lines = p.read_text(encoding="utf-8").splitlines() if p.exists() else []
    lines = fn(lines)
    p.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")


def set_status(unit, status, version="RMKE01"):
    if status not in STATES:
        raise ValueError(status)
    unit = norm(unit)
    module, old = find(unit, version)
    if module is None:
        raise KeyError(f"{unit} is not registered in any objects.txt")

    def edit(lines):
        out = []
        for line in lines:
            body = line.split("#", 1)[0].split()
            if len(body) >= 2 and body[1] == unit:
                line = " ".join([status, *body[1:]])
            out.append(line)
        return out
    _rewrite(module, edit, version)
    return module, old


def add(unit, status="NonMatching", module=None, opts=(), version="RMKE01"):
    """Register a unit; returns False if it already is."""
    unit = norm(unit)
    if find(unit, version)[0]:
        return False
    module = module or module_of(unit)
    if module is None:
        raise KeyError(f"cannot tell the module of {unit}; pass it")
    _rewrite(module, lambda lines: lines + [" ".join([status, unit, *opts])], version)
    return True


def remove(unit, version="RMKE01"):
    unit = norm(unit)
    module, _ = find(unit, version)
    if module is None:
        return False

    def edit(lines):
        return [l for l in lines if not (len(l.split("#", 1)[0].split()) >= 2 and l.split()[1] == unit)]
    _rewrite(module, edit, version)
    return True


if __name__ == "__main__":
    if len(sys.argv) >= 2 and sys.argv[1] == "list":
        for module in ([sys.argv[2]] if len(sys.argv) > 2 else MODULES):
            for status, src, opts in read(module):
                print(f"{module:8s} {status:12s} {src} {' '.join(opts)}")
    else:
        sys.exit(__doc__)
