#!/usr/bin/env python3
"""Independent re-check of the submission package.

Checks:
  1. MANIFEST.sha256 (every file hash);
  2. the four cap-33 run logs (state/level/complete/hit/overflow values, histogram sums, histogram equality between the
     two implementations, and the hit sets);
  3. the P2 replication verification JSON;
  4. the closure-certificate log;
  5. the Q/E model cross-check results;
  6. every certificate: replay with the vendored ms14 verifier AND with the vendored official SAIR 14-move verifier.

Run:  python3 code/verify_package.py
Exit code 0 iff everything passes (prints "PACKAGE VERIFICATION: PASS").
"""
import glob
import hashlib
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
FAILS = []
PASSES = 0


def ok(msg):
    global PASSES
    PASSES += 1
    print("ok:", msg)


def fail(msg):
    FAILS.append(msg)
    print("FAIL:", msg)


def read_text(path):
    raw = Path(path).read_bytes()
    for enc in ("utf-8-sig", "utf-16", "latin-1"):
        try:
            return raw.decode(enc)
        except Exception:
            continue
    return raw.decode("latin-1")


def parse_log(path):
    text = read_text(path)
    results = re.findall(r"RESULT[^\r\n]*", text)
    if not results:
        return None
    hits = re.findall(r"HIT target[^\r\n]*", text)
    idx = text.rfind("states by total length:")
    hist = {}
    if idx >= 0:
        tail = text[idx + len("states by total length:"):].replace("\r", "").replace("\n", "")
        hist = {int(k): int(v) for k, v in re.findall(r"(\d+):(\d+)", tail)}

    def field(name, default=None):
        m = re.search(name + r"=(\d+)", results[-1])
        return int(m.group(1)) if m else default

    return {"result": results[-1], "hits": hits, "hist": hist,
            "states": field("states"), "levels": field("levels"),
            "complete": field("complete"), "hit": field("hit", None),
            "overflow": field("overflow")}


def check_manifest():
    mf = ROOT / "MANIFEST.sha256"
    if not mf.exists():
        fail("MANIFEST.sha256 missing")
        return
    n = 0
    for line in mf.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        m = re.match(r"([0-9a-f]{64})  (.+)$", line)
        if not m:
            fail("bad manifest line: " + line[:90])
            continue
        h, rel = m.groups()
        p = ROOT / rel
        n += 1
        if not p.exists():
            fail("missing file: " + rel)
        elif hashlib.sha256(p.read_bytes()).hexdigest() != h:
            fail("hash mismatch: " + rel)
    if not FAILS:
        ok("MANIFEST.sha256: %d files verified" % n)


CAP33 = [
    ("evidence/acsearch_P1_c33_allhits.log", 94705057, 448, 1, 1),
    ("evidence/ms14ind_P1_c33_rerun.log", 94705057, 448, 1, 1),
    ("evidence/P2_c33_allhits.log", 235809128, 457, 1, 0),
    ("evidence/ms14ind_P2_c33.log", 235809128, 457, 1, 0),
]


def check_cap33():
    parsed = {}
    for rel, states, levels, complete, hits_expected in CAP33:
        p = ROOT / rel
        if not p.exists():
            fail("missing log: " + rel)
            continue
        d = parse_log(p)
        if d is None:
            fail("no RESULT line in " + rel)
            continue
        parsed[rel] = d
        for name, exp in (("states", states), ("levels", levels), ("complete", complete), ("overflow", 0)):
            if d[name] != exp:
                fail("%s: %s=%s expected %s" % (rel, name, d[name], exp))
        if hits_expected == 0 and d["hits"]:
            fail("%s: unexpected HIT lines: %r" % (rel, d["hits"][:2]))
        if hits_expected and len(d["hits"]) != hits_expected:
            fail("%s: %d HIT lines, expected %d" % (rel, len(d["hits"]), hits_expected))
        if sum(d["hist"].values()) != d["states"]:
            fail("%s: histogram sum %d != states %d" % (rel, sum(d["hist"].values()), d["states"]))
        if len(d["hist"]) != 20:
            fail("%s: expected 20 histogram bins, got %d" % (rel, len(d["hist"])))
    pairs = [("evidence/acsearch_P1_c33_allhits.log", "evidence/ms14ind_P1_c33_rerun.log"),
             ("evidence/P2_c33_allhits.log", "evidence/ms14ind_P2_c33.log")]
    for lean, ind in pairs:
        if lean in parsed and ind in parsed:
            if parsed[lean]["hist"] != parsed[ind]["hist"]:
                fail("histograms differ: %s vs %s" % (lean, ind))
            else:
                ok("cap-33 histogram identical between implementations: " + lean.split("/")[-1])
    if "evidence/acsearch_P1_c33_allhits.log" in parsed:
        hits = parsed["evidence/acsearch_P1_c33_allhits.log"]["hits"]
        if len(hits) == 1 and "target 6" in hits[0]:
            ok("P1 cap-33: single hit is the known C6 link (target 6)")
        else:
            fail("P1 cap-33 hit set unexpected: %r" % (hits[:3],))
    if "evidence/P2_c33_allhits.log" in parsed:
        ok("P2 cap-33: no target in the component (hit set empty)")


