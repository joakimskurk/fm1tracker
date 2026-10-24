# FM1Tracker Current Test State

Date: 2026-10-24  
Purpose: **current verification overlay**

Legend: ✅ verified/frozen, ⬜ active/future, ⛔ physical unavailable.

## Completed baselines

- Phase 1-3: COMPLETE ✅
- D046 Phase 4 all-8-BIT baseline: COMPLETE ✅ run `40124536375`, job `120963022289`
- D048 WP5.0 mixed-engine routing: COMPLETE ✅ run `40129822006`, job `120978955451`
- D049 WP5.1 fixed-point FM operator: COMPLETE ✅ run `40134823600`, host `120994003059`, target `120994002842`
- D050 WP5.2 curated FM4 algorithm routing: COMPLETE ✅ run `40136425976`, host `120998804031`, target `120998804175`

## WP5.2 final evidence

- source-absent D050 run `40135979431`, job `120997461479` ✅
- four stable algorithms/topologies ✅
- fixed OP4 -> OP3 -> OP2 -> OP1 evaluation ✅
- all four operators advance once/sample ✅
- 65,536 renders per algorithm stress ✅
- algorithm-switch phase continuity ✅
- frozen long hashes exact ✅
- no loops/graph traversal/heap/float/int64/divide/modulo/store/router access in render ✅
- PI32v2 router object text+rodata 362 B / 2,048 B ceiling ✅
- object SHA `ed85fe6ed70402bd3b0727e59b1c8eee21f6d15f264ec8dc439603968a38f469` ✅
- full target exact WP5.0: 72,792 / 19,676 / 113,000 B ✅
- BIN/FWSC exact WP5.0 ✅
- evidence `WP5_2_VERIFICATION.md` ✅

## WP5.3 current state - ACTIVE CONTRACT-FIRST

Issue #46. Planned branch `phase5/fm4-ratios`.

| Check | Status |
|---|---|
| inspect canonical base pitch/increment helper | ⬜ |
| compact musically useful ratio set | ⬜ |
| stable internal ratio IDs | ⬜ |
| exact mathematical ratio meaning | ⬜ |
| integer transform arithmetic | ⬜ |
| exact rounding policy | ⬜ |
| overflow/clamp policy | ⬜ |
| supported base-increment domain | ⬜ |
| sub-unity/fractional ratio decision | ⬜ |
| deterministic low/mid/high vectors | ⬜ |
| high-register edge vectors | ⬜ |
| four-operator mapping API/state | ⬜ |
| realtime/source ceiling | ⬜ |
| pinned PI32v2 object ceiling | ⬜ |
| D051 contract frozen before production source | ⬜ |
| source-absent reference CI | ⬜ |
| production ratio implementation | ⬜ blocked until D051 gate |
| D049/D050 regressions remain exact | ⬜ |
| full target remains exact WP5.0 while isolated | ⬜ |
| `WP5_3_VERIFICATION.md` | ⬜ |

## Explicit WP5.3 non-goals

No final level/mod-index UX, feedback, envelopes, FM voice allocation/router integration, FM parameter pages/p-lock IDs or audible mixed FM production integration belong to WP5.3.

## Physical hardware boundary

No physical FM-1 is available. `hardware_margin_claim=none`.
