#!/usr/bin/env python3
"""Unpack the files the build needs from your own disc image.

Usage:
    python tools/setup_orig.py [--version RMKE01] [path/to/disc.wbfs]

Writes, under orig/<version>/ (git-ignored):
    sys/main.dol
    files/Module/Final/MSW.sel
    files/Module/Final/<Module>.rso   the RSO modules, LH-decompressed
    files/Module/Final/<Module>.rel   the same modules converted to REL

With no disc path, it uses the first image found in orig/<version>/disc/.
Also seeds config/<version>/<Module>/symbols.txt from each module's export
table the first time it runs.
"""
import argparse
import hashlib
import json
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lh
import rso as rsolib
import rso2rel

ROOT = Path(__file__).resolve().parent.parent
MODULE_DIR = "files/Module/Final"
DISC_EXTS = {".iso", ".wbfs", ".rvz", ".wia", ".ciso", ".gcz", ".nfs", ".tgc"}
SECTION_NAMES = {1: ".text", 2: ".ctors", 3: ".dtors", 4: ".rodata", 5: ".data", 6: ".bss"}


def sha1(path: Path) -> str:
    return hashlib.sha1(path.read_bytes()).hexdigest()


def seed_symbols(mod: rsolib.Rso, path: Path) -> None:
    lines = []
    for name, section, offset in sorted(mod.exports, key=lambda e: (e[1], e[2], e[0])):
        if name.startswith(".") or section not in SECTION_NAMES or not mod.sections[section][1]:
            continue
        if name in ("_prolog", "_epilog", "_unresolved") or name.startswith(("_ctors", "_dtors")):
            continue
        kind = "function" if section == 1 else "object"
        lines.append(f"{name} = {SECTION_NAMES[section]}:0x{offset:08X}; // type:{kind} scope:global\n")
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("".join(lines), encoding="utf-8", newline="\n")


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--version", default="RMKE01")
    ap.add_argument("--dtk", type=Path, default=ROOT / "build" / "tools" / "dtk.exe")
    ap.add_argument("disc", type=Path, nargs="?")
    args = ap.parse_args()

    orig = ROOT / "orig" / args.version
    config = ROOT / "config" / args.version
    disc = args.disc
    if disc is None:
        found = sorted(p for p in (orig / "disc").glob("*") if p.suffix.lower() in DISC_EXTS)
        if not found:
            sys.exit(f"No disc image given and none found in {orig / 'disc'}")
        disc = found[0]
    if not args.dtk.exists():
        # Same pinned version the build uses (config.dtk_tag in configure.py).
        tag = re.search(r'^config\.dtk_tag = "([^"]+)"', (ROOT / "configure.py").read_text(), re.M).group(1)
        args.dtk.parent.mkdir(parents=True, exist_ok=True)
        subprocess.run([sys.executable, str(ROOT / "tools" / "download_tool.py"), "dtk",
                        str(args.dtk), "--tag", tag], check=True)

    ids = json.loads((config / "module_ids.json").read_text())
    out_dir = orig / MODULE_DIR
    out_dir.mkdir(parents=True, exist_ok=True)
    (orig / "sys").mkdir(parents=True, exist_ok=True)

    def copy(inner: str, dest: Path) -> None:
        subprocess.run([str(args.dtk), "vfs", "cp", "-q", f"{disc}:{inner}", str(dest)], check=True)

    copy("sys/main.dol", orig / "sys")
    copy(f"{MODULE_DIR}/MSW.sel", out_dir)
    tmp = ROOT / "build" / "setup_orig"
    tmp.mkdir(parents=True, exist_ok=True)
    mods = {}
    for name in ids:
        copy(f"{MODULE_DIR}/{name}.cmp", tmp)
        data = lh.decompress((tmp / f"{name}.cmp").read_bytes())
        (out_dir / f"{name}.rso").write_bytes(data)
        mods[name] = rsolib.parse(data)

    dol = rso2rel.read_dol_symbols(config / "symbols.txt", config / "splits.txt")
    for name, (data, unresolved) in rso2rel.convert(mods, ids, dol).items():
        (out_dir / f"{name}.rel").write_bytes(data)
        sym_path = config / name / "symbols.txt"
        if not sym_path.exists():
            seed_symbols(mods[name], sym_path)
        splits = config / name / "splits.txt"
        if not splits.exists():
            splits.write_text("", encoding="utf-8")
        print(f"{name}: rso {sha1(out_dir / (name + '.rso'))}  rel {sha1(out_dir / (name + '.rel'))}")
    print(f"main.dol: {sha1(orig / 'sys' / 'main.dol')}")


if __name__ == "__main__":
    main()
