#!/usr/bin/env python3
"""Find functions whose code resembles a given function.

    python tools/decomp/similar.py <function> [--module M] [--matched] [--limit N]

Prints exact copies first (same instructions once symbol names are masked),
then the nearest neighbours by instruction sequence. With --matched, only
neighbours that already match are listed: their source is the best starting
point for the new function.
"""
import argparse
import difflib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("function")
    ap.add_argument("--version", default="RMKE01")
    ap.add_argument("--module")
    ap.add_argument("--matched", action="store_true")
    ap.add_argument("--limit", type=int, default=10)
    args = ap.parse_args()

    index_path = ROOT / "build" / "decomp" / f"index_{args.version}.json"
    index = json.loads(index_path.read_text(encoding="utf-8"))
    targets = [f for f in index
               if f["name"] == args.function and (not args.module or f["module"] == args.module)]
    if not targets:
        raise SystemExit(f"{args.function!r} is not in the index (run tools/decomp/index.py)")
    target = targets[0]
    ops = target["ops"].split()
    print(f"{target['name']}  [{target['module']}/{target['unit']}]  {target['size']} bytes")

    matched = set()
    report_path = ROOT / "build" / args.version / "report.json"
    if report_path.exists():
        for unit in json.loads(report_path.read_text(encoding="utf-8"))["units"]:
            module = unit["name"].partition("/")[0]
            for fn in unit.get("functions", []):
                if fn.get("fuzzy_match_percent", 0) >= 100.0:
                    matched.add((module, fn["name"]))

    def tag(f):
        return "matched" if (f["module"], f["name"]) in matched else ""

    copies = [f for f in index if f["hash"] == target["hash"] and f is not target]
    print(f"\nexact copies: {len(copies)}")
    for f in copies[:args.limit]:
        print(f"  {f['module']:9s} {f['name']}  ({f['unit']}) {tag(f)}")

    grams = set(zip(ops, ops[1:]))
    scored = []
    for f in index:
        if f["hash"] == target["hash"]:
            continue
        if args.matched and (f["module"], f["name"]) not in matched:
            continue
        n = f["size"] // 4
        if not 0.6 * len(ops) <= n <= 1.6 * len(ops):
            continue
        other = f["ops"].split()
        og = set(zip(other, other[1:]))
        if grams and len(grams & og) / max(len(grams | og), 1) < 0.35:
            continue
        scored.append((difflib.SequenceMatcher(None, ops, other, autojunk=False).ratio(), f))
    scored.sort(key=lambda t: -t[0])
    seen = set()
    shown = 0
    print("\nnearest:")
    for ratio, f in scored:
        if f["hash"] in seen:
            continue
        seen.add(f["hash"])
        print(f"  {100 * ratio:5.1f}%  {f['size']:5d}  {f['module']:9s} {f['name']}  ({f['unit']}) {tag(f)}")
        shown += 1
        if shown >= args.limit:
            break


if __name__ == "__main__":
    main()
