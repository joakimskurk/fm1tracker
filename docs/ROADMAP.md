# FM1Tracker Roadmap

FM1Tracker is a hardware-first tracker/sequencer firmware for the M-VAVE FM-1.

The roadmap defines product direction and phase order. `WORK_PACKAGES.md` defines the bounded package that may be changed now. `PROJECT_STATE.md` is the execution source of truth.

## Product north star

**Tracker depth underneath, immediate capture on top.**

The instrument should feel playable before it feels editable:

- play first;
- capture when inspiration happens;
- tracker data remains the source of truth;
- editing remains deterministic and compact;
- engine complexity must never destabilize transport, p-lock timing or live capture;
- eight logical tracker tracks remain the baseline unless an explicit future product decision changes it.

A reference performance setup should eventually accommodate:

- drums;
- bass;
- one polyphonic synth role;
- one lead role;
- four additional flexible tracks.

Roles are not fixed track types.

If measured CPU/RAM permits, target roughly 3-6 voices for at least one polyphonic instrument/track without silently reducing the eight logical tracker tracks.

## Current status

- Phase 1 clean platform baseline: COMPLETE
- Phase 2 tracker core: COMPLETE
- Phase 3 p-locks/effects/runtime ingress: COMPLETE
- Phase 4 first complete 8-BIT engine family: COMPLETE at host/device-build level
- Phase 5 mixed-engine routing + FM4: IN PROGRESS, WP5.0 active
- mandatory live performance capture milestone: PLANNED after engine foundations
- Phase 6 song/order/persistence: PLANNED
- Phase 7 physical hardware UX freeze: BLOCKED until physical FM-1 is available
- Phase 8 interoperability: PLANNED
- Phase 9 optional extensions: DEFERRED

Canonical Phase 4 closeout is `docs/WP4_10_VERIFICATION.md`, run `40124536375`, job `120963022289`.

## Architecture direction after D047

FM1Tracker is **one engine-agnostic tracker/sequencer**.

Engine family is not a project-wide mode. Instrument identity owns family + engine, and tracker rows/tracks reference instruments. This permits future mixed configurations such as 8-BIT voices, FM4 and later explicitly approved families in one project, subject to measured budgets.

D047 supersedes only the old future project-wide 8-BIT/16-BIT restriction. It does not rewrite historical Phase 1-4 verification.

The complete all-8-BIT D046 configuration remains the regression baseline for all mixed-engine work.

## Phase 0 / 1 - foundation and clean platform baseline - COMPLETE

Established:

- hardware-first architecture boundary;
- upstream provenance and GPL-3.0-compatible project baseline;
- fixed package identity `FM-1_987`;
- pinned JieLi/AC79 build inputs;
- resource and HAL/build safety checks.

## Phase 2 - tracker core - COMPLETE

Established:

- 8 logical tracks;
- 64 steps per pattern;
- 16 pattern slots;
- 8-byte `FmtStep`;
- exact integer transport;
- STEP REC;
- NOTE/REST/TIE/OFF semantics;
- provisional 16-white-key step pages;
- provisional eight-black-key track selection;
- compact bounded tracker screen.

The physical key mappings remain provisional until Phase 7 hardware validation.

## Phase 3 - p-locks and tracker effects - COMPLETE

Established:

- stable semantic parameter IDs;
- global 2,048-record sparse p-lock pool;
- held-step edit/browse/delete;
- row-local publication snapshots;
- exact six-tick tracker-effect timing;
- ARP/SLIDE/PORTA/VIBRATO;
- RETRIG/NOTE DELAY/NOTE CUT;
- bounded 32-entry main-to-audio audition queue;
- bounded visual feedback;
- full worst-case Phase 3 closeout.

## Phase 4 - first complete engine family: 8-BIT/chip - COMPLETE

The first engine family contains:

- PULSE;
- NOISE;
- TRIANGLE;
- WAVETABLE-8;
- common integer ADSR;
- deterministic eight-slot voice ownership/allocation;
- engine parameter pages and stable p-lock mappings;
- linked playback across all eight tracker tracks.

D046 exit verification proves:

- complete 2,048-lock representative workload;
- all 8 tracks and all 4 engines;
- 4,096 rows / 64 wraps, repeat-identical;
- deterministic D033 action and D043 allocation counts;
- zero active cross-track steals;
- D034 overflow/recovery while tracker runs;
- structural realtime boundedness;
- inherited regressions;
- exact pinned target resources/artifacts.

Production remains frozen to `c93c39ad344760b4da0dfbc76829ca5fef886174`.

No physical FM-1 was available, so Phase 4 makes no measured hardware execution-margin claim.

## Phase 5 - mixed-engine routing + FM4 - IN PROGRESS

### WP5.0 - mixed-engine routing contract + migration - ACTIVE

Before FM4 implementation, freeze and prove the common router:

- per-instrument family + engine is authoritative;
- `FmtSongMeta.mode` no longer acts as a global playback family gate;
- multiple engine families may coexist across tracks;
- one track may switch instruments/families deterministically;
- one owned primary sequenced voice per logical track across all families;
- no still-owned active cross-track primary voice stealing;
- bounded static dispatch, no dynamic plugin system;
- measured ownership/allocator representation;
- common publication/parameter adapter boundary;
- non-applicable p-lock IDs fail safely and are never reinterpreted;
- complete all-8-BIT D046 behavior remains regression-identical.

WP5.0 is a mandatory architecture migration. Do not start FM4 production source until its contract and focused tests are frozen.

### WP5.1-WP5.10 - FM4

