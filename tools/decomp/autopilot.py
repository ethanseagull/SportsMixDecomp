#!/usr/bin/env python3
"""Matching work that needs no agent. Run it before handing a module to a session.

  ports   For every untouched unit whose sibling in another module has source
          (port_unit.py), write the renamed copy, build it and keep it only if
          it compiles. Units where every function is 100% are marked Matching
          and must pass the full build and hash check, else they go back to
          NonMatching; units with less than --keep of their bytes at 100% are
          removed again. What stays is a scored draft for the matcher.

  regs    For one function whose only difference is register allocation
          (diff.py says "registers"/"schedule"), try a bounded, deterministic
          set of local-declaration changes, the fix that worked by hand
          three times: reorder runs of adjacent declarations, and hoist a
          declaration with an initialiser to the top of its block. Only an
          exact match is kept; otherwise the source is left as it was.
          This is not the permuter CLAUDE.md rules out: every candidate is
          ordinary, readable C++ that differs only in where a local is declared.

  all     ports, then regs for every register-only function of every unit with
          source in the scope.

Usage:
    python tools/decomp/autopilot.py ports [--module BasketF] [--keep 0.5] [--dry]
    python tools/decomp/autopilot.py regs <unit> <function> [--max 24]
    python tools/decomp/autopilot.py all [--module BasketF]

Every change it keeps is logged to notes/log.md. It never commits.
"""
import argparse
import datetime
import itertools
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import diff  # noqa: E402
import objects  # noqa: E402
import port_unit  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent.parent
PY = sys.executable
VERSION = "RMKE01"


def build(*targets, quiet=True):
    out = subprocess.DEVNULL if quiet else None
    return subprocess.call([PY, "tools/decomp/build.py", *targets], cwd=ROOT, stdout=out, stderr=out) == 0


def obj(unit):
    return f"build/{VERSION}/src/{unit}.o"


def hashes_ok():
    if not build():
        return False
    dtk = ROOT / "build" / "tools" / "dtk.exe"
    return subprocess.run([str(dtk), "shasum", "-c", f"config/{VERSION}/build.sha1"], cwd=ROOT,
                          capture_output=True).returncode == 0


def log(lines):
    p = ROOT / "notes" / "log.md"
    if not p.exists():
        return
    today = datetime.date.today().isoformat()
    with p.open("a", encoding="utf-8", newline="\n") as fh:
        for line in lines:
            fh.write(f"- {today} | autopilot | {line}\n")


# ---------------------------------------------------------------- ports

def cmd_ports(args):
    us = port_unit.Units(VERSION)
    todo = []
    for key, u in us.units.items():
        if u["source"] or (args.module and u["module"] != args.module):
            continue
        if objects.find(u["name"] + ".cpp")[0]:
            continue
        code = sum(s for _, s, _ in u["fns"])
        sib = us.siblings(key)
        if code and sib and sib[0][0] >= args.min * code:
            todo.append((key, sib[0][1], code, sib[0][0]))
    todo.sort(key=lambda r: -r[3])
    print(f"{len(todo)} units to port")
    if args.dry:
        for key, src, code, cov in todo:
            print(f"  {key} <- {src}  ({cov}/{code} bytes)")
        return
    written = {}
    for key, src, _, _ in todo:
        written[key] = port_unit.write_port(us, key, src, log=lambda *_: None)
    # compile each on its own so one failure does not hide the others
    build(f"build/{VERSION}/report.json")  # re-runs configure for the new objects.txt lines
    failed = []
    for key in list(written):
        name = us.units[key]["name"]
        if not build(obj(name)):
            failed.append(key)
            port_unit.undo_port(written.pop(key), name)
    build(f"build/{VERSION}/report.json")
    units = diff.report_units(VERSION)
    kept, removed, matching = [], [], []
    for key, res in list(written.items()):
        name = us.units[key]["name"]
        scores = units.get(name, (None, {}, None))[1]
        sizes = {n: s for n, s, _ in us.units[key]["fns"]}
        total = sum(sizes.values()) or 1
        done = sum(sizes.get(f, 0) for f, s in scores.items() if s >= 100.0)
        if scores and all(s >= 100.0 for s in scores.values()):
            matching.append(key)
        elif done / total >= args.keep:
            kept.append((key, done, total))
        else:
            removed.append((key, done, total))
            port_unit.undo_port(written.pop(key), name)
    # Matching candidates: all together, then one by one if the hash check fails
    confirmed = []
    if matching:
        for key in matching:
            objects.set_status(us.units[key]["name"], "Matching")
        if hashes_ok():
            confirmed = matching
        else:
            for key in matching:
                objects.set_status(us.units[key]["name"], "NonMatching")
            for key in matching:
                objects.set_status(us.units[key]["name"], "Matching")
                if hashes_ok():
                    confirmed.append(key)
                else:
                    objects.set_status(us.units[key]["name"], "NonMatching")
                    kept.append((key, 1, 1))
    if not hashes_ok():
        sys.exit("the full build fails after the ports; look at the last units added (git status)")
    print(f"Matching: {len(confirmed)}")
    for key in confirmed:
        print(f"  {key}")
    print(f"kept as NonMatching drafts: {len(kept)}")
    for key, done, total in kept:
        print(f"  {key}  {done}/{total} bytes at 100%")
    print(f"removed (did not compile): {len(failed)}; removed (under {args.keep:.0%}): {len(removed)}")
    log([f"ports: {len(confirmed)} units Matching ({', '.join(k.split('/')[-1] for k in confirmed) or '-'}); "
         f"{len(kept)} drafts kept NonMatching; {len(failed)} did not compile, {len(removed)} under {args.keep:.0%}"])


