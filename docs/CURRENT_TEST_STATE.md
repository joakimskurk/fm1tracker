# FM1Tracker Current Test State

Date: 2026-10-25  
Purpose: **current verification overlay**

Legend: ✅ verified/frozen, ⬜ active/future, ⛔ physical unavailable.

## Completed baselines

- Phase 1-3: COMPLETE ✅
- D046 Phase 4 all-8BIT baseline: COMPLETE ✅ run `40124536375`, job `120963022289`
- D048 WP5.0 mixed-engine routing: COMPLETE ✅
- D049 WP5.1 fixed-point FM operator: COMPLETE ✅
- D050 WP5.2 curated FM4 routing: COMPLETE ✅
- D051 WP5.3 integer ratio/pitch mapping: COMPLETE ✅
- D052 WP5.4 level/index mapping: COMPLETE ✅
- D053 WP5.5 bounded OP4 feedback: COMPLETE ✅
- D054 WP5.6 FM envelopes: COMPLETE ✅ final run `40168286355`
- D055 WP5.7 FM voice/router integration: COMPLETE ✅ closeout run `40177151084`

## WP5.8 / D056 final state

| Check | Status |
|---|---|
| D056 contract frozen before production | ✅ |
| source-absent preimplementation gate | ✅ run `40178938471` |
| semantic FM parameter IDs | ✅ 27 |
| logical FM pages | ✅ 7 |
| `FmtFmParamSet` | ✅ 27 B |
| mutable FM instrument base bank | ✅ 432 B / 16 instruments |
| sparse p-lock limit | ✅ four per step unchanged |
| FM lock >255 | ✅ rejected, no truncation |
| generalized physical row cache | ✅ 8704 B |
| historical 8BIT row-cache view | ✅ first 2048 B unchanged |
| deterministic FM override publication | ✅ inactive bank then selector flip |
| store-backed NOTE inheritance | ✅ exact |
| wrong-family IDs | ✅ preserved by D028, ignored by FM resolver |
| malformed sparse state | ✅ empty fail-safe publication |
| current active FM patch snapshots | ✅ 224 B + 8 B validity |
| per-row full FM patch cache | ✅ 0 B |
| sparse store/editor reads in sample renderer | ✅ 0 |
| FM p-lock audio deterministic and audible | ✅ |
| held-step FM page editor | ✅ page-aware, main/UI only |
| legacy generic/8BIT editor behavior | ✅ unchanged |
| mutable base API | ✅ bounded, instrument-local, no cache rebuild |
| D049-D055 DSP/voice source | ✅ frozen |
| inherited D054 -> D049 host gates | ✅ exact |
| exact all-8BIT production WAV | ✅ SHA `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076` |
| realtime audit | ✅ bounded, no heap/float/int64/store/editor sample reads |
| accepted closeout run | ✅ `40203483720` |
| closeout host | ✅ `121199398344` |
| closeout target | ✅ `121199398538` |
| closeout host artifact | ✅ `12602299028`, SHA `5669fe35f7d1f09e4515498f1e57b1de9b57e5bac850514f7df4019a53fb877c` |
| closeout target artifact | ✅ `12603110877`, SHA `75fa1c19938bda8bcec4b5bc68efa2f2a1c8a556da747ec9906868abda53de0d` |
| pinned target image | ✅ 79804 B <= 93432 B |
| pinned target normal RAM | ✅ 27792 B <= 28656 B |
| pinned target pool | ✅ 113000 B exact |
| FWSC SHA-256 | ✅ `4d2805f428f5114531b4c3d379576d2b207cbc9a6c04fcbdd5263cfaa0cc84a0` |
| durable closeout evidence | ✅ `WP5_8_CLOSEOUT_EVIDENCE.md` |

The final normal-RAM margin is 864 B to the frozen D056 package ceiling. This is not a physical free-RAM claim, but WP5.9/WP5.10 must treat resource growth as a measured constraint.

The first closeout run `40184529349` is historical non-accepted evidence: its host wrapper failed because `tee` could race directory creation even though the host test script itself reached its green summary. The accepted rerun pre-created the evidence directory and passed without weakening tests.

## WP5.9 next state

Mixed 8-BIT + FM4 validation is next. Required before production changes:

- audit current simultaneous-family fixtures and same-track family-switch behavior ⬜
- define deterministic simultaneous-family workload(s) ⬜
- define same-track switch coverage including state/ownership cleanup ⬜
- freeze exact all-8BIT regression gate ⬜
- freeze resource/realtime expectations with D056 RAM context visible ⬜
- freeze fail-closed host + pinned-target verification ⬜

Future virtual-analog/subtractive and drum engines remain product directions but are not WP5.9 implementation scope.

## Physical hardware boundary

No physical FM-1 is available. `hardware_margin_claim=none`.