def check_replication_json():
    p = ROOT / "evidence" / "P2_c33_replication_verification.json"
    if not p.exists():
        fail("missing P2 replication verification JSON")
        return
    d = json.loads(p.read_text(encoding="utf-8"))
    if d.get("verification_pass") is True:
        ok("P2 cap-33 replication: verification_pass=true")
    else:
        fail("P2 cap-33 replication: verification_pass != true")


def check_closure():
    p = ROOT / "evidence" / "closure_AK3_c28.log"
    if not p.exists():
        fail("missing closure log")
        return
    t = read_text(p)
    if "VERDICT: PASS" in t and "violations: 0" in t:
        ok("AK(3) cap-28 closure certificate: PASS, 0 violations")
    else:
        fail("closure certificate did not pass")


def check_crosscheck():
    p = ROOT / "evidence" / "crosscheck_results.txt"
    if not p.exists():
        fail("missing crosscheck results")
        return
    t = read_text(p)
    expects = ["own classes 17718", "own classes 21932",
               "projected classes 473", "projected classes 521",
               "own-not-acsearch 0", "projected-not-in-acsearch 0",
               "exit=0"]
    missing = [e for e in expects if e not in t]
    if missing:
        fail("crosscheck results missing: %r" % missing)
    else:
        ok("Q/E model cross-checks: expected numbers and zero differences present")


def check_certificates():
    certs = [ROOT / "certs" / "P1_equiv_C6.json",
             ROOT / "certs" / "P1_phi1_C6_461.json",
             ROOT / "certs" / "C6_phi1_C6_1920.json"]
    certs += sorted((ROOT / "certs" / "phi").glob("*.json"))
    # (1) vendored ms14 verifier
    sys.path.insert(0, str(ROOT / "third_party" / "ms14"))
    import verify as V  # noqa: E402
    for c in certs:
        cert = json.loads(c.read_text(encoding="utf-8"))
        good, msg, st = V.verify(cert)
        if not good:
            fail("ms14 verifier FAIL %s: %s" % (c.name, msg))
        else:
            extra = ""
            if st:
                extra = " moves=%s classical=%s peak=%s" % (st.get("moves"), st.get("classical"), st.get("peak_total"))
            ok("ms14 verifier PASS %s%s" % (c.name, extra))
    # (2) official SAIR 14-move semantics
    sys.path.insert(0, str(ROOT / "third_party" / "sair-verifier" / "competition" / "tools"))
    try:
        import verifier.core as core  # noqa: E402
    except Exception as exc:  # pragma: no cover
        fail("cannot import vendored SAIR verifier: %r" % (exc,))
        return

    def freered(w):
        out = []
        for a in w:
            if out and out[-1] == -a:
                out.pop()
            else:
                out.append(a)
        return out

    def convert(moves):
        ids = []
        for mv in moves:
            op = mv[0]
            if op == "invert":
                ids.append(0 if mv[1] == 0 else 1)
            elif op == "mul":
                i, j, s = mv[1], mv[2], mv[3]
                if i == 0 and j == 1:
                    ids.append(2 if s == 1 else 3)
                elif i == 1 and j == 0:
                    ids.append(4 if s == 1 else 5)
                else:
                    raise ValueError("unsupported mul " + repr(mv))
            elif op == "conj":
                i, g = mv[1], mv[2]
                gmap = {1: 0, -1: 1, 2: 2, -2: 3}
                ids.append((6 if i == 0 else 10) + gmap[g])
            else:
                raise ValueError("unsupported move " + repr(mv))
        return ids

    for c in certs:
        cert = json.loads(c.read_text(encoding="utf-8"))
        # the official verifier expects a pair of freely reduced TUPLES
        state = tuple(tuple(freered(r)) for r in cert["initial"])
        try:
            ids = convert(cert["moves"])
            for m in ids:
                state = core.apply_move(state, m)
        except Exception as exc:  # pragma: no cover
            fail("official replay exception %s: %r" % (c.name, exc))
            continue
        reached = [list(state[0]), list(state[1])]
        good = True
        if cert.get("reached") is not None:
            good = reached == [list(r) for r in cert["reached"]]
        if good:
            ok("official SAIR replay PASS %s (%d moves)" % (c.name, len(ids)))
        else:
            fail("official SAIR replay terminal mismatch %s" % c.name)


def main():
    print("package root:", ROOT)
    check_manifest()
    check_cap33()
    check_replication_json()
    check_closure()
    check_crosscheck()
    check_certificates()
    print()
    if FAILS:
        print("PACKAGE VERIFICATION: FAIL (%d checks failed)" % len(FAILS))
        return 1
    print("PACKAGE VERIFICATION: PASS (%d checks passed)" % PASSES)
    return 0


if __name__ == "__main__":
    sys.exit(main())
