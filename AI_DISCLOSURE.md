# AI disclosure

This package documents computational research produced with substantial AI assistance. Following the practice of the
ms14 repository that this work verifies and extends, we disclose the following.

## Generation

- The independent search implementation (`code/ms14_engine.cpp`), the closure-certificate checker
  (`code/closure_check.cpp`), the certificate replayer (`code/sair_replay.py`), the package verification script
  (`code/verify_package.py`), the packaging scripts, and all prose in this repository were produced by an AI agent
  (DeepSeek Harness, model `deepseek-flash`) in sessions on 2026-11-07/08 and 2026-11-12, under the direction of the
  human user operating the agent.
- The main cap-33 enumerations were executed with the **third-party** ms14 search engine
  (`third_party/ms14/acsearch_lean.cpp`, Apache-2.0), which is itself AI-generated and explicitly not peer reviewed.
  The ms14 project is cited and its files are included unmodified.
- The official SAIR verifier code (`third_party/sair-verifier/`) is a third-party artifact of the SAIR Foundation /
  Caltech ACC Challenge and is included unmodified for replay.

## What is and is not verified

- The computational claims in this package are reproducible: every log has a SHA-256 in `MANIFEST.sha256`, the exact
  commands and environment are in `BUILD.md`, and `code/verify_package.py` re-checks the logs, replays the certificates
  and re-runs the closure-certificate check.
- The central enumeration results (P1 and P2 cap-33 components) were each produced twice, by two independently written
  implementations (third-party ms14 engine and our own engine), with identical state counts, level counts, complete
  flags, length histograms and hit sets.
- The mathematical inference from an exhausted capped component to a peak lower bound relies on the
  projection/completeness argument documented in the ms14 repository (with the published condition
  `K >= floor((L-m)/2)`). That argument is **not formalized** in a proof assistant here. We independently re-ran the
  ms14 Q/E small-cap cross-checks of the model (passing, with numbers matching the published ones), but a formal proof
  of the completeness lemma remains future work.
- No claim in this package has been peer reviewed. Nothing here proves or disproves the Andrews–Curtis conjecture or
  Stable AC.

## Human responsibility

The human submitter is responsible for the submission, for the accuracy of the cited prior work, and for any use of
this material. The AI agent had no ability to submit, publish, or contact third parties; the package was prepared
locally and is submitted only if and when a human does so.
