#!/usr/bin/env python3
"""Compare one unit's compiled functions with the original and say what kind of difference is left.

Reading two disassemblies side by side to decide "near" or "unverified" cost a
matcher several turns per function. This does the comparison and labels it:

  match         same instructions (names of anonymous or file-local data may differ)
  registers     same instructions once registers are renumbered by first use:
                only register allocation differs            -> near candidate
  schedule      same instructions in another order (registers ignored)
                                                            -> near candidate
  stack         same instructions in any order once registers and stack-slot
                offsets (locals at other r1 offsets, frame size) are ignored
                                                            -> near candidate
  real          instructions differ; hints say how (frame size, constants,
                calls, loads/stores, conversions, branches, constant addressing)
  missing       the function is not in the compiled object

Names of anonymous data (@123, lbl_*) are masked, so two loads of *different*
literals look the same to the comparison, and objdiff's score has the same
blind spot (ArtsOptFadeOut(0.0f, 5.0f) scores 100% against an original that
passes 5.0f, 0.0f). Every class except `real` and `missing` is therefore also
checked for data values: each literal or data object an instruction reads is
looked up in both disassemblies and its bytes compared. A function that loads a
different value is reported as `real` with a "data values differ" hint.

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
import decomp_common as dc  # noqa: E402  (tools/decomp/decomp_common.py)

ROOT = Path(__file__).resolve().parent.parent.parent
INSN = re.compile(r"^/\* .*?\*/\t(\S+)(?:\s+(.*))?$")
FIXED_REGS = {"r0", "r1", "r2", "r13", "f0"}
IMM_OPS = ("li", "lis", "addi", "cmpwi", "cmplwi", "subi", "ori")
NUM = re.compile(r"(?<![\w(])-?0x[0-9a-fA-F]+|(?<![\w(r])\b\d+\b(?!\()")


def report_units(version):
    report = dc.load_report(version)  # regenerated first if an object is newer
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


STACK_OFF = re.compile(r"-?(?:0x[0-9a-fA-F]+|\d+)\(r1\)")
STACK_ADDR = re.compile(r"^(r\d{1,2}), r1, -?(?:0x[0-9a-fA-F]+|\d+)$")


def no_stack(insns):
    """Mask stack-slot offsets: r1-relative loads/stores, local addresses (addi rX, r1, N)
    and the frame size, so locals placed in other slots compare equal."""
    out = []
    for mn, ops in insns:
        ops = STACK_OFF.sub("S(r1)", ops)
        if mn == "addi":
            ops = STACK_ADDR.sub(r"\1, r1, S", ops)
        out.append((mn, ops))
    return out


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


OBJ_START = re.compile(r'^\.obj\s+("[^"]*"|[^\s,]+)')
DATA_REF = re.compile(r'("[^"]*"|[A-Za-z_.$@][\w.$@<>,:\\-]*?)(\+0x[0-9A-Fa-f]+|\+\d+)?@(l|sda21|sda2)\b')
WIDTH = {"lfs": 4, "lfsu": 4, "lfsx": 4, "lfd": 8, "lfdu": 8, "lwz": 4, "lwzu": 4, "lhz": 2, "lha": 2,
         "lhzu": 2, "lhau": 2, "lbz": 1, "lbzu": 1, "psq_l": 8, "psq_lu": 8, "lmw": 4}
ADDRESS_OF = ("addi", "addic", "ori", "la")
ADDRESS_SPAN = 64  # bytes compared when an instruction takes a data object's address


def _int(tok):
    return int(tok, 0)


def _unescape(s):
    out = bytearray()
    i = 0
    simple = {"n": 10, "t": 9, "r": 13, "0": 0, "\\": 92, '"': 34, "'": 39, "a": 7, "b": 8, "f": 12, "v": 11}
    while i < len(s):
        c = s[i]
        if c == "\\" and i + 1 < len(s):
            n = s[i + 1]
            if n == "x" and re.match(r"[0-9A-Fa-f]{2}", s[i + 2:i + 4]):
                out.append(int(s[i + 2:i + 4], 16))
                i += 4
                continue
            if n in simple:
                out.append(simple[n])
                i += 2
                continue
        out += c.encode("utf-8", errors="replace")
        i += 1
    return bytes(out)


def parse_data(text):
    """{symbol: (bytes, {offset: pointer target})} for every .obj ... .endobj block.

    Pointers (.4byte sym, .rel sym) are left as zeros in the bytes and listed by
    offset, so a table of function pointers compares by (masked) target name."""
    import struct
    out, name, buf, ptrs = {}, None, None, None
    for line in text.splitlines():
        m = OBJ_START.match(line)
        if m:
            name, buf, ptrs = m.group(1).strip('"'), bytearray(), {}
            continue
        if name is None:
            continue
        s = line.strip()
        if s.startswith(".endobj"):
            out[name] = (bytes(buf), ptrs)
            name = None
            continue
        d, _, arg = s.partition(" ")
        arg = arg.strip()
        if not arg.startswith('"'):
            arg = arg.split(" #", 1)[0].strip()  # dtk comments, e.g. ".4byte 0xFFFFFFFF # NaN"
        try:
            if d in (".4byte", ".2byte", ".byte"):
                width = {".4byte": 4, ".2byte": 2, ".byte": 1}[d]
                for tok in ([t.strip() for t in arg.split(",")] if d == ".byte" else [arg]):
                    try:
                        buf += (_int(tok) & ((1 << (8 * width)) - 1)).to_bytes(width, "big")
                    except ValueError:  # a symbol: a pointer (quotes kept, mask_symbols needs them)
                        ptrs[len(buf)] = tok
                        buf += bytes(width)
            elif d == ".rel":
                ptrs[len(buf)] = arg.split(",")[0].strip()
                buf += bytes(4)
            elif d == ".float":
                buf += struct.pack(">f", float(arg))
            elif d == ".double":
                buf += struct.pack(">d", float(arg))
            elif d == ".skip":
                buf += bytes(_int(arg))
            elif d in (".string", ".ascii", ".string16"):
                body = _unescape(arg[1:-1]) if arg.startswith('"') else b""
                if d == ".string16":
                    buf += body.decode("utf-8", errors="replace").encode("utf-16-be") + b"\0\0"
                else:
                    buf += body + (b"\0" if d == ".string" else b"")
        except (ValueError, OverflowError):
            pass  # unparsed directive: the object keeps what was read so far
    return out


def _value_at(data, sym, off, width):
    """(bytes, ((rel offset, masked pointer), ...)) at sym+off, or None if unknown."""
    if sym not in data:
        return None
    raw, ptrs = data[sym]
    if width is None:
        width = max(0, min(ADDRESS_SPAN, len(raw) - off))
    if off + width > len(raw):
        return None
    # A pointer to anonymous data compares as ANON whatever its offset: the original
    # splits a pool into one label per object, our object addresses one @N + offset.
    p = tuple(sorted((o - off, re.sub(r"ANON(\+0x[0-9A-Fa-f]+|\+\d+)?", "ANON", mask_symbols(t)))
                     for o, t in ptrs.items() if off <= o < off + width))
    return raw[off:off + width], p


def _show(mn, val):
    import struct
    raw, ptrs = val
    if ptrs:
        return "pointer(s) to " + ", ".join(t for _, t in ptrs)
    if mn.startswith("lfs") and len(raw) == 4:
        return repr(struct.unpack(">f", raw)[0])
    if mn.startswith("lfd") and len(raw) == 8:
        return repr(struct.unpack(">d", raw)[0])
    if len(raw) > 16:
        return raw[:16].hex() + "..."
    return "0x" + raw.hex() if raw else "(empty)"


def data_reads(insns, data):
    """[(index, mnemonic, value)] for every instruction that reads data or takes a
    data object's address; value is None where the object can't be resolved."""
    out = []
    for i, (mn, ops) in enumerate(insns):
        width = WIDTH.get(mn)
        if width is None and mn not in ADDRESS_OF:
            continue
        for m in DATA_REF.finditer(ops):
            sym = m.group(1).strip('"')
            off = int(m.group(2)[1:], 0) if m.group(2) else 0
            out.append((i, mn, _value_at(data, sym, off, width)))
    return out


def _common(reads_a, reads_b):
    """Address-of reads see a whole object, whose size can differ by alignment
    padding alone (a vtable of 0xC bytes in ours, 0x10 in the original): compare
    only the bytes both have."""
    out_a, out_b = [], []
    for (i, mn, va), (j, mn_b, vb) in zip(reads_a, reads_b):
        if mn in ADDRESS_OF and va is not None and vb is not None:
            n = min(len(va[0]), len(vb[0]))
            va = (va[0][:n], tuple(p for p in va[1] if p[0] < n))
            vb = (vb[0][:n], tuple(p for p in vb[1] if p[0] < n))
        out_a.append((i, mn, va))
        out_b.append((j, mn_b, vb))
    return out_a, out_b


# Pointer targets that can't be checked by name: functions the original has no
# name for (fn_8003B624) and cross-module references dtk leaves unresolved.
UNNAMED_TARGET = re.compile(r"^(?:fn_[0-9A-Fa-f]{6,8}|_unresolved)$")


_CODE = None


def _code_hashes():
    """{function name: set of code hashes} from the function index, so two names
    for the same code compare equal: the DOL linker folds identical functions,
    and the original's vtable then names the one copy it kept (CStgMdl's
    IsSceneEntry slot names nw4r WsdPlayer::GetVoiceOutCount, both li r3, 0; blr)."""
    global _CODE
    if _CODE is None:
        _CODE = {}
        for f in dc.load_index():
            # the index hash masks symbol names, so only functions that refer to
            # none (no call, branch or address) are known to be the same bytes
            if not set(f["ops"].split()) & {"bl", "b", "lis", "bctrl"}:
                _CODE.setdefault(f["name"], set()).add(f["hash"])
    return _CODE


def _same_ptr(ta, tb):
    """ta: pointer target in the original, tb: in ours (masked names)."""
    if ta == tb:
        return True
    if ta is None or tb is None:
        return False
    if UNNAMED_TARGET.match(ta) or UNNAMED_TARGET.match(tb):
        return True
    if ta == "ANON" and not tb.startswith("ANON"):
        return True  # the original has no name for this data (lbl_*), ours gave it one
    code = _code_hashes()
    return bool(code.get(ta, set()) & code.get(tb, set()))


def same_value(va, vb):
    """Equal bytes, and pointers equal by masked name (an unnamed target matches any)."""
    (ra, pa), (rb, pb) = va, vb
    if ra != rb:
        return False
    pa, pb = dict(pa), dict(pb)
    return all(_same_ptr(pa.get(o), pb.get(o)) for o in set(pa) | set(pb))


def _first_difference(va, vb):
    """'at +0xN: X vs Y' for the first byte or pointer where two object reads differ."""
    (ra, pa), (rb, pb) = va, vb
    pa, pb = dict(pa), dict(pb)
    for off in range(0, max(len(ra), len(rb)), 4):
        ta, tb = pa.get(off), pb.get(off)
        if not _same_ptr(ta, tb) or ra[off:off + 4] != rb[off:off + 4]:
            def one(raw, t):
                return t if t else "0x" + raw[off:off + 4].hex()
            return f"at +0x{off:X}: {one(ra, ta)} in the original, {one(rb, tb)} in ours"
    return "same bytes, different pointers"


def data_hint(orig, ours, data_a, data_b, aligned):
    """A hint when a data read gets a different value in ours, else None."""
    ra, rb = data_reads(orig, data_a), data_reads(ours, data_b)
    if len(ra) == len(rb):
        ra, rb = _common(ra, rb)
    if aligned and len(ra) == len(rb):
        for (i, mn, va), (_, _, vb) in zip(ra, rb):
            if va is not None and vb is not None and not same_value(va, vb):
                if mn in ADDRESS_OF:
                    return (f"data values differ: instruction {i + 1} ({mn}) takes the address of an object whose "
                            f"contents differ {_first_difference(va, vb)} (a table entry, string or initialiser)")
                return (f"data values differ: instruction {i + 1} ({mn}) reads {_show(mn, va)} in the original, "
                        f"{_show(mn, vb)} in ours (wrong constant, swapped arguments or another object)")
        return None
    # Out of order: pair by value, loads only (an address-of read's span depends
    # on which object it is paired with, and the pairing is unknown here). Only
    # when every load resolves on both sides: a literal defined in another unit
    # is unknown in one disassembly and would unbalance the counts.
    la = [(mn, v) for _, mn, v in ra if mn not in ADDRESS_OF]
    lb = [(mn, v) for _, mn, v in rb if mn not in ADDRESS_OF]
    if len(la) != len(lb) or any(v is None for _, v in la + lb):
        return None
    ca, cb = Counter(la), Counter(lb)
    if ca != cb:
        only_a = [f"{mn} {_show(mn, v)}" for (mn, v) in (ca - cb)]
        only_b = [f"{mn} {_show(mn, v)}" for (mn, v) in (cb - ca)]
        return (f"data values differ: the original reads {', '.join(only_a[:4]) or '-'}, "
                f"ours {', '.join(only_b[:4]) or '-'}")
    return None


def classify(orig, ours, data_a=None, data_b=None):
    """data_a/data_b: parse_data() of the original and of our disassembly. Without
    them the data-value check is skipped (the old behaviour)."""
    a = [(m, mask_symbols(o)) for m, o in orig]
    b = [(m, mask_symbols(o)) for m, o in ours]
    if a == b:
        cls = "match"
    elif canon_regs(a) == canon_regs(b):
        cls = "registers"
    elif Counter(no_regs(a)) == Counter(no_regs(b)):
        cls = "schedule"
    elif Counter(no_regs(no_stack(a))) == Counter(no_regs(no_stack(b))):
        cls = "stack"
    else:
        h = hints(a, b)
        if data_a is not None:
            dh = data_hint(orig, ours, data_a, data_b, aligned=False)
            if dh:
                h.append(dh)
        return "real", h
    if data_a is not None:
        dh = data_hint(orig, ours, data_a, data_b, aligned=cls in ("match", "registers"))
        if dh:
            return "real", [dh]
    return cls, []


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
    orig_text = (ROOT / "build" / version / ("" if module == "main" else module) / "asm" / (unit + ".s")).read_text(encoding="utf-8", errors="replace")
    ours_text = disasm_ours(version, unit)
    orig, ours = parse(orig_text), parse(ours_text)
    data_a, data_b = parse_data(orig_text), parse_data(ours_text)
    rows = []
    for fn, insns in orig.items():
        if wanted is not None and fn not in wanted:
            continue
        if fn not in ours:
            rows.append((fn, "missing", ["not in the compiled object (named differently, or not written)"], insns, []))
            continue
        cls, why = classify(insns, ours[fn], data_a, data_b)
        rows.append((fn, cls, why, insns, ours[fn]))
    return rows


def verdict(cls, score):
    if cls in ("registers", "schedule", "stack"):
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
