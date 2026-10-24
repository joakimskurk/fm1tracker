# FM1Tracker Current Test State

Date: 2026-10-24  
Purpose: **current in-flight verification overlay**

`TEST_MATRIX.md` remains the detailed historical regression inventory. This file records the current WP4.10/D046 state and the new D047/WP5.0 obligations without rewriting historical rows before Phase 4 closeout evidence is final.

When WP4.10 closes, its final verified results should be folded into `TEST_MATRIX.md` together with `WP4_10_VERIFICATION.md`. Until then, this file + `PROJECT_STATE.md` are authoritative for current D046 progress.

## WP4.10 / D046 current state

Legend:

- ✅ focused gate verified
- 🟨 evidence exists but final aggregate closeout still pending
- ⛔ blocks Phase 4 closeout
- ⬜ physical/future work not yet possible

| Check | Current status | Evidence / note |
|---|---|---|
| Production source remains frozen to D045 baseline `c93c39ad...` | ✅ focused gates | representative + realtime audit both verify production freeze; final aggregate still pending |
| Full 2,048-record D028 workload across 8 tracks x 64 rows | ✅ | representative run `40118888617`; 512 owners / 2,048 locks / 4 engines |
| All four 8-BIT engines under representative D031/D032/D033/D044 pressure | ✅ | representative run `40118888617` |
| 4,096-row / 64-wrap linked long run | ✅ | `WP4_10_LONG_RUN_OK=1 rows=4096 wraps=64 ticks=163840` |
| D043 release-tail contention / no owned cross-track primary steal | ✅ focused fixture | representative D046 workload exercises the D043 contention boundary |
| Concurrent D034 queue full/overflow/recovery while tracker runs | ✅ | `WP4_10_QUEUE_PRESSURE_OK=1 short_overflow=16 long_overflow=40 recovery=1 tracker_continues=1` |
| Structural callback/realtime ownership audit | ✅ | run `40120436446`, job `120950599001` |
| Queue work bounded to capacity 32 / O(1) overflow discard | ✅ | `WP4_10_QUEUE_BOUND_OK=1 capacity=32 callback_cap=32 overflow_discard_o1=1 spin_retry=0 irq_masking=0` |
| Linked engine render scans exactly 8 fixed slots | ✅ | `WP4_10_ENGINE_BOUND_OK=1 render_slots=8 mutable_store_reads=0 lock_list_reads=0` |
| One transport-derived six-tick tracker clock / row boundary wins | ✅ | `WP4_10_SINGLE_CLOCK_OK=1 row_phase=661500 tick_phase=110250 ticks_per_row=6` |
| No float/double in audited realtime callback/leaves | ✅ | `WP4_10_INTEGER_REALTIME_OK=1 ... float_double=0` |
| No runtime 64-bit types in audited callback/realtime leaves | ✅ | `callback_64bit_types=0 leaf_64bit_types=0` |
| Inherited non-obsolete regression suite + canonical WP4.9 linked evidence | 🟨 | aggregate reuses authoritative WP4.9 closeout; first aggregate run is red and exact failing command still needs isolation |
| Pinned target build/resource/artifact identity re-proved by final aggregate | 🟨 | WP4.9 identity is already frozen; D046 aggregate proof not yet green |
| Final `WP4_10_PHASE4_EXIT_OK` / `WP4_10_EXIT_OK` | ⛔ | run `40120541899`, job `120950915360` failed in aggregate step |
| Physical FM-1 execution margin | ⬜ | no physical FM-1 available; no hardware-margin claim is allowed |
| Physical install/boot/audio/control/screen validation | ⬜ | later hardware gate |

### Current Phase 4 blocker

The only accepted Phase 4 blocker is the red aggregate exit gate:

```text
run 40120541899
job 120950915360
step: Run aggregate D046 Phase 4 exit gate
```

Because the focused representative and realtime audits are green, the next task is to isolate whether the aggregate failure is:

1. an aggregate script/path/grep/harness problem; or
2. a real failure in the inherited regression/pinned-target chain.

Do not close Phase 4 or modify production for D047 until this is resolved under the D046 contract.

## D047 mixed-engine architecture state

D047 is **architecture/product direction already frozen**, not implemented production behavior.

| Check | Status | Required package |
|---|---|---|
| Project-wide 8-BIT/16-BIT restriction superseded in current architecture docs | ✅ | D047 documentation |
| Existing Phase 4 all-8-BIT implementation retained as valid frozen baseline | ✅ | D047 non-destructive rule |
| `FmtStep` remains 8 B and instrument indirection reused | ✅ design compatibility | existing model + D047 |
| `FmtInstrumentHeader` remains 4 B and current family values preserved | ✅ design compatibility | existing model + D047 |
| Historical D037-D046 contracts/evidence remain unchanged | ✅ policy | D047 |
| Per-instrument family + engine becomes authoritative playback routing | ⬜ | WP5.0 |
| `FmtSongMeta.mode` stops gating valid instrument playback | ⬜ | WP5.0 |
| Two or more engine families coexist across logical tracks | ⬜ | WP5.0 + first FM4 integration fixture |
| Same track can deterministically change instrument across families | ⬜ | WP5.0 |
| At most one owned primary sequenced voice per logical track across families | ⬜ | WP5.0 |
| No active cross-track primary steal across engine families | ⬜ | WP5.0 |
| Cross-family runtime ownership/allocator representation measured and frozen | ⬜ | WP5.0 |
| Dispatch remains static/bounded, no dynamic plugin architecture | ⬜ | WP5.0 |
| Non-applicable p-lock IDs ignored safely without reinterpretation | ⬜ | WP5.0 + engine parameter tests |
| 8-BIT-only behavior remains regression-equivalent through mixed router | ⬜ | WP5.0 closeout |
| Mixed 8-BIT + FM4 simultaneous target fixture | ⬜ | WP5.9 |
| Worst-case mixed-engine host/device-build benchmark | ⬜ | WP5.10 |
| Physical mixed-engine callback margin | ⬜ | later physical FM-1 gate |

## Phase 5 / FM4 test obligations after WP5.0

The FM4 family must then prove at least:

| Check | Host/device-build requirement |
|---|---|
| Four-operator fixed-point primitive | deterministic vectors + target compile |
| Curated algorithm routing | exact graph/routing fixtures |
| Ratio table / pitch | deterministic tuning vectors across supported range |
| Modulation index / operator levels | bounded integer mappings |
| Feedback | explicitly bounded, deterministic, no overflow/unstable runtime work |
| FM envelope strategy | deterministic NOTE/TIE/RETRIG/OFF semantics |
| FM voice integration | bounded ownership/allocation with no cross-track steal |
| FM semantic parameter IDs/pages | stable collision-safe D027 assignments + p-lock tests |
| Mixed 8-BIT + FM4 instrument switching | deterministic same-track/cross-track fixtures |
| Inherited 8-BIT regression | complete current family remains deterministic |
| Mixed resource closeout | exact image/RAM/pool + target artifacts + structural realtime audit |

## Regression rule

D047 is a superseding architecture decision, not permission to silently invalidate old verification.

When WP5.0 begins:

- preserve all semantically applicable Phase 1-4 tests;
- keep the all-8-BIT path as an explicit regression configuration;
- add mixed-family tests beside it;
- replace a historical assertion only when a new decision explicitly supersedes its semantics;
- never delete evidence merely because the architecture has become more general.
