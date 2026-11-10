#!/usr/bin/env python3
"""Find the data ranges of a main.dol unit and add them to its split.

The DOL units were carved from the EU project's layout, which gives each unit
only a .text range. A compiled unit also emits data (float constants in
.sdata2, vtables in .data, strings in .rodata, statics in .sbss); while the
split has no range for them, the original copies stay in the auto-generated
data units and the link fails (multiply-defined) or the DOL grows.

For every data object our object emits, this finds the original address:
  - by name, when the object is global and named in symbols.txt;
  - otherwise through the code: the same instruction of the same function
    refers to it in ours and in the original slice.
Each section's objects must sit at one consistent offset from the original;
the range is then [base, base + our section size).

Usage:
    python tools/decomp/dol_data.py <unit>            report
    python tools/decomp/dol_data.py <unit> --write    add the ranges to config/RMKE01/splits.txt
"""
import argparse
import json
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import diff  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent.parent
SYM = re.compile(r'^(\S+) = (\.\w+):0x([0-9A-Fa-f]+); // type:(\w+)(?: size:0x([0-9A-Fa-f]+))?')
REF = re.compile(r'("[^"]+"|[A-Za-z_@$.][\w@$.<>,]*)@(ha|h|l|sda21)\b|\b(?:bl?|b[a-z]*)\s+("[^"]+"|[A-Za-z_@$][\w@$.<>,]*)\s*$')
SEC_HDR = re.compile(r'^# 0x([0-9A-F]+)\.\.0x([0-9A-F]+) \| size: 0x([0-9A-F]+)')
OBJ = re.compile(r'^# (\.\w+):0x([0-9A-F]+) \| size: 0x([0-9A-F]+)')
OBJNAME = re.compile(r'^\.obj ("[^"]+"|\S+), (\w+)')
SECTIONS = (".data", ".rodata", ".sdata", ".sdata2", ".sbss", ".sbss2", ".bss", ".ctors", ".dtors")


def load_symbols(version):
    by_name, by_addr = {}, {}
    for line in (ROOT / "config" / version / "symbols.txt").read_text(encoding="utf-8").splitlines():
        m = SYM.match(line)
        if m:
            entry = (m.group(2), int(m.group(3), 16), int(m.group(5) or "0", 16))
            by_name[m.group(1)] = entry
            by_addr.setdefault(int(m.group(3), 16), m.group(1))
    return by_name, by_addr


def our_data(text):
    """{section: (size, [(name, offset, size)])}"""
    out, size, cur = {}, None, None
    pending = None
    for line in text.splitlines():
        m = SEC_HDR.match(line)
        if m:
            size = int(m.group(3), 16)
            continue
        s = line.strip()
        if s.startswith(".section") or s in (".text", ".data", ".bss", ".rodata"):
            name = s.split()[1].strip('",') if s.startswith(".section") else s
            cur = name if name in SECTIONS else None
            if cur:
                out.setdefault(cur, [size, []])
            continue
        m = OBJ.match(line)
        if m:
            pending = (int(m.group(2), 16), int(m.group(3), 16))
            continue
        m = OBJNAME.match(s)
        if m and cur and pending:
            out[cur][1].append((m.group(1).strip('"'), pending[0], pending[1]))
            pending = None
    return out


