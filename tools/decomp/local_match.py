#!/usr/bin/env python3
"""Match functions with a local model (Ollama) in a build-score-retry loop.

A local model in Claude Code spends its small context on the agent harness:
in a 64K window, Flint 40B ran out twice and made no edit in 20 minutes. Given
a compact context pack instead (the m2c draft plus the source of a matched
look-alike), the same model wrote an exact match in 17 seconds. This tool does
the second: it builds the pack, asks the model for the function, inserts it,
builds the object, scores it with diff.py (which also checks data values) and
feeds the result back, for a fixed number of attempts.

The source keeps the model's function only when diff.py reports `match`.
Otherwise the file is restored and the best attempt saved in
notes/local-match/ for a person or a stronger model to finish. A match still
needs the full build before it counts (CLAUDE.md, "Loop for one function").

Usage:
    python tools/decomp/local_match.py <function> [--module M]
    python tools/decomp/local_match.py --pick N [--module M] [--max-size 300]
            the N smallest unmatched functions in units with source that have
            a matched look-alike
    options: --model flint:40b  --attempts 4  --no-copies (leave exact copies
             out of the pack)  --blank (benchmark: empty an already written
             function first; the original is restored afterwards)

Run it in a worktree (tools/decomp/worktree.py new <name>): it edits src/.
Needs Ollama running (http://localhost:11434) with the model pulled.
"""
import argparse
import json
import re
import subprocess
import sys
import time
import urllib.request
from datetime import datetime
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import diff  # noqa: E402
import decomp_common as dc  # noqa: E402  (tools/decomp/decomp_common.py)

ROOT = Path(__file__).resolve().parent.parent.parent
PY = ["py", "-3.12"]
OLLAMA = "http://localhost:11434/api/chat"
LOG = ROOT / "notes" / "local-match.log"
BEST = ROOT / "notes" / "local-match"
STUB_BODY = "{\n    // TODO: local_match.py\n}"
NOISE = re.compile(r"deprecated|multibyte|^#\s*$|Usage Warning")


def run(args, timeout=600):
    r = subprocess.run(PY + args, cwd=ROOT, capture_output=True, text=True, timeout=timeout)
    return r.returncode, r.stdout, r.stderr


def demangle(sym):
    r = subprocess.run([str(ROOT / "build" / "tools" / "dtk.exe"), "demangle", sym], capture_output=True, text=True)
    out = r.stdout.strip()
    return out if r.returncode == 0 and out else None


def qualname(demangled):
    """'msw::CX::Method' from 'msw::CX::Method(int) const'."""
    depth, i = 0, 0
    for i, c in enumerate(demangled):
        if c == "<":
            depth += 1
        elif c == ">":
            depth -= 1
        elif c == "(" and depth == 0:
            break
    return demangled[:i]


def find_definition(text, qname, nargs=None):
    """(start, end) of the definition of qname in a source file, or None.
    A definition starts at column 0 and ends at the next '}' at column 0."""
    pat = re.compile(r"^(?:[^\n;{}]*?[ \t*&])?" + re.escape(qname) + r"\s*\(", re.M)
    for m in pat.finditer(text):
        close = text.find("\n}", m.end())
        brace = text.find("{", m.end())
        semi = text.find(";", m.end())
        if close < 0 or brace < 0 or (0 <= semi < brace):
            continue  # a declaration or call, not a definition
        if nargs is not None:
            params = text[m.end():text.find(")", m.end())]
            n = 0 if not params.strip() or params.strip() == "void" else params.count(",") + 1
            if n != nargs:
                continue
        return m.start(), close + 2
    return None


def count_args(demangled):
    inner = demangled[len(qualname(demangled)):]
    inner = inner[inner.find("(") + 1:inner.rfind(")")]
    if not inner.strip() or inner.strip() == "void":
        return 0
    depth, n = 0, 1
    for c in inner:
        depth += c in "<("
        depth -= c in ">)"
        n += c == "," and depth == 0
    return n


def index():
    return dc.load_index()


def lookalikes(fn, module, no_copies, limit=2):
    """[(similarity, module, function, unit)] of matched look-alikes, best first."""
    code, out, err = run(["tools/decomp/similar.py", fn, "--matched", "--limit", "8"] + (["--module", module] if module else []))
    rows, section = [], None
    for line in out.splitlines():
        if line.startswith("exact copies"):
            section = "copy"
        elif line.startswith("nearest"):
            section = "near"
        elif section == "copy" and not no_copies:
            m = re.match(r"\s+(\S+)\s+(\S+)\s+\((\S+)\)\s+matched", line)
            if m:
                rows.append((100.0, m.group(1), m.group(2), m.group(3)))
        elif section == "near":
            m = re.match(r"\s+([\d.]+)%\s+\d+\s+(\S+)\s+(\S+)\s+\((\S+)\)\s+matched", line)
            if m and float(m.group(1)) >= 60.0:
                if no_copies and float(m.group(1)) >= 100.0:
                    continue
                rows.append((float(m.group(1)), m.group(2), m.group(3), m.group(4)))
    return rows[:limit]


