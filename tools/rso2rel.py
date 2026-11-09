#!/usr/bin/env python3
"""Convert the game's RSO modules to REL modules for decomp-toolkit.

decomp-toolkit's project system only analyses and relinks REL modules. An RSO
holds the same things (sections, a bss size, relocations), with one difference:
relocations against other modules go through symbol names instead of
(module, section, offset). This tool resolves those names and rewrites each
module as a version 3 REL. Section contents and section numbering are kept, so
a relinked REL that matches the converted one byte for byte has the same code,
data and relocations as the original RSO.

Symbols that no module defines (never-linked virtuals) are pointed at the
module's own `_unresolved` function, which is what the RSO loader does.
"""
import argparse
import json
import struct
from pathlib import Path
from typing import Dict, List, Tuple

import rso as rsolib

R_DOLPHIN_NOP = 201
R_DOLPHIN_SECTION = 202
R_DOLPHIN_END = 203
REL_HEADER_SIZE = 0x4C

# Alignment of each section in the file (matches the `Sections:` headers in
# config/<version>/<Module>/splits.txt) and the module-wide alignment fields.
SECTION_ALIGN = {1: 4, 2: 4, 3: 4, 4: 8, 5: 8}
ALIGN = 8


def read_dol_symbols(symbols: Path, splits: Path) -> Dict[str, Tuple[int, int]]:
    """main.dol symbols: name -> (address, section number).

    Section numbers follow the linked ELF: the order of the `Sections:` header
    in splits.txt, counting from 1.
    """
    order = {}
    in_header = False
    for line in splits.read_text(encoding="utf-8").splitlines():
        if line.startswith("Sections:"):
            in_header = True
        elif in_header and line.startswith("	"):
            order[line.split()[0]] = len(order) + 1
        elif in_header:
            break
    out = {}
    for line in symbols.read_text(encoding="utf-8").splitlines():
        if " = " not in line:
            continue
        name, rest = line.split(" = ", 1)
        section, addr = rest.split(";")[0].split(":")
        if section in order:
            out[name] = (int(addr, 16), order[section])
    return out


def encode_relocs(relocs: List[Tuple[int, int, int, int, int]]) -> bytes:
    """relocs: (section, offset, type, target_section, addend), any order."""
    out = bytearray()
    cur_section = None
    pos = 0
    for section, offset, rtype, tsec, addend in sorted(relocs):
        if section != cur_section:
            out += struct.pack(">HBBI", 0, R_DOLPHIN_SECTION, section, 0)
            cur_section = section
            pos = 0
        delta = offset - pos
        while delta > 0xFFFF:
            out += struct.pack(">HBBI", 0xFFFF, R_DOLPHIN_NOP, 0, 0)
            delta -= 0xFFFF
        out += struct.pack(">HBBI", delta, rtype, tsec, addend)
        pos = offset
    out += struct.pack(">HBBI", 0, R_DOLPHIN_END, 0, 0)
    return bytes(out)


