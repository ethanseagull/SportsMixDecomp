#!/usr/bin/env python3
"""Commit some paths without touching the shared git index.

Parallel sessions share one checkout and so one `.git/index`; staging in it
let one session's files ride along in another's commit (notes/log.md,
2026-11-09). This commits exactly the given paths, as they are on disk, on top
of the current branch, through a private index:

    read-tree HEAD -> add the paths -> write-tree -> commit-tree -> update-ref

update-ref is given the old value, so a commit made by another session in the
meantime makes this fail instead of being overwritten (run it again).
Afterwards the shared index is reset for these paths only, so `git status`
does not show them as staged changes.

The full build must have passed after the last change to any of the paths:
the hash-check stamp build/<version>/ok has to be newer than every file
committed. --no-build-check skips that (notes and tools only).

Usage:
    python tools/decomp/commit.py -m "Match game/boss/CFoo.cpp" -- src/game/boss/CFoo.cpp include/... configure.py
"""
import argparse
import os
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent


def git(*args, env=None, input=None):
    r = subprocess.run(["git", *args], cwd=ROOT, env=env, input=input,
                       capture_output=True, text=True)
    if r.returncode:
        sys.exit(f"git {' '.join(args)} failed:\n{r.stderr.strip()}")
    return r.stdout.strip()


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("-m", "--message", required=True)
    ap.add_argument("--version", default="RMKE01")
    ap.add_argument("--no-build-check", action="store_true")
    ap.add_argument("paths", nargs="+")
    args = ap.parse_args()

    branch = git("symbolic-ref", "--short", "HEAD")
    if branch in ("main", "master"):
        sys.exit(f"refusing to commit on {branch}")
    for p in args.paths:
        if p.replace("\\", "/").startswith(("orig/", "build/")):
            sys.exit(f"{p}: orig/ and build/ are never committed")

    if not args.no_build_check:
        stamp = ROOT / "build" / args.version / "ok"
        if not stamp.exists():
            sys.exit("no hash-check stamp: run py -3.12 tools/decomp/build.py first")
        t = stamp.stat().st_mtime
        for p in args.paths:
            for f in ([ROOT / p] if (ROOT / p).is_file() else (ROOT / p).rglob("*")):
                if f.is_file() and f.stat().st_mtime > t:
                    sys.exit(f"{f.relative_to(ROOT)} changed after the last passing build; build again")

    old = git("rev-parse", "HEAD")
    fd, index = tempfile.mkstemp(prefix="decomp-index-")
    os.close(fd)
    os.unlink(index)
    env = dict(os.environ, GIT_INDEX_FILE=index)
    try:
        git("read-tree", "HEAD", env=env)
        git("add", "-A", "--", *args.paths, env=env)
        tree = git("write-tree", env=env)
    finally:
        if os.path.exists(index):
            os.unlink(index)
    # (not HEAD^{tree}: on Windows the git shim can eat the caret)
    if tree == git("log", "-1", "--format=%T", old):
        sys.exit("nothing to commit for these paths")
    new = git("commit-tree", tree, "-p", old, input=args.message + "\n")
    git("update-ref", "-m", "commit: " + args.message.splitlines()[0], f"refs/heads/{branch}", new, old)
    # Make the shared index agree with the new HEAD for these paths only.
    subprocess.run(["git", "reset", "-q", "HEAD", "--", *args.paths], cwd=ROOT, capture_output=True)
    print(git("log", "--oneline", "-1", new))
    print(git("show", "--stat", "--format=", new))


if __name__ == "__main__":
    main()
