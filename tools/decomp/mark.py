#!/usr/bin/env python3
"""Set a unit's status in configure.py: Matching, Equivalent or NonMatching.

    python tools/decomp/mark.py game/boss/CFoo.cpp Matching [more units...]

Only the first argument of the unit's Object(...) changes; extra options such
as extra_cflags stay. Run configure.py and the full build afterwards; if the
hash check fails, mark it back (hard rule 2).
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
STATES = ("Matching", "Equivalent", "NonMatching")


def main():
    if len(sys.argv) < 3 or sys.argv[2] not in STATES:
        sys.exit(__doc__)
    state = sys.argv[2]
    units = [sys.argv[1], *sys.argv[3:]]
    cfg = ROOT / "configure.py"
    text = cfg.read_text(encoding="utf-8")
    for unit in units:
        unit = unit if unit.endswith((".c", ".cpp")) else unit + ".cpp"
        pat = re.compile(r'Object\((?:Matching|Equivalent|NonMatching|MatchingFor\([^)]*\)), "' + re.escape(unit) + '"')
        if not pat.search(text):
            sys.exit(f"{unit}: no Object(...) entry in configure.py")
        text = pat.sub(f'Object({state}, "{unit}"', text, count=1)
        print(f"{unit}: {state}")
    cfg.write_text(text, encoding="utf-8", newline="\n")


if __name__ == "__main__":
    main()
