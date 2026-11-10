#!/usr/bin/env python3
"""Set a unit's status: Matching, Equivalent or NonMatching.

    python tools/decomp/mark.py game/boss/CFoo Matching [more units...]

Edits the unit's line in config/<version>/<module>/objects.txt; options such
as pch= stay. The next build re-runs configure.py by itself. Run the full
build afterwards; if the hash check fails, mark it back (hard rule 2).
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import objects  # noqa: E402


def main():
    if len(sys.argv) < 3 or sys.argv[2] not in objects.STATES:
        sys.exit(__doc__)
    state = sys.argv[2]
    for unit in [sys.argv[1], *sys.argv[3:]]:
        try:
            module, old = objects.set_status(unit, state)
        except KeyError as e:
            sys.exit(str(e))
        print(f"{objects.norm(unit)}: {old} -> {state} ({module})")


if __name__ == "__main__":
    main()
