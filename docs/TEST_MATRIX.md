# FM1Tracker Test Matrix

This file is the current regression index. Exact historical vectors, hashes and package evidence remain in the corresponding verification/contract documents.

Legend: ✅ verified/frozen, ⬜ active/future requirement, ⛔ physical hardware unavailable.

## Permanent invariants

| Check | Status |
|---|---|
| package identity `FM-1_987` | ✅ |
| pinned JieLi/AC79 build path | ✅ |
| 8 logical tracks x 64 steps x 16 patterns | ✅ |
| `FmtStep` exactly 8 B | ✅ |
| deterministic transport / row-boundary ordering | ✅ |
| 2,048 sparse p-locks, max four per step | ✅ |
| D032 exact six-tick tracker clock | ✅ |
| D034 exact 32-entry SPSC queue | ✅ |
| no active still-owned cross-track primary stealing | ✅ |
| no float in firmware DSP | ✅ through D056, required onward |
| no runtime 64-bit division in firmware DSP | ✅ through D056, required onward |
| exact WP5.0 all-8BIT production WAV | ✅ `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076` |
| physical AC791N execution margin | ⛔ no physical FM-1 |

## Phase 5 foundation

- D048 WP5.0 mixed-engine routing ✅
- D049 WP5.1 operator ✅
- D050 WP5.2 algorithms ✅
- D051 WP5.3 ratio/pitch ✅
- D052 WP5.4 LEVEL/INDEX ✅
- D053 WP5.5 feedback ✅
- D054 WP5.6 four operator envelopes ✅
- D055 WP5.7 FM voice/router integration ✅

## D056 WP5.8 FM parameter pages + p-lock mapping

- contract + source-absent freeze before production ✅
- 27 semantic FM IDs / seven pages ✅
- 27 B normalized parameter set ✅
- 432 B mutable sixteen-instrument FM base bank ✅
- four sparse locks/step unchanged ✅
- >255 FM sparse values rejected without truncation ✅
- generalized row cache 8704 B ✅
- historical 8BIT view 2048 B at offset 0 ✅
- deterministic double-buffered FM override publication ✅
- NOTE instrument inheritance / STEP_FULL / malformed fail-safe behavior ✅
- current active FM patch state 224 B + 8 B validity ✅
- per-row full FM patch cache 0 B ✅
- no sparse store/editor traversal in sample renderer ✅
- deterministic FM p-lock audio changes output ✅
- page-aware held-step FM editor ✅
- mutable base API instrument isolation / p-lock precedence ✅
- D049-D055 synthesis/voice/generated data frozen ✅
- inherited D054 -> D049 regression chain exact ✅
- exact all-8BIT regression equivalence ✅
- realtime audit heap/float/int64/store/editor sample reads absent ✅
- accepted closeout run `40203483720` ✅
- host `121199398344` ✅
- target `121199398538` ✅
- host artifact `12602299028`, SHA `5669fe35f7d1f09e4515498f1e57b1de9b57e5bac850514f7df4019a53fb877c` ✅
- target artifact `12603110877`, SHA `75fa1c19938bda8bcec4b5bc68efa2f2a1c8a556da747ec9906868abda53de0d` ✅
- full target 79804 image / 27792 RAM / 113000 pool ✅
- D056 ceilings 93432 image / 28656 RAM / 113000 pool ✅
- FWSC SHA `4d2805f428f5114531b4c3d379576d2b207cbc9a6c04fcbdd5263cfaa0cc84a0` ✅
- durable evidence `WP5_8_CLOSEOUT_EVIDENCE.md` ✅

The target is 864 B below the frozen D056 normal-RAM package ceiling. WP5.9/WP5.10 must keep resource measurement explicit; this value is not a physical-hardware free-RAM claim.

## WP5.9 mixed 8-BIT + FM4 validation - NEXT

Contract-first requirements:

- audit current mixed-family fixtures and state transitions ⬜
- simultaneous 8-BIT + FM4 workload with deterministic repeatability ⬜
- same-track family switches with ownership/state cleanup assertions ⬜
- no active cross-track primary stealing ⬜
- exact all-8BIT baseline remains bit-identical ⬜
- FM default/no-lock behavior remains D055/D056 equivalent ⬜
- sparse p-lock behavior remains D056 equivalent during mixed operation ⬜
- realtime/sample-path boundedness unchanged ⬜
- target resource measurement recorded, including delta from D056 ⬜
- fail-closed host + pinned-target closeout ⬜

Future virtual-analog/subtractive and drum engines are persistent product scope but are not WP5.9 implementation scope.

No physical FM-1 is available. `hardware_margin_claim=none`.