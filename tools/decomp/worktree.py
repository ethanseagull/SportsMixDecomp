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
    python tools/decomp/worktree.py overlap           shared files changed on two or more unlanded branches

From inside a worktree (the session itself, after every committed unit):
    python tools/decomp/worktree.py land              bring the main branch in, full build + hash check,
                                                       then fast-forward the main branch to this branch

Then start the session in the worktree folder, e.g. `/match-run BasketF`.

Why `land` exists: on 2026-11-10 seven worktrees ran all day and were merged
only at the end. Their copies of the shared headers (CItem, CScnObj, CGame,
CStg*, CFsm, ...) had drifted apart; merging took dozens of hand-resolved
conflicts and two regressions. Landing after every unit keeps each conflict
small and puts it in front of the session that knows its own change.
"""
import argparse
import os
import re
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


def main_branch(cwd=ROOT):
    return run("git", "symbolic-ref", "--short", "HEAD", cwd=cwd, quiet=True).stdout.strip()


def main_checkout() -> Path:
    """The main checkout, also when this script runs from a worktree's copy."""
    if (ROOT / ".git").is_dir():
        return ROOT
    # The .git file holds "gitdir: <main>/.git/worktrees/<name>" (cmd_new writes a
    # native path; git rev-parse from the MSYS git on PATH would print /c/...).
    gitdir = (ROOT / ".git").read_text().split("gitdir:", 1)[1].strip()
    m = re.match(r"^/([a-zA-Z])/(.*)$", gitdir)
    if m:
        gitdir = f"{m.group(1)}:/{m.group(2)}"
    return Path(gitdir).resolve().parent.parent.parent


def hide_linked_placeholders(wt: Path):
    """Tracked files inside a linked folder (orig/RMKP01/.gitkeep) are 'beyond a
    symbolic link' for git: it cannot stash or merge around them, and every
    merge in the worktree fails. Tell git to leave them alone there."""
    for rel in links():
        for f in run("git", "ls-files", "--", rel, cwd=wt, check=False, quiet=True).stdout.split():
            run("git", "update-index", "--skip-worktree", f, cwd=wt, check=False, quiet=True)


SHARED = ["include", "src", "config", "configure.py", "tools"]


def dirty(cwd):
    """Uncommitted changes to tracked files that a merge or land would touch."""
    out = run("git", "status", "--porcelain", "--untracked-files=no", "--", *SHARED, cwd=cwd, quiet=True).stdout
    return [l[3:] for l in out.splitlines() if l.strip()]


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
    # An MSYS git writes "gitdir: /c/..." which the Windows git cannot follow; use a native path.
    gitfile = wt / ".git"
    gitdir = (ROOT / ".git" / "worktrees" / name).resolve().as_posix()
    gitfile.write_text(f"gitdir: {gitdir}\n")
    for rel in links():
        dst = wt / rel
        if dst.is_dir() and not any(f.name != ".gitkeep" for f in dst.rglob("*")):
            for f in dst.rglob(".gitkeep"):
                f.unlink()
            dst.rmdir()  # only a tracked placeholder was there
        link(ROOT / rel, dst)
    hide_linked_placeholders(wt)
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
    hide_linked_placeholders(wt)
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
        sys.exit("conflicts; merge aborted. Have the worktree's session run `worktree.py land` (it brings this"
                 " branch in, stops on the conflicts for it to resolve, builds, then lands).\n" + r.stdout + r.stderr)
    if not hashes_ok(ROOT):
        # only our own merge is undone; the working tree's other changes stay
        run("git", "reset", "--merge", old)
        sys.exit(f"the merged tree fails the build or the hash check; merge undone (HEAD back at {old[:7]})")
    print(f"merged {branch}; full build and all hashes OK")


