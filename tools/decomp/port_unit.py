#!/usr/bin/env python3
"""Write a unit from its finished sibling in another module.

The game's per-mode modules repeat whole files with the mode's name changed:
CEffectCtrlTournament / CEffectCtrlMenu / CEffectCtrlBoss / CEffectCtrlBasket
..., CItemBallBoss / CItemBallVolley ..., CBallManBoss / CBallManHockey ...
Once one of them has source, the others start from it.

  list                 untouched units with a sibling that has source, best first
  <unit>               show the sibling it would copy and the expected overlap
  <unit> --write       write the renamed .cpp (and any missing per-mode headers),
                       register it NonMatching in its module's objects.txt, build
                       it and print tools/decomp/diff.py's verdict per function

Renaming: the mode word (Tournament, Menu, Boss, Basket, Volley, Dodge, Hockey)
is replaced where it is part of a longer identifier or path (CEffectCtrlBoss,
CStgGmk_X_Boss_Act, game/msw/CItemBallBoss.hpp). Standalone words are left.
The result is a first draft: scores below 100% are the matcher's job, as usual
(field offsets, overrides and constants differ between modes more often than
code shape does).

Usage:
    python tools/decomp/port_unit.py list [--module BasketF] [--limit 40]
    python tools/decomp/port_unit.py game/basket/CEffectCtrlBasket [--from BossF/game/boss/CEffectCtrlBoss] [--write]
"""
import argparse
import json
import re
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import objects  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent.parent
MODES = {  # module -> (source directory, mode word)
    "TourF": ("tour", "Tournament"),
    "MenuF": ("menu", "Menu"),
    "BossF": ("boss", "Boss"),
    "BasketF": ("basket", "Basket"),
    "VolleyF": ("volley", "Volley"),
    "DodgeF": ("dodge", "Dodge"),
    "HockeyF": ("hockey", "Hockey"),
}
WORDS = "|".join(w for _, w in MODES.values())


def stem(name):
    """Mangled or unit name with length prefixes and mode words removed."""
    s = re.sub(r"\d+(?=[A-Za-z_])", "", name)
    return re.sub(WORDS, "<G>", s)


def rename(text, src_word, dst_word, src_dir, dst_dir):
    text = re.sub(r"(?<=[A-Za-z0-9_])" + src_word + r"(?=[A-Z_0-9]|\b)", dst_word, text)
    # A mode word at the start of an identifier is left alone (TournamentWork is a
    # shared DOL namespace), except the module entry point.
    text = re.sub(r"\b" + src_word + r"ModuleMain\b", dst_word + "ModuleMain", text)
    # include guards and other all-caps macros: CEFFECTCTRLTOURNAMENT_HPP_
    text = re.sub(r"\b[A-Z0-9_]*" + src_word.upper() + r"[A-Z0-9_]*\b",
                  lambda m: m.group(0).replace(src_word.upper(), dst_word.upper()), text)
    # the mode in prose ("the tournament map"); "menu" is too common a word
    if src_word != "Menu" and dst_word != "Menu":
        text = re.sub(r"\b" + src_word.lower() + r"\b", dst_word.lower(), text)
    text = text.replace(f"game/{src_dir}/", f"game/{dst_dir}/")
    return text


class Units:
    def __init__(self, version):
        self.version = version
        report = json.loads((ROOT / "build" / version / "report.json").read_text(encoding="utf-8"))
        self.units = {}
        for u in report["units"]:
            module, _, name = u["name"].partition("/")
            if module not in MODES:
                continue
            fns = [(f["name"], int(f["size"]), f.get("fuzzy_match_percent", 0.0)) for f in u.get("functions", [])]
            self.units[u["name"]] = {
                "module": module, "name": name, "fns": fns,
                "source": u.get("metadata", {}).get("source_path"),
            }
        self.by_stem = defaultdict(list)
        for key, u in self.units.items():
            self.by_stem[stem(u["name"].rsplit("/", 1)[-1])].append(key)

    def siblings(self, key):
        """Units with source in other modules, best first: (score bytes, key)."""
        t = self.units[key]
        want = {stem(n): s for n, s, _ in t["fns"]}
        out = []
        for k, u in self.units.items():
            if k == key or u["module"] == t["module"] or not u["source"]:
                continue
            done = {stem(n) for n, _, p in u["fns"] if p >= 100.0}
            score = sum(s for st, s in want.items() if st in done)
            if score:
                same_name = stem(u["name"].rsplit("/", 1)[-1]) == stem(t["name"].rsplit("/", 1)[-1])
                out.append((score + (1 << 30) * same_name, score, k))
        out.sort(reverse=True)
        return [(score, k) for _, score, k in out]


def cmd_list(us, args):
    rows = []
    for key, u in us.units.items():
        if u["source"] or (args.module and u["module"] != args.module):
            continue
        sib = us.siblings(key)
        if not sib:
            continue
        total = sum(s for _, s, _ in u["fns"])
        rows.append((sib[0][0], total, key, sib[0][1]))
    rows.sort(reverse=True)
    print(f"{'covered':>8s} {'unit':>8s}  target  <-  sibling")
    for covered, total, key, src in rows[:args.limit]:
        print(f"{covered:8d} {total:8d}  {key}  <-  {src}")
    print(f"\n{len(rows)} units, {sum(r[0] for r in rows)} bytes of functions whose sibling is matched")


