# Third-party notices

This package contains unmodified third-party code and data. All third-party material is used under the licenses below;
we claim no copyright over it. Our own code, text and generated certificates are under the Apache License 2.0
(see `LICENSE`).

## 1. ms14 repository — search engine, ledger tools, certificates

- Project: **alejandrozarco/andrews-curtis-ms14** — "Andrews–Curtis, two length-14 Miller–Schupp presentations:
  an AC-equivalence certificate and capped exhaustive searches (unreviewed, AI-produced)".
- Locations: https://github.com/alejandrozarco/andrews-curtis-ms14 ;
  Zenodo, version v2 (2026-10-06), DOI `10.5281/zenodo.23196602` (concept DOI `10.5281/zenodo.23196601`).
- License: **Apache License 2.0** (stated in the repository).
- Files copied **unmodified** into this package:
  - `third_party/ms14/acsearch.cpp`, `third_party/ms14/acsearch_lean.cpp` — capped-component search engines;
  - `third_party/ms14/make_cert.py`, `third_party/ms14/verify.py` — path-to-ledger expansion and independent ledger verifier;
  - `certs/P1_equiv_C6.json`, `certs/phi/phi*_ak3_to_ak3.json` — certificates from the repository.
- We additionally **use** its search-model specification, its published capped-component counts (caps 29–32) and its
  path files for validation/reproduction. Its P1 cap-33 log (an aborted K=8 run) is not part of the published results;
  our cap-33 runs were produced with its `acsearch_lean.cpp` at `K = 9` and are shipped here.
- Provenance note: the copy was fetched from the GitHub repository on 2026-11-07; file SHA-256s are in
  `MANIFEST.sha256`. The repository's related SAIR Proof Track submission is **ACC-P000012** (2026-09-30, Alexander Keur),
  which states the result `>= 33` and labels the enumerations "computational, not independently verified".

## 2. SAIR ACC reference implementation — official move semantics

- Project: **SAIRcompetition/Andrews-Curtis** — competition rules, problems, and reference verifier.
- Location: https://github.com/SAIRcompetition/Andrews-Curtis
- Commit used: `a0fd6e6` (recorded in the ms14 cross-check README as the version used for its E test; same file set).
- License: **Apache License 2.0** (stated in the repository).
- Files copied **unmodified** into `third_party/sair-verifier/competition/tools/verifier/`:
  `__init__.py`, `core.py`, `canon.py`, `specs.py`, `cli.py`, `stable_core.py`, `submission.py`, and the data files
  `data/move_spec.json`, `data/stable_move_spec.json`.
- Used to replay every certificate in `certs/` under the official 14-move semantics (`code/sair_replay.py`), and by the
  E model cross-check.

## 3. nahomar/andrews-curtis-solver

- Location: https://github.com/nahomar/andrews-curtis-solver ; License: **MIT**.
- No files copied. Used for: the residual length-14 classes (C3, C4, C6) and the published rotation-only cap-28
  component sizes (P1 680,700; P2 1,880,041; AK(3) 8,650,489; C3 5,981,990; C4 108,455; C6 100,703), which our
  implementation reproduces (see `evidence/rotation_only_benchmark.txt`, `evidence/nahomar_rotationonly_replication.txt`).

## 4. SAIR census repository

- Project: **alejandrozarco/andrews-curtis-sair-census** (2026-11-10 snapshot; Zenodo `10.5281/zenodo.24024379`;
  Apache-2.0). No files copied. Its published bounds are cited for context: `ac-02089` bound 30 (cap 29), `ac-08126`
  bound 29 (cap 28), `ac-00399` bound 27 (cap 26) — all weaker than the cap-33 results in this package.

## 5. Literature cited (no files copied)

- D. Carreras, *Machine-checkable equivalence certificates at the length-14 Andrews–Curtis frontier*, arXiv:2607.23611.
- A. Shehper et al., *What makes math problems hard for reinforcement learning: a case study*, arXiv:2408.15332.
- SAIR Foundation / Caltech ACC Challenge: https://competition.sair.foundation/competitions/acc (rules under
  https://github.com/SAIRcompetition/Andrews-Curtis/tree/main/competition/rules).

## 6. Data and media included

All run logs in `evidence/` were produced by us (2026-11-07/08 and 2026-11-12) using the binaries recorded in
`evidence/build_env.txt`. The large closure-certificate key file `AK3_c28.keys` (139,109,984 bytes) is **not**
included; its SHA-256 and regeneration command are in `evidence/keys_AK3_c28.sha256` and `BUILD.md`.
