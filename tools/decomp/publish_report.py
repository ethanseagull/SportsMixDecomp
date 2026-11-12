"""Copies the last build's objdiff report to progress/RMKE01/report.json, which CI
uploads to decmp.dev (the game files are not available on CI, so it cannot build).
Run it after a passing full build, before a push:

    py -3.12 tools/decomp/publish_report.py
"""
import pathlib
import shutil

ROOT = pathlib.Path(__file__).resolve().parents[2]
VERSION = "RMKE01"


def main():
    src = ROOT / "build" / VERSION / "report.json"
    dst = ROOT / "progress" / VERSION / "report.json"
    dst.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(src, dst)
    print(f"copied {src.relative_to(ROOT)} -> {dst.relative_to(ROOT)} ({dst.stat().st_size // 1024} KB)")


if __name__ == "__main__":
    main()
