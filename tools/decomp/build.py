#!/usr/bin/env python3
"""Run ninja with a lock, so two sessions in one checkout never build at once.

Usage:
    python tools/decomp/build.py [ninja arguments]

With no arguments this is the full build and hash check. A second caller
waits until the first has finished.
"""
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
LOCK = ROOT / "build" / "decomp" / "build.lock"


def main():
    LOCK.parent.mkdir(parents=True, exist_ok=True)
    with open(LOCK, "a+") as fh:
        waited = False
        while True:
            try:
                if sys.platform == "win32":
                    import msvcrt
                    fh.seek(0)
                    msvcrt.locking(fh.fileno(), msvcrt.LK_NBLCK, 1)
                else:
                    import fcntl
                    fcntl.flock(fh, fcntl.LOCK_EX | fcntl.LOCK_NB)
                break
            except OSError:
                if not waited:
                    print("another build is running; waiting", file=sys.stderr)
                    waited = True
                time.sleep(1)
        return subprocess.call([sys.executable, "-m", "ninja", *sys.argv[1:]], cwd=ROOT)


if __name__ == "__main__":
    sys.exit(main())
