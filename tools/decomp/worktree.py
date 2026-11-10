#!/usr/bin/env python3
"""Give a parallel session a checkout of its own.

Sessions sharing one checkout share the git index, configure output, build/
and every header, and on 2026-11-09 they tripped over each other several times.
A git worktree gives each its own files, index, branch and build directory.
The untracked parts the rules live in are linked in, so the hook, skills and
notes work the same there:

  linked (shared with the main checkout): .claude/, notes/, orig/<version>/,
                                          build/compilers, build/tools
  copied: CLAUDE.md
  own:    everything tracked, build/RMKE01, the git index, branch wt/<name>

Usage (from the main checkout):
    python tools/decomp/worktree.py new <name>        create ../SportsMixDecomp-wt/<name> on branch wt/<name>
                                                       from this checkout's HEAD, then configure and build it
    python tools/decomp/worktree.py list
    python tools/decomp/worktree.py sync <name>       merge the main branch into the worktree (new headers etc.)
    python tools/decomp/worktree.py merge <name>      merge wt/<name> into this checkout's branch, then
                                                       full build + hash check; undone if either fails
    python tools/decomp/worktree.py remove <name>     delete the worktree and its branch (refuses if unmerged)

Then start the session in the worktree folder, e.g. `/match-run BasketF`.
"""
import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
BASE = ROOT.parent / (ROOT.name + "-wt")
BASE_LINKS = [".claude", "notes", "build/compilers", "build/tools"]


def links():
    """Paths linked into a worktree. orig/ itself is not linked because the repo
    tracks a placeholder in it (orig/RMKP01/.gitkeep); each version folder is."""
    out = list(BASE_LINKS)
    if (ROOT / "orig").is_dir():
        out += [f"orig/{d.name}" for d in sorted((ROOT / "orig").iterdir()) if d.is_dir()]
    return out
COPIES = ["CLAUDE.md"]


def run(*cmd, cwd=ROOT, check=True, quiet=False):
    r = subprocess.run(list(cmd), cwd=cwd, capture_output=True, text=True)
    if check and r.returncode:
        sys.exit(f"{' '.join(cmd)} failed:\n{r.stdout}{r.stderr}")
    if not quiet and r.stdout.strip():
        print(r.stdout.strip())
    return r


def is_link(p: Path) -> bool:
    try:
        return p.is_symlink() or bool(os.readlink(p))
    except OSError:
        return False


def link(src: Path, dst: Path):
    dst.parent.mkdir(parents=True, exist_ok=True)
    if dst.exists() or dst.is_symlink():
        if not is_link(dst):
            sys.exit(f"{dst} exists and is not a link; refusing to continue")
        return
    if os.name == "nt":
        # a directory junction needs no admin rights
        run("cmd", "/c", "mklink", "/J", str(dst), str(src), quiet=True)
    else:
        dst.symlink_to(src, target_is_directory=True)


def unlink_links(wt: Path):
    """Remove the links only (never what they point at) before deleting a worktree."""
    for rel in links():
        p = wt / rel
        if os.name == "nt" and p.exists() and is_link(p):
            run("cmd", "/c", "rmdir", str(p), check=False, quiet=True)  # removes the junction itself
        elif p.is_symlink():
            p.unlink()


def main_branch():
    return run("git", "symbolic-ref", "--short", "HEAD", quiet=True).stdout.strip()


