#!/usr/bin/env python3
"""Re-run the public-record novelty check against the SAIR ACC Proof Track submissions.

Usage:  python3 code/check_novelty.py
Exit code 0 on success (regardless of hits); prints a warning if any public submission mentions values close to ours.
Run this on the day of submission and update submission/NOVELTY_SCAN.md if anything changed.
"""
import json
import re
import sys
import urllib.request
from pathlib import Path

API = "https://api.sair.foundation/api/competitions/acc/contributor-network"
ROOT = Path(__file__).resolve().parent.parent
SNAPSHOT = ROOT / "evidence" / "sair_acc_contributor_network_raw.json"

KEYWORDS = [
    "cap 33", "cap-33", "cap33", "length 33", "peak 33",
    "94705057", "94,705,057", "235809128", "235,809,128",
    "length 34", "peak 34", ">=34", ">= 34", "≥34", ">= 34",
    "capped component", "component enumeration",
]


def load_submissions():
    try:
        req = urllib.request.Request(API, headers={"User-Agent": "acc-submission-package/1.0"})
        with urllib.request.urlopen(req, timeout=60) as fh:
            data = json.load(fh)
        return data, "live SAIR API"
    except Exception as exc:  # offline / rate-limited: fall back to the bundled snapshot
        if SNAPSHOT.exists():
            data = json.loads(SNAPSHOT.read_text(encoding="utf-8-sig"))
            return data, "bundled snapshot 2026-11-12 (live query failed: %r)" % (exc,)
        raise


def main():
    print("novelty source:", end=" ")
    data, source = load_submissions()
    print(source)
    items = data.get("data", [])
    print("public ACC Proof Track submissions:", len(items))
    hits = []
    for it in items:
        blob = " ".join(str(it.get(k, "")) for k in
                        ("publicCode", "artifactTitle", "artifactExcerpt", "cheatsheetTitle")).lower()
        for kw in KEYWORDS:
            if kw.lower() in blob:
                hits.append({"code": it.get("publicCode"), "date": it.get("publishedAt"),
                             "author": it.get("entityName"), "keyword": kw,
                             "title": it.get("artifactTitle")})
    print("keyword hits:", len(hits))
    for h in hits[:20]:
        print("  -", h)
    for it in items:
        if it.get("publicCode") == "ACC-P000012":
            print("closest prior submission ACC-P000012:", it.get("publishedAt"), "|", it.get("artifactTitle"))
    latest = sorted(items, key=lambda x: x.get("publishedAt") or "")[-3:]
    print("latest three public submissions:")
    for it in latest:
        print("  -", it.get("publicCode"), (it.get("publishedAt") or "")[:10], "|", it.get("artifactTitle"))
    if hits:
        print()
        print("WARNING: some public submissions mention values related to this package.")
        print("Re-read them before claiming the novelty statement in submission/NOVELTY_SCAN.md.")
    else:
        print()
        print("No public submission mentions the cap-33 values/bounds used here.")
        print("Record the date/time of this check in submission/NOVELTY_SCAN.md before submitting.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
