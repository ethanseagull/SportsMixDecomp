#!/usr/bin/env python3
"""Find functions that read different data than the original although the code looks the same.

diff.py masks the names of anonymous data (@123, lbl_*), and objdiff's score has
the same blind spot, so a function that loads 0.0f where the original loads
5.0f (swapped arguments, a wrong constant, the wrong table) can show as 100% and
"match". diff.py now compares the values behind every data read; this runs that
check over every unit that has source and lists the functions it catches:

  caught      the instructions match (match/registers/schedule/stack) but a data
              read gets a different value: the function is wrong, whatever
              objdiff's score says
  also        already `real` for other reasons; the data difference is one more hint

Units marked Matching pass the build's hash check, so anything caught there is
a false positive of this check, reported separately so it can be fixed.

Usage:
    python tools/decomp/datacheck.py [--module M]     writes notes/data-audit.txt
Build first (py -3.12 tools/decomp/build.py): it reads build/RMKE01/report.json
and the compiled objects.
"""
import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import diff  # noqa: E402
import equivalent  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent.parent


def unit_status(version):
    """{(module, unit): Matching|Equivalent|NonMatching} from every objects.txt."""
    out = {}
    for p in sorted((ROOT / "config" / version).glob("*/objects.txt")):
        for line in p.read_text(encoding="utf-8").splitlines():
            parts = line.split()
            if len(parts) >= 2 and not line.startswith("#"):
                out[(p.parent.name, parts[1].removesuffix(".cpp").removesuffix(".c"))] = parts[0]
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--module")
    ap.add_argument("--version", default="RMKE01")
    args = ap.parse_args()
    units = diff.report_units(args.version)
    status = unit_status(args.version)
    accepted = equivalent.load(args.version)
    caught, also, errors = [], [], []
    todo = [(u, m) for u, (m, _, src) in units.items() if src and args.module in (None, m)]
    for n, (unit, module) in enumerate(sorted(todo), 1):
        if n % 50 == 0:
            print(f"  {n}/{len(todo)} units", file=sys.stderr)
        try:
            rows = diff.analyse(args.version, unit, module)
        except (SystemExit, Exception) as e:  # missing object or asm
            errors.append((module, unit, str(e).splitlines()[0] if str(e) else type(e).__name__))
            continue
        scores = units[unit][1]
        for fn, cls, why, a, b in rows:
            dh = [h for h in why if h.startswith("data values differ")]
            if not dh:
                continue
            old, _ = diff.classify(a, b)  # without the data check
            st = status.get((module, unit), "?")
            acc = accepted.get((module, fn), ("", 0, ""))[0]
            row = (st, module, unit, fn, scores.get(fn, 0.0), old, acc, dh[0])
            (caught if old != "real" else also).append(row)

    false_pos = [r for r in caught if r[0] == "Matching"]
    real = [r for r in caught if r[0] != "Matching"]
    lines = [f"# data audit: functions whose data reads differ from the original ({len(todo)} units checked)",
             "# caught = the instructions match, so objdiff and the old diff.py called them correct",
             ""]
    for title, rows in ((f"caught in Equivalent/NonMatching units (wrong code): {len(real)}", real),
                        (f"caught in Matching units (false positives of the check, please report): {len(false_pos)}", false_pos),
                        (f"already real for other reasons, data differs too: {len(also)}", also)):
        lines.append(f"## {title}")
        for st, module, unit, fn, score, old, acc, h in sorted(rows):
            tag = f" [{acc}]" if acc else ""
            lines.append(f"{score:5.1f}  {st:11s} {module} {unit} {fn}  (was {old}){tag}")
            lines.append(f"       {h}")
        lines.append("")
    if errors:
        lines.append(f"## units not checked: {len(errors)}")
        lines += [f"  {m} {u}: {e}" for m, u, e in errors]
    out = ROOT / "notes" / "data-audit.txt"
    out.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")
    print(f"caught (wrong code):        {len(real)}")
    print(f"caught in Matching units:   {len(false_pos)}  (false positives of the check)")
    print(f"real anyway, data differs:  {len(also)}")
    print(f"units not checked:          {len(errors)}")
    print(f"written to {out.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
