# FM1Tracker Work Packages

This document translates `ROADMAP.md` into bounded implementation units. The roadmap defines direction; this file defines what may be changed now.

## Rules

1. Work on one active package at a time unless a dependency genuinely requires otherwise.
2. A package is complete only when every exit criterion passes or a waiver is explicitly recorded.
3. Do not pull later-phase features forward for convenience.
4. Firmware behavior changes require tests/benchmarks appropriate to that layer.
5. Update `PROJECT_STATE.md` when a package starts, blocks or completes.
6. Architectural deviations must be recorded before they spread into code.
7. Third-party code/assets require provenance/license review in `THIRD_PARTY.md` before import.
8. Detailed completed-package evidence belongs in `WP*_VERIFICATION.md`; this file stays focused on execution order and boundaries.
9. Historical verification contracts are not rewritten to imply later architecture existed earlier. Superseding decisions change future work only unless an explicit migration package says otherwise.
10. No later work package may silently weaken the eight-logical-track baseline, realtime boundedness, deterministic tracker timing or physical-hardware caveats.

---

# Phase 1 - clean Dinghy baseline

Status: **COMPLETE - 2026-10-23**

- WP1.1 upstream provenance freeze - COMPLETE
- WP1.2 minimal complete Dinghy import - COMPLETE
- WP1.3 pristine baseline verification - COMPLETE
- WP1.4 FM1Tracker identity rename - COMPLETE
- WP1.5 renamed baseline verification and budget capture - COMPLETE

Evidence: `WP1_3_VERIFICATION.md`, `WP1_4_VERIFICATION.md`, `WP1_5_VERIFICATION.md`.

Final Phase 1 baseline: 55,088 B image, 12,368 B normal RAM, 30,720 B pool RAM, identity `FM-1_987`.

---

# Phase 2 - tracker core

Status: **COMPLETE - 2026-10-23**

- WP2.1 compile-time data model - COMPLETE - issue #9
- WP2.2 bounds-safe pattern store - COMPLETE - issue #10
- WP2.3 integer transport clock - COMPLETE - issue #11
- WP2.4 one-track audible playback - COMPLETE - issue #12
- WP2.5 16-step page input - COMPLETE - issue #13
- WP2.6 track selection prototype - COMPLETE - issue #14
- WP2.7 STEP REC note entry - COMPLETE - issue #15
- WP2.8 REST / TIE / NOTE-OFF semantics - COMPLETE - issue #16
- WP2.9 compact tracker screen - COMPLETE - issue #17
- WP2.10 Phase 2 integration test and closeout - COMPLETE - issue #18

Core Phase 2 result:

- 8 tracks x 64 steps x 16 patterns
- `FmtStep` exactly 8 B
- one 65,896 B pool-backed `FmtSong`
- exact integer transport
- deterministic NOTE/REST/TIE/OFF STEP REC
- provisional white-key step pages / black-key track selection
- bounded tracker screen

Evidence: `WP2_1_VERIFICATION.md` through `WP2_9_VERIFICATION.md`, `WP2_10_TEST_PLAN.md`, `PHASE2_VERIFICATION.md`.

---

# Phase 3 - p-locks and tracker effects

Status: **COMPLETE - 2026-10-24**

Parent issue: #20

- WP3.1 stable parameter ID registry - COMPLETE - issue #19 - D027
- WP3.2 sparse four-lock-per-step storage - COMPLETE - issue #21 - D028
- WP3.3 hold-step + encoder p-lock editing - COMPLETE - issue #22 - D029
- WP3.4 p-lock browse / single-lock delete UX - COMPLETE - issue #23 - D030
- WP3.5 basic pitch/volume p-lock playback - COMPLETE - issue #24 - D031
- WP3.6 arpeggio / slide / portamento / vibrato effects - COMPLETE - issue #25 - D032
- WP3.7 retrig / note delay / note cut - COMPLETE - issue #26 - D033
- WP3.8 bounded event queue and overflow policy - COMPLETE - issue #27 - D034
- WP3.9 lock/effect visual feedback - COMPLETE - issue #28 - D035
- WP3.10 Phase 3 worst-case tests and exit gate - COMPLETE - issue #29 - D036

Final Phase 3 baseline:

- image 66,960 B
- normal RAM 13,296 B
- pool RAM 113,000 B
- global sparse lock pool 2,048 x 8 B = 16,384 B
- exact 32-entry realtime queue
- exact six-tick tracker-effect clock
- 4,096-row / 64-wrap worst-case tracker/effect proof
- run `40084706845`, job `120845144453`

