# FM1Tracker Current Test State

Date: 2026-10-25  
Purpose: **current verification overlay**

Legend: ✅ verified/frozen, ⬜ active/future, ⛔ physical unavailable.

## Completed baselines

- Phase 1-3: COMPLETE ✅
- D046 Phase 4 all-8BIT baseline: COMPLETE ✅ run `40124536375`, job `120963022289`
- D048 WP5.0 through D056 WP5.8: COMPLETE ✅
- WP5.9 mixed 8-BIT + FM4 validation: COMPLETE ✅ closeout run `40210173573`

## Accepted parent resources

| Check | D056 | WP5.9 |
|---|---:|---:|
| image | 79,804 B | 79,836 B |
| normal RAM | 27,792 B | 27,792 B |
| pool | 113,000 B | 113,000 B |

WP5.9 target-validated production checkpoint: `81d3464059270130747e26fcc950b7a2e1bb03e1`.

## WP5.10 active state

Issue `#53`, branch `phase5/worst-case-benchmark`.

| Check | Status |
|---|---|
| precontract audit | ✅ `docs/WP5_10_PRECONTRACT_AUDIT.md` |
| benchmark contract | ✅ FROZEN `docs/WP5_10_WORST_CASE_MIXED_BENCHMARK_CONTRACT.md` |
| production source freeze | ✅ baseline `81d3464` |
| contract-freeze script | ✅ committed, CI result pending at contract checkpoint |
| S1 exact 8 FM4 + 8 8-BIT occupancy requirement | ✅ frozen |
| S1 warmup | ✅ 256 frames |
| S1 measured window | ✅ 4096 frames |
| S1 repetitions | ✅ 3 |
| S2 queue depth at callback entry | ✅ 32 |
| S2 engine occupancy | ✅ 8+8 |
| S2 audition voices | ✅ 4 |
| S2 audition sine calls | ✅ 1024 in one 256-frame callback |
| S2 repetitions | ✅ 3 |
| host timing as physical gate | ⛔ forbidden |
| target runtime timing | ⬜ deferred without reproducible execution-return path |
| mixed benchmark implementation | ⬜ NEXT after green contract freeze |
| dual-engine realtime audit implementation | ⬜ |
| inherited regressions | ⬜ |
| uninstrumented target footprint closeout | ⬜ |
| optimization sub-contract | ⬜ only if accepted evidence requires it |
| Phase 5 exit decision | ⬜ |

## Evidence boundary

The contract keeps distinct:

1. deterministic host workload shape/state/audio evidence;
2. host timing as informational only;
3. uninstrumented pinned-target build/resource evidence;
4. target/emulator TIMER4 telemetry only if a reproducible runtime-return path exists;
5. physical FM-1 realtime/audio/free-RAM evidence as unavailable.

No production optimization is authorized.

No physical FM-1 is available. `hardware_margin_claim=none`.
