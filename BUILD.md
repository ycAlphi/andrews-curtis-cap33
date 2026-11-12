# Build and reproduction instructions

Environment used: **Windows 11**, `g++ (GCC) 16.2.0` from w64devkit v2.11.0, **Python 3.11.0**, 20 logical cores,
15.7 GB RAM. The build is deterministic: the four compiled binaries used in the runs are
```
acsearch.exe       sha256 14390ab5a85e23778e4ccb927f3cb15b961e8a5013d11aca421f01aa175241b9
acsearch_lean.exe  sha256 62f911694f57848ed41010ace7fbb80575cadc6f7a7f051606c03fb82ec1c3c0
ms14_engine.exe    sha256 c33897034b4e3fd3619906140761cd2c1ff4b6cfb41774bdc15b8a6ad56780dd
closure_check.exe  sha256 924712d790b3a95828631c155227ca5b793ce937d492d5070b92bf974f8c5710
```
A rebuild of `ms14_engine.exe` from source with the command below was bit-identical to the binary used in the runs.

## 1. Build

```bash
cd submission_package
mkdir -p bin
g++ -O3 -std=c++17 -pthread -march=native -o bin/acsearch_lean third_party/ms14/acsearch_lean.cpp
g++ -O3 -std=c++17 -pthread -march=native -o bin/acsearch      third_party/ms14/acsearch.cpp
g++ -O3 -std=c++17 -pthread -march=native -o bin/ms14_engine    code/ms14_engine.cpp
g++ -O3 -std=c++17 -pthread -march=native -o bin/closure_check  code/closure_check.cpp
```
(On Windows we used w64devkit; any C++17 compiler with `<thread>` support should work.)

## 2. The two cap-33 runs (the main results)

Target order (indices 0–6): 0 = AK(3) `xyxYXY|xxxxYYY`, 1 = P1 `xxxyXXY|xyyXYYY`, 2 = P2 `xxxyXXy|xyyXYYY`,
3 = trivial `x|y`, 4 = C3 `xxyXy|xyyyyyxYY`, 5 = C4 `xxxxyXy|xyyyxYY`, 6 = C6 `xxyxxYY|xyXYYXy`.

**P1** (`ac-02089`, words `XyyxYYY|XXXYxxy`) — produced 2026-11-07 with the ms14 lean engine, 12 threads:
```bash
mkdir -p out
bin/acsearch_lean --start "XyyxYYY|XXXYxxy" --cap 33 --conj 9 --threads 12 --maxstates 160000000 --all-hits \
  --target "xyxYXY|xxxxYYY" --target "xxxyXXY|xyyXYYY" --target "xxxyXXy|xyyXYYY" --target "x|y" \
  --target "xxyXy|xyyyyyxYY" --target "xxxxyXy|xyyyxYY" --target "xxyxxYY|xyXYYXy" \
  --out out/P1_c33 > evidence/acsearch_P1_c33_allhits.log 2>&1
```
Expected: `states=94705057 levels=448 complete=1 hit=1 overflow=0` and one `HIT target 6` line.
Independent replication:
```bash
bin/ms14_engine --start "XyyxYYY|XXXYxxy" --cap 33 --conj 9 --threads 10 --maxstates 105000000 \
  --target "xyxYXY|xxxxYYY" --target "xxxyXXY|xyyXYYY" --target "xxxyXXy|xyyXYYY" --target "x|y" \
  --target "xxyXy|xyyyyyxYY" --target "xxxxyXy|xyyyxYY" --target "xxyxxYY|xyXYYXy" \
  > evidence/ms14ind_P1_c33_rerun.log 2>&1
```
Expected the same numbers (a `HIT target 6` line and no other hits).

**P2** (`ac-08126`, words `XyyxYYY|XXXYxxY`) — produced 2026-11-12 with the ms14 lean engine, 16 threads:
```bash
bin/acsearch_lean --start "XyyxYYY|XXXYxxY" --cap 33 --conj 9 --threads 16 --maxstates 260000000 --all-hits \
  --target "xyxYXY|xxxxYYY" --target "xxxyXXY|xyyXYYY" --target "xxxyXXy|xyyXYYY" --target "x|y" \
  --target "xxyXy|xyyyyyxYY" --target "xxxxyXy|xyyyxYY" --target "xxyxxYY|xyXYYXy" \
  --out out/P2_c33 > evidence/P2_c33_allhits.log 2>&1
```
Expected: `states=235809128 levels=457 complete=1 hit=0 overflow=0`, no `HIT` lines.
Independent replication:
```bash
bin/ms14_engine --start "XyyxYYY|XXXYxxY" --cap 33 --conj 9 --threads 16 --maxstates 250000000 \
  --target "xyxYXY|xxxxYYY" --target "xxxyXXY|xyyXYYY" --target "xxxyXXy|xyyXYYY" --target "x|y" \
  --target "xxyXy|xyyyyyxYY" --target "xxxxyXy|xyyyxYY" --target "xxyxxYY|xyXYYXy" \
  > evidence/ms14ind_P2_c33.log 2>&1
```
Expected the same numbers, no hits. Resources: P1 run ~21 min and ~2.5 GB; P2 run ~40 min and ~4.2 GB.

