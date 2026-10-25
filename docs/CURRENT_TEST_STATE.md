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
| validation-first production freeze | ✅ |
| real simultaneous FM4 + 8-BIT coverage | ✅ |
| parameterized high/low peak crossing | ✅ |
| one-step delayed overlap | ✅ |
| delayed single-family PCM -> FM | ✅ target PASS |
| delayed single-family FM -> PCM | ✅ target PASS |
| delayed mixed -> PCM-only | ✅ target PASS |
| delayed cross-family defect reproduced before repair | ✅ `docs/WP5_9_DEFECT_001_DELAYED_CROSS_FAMILY.md` |
| defect fix separately frozen | ✅ `docs/WP5_9_DEFECT_001_FIX_CONTRACT.md` |
| production fix scope | ✅ `firmware/src/dinghy.c` only |
| inherited all-8BIT regression | ✅ |
| inherited D055 default/no-lock FM regression | ✅ |
| inherited D056 locked-FM regression | ✅ |
| accepted closeout run | ✅ `40210173573` |
| closeout target | ✅ `121219510315` |
| closeout host | ✅ `121219510342` |
| WP5.9 target mixed validation | ✅ PASS |
| D056 target closeout regression | ✅ PASS |
| pinned target image | ✅ 79,836 B |
| pinned target normal RAM | ✅ 27,792 B |
| pinned target pool | ✅ 113,000 B |
| D056 -> WP5.9 image delta | ✅ +32 B |
| D056 -> WP5.9 normal RAM delta | ✅ +0 B |
| D056 -> WP5.9 pool delta | ✅ +0 B |
| eight logical tracks preserved | ✅ |
| durable closeout evidence | ✅ `docs/WP5_9_CLOSEOUT_EVIDENCE.md` |

The +32 B image delta is the measured cost of the separately governed defect correction and remains within the frozen +1024 B amendment cap. Normal RAM and pool remain bit-for-bit unchanged in size.

The documentation-only closeout/handoff commit after `81d3464` is not a new target-validated firmware checkpoint.

## WP5.10 next state

**WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit** is next. Before any production optimization:

- precontract audit ⬜
- freeze realistic worst-case mixed-engine workload ⬜
- preserve all eight logical tracks ⬜
- freeze reproducible host + target benchmark path ⬜
- measure target/resource effects against the D056/WP5.9 baseline ⬜
- determine whether a real measured constraint exists ⬜
- if and only if required, freeze a bounded optimization sub-contract ⬜
- record Phase 5 exit decision ⬜

Future virtual-analog/subtractive and drum engines remain persistent product directions but are not WP5.10 implementation scope.

## Physical hardware boundary

No physical FM-1 is available. Physical audio stability remains unverified. `hardware_margin_claim=none`.
