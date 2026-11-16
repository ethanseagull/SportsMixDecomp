"""Shared helpers for tools/decomp: paths, the function index and the objdiff report.

The tools used to load these each in their own way; a missing index crashed
some, others went on silently without it, and none noticed an index or a
report.json left behind by a later build (on 2026-11-15 the main checkout's
index was five days older than the split and lacked 5797 functions).

    load_index()        the function index, rebuilt first if it is missing or
                        was made from another split (about 7 s)
    ensure_report()     rebuild build/<version>/report.json if a compiled
                        object is newer than it (well under a second)
    load_report()       report.json, fresh (calls ensure_report)
"""
import hashlib
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
VERSION = "RMKE01"
_cache = {}


def _note(msg):
    print(f"({msg})", file=sys.stderr, flush=True)


# --- function index -------------------------------------------------------

def index_path(version=VERSION):
    return ROOT / "build" / "decomp" / f"index_{version}.json"


def _split_digest(version):
    cfg = ROOT / "build" / version / "config.json"
    return hashlib.sha1(cfg.read_bytes()).hexdigest() if cfg.exists() else ""


def index_is_stale(version=VERSION):
    """True if the index is missing or was built from another split. The
    split's config.json is rewritten by every configure, so its digest, not
    its time, is what tells."""
    p = index_path(version)
    stamp = p.with_suffix(".split")
    if not p.exists():
        return True
    if not stamp.exists():
        return True
    return stamp.read_text().strip() != _split_digest(version)


def rebuild_index(version=VERSION):
    r = subprocess.run([sys.executable, str(ROOT / "tools" / "decomp" / "index.py"), "--version", version],
                       cwd=ROOT, capture_output=True, text=True)
    if r.returncode:
        raise SystemExit("tools/decomp/index.py failed:\n" + (r.stdout + r.stderr)[-1500:])
    index_path(version).with_suffix(".split").write_text(_split_digest(version))


def load_index(version=VERSION, rebuild=True):
    """[{module, unit, name, size, hash, ops}] for every function of the split."""
    key = ("index", version)
    if key not in _cache:
        if rebuild and index_is_stale(version) and (ROOT / "build" / version / "config.json").exists():
            _note("function index missing or from another split: rebuilding it")
            rebuild_index(version)
        p = index_path(version)
        _cache[key] = json.loads(p.read_text(encoding="utf-8")) if p.exists() else []
    return _cache[key]


# --- objdiff report ---------------------------------------------------------

def report_path(version=VERSION):
    return ROOT / "build" / version / "report.json"


def report_is_stale(version=VERSION):
    """True if some compiled object of ours is newer than report.json."""
    rp = report_path(version)
    if not rp.exists():
        return True
    t = rp.stat().st_mtime
    src = ROOT / "build" / version / "src"
    return src.exists() and any(o.stat().st_mtime > t for o in src.rglob("*.o"))


def ensure_report(version=VERSION):
    """Rebuild report.json when an object is newer, so scores are never stale
    (a stub once showed 100% because only its object had been rebuilt)."""
    if report_is_stale(version):
        _note("report.json older than a compiled object: regenerating it")
        r = subprocess.run([sys.executable, str(ROOT / "tools" / "decomp" / "build.py"),
                            f"build/{version}/report.json"], cwd=ROOT, capture_output=True, text=True)
        if r.returncode:
            _note("could not regenerate report.json; scores may be stale")
        _cache.pop(("report", version), None)


def load_report(version=VERSION, fresh=True):
    key = ("report", version)
    if fresh:
        ensure_report(version)
    if key not in _cache:
        _cache[key] = json.loads(report_path(version).read_text(encoding="utf-8"))
    return _cache[key]
