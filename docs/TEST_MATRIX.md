# FM1Tracker Test Matrix

This file is the current regression index. Exact historical vectors, hashes and package evidence remain in the corresponding `WP*_VERIFICATION.md`, contract and test-plan documents.

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
| no float in firmware DSP | ✅ through D055, required onward |
| no runtime 64-bit division in firmware DSP | ✅ through D055, required onward |
| physical AC791N execution margin | ⛔ no physical FM-1 |

## Phase 5 FM foundation

- D048 WP5.0 mixed-engine routing ✅
- D049 WP5.1 operator ✅
- D050 WP5.2 algorithms ✅
- D051 WP5.3 ratio/pitch ✅
- D052 WP5.4 LEVEL/INDEX ✅
- D053 WP5.5 feedback ✅
- D054 WP5.6 four operator envelopes ✅

## D055 WP5.7 FM voice/router integration

- D055 frozen before production integration ✅
- source-absent gate `40171940961` ✅
- eight family-local FM slots ✅
- 92 B voice / 788 B engine ✅
- exact D043 allocator policy ✅
- no active cross-track primary stealing ✅
- deterministic detached release-tail reuse ✅
- NOTE/RETRIG/OFF/CUT integration ✅
- D050 externally-rendered-OP4 seam backward-compatible ✅
- D048 FM family dispatch integrated ✅
- actual `FM4 -> 8-BIT -> FM4` production fixture deterministic ✅
- mixed fixture SHA `e7bef408b3ce8c40e5b51cc6489dc39068ade82a86ccad0297070a3d2f2df034` ✅
- exact all-8BIT regression SHA `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076` ✅
- D043/D049/D051-D054/generated tables unchanged ✅
- fixed realtime work; heap/float/int64/runtime divmod absent ✅
- PI32v2 engine+algorithm text+rodata 1664 B ✅
- full target 77048 image / 20464 RAM / 113000 pool ✅
- implementation run `40173076046`, host `121107852886`, target `121107852943` ✅
- durable evidence `WP5_7_VERIFICATION.md` ✅
- final documentation-head rerun ⬜

## WP5.8 FM parameter pages + p-lock mapping - NEXT

Required contract-first work:

- audit existing parameter semantic-ID namespace ⬜
- audit page layout/selection and current 8-BIT family behavior ⬜
- audit 2,048 sparse p-lock storage and max-four-per-step constraint ⬜
- freeze stable FM parameter IDs/pages ⬜
- freeze family-specific invalid-ID behavior ⬜
- define mutable FM instrument parameter storage replacing the WP5.7 immutable bridge patch ⬜
- preserve deterministic row-boundary/p-lock timing ⬜
- preserve exact all-8BIT regression equivalence ⬜
- freeze RAM/image ceilings before production implementation ⬜
- source-absent gate before mutable FM parameter production source ⬜

No physical FM-1 is available. `hardware_margin_claim=none`.
