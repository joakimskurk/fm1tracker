# FM1Tracker Current Test State

Date: 2026-10-25  
Purpose: **current verification overlay**

Legend: ✅ verified/frozen, ⬜ active/future, ⛔ physical unavailable.

## Completed baselines

- Phase 1-3: COMPLETE ✅
- D046 Phase 4 all-8-BIT baseline: COMPLETE ✅ run `40124536375`, job `120963022289`
- D048 WP5.0 mixed-engine routing: COMPLETE ✅
- D049 WP5.1 fixed-point FM operator: COMPLETE ✅
- D050 WP5.2 curated FM4 routing: COMPLETE ✅
- D051 WP5.3 integer ratio/pitch mapping: COMPLETE ✅
- D052 WP5.4 level/index mapping: COMPLETE ✅
- D053 WP5.5 bounded OP4 feedback: COMPLETE ✅ final run `40165352623`

## WP5.6 / D054 technical state - COMPLETE

| Check | Status |
|---|---|
| four independent per-operator ADSR architecture | ✅ |
| D054 frozen before production source | ✅ |
| source-absent gate | ✅ run `40167537148`, job `121091550742` |
| state size | ✅ 20 B |
| control size | ✅ 16 B |
| Q15.16 level domain | ✅ max `0x7fff0000` |
| stage IDs OFF/A/D/S/R | ✅ 0/1/2/3/4 |
| 256-entry A/D/R table | ✅ 1,024 B |
| time-table SHA | ✅ `6e94aa6553b78919913d49a24c80ca7317c913fff344294a107137f85ffbc025` |
| SUSTAIN map exact | ✅ SHA `a799f70bd763053e79c3d9de6acdda97523fe54632105756c4252dc33d51c815` |
| 65,536 LEVEL composition pairs | ✅ SHA `196659105845f6183b0779183a583226129a5b7f35e8ac8ac811479b117e606b` |
| zero-time A/D/R semantics | ✅ |
| hard retrigger | ✅ zero |
| release from current level | ✅ |
| max release termination | ✅ `2701155` advances |
| 65,536-sample 4-op stress | ✅ SHA `8cdf12197813d5718b3b4c5a40375534f93ea006f61e5aa325f4b48af71e0849` |
| production source boundary | ✅ exactly 4 allowed files |
| generated table regeneration | ✅ byte-identical |
| no loops/div/mod/int64/float/heap/mutable globals | ✅ |
| inherited D053-D049 + WP5.0 host WAV | ✅ |
| PI32v2 envelope object | ✅ 1,680 B / 2,048 B ceiling |
| object SHA | ✅ `22fa7937d7776fea37c8e758ca83ee0a3f5d6c506ebd4661a8290a4516e048d1` |
| full target exact WP5.0 | ✅ 72,792 / 19,676 / 113,000 B |
| BIN/FWSC exact WP5.0 | ✅ |
| implementation run | ✅ `40167933064` |
| host job | ✅ `121092688126` |
| target job | ✅ `121092688295` |
| `WP5_6_VERIFICATION.md` | ✅ |
| final documentation-head rerun | ⬜ pending |

Canonical implementation artifacts:

- host ID `12588901443`, SHA `4452eec3d50127a392dad79246b54cf93883837f9a02b2af64848d129c76a202`
- target ID `12588804647`, SHA `c8eed7e762c3d58da18f0a3b5fbad28878b6da2e70181d22f2376ea7c3686f20`

## WP5.7 next state - PLANNED

Issue #50. Expected branch `phase5/fm4-voice-integration`.

WP5.7 remains blocked until WP5.6 closeout-head host+target verification is green and #49 is closed.

D055 must be frozen contract-first before voice/router integration changes.

## Physical hardware boundary

No physical FM-1 is available. `hardware_margin_claim=none`.