def cmd_land():
    """Run from inside a worktree. Main branch in, build + hashes, then fast-forward."""
    wt = ROOT
    if (wt / ".git").is_dir():
        sys.exit("run `land` from inside a worktree; from the main checkout use `merge <name>`")
    main = main_checkout()
    branch = main_branch(wt)
    target = main_branch(main)
    left = dirty(wt)
    if left:
        sys.exit("commit (or revert) these first; land only moves committed work:\n  " + "\n  ".join(left))
    lock = main / ".git" / "decomp-land.lock"
    try:
        fd = os.open(lock, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
        os.write(fd, branch.encode())
        os.close(fd)
    except FileExistsError:
        sys.exit(f"another session is landing ({lock.read_text(errors='replace').strip()}); "
                 "wait a minute and run land again (delete the lock file only if no land is running)")
    try:
        hide_linked_placeholders(wt)
        before = run("git", "rev-parse", "HEAD", cwd=wt, quiet=True).stdout.strip()
        r = run("git", "merge", "--no-edit", target, cwd=wt, check=False, quiet=True)
        if r.returncode:
            conflicts = run("git", "diff", "--name-only", "--diff-filter=U", cwd=wt, quiet=True).stdout.split()
            if not conflicts:
                run("git", "merge", "--abort", cwd=wt, check=False, quiet=True)
                sys.exit("merge failed without conflicts; nothing changed:\n" + r.stdout + r.stderr)
            sys.exit(
                f"{target} conflicts with this branch in {len(conflicts)} file(s); the merge is left in place:\n  "
                + "\n  ".join(conflicts)
                + "\nResolve each one keeping both sessions' facts (notes/decisions.md, 'Landing'), run the full"
                  " build, `git add` the files, `git commit --no-edit`, then run land again."
                  "\nTo back out instead: git merge --abort")
        if not hashes_ok(wt):
            run("git", "reset", "--merge", before, cwd=wt, check=False, quiet=True)
            sys.exit(f"this branch with {target} merged in fails the build or the hash check; merge undone."
                     f"\nSomething in {target} breaks this branch's units (or the reverse): run"
                     f" `git merge {target}`, fix it, full build, commit, then land again.")
        busy = set(dirty(main))
        changed = set(run("git", "diff", "--name-only", f"{target}..{branch}", cwd=wt, quiet=True).stdout.split())
        clash = sorted(busy & changed)
        if clash:
            sys.exit(f"the main checkout has uncommitted edits to files this land would change; nothing landed:\n  "
                     + "\n  ".join(clash) + "\nAsk the orchestrator to commit or set them aside, then land again.")
        r = run("git", "merge", "--ff-only", branch, cwd=main, check=False, quiet=True)
        if r.returncode:
            sys.exit(f"{target} moved during the build; run land again.\n" + r.stdout + r.stderr)
        print(f"landed {branch} on {target}: full build and all hashes OK")
    finally:
        try:
            lock.unlink()
        except OSError:
            pass


def cmd_overlap(quiet_if_none=False):
    """Shared files (include/, config/) changed on two or more unlanded branches,
    or on a branch and on the main branch since that branch last synced."""
    main = main_checkout()
    target = main_branch(main)
    branches = [b.strip(" *+") for b in run("git", "branch", "--list", "wt/*", "--no-merged", target,
                                             cwd=main, quiet=True).stdout.splitlines() if b.strip()]
    touched = {}
    for b in branches:
        base = run("git", "merge-base", target, b, cwd=main, quiet=True).stdout.strip()
        files = run("git", "diff", "--name-only", f"{base}..{b}", "--", "include", cwd=main, quiet=True).stdout.split()
        main_files = run("git", "diff", "--name-only", f"{base}..{target}", "--", "include", cwd=main,
                         quiet=True).stdout.split()
        for f in files:
            touched.setdefault(f, set()).add(b)
            if f in main_files:
                touched[f].add(f"{target} (since {b} last synced)")
    shared = {f: s for f, s in touched.items() if len(s) > 1}
    if not shared:
        if not quiet_if_none:
            print("no shared header is changed on more than one unlanded branch")
        return
    print(f"{len(shared)} shared header(s) changed on more than one branch; land soon (worktree.py land) to keep "
          "the merges small:")
    for f in sorted(shared):
        print(f"  {f}: " + ", ".join(sorted(shared[f])))


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
    ap.add_argument("cmd", choices=["new", "list", "sync", "merge", "remove", "land", "overlap"])
    ap.add_argument("name", nargs="?")
    ap.add_argument("--no-build", action="store_true")
    ap.add_argument("--quiet-if-none", action="store_true", help="overlap: print nothing when there is none")
    args = ap.parse_args()
    if args.cmd not in ("list", "land", "overlap") and not args.name:
        sys.exit("give the worktree's name")
    {"new": lambda: cmd_new(args.name, not args.no_build), "list": cmd_list,
     "sync": lambda: cmd_sync(args.name), "merge": lambda: cmd_merge(args.name),
     "remove": lambda: cmd_remove(args.name), "land": cmd_land,
     "overlap": lambda: cmd_overlap(args.quiet_if_none)}[args.cmd]()


if __name__ == "__main__":
    main()
