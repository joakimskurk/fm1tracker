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
- D056 WP5.8 FM parameter pages and p-lock mapping: COMPLETE ✅ closeout run `40203483720`
- WP5.9 mixed 8-BIT + FM4 validation: COMPLETE ✅ closeout run `40210173573`

## WP5.8 / D056 accepted baseline

| Check | Status |
|---|---|
| accepted closeout run | ✅ `40203483720` |
| closeout host | ✅ `121199398344` |
| closeout target | ✅ `121199398538` |
| pinned target image | ✅ 79,804 B |
| pinned target normal RAM | ✅ 27,792 B |
| pinned target pool | ✅ 113,000 B |
| FWSC SHA-256 | ✅ `4d2805f428f5114531b4c3d379576d2b207cbc9a6c04fcbdd5263cfaa0cc84a0` |
| exact all-8BIT production WAV | ✅ `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076` |
| durable closeout evidence | ✅ `docs/WP5_8_CLOSEOUT_EVIDENCE.md` |

The D056 27,792 B normal-RAM result remains planning context. It is not a physical free-RAM claim.

## WP5.9 final state

Last target-validated production checkpoint:
`81d3464059270130747e26fcc950b7a2e1bb03e1`

| Check | Status |
|---|---|
| precontract audit | ✅ `docs/WP5_9_PRECONTRACT_AUDIT.md` |
| mixed-validation contract frozen | ✅ `docs/WP5_9_MIXED_VALIDATION_CONTRACT.md` |
| delayed cross-family defect reproduced before repair | ✅ `docs/WP5_9_DEFECT_001_DELAYED_CROSS_FAMILY.md` |
| defect fix separately frozen | ✅ `docs/WP5_9_DEFECT_001_FIX_CONTRACT.md` |
| production fix scope | ✅ `firmware/src/dinghy.c` only |
| accepted closeout run | ✅ `40210173573` |
| closeout target | ✅ `121219510315` |
| closeout host | ✅ `121219510342` |
| pinned target image | ✅ 79,836 B |
| pinned target normal RAM | ✅ 27,792 B |
| pinned target pool | ✅ 113,000 B |
| D056 -> WP5.9 image delta | ✅ +32 B |
| D056 -> WP5.9 normal RAM delta | ✅ +0 B |
| D056 -> WP5.9 pool delta | ✅ +0 B |
| eight logical tracks preserved | ✅ |
| durable closeout evidence | ✅ `docs/WP5_9_CLOSEOUT_EVIDENCE.md` |

The documentation-only closeout/handoff commit after `81d3464` is not a new target-validated firmware checkpoint.

## WP5.10 active state

**WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit** is ACTIVE on issue `#53`, branch `phase5/worst-case-benchmark`.

| Check | Status |
|---|---|
| precontract audit | ✅ `docs/WP5_10_PRECONTRACT_AUDIT.md` |
| real linked callback inspected | ✅ both family renderers + tracker/control + audition path |
| family-local voice bounds identified | ✅ 8 8-BIT + 8 FM4 slots, 8 logical tracker tracks |
| mixed occupancy must be observed, not assumed | ✅ |
| full callback bounded extras identified | ✅ queue cap 32 + 4 audition voices |
| inherited test-only instrumentation strategy identified | ✅ WP4.10/WP5.9 |
| 24 MHz target TIMER4 candidate identified | ✅ `fm1_ticks()` |
| automated physical target runtime path | ⛔ unavailable |
| host wall-clock accepted as physical timing gate | ⛔ forbidden |
| benchmark contract frozen | ⬜ NEXT |
| fail-closed contract-freeze gate | ⬜ NEXT |
| mixed saturation workload implemented | ⬜ |
| full callback pressure workload implemented | ⬜ |
| exact reachable mixed occupancy recorded | ⬜ |
| deterministic repeated benchmark digests | ⬜ |
| extended dual-engine realtime audit | ⬜ |
| inherited WP5.9/D056/D055/all-8BIT regressions | ⬜ |
| uninstrumented target footprint closeout | ⬜ |
| optimization sub-contract | ⬜ only if measured constraint requires it |
| Phase 5 exit decision | ⬜ |

### Required benchmark evidence boundary

The contract must keep distinct:

1. deterministic host workload shape/state/audio evidence;
2. host timing, if collected, as informational only;
3. target build image/RAM/pool evidence from the uninstrumented production firmware;
4. target/emulator TIMER4 telemetry only if a reproducible runtime-return path is established;
5. physical FM-1 realtime/audio/free-RAM evidence as unavailable.

No production optimization is authorized yet.

## Physical hardware boundary

No physical FM-1 is available. Physical audio stability and real-device CPU headroom remain unverified. `hardware_margin_claim=none`.
