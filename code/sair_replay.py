#!/usr/bin/env python3
"""Independently replay the ms14 certificates in the OFFICIAL SAIR ACC move
language (14-move spec ac-r2-v1), using the competition's own verifier core.

The ms14 certificates are stored in the Carreras-style ledger format
(["invert", i], ["mul", i, j, s], ["conj", i, g]); here they are converted to
the official move ids 0..13 and replayed with `verifier.core.apply_move` from
github.com/SAIRcompetition/Andrews-Curtis (commit a0fd6e6), an implementation
we did not write.

Checks per certificate:
  (a) the converted ledger is legal under the official move semantics;
  (b) the official terminal state equals the terminal reached by our own
      independent replay (see code/ in the previous run) -- i.e. both
      implementations agree on every step's effect;
  (c) the peak total relator length is recorded.

NOTE: the official verifier's `verify()` only accepts *trivialisations* and
also enforces the competition's length/work limits, so for these AC-equivalence
certificates we call apply_move directly (as ms14's own sair_check does).

Usage:
  python sair_replay.py <certs_dir> <ms14_verify_dir> <verifier_package_parent>
"""
import importlib
import json
import sys
from pathlib import Path

C2I = {"x": 1, "X": -1, "y": 2, "Y": -2}


def encode(words):
    return [[C2I[c] for c in w] for w in words]


def freered(w):
    out = []
    for a in w:
        if out and out[-1] == -a:
            out.pop()
        else:
            out.append(a)
    return tuple(out)


def convert_moves(moves):
    """Carreras ledger moves -> official 14-move ids."""
    ids = []
    for mv in moves:
        op = mv[0]
        if op == "invert":
            i = mv[1]
            ids.append(0 if i == 0 else 1)
        elif op == "mul":
            i, j, s = mv[1], mv[2], mv[3]
            if i == 0 and j == 1:
                ids.append(2 if s == 1 else 3)
            elif i == 1 and j == 0:
                ids.append(4 if s == 1 else 5)
            else:
                raise ValueError(f"mul with unsupported relator pair: {mv}")
        elif op == "conj":
            i, g = mv[1], mv[2]
            gmap = {1: 0, -1: 1, 2: 2, -2: 3}
            if g not in gmap:
                raise ValueError(f"bad conjugator: {mv}")
            base = 6 if i == 0 else 10
            ids.append(base + gmap[g])
        else:
            raise ValueError(f"unsupported move {mv}")
    return ids


def main():
    certs_dir = Path(sys.argv[1])
    verifier_parent = Path(sys.argv[2])
    sys.path.insert(0, str(verifier_parent))
    core = importlib.import_module("verifier.core")
    print(f"official verifier loaded: {core.__file__}")
    print(f"move spec version: {core.MOVE_SPEC_VERSION}, moves: {core.NUM_MOVES}")

    targets = []
    for p in sorted(certs_dir.glob("*.json")):
        targets.append(p)
    for p in sorted((certs_dir / "phi").glob("*.json")):
        targets.append(p)

    all_ok = True
    for path in targets:
        cert = json.loads(path.read_text(encoding="utf-8"))
        initial = [freered(list(r)) for r in cert["initial"]]
        state = (initial[0], initial[1]) if len(initial) == 2 else tuple(initial)
        try:
            ids = convert_moves(cert["moves"])
        except ValueError as exc:
            print(f"FAIL {path.name}: conversion error: {exc}")
            all_ok = False
            continue
        peak = len(state[0]) + len(state[1])
        ok = True
        for k, m in enumerate(ids):
            try:
                state = core.apply_move(state, m)
            except Exception as exc:  # noqa: BLE001
                print(f"FAIL {path.name}: official apply_move raised at {k}: {exc}")
                ok = False
                break
            tot = len(state[0]) + len(state[1])
            peak = max(peak, tot)
        if not ok:
            all_ok = False
            continue
        official_terminal = [list(state[0]), list(state[1])]
        # (b) compare with our own independent replay terminal (recomputed here
        # from the certificate's own move list with an independent implementation)
        our = [list(r) for r in cert["initial"]]
        for mv in cert["moves"]:
            if mv[0] == "invert":
                our[mv[1]] = list(freered([-a for a in reversed(our[mv[1]])]))
            elif mv[0] == "mul":
                i, j, s = mv[1], mv[2], mv[3]
                other = our[j] if s == 1 else [-a for a in reversed(our[j])]
                our[i] = list(freered(our[i] + other))
            elif mv[0] == "conj":
                i, g = mv[1], mv[2]
                our[i] = list(freered([g] + our[i] + [-g]))
            else:
                raise SystemExit(f"unexpected move {mv}")
        agree = official_terminal == [list(r) for r in our]
        reached_ok = True
        if "reached" in cert and cert["reached"] is not None:
            reached_ok = official_terminal == [list(r) for r in cert["reached"]]
        status = "PASS" if (agree and reached_ok) else "FAIL"
        if status == "FAIL":
            all_ok = False
        print(f"{status} {path.name}: {len(ids)} official moves, peak={peak}, "
              f"implementations_agree={agree}, reached_field_ok={reached_ok}")
    print()
    print("VERDICT:", "PASS (official SAIR move semantics replay all ms14 certificates)"
          if all_ok else "FAIL")
    return 0 if all_ok else 1


if __name__ == "__main__":
    sys.exit(main())
