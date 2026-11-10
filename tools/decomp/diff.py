#!/usr/bin/env python3
"""Compare one unit's compiled functions with the original and say what kind of difference is left.

Reading two disassemblies side by side to decide "near" or "unverified" cost a
matcher several turns per function. This does the comparison and labels it:

  match         same instructions (names of anonymous or file-local data may differ)
  registers     same instructions once registers are renumbered by first use:
                only register allocation differs            -> near candidate
  schedule      same instructions in another order (registers ignored)
                                                            -> near candidate
  real          instructions differ; hints say how (frame size, constants,
                calls, loads/stores, conversions, branches, constant addressing)
  missing       the function is not in the compiled object

Usage:
    python tools/decomp/diff.py <unit>                  every function not at 100%
    python tools/decomp/diff.py <unit> <function>       one function, with the full diff
    python tools/decomp/diff.py <unit> --all            every function
    python tools/decomp/diff.py <unit> --json           machine-readable

<unit> is the unit name (game/menu/CFade) or any unique part of it. The
compiled object must be current: build it first
(py -3.12 tools/decomp/build.py build/RMKE01/src/<unit>.o build/RMKE01/report.json).
"""
import argparse
import difflib
import json
import re
import subprocess
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
INSN = re.compile(r"^/\* .*?\*/\t(\S+)(?:\s+(.*))?$")
FIXED_REGS = {"r0", "r1", "r2", "r13", "f0"}
IMM_OPS = ("li", "lis", "addi", "cmpwi", "cmplwi", "subi", "ori")
NUM = re.compile(r"(?<![\w(])-?0x[0-9a-fA-F]+|(?<![\w(r])\b\d+\b(?!\()")


def report_units(version):
    report = json.loads((ROOT / "build" / version / "report.json").read_text(encoding="utf-8"))
    out = {}
    for u in report["units"]:
        module, _, name = u["name"].partition("/")
        out[name] = (module, {f["name"]: f.get("fuzzy_match_percent", 0.0) for f in u.get("functions", [])},
                     u.get("metadata", {}).get("source_path"))
    return out


def find_unit(units, query):
    if query in units:
        return query
    hits = [n for n in units if query in n]
    exact = [n for n in hits if n.endswith("/" + query)]
    hits = exact if len(exact) == 1 else hits
    if len(hits) != 1:
        sys.exit(f"unit {query!r}: {len(hits)} matches {hits[:6]}")
    return hits[0]


def parse(text):
    """{function: [(mnemonic, operands)]}"""
    out, cur = {}, None
    for line in text.splitlines():
        if line.startswith(".fn "):
            rest = line[4:].strip()
            cur = rest[1:rest.index('"', 1)] if rest.startswith('"') else rest.split(",")[0].strip()
            out[cur] = []
        elif line.startswith(".endfn"):
            cur = None
        elif cur is not None:
            m = INSN.match(line)
            if m:
                out[cur].append((m.group(1), (m.group(2) or "").strip()))
    return out


def mask_symbols(ops):
    ops = re.sub(r"\.L_[0-9A-Fa-f]+", ".L", ops)
    ops = re.sub(r'"[^"]*"', "ANON", ops)  # quoted: file-local statics, @unnamed@ / @LOCAL@ names
    ops = re.sub(r"(?:lbl_\w+|\.\.\.\w+\.\d+|@\d+|\w*\$\d+)", "ANON", ops)
    return ops


def canon_regs(insns):
    """Renumber GPRs and FPRs by order of first use."""
    maps = {"r": {}, "f": {}}

    def sub(m):
        reg = m.group(0)
        if reg in FIXED_REGS:
            return reg
        table = maps[reg[0]]
        if reg not in table:
            table[reg] = f"{reg[0].upper()}{len(table)}"
        return table[reg]
    return [(mn, re.sub(r"\b[rf]\d{1,2}\b", sub, ops)) for mn, ops in insns]


def no_regs(insns):
    return [(mn, re.sub(r"\b[rf]\d{1,2}\b", "R", ops)) for mn, ops in insns]


