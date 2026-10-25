# FM1Tracker Test Matrix

This file is the current regression index. Exact historical vectors, hashes and package evidence remain in the corresponding `WP*_VERIFICATION.md`, contract and test-plan documents.

Legend:

- ✅ verified/frozen
- ⬜ active/future requirement
- ⛔ physical hardware unavailable

## Permanent invariants

| Check | Status |
|---|---|
| package identity `FM-1_987` | ✅ |
| pinned JieLi/AC79 build path | ✅ |
| 8 logical tracks x 64 steps x 16 patterns | ✅ |
| `FmtStep` exactly 8 B | ✅ |
| exact integer transport / row-boundary ordering | ✅ |
| 2,048 sparse p-locks, max four per step | ✅ |
| D032 exact six-tick tracker clock | ✅ |
| D034 exact 32-entry SPSC queue | ✅ |
| no active still-owned cross-track primary stealing | ✅ |
| no float in firmware DSP | ✅ through WP5.4, required onward |
| no runtime 64-bit division in firmware DSP | ✅ through WP5.4, required onward |
| physical AC791N execution margin | ⛔ no physical FM-1 |

## Completed Phase 5 FM foundations

### D048 WP5.0 mixed-engine routing

- per-instrument family/engine routing ✅
- song mode non-gating ✅
- 64 B identity publication / 8 B family route ✅
- D046 audio exact ✅
- target 72,792 / 19,676 / 113,000 B ✅
- run `40129822006`, job `120978955451` ✅

### D049 WP5.1 fixed-point FM operator

- 4 B state, generated 257-entry Q15 quarter-wave table ✅
- table SHA `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68` ✅
- long SHA `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66` ✅
- PI32v2 object 692 B ✅
- full app exact WP5.0 ✅

### D050 WP5.2 curated FM4 routing

- four static algorithms and fixed OP4 -> OP3 -> OP2 -> OP1 evaluation ✅
- every operator advances once/sample ✅
- 16 B routing state ✅
- fixed vector/hash/stress suite exact ✅
- PI32v2 router object 362 B ✅
- full app exact WP5.0 ✅

### D051 WP5.3 integer ratio/pitch mapping

- D051 frozen/source-absent gate ✅
- 16 stable Q8 ratios ✅
- exhaustive 262,144 outputs SHA `5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c` ✅
- 27,615 intentional ceiling outputs ✅
- PI32v2 ratio object 264 B ✅
- object SHA `aa5bd8b9e1afa72054af2effeefbf468cfbb8b2b8d978e028540a30dc162956d` ✅
- canonical run `40139154497` ✅

### D052 WP5.4 level/modulation-index mapping

- D052 frozen before production controls source ✅
- source-absent run `40140119411`, job `121009903030` ✅
- four LEVEL bytes + one global INDEX byte ✅
- exact 256-value LEVEL Q15 mapping ✅
- exact 256-value INDEX Q14 mapping ✅
- exhaustive mapping SHA `26e55d6a5fcacb5f432ecc3620b41a1c18e23f277aea6a13cb72d988a1929b38` ✅
- 65,536 edge-safety payload SHA `cf642aee2276abf355a62b7fb0ff1d8ff496e49b9d0a3397a0acc88fff421de0` ✅
- worst edge `2147287044`, int32 margin `196603` ✅
- D051 composition SHA `e1349b6d834de9655c2417e4a7a5aabdf64bed6138d8026bec10e23a4465486c` ✅
- transient mapped struct 10 B ✅
- source boundary only `fmt_fm_controls.h/.c` ✅
- static no-loop/div/mod/int64/float/heap/table/external-DSP audit ✅
- inherited D049/D050/D051/WP5.0 regressions exact ✅
- PI32v2 controls object 108 B / 1,024 B ceiling ✅
- object SHA `05867ebe120829d1e3b8bf6f0504f29d63e9643e4fa5eede4c6d858d31d25059` ✅
- full target exact 72,792 / 19,676 / 113,000 B ✅
- BIN `a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125` ✅
- FWSC `cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073` ✅
- canonical run `40143482844`, host `121019875458`, target `121019875309` ✅
- evidence `WP5_4_VERIFICATION.md` ✅

## WP5.5 bounded feedback - NEXT CONTRACT-FIRST

Issue #48.

| Check | Status |
|---|---|
| smallest deterministic feedback insertion point | ⬜ |
| one bounded V1 feedback topology | ⬜ |
| stable feedback control domain | ⬜ |
| fixed-size history/state ownership | ⬜ |
| reset/algorithm-switch history semantics | ⬜ |
| D051/D052 worst-case arithmetic proof | ⬜ |
| deterministic vectors and long stress | ⬜ |
| state/realtime/PI32v2 ceilings | ⬜ |
| D053 frozen before production source | ⬜ |
| source-absent reference CI | ⬜ |
| production feedback implementation | ⬜ blocked until D053 gate |
| inherited D049-D052 regressions | ⬜ |
| full app exact WP5.0 while isolated | ⬜ |
| `WP5_5_VERIFICATION.md` | ⬜ |

D053 does not exist yet.

## Remaining Phase 5

WP5.6 envelope, WP5.7 FM voice/router integration, WP5.8 FM pages/p-locks, WP5.9 mixed validation, WP5.10 worst-case Phase 5 exit.

No physical FM-1 is available. `hardware_margin_claim=none`.
