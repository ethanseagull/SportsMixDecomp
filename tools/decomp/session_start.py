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
    if worktree:
        main = run(sys.executable, "-c",
                   "import sys; sys.path.insert(0, 'tools/decomp'); import worktree as w; "
                   "print(w.main_branch(w.main_checkout()))")
        if main:
            behind = run("git", "rev-list", "--count", f"HEAD..{main}")
            ahead = run("git", "rev-list", "--count", f"{main}..HEAD")
            print(f"{main} has {behind or '?'} commit(s) this branch lacks; this branch has {ahead or '?'} not landed."
                  " Land after every committed unit: py -3.12 tools/decomp/worktree.py land")
            # Old tools in a worktree skip newer checks (73 near records bypassed the
            # near gate on 11-12..14 that way): say so before any matching starts.
            newer = run("git", "log", "--oneline", f"HEAD..{main}", "--", "tools/")
            if newer:
                print(f"WARNING: {main} has newer tools ({len(newer.splitlines())} commit(s) to tools/). "
                      f"Bring them in before matching: py -3.12 tools/decomp/worktree.py land (or sync from the main checkout).")
    overlap = run(sys.executable, "tools/decomp/worktree.py", "overlap", "--quiet-if-none")
    if overlap:
        print(overlap)
    out = run(sys.executable, "tools/decomp/next.py")
    lines = [l for l in out.splitlines() if l.strip()]
    if lines:
        print("\n".join(lines[:14]))
    else:
        print("(next.py printed nothing: it failed or took over 60 s; run py -3.12 tools/decomp/next.py by hand)")
    # The files every agent reads stay small enough to be read whole.
    budgets = {"CLAUDE.md": 10, "notes/toolchain.md": 14, "notes/idioms.md": 18,
               "notes/decisions.md": 10, "notes/log.md": 120}
    over = [f"{p} {(ROOT / p).stat().st_size // 1024} KB (budget {kb} KB)" for p, kb in budgets.items()
            if (ROOT / p).exists() and (ROOT / p).stat().st_size > kb * 1024]
    if over:
        print("Over budget, curate them (notes/README.md, 'Keeping notes small'): " + "; ".join(over))
    print("Read notes/README.md (what to read, and where findings go); tools are listed in CLAUDE.md.")


if __name__ == "__main__":
    try:
        main()
    except Exception as e:  # a hook must never block the session
        print(f"(session_start.py: {e})")
