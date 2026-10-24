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
- D051 WP5.3 integer FM4 ratio mapping: COMPLETE ✅ run `40139154497`, host `121006986068`, target `121006985928`

## WP5.3 / D051 final state - COMPLETE

| Check | Status |
|---|---|
| D051 frozen before production source | ✅ |
| source-absent reference CI | ✅ run `40138575267`, job `121005262360` |
| stable 16-ID Q8 ratio set | ✅ |
| base/clamp/rounding semantics | ✅ |
| invalid-ID and malformed-base behavior | ✅ |
| exhaustive 16,384 x 16 mapping | ✅ SHA `5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c` |
| exact intentional ceiling outputs | ✅ 27,615 |
| production source boundary | ✅ only `fmt_fm_ratio.h/.c` |
| static no-loop/div/mod/int64/float/heap audit | ✅ |
| D038/D049/D050 unchanged | ✅ |
| D049/D050 inherited regressions | ✅ |
| full host app WAV exact WP5.0 | ✅ |
| PI32v2 ratio object | ✅ 264 B / 1,024 B ceiling |
| ratio object SHA | ✅ `aa5bd8b9e1afa72054af2effeefbf468cfbb8b2b8d978e028540a30dc162956d` |
| full target exact WP5.0 | ✅ 72,792 / 19,676 / 113,000 B |
| BIN/FWSC exact WP5.0 | ✅ |
| `WP5_3_VERIFICATION.md` | ✅ |

Canonical artifacts:

- host ID `12577038930`, SHA `2b632171295a72854b08b6451b122b670dac81304323e4c86af0ed460d3cccb1`
- target ID `12578054556`, SHA `97a1a2f166c578a616d8f488e779b9905d630f20a2e5f1971542b8a2af0e29b3`

## WP5.4 next state - CONTRACT-FIRST

Issue #47. Planned branch `phase5/fm4-levels`.

| Check | Status |
|---|---|
| inspect D049 Q15 level semantics | ⬜ |
| inspect D050 modulation-depth/carrier semantics | ⬜ |
| choose bounded carrier/modulator control model | ⬜ |
| decide global vs per-operator/per-edge modulation representation | ⬜ |
| freeze stable internal control domains | ⬜ |
| freeze integer mappings/clamp/rounding | ⬜ |
| prove all four D050 algorithms | ⬜ |
| representative D051 ratio fixtures | ⬜ |
| source/realtime/object ceilings | ⬜ |
| D052 frozen before production source | ⬜ |
| source-absent reference CI | ⬜ |

D052 does not exist yet.

## Physical hardware boundary

No physical FM-1 is available. `hardware_margin_claim=none`.