def cmd_new(name, build):
    wt = BASE / name
    branch = f"wt/{name}"
    if wt.exists():
        sys.exit(f"{wt} exists")
    # (not rev-parse --show-toplevel: the git on PATH may print MSYS paths like /c/...)
    if not (ROOT / ".git").is_dir():
        sys.exit("run this from the main checkout, not from a worktree")
    BASE.mkdir(parents=True, exist_ok=True)
    run("git", "worktree", "add", "-b", branch, str(wt), "HEAD")
    for rel in links():
        dst = wt / rel
        if dst.is_dir() and not any(f.name != ".gitkeep" for f in dst.rglob("*")):
            for f in dst.rglob(".gitkeep"):
                f.unlink()
            dst.rmdir()  # only a tracked placeholder was there
        link(ROOT / rel, dst)
    for rel in COPIES:
        shutil.copy2(ROOT / rel, wt / rel)
    print(f"worktree {wt} on {branch}")
    if build:
        py = sys.executable
        subprocess.call([py, "configure.py"], cwd=wt, stdout=subprocess.DEVNULL)
        rc = subprocess.call([py, "tools/decomp/build.py"], cwd=wt, stdout=subprocess.DEVNULL)
        print("full build:", "passed" if rc == 0 else "FAILED (look before starting a session there)")
    print(f"start the session in {wt}")


def cmd_list():
    run("git", "worktree", "list")


def cmd_sync(name):
    wt = BASE / name
    r = run("git", "merge", "--no-edit", main_branch(), cwd=wt, check=False)
    if r.returncode:
        run("git", "merge", "--abort", cwd=wt, check=False)
        sys.exit("conflicts; merge aborted. Resolve by hand in the worktree:\n" + r.stdout + r.stderr)
    print(f"{name}: up to date with {main_branch()}")


def hashes_ok(cwd):
    py = sys.executable
    subprocess.call([py, "configure.py"], cwd=cwd, stdout=subprocess.DEVNULL)
    if subprocess.call([py, "tools/decomp/build.py"], cwd=cwd, stdout=subprocess.DEVNULL):
        return False
    dtk = cwd / "build" / "tools" / ("dtk.exe" if os.name == "nt" else "dtk")
    return subprocess.run([str(dtk), "shasum", "-c", "config/RMKE01/build.sha1"], cwd=cwd,
                          capture_output=True).returncode == 0


def cmd_merge(name):
    branch = f"wt/{name}"
    old = run("git", "rev-parse", "HEAD", quiet=True).stdout.strip()
    r = run("git", "merge", "--no-edit", branch, check=False)
    if r.returncode:
        run("git", "merge", "--abort", check=False)
        sys.exit("conflicts; merge aborted. Run `worktree.py sync` in the worktree, resolve there, and merge again.\n"
                 + r.stdout + r.stderr)
    if not hashes_ok(ROOT):
        # only our own merge is undone; the working tree's other changes stay
        run("git", "reset", "--merge", old)
        sys.exit(f"the merged tree fails the build or the hash check; merge undone (HEAD back at {old[:7]})")
    print(f"merged {branch}; full build and all hashes OK")


def cmd_remove(name):
    wt = BASE / name
    branch = f"wt/{name}"
    merged = run("git", "branch", "--merged", "HEAD", quiet=True).stdout.split()
    if branch not in merged:
        sys.exit(f"{branch} is not merged into {main_branch()}; merge it first (or delete by hand)")
    unlink_links(wt)
    left = [rel for rel in links() if is_link(wt / rel)]
    if left:
        # never let a recursive delete follow a link into orig/ or the shared toolchain
        sys.exit(f"could not remove the links {left} in {wt}; remove them by hand (rmdir, not rm -r), then retry")
    run("git", "worktree", "remove", "--force", str(wt))
    run("git", "branch", "-d", branch)
    print(f"removed {wt} and {branch}")


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("cmd", choices=["new", "list", "sync", "merge", "remove"])
    ap.add_argument("name", nargs="?")
    ap.add_argument("--no-build", action="store_true")
    args = ap.parse_args()
    if args.cmd != "list" and not args.name:
        sys.exit("give the worktree's name")
    {"new": lambda: cmd_new(args.name, not args.no_build), "list": cmd_list,
     "sync": lambda: cmd_sync(args.name), "merge": lambda: cmd_merge(args.name),
     "remove": lambda: cmd_remove(args.name)}[args.cmd]()


if __name__ == "__main__":
    main()
