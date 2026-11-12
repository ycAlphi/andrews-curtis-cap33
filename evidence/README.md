# Evidence directory

All logs here were produced by the runs described in `../BUILD.md`; SHA-256 of every file is in `../MANIFEST.sha256`.
File names containing `ms14ind_` are outputs of our independent implementation (`code/ms14_engine.cpp`); files
`acsearch_*` are outputs of the third-party ms14 lean engine; `crosscheck_*` are the model cross-checks; `sair_*` are
official-SAIR-semantics replays.

## Main cap-33 runs (the results)

| file | what it is | key values |
|---|---|---|
| `acsearch_P1_c33_allhits.log` | P1 = ac-02089, cap 33, K=9, `--all-hits`, ms14 lean engine, 12 threads (2026-11-07) | `states=94,705,057`, `levels=448`, `complete=1`, `hit=1` (only target 6 = C6 at level 48), `overflow=0`, 1278 s |
| `ms14ind_P1_c33_rerun.log` | independent replication of the P1 run (our engine, 10 threads, 2026-11-08) | identical states/levels/complete/histogram; only C6 hit |
| `P2_c33_allhits.log` | P2 = ac-08126, cap 33, K=9, `--all-hits`, ms14 lean engine, 16 threads (2026-11-12) | `states=235,809,128`, `levels=457`, `complete=1`, `hit=0`, `overflow=0`, 2429 s |
| `ms14ind_P2_c33.log` | independent replication of the P2 run (our engine, 16 threads, 2026-11-12) | identical states/levels/complete/histogram, no hits (3063 s) |
| `P2_c33_summary.json` | parsed P2 result plus full length histogram (sum = 235,809,128) | — |
| `P2_c33_replication_verification.json` | programmatic comparison of the P2 run and its replication; `verification_pass: true` | — |
| `acsearch_P1_c33_lean.log` | first P1 cap-33 attempt *without* `--all-hits`: it stopped at the first hit (C6) after 58,116,954 states and is **not** a completeness result. Kept for transparency. | — |

## Reproduction of published counts

| file | what it is |
|---|---|
| `ms14ind_P1_c24.log`, `..._c29/c30/c31/c32.log` | our engine reproducing the ms14 P1 capped counts (17,718 / 2,267,706 / 5,349,664 / 15,165,074 / 32,699,098) |
| `ms14ind_P2_c24.log`, `..._c29/c30/c31/c32.log` | our engine reproducing the ms14 P2 capped counts (21,932 / 6,331,998 / 11,889,832 / 39,047,968 / 71,499,215) |
| `ms14ind_validation_results.txt` | summary of all those runs |
| `rotation_only_benchmark.txt`, `nahomar_rotationonly_replication.txt` | our reproduction of the six nahomar rotation-only cap-28 counts (P1 680,700; P2 1,880,041; AK(3) 8,650,489; C3 5,981,990; C4 108,455; C6 100,703) |

## Model cross-checks (Q/E), re-run independently

| file | expected content |
|---|---|
| `crosscheck_results.txt` | one line per test with the verdict and wall time |
| `crosscheck_Q_P1_c24.log` | `own classes 17718, acsearch classes 17718, ... 0, ... 0` |
| `crosscheck_Q_P2_c24.log` | `own classes 21932, acsearch classes 21932, ... 0, ... 0` |
| `crosscheck_E_P1_c20.log` | `exact states 1219064, empty-relator states 0, projected classes 473, acsearch classes 473, projected-not-in-acsearch 0` |
| `crosscheck_E_P2_c20.log` | `exact states 696024, ... projected classes 521, acsearch classes 521, ... 0` |

## Certificates and their replay

| file | what it is |
|---|---|
| `sair_official_replay.log` | official SAIR 14-move replay of the ms14 certificates (P1~C6, φ bridges) — all PASS |
| `sair_official_replay_new_certs.log` | official SAIR replay of the two new ledgers (P1~φ₁(C6) 461 moves, C6~φ₁(C6) 1920 moves) — PASS |
| `P1_C6_c30_regenerated.json` | the ms14 1066-move P1~C6 ledger regenerated from its path file (byte-identical move list) |
| `P1_c33_C6path.log`, `P1_c33_C6path.paths` | the 48-hop quotient path to the C6 class found by the cap-33 search (peak 33) |
| `C6_PATH_RECONSTRUCTION.md` | record of the C6-path analysis: why its expansion ends in the φ₁-orbit of C6 and not at C6 |

## Closure certificate (AK(3), cap 28)

| file | what it is |
|---|---|
| `closure_AK3_c28.log` | independent closure checker on 8,694,374 classes: 238,806,300 moves re-generated, 0 violations, start present, trivial presentation absent ⇒ `B(AK3) ≥ 29` |
| `closure_AK3_c24.log` | small demonstration (199,782 classes, 4,421,502 moves, 0 violations) |
| `AK3_c27.log`, `AK3_c28.log`, `ak3_component_ladder.txt` | the underlying AK(3) component enumerations |
| `keys_AK3_c28.sha256` | SHA-256 (and size) of the 139 MB `AK3_c28.keys` file, which is **not** shipped here; regenerate with `BUILD.md` §4 |

## Novelty-scan record

| file | what it is |
|---|---|
| `sair_acc_contributor_network_raw.json` | raw snapshot of the SAIR public API list of ACC Proof Track submissions (2026-11-12) |
| `sair_acc_proof_submissions_summary.json` | parsed list (23 submissions: codes, dates, authors, titles, claim types, excerpts) |

## Notes

- Log lines from native programs may contain line breaks inserted by terminal/redirect buffering; the histogram parser in
  `code/verify_package.py` joins lines after `states by total length:` before parsing (the numbers are unaffected).
- `P2_c33_allhits2.paths` (empty, 0 bytes) was produced with `--out` for the P2 run; it is empty because no target was
  hit. It is not included here.