def refs(insns):
    out = []
    for _, ops in insns:
        m = REF.search(ops)
        out.append((m.group(1) or m.group(3)).strip('"') if m else None)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("unit")
    ap.add_argument("--write", action="store_true")
    ap.add_argument("--version", default="RMKE01")
    args = ap.parse_args()
    unit = args.unit[:-4] if args.unit.endswith(".cpp") else args.unit

    by_name, by_addr = load_symbols(args.version)
    ours_text = diff.disasm_ours(args.version, unit)
    orig_text = (ROOT / "build" / args.version / "asm" / (unit + ".s")).read_text(encoding="utf-8", errors="replace")
    ours, orig = diff.parse(ours_text), diff.parse(orig_text)

    # our symbol -> original symbol, from the same instruction of the same function
    pair = {}
    for fn, insns in ours.items():
        if fn in orig and len(orig[fn]) == len(insns):
            for a, b in zip(refs(insns), refs(orig[fn])):
                if a and b:
                    pair.setdefault(a, b)

    # Functions of the slice still under a generated name: ours at the same
    # position, absent from the original by name, same length.
    o_only = [f for f in orig if f not in ours]
    u_only = [f for f in ours if f not in orig]
    o_list, u_list = list(orig), list(ours)
    for f in o_only:
        if re.match(r"fn_[0-9A-F]{8}$", f):
            i = o_list.index(f)
            if i < len(u_list) and u_list[i] in u_only and len(ours[u_list[i]]) == len(orig[f]):
                pair.setdefault(u_list[i], f)
                for a, b in zip(refs(ours[u_list[i]]), refs(orig[f])):
                    if a and b:
                        pair.setdefault(a, b)

    # ... and from the same word of the same named data object (vtables)
    ours_words = data_words(ours_text)
    if ours_words:
        orig_words = {}
        for f in current_asm(args.version):
            if f.name.startswith("auto_") and "_text" in f.name:
                continue
            text = f.read_text(encoding="utf-8", errors="replace")
            if any(f".obj {n}," in text for n in ours_words):
                orig_words.update({k: v for k, v in data_words(text).items() if k in ours_words})
        for name, words in ours_words.items():
            if name in orig_words and len(orig_words[name]) == len(words):
                for a, b in zip(words, orig_words[name]):
                    if a and b:
                        pair.setdefault(a, b)

    # Generated original names our code gives a real name to: rename them in
    # symbols.txt, so weak copies we emit fold into the original's and
    # objdiff pairs the functions.
    renames = {b: a for a, b in pair.items()
               if re.match(r"(fn|lbl)_[0-9A-F]{8}$", b) and not a.startswith("@") or
               (b.startswith(("fn_", "lbl_")) and a.startswith("@LOCAL@"))}
    renames = {b: a for b, a in renames.items() if not re.match(r"(fn|lbl)_[0-9A-F]{8}$", a)}
    for b, a in sorted(renames.items()):
        print(f"RENAME {b} -> {a}")
    if args.write and renames:
        rename_symbols(args.version, renames)
        by_name, by_addr = load_symbols(args.version)
        pair = {a: renames.get(b, b) for a, b in pair.items()}

    data = our_data(ours_text)
    ranges, problems = {}, []
    for sec, (size, objs) in data.items():
        if not size:
            continue
        bases = {}
        for name, off, osize in objs:
            target = name if name in by_name else pair.get(name)
            if target is None or target not in by_name:
                problems.append(f"{sec} {name}: original not found (not named, not referenced from matching code)")
                continue
            osec, addr, _ = by_name[target]
            if osec != sec:
                problems.append(f"{sec} {name}: original {target} is in {osec}")
                continue
            bases.setdefault(addr - off, []).append(name)
        if len(bases) != 1:
            problems.append(f"{sec}: {'no' if not bases else 'inconsistent'} base "
                            + ", ".join(f"{b:#x}: {n[:3]}" for b, n in bases.items()))
            continue
        base = next(iter(bases))
        end = base + size
        # The original symbol may include the padding to the next object's
        # alignment (the linker adds it back); end the range where it ends.
        for a, n in by_addr.items():
            _, _, nsize = by_name[n]
            if by_name[n][0] == sec and a < end < a + nsize and a + nsize - end < 8:
                end = a + nsize
        ranges[sec] = (base, end)

    # A local object (a pooled constant, a static) that code outside the
    # slice also uses means the source file is wider than the slice: our
    # object keeps it local, so the outside reference cannot link.
    exported = {n for objs in data.values() for n, _, _ in objs[1]}
    users = outside_users(args.version, unit)
    for sec, (s, e) in ranges.items():
        inside = sorted((a, n) for a, n in by_addr.items() if s <= a < e and by_name[n][0] == sec)
        print(f"{sec:8s} start:0x{s:08X} end:0x{e:08X}  ({len(inside)} original symbols)")
        for a, n in inside:
            if n not in exported and n in users:
                problems.append(f"{sec} {n} is local in ours but also used outside the slice by "
                                + ", ".join(sorted(users[n])[:6]) + ": widen the unit to that file's functions")
    for p in problems:
        print("PROBLEM " + p)
    if problems:
        sys.exit(1)
    if args.write and ranges:
        write(args.version, unit, ranges)