# ---------------------------------------------------------------- regs

DECL = re.compile(r"^(\s*)((?:const\s+|unsigned\s+|signed\s+|register\s+|static\s+)*[A-Za-z_][\w:<>,]*"
                  r"(?:\s*[\*&]+\s*|\s+)[A-Za-z_]\w*)(\s*=\s*[^;]+)?;\s*(//.*)?$")
NOT_TYPES = {"return", "delete", "goto", "case", "else", "throw", "new"}


def demangled(module, unit, fn):
    text = (ROOT / "build" / VERSION / module / "asm" / (unit + ".s")).read_text(encoding="utf-8", errors="replace")
    lines = text.splitlines()
    for i, line in enumerate(lines):
        if line.startswith(".fn ") and line[4:].split(",")[0].strip().strip('"') == fn:
            for back in range(i - 1, max(i - 4, 0), -1):
                if lines[back].startswith("# ") and "(" in lines[back]:
                    return lines[back][2:].strip()
    return None


def find_body(src, qual):
    """(start, end) line indexes of the body's braces for the definition of qual (ns::Class::fn)."""
    parts = qual.split("(")[0].split("::")
    name = parts[-1]
    owner = parts[-2] if len(parts) > 1 else None
    pat = re.compile((re.escape(owner) + r"::" if owner else "") + re.escape(name) + r"\s*\(")
    lines = src.splitlines()
    for i, line in enumerate(lines):
        if pat.search(line) and not line.rstrip().endswith(";"):
            depth, start = 0, None
            for j in range(i, len(lines)):
                for ch in lines[j]:
                    if ch == "{":
                        depth += 1
                        if start is None:
                            start = j
                    elif ch == "}":
                        depth -= 1
                        if start is not None and depth == 0:
                            return start, j
                if j > i + 3 and start is None:
                    break
    return None


def decl_lines(lines, start, end):
    out = []
    for k in range(start + 1, end):
        m = DECL.match(lines[k])
        if m and m.group(2).split()[0] not in NOT_TYPES:
            out.append(k)
    return out


def _name(line):
    return DECL.match(line).group(2).split()[-1].lstrip("*&")


def _uses_later(lines, order):
    """True if an initialiser in `order` reads a name declared after it."""
    for pos, k in enumerate(order):
        init = DECL.match(lines[k]).group(3) or ""
        if any(re.search(r"\b" + re.escape(_name(lines[p])) + r"\b", init) for p in order[pos + 1:]):
            return True
    return False


def candidates(lines, start, end):
    """Declaration changes, cheapest first: adjacent swaps, hoists, then whole-run permutations."""
    decls = decl_lines(lines, start, end)
    runs, run = [], []
    for k in decls:
        if run and k != run[-1] + 1:
            runs.append(run)
            run = []
        run.append(k)
    if run:
        runs.append(run)
    seen = {"\n".join(lines)}

    def emit(what, new):
        key = "\n".join(new)
        if key not in seen:
            seen.add(key)
            return [(what, new)]
        return []

    # 1. swap two adjacent declarations
    for run in runs:
        for a, b in zip(run, run[1:]):
            order = [b, a]
            if _uses_later(lines, order):
                continue
            new = list(lines)
            new[a], new[b] = lines[b], lines[a]
            yield from emit(f"swap lines {a + 1} and {b + 1}", new)
    # 2. hoist a declaration with an initialiser: declare it earlier, assign in place
    for k in decls:
        m = DECL.match(lines[k])
        if not m.group(3):
            continue
        indent, decl, init = m.group(1), m.group(2), m.group(3)
        var = _name(lines[k])
        for pos in sorted({start + 1, *(d for d in decls if d < k)}):
            new = list(lines)
            new[k] = f"{indent}{var}{init};" + (f" {m.group(4)}" if m.group(4) else "")
            lead = lines[pos][:len(lines[pos]) - len(lines[pos].lstrip())] or indent
            new.insert(pos, f"{lead}{decl.strip()};")
            yield from emit(f"hoist '{var}' to line {pos + 1}", new)
    # 3. any order of a run of adjacent declarations
    for run in runs:
        if len(run) < 3 or len(run) > 6:
            continue
        for perm in itertools.permutations(run):
            if list(perm) == run or _uses_later(lines, list(perm)):
                continue
            new = list(lines)
            for dst, src in zip(run, perm):
                new[dst] = lines[src]
            yield from emit(f"reorder lines {run[0] + 1}-{run[-1] + 1}", new)