def source_of(units, unit):
    src = units.get(unit, (None, None, None))[2]
    return ROOT / src if src else None


def header_facts(qname, code_snippets, limit=40):
    """Header lines of the target's class that mention identifiers used in the snippets."""
    parts = qname.split("::")
    if len(parts) < 2:
        return ""
    cls = parts[-2]
    idents = set(re.findall(r"\b(m_\w+|s_\w+|[A-Z]\w+_?|[a-z]\w*_)\b", "\n".join(code_snippets)))
    lines = []
    for h in sorted((ROOT / "include").rglob("*.h*")):
        try:
            text = h.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        if not re.search(r"\b(class|struct)\s+" + re.escape(cls) + r"\b", text):
            continue
        for ln in text.splitlines():
            s = ln.strip()
            if s.endswith(";") and any(re.search(r"\b" + re.escape(i) + r"\b", s) for i in idents):
                lines.append(f"{h.relative_to(ROOT).as_posix()}: {s}")
        if lines:
            break
    return "\n".join(dict.fromkeys(lines[:limit]))


_HEADERS = None


def headers():
    """[(path, text)] of every header, read once."""
    global _HEADERS
    if _HEADERS is None:
        _HEADERS = []
        for h in sorted((ROOT / "include").rglob("*.h*")):
            try:
                _HEADERS.append((h, h.read_text(encoding="utf-8", errors="replace")))
            except OSError:
                pass
    return _HEADERS


def inline_in_header(qname):
    """The header that defines qname inline in its class body, or None. The unit
    then only holds a weak copy, and a definition in the .cpp is a redefinition."""
    parts = qname.split("::")
    if len(parts) < 2:
        return None
    method = parts[-1]
    for h, body in class_headers(parts[-2], depth=1):
        pat = re.compile(r"(?<![\w~])" + re.escape(method) + r"\s*\([^;{}]*\)\s*(?:const\s*)?(?::[^;{]*)?\{")
        if pat.search(body):
            return h
    return None


def class_headers(cls, depth=6):
    """Header texts of a class and its bases, nearest first: [(path, class body)]."""
    out, todo, seen = [], [cls], set()
    while todo and len(seen) < depth:
        name = todo.pop(0)
        if name in seen:
            continue
        seen.add(name)
        decl = re.compile(r"\b(?:class|struct)\s+" + re.escape(name) + r"\b\s*(?::\s*([^{;]+))?\{")
        for h, text in headers():
            m = decl.search(text)
            if m:
                # only the class body: one header often declares several classes
                level, end = 0, len(text)
                for i in range(m.end() - 1, len(text)):
                    level += text[i] == "{"
                    level -= text[i] == "}"
                    if level == 0:
                        end = i
                        break
                out.append((h, text[m.end():end]))
                for base in re.findall(r"(?:public|protected|private)\s+([\w:]+)", m.group(1) or ""):
                    todo.append(base.split("::")[-1])
                break
    return out


def offset_facts(qname, draft, limit=30):
    """Our names for the m2c placeholders: unk964 -> the declaration commented 0x964
    in the class or one of its bases (fields and vtable slots carry offset comments)."""
    parts = qname.split("::")
    if len(parts) < 2:
        return ""
    # this->unk5C->unkB94(...) is vtable slot 0xB94; this->unk964 is the field at 0x964
    slots = set(re.findall(r"unk5C->unk([0-9A-F]{1,4})\b", draft))
    fields = set(re.findall(r"this->unk([0-9A-F]{1,4})\b(?!\s*->)", draft)) - {"5C"}
    if not slots and not fields:
        return ""
    found = []
    hdrs = class_headers(parts[-2])
    for kind, offs in (("field", fields), ("virtual", slots)):
        for off in sorted(offs, key=lambda x: int(x, 16)):
            pat = re.compile(r"//\s*0x0*" + (off.lstrip("0") or "0") + r"\b", re.I)
            for h, text in hdrs:
                hit = next((ln.strip() for ln in text.splitlines() if pat.search(ln) and ";" in ln
                            and (("virtual" in ln) == (kind == "virtual"))), None)
                if hit:
                    found.append(f"unk{off} ({kind} 0x{off}) -> {hit}   ({h.relative_to(ROOT).as_posix()})")
                    break
    return "\n".join(found[:limit])


