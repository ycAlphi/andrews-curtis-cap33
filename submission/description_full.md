# Description (full version for repository / paper-like link)

**Partial computational result. Nothing here proves or disproves the Andrews–Curtis conjecture or Stable AC.**

## Setting

We work in the SAIR ACC 14-move model (`ac-r2-v1`, see `third_party/sair-verifier/`): invert a relator; multiply it by
the other relator or its inverse; conjugate it by a single signed generator; freely reduce after each move. The goal in
the competition is the exact ordered pair `(x,y)`. For a presentation `P` and a target `Q`, let `B(P,Q)` be the infimum
over AC-equivalence ledgers from `P` to `Q` of the maximum total relator length along the ledger (the **peak**), and
`B(P) := B(P, <x,y|x,y>)`.

We study the two unresolved length-14 Miller–Schupp presentations and the residual classes singled out by the
length-14 classification (Shehper et al., arXiv:2408.15332; Carreras, arXiv:2607.23611; nahomar/andrews-curtis-solver):

| name | SAIR id | presentation | relator words (`X=x⁻¹`, `Y=y⁻¹`) |
|---|---|---|---|
| P1 | `ac-02089` | MS(2, x⁻²y⁻¹x²y) | `XyyxYYY \| XXXYxxy` |
| P2 | `ac-08126` | MS(2, x⁻²y⁻¹x²y⁻¹) | `XyyxYYY \| XXXYxxY` |
| AK(3) | `ac-00399` | `<x,y \| x³=y⁴, xyx=yxy>` | `xxxYYYY \| xyxYXY` |
| C3 | — | residual class | `xxyXy \| xyyyyyxYY` |
| C4 | — | residual class | `xxxxyXy \| xyyyxYY` |
| C6 | — | residual class | `xxyxxYY \| xyXYYXy` |

## The search model and its completeness condition

We use the capped quotient search model documented in the ms14 repository (ACC-P000012; Apache-2.0; AI-generated, not
peer reviewed). States are unordered pairs of nonempty cyclically reduced words, modulo rotation and inversion of either
word, swapping, and the 8 signed letter permutations. A move replaces one relator by the cyclic reduction of

  `rot_i(u) · c · rot_j(v^{±1}) · c^{-1}`,

with `c` a reduced word; for `c = 1` all rotations are used, for `|c| ≥ 1` only junction-free conjugators (other `c`
reduce to shorter ones with other rotations/inversions). Only states with total length `≤ L` are kept.

The projection argument connecting this quotient model to the exact 14-move graph is as follows (as documented in the
ms14 README; we did not reprove it here, but re-ran its small-cap checks): write an exact state as
`(a u a^{-1}, b v b^{-1})` with `u, v` cyclically reduced; it projects to the class `{u,v}`. A multiplication of one
relator by the other projects to one of the capped moves above with conjugator `c = a^{-1}b` (up to rotations and an
inverse), and `|a|+|b| ≤ (L-m)/2` where `m` is the minimal total length in the component. Hence with `K ≥ ⌊(L-m)/2⌋`
the quotient graph contains the image of the exact graph of peak `≤ L`; if the cap-`L` component is complete and the
target class is absent, every exact ledger reaching the target has peak `> L`, i.e. `≥ L+1`.

We re-ran the model cross-checks published with the ms14 repository, independently, and obtained the same numbers:
- **Q** (independent Python quotient implementation vs the search dumps, cap 24): P1 17,718 classes, P2 21,932 classes,
  zero differences in both directions.
- **E** (exact graph under the official `verifier.core.apply_move`, cap 20, projected): P1 1,219,064 exact states
  projecting onto 473 classes, P2 696,024 onto 521 classes, all inside the search components (zero missing).

## New results

Complete cap-33 components (`L = 33`, `K = 9`, satisfying `K ≥ ⌊(33-14)/2⌋`):

**P1 = ac-02089** (`states = 94,705,057`, `levels = 448`, `complete = 1`, `hit = 1`):
the only target met is C6 (the known P1~C6 link, at level 48). It follows that every SAIR AC path from ac-02089 to
ac-00399, to ac-08126, or to the trivial presentation has peak `≥ 34`.

**P2 = ac-08126** (`states = 235,809,128`, `levels = 457`, `complete = 1`, `hit = 0`):
no target is met — AK(3), P1, the trivial presentation, C3, C4 and C6 are all absent. It follows that every SAIR AC path
from ac-08126 to ac-00399, to ac-02089, to the trivial presentation, or to C3/C4/C6 has peak `≥ 34`.