Evidence: `WP3_1_VERIFICATION.md` through `WP3_10_VERIFICATION.md` plus package contracts/test plans.

---

# Phase 4 - first engine family: 8-BIT/chip

Status: **COMPLETE - 2026-10-24**

Parent issue: #30

- WP4.1 engine interface contract - COMPLETE - issue #31 - D037
- WP4.2 PULSE - COMPLETE - issue #32 - D038
- WP4.3 NOISE - COMPLETE - issue #33 - D039
- WP4.4 TRIANGLE - COMPLETE - issue #34 - D040
- WP4.5 WAVETABLE-8 - COMPLETE - issue #35 - D041
- WP4.6 common envelope primitive - COMPLETE - issue #36 - D042
- WP4.7 deterministic voice allocation/stealing - COMPLETE - issue #37 - D043
- WP4.8 four-parameter pages + p-lock mapping - COMPLETE - issue #39 - D044
- WP4.9 linked 8-BIT engine integration - COMPLETE - issue #40 - D045
- WP4.10 representative eight-track benchmark + Phase 4 closeout - COMPLETE - issue #41 - D046

WP4.10 final closeout:

- production remains byte-identical to `c93c39ad344760b4da0dfbc76829ca5fef886174`
- canonical verification head `2379d3e7e52d1cefe9f845f8287ad0ccda8c019a`
- run `40124536375`
- job `120963022289`
- representative WAV SHA-256 `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9`
- complete 2,048-lock / 512-owner workload
- 4,096-row / 64-wrap repeat-identical linked long run
- exact D033 action and D043 allocator outcome counts
- zero active cross-track steals
- structural realtime audit green
- inherited regressions green
- pinned target artifacts/resources exact
- `hardware_margin_claim=none`

Final linked target footprint: 72,504 B image / 19,604 B normal RAM / 113,000 B pool.

Evidence: `WP4_1_VERIFICATION.md` through `WP4_9_VERIFICATION.md`, `WP4_10_PHASE4_EXIT_CONTRACT.md`, `WP4_10_VERIFICATION.md`.

D047 does not rewrite Phase 4 evidence. The all-8-BIT path is the first complete engine family and remains the regression baseline for mixed-engine work.

---

# Phase 5 - mixed-engine routing + FM4

Status: **IN PROGRESS - WP5.0 active**

Normative future direction: `D047_MIXED_ENGINE_ARCHITECTURE.md`.

## WP5.0 - mixed-engine routing contract + migration - ACTIVE

This package is mandatory before FM4 production integration.

Allowed scope:

- freeze per-instrument family + engine as authoritative routing identity;
- freeze the new non-gating meaning of `FmtSongMeta.mode` while preserving current layout until persistence migration is explicitly designed;
- allow different tracks to route different engine families simultaneously;
- allow deterministic same-track instrument changes across families;
- preserve at most one owned primary sequenced voice per logical track across all families;
- forbid active cross-track primary stealing;
- measure and freeze the cross-family ownership/allocator representation;
- keep dispatch static and bounded, with no plugin/dynamic-allocation architecture;
- freeze the common publication/parameter adapter boundary;
- define safe behavior for non-applicable engine-specific p-lock IDs;
- preserve the complete all-8-BIT D046 path as a regression baseline.

Required execution order:

1. create/freeze the WP5.0 normative contract;
2. capture focused pre-migration tests for current project-wide family-gate behavior;
3. freeze expected mixed-family routing/ownership semantics;
4. only then change production routing;
5. run all inherited D046 regressions plus new mixed-family tests;
6. measure target resource delta and close WP5.0 before starting FM4 primitives.

Explicitly out of scope until WP5.0 closes:

- FM4 operator implementation;
- FM algorithms;
- new FM parameter IDs/pages;
- new persistence format;
- live performance capture implementation;
- optional modern engine families.

## WP5.1 - FM operator primitive

- four fixed-point operators;
- deterministic phase/level primitive;
- no float and no runtime 64-bit division;
- host vectors + target compile before algorithm routing.

## WP5.2 - curated FM algorithm routing

- freeze a small musically useful algorithm set;
- bounded static graph/routing only;
- exact deterministic fixtures per algorithm.

## WP5.3 - ratio table

- precomputed integer ratio choices;
- deterministic pitch/tuning fixtures across supported range;
- no unbounded runtime math.

## WP5.4 - levels / modulation index

- bounded integer operator-level and modulation-index mappings;
- exact saturation/edge behavior.