def build_pack(fn, module, unit, units, args):
    dem = demangle(fn) or fn
    q = qualname(dem)
    code, draft, _ = run(["tools/decomp/draft.py", fn, "--module", module], timeout=240)
    draft = "\n".join(draft.splitlines()[:160]) if code == 0 else "(m2c failed on this function)"
    sibs = []
    for sim, smod, sfn, sunit in lookalikes(fn, module, args.no_copies):
        p = source_of(units, sunit)
        sdem = demangle(sfn)
        if not p or not p.exists() or not sdem:
            continue
        text = p.read_text(encoding="utf-8", errors="replace")
        span = find_definition(text, qualname(sdem), count_args(sdem))
        if span:
            sibs.append((sim, smod, sdem, text[span[0]:span[1]]))
    facts = header_facts(q, [s[3] for s in sibs] + [draft])
    offsets = offset_facts(q, draft)
    pack = [f"You are decompiling a Wii game (Metrowerks CodeWarrior C++ for PowerPC) into C++ that compiles to the exact original instructions.",
            f"\nWrite this function: {dem}",
            f"\nm2c draft of the original assembly (placeholder names; unkXX = unknown fields; this->unk5C-style calls through offset 0x5C are usually virtual calls through the vtable; not matching code):\n{draft}"]
    for sim, smod, sdem, src in sibs:
        pack.append(f"\nAn already matched function with {sim:.0f}% similar instructions ({smod}: {sdem}). Its source:\n{src}")
    if offsets:
        pack.append(f"\nWhat the m2c placeholders are in our headers (field offset or vtable slot -> declaration; "
                    f"unkXXX through the vtable pointer at 0x5C is a virtual call):\n{offsets}")
    if facts:
        pack.append(f"\nDeclarations from our headers that may be relevant:\n{facts}")
    pack.append("\nRules: use the real member and method names from the headers and look-alikes, not m2c placeholders. "
                "No inline asm, no #pragma, no hard-coded addresses. Reply with only the complete function definition "
                "(signature and body, written as at file scope with its qualified name) in one ```cpp code block.")
    return dem, q, "\n".join(pack), len(sibs)


def ask(model, messages, think=True, timeout=900):
    req = {"model": model, "stream": False, "messages": messages, "options": {"num_predict": 16384}}
    if not think:
        req["think"] = False
    r = json.loads(urllib.request.urlopen(urllib.request.Request(OLLAMA, json.dumps(req).encode(), {"Content-Type": "application/json"}), timeout=timeout).read())
    reply = r["message"]["content"]
    blocks = re.findall(r"```(?:cpp|c\+\+|c)?\s*\n(.*?)```", reply, re.S)
    return reply, (blocks[-1].strip() if blocks else "")


def score(unit, fn):
    obj = f"build/RMKE01/src/{unit}.o"
    code, out, err = run(["tools/decomp/build.py", obj])
    if code:
        lines = [l for l in (out + err).splitlines() if l.strip() and not NOISE.search(l)]
        return "build", "does not compile:\n" + "\n".join(lines[-20:])
    code, out, err = run(["tools/decomp/diff.py", unit, fn])
    first = (out.splitlines() or ["?"])[0]
    return first.split()[0] if first.split() else "?", "\n".join(out.splitlines()[:45])


RANK = {"match": 0, "registers": 1, "schedule": 2, "stack": 3, "real": 4, "missing": 5, "build": 6, "?": 7}