Both runs carry the length histograms quoted in `README.md` and in `evidence/`. The published predecessor (ACC-P000012 /
ms14) established `≥ 33` for both starts (caps 29–32) and explicitly labels its enumerations "computational, not
independently verified"; this package is the independent verification plus one more cap for both starts.

## Independent verification

- A second implementation (`code/ms14_engine.cpp`) was written from the published search semantics, sharing no code
  with the ms14 engine. It reproduces, digit for digit: all 11 published capped-component counts (P1 caps
  24/29/30/31/32, P2 caps 24/29/30/31/32, AK(3) cap 26; including P1 cap-32 = 32,699,098 and P2 cap-32 = 71,499,215,
  with levels and length histograms), the six nahomar rotation-only cap-28 counts, and the Q/E model cross-checks. It
  reproduces both cap-33 runs exactly: same state counts, level counts, complete flags, length histograms and hit sets
  (P2 verification record: `evidence/P2_c33_replication_verification.json`, `verification_pass: true`).
- The ms14 1066-move P1~C6 ledger is regenerated byte-for-byte from its path file; every certificate in `certs/` is
  replayed under the official SAIR 14-move implementation and under the independent ms14 verifier.

## Machine-checkable closure certificate

For AK(3) at cap 28 we ship a closure certificate: a sorted 128-bit canonical-class-key file (8,694,374 classes;
139,109,984 bytes; SHA-256 in `evidence/keys_AK3_c28.sha256`, regeneration command in `BUILD.md`) plus a standalone
checker (`code/closure_check.cpp`) that re-generates all capped moves from scratch (238,806,300 moves checked,
0 violations), verifies the start class is present and the trivial presentation is absent. This certifies
`B(AK(3)) ≥ 29` without trusting the search program's BFS conclusion.

## New elementary ledgers (all replayable with the official verifier)

- P1 ~ φ₁(C6): 461 moves, peak 49 (φ₁: x ↦ x⁻¹).
- C6 ~ φ₁(C6): 1920 moves, peak 43.
These are new AC-equivalences found by expanding a cap-33 quotient path and a `--nosym` cap-30 path respectively. They do
**not** improve the shortest known P1~C6 ledger (1066 moves, peak 41), and P1 ~ φ₁(C6) is not by itself a certificate for
P1 ~ C6: the φ-quotient of the search means the 48-hop chain ends in the φ₁-orbit of C6.

## Limitations

- All statements are model-relative (SAIR 14 moves through the capped quotient model and its published completeness
  condition with `m = 14`). They do not resolve the general or stable conjecture, nor the Shehper et al. claims.
- The cap-33 enumerations were produced by the third-party ms14 engine (Apache-2.0, AI-generated, not peer reviewed) and
  replicated by the independent implementation shipped here. The completeness argument is documented but not
  formalized in a proof assistant.
- Novelty is relative to the public record as of 2026-11-12; see `submission/NOVELTY_SCAN.md`.

## Reproduction

All commands, environment, hashes and logs are in `BUILD.md`, `MANIFEST.sha256` and `evidence/`. `code/verify_package.py`
re-checks the whole package (checksums, log values, certificates under both verifiers, closure certificate, crosschecks).

## References

1. ACC-P000012, "ac-02089 is AC-equivalent to C6; no AC path within total length 32 from ac-02089/ac-08126 to the trivial
   presentation or AK(3)", SAIR ACC Proof Track, 2026-09-30 (Alexander Keur); repository
   `alejandrozarco/andrews-curtis-ms14` (Apache-2.0; Zenodo 10.5281/zenodo.23196602, concept 10.5281/zenodo.23196601).
2. `alejandrozarco/andrews-curtis-sair-census` (2026-11-10; Zenodo 10.5281/zenodo.24024379; Apache-2.0).
3. `nahomar/andrews-curtis-solver` (MIT): residual length-14 classes and rotation-only cap-28 component sizes.
4. `SAIRcompetition/Andrews-Curtis` (Apache-2.0): problems, move spec `ac-r2-v1`, official verifier (commit `a0fd6e6`).
5. D. Carreras, arXiv:2607.23611; A. Shehper et al., arXiv:2408.15332.