## WP5.5 - bounded feedback

- feedback only on explicitly frozen operator/subset;
- deterministic bounded integer behavior;
- overflow/stability stress.

## WP5.6 - FM envelope strategy

- freeze shared/common versus per-operator envelope strategy from measured cost;
- deterministic NOTE/TIE/RETRIG/OFF semantics.

## WP5.7 - FM voice allocation/integration

- integrate FM family into the WP5.0 router/ownership model;
- deterministic bounded voice ownership;
- no active cross-track primary stealing;
- measured polyphony, never silently reduce eight logical tracker tracks.

## WP5.8 - FM parameter pages + p-lock mapping

- stable collision-safe D027 semantic parameter IDs;
- bounded four-control pages;
- safe non-applicable parameter behavior;
- deterministic row publication/p-lock fixtures.

## WP5.9 - mixed 8-BIT + FM4 validation

- simultaneous 8-BIT + FM4 tracks;
- same-track cross-family instrument switching;
- pitch/modulation/p-lock precedence;
- exact all-8-BIT regression equivalence retained.

## WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit

- representative mixed-family workload;
- structural realtime bound audit;
- inherited Phase 1-4 regressions;
- exact target image/RAM/pool/artifact evidence;
- no physical execution-margin claim without physical FM-1.

---

# Mandatory core milestone - live performance capture + performance FX

Status: **PLANNED after engine foundations; package IDs deferred until Phase 5 measurements exist**

This is core product work, not optional Phase 9 work.

Must include:

- PLAY continues while user performs selected track;
- REC can arm/disarm capture without stopping transport;
- deterministic live-note time mapping;
- seamless track switching without misrouting writes or cutting unrelated tracks;
- deterministic drum performance/capture representation;
- transport-locked metronome;
- bounded CHANNEL and GLOBAL performance/automation ownership;
- key-distributed performance FX with tracker-editable captured data;
- bounded sparse parameter/FX capture rather than unbounded ADC streams;
- measured reference setup supporting drums, bass, poly, lead and flexible tracks;
- target 3-6 voices for at least one polyphonic instrument/track if real budgets allow, without silently reducing eight logical tracks.

Open decisions are frozen only when this milestone begins: quantization/timestamp policy, overwrite/merge/overdub, thinning/interpolation, drum representation/layout, count-in, GLOBAL automation storage, FX key layout and actual polyphony.

Reference: `LIVE_PERFORMANCE_CAPTURE_VISION.md`.

---

# Phase 6 - song/order/persistence

Status: **PLANNED**

- WP6.1 order-list model
- WP6.2 safe pattern switching
- WP6.3 repeats / loop ranges
- WP6.4 binary format v1
- WP6.5 flash audit
- WP6.6 save
- WP6.7 load + CRC
- WP6.8 corrupted-data recovery
- WP6.9 migration/version harness
- WP6.10 power-cycle round trip

Persistence must not be designed around obsolete project-wide engine-mode assumptions. It must inherit D047/WP5.0 routing semantics.

---

# Phase 7 - physical hardware UX freeze

Status: **BLOCKED until physical FM-1 is available**

- WP7.1 STEP REC / REST / TIE / OFF hardware test
- WP7.2 step-page ergonomics
- WP7.3 track-selection ergonomics
- WP7.4 p-lock ergonomics
- WP7.5 chord behavior
- WP7.6 tracker-screen readability
- WP7.7 V1 control-map freeze

Also validate live PLAY/REC, track switching during capture, drums/poly behavior, metronome, CHANNEL/GLOBAL FX/automation, physical install/boot/audio and measured execution margin.

Provisional white-key step pages and black-key track selection remain provisional until this phase.

---

# Phase 8 - interoperability

Status: **PLANNED**

- internal interchange specification;
- MIDI transfer investigation;
- Furnace export/import audit;
- converter prototype if justified;
- explicit round-trip/non-round-trip decision.

D003 remains authoritative: Furnace is a workflow/interoperability reference, not a full engine port.

---

# Phase 9 - optional extensions

Status: **OPTIONAL / DEFERRED**

Candidates only after core product milestones and explicit budget/license review:

- WAVETABLE-16;
- PCM/sample engines beyond approved core drum needs;
- swing;
- microtiming;
- polymeter;
- additional VA/supersaw/wavetable families;
- extra DSP effects;
- licensed chip cores.

Mixed engine families are no longer a Phase 9 concept: D047 promotes the routing architecture itself into WP5.0. Individual additional engine families still require their own bounded work packages.
