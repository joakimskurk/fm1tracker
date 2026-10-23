# FM1Tracker Test Matrix

This file tracks what must be verified at each layer. It is not a list of tests that already pass.

Legend:
- ✅ verified
- 🟨 partial / host-only / environment-limited
- ⬜ not yet verified
- ⛔ blocked with reason recorded in `PROJECT_STATE.md`

## Platform / baseline

| Check | Host | Firmware build | Physical FM-1 | Status |
|---|---:|---:|---:|---|
| Pinned Dinghy provenance complete | n/a | n/a | n/a | ✅ |
| Pristine Dinghy host build | required | n/a | n/a | ⬜ |
| Pristine Dinghy firmware build | n/a | required | optional | ⬜ |
| FM1Tracker renamed host build | required | n/a | n/a | ⬜ |
| FM1Tracker renamed firmware build | n/a | required | optional | ⬜ |
| USB rescue/update path unchanged | n/a | inspect/build | required later | ⬜ |
| Baseline RAM usage captured | host estimate optional | required | n/a | ⬜ |
| Baseline pool usage captured | host estimate optional | required | n/a | ⬜ |
| Baseline code size captured | n/a | required | n/a | ⬜ |
| Baseline audio callback timing | useful | useful | preferred | ⬜ |

## Phase 2 tracker core

| Check | Automated host test | Host interaction | Hardware |
|---|---:|---:|---:|
| Step accessor rejects out-of-range track/pattern/step | required | n/a | n/a |
| Pattern wrap is exactly 64 steps | required | observable | later |
| Four 16-step pages map to 1–64 without overlap/gaps | required | required | required |
| Integer clock has no cumulative drift over long run | required | optional | preferred |
| STEP REC inserts note at cursor | required | required | required |
| Note entry advances exactly one logical step | required | required | required |
| REST creates empty step and advances once | required | required | required |
| TIE semantics deterministic | required | required | required |
| NOTE OFF semantics deterministic | required | required | required |
| Track switch never writes previous track accidentally | required | required | required |
| Edit while stopped is deterministic | required | required | required |
| Tracker display uses bounded/dirty redraw | inspect/test | observable | required |

## Phase 3 p-locks/effects

| Check | Automated host test | Hardware |
|---|---:|---:|
| Maximum four locks per step enforced | required | optional |
| Replace existing parameter lock is deterministic | required | required |
| Delete lock cannot corrupt neighboring data | required | required |
| Stable parameter IDs are versioned/documented | required | n/a |
| Lock application does not mutate stored base instrument | required | observable |
| Queue overflow policy deterministic | required | stress test |
| Note delay/cut boundary behavior | required | required |
| Retrig count/timing bounded | required | required |

## Phase 4 8-BIT engine

| Check | Host | Firmware/hardware |
|---|---:|---:|
| Pitch table accuracy across intended range | required | required |
| No float/double symbols | inspect/build | required |
| No runtime 64-bit divide | inspect/build | required |
| PULSE stable at duty extremes | required | required |
| NOISE deterministic where intended | required | required |
| TRIANGLE no overflow/wrap artifact | required | required |
| WAVETABLE index bounds | required | required |
| Voice stealing deterministic | required | required |
| Representative 8-track pattern timing | benchmark | required |
| P-lock modulation remains stable | required | required |

## Phase 5 FM4

| Check | Host | Firmware/hardware |
|---|---:|---:|
| Operator pitch/ratio correctness | required | required |
| Algorithm routing matches specification | required | required |
| Feedback bounded under all values | stress test | required |
| Modulation accumulation cannot overflow unsafely | stress test | required |
| Voice allocation deterministic | required | required |
| Worst-case render timing within accepted budget | benchmark | required |

## Phase 6 persistence

| Check | Automated | Hardware |
|---|---:|---:|
| Serializer round trip byte/semantic equivalence | required | optional |
| Magic/version/length/CRC validation | required | optional |
| Truncated project fails safely | required | optional |
| Corrupt project fails safely | required | required |
| Unknown future version fails safely | required | optional |
| Save/load preserves reference project | required | required |
| Save path does not unexpectedly block audio | host inspect | required |
| Migration fixture retained for every persisted version | required | n/a |

## Phase 7 UX freeze

Physical FM-1 required for final signoff:

- STEP REC is understandable without repeated manual lookup.
- REST gesture is fast and comfortable.
- 16-step key page is visually/physically unambiguous.
- track selection does not cause frequent accidental notes/edits.
- hold-step + knob p-locking is comfortable.
- chord advance behavior matches user expectation.
- tracker rows are readable on 240 × 240 in realistic use.

## Regression rule

Once a check becomes ✅, later phases must keep it passing. If a deliberate design change invalidates a previous check, record a superseding decision and replace the test rather than silently dropping coverage.