def header_closure(path_text, src_word, dst_word, src_dir, dst_dir, written, log=print):
    """Write renamed copies of per-mode headers the renamed source includes but lacks."""
    for inc in re.findall(r'#include "([^"]+)"', path_text):
        if (ROOT / "include" / inc).exists() or inc in written:
            continue
        orig = rename(inc, dst_word, src_word, dst_dir, src_dir)
        src = ROOT / "include" / orig
        if orig == inc or not src.exists():
            log(f"  missing header {inc} (no sibling header {orig})")
            continue
        text = rename(src.read_text(encoding="utf-8"), src_word, dst_word, src_dir, dst_dir)
        guard_src = re.sub(r"[^A-Za-z0-9]", "_", orig).upper()
        guard_dst = re.sub(r"[^A-Za-z0-9]", "_", inc).upper()
        text = text.replace(guard_src, guard_dst)
        dst = ROOT / "include" / inc
        dst.parent.mkdir(parents=True, exist_ok=True)
        dst.write_text(text, encoding="utf-8", newline="\n")
        written.add(inc)
        log(f"  wrote include/{inc}  (from {orig})")
        header_closure(text, src_word, dst_word, src_dir, dst_dir, written, log)


def resolve(us, unit):
    """Unit key (Module/game/...) for a unit name or key."""
    if unit in us.units:
        return unit
    hits = [k for k in us.units if k.endswith("/" + unit) or k.split("/", 1)[1] == unit]
    if len(hits) != 1:
        sys.exit(f"unit {unit!r}: {len(hits)} matches {hits[:5]}")
    return hits[0]


def write_port(us, key, src_key, log=print):
    """Write the renamed source and missing per-mode headers and register the unit.

    Returns {"files": [paths written], "registered": bool, "extra": [...], "missing": [...]}
    so a caller can undo it.
    """
    t, s = us.units[key], us.units[src_key]
    src_dir, src_word = MODES[s["module"]]
    dst_dir, dst_word = MODES[t["module"]]
    text = (ROOT / s["source"]).read_text(encoding="utf-8")
    text = rename(text, src_word, dst_word, src_dir, dst_dir)
    text = re.sub(r"\b" + s["module"] + r"\b", t["module"], text)  # "the TourF module" in comments
    text = (f"// Written from {s['source']} by tools/decomp/port_unit.py "
            f"({src_word} -> {dst_word}); check against {t['module']}.\n") + text
    dst = ROOT / "src" / (t["name"] + ".cpp")
    dst.parent.mkdir(parents=True, exist_ok=True)
    dst.write_text(text, encoding="utf-8", newline="\n")
    log(f"  wrote {dst.relative_to(ROOT)}")
    headers = set()
    header_closure(text, src_word, dst_word, src_dir, dst_dir, headers)
    registered = objects.add(t["name"] + ".cpp", "NonMatching", t["module"])
    if registered:
        log(f"  registered {t['name']}.cpp NonMatching in config/{us.version}/{t['module']}/objects.txt")
    have = {stem(n) for n, _, _ in t["fns"]}
    # weak inlines of shared base classes (bcs::CUnivTask::GetFrameRateF) are kept
    # by whichever unit comes first; only the mode class's own functions matter
    extra = [n for n, _, _ in s["fns"] if stem(n) not in have and src_word in n]
    missing = [n for n, _, _ in t["fns"] if stem(n) not in {stem(m) for m, _, _ in s["fns"]}]
    return {"files": [dst] + [ROOT / "include" / h for h in sorted(headers)],
            "registered": registered, "extra": extra, "missing": missing}


def undo_port(result, unit_name):
    for f in result["files"]:
        if f.exists():
            f.unlink()
    if result["registered"]:
        objects.remove(unit_name + ".cpp")


def cmd_port(us, args):
    key = resolve(us, args.unit)
    t = us.units[key]
    sib = us.siblings(key)
    if args.source:
        src_key = args.source
        if src_key not in us.units or not us.units[src_key]["source"]:
            sys.exit(f"{src_key}: no such unit with source")
    elif sib:
        src_key = sib[0][1]
    else:
        sys.exit(f"{key}: no sibling with source in another module")
    s = us.units[src_key]
    total = sum(sz for _, sz, _ in t["fns"])
    print(f"{key}  ({len(t['fns'])} functions, {total} bytes)")
    print(f"  from {src_key}  ({s['source']})")
    for score, k in sib[:4]:
        print(f"    candidate {k}: {score} bytes covered")
    if t["source"] and not args.force:
        sys.exit(f"  {key} already has source {t['source']}; --force to overwrite")
    if not args.write:
        print("  (dry run; --write to create it)")
        return
    res = write_port(us, key, src_key)
    for n in res["extra"]:
        print(f"  CHECK: the sibling defines {n}; the target unit has no such function "
              f"(remove it and its declaration unless the target's vtable names it)")
    for n in res["missing"]:
        print(f"  TODO: {n} is not in the sibling; write it")
    if args.no_build:
        return
    py = sys.executable
    obj = f"build/{us.version}/src/{t['name']}.o"
    if subprocess.call([py, "tools/decomp/build.py", obj], cwd=ROOT):
        print("  compile failed: fix the source or headers, then build the object again")
        return
    subprocess.call([py, "tools/decomp/build.py", f"build/{us.version}/report.json"], cwd=ROOT,
                    stdout=subprocess.DEVNULL)
    subprocess.call([py, "tools/decomp/diff.py", t["name"], "--all"], cwd=ROOT)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("unit", help="'list' or a unit name")
    ap.add_argument("--from", dest="source", help="sibling unit key, e.g. BossF/game/boss/CEffectCtrlBoss")
    ap.add_argument("--module")
    ap.add_argument("--limit", type=int, default=40)
    ap.add_argument("--write", action="store_true")
    ap.add_argument("--force", action="store_true")
    ap.add_argument("--no-build", action="store_true")
    ap.add_argument("--version", default="RMKE01")
    args = ap.parse_args()
    us = Units(args.version)
    if args.unit == "list":
        cmd_list(us, args)
    else:
        cmd_port(us, args)


if __name__ == "__main__":
    main()
