# FM1Tracker Work Packages

This document translates `ROADMAP.md` into bounded implementation units. `PROJECT_STATE.md` is the single source of truth for what is active now.

## Rules

1. Work on one active package at a time unless a dependency genuinely requires otherwise.
2. A package is complete only when every exit criterion passes or a waiver is explicitly recorded.
3. Do not pull later-package features forward for convenience.
4. Firmware behavior changes require tests/benchmarks appropriate to that layer.
5. Update `PROJECT_STATE.md` when a package starts, blocks or completes.
6. Architectural deviations must be recorded before they spread into code.
7. Third-party code/assets require provenance/license review before import.
8. Detailed completed-package evidence belongs in `WP*_VERIFICATION.md`.
9. Historical contracts are not rewritten to imply later architecture existed earlier.
10. Eight logical tracks, deterministic timing and bounded realtime work may not be silently weakened.

## Phase 1 - clean Dinghy baseline - COMPLETE

WP1.1-WP1.5 complete. Final baseline: 55,088 B image / 12,368 B RAM / 30,720 B pool, identity `FM-1_987`.

## Phase 2 - tracker core - COMPLETE

WP2.1-WP2.10 complete. Established 8 tracks x 64 steps x 16 patterns, exact 8 B `FmtStep`, pool-backed song, exact integer transport, NOTE/REST/TIE/OFF STEP REC, provisional controls and bounded tracker screen.

## Phase 3 - p-locks/effects/runtime ingress - COMPLETE

WP3.1-WP3.10 complete, D027-D036. Final target: 66,960 B image / 13,296 B RAM / 113,000 B pool.

## Phase 4 - first engine family: 8-BIT/chip - COMPLETE

WP4.1-WP4.10 complete, D037-D046. Canonical exit: run `40124536375`, job `120963022289`, evidence `WP4_10_VERIFICATION.md`.

## Phase 5 - mixed-engine routing + FM4 - IN PROGRESS

Parent issue: #42.

### WP5.0 - mixed-engine routing - COMPLETE - issue #43 - D048

Per-instrument family/engine routing is implemented and verified. Canonical run `40129822006`, job `120978955451`. Final target 72,792 B image / 19,676 B RAM / 113,000 B pool. Evidence `WP5_0_VERIFICATION.md`.

### WP5.1 - fixed-point FM operator - COMPLETE - issue #44 - D049

D049 isolated operator is implemented and verified:

- 4 B uint32 phase state;
- signed int32 phase modulation;
- generated 257-entry / 514 B quarter-wave;
- exact deterministic vectors/hashes;
- no forbidden realtime arithmetic/allocation;
- pinned PI32v2 object text+rodata 692 B;
- full app exact WP5.0.

Canonical run `40134823600`, host job `120994003059`, target job `120994002842`. Evidence `WP5_1_VERIFICATION.md`.

### WP5.2 - curated FM algorithm routing - ACTIVE - issue #45 - D050

D050 is frozen before production routing source.

Stable algorithm IDs:

```text
0 STACK4    OP4 -> OP3 -> OP2 -> OP1
1 FAN3      OP2 + OP3 + OP4 -> OP1
2 DUAL2     OP4 -> OP3 plus OP2 -> OP1
3 PARALLEL4 OP1 + OP2 + OP3 + OP4
```

Frozen execution model:

- operator index 0..3 = OP1..OP4;
- evaluation always OP4 -> OP3 -> OP2 -> OP1;
- every operator renders exactly once per sample;
- state exactly four D049 operators / 16 B, no extra history;
- invalid algorithm -> STACK4;
- internal mod depth 0..16383;
- edge mapping `sample * depth * 4`;
- multiple phase offsets accumulate modulo 2^32;
- carrier counts only 1/2/4 with power-of-two normalization toward zero;
- explicit four-case dispatch, no runtime graph traversal;
- router-only PI32v2 object ceiling 2,048 B;
- full app remains exact WP5.0 while isolated.

Required execution order from current checkpoint:

1. run the committed source-absent D050 reference gate;
2. require all topology, depth, wrap, normalization, first-12 and long-hash vectors green;
3. require 65,536 renders per algorithm stress green;
4. re-prove D049 and WP5.0 host baseline;
5. only after green gate add `fmt_fm_algorithm.h/.c`;
6. implement exactly D050 with no later-package features;
7. run focused production vectors, static audit and pinned PI32v2 object measurement;
8. prove full target remains exact WP5.0;
9. write `WP5_2_VERIFICATION.md` before WP5.3 begins.

Explicitly out of scope:

- ratio/tuning policy, WP5.3;
- final level/mod-index UI mapping, WP5.4;
- feedback, WP5.5;
- envelopes, WP5.6;
- FM voice/D048 integration, WP5.7;
- FM pages/p-lock IDs, WP5.8;
- audible mixed 8-BIT + FM4, WP5.9.

### WP5.3 - ratio table - PLANNED

Precomputed integer ratio choices and pitch/tuning fixtures.

### WP5.4 - levels / modulation index - PLANNED

Bounded integer operator-level and modulation-index mappings.

### WP5.5 - bounded feedback - PLANNED

Explicit bounded feedback topology with deterministic overflow/stability stress.

### WP5.6 - FM envelope strategy - PLANNED

Freeze shared/common versus per-operator strategy from measured cost.

### WP5.7 - FM voice allocation/integration - PLANNED

Integrate FM family into D048 ownership/router semantics without active cross-track primary stealing.

### WP5.8 - FM parameter pages + p-lock mapping - PLANNED

Stable semantic IDs/pages and safe family-specific parameter behavior.

### WP5.9 - mixed 8-BIT + FM4 validation - PLANNED

Simultaneous families and same-track family switches with exact all-8-BIT regression equivalence.

### WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit - PLANNED

Representative mixed workload, realtime audit, inherited regressions and exact target closeout.

## Mandatory core milestone - live performance capture + performance FX

PLANNED after engine foundations. Must retain continuous PLAY during performance/capture, deterministic capture mapping, safe track switching, drums, transport-locked metronome, bounded CHANNEL/GLOBAL automation/performance FX and measured polyphony without silently reducing eight logical tracks.

Reference: `LIVE_PERFORMANCE_CAPTURE_VISION.md`.

## Phase 6 - song/order/persistence - PLANNED

Order list, safe pattern switching, repeats/loops, versioned binary format, flash audit, save/load/CRC, corruption recovery and migration harness. Persistence inherits D047/D048 per-instrument routing semantics.

## Phase 7 - physical hardware UX freeze - BLOCKED

Requires physical FM-1.

## Phase 8 - interoperability - PLANNED

Internal interchange, MIDI investigation, Furnace import/export audit and converter prototype if justified. D003 remains authoritative: Furnace is a workflow/interoperability reference, not a full engine port.

## Phase 9 - optional extensions - DEFERRED

Possible later engines/features only after explicit CPU/RAM/parameter/license review.

`hardware_margin_claim=none` until physical hardware evidence exists.