> **Windows hint:** redirect native programs with `cmd /c "... > log 2>&1"` (or `2>` to a file) rather than PowerShell's
> `*>`; PowerShell's error-record formatting can wrap long stderr lines. If a histogram line is wrapped, join the lines
> after the `states by total length:` marker before parsing (the shipped logs were handled this way).

## 3. Model cross-checks (ms14 Q/E)

The non-lean engine is used to dump the small-cap components (note `--dump` requires `--out`):
```bash
for start in "XyyxYYY|XXXYxxy" "XyyxYYY|XXXYxxY"; do
  for cap in 24 20; do
    if [ "$cap" = 24 ]; then K=8; else K=8; fi
    bin/acsearch --start "$start" --cap $cap --conj $K --threads 4 --maxstates 3000000 --target "x|y" \
      --out "out/$(echo $start | tr '|' '_')_c$cap" --dump > "out/$(echo $start | tr '|' '_')_c$cap.log" 2>&1
  done
done
# crosscheck.py expects a plain-text sibling log `<dump_stem>.log` containing "RESULT ... complete=1";
# convert the Windows UTF-16 logs if necessary.
SAIR_REPO="$PWD/third_party/sair-verifier" python3 code/crosscheck.py Q "XyyxYYY|XXXYxxy" 24 out/XyyxYYY_XXXYxxy_c24.keys
SAIR_REPO="$PWD/third_party/sair-verifier" python3 code/crosscheck.py Q "XyyxYYY|XXXYxxY" 24 out/XyyxYYY_XXXYxxY_c24.keys
SAIR_REPO="$PWD/third_party/sair-verifier" python3 code/crosscheck.py E "XyyxYYY|XXXYxxy" 20 out/XyyxYYY_XXXYxxy_c20.keys
SAIR_REPO="$PWD/third_party/sair-verifier" python3 code/crosscheck.py E "XyyxYYY|XXXYxxY" 20 out/XyyxYYY_XXXYxxY_c20.keys
```
Expected (as recorded in the ms14 crosscheck and reproduced here): Q P1 17,718 classes (0 difference); Q P2 21,932
(0 difference); E P1 1,219,064 exact states -> 473 classes (all inside); E P2 696,024 -> 521 classes (all inside).

## 4. Closure certificate for AK(3) cap 28

```bash
# regenerate the key file (8,694,374 classes, 139,109,984 bytes; SHA-256 in evidence/keys_AK3_c28.sha256)
bin/ms14_engine --start "xxxYYYY|xyxYXY" --cap 28 --conj 7 --threads 4 --maxstates 12000000 \
  --target "x|y" --dump out/AK3_c28.keys > evidence/AK3_c28_dump.log 2>&1
bin/closure_check --keys out/AK3_c28.keys --cap 28 --conj 7 --start "xxxYYYY|xyxYXY" \
  --target "x|y" --expect 8694374 --threads 6
```
Expected output ends with `closure: checked 238806300 moves from 8694374 states; violations: 0` and
`VERDICT: PASS`. The shipped `evidence/closure_AK3_c28.log` is the log from this command.

## 5. Certificates

```bash
# independent ledger verifier (ms14, vendored)
PYTHONPATH=third_party/ms14 python3 -c "
import json,verify as V,glob
for f in ['certs/P1_equiv_C6.json','certs/P1_phi1_C6_461.json','certs/C6_phi1_C6_1920.json']+sorted(glob.glob('certs/phi/*.json')):
    cert=json.load(open(f)); ok,msg,st=V.verify(cert); print(f, ok, msg, st and {k:st[k] for k in ('moves','classical','peak_total')})
"
# official SAIR 14-move semantics (vendored verifier package)
python3 code/sair_replay.py certs third_party/sair-verifier/competition/tools
```
Expected: every certificate PASS. The replayer converts the ledger moves to SAIR move ids 0–13 and compares with an
independent re-implementation step by step.

## 6. Verify the whole package

```bash
python3 code/verify_package.py     # -> "PACKAGE VERIFICATION: PASS"
```
