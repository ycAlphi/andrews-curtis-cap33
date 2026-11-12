# Problems, relators and targets

## Presentations

Words over `x, X=x⁻¹, y, Y=y⁻¹` (the search conventions of the ms14 engine and of the official SAIR verifier use the
integer encoding x=1, X=-1, y=2, Y=-2). "Canonical key" below is the ms14 canonical string form used in the logs.

| name | SAIR id | mathematical presentation | relator words | ms14 canonical key |
|---|---|---|---|---|
| **P1** | `ac-02089` | MS(2, x⁻²y⁻¹x²y) | `XyyxYYY`, `XXXYxxy` | `xxxyXXY\|xyyXYYY` |
| **P2** | `ac-08126` | MS(2, x⁻²y⁻¹x²y⁻¹) | `XyyxYYY`, `XXXYxxY` | `xxxyXXy\|xyyXYYY` |
| **AK(3)** | `ac-00399` | `<x,y \| x³=y⁴, xyx=yxy>` | `xxxYYYY`, `xyxYXY` | `xyxYXY\|xxxxYYY` (φ-orbit canonical) |
| trivial | — | `<x,y \| x, y>` | `x`, `y` | `x\|y` |
| C3 | — | residual class (nahomar) | `xxyXy`, `xyyyyyxYY` | — |
| C4 | — | residual class (nahomar) | `xxxxyXy`, `xyyyxYY` | — |
| C6 | — | residual class (nahomar) | `xxyxxYY`, `xyXYYXy` | — |

Note on AK(3): the canonical key printed by the ms14 engine for `xxxYYYY|xyxYXY` is `xyxYXY|xxxxYYY` because the
canonical form takes the minimum over the 8 signed letter permutations and rotations/inversions; it is the same class.

## Target list (order used in the runs)

The engine is given `--target` arguments in this order; hit messages report the index.

| index | target | canonical key |
|---|---|---|
| 0 | AK(3) = ac-00399 | `xyxYXY\|xxxxYYY` |
| 1 | P1 = ac-02089 | `xxxyXXY\|xyyXYYY` |
| 2 | P2 = ac-08126 | `xxxyXXy\|xyyXYYY` |
| 3 | trivial presentation | `x\|y` |
| 4 | C3 | `xxyXy\|xyyyyyxYY` |
| 5 | C4 | `xxxxyXy\|xyyyxYY` |
| 6 | C6 | `xxyxxYY\|xyXYYXy` |

## Reading the logs

Each run log ends with
```
RESULT start=... cap=33 conj=9 nosym=0 states=N levels=L complete=1 hit=H overflow=0 time=Ts
states by total length: 14:... 33:...
```
`complete=1` means the capped component was exhausted; `hit` counts target classes met; the histogram counts classes by
total relator length. `evidence/acsearch_P1_c33_allhits.log` also contains one `HIT target 6 ... at level 48` line
(the known P1~C6 link); the P2 log contains no `HIT` line.

## Machine-readable data

`data/targets.txt` contains the seven target strings in order (one per line).
