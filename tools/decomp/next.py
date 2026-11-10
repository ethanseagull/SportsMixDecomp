#!/usr/bin/env python3
"""Say what to do next.

Reads the state of the checkout (last build, objdiff report, uncommitted
files, parked functions) and prints the one next action, most urgent first:

  1. the full build is missing or older than the sources  -> build
  2. a registered unit has every function finished        -> mark it Matching or Equivalent
  3. a registered unit is partly matched                  -> its next function
  4. an untouched unit whose sibling in another module has source
                                                          -> port_unit.py --write
  5. otherwise                                            -> the easiest untouched unit

Units listed in notes/blocked.txt ("<unit>: <reason>" per line) are skipped.

Usage:
    python tools/decomp/next.py [Module]      e.g. next.py TourF
    python tools/decomp/next.py --list 10     also list the next ten units after it
"""
import argparse
import json
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from copies import Copies  # noqa: E402
import equivalent  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent.parent
BUILD = "py -3.12 tools/decomp/build.py"
DRAFT_OVER = 120   # bytes; smaller functions are quicker to write from the assembly


def newest(paths):
    return max((p.stat().st_mtime for p in paths if p.is_file()), default=0)


def build_state(version):
    ok = ROOT / "build" / version / "ok"
    if not ok.exists():
        return "missing"
    inputs = [ROOT / "configure.py"] + list((ROOT / "config" / version).glob("*/objects.txt"))
    for d in ("src", "include"):
        inputs += list((ROOT / d).rglob("*"))
    inputs += list((ROOT / "config" / version).rglob("*.txt")) + list((ROOT / "config" / version).rglob("*.yml"))
    return "stale" if newest(inputs) > ok.stat().st_mtime else "ok"


def uncommitted():
    try:
        out = subprocess.run(["git", "status", "--porcelain", "--", "src", "include", "config", "configure.py", "tools"],
                             cwd=ROOT, capture_output=True, text=True, timeout=30).stdout
    except (OSError, subprocess.TimeoutExpired):
        return []
    return [line[3:] for line in out.splitlines() if line.strip()]


