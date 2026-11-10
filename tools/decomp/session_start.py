#!/usr/bin/env python3
"""SessionStart hook: put the checkout's real state in front of a new session.

Prints the branch, whether this is a worktree, unmerged worktree branches, and
next.py's summary and next action. Facts come from the checkout, not from
memory or an old log. Must stay fast and never fail the session.
"""
import subprocess
import sys
from pathlib import Path

ROOT = Path.cwd()


def run(*cmd, timeout=60):
    try:
        r = subprocess.run(list(cmd), cwd=ROOT, capture_output=True, text=True, timeout=timeout)
        return r.stdout.strip()
    except (OSError, subprocess.TimeoutExpired):
        return ""


def main():
    if not (ROOT / "tools" / "decomp" / "next.py").exists():
        return
    branch = run("git", "symbolic-ref", "--short", "HEAD")
    top = run("git", "rev-parse", "--git-common-dir")
    worktree = top and not top.rstrip("/\\").endswith(".git") or (ROOT / ".git").is_file()
    print(f"[decomp state at session start] branch {branch}" + (" (a worktree; merge back with tools/decomp/worktree.py)" if worktree else ""))
    wts = [l for l in run("git", "branch", "--list", "wt/*", "--no-merged", "HEAD").splitlines() if l.strip()]
    if wts and not worktree:
        print("unmerged worktree branches: " + ", ".join(w.strip(" *+") for w in wts))
    out = run(sys.executable, "tools/decomp/next.py")
    lines = [l for l in out.splitlines() if l.strip()]
    print("\n".join(lines[:14]))
    print("Read notes/decisions.md and notes/autonomy.md before a run; tools are listed in CLAUDE.md.")


if __name__ == "__main__":
    try:
        main()
    except Exception as e:  # a hook must never block the session
        print(f"(session_start.py: {e})")
