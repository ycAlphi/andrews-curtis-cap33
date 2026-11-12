# Independent verification and a cap-33 extension of the length-14 bounded Andrews–Curtis components

**Status**: submission package for the **SAIR ACC Proof Track** (draft; not yet submitted).
**Claim type**: partial computational result. **Nothing here proves or disproves the Andrews–Curtis conjecture or Stable AC.**

## TL;DR

In the SAIR ACC **14-move model** (`ac-r2-v1`), through the capped quotient search model of the ms14 repository
([alejandrozarco/andrews-curtis-ms14](https://github.com/alejandrozarco/andrews-curtis-ms14), Zenodo `10.5281/zenodo.23196602`),
we exhaustively enumerated the cap-33 components (conjugator bound `K = 9`, satisfying the published completeness condition
`K >= floor((L-14)/2)`) of the two unresolved length-14 Miller–Schupp presentations

- **P1** = `ac-02089` = MS(2, x⁻²y⁻¹x²y) = `<x,y | XyyxYYY, XXXYxxy>`
- **P2** = `ac-08126` = MS(2, x⁻²y⁻¹x²y⁻¹) = `<x,y | XyyxYYY, XXXYxxY>`

with targets AK(3) = `ac-00399` = `<xxxYYYY, xyxYXY>`, the other start, the trivial presentation `<x|y>`, and the residual
classes C3, C4, C6.

| run | states | levels | complete | targets hit | consequence (model-relative) |
|---|---|---|---|---|---|
| **P1 cap 33** | **94,705,057** | 448 | yes | only C6 (the known P1~C6 link) | `B(P1,AK(3)) >= 34`, `B(P1,P2) >= 34`, `B(P1) >= 34` |
| **P2 cap 33** | **235,809,128** | 457 | yes | none | `B(P2,AK(3)) >= 34`, `B(P2,P1) >= 34`, `B(P2) >= 34`, `B(P2,C6/C3/C4) >= 34` |

Here `B(P,Q)` is the infimum over AC-equivalence ledgers from `P` to `Q` of the maximum total relator length (peak).
Both runs were **replicated exactly by a second, independently written implementation**
(`code/ms14_engine.cpp`; same states, levels, complete flag, length histogram and hit set).
The published predecessor — SAIR Proof Track submission **ACC-P000012** (2026-09-30, Alexander Keur; the ms14 project) —
established the bound `>= 33` and labels its enumerations "computational, not independently verified". This package
is an independent verification plus a one-cap extension of that result.

## What is in this package

```
README.md                     this file
report/REPORT.md              technical report (method, results, limitations, references)
submission/                   exact SAIR Proof Track submission fields, description, checklist, novelty record
code/                         independent engine, official-semantics replayer, closure checker, package verifier
certs/                        elementary AC ledgers (regenerated / newly found), all replayable
evidence/                     run logs, replication checks, model cross-checks, closure certificates, hashes
third_party/ms14/             unmodified third-party search engine and ledger tools (Apache-2.0, cited)
third_party/sair-verifier/    unmodified official SAIR verifier package (Apache-2.0, commit a0fd6e6, cited)
data/                         problem IDs, relator words, target list
LICENSE                       Apache-2.0
THIRD_PARTY_NOTICES.md        attribution and licenses of third-party material
AI_DISCLOSURE.md              AI-generation disclosure
CITATION.cff
MANIFEST.sha256               SHA-256 of every file in this package
```

## Quick check (recommended first step)

```bash
python3 code/verify_package.py
```

It (a) verifies all SHA-256 checksums, (b) re-reads the evidence logs and checks the exact state counts / level counts /
complete flags / hit sets, (c) replays every certificate with the vendored ms14 verifier **and** with the vendored
official SAIR 14-move implementation, and (d) checks the closure certificate and the Q/E model cross-check logs.
It should print `PACKAGE VERIFICATION: PASS`.

## Results in detail

### P1 = ac-02089, cap 33, K = 9
```
RESULT start=XyyxYYY|XXXYxxy cap=33 conj=9 nosym=0 states=94705057 levels=448 complete=1 hit=1 overflow=0 time=1278s
states by total length: 14:2 15:4 16:11 17:52 18:83 19:337 20:484 21:1840 22:2836 23:10279
  24:16097 25:56953 26:93180 27:326697 28:553234 29:1902711 30:3288793 31:10875136
  32:18232112 33:59344216
```
The only target met is C6 (target index 6, at level 48), i.e. the known link P1 ~ C6 certified by the 1066-move ledger.
No other target (AK(3), P2, trivial, C3, C4) belongs to the cap-33 component.
Independent replication (`code/ms14_engine.cpp`, `maxstates=105M`): identical `states=94705057, levels=448, complete=1`
and identical histogram; the only hit is again C6.

### P2 = ac-08126, cap 33, K = 9
```
RESULT start=XyyxYYY|XXXYxxY cap=33 conj=9 nosym=0 states=235809128 levels=457 complete=1 hit=0 overflow=0 time=2429s
states by total length: 14:2 15:14 16:26 17:181 18:236 19:1113 20:1264 21:5654 22:6543 23:30516
  24:35041 25:164342 26:191672 27:902871 28:1074459 29:5078553 30:6137130 31:28633854
  32:34105282 33:159440375
```
No target is met: AK(3), P1, the trivial presentation, C3, C4 and C6 are all absent from the cap-33 component.
Independent replication (`code/ms14_engine.cpp`, `maxstates=250M`): identical `states=235809128, levels=457,
complete=1`, identical histogram, no target hit (`evidence/P2_c33_replication_verification.json`,
`verification_pass: true`).

### Why a `complete=1` component gives a peak lower bound

The capped component enumerates all quotients (modulo relator rotation, inversion, swap and the 8 signed letter
permutations) of exact presentations reachable under SAIR moves without total cyclically reduced relator length
exceeding `L = 33`. Every exact state `(a u a^{-1}, b v b^{-1})` projects to `{u,v}`, and a multiplication projects to the
bounded-conjugator move used by the enumeration; with `K >= floor((L-m)/2)`, `m = 14` the minimal total length in the
component, the quotient graph contains the image of the exact graph. Hence if the cap-33 component is complete and
does not contain the target class, no SAIR ledger reaching that target can keep the peak at `<= 33`, so the peak is
`>= 34`. We re-ran the ms14 small-cap cross-checks of this model independently: Q at cap 24 (independent quotient
implementation vs the search dumps) gives exactly the same class sets (P1: 17,718; P2: 21,932), and E at cap 20
(exact graph under the official `verifier.core.apply_move`, projected) stays inside the search components
(P1: 1,219,064 exact states -> 473 classes; P2: 696,024 -> 521 classes), matching the published cross-check numbers.

### Other artifacts

- **Certificates** (`certs/`), all replayed with the official SAIR 14-move implementation:
  - `P1_equiv_C6.json`: 1066 elementary moves, peak 41 (regenerated byte-for-byte from the ms14 path file);
  - `phi/`: eight AK(3) ~ φ(AK(3)) bridges (0–151 moves);
  - `P1_phi1_C6_461.json`: P1 ~ φ₁(C6), 461 moves, peak 49 (φ₁: x -> x⁻¹), found in this work;
  - `C6_phi1_C6_1920.json`: C6 ~ φ₁(C6), 1920 moves, peak 43, found in this work.
  These do **not** improve the shortest known P1~C6 ledger, and `P1 ~ φ₁(C6)` is not by itself a certificate for `P1 ~ C6`.
- **Closure certificate** for AK(3) at cap 28: a sorted 128-bit canonical-key file (`AK3_c28.keys`, 8,694,374 states,
  139,109,984 bytes, not shipped here — SHA-256 in `evidence/keys_AK3_c28.sha256`; regenerate with the command in
  `BUILD.md`) plus the standalone checker `code/closure_check.cpp`, which re-generates all capped moves
  (238,806,300 moves checked, 0 violations), verifies the start class is present and the trivial presentation absent,
  certifying `B(AK(3)) >= 29` without trusting the search's BFS conclusion.
- **Replication of the published counts**: 11/11 ms14 capped-component counts (P1 caps 24/29/30/31/32, P2 caps
  24/29/30/31/32, AK(3) cap 26; including P1 cap-32 = 32,699,098 and P2 cap-32 = 71,499,215) reproduced digit-for-digit
  with matching length histograms, and 6/6 nahomar rotation-only cap-28 counts.

## Reproducing everything

See `BUILD.md` for build commands and exact run lines (GCC/G++, Python 3.11; Windows or Linux; 20 logical cores,
15.7 GB RAM used; the P2 run needs ~4.2 GB and ~40 min; the P1 run ~2.5 GB and ~21 min). All logs in `evidence/`
carry SHA-256 in `MANIFEST.sha256`; binaries used are recorded in `evidence/build_env.txt`.

## Novelty statement (as of 2026-11-12)

Based on a scan of the public SAIR ACC Proof Track submissions (23 public submissions on 2026-11-12; see
`submission/NOVELTY_SCAN.md` and `evidence/sair_acc_proof_submissions_summary.json`), the ms14 repository and Zenodo
record (v2, 2026-10-06), the SAIR census v2 (2026-11-10), arXiv listings, GitHub and Zenodo, no public artifact reports
a complete cap-33 component for ac-02089/ac-08126 or the bound `B >= 34`; the closest is ACC-P000012 (2026-09-30)
with `>= 33`, self-labelled "not independently verified". This is a novelty statement relative to the public record, not
an absolute priority claim; it does not exclude unpublished work.

## Limitations

All statements are **model-relative** (SAIR 14 moves through the capped quotient model and its published completeness
condition). They do not resolve the general (or stable) Andrews–Curtis conjecture, nor the Shehper et al. claims that
the residual length-14 presentations are AC-equivalent to AK(3). The cap-33 enumerations were produced by the
third-party ms14 engine (Apache-2.0, AI-generated, not peer reviewed) and replicated by the independent implementation
shipped here; the projection/completeness argument is documented but not formalized in a proof assistant.

## License and citation

Our code and text are released under the Apache License 2.0 (`LICENSE`); third-party material retains its license —
see `THIRD_PARTY_NOTICES.md`. If you use this package, please cite the accompanying report (`CITATION.cff`) and the
prior work listed there.

## Submission status

Not submitted. The SAIR ACC Proof Track submission fields and copy-paste text are in `submission/`.
Deadline for the track: **2026-12-01T11:59:59Z** (= 2026-11-30 AoE).
