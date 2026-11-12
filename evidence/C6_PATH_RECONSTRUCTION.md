# C6 level-48 路径重建记录（续算第一优先级）

**目的**：P1 cap-33 `--all-hits` 枚举在 **level 48** 首次命中 C6（已知的 P1~C6 链接）。用户建议：重建该命中路径，检查是否能得到比 ms14 现有 **1066 步 / peak 41** 更短的 P1~C6 证书。

**结论摘要**：
1. 路径成功重建：**48 个商图移动（49 个状态），路径峰值 33**。
2. 展开为显式初等 AC 账本后：**461 步、peak_total 49、peak_single 33**，但它证明的是 **P1 ~ φ₁(C6)**（x→x⁻¹ 的带号生成元置换像），不是 P1 ~ C6——因为商图按 8 个 φ 取商，链的端点落在 φ₁/φ₂ 轨道，且**最后一个 hop 的全部 7 种实现都固定落在 φ₁/φ₂ 轨道（无 p=0 实现）**。
3. 因此该路线**没有**得到更短的直接 P1~C6 证书；但额外发现并机器验证了 **C6 ~ φ₁(C6) 的 AC 等价**：
   - 商图路径：`--nosym`（不取 φ 商）搜索 `C6 → φ₁(C6)`，cap=30、conj=8 下 **level 190 命中**（37,312,600 状态，peak 30）；
   - 展开账本：**1920 步、peak_total 43、peak_single 30**；
   - 该桥与 461 步证书复合可得**直接的 P1~C6 账本（2381 步）**，比现有 1066 步更长，故不构成改进。
4. 对照：ms14 的 1066 步 P1~C6 证书被本 run 用 `make_cert.py --cap 30 --conj 8` 从 `runs/P1_to_C6_c30.paths` **逐字节重生成**（1066/1066 classical、peak 41、PASS），验证了整条证书管线的理解。

**教训（用户设想中需要修正之处）**：`level` 是**商图移动**的 BFS 深度，不是官方初等移动步数；每个商图移动展开成 ~10-20 个初等移动（48 hop → 461 moves）。此外商图按 φ 取商，"命中 C6"实际是"命中 C6 的 φ-轨道"，恢复直接 P1~C6 需要 φ 桥（对 C6 需前述 1920 步桥）。

## 详细步骤与证据

### 1. 路径重建（non-lean 引擎，带 parent 数组）
```
acsearch.exe --start "XyyxYYY|XXXYxxy" --cap 33 --conj 9 --threads 10 --maxstates 70000000 \
  --target "xxyxxYY|xyXYYXy" --out <writable>/P1_c33_C6path
```
结果：
```
HIT target 0 (xxyxxYY|xyXYYXy) at level 48, path 49 states, peak total 33
RESULT start=XyyxYYY|XXXYxxy cap=33 conj=9 nosym=0 states=57747762 levels=48 complete=0 hit=1 overflow=0 time=710s
```
- 首次运行 `--out` 指向 `problem_bank/...` 时**路径文件未写出**：新编译的未签名 exe 的 `fopen` 被系统拒绝，而引擎未检查 `pf==NULL`（`fopen` 失败静默）。改用工作区根目录下的可写路径 `.tools/out/` 重跑后成功（同样 710s），再复制进 `evidence/`。
- 文件：`evidence/P1_c33_C6path.paths`（51 行 = 头 + 49 状态 + `# end`）、`evidence/P1_c33_C6path.log`。

### 2. 展开 + φ 相位问题
```
python make_cert.py --paths evidence/P1_c33_C6path.paths --which 0 \
  --start "XyyxYYY|XXXYxxy" --target "xxyxxYY|xyXYYXy" --cap 33 --conj 9 --out ...
```
输出：`endpoint is in the orbit of phi_1(target)` → 需要 `--suffix-cert`。
- 最后 hop 的 7 种匹配实现全部落在 `(φ₁, φ₂)` 轨道（脚本 `verify last hop`，2026-11-07）：**无 p=0 实现** ⇒ 改变最后 hop 无法消除 φ 相位。
- 改用 **target = φ₁(C6) = "XXyXXYY|XyxYYxy"** 直接产出合法证书：`PASS AC-equivalence {'moves': 461, 'classical': 461, 'peak_total': 49, 'peak_single': 33}`。
- 文件：`evidence/P1_phi1_C6_c33_48hop.json`。

### 3. C6 ~ φ₁(C6) 桥（新发现）
- `--nosym` 搜索（不取 φ 商），start C6 = "xxyxxYY|xyXYYXy"，target φ₁(C6) = "XXyXXYY|XyxYYxy"：
  - cap 26/27/28/29：complete=1、无命中（状态数 38,721 / 95,602 / 204,496 / 504,430；峰值下界 ≥30）；
  - **cap 30：HIT，level 190，states=37,312,600，peak 30**（`evidence/acsearch_P1_c33_allhits` 之外的独立搜索）。
- 展开：`make_cert.py --nosym --cap 30 --conj 8` → `PASS AC-equivalence {'moves': 1920, 'classical': 1920, 'peak_total': 43, 'peak_single': 30}`。
- 文件：`evidence/bridge_phi1C6_c30.paths`、`evidence/C6_phi1_C6_bridge.json`。

### 4. 官方 SAIR 14 步语义复核（两个新证书）
```
python code/sair_replay.py .tools/out/certs_new <sair verifier package parent>
PASS C6_phi1_C6.json: 1920 official moves, peak=43, implementations_agree=True, reached_field_ok=True
PASS P1_phi1_C6.json: 461 official moves, peak=49, implementations_agree=True, reached_field_ok=True
```

### 5. 与 ms14 证书的关系
- `P1_equiv_C6.json`（1066 步、peak 41）由 `P1_to_C6_c30.paths` + `make_cert --cap 30 --conj 8` **逐字节重生成**（3 秒），等价性 PASS。
- 我们的 461 步账本 = P1 ~ φ₁(C6)，**不是** P1 ~ C6；它比 1066 步短，但目标不同且峰值更差（49 vs 41）。因此**不能说我们"改进了 P1~C6 的证书"**，除非另外的 φ 桥能缩短复合长度（当前 1920 步桥使复合为 2381 步，更差）。

## 文件清单
- `evidence/P1_c33_C6path.paths` / `.log`（路径重建）
- `evidence/P1_phi1_C6_c33_48hop.json`（461 步 P1~φ₁(C6) 账本）
- `evidence/bridge_phi1C6_c30.paths`（190 跳 C6~φ₁(C6) 路径）
- `evidence/C6_phi1_C6_bridge.json`（1920 步 C6~φ₁(C6) 账本）
- `evidence/P1_C6_c30_regenerated.json`（1066 步 P1~C6 逐字节重生成）
- 官方重放日志：`.tools/out/certs_new` 的 `sair_replay` 输出（已保存到 `evidence/sair_official_replay_new_certs.log`）