def score_of(unit, fn):
    build(f"build/{VERSION}/report.json")
    return diff.report_units(VERSION)[unit][1].get(fn, 0.0)


def cmd_regs(unit_query, fn, limit, quiet=False):
    """Hill-climb over declaration changes. Every candidate only moves where a
    local is declared, so keeping one that raises the score cannot change
    behaviour; the climb restarts from it. Stops at 100%, at the budget, or
    when no candidate helps."""
    units = diff.report_units(VERSION)
    unit = diff.find_unit(units, unit_query)
    module, scores, source = units[unit]
    if not source:
        sys.exit(f"{unit} has no source")
    path = ROOT / source
    qual = demangled(module, unit, fn)
    if not qual:
        print(f"  {fn}: no demangled name in the split asm; skipped")
        return False
    original = path.read_text(encoding="utf-8")
    cur = original.split("\n")
    best = scores.get(fn, 0.0)
    start_score = best
    steps, tried = [], 0
    improved = True
    while improved and tried < limit and best < 100.0:
        improved = False
        body = find_body("\n".join(cur), qual)
        if not body:
            print(f"  {fn}: definition of {qual} not found in {source}; skipped")
            break
        for what, new in candidates(cur, body[0], body[1]):
            if tried >= limit:
                break
            tried += 1
            path.write_text("\n".join(new), encoding="utf-8", newline="\n")
            if not build(obj(unit)):
                continue
            rows = diff.analyse(VERSION, unit, module, {fn})
            if not rows or rows[0][1] not in ("match", "registers", "schedule"):
                continue  # only moves that keep the function register/order-only
            s = score_of(unit, fn)
            if s > best:
                best, cur, improved = s, new, True
                steps.append(f"{what} -> {s:.1f}%")
                break
    if best > start_score:
        path.write_text("\n".join(cur), encoding="utf-8", newline="\n")
        build(obj(unit), f"build/{VERSION}/report.json")
        print(f"  {fn}: {start_score:.1f}% -> {best:.1f}% in {tried} tries: " + "; ".join(steps))
        log([f"regs: {module} {unit} {fn} {start_score:.1f}% -> {best:.1f}% by declaration order ({'; '.join(steps)})"])
        return best >= 100.0
    path.write_text(original, encoding="utf-8", newline="\n")
    build(obj(unit), f"build/{VERSION}/report.json")
    if not quiet:
        print(f"  {fn}: no gain in {tried} tries; source restored")
    return False


def cmd_all(args):
    cmd_ports(args)
    units = diff.report_units(VERSION)
    for unit, (module, scores, source) in sorted(units.items()):
        if not source or (args.module and module != args.module):
            continue
        open_fns = [f for f, s in scores.items() if s < 100.0]
        if not open_fns:
            continue
        for fn, cls, *_ in diff.analyse(VERSION, unit, module, set(open_fns)):
            if cls in ("registers", "schedule"):
                print(f"{module}/{unit}:")
                cmd_regs(unit, fn, args.max)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("cmd", choices=["ports", "regs", "all"])
    ap.add_argument("unit", nargs="?")
    ap.add_argument("function", nargs="?")
    ap.add_argument("--module")
    ap.add_argument("--min", type=float, default=0.6, help="ports: sibling must cover this share of the unit")
    ap.add_argument("--keep", type=float, default=0.5, help="ports: keep a draft with at least this share at 100%%")
    ap.add_argument("--max", type=int, default=24, help="regs: candidates per function")
    ap.add_argument("--dry", action="store_true")
    args = ap.parse_args()
    if args.cmd == "ports":
        cmd_ports(args)
    elif args.cmd == "regs":
        if not (args.unit and args.function):
            sys.exit("regs needs <unit> <function>")
        cmd_regs(args.unit, args.function, args.max)
    else:
        cmd_all(args)


if __name__ == "__main__":
    main()
