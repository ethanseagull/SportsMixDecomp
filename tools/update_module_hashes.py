#!/usr/bin/env python3
"""Refresh module hashes in config.yml and build.sha1 after tools/setup_orig.py.

Only needed when tools/rso2rel.py changes how modules are converted.
"""
import argparse
import hashlib
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--version", default="RMKE01")
    args = ap.parse_args()
    v = args.version
    config = ROOT / "config" / v
    ids = json.loads((config / "module_ids.json").read_text())
    yml = (config / "config.yml").read_text().split("modules:")[0].rstrip("\n") + "\nmodules:\n"
    dol_hash = re.search(r"^hash: (\w+)", yml, re.M).group(1)
    sha = [f"{dol_hash}  build/{v}/main.dol"]
    for name in ids:
        h = hashlib.sha1((ROOT / "orig" / v / "files/Module/Final" / f"{name}.rel").read_bytes()).hexdigest()
        yml += (f"- object: files/Module/Final/{name}.rel\n  hash: {h}\n  name: {name}\n"
                f"  symbols: config/{v}/{name}/symbols.txt\n  splits: config/{v}/{name}/splits.txt\n")
        sha.append(f"{h}  build/{v}/{name}/{name}.rel")
    (config / "config.yml").write_text(yml, newline="\n")
    (config / "build.sha1").write_text("\n".join(sha) + "\n", newline="\n")


if __name__ == "__main__":
    main()