class UnitState:
    def __init__(self, raw, parked, accepted=()):
        self.module, _, self.name = raw["name"].partition("/")
        meta = raw.get("metadata", {})
        self.auto = meta.get("auto_generated", False)
        self.source = meta.get("source_path")
        self.complete = meta.get("complete", False)
        self.fns = [(f["name"], int(f["size"]), f.get("fuzzy_match_percent", 0.0)) for f in raw.get("functions", [])]
        # functions accepted without an exact match (config/<version>/equivalent.txt) are finished
        self.accepted = [n for n, _, p in self.fns if p < 100.0 and (self.module, n) in accepted]
        self.todo = sorted(((s, n) for n, s, p in self.fns
                            if p < 100.0 and (self.module, n) not in accepted), key=lambda x: x[0])
        self.parked = [n for _, n in self.todo if n in parked]
        self.open = [(s, n) for s, n in self.todo if n not in parked]
        self.code = sum(s for _, s, _ in self.fns)
        self.largest = max((s for _, s, _ in self.fns), default=0)

    @property
    def label(self):
        return f"{self.module} {self.name}"


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("module", nargs="?")
    ap.add_argument("--version", default="RMKE01")
    ap.add_argument("--list", type=int, default=0, metavar="N", help="also list the next N untouched units")
    args = ap.parse_args()

    report_path = ROOT / "build" / args.version / "report.json"
    state = build_state(args.version)
    if not report_path.exists():
        print(f"NEXT: run the full build, there is no report yet\n  {BUILD}")
        return
    report = json.loads(report_path.read_text(encoding="utf-8"))
    try:
        copies = Copies(args.version, report)
    except (OSError, ValueError):
        copies = None

    def copy_line(u, fn):
        o = copies.origin(u.module, fn) if copies else None
        if o:
            kind, (_, src_fn, src) = o
            what = "an exact copy" if kind == "exact" else "the same shape as"
            print(f"  this is {what} of matched {src_fn} in {src}: start from that source")
        return bool(o)
    attempts = ROOT / "notes" / "attempts"
    parked = {p.stem for p in attempts.glob("*.md")} if attempts.is_dir() else set()
    blocked_file = ROOT / "notes" / "blocked.txt"
    blocked = {}
    if blocked_file.exists():
        for line in blocked_file.read_text(encoding="utf-8").splitlines():
            if ":" in line and not line.startswith("#"):
                name, reason = line.split(":", 1)
                blocked[name.strip()] = reason.strip()
    accepted = equivalent.load(args.version)
    units = [UnitState(u, parked, accepted) for u in report["units"]]
    units = [u for u in units if u.fns and f"{u.module}/{u.name}" not in blocked]
    scope = [u for u in units if args.module in (None, u.module)]
    if not scope:
        sys.exit(f"no units in module {args.module!r}")

    m = report["measures"]
    done = [u for u in scope if u.complete]
    started = [u for u in scope if u.source and not u.complete]
    fresh = [u for u in scope if not u.source and not u.auto and u.todo]
    where = args.module or "all"
    print(f"Progress: {m['matched_functions']} / {m['total_functions']} functions, "
          f"{float(m['matched_code_percent']):.3f}% of code (whole game)")
    near = sum(v[0] == "near" for v in accepted.values())
    if accepted:
        print(f"Accepted without an exact match: {near} near, {len(accepted) - near} unverified")
    done += [u for u in scope if u.source and not u.complete and not u.todo and u.accepted]
    started = [u for u in started if u.todo or not u.accepted]
    print(f"Units ({where}): {len(done)} done, {len(started)} started, {len(fresh)} untouched; "
          f"{len(parked)} functions parked, {len(blocked)} units blocked")
    dirty = uncommitted()
    if dirty:
        print(f"Uncommitted: {len(dirty)} files ({', '.join(dirty[:4])}{', ...' if len(dirty) > 4 else ''})")

    def finish(scope_hint):
        print(f"To hand this to a run:  /match-run {scope_hint}")

    if state == "missing":
        print("\nNEXT: run the full build; there is no verified build")
        print(f"  {BUILD}")
        print("  if it fails: fix or revert the last change before anything else")
        return
    if state == "stale":
        # not a blocker: with several sessions at work, someone's sources are always newer
        print(f"Build: sources changed since the last verified build; the numbers may lag. Refresh: {BUILD}")

    ready = [u for u in started if not u.todo]
    if ready:
        u = ready[0]
        print(f"\nNEXT: integrate {u.label}: every function is at 100% but the unit is not marked Matching")
        print(f"  1. py -3.12 tools/decomp/mark.py {u.name} Matching")
        print(f"  2. {BUILD}  (re-runs configure by itself)")
        print("  3. hash check passes: commit the unit. Fails: set it back to NonMatching and log why")
        finish(f"finish {u.name}")
        return

    active = [u for u in started if u.open]
    if active:
        u = max(active, key=lambda x: len(x.fns) - len(x.todo))
        # copies of matched code first: they cost one attempt
        ready = [(s, n) for s, n in u.open if copies and copies.origin(u.module, n)]
        size, fn = (ready or u.open)[0]
        print(f"\nNEXT: continue {u.label}: {len(u.fns) - len(u.todo)} of {len(u.fns)} functions matched, "
              f"{len(u.parked)} parked")
        print(f"  source: {u.source}")
        print(f"  next function: {fn} ({size} bytes)")
        if copy_line(u, fn):
            pass
        elif size > DRAFT_OVER:
            print(f"  draft:  py -3.12 tools/decomp/draft.py \"{fn}\" --module {u.module}")
        print(f"  copies: py -3.12 tools/decomp/similar.py \"{fn}\" --matched")
        print(f"  score:  py -3.12 tools/decomp/status.py unit {u.name}")
        finish(f"finish {u.name}")
        return

    stuck = [u for u in started if u.todo]
    if stuck:
        print(f"\nNote: {len(stuck)} started units have only parked functions left "
              f"({', '.join(u.name for u in stuck[:3])}); they wait for more matched neighbours.")

    if not fresh:
        if args.module:
            print(f"\nNEXT: nothing left to start in {args.module}. Pick another module, or revisit notes/attempts/.")
        else:
            print("\nNEXT: every module unit is started. Carve units in main.dol, or revisit notes/attempts/.")
        return
    # a unit another module already has source for starts from that source
    try:
        import port_unit
        pu = port_unit.Units(args.version)
    except (OSError, ValueError, ImportError):
        pu = None
    if pu:
        best = None
        for v in fresh:
            key = f"{v.module}/{v.name}"
            if key not in pu.units or v.code == 0:
                continue
            sib = pu.siblings(key)
            if sib and sib[0][0] >= 0.6 * v.code and (best is None or sib[0][0] > best[0]):
                best = (sib[0][0], v, sib[0][1])
        if best:
            covered, v, src = best
            print(f"\nNEXT: port {v.label} from {src}: {covered} of its {v.code} bytes are matched there")
            print(f"  py -3.12 tools/decomp/port_unit.py {v.name} --write")
            print("  then act on its CHECK/TODO lines and match the rest as usual")
            finish(v.module)
            return

    # easiest first: small functions match at the first attempt far more often than large ones
    # (a one- or two-function unit is often a fragment of its neighbour: leave those for later)
    fresh.sort(key=lambda u: (len(u.fns) < 3, u.largest > 400, u.largest > 1200, u.code, u.label))
    u = fresh[0]
    size, fn = u.open[0] if u.open else u.todo[0]
    lib = u.module
    print(f"\nNEXT: start {u.label}: {len(u.fns)} functions, {u.code} bytes, largest {u.largest}")
    print(f"  1. add  NonMatching {u.name}.cpp  to config/RMKE01/{lib}/objects.txt, "
          f"create src/{u.name}.cpp")
    print(f"  2. {BUILD}  (must still pass; then tools/decomp/boundaries.py {u.name} once it compiles)")
    print(f"  3. first function: {fn} ({size} bytes)")
    if size > DRAFT_OVER:
        print(f"     draft: py -3.12 tools/decomp/draft.py \"{fn}\" --module {u.module}")
    print(f"  4. score: py -3.12 tools/decomp/status.py unit {u.name}")
    if copies:
        todo, exact, shape = copies.unit_summary(u.module, u.name)
        if exact + shape:
            print(f"  {exact + shape} of its {todo} bytes are copies of matched code: "
                  f"py -3.12 tools/decomp/copies.py unit {u.name}")
    finish(u.module if u.module != "main" else "")
    if args.list:
        print(f"\nThen ({'in ' + args.module if args.module else 'any module'}):")
        for v in fresh[1:args.list + 1]:
            print(f"  {v.module:8s} {v.name:44s} {len(v.fns):4d} fn {v.code:7d} bytes, largest {v.largest}")


if __name__ == "__main__":
    main()
