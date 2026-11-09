#!/usr/bin/env python3
"""Progress and work queue, read from the objdiff report that `ninja` writes.

    python tools/decomp/status.py                   totals per module
    python tools/decomp/status.py unit <name>       every function in a unit, with match %
    python tools/decomp/status.py queue [options]   unmatched functions, best payoff first

The queue favours functions that are small, have a real name, and whose code
also appears elsewhere (one match then covers every copy). Run
tools/decomp/index.py first so duplicate counts are available.
"""
import argparse
import json
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent


def load(version: str):
    report = json.loads((ROOT / "build" / version / "report.json").read_text(encoding="utf-8"))
    index_path = ROOT / "build" / "decomp" / f"index_{version}.json"
    index = json.loads(index_path.read_text(encoding="utf-8")) if index_path.exists() else []
    return report, index


def functions(report):
    for unit in report["units"]:
        module, _, name = unit["name"].partition("/")
        for fn in unit.get("functions", []):
            yield module, name, fn["name"], int(fn["size"]), fn.get("fuzzy_match_percent", 0.0)


def is_named(name: str) -> bool:
    return not name.startswith(("fn_", "gap_", "lbl_", "@"))


def cmd_totals(report, index, args):
    per = defaultdict(lambda: [0, 0, 0, 0])
    for module, _, _, size, pct in functions(report):
        row = per[module]
        row[0] += 1
        row[1] += size
        if pct >= 100.0:
            row[2] += 1
            row[3] += size
    print(f"{'module':10s} {'functions':>10s} {'matched':>8s} {'code bytes':>11s} {'matched':>9s}")
    for module, (n, size, mn, msize) in sorted(per.items(), key=lambda kv: -kv[1][1]):
        print(f"{module:10s} {n:10d} {mn:8d} {size:11d} {msize:9d}  {100 * msize / size:6.2f}%")
    m = report["measures"]
    print(f"\nall: {m['matched_functions']} / {m['total_functions']} functions, "
          f"{float(m['matched_code_percent']):.3f}% of code matched")


def cmd_unit(report, index, args):
    hits = [u for u in report["units"] if args.name in u["name"]]
    if not hits:
        raise SystemExit(f"no unit matching {args.name!r}")
    for unit in hits[:5]:
        print(unit["name"], f"({unit['measures'].get('fuzzy_match_percent', 0):.1f}%)")
        for fn in unit.get("functions", []):
            print(f"  {fn.get('fuzzy_match_percent', 0):6.1f}%  {int(fn['size']):6d}  {fn['name']}")


def cmd_queue(report, index, args):
    hash_of = {(f["module"], f["name"]): f["hash"] for f in index}
    copies = Counter(f["hash"] for f in index)
    seen = set()
    rows = []
    for module, unit, name, size, pct in functions(report):
        if pct >= 100.0 or size < args.min_size or size > args.max_size:
            continue
        if args.module and module != args.module:
            continue
        if args.named and not is_named(name):
            continue
        digest = hash_of.get((module, name))
        if digest:
            if digest in seen:
                continue  # one entry per distinct function
            seen.add(digest)
        n = copies.get(digest, 1)
        # Payoff: bytes covered across all copies, discounted by how hard a
        # function of this size is to match.
        rows.append((-(n * size) / (size ** 1.5), size, n, module, unit, name))
    rows.sort()
    print(f"{'size':>6s} {'copies':>6s}  module    function  (unit)")
    for _, size, n, module, unit, name in rows[:args.limit]:
        print(f"{size:6d} {n:6d}  {module:9s} {name}  ({unit})")
    print(f"\n{len(rows)} candidates")


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--version", default="RMKE01")
    sub = ap.add_subparsers(dest="cmd")
    u = sub.add_parser("unit")
    u.add_argument("name")
    q = sub.add_parser("queue")
    q.add_argument("--module")
    q.add_argument("--min-size", type=int, default=16)
    q.add_argument("--max-size", type=int, default=400)
    q.add_argument("--limit", type=int, default=30)
    q.add_argument("--named", action="store_true", help="only functions with a real name")
    args = ap.parse_args()
    report, index = load(args.version)
    {"unit": cmd_unit, "queue": cmd_queue}.get(args.cmd, cmd_totals)(report, index, args)


if __name__ == "__main__":
    main()
