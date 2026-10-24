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

Evidence: `WP1_3_VERIFICATION.md`, `WP1_4_VERIFICATION.md`, `WP1_5_VERIFICATION.md`.

## Phase 2 - tracker core - COMPLETE

WP2.1-WP2.10 complete.

Established: 8 tracks x 64 steps x 16 patterns, exact 8 B `FmtStep`, pool-backed song, exact integer transport, NOTE/REST/TIE/OFF STEP REC, provisional step/track controls and bounded tracker screen.

Evidence: `WP2_1_VERIFICATION.md` through `WP2_9_VERIFICATION.md`, `WP2_10_TEST_PLAN.md`, `PHASE2_VERIFICATION.md`.

## Phase 3 - p-locks/effects/runtime ingress - COMPLETE

WP3.1-WP3.10 complete, D027-D036.

Established: stable parameter IDs, 2,048 sparse locks, held-step editing, row snapshots, exact six-tick effects, D033 event scheduling, 32-entry SPSC queue, bounded UI and full worst-case closeout.

Final target: 66,960 B image / 13,296 B RAM / 113,000 B pool.

## Phase 4 - first engine family: 8-BIT/chip - COMPLETE

WP4.1-WP4.10 complete, D037-D046.

Established: PULSE, NOISE, TRIANGLE, WAVETABLE-8, common ADSR, deterministic eight-slot allocation, parameter pages and linked all-eight-track playback.

Canonical Phase 4 exit: run `40124536375`, job `120963022289`, evidence `WP4_10_VERIFICATION.md`.

Final D046 target: 72,504 B image / 19,604 B RAM / 113,000 B pool.

D047 does not rewrite this history. The complete all-8-BIT path remains the regression baseline.

## Phase 5 - mixed-engine routing + FM4 - IN PROGRESS

Parent issue: #42.

### WP5.0 - mixed-engine routing contract + migration - COMPLETE - issue #43 - D048

D047/D048 migrated future sound architecture from project-wide engine family to per-instrument family + engine routing.

Frozen result:

- `FmtStep` remains 8 B.
- `FmtInstrumentHeader` remains 4 B.
- `FmtSongMeta.mode` is compatibility/default metadata, not an audio eligibility gate.
- 64 B instrument identity publication.
- 8 B global per-track family route.
- static bounded dispatch.
- same-track cross-family replacement is deterministic and fail-safe.
- unknown/unsupported family/engine fails toward silence.
- non-applicable p-locks are ignored, never reinterpreted.
- one shared 2,048 B row-page cache remains.
- no FM4 production DSP entered WP5.0.
- D046 representative and normal WAVs remain byte-identical.
- inherited Phase 1-4 regressions and realtime audits pass.

Canonical exit: run `40129822006`, job `120978955451`, head `813792fb25632b4d7acda787422e51f713fad21e`.

Final target: 72,792 B image / 19,676 B RAM / 113,000 B pool, delta +288 / +72 / +0 B versus D046.

Evidence: `WP5_0_VERIFICATION.md`.

### WP5.1 - fixed-point FM operator primitive - ACTIVE - issue #44

This is the only active package.

Goal: implement the smallest deterministic fixed-point operator primitive needed by later FM4 work.

Required execution order:

1. inspect accepted 8-BIT fixed-point pitch/phase/oscillator conventions;
2. write/freeze a normative D049/WP5.1 contract before production FM DSP changes;
3. freeze operator state and exact integer formats;
4. freeze deterministic vectors and edge behavior;
5. define target/realtime/resource guards;
6. implement the primitive only after the contract/tests exist;
7. run host vectors, inherited regressions and pinned target compile/resource measurement;
8. write `WP5_1_VERIFICATION.md` before WP5.2 starts.

Allowed scope:

- fixed-point phase accumulator/operator state;
- deterministic integer carrier lookup primitive;
- signed phase-modulation input;
- bounded operator level/output scaling;
- reset/init primitive;
- exact host vectors and edge tests;
- target compile/resource measurement;
- no-float/no-heap/no-runtime-64-bit-division guards.

Explicitly out of scope:

- algorithm routing, WP5.2;
- ratio table/pitch policy, WP5.3;
- final operator-level/modulation-index mappings, WP5.4;
- feedback, WP5.5;
- envelope strategy, WP5.6;
- FM voice allocation/router integration, WP5.7;
- FM parameter pages/p-lock mapping, WP5.8;
- simultaneous production 8-BIT + FM4 playback, WP5.9.

Exit criteria:

- D049/WP5.1 contract frozen before implementation;
- exact state sizes/formats compile-time asserted;
- deterministic phase/modulation/level/reset vectors green;
- bounded overflow/saturation behavior proven;
- no float/double, heap or runtime 64-bit division;
- pinned JieLi/AC79 target compile green;
- exact image/RAM/pool delta recorded and accepted;
- semantically applicable WP5.0/D046 regressions green;
- durable `WP5_1_VERIFICATION.md` written.

### WP5.2 - curated FM algorithm routing - PLANNED

Small musically useful static algorithm set, exact deterministic graph fixtures.

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

Stable semantic IDs/pages, safe family-specific parameter behavior.

### WP5.9 - mixed 8-BIT + FM4 validation - PLANNED

Simultaneous families and same-track family switches with exact all-8-BIT regression equivalence.

### WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit - PLANNED

Representative mixed workload, realtime audit, inherited regressions and exact target closeout.

## Mandatory core milestone - live performance capture + performance FX

PLANNED after engine foundations. This is core product work, not optional Phase 9 work.

Must include continuous PLAY during performance/capture, deterministic capture mapping, safe track switching, drums, transport-locked metronome, bounded CHANNEL/GLOBAL automation and performance FX, and measured polyphony without silently reducing eight logical tracks.

Reference: `LIVE_PERFORMANCE_CAPTURE_VISION.md`.

## Phase 6 - song/order/persistence - PLANNED

Order list, safe pattern switching, repeats/loops, versioned binary format, flash audit, save/load/CRC, corruption recovery and migration harness.

Persistence must inherit D047/D048 per-instrument routing semantics.

## Phase 7 - physical hardware UX freeze - BLOCKED

Requires physical FM-1 for control feel, install/boot/audio, display and measured callback margin.

## Phase 8 - interoperability - PLANNED

Internal interchange, MIDI investigation, Furnace import/export audit and converter prototype if justified.

D003 remains authoritative: Furnace is a workflow/interoperability reference, not a full engine port.

## Phase 9 - optional extensions - DEFERRED

Possible later engines/features only after explicit CPU/RAM/parameter/license review, including additional VA/supersaw/wavetable families, PCM beyond approved core needs, swing, microtiming, polymeter and extra DSP effects.

`hardware_margin_claim=none` until physical hardware evidence exists.