def hints(a, b):
    out = []
    fa = [o for m, o in a if m == "stwu"][:1]
    fb = [o for m, o in b if m == "stwu"][:1]
    if fa != fb:
        out.append(f"stack frame differs ({fa or 'none'} vs {fb or 'none'}): a local, temporary or saved register more or less")
    calls_a = [o for m, o in a if m == "bl"]
    calls_b = [o for m, o in b if m == "bl"]
    if calls_a != calls_b:
        if Counter(calls_a) == Counter(calls_b):
            out.append("same calls in a different order")
        else:
            missing = Counter(calls_a) - Counter(calls_b)
            extra = Counter(calls_b) - Counter(calls_a)
            if missing:
                out.append("calls only in the original: " + ", ".join(sorted(missing)))
            if extra:
                out.append("calls only in ours: " + ", ".join(sorted(extra)))
    ia = Counter(NUM.findall(" ".join(o for m, o in a if m in IMM_OPS)))
    ib = Counter(NUM.findall(" ".join(o for m, o in b if m in IMM_OPS)))
    if ia != ib:
        out.append(f"immediates differ: original has {dict(ia - ib) or '-'}, ours {dict(ib - ia) or '-'}")

    def mem(ins):
        return Counter(re.sub(r"\b[rf]\d+\b", "R", f"{m} {o}") for m, o in ins
                       if re.match(r"(l|st)[bhwf]", m) and "(r1)" not in o and "@l(" not in o)
    ma, mb = mem(a), mem(b)
    if ma != mb:
        out.append(f"memory accesses differ: {sum((ma - mb).values())} only in the original, "
                   f"{sum((mb - ma).values())} only in ours (field offsets, widths or signedness)")
    da = sum(1 for m, o in a if "@l(" in o and m.startswith("l"))
    db = sum(1 for m, o in b if "@l(" in o and m.startswith("l"))
    if abs(da - db) >= 2:
        out.append(f"loads through @l: original {da}, ours {db}: constants or globals addressed directly in one "
                   "and through a kept base register in the other (no behavioural difference by itself)")

    def ext(ins):
        return Counter(m for m, _ in ins if m in ("extsb", "extsh", "clrlwi", "rlwinm", "xoris", "frsp", "fctiwz"))
    if ext(a) != ext(b):
        out.append(f"conversions differ: original {dict(ext(a))}, ours {dict(ext(b))} (a type or a cast)")

    def br(ins):
        return Counter(m for m, _ in ins if m.startswith("b") and m not in ("bl", "blr", "bctr", "bctrl"))
    if br(a) != br(b):
        out.append(f"branches differ: original {dict(br(a))}, ours {dict(br(b))} (condition sense or control flow)")
    if len(a) != len(b):
        out.append(f"length {len(a)} vs {len(b)} instructions")
    return out


def classify(orig, ours):
    a = [(m, mask_symbols(o)) for m, o in orig]
    b = [(m, mask_symbols(o)) for m, o in ours]
    if a == b:
        return "match", []
    if canon_regs(a) == canon_regs(b):
        return "registers", []
    if Counter(no_regs(a)) == Counter(no_regs(b)):
        return "schedule", []
    return "real", hints(a, b)


def disasm_ours(version, unit):
    obj = ROOT / "build" / version / "src" / (unit + ".o")
    if not obj.exists():
        sys.exit(f"{obj.relative_to(ROOT)} does not exist: build it first")
    out = ROOT / "build" / "decomp" / "disasm" / (unit.replace("/", "_") + ".s")
    out.parent.mkdir(parents=True, exist_ok=True)
    if not out.exists() or out.stat().st_mtime < obj.stat().st_mtime:
        dtk = ROOT / "build" / "tools" / "dtk.exe"
        r = subprocess.run([str(dtk), "elf", "disasm", str(obj), str(out)], capture_output=True, text=True)
        if r.returncode:
            sys.exit("dtk elf disasm failed:\n" + r.stderr)
    return out.read_text(encoding="utf-8", errors="replace")


def analyse(version, unit, module, wanted=None):
    """[(function, class, hints, original insns, our insns)]"""
    orig = parse((ROOT / "build" / version / module / "asm" / (unit + ".s")).read_text(encoding="utf-8", errors="replace"))
    ours = parse(disasm_ours(version, unit))
    rows = []
    for fn, insns in orig.items():
        if wanted is not None and fn not in wanted:
            continue
        if fn not in ours:
            rows.append((fn, "missing", ["not in the compiled object (named differently, or not written)"], insns, []))
            continue
        cls, why = classify(insns, ours[fn])
        rows.append((fn, cls, why, insns, ours[fn]))
    return rows


def verdict(cls, score):
    if cls in ("registers", "schedule"):
        if score is not None and score >= 85.0:
            return 'near: equivalent.py add <Module> <function> "register allocation/order only"'
        return "register/order only but under 85%: try tools/decomp/autopilot.py regs, else unverified"
    if cls == "real":
        return "fix the source; when the budget is spent, unverified with these hints as the reason"
    if cls == "missing":
        return "write it, or check its name and the unit boundary (tools/decomp/boundaries.py)"
    return ""


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("unit")
    ap.add_argument("function", nargs="?")
    ap.add_argument("--all", action="store_true")
    ap.add_argument("--json", action="store_true")
    ap.add_argument("--version", default="RMKE01")
    args = ap.parse_args()
    units = report_units(args.version)
    unit = find_unit(units, args.unit)
    module, scores, _ = units[unit]

    if args.function:
        wanted = {args.function}
    elif args.all:
        wanted = None
    else:
        wanted = {f for f, s in scores.items() if s < 100.0}
        if not wanted:
            print(f"{module}/{unit}: every function at 100%")
            return
    rows = analyse(args.version, unit, module, wanted)
    if args.json:
        print(json.dumps([{"function": fn, "score": scores.get(fn), "class": cls, "hints": why}
                          for fn, cls, why, _, _ in rows], indent=1))
        return
    for fn, cls, why, a, b in rows:
        score = scores.get(fn)
        print(f"{cls:9s} {score if score is not None else 0:5.1f}%  {fn}")
        for h in why:
            print(f"            - {h}")
        v = verdict(cls, score)
        if v:
            print(f"            => {v}")
        if args.function and cls != "match":
            ca = [f"{m} {o}" for m, o in canon_regs([(m, mask_symbols(o)) for m, o in a])]
            cb = [f"{m} {o}" for m, o in canon_regs([(m, mask_symbols(o)) for m, o in b])]
            print("\n  (registers renumbered by first use; - original, + ours)")
            for line in difflib.unified_diff(ca, cb, "original", "ours", n=1, lineterm=""):
                print("  " + line)


if __name__ == "__main__":
    main()
