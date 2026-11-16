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

A near entry must be proven, not only explained: `add` runs diff.py's
classifier on the function. `registers` (same code once registers are
renumbered) and `schedule` (same instructions in another order) are accepted as
near at once. A `real` difference is refused as near unless a reviewer has
checked the stated reason (--reviewed, with "reviewer" in the reason); otherwise
record it --unverified. The class found is stored at the start of the reason.

Usage:
    python tools/decomp/equivalent.py add <module> <function> "<reason>" [--unverified] [--reviewed]
    python tools/decomp/equivalent.py list [--module M]
    python tools/decomp/equivalent.py check        drop entries that now match exactly
    python tools/decomp/equivalent.py audit [--module M]
                    classify every near entry with diff.py; writes notes/near-audit.txt
                    (notes/near-audit-<M>.txt with --module)
"""
import argparse
import json
import sys
from pathlib import Path
import decomp_common as dc  # noqa: E402  (tools/decomp/decomp_common.py)

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
                key = (parts[0], parts[1])
                new = (parts[2], float(parts[3]), parts[4] if len(parts) > 4 else "")
                # The files merge with git's union driver (.gitattributes), so one
                # function can appear twice after a merge: keep the higher score,
                # and near over unverified at the same score. save() writes one line.
                old = out.get(key)
                if old is None or (new[1], new[0] == "near") > (old[1], old[0] == "near"):
                    out[key] = new
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
    report = dc.load_report(version)
    out = {}
    for unit in report["units"]:
        module = unit["name"].partition("/")[0]
        has_source = unit.get("metadata", {}).get("source_path") is not None
        for fn in unit.get("functions", []):
            out[(module, fn["name"])] = (fn.get("fuzzy_match_percent", 0.0), has_source)
    return out


def units_of(version):
    """{(module, function): unit name without the module prefix}"""
    report = dc.load_report(version)
    out = {}
    for unit in report["units"]:
        module, _, name = unit["name"].partition("/")
        for fn in unit.get("functions", []):
            out[(module, fn["name"])] = name
    return out


def classify_many(version, wanted):
    """{(module, function): (class, hints)} via diff.py, one disassembly per unit.

    wanted: iterable of (module, function). Class is match/registers/schedule/real/
    missing, or 'error' with the reason as the only hint."""
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    import diff  # noqa: E402  (tools/decomp/diff.py)
    where = units_of(version)
    by_unit = {}
    out = {}
    for key in wanted:
        unit = where.get(key)
        if unit is None:
            out[key] = ("error", ["not in the report"])
        else:
            by_unit.setdefault((key[0], unit), set()).add(key[1])
    for (module, unit), fns in sorted(by_unit.items()):
        try:
            rows = diff.analyse(version, unit, module, fns)
        except SystemExit as e:
            for fn in fns:
                out[(module, fn)] = ("error", [str(e)])
            continue
        except Exception as e:  # unreadable asm, missing object
            for fn in fns:
                out[(module, fn)] = ("error", [f"{type(e).__name__}: {e}"])
            continue
        for fn, cls, why, _, _ in rows:
            out[(module, fn)] = (cls, why)
        for fn in fns:
            out.setdefault((module, fn), ("error", ["not in the original asm of its unit"]))
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
    a.add_argument("--reviewed", action="store_true",
                   help="near for a 'real' difference: a reviewer checked that the stated difference "
                        "cannot change behaviour (say 'reviewer' in the reason)")
    ls = sub.add_parser("list")
    ls.add_argument("--module")
    sub.add_parser("check")
    au = sub.add_parser("audit")
    au.add_argument("--module")
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
        reason = args.reason.strip()
        if cls == "near" and not args.tailcall:
            kind, why = classify_many(args.version, [(args.module, args.function)])[(args.module, args.function)]
            if kind in ("registers", "schedule", "stack"):
                reason = f"[{kind}] {reason}"
            elif kind == "real":
                if not args.reviewed or "reviewer" not in reason.lower():
                    sys.exit("diff.py classes this difference as 'real', not registers/schedule/stack:\n  - "
                             + "\n  - ".join(why or ["(no hints)"])
                             + "\nRecord it --unverified, or have a reviewer confirm the reason cannot change "
                               "behaviour and add it with --reviewed ('reviewer' in the reason).")
                reason = f"[real, reviewed] {reason}"
            else:
                sys.exit(f"diff.py could not classify it ({kind}: {'; '.join(why)}); rebuild the unit's object first")
        entries[(args.module, args.function)] = (cls, score, reason)
        save(args.version, entries, [args.module])
        print(f"recorded {args.module} {args.function} as {cls} at {score:.1f}%")
    elif args.cmd == "list":
        for (module, fn), (cls, score, reason) in sorted(entries.items()):
            if args.module in (None, module):
                print(f"{cls:10s} {score:5.1f}%  {module} {fn}\n            {reason}")
        near = sum(v[0] == "near" for v in entries.values())
        print(f"{near} near, {len(entries) - near} unverified")
    elif args.cmd == "audit":
        near = {k: v for k, v in entries.items() if v[0] == "near" and args.module in (None, k[0])}
        found = classify_many(args.version, near)
        groups = {}
        for k in sorted(near, key=lambda k: near[k][1]):
            kind, why = found.get(k, ("error", ["?"]))
            groups.setdefault(kind, []).append((k, near[k], why))
        order = ["real", "missing", "error", "registers", "schedule", "stack", "match"]
        lines = [f"# near audit: diff.py class of every near entry ({len(near)})",
                 "# registers/schedule/stack = proven harmless; real = the stated reason needs a check "
                 "(lowest score first); match = now 100%, run equivalent.py check", ""]
        for kind in order + sorted(set(groups) - set(order)):
            rows = groups.get(kind, [])
            if not rows:
                continue
            lines.append(f"## {kind}: {len(rows)}")
            for (module, fn), (_, score, reason), why in rows:
                lines.append(f"{score:5.1f}  {module} {fn}")
                if kind not in ("registers", "schedule", "stack", "match"):
                    lines.append(f"       claimed: {reason}")
                    for h in why:
                        lines.append(f"       found:   {h}")
            lines.append("")
        # one file per module, so parallel review sessions do not overwrite each other's audit
        out = ROOT / "notes" / (f"near-audit-{args.module}.txt" if args.module else "near-audit.txt")
        out.write_text("\n".join(lines), encoding="utf-8", newline="\n")
        for kind in order:
            if groups.get(kind):
                print(f"{kind:10s} {len(groups[kind])}")
        print(f"written to {out.relative_to(ROOT)}")
    else:
        now = scores(args.version)
        keep = {k: v for k, v in entries.items() if now.get(k, (0.0, False))[0] < 100.0}
        if len(keep) != len(entries):
            save(args.version, keep, sorted({m for m, _ in entries}))
        print(f"{len(entries) - len(keep)} entries now match exactly and were dropped; {len(keep)} remain")


if __name__ == "__main__":
    main()
