#!/usr/bin/env python3
"""Find unmatched functions that are copies of code already matched.

The game repeats a lot of code: inline functions emitted in several files, and
per-sport classes (CChrCoreBasket, CChrCoreVolley, ...) with the same bodies.
Once one copy is matched, the others need its source with the names changed,
not a fresh attempt.

  exact   same instructions, registers and constants; only symbol names differ
  shape   same instruction sequence; constants or field offsets differ

Usage:
    python tools/decomp/copies.py                    everything that can be copied now, by unit
    python tools/decomp/copies.py --module TourF
    python tools/decomp/copies.py unit <unit>        per function: where to copy from
"""
import argparse
import json
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent


class Copies:
    def __init__(self, version="RMKE01", report=None):
        base = ROOT / "build"
        if report is None:
            report = json.loads((base / version / "report.json").read_text(encoding="utf-8"))
        index = json.loads((base / "decomp" / f"index_{version}.json").read_text(encoding="utf-8"))
        self.info = {(f["module"], f["name"]): f for f in index}
        self.exact = defaultdict(list)   # hash -> matched (module, name, source)
        self.shape = defaultdict(list)   # mnemonic sequence -> matched
        self.units = {}                  # (module, unit) -> [(name, size, matched)]
        for unit in report["units"]:
            module, _, uname = unit["name"].partition("/")
            source = unit.get("metadata", {}).get("source_path")
            rows = []
            for fn in unit.get("functions", []):
                done = fn.get("fuzzy_match_percent", 0.0) >= 100.0 and source is not None
                rows.append((fn["name"], int(fn["size"]), done))
                f = self.info.get((module, fn["name"]))
                if done and f:
                    self.exact[f["hash"]].append((module, fn["name"], source))
                    self.shape[f["ops"]].append((module, fn["name"], source))
            self.units[(module, uname)] = rows

    def origin(self, module, name):
        """('exact'|'shape', (module, name, source)) of a matched copy, or None."""
        f = self.info.get((module, name))
        if not f or f["size"] < 12:   # a one- or two-instruction body needs no template
            return None
        if self.exact.get(f["hash"]):
            return "exact", self.exact[f["hash"]][0]
        if f["size"] >= 16 and self.shape.get(f["ops"]):
            return "shape", self.shape[f["ops"]][0]
        return None

    def unit_summary(self, module, unit):
        """(bytes unmatched, bytes of those with an exact copy, bytes with a same-shape copy)."""
        todo = exact = shape = 0
        for name, size, done in self.units.get((module, unit), []):
            if done:
                continue
            todo += size
            o = self.origin(module, name)
            if o:
                exact += size * (o[0] == "exact")
                shape += size * (o[0] == "shape")
        return todo, exact, shape


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("command", nargs="?", choices=["unit"])
    ap.add_argument("name", nargs="?")
    ap.add_argument("--module")
    ap.add_argument("--version", default="RMKE01")
    ap.add_argument("--limit", type=int, default=30)
    args = ap.parse_args()
    c = Copies(args.version)

    if args.command == "unit":
        hits = [k for k in c.units if args.name and args.name in k[1] and args.module in (None, k[0])]
        if not hits:
            raise SystemExit(f"no unit matching {args.name!r}")
        for module, unit in hits[:5]:
            print(f"{module} {unit}")
            for name, size, done in c.units[(module, unit)]:
                if done:
                    continue
                o = c.origin(module, name)
                if o:
                    kind, (m, n, src) = o
                    print(f"  {kind:5s} {size:6d}  {name}\n               <- {n}  ({src})")
            todo, exact, shape = c.unit_summary(module, unit)
            print(f"  unmatched {todo} bytes: {exact} exact copies, {shape} same shape")
        return

    rows = []
    for (module, unit), fns in c.units.items():
        if args.module not in (None, module):
            continue
        todo, exact, shape = c.unit_summary(module, unit)
        if exact or shape:
            rows.append((exact + shape, exact, shape, todo, module, unit))
    rows.sort(reverse=True)
    print(f"{'exact':>7s} {'shape':>7s} {'unmatched':>9s}  unit")
    for _, exact, shape, todo, module, unit in rows[:args.limit]:
        print(f"{exact:7d} {shape:7d} {todo:9d}  {module} {unit}")
    print(f"\n{sum(r[1] for r in rows)} bytes are exact copies of matched code, "
          f"{sum(r[2] for r in rows)} the same shape, in {len(rows)} units")


if __name__ == "__main__":
    main()
