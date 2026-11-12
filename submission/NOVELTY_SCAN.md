# Novelty scan record

**Scan date:** 2026-11-12 (last update 2026-11-12 ~18:40 SGT). Re-run immediately before submitting
(`python3 code/check_novelty.py`).

## Method

1. **SAIR public API — all public ACC Proof Track submissions** (authoritative public record):
   `GET https://api.sair.foundation/api/competitions/acc/contributor-network`
   (OpenAPI: `https://api.sair.foundation/openapi.json`; site: <https://competition.sair.foundation/contibutor-network>).
   Extracted titles, authors, dates (`publishedAt`), claim types, completeness, GitHub links and excerpts for every
   item. Raw snapshot: `evidence/sair_acc_contributor_network_raw.json`; parsed list:
   `evidence/sair_acc_proof_submissions_summary.json`.
2. **ms14 repository and Zenodo**: README/commits as of 2026-11-06; Zenodo record 23196602 v2 (2026-10-06). The README
   covers caps 29–32 only; no cap-33 or 94,705,057/235,809,128 values.
3. **SAIR census repository v2** (2026-11-10 snapshot; Zenodo 24024379): `results/bounds.csv` has `ac-02089`
   bound 30 (cap 29), `ac-08126` bound 29 (cap 28), `ac-00399` bound 27 (cap 26) — all below `>= 34`.
4. **arXiv API** (`all:"Andrews-Curtis"`, sorted by submission date): latest relevant entries 2026-09-24 (SAGE, ML),
   2026-08-23, 2026-07-26 (Carreras), 2026-06-19, 2026-06-04. No post-2026-09-24 AC papers; no cap-33/≥34 claim.
5. **GitHub repository search** (`Andrews-Curtis conjecture`, sorted by update): active repos (pragyaangaur, nahomar,
   munimtechnologies/ac-discovery, Amey-Thakur, ph4ge) are Discovery-Track searches, length-14 censuses or Lean
   formalisations; no cap-33 P1/P2 component record found.
6. **Zenodo search** for "Andrews-Curtis": latest relevant record 2026-09-30 supplementary material; no new bound record.

## Result

**23 public ACC Proof Track submissions** as of 2026-11-12. **None** reports a complete cap-33 component for
`ac-02089`/`ac-08126`, the values 94,705,057 / 235,809,128, or the bound `>= 34`.

Live re-check (2026-11-12 ~21:00 SGT, `python3 code/check_novelty.py`, live SAIR API with a User-Agent header):
23 submissions, 0 keyword hits, closest prior submission still ACC-P000012 (2026-09-30). Re-run the script on the
submission day; if the live query is unavailable the script falls back to the bundled snapshot
`evidence/sair_acc_contributor_network_raw.json` and says so.

Closest public predecessor: **ACC-P000012** (published 2026-09-30; author Alexander Keur) —
*"ac-02089 is AC-equivalent to C6; no AC path within total length 32 from ac-02089/ac-08126 to the trivial presentation
or AK(3)"*, direction `disproof`, completeness `partial`. Its own text labels the capped enumerations
"computational, not independently verified". Our package extends it by one cap (both starts) and adds the independent
replication plus a machine-checkable closure certificate.

Other related public submissions (context, none overlapping the cap-33 claims): ACC-P000023 (Lean-certified AC-classes,
47 AK(3)-equivalent instances), ACC-P000021 (length-14 census / cap-25 box / AC-19 peaks), ACC-P000010 (certified peak
lower bounds on some scored rows), ACC-P000024 (AK(3) to a fixed pair under cap 22), ACC-P000013/ACC-P000018
(Miller–Schupp families), ACC-P000008 (detour width).

## Statement to use

> "To the best of our knowledge, based on the public SAIR ACC Proof Track submissions (23 as of 2026-11-12), the ms14
> repository and its Zenodo record, the SAIR census v2, arXiv, GitHub and Zenodo, no public artifact reports the complete
> cap-33 component of ac-02089 or ac-08126, nor the bound ≥ 34. This submission extends ACC-P000012 (2026-09-30) by one
> cap for both starts and independently reproduces its published counts. It remains a partial computational result and
> does not prove or disprove Andrews–Curtis."

## Caveats

- Public-record statement only; cannot exclude private/unpublished work.
- The ms14 authors (who submitted ACC-P000012) could extend their own computation at any time; check again just before
  submitting.
- The SAIR platform may publish new Proof Track submissions after this scan; `code/check_novelty.py` re-runs the check.