Planned sequence:

1. FM operator primitive
2. curated algorithm routing
3. ratio table
4. operator levels / modulation index
5. bounded feedback
6. envelope strategy
7. voice allocation/integration
8. parameter pages + p-lock mapping
9. mixed 8-BIT + FM4 validation
10. worst-case mixed-engine benchmark and Phase 5 exit

FM4 principles:

- compact fixed-point four-operator FM;
- small curated algorithm set rather than unrestricted routing matrix;
- precomputed integer ratios;
- bounded modulation/feedback;
- common envelope first unless measured budget justifies more;
- musically useful controls rather than superficial DX-style completeness;
- no float in firmware DSP;
- no runtime 64-bit division;
- never sacrifice tracker timing or deterministic ownership for synth complexity.

When resource pressure appears, reduce in this order where appropriate:

1. control-rate work;
2. runtime math through tables/shifts;
3. interpolation/modulation complexity;
4. simultaneous engine voice count only through an explicit measured decision;
5. algorithm complexity;
6. defer the engine/feature rather than destabilize core tracker behavior.

Eight logical tracker tracks are not silently reduced.

## Mandatory core milestone - live performance capture + performance FX

This milestone is part of the core product path. It is not optional Phase 9 work.

Two workflows must coexist:

### Precise stopped STEP REC

Already established in Phase 2 and retained.

### Live capture while transport runs

Target behavior:

- playback continues while the user plays the selected track;
- pressing REC while playback runs arms capture without stopping transport;
- captured notes become deterministic tracker data;
- pressing REC again disarms capture while playback continues;
- track switching is fast and does not reset transport or cut unrelated tracks;
- subsequent capture always targets the newly selected track and never writes back to the old track;
- UI makes the current capture target unmistakable.

### Drums

The final design must support multi-drum keyboard performance, live audition and deterministic capture. Exact tracker representation and keyboard layout are intentionally not frozen yet.

### Metronome

Must:

- be independently enabled;
- use the same transport clock;
- never drift from tracker timing;
- not consume a logical track by default.

Count-in remains a later explicit decision.

### Performance FX and automation

The physical concept is rhythmically playable effects distributed across keys/buttons, with pots controlling the selected effect.

Captured gestures must remain tracker-editable and bounded.

Two scopes are expected:

- CHANNEL: belongs to a logical track/instrument context;
- GLOBAL: affects the shared mix/performance context.

GLOBAL automation may require its own bounded representation instead of forcing it into per-track p-lock storage.

REC must not record an unbounded raw ADC stream. Notes, parameters and FX should become sparse/bounded tracker data.

Open decisions for this milestone:

- live-note quantization/timestamp policy;
- overwrite vs merge vs overdub;
- automation thinning/interpolation;
- drum representation/layout;
- count-in behavior;
- GLOBAL automation storage/ownership;
- performance-FX key layout;
- actual polyphony from measured Phase 5 and physical hardware budgets.

Reference: `LIVE_PERFORMANCE_CAPTURE_VISION.md`.

## Phase 6 - song/order/persistence

Planned:

- order-list model;
- safe pattern switching;
- repeats and loop ranges;
- binary format v1;
- flash/write-path audit;
- save;
- load + CRC;
- corrupted/truncated/future-data recovery;
- version/migration harness;
- power-cycle round trip.

Persistence must encode the post-D047/WP5.0 engine-routing model, not obsolete project-wide family assumptions.

## Phase 7 - physical hardware UX freeze

Requires a physical FM-1.

Validate and freeze:

- STEP REC feel;
- REST/TIE/OFF gestures;
- 16-step white-key pages;
- black-key track selection;
- p-lock editing/deletion ergonomics;
- chord behavior;
- tracker readability on 240x240;
- live PLAY/REC and track switching;
- drums/poly behavior;
- metronome;
- CHANNEL/GLOBAL performance FX;
- physical install/boot/audio behavior;
- measured worst-case AC791N execution margin.

This phase produces the V1 physical control-map freeze.

## Phase 8 - interoperability

Investigate only after core tracker/engine/persistence behavior is stable:

- internal interchange specification;
- MIDI transfer possibilities;
- Furnace export/import audit;
- converter prototype if legally and technically appropriate;
- explicit round-trip/non-round-trip decision.

D003 remains authoritative: Furnace is a workflow/interoperability reference, not a plan to port the complete Furnace engine/codebase.

## Phase 9 - optional extensions

Deferred candidates:

- WAVETABLE-16;
- PCM/sample engines beyond approved core drum needs;
- swing;
- microtiming;
- polymeter;
- additional VA/supersaw/wavetable families;
- extra DSP effects;
- licensed chip cores.

Every additional engine remains a bounded family package with CPU/RAM/parameter/licensing gates.

## Illustrative release progression

These labels describe capability milestones, not guaranteed public version numbers:

- 0.1 host tracker skeleton
- 0.2 FM-1 STEP REC
- 0.3 visible/p-lock tracker
- 0.4 complete 8-BIT family
- 0.5 mixed-engine routing + FM4
- 0.6 live performance capture
- 0.7 song/order/save/load
- 1.0 physical hardware UX and control-map freeze

## Non-negotiable roadmap rules

- tracker remains the source of truth;
- transport/effect timing stays deterministic;
- realtime work stays structurally bounded;
- no silent reduction from eight logical tracks;
- physical-hardware claims require physical hardware evidence;
- architectural changes are recorded before implementation;
- one active work package at a time;
- completed regressions remain active unless explicitly superseded by a recorded decision.