def convert(mods: Dict[str, rsolib.Rso], ids: Dict[str, int], dol: Dict[str, Tuple[int, int]]):
    exports = {}  # name -> (module id, section, offset)
    for name, mod in mods.items():
        for sym, section, offset in mod.exports:
            if not sym.startswith(".") and 0 < section < len(mod.sections) and mod.sections[section][1]:
                exports.setdefault(sym, (ids[name], section, offset))

    result = {}
    for name, mod in mods.items():
        mid = ids[name]
        prolog_sec, epilog_sec, unresolved_sec, _ = mod.section_ids
        by_module: Dict[int, list] = {}
        unresolved_names = set()
        unresolved_at = (unresolved_sec, mod.header["unresolved"])
        data = {i: bytearray(mod.data[off:off + size]) for i, (off, size) in enumerate(mod.sections) if off}

        def branch(sec: int, off: int, target: int) -> None:
            """Point the branch at `off` to `target` (same section), as the linker would."""
            (insn,) = struct.unpack_from(">I", data[sec], off)
            struct.pack_into(">I", data[sec], off, (insn & 0xFC000003) | ((target - off) & 0x03FFFFFC))

        def add(module: int, sec: int, off: int, rtype: int, tsec: int, addend: int) -> None:
            if rtype == rsolib.R_PPC_REL24:
                if module == mid and tsec == sec:
                    # Resolved at link time; a relinked module carries no relocation here.
                    branch(sec, off, addend)
                    return
                if sec == unresolved_at[0]:
                    # makerel parks calls that leave the module on _unresolved.
                    branch(sec, off, unresolved_at[1])
            by_module.setdefault(module, []).append((sec, off, rtype, tsec, addend))

        for r in mod.internal:
            sec, off = mod.locate(r.offset)
            add(mid, sec, off, r.type, r.target, r.addend)
        for r in mod.external:
            sec, off = mod.locate(r.offset)
            sym = mod.imports[r.target]
            if sym in dol:
                addr, dol_sec = dol[sym]
                add(0, sec, off, r.type, dol_sec, addr + r.addend)
            elif sym in exports:
                tmid, tsec, toff = exports[sym]
                add(tmid, sec, off, r.type, tsec, toff + r.addend)
            else:
                unresolved_names.add(sym)
                add(mid, sec, off, r.type, unresolved_at[0], unresolved_at[1] + r.addend)

        # Layout: header, section table, section data, import table, relocations.
        nsec = len(mod.sections)
        body = bytearray(REL_HEADER_SIZE + 8 * nsec)
        table = []
        for i, (off, size) in enumerate(mod.sections):
            if off == 0:
                table.append((0, size))
                continue
            align = SECTION_ALIGN.get(i, 8)
            while len(body) % align:
                body.append(0)
            start = len(body)
            body += data[i]
            table.append((start | (1 if i == prolog_sec else 0), size))
        while len(body) % 4:
            body.append(0)

        # Version 3 order: other modules, then this module, then main.dol.
        order = sorted(m for m in by_module if m not in (0, mid))
        if mid in by_module:
            order.append(mid)
        if 0 in by_module:
            order.append(0)
        imp_off = len(body)
        rel_off = imp_off + 8 * len(order)
        blobs = [encode_relocs(by_module[m]) for m in order]
        imp = bytearray()
        cursor = rel_off
        fix_size = rel_off
        for m, blob in zip(order, blobs):
            imp += struct.pack(">II", m, cursor)
            if m == mid:
                fix_size = cursor
            cursor += len(blob)
        if mid not in by_module and 0 in by_module:
            fix_size = struct.unpack_from(">I", imp, 8 * order.index(0) + 4)[0]
        body += imp
        for blob in blobs:
            body += blob

        struct.pack_into(
            ">12I4B6I", body, 0,
            mid, 0, 0, nsec, REL_HEADER_SIZE, 0, 0, 3, mod.header["bss_size"],
            rel_off, imp_off, len(imp),
            prolog_sec, epilog_sec, unresolved_sec, 0,
            mod.header["prolog"], mod.header["epilog"], mod.header["unresolved"],
            ALIGN, ALIGN, fix_size,
        )
        for i, (off, size) in enumerate(table):
            struct.pack_into(">II", body, REL_HEADER_SIZE + 8 * i, off, size)
        result[name] = (bytes(body), sorted(unresolved_names))
    return result


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--symbols", type=Path, required=True, help="main.dol symbols.txt")
    ap.add_argument("--splits", type=Path, required=True, help="main.dol splits.txt")
    ap.add_argument("--out", type=Path, required=True, help="output directory")
    ap.add_argument("--ids", type=Path, required=True, help="JSON file: module name -> REL id")
    ap.add_argument("modules", type=Path, nargs="+", help="decompressed RSO files")
    args = ap.parse_args()

    ids = json.loads(args.ids.read_text())
    dol = read_dol_symbols(args.symbols, args.splits)
    mods = {p.stem: rsolib.parse(p.read_bytes()) for p in args.modules}
    args.out.mkdir(parents=True, exist_ok=True)
    for name, (data, unresolved) in convert(mods, ids, dol).items():
        (args.out / f"{name}.rel").write_bytes(data)
        note = f", {len(unresolved)} unresolved imports" if unresolved else ""
        print(f"{name}: id {ids[name]}, {len(data)} bytes{note}")


if __name__ == "__main__":
    main()
