#!/usr/bin/env python3
"""Record functions whose source is accepted without an exact match.

The project's target is working source. A function that does not reach 100%
in objdiff can still be finished, in one of two classes:

  near        score of 85% or more, and every remaining difference explained
              as something that cannot change behaviour (register choice,
              instruction order, stack slot). Treated as correct.
  unverified  written and compiling, but further from the original or with a
              difference nobody has explained. Needs a behaviour test later.

The lists are config/<version>/<module>/equivalent.txt, one per module (so
sessions in different modules never edit the same file), one function per line:
    <module> <function> <class> <score> <reason>

Usage:
    python tools/decomp/equivalent.py add <module> <function> "<reason>" [--unverified]
    python tools/decomp/equivalent.py list [--module M]
    python tools/decomp/equivalent.py check        drop entries that now match exactly
"""
import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
NEAR_MIN = 85.0
UNVERIFIED_MIN = 40.0


def path(version, module):
    return ROOT / "config" / version / module / "equivalent.txt"


def load(version="RMKE01"):
    """{(module, function): (class, score, reason)}, from every module's list."""
    out = {}
    for p in sorted((ROOT / "config" / version).glob("*/equivalent.txt")):
        for line in p.read_text(encoding="utf-8").splitlines():
            parts = line.split(None, 4)
            if len(parts) >= 4 and not line.startswith("#"):
                out[(parts[0], parts[1])] = (parts[2], float(parts[3]), parts[4] if len(parts) > 4 else "")
    return out


def save(version, entries, modules=None):
    """Write the lists of `modules` (default: every module that has entries)."""
    by_module = {}
    for (module, fn), v in entries.items():
        by_module.setdefault(module, {})[fn] = v
    for module in (modules or by_module):
        rows = by_module.get(module, {})
        lines = ["# <module> <function> <near|unverified> <objdiff score> <why the difference is accepted>"]
        for fn, (cls, score, reason) in sorted(rows.items()):
            lines.append(f"{module} {fn} {cls} {score:.1f} {reason}")
        p = path(version, module)
        if rows or p.exists():
            p.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")


def scores(version):
    report = json.loads((ROOT / "build" / version / "report.json").read_text(encoding="utf-8"))
    out = {}
    for unit in report["units"]:
        module = unit["name"].partition("/")[0]
        has_source = unit.get("metadata", {}).get("source_path") is not None
        for fn in unit.get("functions", []):
            out[(module, fn["name"])] = (fn.get("fuzzy_match_percent", 0.0), has_source)
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--version", default="RMKE01")
    sub = ap.add_subparsers(dest="cmd", required=True)
    a = sub.add_parser("add")
    a.add_argument("module")
    a.add_argument("function")
    a.add_argument("reason")
    a.add_argument("--unverified", action="store_true")
    a.add_argument("--tailcall", action="store_true",
                   help="near below the floor: the only difference is a tail call vs a framed call to the "
                        "same function, confirmed by a reviewer (notes/decisions.md)")
    ls = sub.add_parser("list")
    ls.add_argument("--module")
    sub.add_parser("check")
    args = ap.parse_args()
    entries = load(args.version)

    if args.cmd == "add":
        if any(c in args.function for c in " \t") or "\n" in args.reason:
            sys.exit("function names have no spaces; the reason is one line")
        got = scores(args.version).get((args.module, args.function))
        if got is None:
            sys.exit(f"{args.module} {args.function} is not in the report (rebuild it first)")
        score, has_source = got
        if not has_source:
            sys.exit("its unit has no source registered in its module's objects.txt")
        if score >= 100.0:
            sys.exit("it matches exactly; nothing to record")
        if len(args.reason.strip()) < 15:
            sys.exit("give a real reason: what differs and why it cannot change behaviour")
        cls = "unverified" if args.unverified else "near"
        floor = UNVERIFIED_MIN if args.unverified else NEAR_MIN
        if args.tailcall:
            if args.unverified:
                sys.exit("--tailcall records a near entry; drop --unverified")
            if "reviewer" not in args.reason.lower():
                sys.exit("--tailcall needs the reviewer's confirmation in the reason (say 'reviewer confirmed')")
            args.reason = "tail call vs framed call only: " + args.reason.strip()
        elif score < floor:
            hint = "" if args.unverified else f"; below {NEAR_MIN:.0f}% use --unverified"
            sys.exit(f"score is {score:.1f}%, under the {floor:.0f}% floor for '{cls}'{hint}")
        entries[(args.module, args.function)] = (cls, score, args.reason.strip())
        save(args.version, entries, [args.module])
        print(f"recorded {args.module} {args.function} as {cls} at {score:.1f}%")
    elif args.cmd == "list":
        for (module, fn), (cls, score, reason) in sorted(entries.items()):
            if args.module in (None, module):
                print(f"{cls:10s} {score:5.1f}%  {module} {fn}\n            {reason}")
        near = sum(v[0] == "near" for v in entries.values())
        print(f"{near} near, {len(entries) - near} unverified")
    else:
        now = scores(args.version)
        keep = {k: v for k, v in entries.items() if now.get(k, (0.0, False))[0] < 100.0}
        if len(keep) != len(entries):
            save(args.version, keep, sorted({m for m, _ in entries}))
        print(f"{len(entries) - len(keep)} entries now match exactly and were dropped; {len(keep)} remain")


if __name__ == "__main__":
    main()
