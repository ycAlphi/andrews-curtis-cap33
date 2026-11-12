# SAIR ACC Proof Track — submission fields (draft, not submitted)

**Deadline (official):** `2026-12-01T11:59:59Z` = **2026-11-30 AoE**. Submission is via the SAIR competition
platform: <https://competition.sair.foundation/competitions/acc> (Proof Track page:
`/competitions/acc/proof-track`). Rules: <https://github.com/SAIRcompetition/Andrews-Curtis/blob/main/competition/rules/proof.md>.

> The ACC competition record in the SAIR public API (`GET https://api.sair.foundation/api/competitions/acc`) shows
> `"submissionMaxEntries": 1` and `"submissionDeadline": "2026-12-01T11:59:59Z"`. Prepare everything before submitting.

## Form fields

| Field | Value to enter |
|---|---|
| **Conjecture** | **AC** (this work is about the ordinary 14-move AC model; not Stable AC) |
| **Direction** | **disproof** — the submission is a partial result about the difficulty of AC-trivialising/identifying two residual length-14 presentations; the description explicitly says no disproof is claimed. (If preferred, `proof` is equally defensible; the field only indicates the working direction.) |
| **Description** | Paste `submission/description_short.md` (or the longer `submission/description_full.md` if the form allows). |
| **Public GitHub repository link** | `<YOUR PUBLIC GITHUB REPO URL>` — push the contents of this package, e.g. `https://github.com/<user>/<repo>` |
| **Git commit hash** | Run `git rev-parse HEAD` in the pushed repository (or copy from the GitHub commit page). Paste the **full 40-hex** hash. It must match the pushed commit exactly. |
| **arXiv or paper link** | Optional. Suggested: the GitHub `report/REPORT.md` (as a paper-like link) or a Zenodo DOI if you archive the package. **arXiv is not required and is not recommended for this partial result.** |
| **Sharing agreement** | Must be checked. Submission is automatically published in the SAIR Contributor Network with a timestamp and version history; it does not certify correctness. |

## Copy-paste description (short, for the form)

See `submission/description_short.md`. The longer version is `submission/description_full.md`.

## Before you submit (checklist)

- [ ] **Final novelty scan** (run it the same day): `python3 code/check_novelty.py` — checks the public SAIR Proof Track
      submissions for cap-33 / `94,705,057` / `235,809,128` / `>= 34` mentions. If anything appears, re-read it and adjust
      the novelty statement (the package's claim is "no public record as of the scan date").
- [ ] Re-run `python3 code/verify_package.py` → must print `PACKAGE VERIFICATION: PASS`.
- [ ] Create the public GitHub repository (suggested name `andrews-curtis-cap33`), push this exact content.
      Do **not** rewrite history after noting the commit hash; if you must, re-record the hash.
- [ ] Run `git rev-parse HEAD` and paste the full hash into the submission form.
- [ ] (Optional) Archive the package on Zenodo (DOI) and put the DOI in the "arXiv or paper link" field.
- [ ] Register / log in to SAIR, register for the ACC competition (individual or team), accept the competition terms.
- [ ] Paste the description; select `AC` + `disproof` (+ `partial` if the form asks for claim completeness).
- [ ] Check the sharing agreement box; submit; **save the receipt / Contributor Network item ID and time** in
      `submission/SUBMISSION_RECEIPT.md` (create it after submitting) and in the run's `SESSION.json`.
- [ ] Post-submission: do not silently change the repository; use a new commit + a new submission version if needed.

## After submitting (record here)

```
date/time:
SAIR submission id:
Contributor Network public code (ACC-PXXXXXX):
GitHub repo URL:
commit hash submitted:
notes:
```

## Notes on the claim wording (important)

- This is a **partial computational result**. It does **not** prove or disprove AC; it does not prove P1/P2 are not
  AC-equivalent to AK(3).
- The novelty statement is relative to the public record (see `submission/NOVELTY_SCAN.md`): the closest public
  submission is **ACC-P000012** (2026-09-30, Alexander Keur; the ms14 project), which states the bound `>= 33` and
  labels its enumerations "computational, not independently verified". Our package adds independent replication and one
  more cap; cite ACC-P000012 and the ms14 repository explicitly.
- All third-party code/data are used under Apache-2.0 (ms14, SAIR verifier) or MIT (nahomar) — see
  `THIRD_PARTY_NOTICES.md`. Disclose AI assistance as in `AI_DISCLOSURE.md` (this follows the ms14 precedent).
