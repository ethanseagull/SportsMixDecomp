"""Decoder for Nintendo's "LH" compression (type byte 0x40): LZ77 + Huffman.

Written from a description of the format (see docs/formats/lh.md), not from
any Nintendo or third-party decoder source.

Layout:
  u32 LE   low byte 0x40, upper 24 bits = decompressed size (0 => a second
           u32 LE holds the size)
  tree A   literal/length tree, 9-bit entries
  tree B   offset-length tree, entry width found by probing (see below)
  stream   Huffman codes, read MSB-first byte by byte

Each tree starts with a size field (1 byte if entries are <= 8 bits, else a
u16 LE) giving the tree's total length in 32-bit words minus one, counting
the size field. Entries follow MSB-first, packed. The size field takes the
place of entry 0; the root is entry 1. An entry holds a child-pair offset in
its low (w-2) bits, and two flags: bit w-1 = left child is a leaf, bit w-2 =
right child is a leaf. The children of entry i sit at (i & ~1) + 2*off + 2.
A leaf slot holds its symbol directly.

Stream symbols from tree A: < 0x100 is a literal byte; otherwise a match of
(sym - 0x100 + 3) bytes. Its distance comes from tree B: symbol n is a bit
count; value = n ? (1 << (n-1)) | next (n-1) bits : 0; distance = value + 1.
"""
import sys


class Bits:
    """MSB-first bit reader over a bytes object."""

    def __init__(self, data, pos):
        self.data = data
        self.pos = pos      # byte index of the next unread byte
        self.acc = 0
        self.n = 0

    def read(self, count):
        while self.n < count:
            self.acc = (self.acc << 8) | self.data[self.pos]
            self.pos += 1
            self.n += 8
        self.n -= count
        value = (self.acc >> self.n) & ((1 << count) - 1)
        self.acc &= (1 << self.n) - 1
        return value


def read_tree(data, pos, width):
    """Read one serialized tree at byte `pos`. Returns (entries, next_pos)."""
    if width <= 8:
        words = data[pos]
        field_bits = 8
    else:
        words = data[pos] | data[pos + 1] << 8
        field_bits = 16
    total = (words + 1) * 4
    bits = Bits(data, pos)
    bits.read(field_bits)
    count = (total * 8 - field_bits) // width
    entries = [0] + [bits.read(width) for _ in range(count)]
    return entries, pos + total


def walk(entries, width, bits):
    """Decode one symbol: walk from the root (entry 1) to a leaf."""
    left_leaf = 1 << (width - 1)
    right_leaf = 1 << (width - 2)
    off_mask = right_leaf - 1
    i = 1
    while True:
        node = entries[i]
        child = (i & ~1) + 2 * (node & off_mask) + 2
        if bits.read(1):
            if node & right_leaf:
                return entries[child + 1]
            i = child + 1
        else:
            if node & left_leaf:
                return entries[child]
            i = child


def header(data):
    """Return (decompressed size, header length), or None if not LH."""
    if len(data) < 4 or data[0] != 0x40:
        return None
    size = data[1] | data[2] << 8 | data[3] << 16
    if size:
        return size, 4
    return int.from_bytes(data[4:8], 'little'), 8


def decompress(data, offset_width=None):
    """Decompress an LH buffer. Raises ValueError on bad data.

    The offset tree's entry width isn't stored in the file; when not given,
    try the plausible widths and keep the one that decodes cleanly."""
    h = header(data)
    if h is None:
        raise ValueError('not LH (first byte is not 0x40)')
    size, pos = h
    lit, pos = read_tree(data, pos, 9)
    widths = [offset_width] if offset_width else [5, 4, 6, 8]
    last_err = None
    for w in widths:
        try:
            return _decode(data, pos, size, lit, w)
        except (ValueError, IndexError) as e:
            last_err = e
    raise ValueError(f'LH decode failed: {last_err}')


def _decode(data, pos, size, lit, ow):
    offs, pos = read_tree(data, pos, ow)
    bits = Bits(data, pos)
    out = bytearray()
    while len(out) < size:
        sym = walk(lit, 9, bits)
        if sym < 0x100:
            out.append(sym)
            continue
        length = sym - 0x100 + 3
        n = walk(offs, ow, bits)
        value = ((1 << (n - 1)) | bits.read(n - 1)) if n else 0
        dist = value + 1
        if dist > len(out):
            raise ValueError(f'distance {dist} before start at {len(out)}')
        start = len(out) - dist
        for k in range(length):
            out.append(out[start + k])
    if len(out) != size:
        raise ValueError(f'overran: {len(out)} > {size}')
    if bits.pos > len(data):
        raise ValueError('read past end of input')
    return bytes(out)


if __name__ == '__main__':
    src = open(sys.argv[1], 'rb').read()
    out = decompress(src)
    if len(sys.argv) > 2:
        open(sys.argv[2], 'wb').write(out)
    print(f'{sys.argv[1]}: {len(src)} -> {len(out)} bytes')