def match_one(fn, module, args, units, idx):
    hits = [f for f in idx if f["name"] == fn and module in (None, f["module"])]
    if not hits:
        return "not in the index"
    module, unit = hits[0]["module"], hits[0]["unit"]
    src = source_of(units, unit)
    if not src or not src.exists():
        return f"unit {unit} has no source file yet (write or port the unit first)"
    original = src.read_bytes()
    text = original.decode("utf-8", errors="replace")
    dem = demangle(fn) or fn
    q = qualname(dem)
    span = find_definition(text, q, count_args(dem))
    if not span:
        h = inline_in_header(q)
        if h:
            return f"defined inline in {h.relative_to(ROOT).as_posix()} (the unit holds a weak copy): fix it there"
    dem, q, pack, nsib = build_pack(fn, module, unit, units, args)
    if args.blank:
        if not span:
            return "--blank: the function is not written in the source"
        head = text[span[0]:span[1]]
        head = head[:head.find("{")]
        text = text[:span[0]] + head + STUB_BODY + text[span[1]:]
        span = find_definition(text, q, count_args(dem))
    print(f"== {module} {fn}\n   {dem}\n   pack: {len(pack)} chars, {nsib} look-alike(s)", flush=True)
    messages = [{"role": "system", "content": "You write C++ for a matching decompilation. Answer with one cpp code block."},
                {"role": "user", "content": pack}]
    best = ("?", "", "")
    t0 = time.time()
    result = "no attempt"
    think = True
    try:
        for attempt in range(1, args.attempts + 1):
            t = time.time()
            try:
                reply, code = ask(args.model, messages, think)
            except Exception as e:  # Ollama down, timeout
                result = f"model error: {e}"
                break
            messages.append({"role": "assistant", "content": reply})
            if not code or q.split("::")[-1] not in code:
                # usually the thinking used up the output budget: answer directly next time
                cls, detail = "?", "the reply has no definition of the function"
                think = False
            else:
                body = code if code.endswith("\n") else code + "\n"
                cur = text[:span[0]] + body.rstrip("\n") + "\n" + text[span[1]:] if span else text.rstrip("\n") + "\n\n" + body
                src.write_text(cur, encoding="utf-8", newline="")
                cls, detail = score(unit, fn)
            print(f"   attempt {attempt}: {cls} ({time.time() - t:.0f}s)", flush=True)
            if RANK.get(cls, 7) < RANK.get(best[0], 7):
                best = (cls, code, detail)
            if cls == "match":
                break
            messages.append({"role": "user", "content": f"Result of attempt {attempt}: {cls}.\n{detail}\n\n"
                             "Fix the function and reply with the complete corrected definition in one ```cpp block."})
        result = best[0]
    finally:
        if best[0] == "match" and not args.blank:
            # keep the matching version: write it back in case a later attempt replaced it
            final = text[:span[0]] + best[1].rstrip("\n") + "\n" + text[span[1]:] if span else text.rstrip("\n") + "\n\n" + best[1] + "\n"
            src.write_text(final, encoding="utf-8", newline="")
            score(unit, fn)
            kept = True
        else:
            src.write_bytes(original)
            run(["tools/decomp/build.py", f"build/RMKE01/src/{unit}.o"])
            kept = False
    secs = time.time() - t0
    if best[1] and best[0] != "match":
        BEST.mkdir(parents=True, exist_ok=True)
        (BEST / (re.sub(r"[^\w.-]", "_", fn)[:120] + ".cpp")).write_text(
            f"// {module} {fn}: best local attempt ({best[0]}, {args.model})\n{best[1]}\n\n/* diff.py:\n{best[2]}\n*/\n", encoding="utf-8")
    LOG.parent.mkdir(parents=True, exist_ok=True)
    with LOG.open("a", encoding="utf-8") as f:
        f.write(f"{datetime.now():%Y-%m-%d %H:%M} {args.model} {module} {fn} {result}"
                f"{' (blank benchmark)' if args.blank else ''}{' kept' if kept else ''} {secs:.0f}s\n")
    print(f"   => {result}{', kept in the source' if kept else ', source restored'} ({secs:.0f}s)", flush=True)
    return result


def pick(n, module, max_size, units, idx):
    """Open functions only: under 100%, in a unit with source, and not already
    accepted as near/unverified (those are finished, CLAUDE.md "Goal")."""
    import equivalent
    accepted = equivalent.load()
    scores = {(m, f): s for u, (m, fs, src) in units.items() if src for f, s in fs.items()}
    cands = sorted((f for f in idx if (f["module"], f["name"]) in scores and scores[(f["module"], f["name"])] < 100.0
                    and (f["module"], f["name"]) not in accepted
                    and f["size"] <= max_size and module in (None, f["module"])), key=lambda f: f["size"])
    out = []
    for f in cands:
        dem = demangle(f["name"])
        if not dem or inline_in_header(qualname(dem)):
            continue  # weak copy of a header inline: not this unit's to write
        if lookalikes(f["name"], f["module"], no_copies=False, limit=1):
            out.append((f["name"], f["module"]))
            if len(out) >= n:
                break
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("function", nargs="?")
    ap.add_argument("--module")
    ap.add_argument("--pick", type=int)
    ap.add_argument("--max-size", type=int, default=300)
    ap.add_argument("--model", default="flint:40b")
    ap.add_argument("--attempts", type=int, default=4)
    ap.add_argument("--no-copies", action="store_true")
    ap.add_argument("--blank", action="store_true")
    ap.add_argument("--allow-main", action="store_true", help="run in the main checkout anyway")
    args = ap.parse_args()
    if (ROOT / ".git").is_dir() and not args.allow_main:
        sys.exit("this is the main checkout: run in a worktree (tools/decomp/worktree.py new <name>) or pass --allow-main")
    units, idx = diff.report_units("RMKE01"), index()
    todo = [(args.function, args.module)] if args.function else pick(args.pick or 1, args.module, args.max_size, units, idx)
    results = [match_one(fn, mod, args, units, idx) for fn, mod in todo]
    print(f"\n{sum(r == 'match' for r in results)} of {len(results)} matched")


if __name__ == "__main__":
    main()
