"""Reader for Wii RSO modules and the static symbol file (.sel).

Mario Sports Mix keeps its per-sport code in RSO modules. decomp-toolkit only
understands REL modules, so tools/rso2rel.py uses this reader to convert them.
"""
import struct
from dataclasses import dataclass, field
from typing import Dict, List, Tuple

HEADER_SIZE = 0x58
_HDR = (
    "next prev num_sections section_info name_off name_size version bss_size "
    "section_ids prolog epilog unresolved int_rel_off int_rel_size ext_rel_off "
    "ext_rel_size exp_off exp_size exp_names imp_off imp_size imp_names"
).split()

R_PPC_NONE = 0
R_PPC_ADDR32 = 1
R_PPC_ADDR24 = 2
R_PPC_ADDR16 = 3
R_PPC_ADDR16_LO = 4
R_PPC_ADDR16_HI = 5
R_PPC_ADDR16_HA = 6
R_PPC_REL24 = 10
R_PPC_REL14 = 11


def _cstr(data: bytes, off: int) -> str:
    return data[off:data.index(b"\0", off)].decode("ascii")


@dataclass
class Reloc:
    offset: int   # file offset of the patched location
    type: int
    target: int   # section index (internal) or import index (external)
    addend: int


@dataclass
class Rso:
    data: bytes
    header: Dict[str, int]
    name: str
    sections: List[Tuple[int, int]]            # (file offset, size); offset 0 = bss/empty
    exports: List[Tuple[str, int, int]]        # (name, section, offset)
    imports: List[str]
    internal: List[Reloc] = field(default_factory=list)
    external: List[Reloc] = field(default_factory=list)

    @property
    def section_ids(self) -> Tuple[int, int, int, int]:
        v = self.header["section_ids"]
        return (v >> 24) & 0xFF, (v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF

    def locate(self, file_off: int) -> Tuple[int, int]:
        """Map a file offset to (section index, offset inside the section)."""
        for i, (off, size) in enumerate(self.sections):
            if off and off <= file_off < off + size:
                return i, file_off - off
        raise ValueError(f"offset {file_off:#x} is outside every section")


def parse(data: bytes) -> Rso:
    h = dict(zip(_HDR, struct.unpack(">22I", data[:HEADER_SIZE])))
    sections = []
    for i in range(h["num_sections"]):
        off, size = struct.unpack_from(">II", data, h["section_info"] + 8 * i)
        sections.append((off & ~1, size))
    exports = []
    for i in range(0, h["exp_size"], 16):
        name_off, value, section, _hash = struct.unpack_from(">IIII", data, h["exp_off"] + i)
        exports.append((_cstr(data, h["exp_names"] + name_off), section, value))
    imports = []
    for i in range(0, h["imp_size"], 12):
        (name_off,) = struct.unpack_from(">I", data, h["imp_off"] + i)
        imports.append(_cstr(data, h["imp_names"] + name_off))
    name = data[h["name_off"]:h["name_off"] + h["name_size"]].rstrip(b"\0").decode("ascii")
    rso = Rso(data, h, name, sections, exports, imports)
    for table, off, size in (
        (rso.internal, h["int_rel_off"], h["int_rel_size"]),
        (rso.external, h["ext_rel_off"], h["ext_rel_size"]),
    ):
        for i in range(0, size, 12):
            where, info, addend = struct.unpack_from(">III", data, off + i)
            table.append(Reloc(where, info & 0xFF, info >> 8, addend))
    return rso


def sel_symbols(data: bytes) -> Dict[str, int]:
    """Exported symbols of the static module (main.dol): name -> address."""
    sel = parse(data)
    out = {}
    for name, section, value in sel.exports:
        base = sel.sections[section][0] if 0 < section < len(sel.sections) else 0
        out[name] = base + value
    return out