def data_words(text):
    """{object name: [symbol or None per .4byte]} for objects holding relocations"""
    out, cur = {}, None
    for line in text.splitlines():
        s = line.strip()
        m = OBJNAME.match(s)
        if m:
            cur = m.group(1).strip('"')
            out[cur] = []
        elif s.startswith(".endobj"):
            if cur and not any(out[cur]):
                del out[cur]
            cur = None
        elif cur is not None and s.startswith(".4byte"):
            v = s.split(None, 1)[1].strip().strip('"')
            out[cur].append(None if re.match(r"0x[0-9A-Fa-f]+$", v) else v)
        elif cur is not None and s.startswith((".2byte", ".byte", ".float", ".double", ".skip", ".string", ".asciz")):
            out[cur].append(None)
    return out


def current_asm(version):
    base = ROOT / "build" / version / "asm"
    cfg = json.loads((ROOT / "build" / version / "config.json").read_text(encoding="utf-8"))
    for u in cfg["units"]:
        name = u["name"]
        f = base / ((name[:-4] if name.endswith((".cpp", ".c")) else name) + ".s")
        if f.exists():
            yield f


def rename_symbols(version, renames):
    p = ROOT / "config" / version / "symbols.txt"
    lines = p.read_text(encoding="utf-8").splitlines()
    for i, line in enumerate(lines):
        name = line.split(" = ", 1)[0]
        if name in renames:
            new = renames[name]
            line = new + " = " + line.split(" = ", 1)[1]
            if new.startswith("@LOCAL@") and "scope:" not in line:
                line += " scope:local"
            lines[i] = line
    p.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")
    print(f"renamed {len(renames)} symbol(s) in {p.relative_to(ROOT)}")


def outside_users(version, unit):
    """{symbol: {functions in other asm files that refer to it}}"""
    out = {}
    base = ROOT / "build" / version / "asm"
    # Only the current split's files: dtk leaves stale ones from older splits.
    cfg = json.loads((ROOT / "build" / version / "config.json").read_text(encoding="utf-8"))
    names = [u["name"] for u in cfg["units"] if u["name"] != unit and u["name"] != unit + ".cpp"]
    for name in names:
        f = base / ((name[:-4] if name.endswith((".cpp", ".c")) else name) + ".s")
        if not f.exists():
            continue
        fn = None
        for line in f.read_text(encoding="utf-8", errors="replace").splitlines():
            if line.startswith(".fn "):
                fn = line[4:].split(",")[0].strip()
            elif fn and "	" in line:
                for m in REF.finditer(line):
                    name = (m.group(1) or m.group(3) or "").strip('"')
                    if name:
                        out.setdefault(name, set()).add(fn)
    return out


def write(version, unit, ranges):
    p = ROOT / "config" / version / "splits.txt"
    lines = p.read_text(encoding="utf-8").splitlines()
    head = unit + ".cpp:"
    i = lines.index(head)
    j = i + 1
    while j < len(lines) and lines[j].startswith("\t"):
        j += 1
    have = {l.split()[0] for l in lines[i + 1:j]}
    new = [f"\t{sec:<11s} start:0x{s:08X} end:0x{e:08X}" for sec, (s, e) in ranges.items() if sec not in have]
    order = [".text", *SECTIONS]
    block = sorted(lines[i + 1:j] + new, key=lambda l: order.index(l.split()[0]) if l.split()[0] in order else 99)
    lines[i + 1:j] = block
    p.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")
    print(f"wrote {len(new)} range(s) to {p.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
