#!/usr/bin/env python3
"""Index every function in the split assembly.

Reads build/<version>/**/asm/*.s (written by `ninja`) and records, for each
function: module, unit, size, a hash of its instructions with addresses and
symbol references masked out, and its mnemonic sequence.

Functions with the same hash are the same code linked into several places
(inlines, templates, classes shared by the sport modules): match one and the
source applies to all of them.

Usage:
    python tools/decomp/index.py            build the index
    python tools/decomp/index.py --dupes    also print the largest duplicate groups
"""
import argparse
import hashlib
import json
import re
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
INSN = re.compile(r"^/\* \S+ \S+  (?:[0-9A-F]{2} ){4}\*/\t(\S+)(?:\s+(.*))?$")
SYMREF = re.compile(r"[A-Za-z_.$@][\w.$@<>,*&:~-]*(?:@(?:ha|h|l|sda21))?")
REG = re.compile(r"^(?:r\d+|f\d+|cr\d|qr\d|lr|ctr|[0-9-]|0x)")


def normalise(operands: str) -> str:
    """Keep registers and immediates; replace symbol and label names."""
    out = []
    for part in re.split(r"([,() ]+)", operands):
        if part and not re.match(r"[,() ]+", part) and not REG.match(part):
            reloc = re.search(r"@(ha|h|l|sda21)$", part)
            part = "SYM" + (reloc.group(0) if reloc else "")
        out.append(part)
    return "".join(out)


def index_file(path: Path, module: str, unit: str, out: list) -> None:
    name = None
    ops, norm = [], []
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        if line.startswith(".fn "):
            rest = line[4:].strip()
            name = rest[1:rest.index('"', 1)] if rest.startswith('"') else rest.split(",")[0].strip()
            ops, norm = [], []
        elif line.startswith(".endfn") and name:
            if ops:
                digest = hashlib.sha1("\n".join(norm).encode()).hexdigest()[:16]
                out.append({"module": module, "unit": unit, "name": name,
                            "size": len(ops) * 4, "hash": digest, "ops": " ".join(ops)})
            name = None
        elif name:
            m = INSN.match(line)
            if m:
                ops.append(m.group(1))
                norm.append(m.group(1) + " " + normalise(m.group(2) or ""))


def build(version: str) -> list:
    base = ROOT / "build" / version
    out = []
    for asm_dir in sorted(base.glob("asm")) + sorted(base.glob("*/asm")):
        module = "main" if asm_dir.parent == base else asm_dir.parent.name
        for path in sorted(asm_dir.rglob("*.s")):
            unit = path.relative_to(asm_dir).with_suffix("").as_posix()
            index_file(path, module, unit, out)
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--version", default="RMKE01")
    ap.add_argument("--dupes", action="store_true", help="print the largest duplicate groups")
    args = ap.parse_args()

    funcs = build(args.version)
    dest = ROOT / "build" / "decomp"
    dest.mkdir(parents=True, exist_ok=True)
    (dest / f"index_{args.version}.json").write_text(json.dumps(funcs), encoding="utf-8")

    groups = defaultdict(list)
    for f in funcs:
        groups[f["hash"]].append(f)
    dup_funcs = sum(len(g) for g in groups.values() if len(g) > 1)
    saved = sum((len(g) - 1) * g[0]["size"] for g in groups.values() if len(g) > 1)
    total = sum(f["size"] for f in funcs)
    print(f"{len(funcs)} functions, {total} bytes of code")
    print(f"{len(groups)} distinct; {dup_funcs} functions have a duplicate elsewhere "
          f"({saved} bytes, {100 * saved / total:.1f}% of code, come free once one copy matches)")
    if args.dupes:
        big = sorted((g for g in groups.values() if len(g) > 1),
                     key=lambda g: -(len(g) - 1) * g[0]["size"])[:25]
        for g in big:
            where = ", ".join(sorted({f["module"] for f in g}))
            print(f"  {g[0]['size']:6d} bytes x{len(g):<3d} {g[0]['name'][:60]:60s} [{where}]")


if __name__ == "__main__":
    main()
