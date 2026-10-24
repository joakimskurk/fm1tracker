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

WP4.1-WP4.10 complete, D037-D046. PULSE, NOISE, TRIANGLE, WAVETABLE-8, common ADSR, deterministic eight-slot allocation, parameter pages and linked all-eight-track playback are verified.

Canonical Phase 4 exit: run `40124536375`, job `120963022289`, evidence `WP4_10_VERIFICATION.md`.

Final D046 target: 72,504 B image / 19,604 B RAM / 113,000 B pool.

## Phase 5 - mixed-engine routing + FM4 - IN PROGRESS

Parent issue: #42.

### WP5.0 - mixed-engine routing contract + migration - COMPLETE - issue #43 - D048

D047/D048 migrated future sound architecture from project-wide engine family to per-instrument family + engine routing.

Frozen result:

- `FmtStep` 8 B and `FmtInstrumentHeader` 4 B unchanged;
- `FmtSongMeta.mode` is non-gating compatibility/default metadata;
- 64 B instrument identity publication;
- 8 B global per-track family route;
- static bounded dispatch;
- deterministic same-track cross-family replacement;
- unsupported/malformed family/engine fails toward silence;
- non-applicable p-locks ignored, never reinterpreted;
- one shared 2,048 B row-page cache;
- no FM DSP in WP5.0;
- D046 all-8-BIT output remains exact.

Canonical exit: run `40129822006`, job `120978955451`, head `813792fb25632b4d7acda787422e51f713fad21e`.

Final target: 72,792 B image / 19,676 B RAM / 113,000 B pool.

Evidence: `WP5_0_VERIFICATION.md`.

### WP5.1 - fixed-point FM operator primitive - COMPLETE - issue #44 - D049

Goal achieved: smallest deterministic fixed-point operator primitive required by later FM4 work, still isolated from the running application.

Frozen/verified result:

- exactly 4 B operator state: one `uint32_t phase`;
- 2^32 phase cycle with natural wrap;
- sample-before-advance ordering;
- signed `int32_t` phase modulation;
- Q15 level `0..32767`;
- generated 257-entry positive quarter-wave table, 514 B payload;
- exact 22-bit interpolation;
- table SHA `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68`;
- 4,096-render SHA `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66`;
- 65,536 symmetry probes;
- no loop/heap/float/int64/divide/modulo/store/router traversal in primitive;
- pinned PI32v2 object text+rodata 692 B against 4,096 B ceiling;
- full app remains exact WP5.0 in WAV, resources, BIN and FWSC.

Canonical exit:

- head `52df09bf363ad5a9ad2d722a38734eaa580ecb90`
- run `40134823600`
- host job `120994003059`
- target job `120994002842`
- evidence `WP5_1_VERIFICATION.md`

WP5.1 deliberately did not add algorithms, ratios, feedback, envelopes, voice integration, p-lock pages or audible FM production playback.

### WP5.2 - curated FM algorithm routing - ACTIVE - issue #45

This is the only active package.

Goal: define and implement a small musically useful static four-operator routing layer on top of D049 without pulling later FM features forward.

Required execution order:

1. inspect D049 operator semantics and accepted Phase 5 FM constraints;
2. select a small curated algorithm set, not an unrestricted routing matrix;
3. freeze stable internal algorithm IDs and exact graph topology;
4. freeze exact per-sample operator evaluation order;
5. freeze signed modulation accumulation/scaling and overflow/saturation policy;
6. freeze carrier summing/output normalization;
7. decide whether routing needs any mutable state before feedback exists;
8. freeze deterministic graph fixtures and long-run hashes;
9. define realtime/source and pinned-target object ceilings;
10. write/freeze D050 `WP5_2_FM_ALGORITHM_CONTRACT.md` before production routing source;
11. run a source-absent pre-implementation reference gate;
12. only then implement routing and verify it;
13. write `WP5_2_VERIFICATION.md` before WP5.3 starts.

Allowed scope:

- static four-operator routing/evaluation order;
- bounded switch/descriptor dispatch;
- signed modulation accumulation/scaling;
- carrier sum/output normalization required by routing;
- deterministic graph vectors/hashes;
- target compile/object measurement;
- inherited D049 and WP5.0 regression gates.

Explicitly out of scope:

- frequency ratios/tuning, WP5.3;
- final level/mod-index UX mappings, WP5.4;
- feedback, WP5.5;
- envelopes, WP5.6;
- FM voice/D048 router integration, WP5.7;
- FM pages/p-lock IDs, WP5.8;
- simultaneous audible 8-BIT + FM4, WP5.9.

Exit criteria:

- D050 contract frozen before production source;
- algorithm IDs/topologies/evaluation order exact;
- deterministic modulation accumulation and carrier sum semantics proven;
- graph vectors and long hashes repeat-identical;
- no heap/float/runtime 64-bit division/unbounded traversal;
- pinned PI32v2 compile and object measurement accepted;
- D049 operator vectors remain exact;
- full app remains exact WP5.0 while routing stays isolated;
- durable `WP5_2_VERIFICATION.md` written.

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

PLANNED after engine foundations. Must retain continuous PLAY during performance/capture, deterministic capture mapping, safe track switching, drums, transport-locked metronome, bounded CHANNEL/GLOBAL automation and performance FX, and measured polyphony without silently reducing eight logical tracks.

Reference: `LIVE_PERFORMANCE_CAPTURE_VISION.md`.

## Phase 6 - song/order/persistence - PLANNED

Order list, safe pattern switching, repeats/loops, versioned binary format, flash audit, save/load/CRC, corruption recovery and migration harness. Persistence inherits D047/D048 per-instrument routing semantics.

## Phase 7 - physical hardware UX freeze - BLOCKED

Requires physical FM-1 for control feel, install/boot/audio, display and measured callback margin.

## Phase 8 - interoperability - PLANNED

Internal interchange, MIDI investigation, Furnace import/export audit and converter prototype if justified. D003 remains authoritative: Furnace is a workflow/interoperability reference, not a full engine port.

## Phase 9 - optional extensions - DEFERRED

Possible later engines/features only after explicit CPU/RAM/parameter/license review.

`hardware_margin_claim=none` until physical hardware evidence exists.
