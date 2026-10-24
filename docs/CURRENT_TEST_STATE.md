# FM1Tracker Current Test State

Date: 2026-10-25  
Purpose: **current verification overlay**

Legend: ✅ verified/frozen, ⬜ active/future, ⛔ physical unavailable.

## Completed baselines

- Phase 1-3: COMPLETE ✅
- D046 Phase 4 all-8-BIT baseline: COMPLETE ✅ run `40124536375`, job `120963022289`
- D048 WP5.0 mixed-engine routing: COMPLETE ✅ run `40129822006`, job `120978955451`
- D049 WP5.1 fixed-point FM operator: COMPLETE ✅ run `40134823600`, host `120994003059`, target `120994002842`
- D050 WP5.2 curated FM4 algorithm routing: COMPLETE ✅ run `40136425976`, host `120998804031`, target `120998804175`

## WP5.3 current state - IMPLEMENTATION AUTHORIZED

Issue #46. Branch `phase5/fm4-ratios`.

| Check | Status |
|---|---|
| inspect canonical D038 pitch helper | ✅ |
| 16-ID curated ratio set | ✅ |
| stable IDs / exact Q8 meanings | ✅ |
| integer transform arithmetic | ✅ |
| round-half-up policy | ✅ |
| output ceiling / clamp policy | ✅ |
| supported base domain | ✅ |
| sub-unity/fractional decision | ✅ |
| low/mid/high vectors | ✅ |
| high-register first-clamp vectors | ✅ |
| four-operator map API/state | ✅ |
| PI32v2 object ceiling 1,024 B | ✅ frozen |
| D051 frozen before production source | ✅ |
| source-absent reference CI | ✅ run `40138575267`, job `121005262360` |
| exhaustive 16,384 x 16 mapping | ✅ SHA `5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c` |
| exact intentional clamps | ✅ 27,615 |
| D049/D050 inherited regressions | ✅ |
| full host app WAV exact WP5.0 | ✅ |
| production `fmt_fm_ratio.*` | ⬜ now authorized |
| pinned PI32v2 ratio object | ⬜ |
| full target exact WP5.0 while isolated | ⬜ |
| `WP5_3_VERIFICATION.md` | ⬜ |

## Source-absent evidence

- D051 contract checkpoint `e3b1272a442091dd9cc140f3d6d357f6215438b5`
- artifact `wp5-3-d051-contract-freeze`
- artifact ID `12577928512`
- artifact SHA `f6ff75f9dd2128f3d79cffd467ee1b88b1422a5daa788516387b7e9c50e19014`
- production ratio source absent ✅
- D038/D049/D050/`dinghy.c` delta zero ✅

## Explicit WP5.3 non-goals

No final level/mod-index UX, feedback, envelopes, FM voice allocation/router integration, FM parameter pages/p-lock IDs or audible mixed FM production integration belong to WP5.3.

## Physical hardware boundary

No physical FM-1 is available. `hardware_margin_claim=none`.
