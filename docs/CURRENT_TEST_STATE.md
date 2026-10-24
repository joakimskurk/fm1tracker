# FM1Tracker Current Test State

Date: 2026-10-24  
Purpose: **current verification overlay**

`TEST_MATRIX.md` remains the detailed historical regression inventory. This file records the final D046 state and the active D047/WP5.0 obligations.

## WP4.10 / D046 final state

Legend:

- ✅ verified
- ⬜ physical/future work not yet possible

| Check | Final status | Evidence / note |
|---|---|---|
| Production source remains frozen to D045 baseline `c93c39ad...` | ✅ | final aggregate run `40124536375`; no production defect or delta |
| Full 2,048-record D028 workload across 8 tracks x 64 rows | ✅ | 512 owners / 2,048 locks / all 4 engines |
| All four 8-BIT engines under D031/D032/D033/D044 pressure | ✅ | canonical D046 representative linked workload |
| Representative 64-row render repeat identity | ✅ | byte-identical WAV, state digest and action/allocation counts |
| Representative WAV | ✅ | SHA-256 `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9`, 353,024 frames |
| 4,096-row / 64-wrap linked long run | ✅ | `ticks=163840`, final `step=0 phase=0` |
| Long-run repeat identity | ✅ | sample digest, state digest and action/allocation counts identical |
| Exact D033 action counts | ✅ | long run: NONE 188,808 / DELAY 1,472 / CUT 2,368 / RETRIG 3,968 |
| Exact D043 allocator outcomes | ✅ | long run: NONE 0 / REUSE 14,336 / FRESH_IDLE 1,800 / STEAL_TAIL 4,352 |
| D043 release-tail contention never steals owned active cross-track primary | ✅ | `active_cross_track_steals=0`, `alloc_invariant_fail=0` |
| Concurrent D034 queue full/overflow/recovery while tracker runs | ✅ | `short_overflow=16 long_overflow=40 recovery=1 tracker_continues=1` |
| Structural callback/realtime ownership audit | ✅ | final aggregate includes focused realtime audit |
| Queue work bounded to capacity 32 / O(1) overflow discard | ✅ | `callback_cap=32`, no spin/retry or IRQ masking |
| Linked engine render scans exactly 8 fixed slots | ✅ | no mutable store or lock-list reads in render |
| One transport-derived six-tick tracker clock / row boundary wins | ✅ | row phase 661500 / tick phase 110250 / 6 ticks per row |
| No float/double in audited realtime callback/leaves | ✅ | `float_double=0` |
| No runtime 64-bit types in audited callback/realtime leaves | ✅ | callback 0 / leaves 0 |
| Inherited non-obsolete regression suite + canonical WP4.9 linked evidence | ✅ | authoritative WP4.9 closeout rerun inside final D046 aggregate |
| Pinned target build/resource/artifact identity | ✅ | exact D045 image/RAM/pool and BIN/FWSC hashes retained |
| Final aggregate D046 exit | ✅ | run `40124536375`, job `120963022289`, head `2379d3e...` |
| Final Phase 4 closeout evidence | ✅ | `docs/WP4_10_VERIFICATION.md` |
| Physical FM-1 execution margin | ⬜ | no physical FM-1 available; `hardware_margin_claim=none` |
| Physical install/boot/audio/control/screen validation | ⬜ | later hardware gate |

Canonical final marker:

```text
WP4_10_EXIT_OK=1 representative_wav_sha256=33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9 inherited_regressions=pass realtime=pass d033_alloc_counts=pass repeat_identity=pass target=pinned identity=FM-1_987 phase4=exit_ready
```

Closeout interpretation:

```text
WP4_10_PHASE4_EXIT_OK=1 hardware_margin_claim=none
```

Phase 4 is complete at host/device-build level.

## D047 mixed-engine architecture state

D047 is **architecture/product direction already frozen**. Production migration begins in WP5.0.

| Check | Status | Required package |
|---|---|---|
| Project-wide 8-BIT/16-BIT restriction superseded in current architecture docs | ✅ | D047 documentation |
| Existing Phase 4 all-8-BIT implementation retained as frozen regression baseline | ✅ | D047 + D046 closeout |
| `FmtStep` remains 8 B and instrument indirection reused | ✅ design compatibility | existing model + D047 |
| `FmtInstrumentHeader` remains 4 B and current family values preserved | ✅ design compatibility | existing model + D047 |
| Historical D037-D046 contracts/evidence remain unchanged | ✅ policy | D047 |
| Per-instrument family + engine becomes authoritative playback routing | ⬜ | WP5.0 |
| `FmtSongMeta.mode` stops gating valid instrument playback | ⬜ | WP5.0 |
| Two or more engine families coexist across logical tracks | ⬜ | WP5.0 + first mixed-family fixture |
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

## Active package test obligations - WP5.0

WP5.0 must freeze tests before changing production routing. At minimum it must prove:

| Check | Requirement |
|---|---|
| Existing all-8-BIT D046 configuration | remains regression-identical |
| Song-level family gate | focused test demonstrates the current restriction before migration |
| Per-instrument family dispatch | deterministic and bounded |
| Simultaneous family coexistence | independent tracks can route different families |
| Same-track cross-family change | deterministic identity/reset semantics |
| Primary voice ownership | max one owned primary per logical track across families |
| Cross-track stealing | still-owned active primary may never be stolen |
| Publication/parameter adapter | bounded, family-safe, no incompatible p-lock reinterpretation |
| Runtime dispatch | static/bounded, no heap or dynamic plugin mechanism |
| Resource delta | measured before accepting routing representation |
| Target build | pinned JieLi/AC79 build remains inside explicit budget |

No FM4 operator source should be linked into production before WP5.0 closes.

## Phase 5 / FM4 obligations after WP5.0

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
| Inherited 8-BIT regression | complete Phase 4 family remains deterministic |
| Mixed resource closeout | exact image/RAM/pool + target artifacts + structural realtime audit |

## Regression rule

D047 is a superseding architecture decision, not permission to silently invalidate old verification.

During WP5.0 and later Phase 5 work:

- preserve all semantically applicable Phase 1-4 tests;
- keep the all-8-BIT D046 path as an explicit regression configuration;
- add mixed-family tests beside it;
- replace a historical assertion only when a new decision explicitly supersedes its semantics;
- never delete evidence merely because the architecture has become more general.
