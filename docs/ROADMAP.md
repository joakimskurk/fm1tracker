# FM1Tracker Roadmap

FM1Tracker is a hardware-first tracker/sequencer firmware for the M-VAVE FM-1.

`PROJECT_STATE.md` defines what is active now. `WORK_PACKAGES.md` defines bounded implementation scope. This file defines product and phase direction.

## Product north star

**Tracker depth underneath, immediate capture on top.**

The instrument should feel playable before it feels editable:

- play first;
- capture when inspiration happens;
- tracker data remains the source of truth;
- editing remains deterministic and compact;
- engine complexity must never destabilize transport, p-lock timing or live capture;
- eight logical tracker tracks remain the baseline unless an explicit future product decision changes it.

A reference performance setup should eventually accommodate drums, bass, one polyphonic synth role, one lead role and four additional flexible tracks. Roles are not fixed track types.

If measured CPU/RAM permits, target roughly 3-6 voices for at least one polyphonic instrument/track without silently reducing eight logical tracks.

## Current status

- Phase 1 clean platform baseline: COMPLETE
- Phase 2 tracker core: COMPLETE
- Phase 3 p-locks/effects/runtime ingress: COMPLETE
- Phase 4 first complete 8-BIT engine family: COMPLETE
- Phase 5 mixed-engine routing + FM4: IN PROGRESS
  - WP5.0 mixed-engine routing: COMPLETE
  - WP5.1 fixed-point FM operator primitive: ACTIVE
- mandatory live performance capture milestone: PLANNED after engine foundations
- Phase 6 song/order/persistence: PLANNED
- Phase 7 physical hardware UX freeze: BLOCKED until physical FM-1 is available
- Phase 8 interoperability: PLANNED
- Phase 9 optional extensions: DEFERRED

Canonical WP5.0 closeout: `docs/WP5_0_VERIFICATION.md`, run `40129822006`, job `120978955451`.

## Architecture direction after D047/D048

FM1Tracker is **one engine-agnostic tracker/sequencer**.

Engine family is not a project-wide mode. Instrument identity owns family + engine, and tracker rows/tracks reference instruments.

D048 production migration is complete:

- `FmtSongMeta.mode` no longer gates valid playback;
- instrument identity is published through a fixed 64 B bank;
- global family route state is fixed at 8 B;
- dispatch is static/bounded;
- same-track cross-family replacement has explicit fail-safe semantics;
- family-local engines retain their own allocation/state;
- the all-8-BIT D046 configuration remains byte-identical regression evidence.

The architecture can support later explicitly approved families such as FM4, VA, supersaw, modern wavetable or drums, but each family requires its own bounded CPU/RAM/parameter/licensing work package. D047 does not promise unlimited simultaneous polyphony.

## Phase 1 - foundation - COMPLETE

Pinned upstream provenance, fixed `FM-1_987` identity, JieLi/AC79 toolchain path and resource/safety baseline.

## Phase 2 - tracker core - COMPLETE

8 tracks x 64 steps x 16 patterns, exact 8 B `FmtStep`, integer transport, stopped STEP REC, NOTE/REST/TIE/OFF semantics, provisional hardware controls and bounded tracker display.

## Phase 3 - p-locks/effects/runtime ingress - COMPLETE

Stable semantic parameter IDs, 2,048 sparse p-locks, editor/browse/delete, row snapshots, exact six-tick effects, delayed/retrig/cut scheduling, 32-entry realtime queue and worst-case closeout.

## Phase 4 - first engine family: 8-BIT/chip - COMPLETE

PULSE, NOISE, TRIANGLE, WAVETABLE-8, common integer ADSR, deterministic allocator, parameter pages and linked eight-track playback.

Canonical D046 run `40124536375`, job `120963022289`.

## Phase 5 - mixed-engine routing + FM4 - IN PROGRESS

### WP5.0 - mixed-engine routing - COMPLETE

D047/D048 are implemented and verified. Final target is 72,792 B image / 19,676 B RAM / 113,000 B pool, only +288 B / +72 B / +0 B versus D046.

No FM4 DSP was introduced in WP5.0.

### WP5.1 - fixed-point FM operator primitive - ACTIVE

Issue #44.

Contract-first goals:

- freeze exact integer operator state and phase format;
- freeze deterministic integer carrier lookup;
- freeze signed phase-modulation input;
- freeze bounded operator level/output behavior;
- freeze reset and overflow/saturation semantics;
- create exact host vectors before implementation;
- prove no float, heap or runtime 64-bit division;
- compile with pinned JieLi/AC79 target and measure exact resource delta.

WP5.1 deliberately excludes algorithms, ratios, feedback, envelopes, voice integration, p-lock pages and mixed production FM playback.

### WP5.2 - curated algorithm routing

Small musically useful static algorithm set with exact deterministic fixtures.

### WP5.3 - ratio table

Precomputed integer ratio choices and tuning fixtures.

### WP5.4 - levels / modulation index

Bounded integer operator-level and modulation-index mappings.

### WP5.5 - bounded feedback

Explicit topology, deterministic integer behavior and stability/overflow stress.

### WP5.6 - envelope strategy

Freeze common versus per-operator strategy from measured cost.

### WP5.7 - FM voice allocation/integration

Integrate FM family into D048 ownership without active cross-track primary stealing.

### WP5.8 - FM parameter pages + p-lock mapping

Stable semantic IDs, bounded pages and safe non-applicable parameter behavior.

### WP5.9 - mixed 8-BIT + FM4 validation

Simultaneous families, same-track switching and full all-8-BIT regression equivalence.

### WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit

Representative mixed workload, realtime audit, inherited regressions and exact target closeout.

## Mandatory core milestone - live performance capture + performance FX

This remains core product work after engine foundations.

Target behavior:

- PLAY continues while performing;
- REC arms/disarms capture without stopping transport;
- captured notes map deterministically to tracker time;
- fast track switching cannot misroute writes or cut unrelated tracks;
- drums support live performance and deterministic capture;
- metronome is transport-locked and does not consume a logical track by default;
- CHANNEL and GLOBAL performance FX/automation remain bounded and tracker-editable;
- raw unbounded ADC streams are never recorded as automation.

Open decisions remain quantization/timestamp policy, overdub policy, automation thinning, drum representation, count-in, global automation storage, FX layout and actual polyphony from measured budgets.

Reference: `LIVE_PERFORMANCE_CAPTURE_VISION.md`.

## Phase 6 - song/order/persistence - PLANNED

Order list, pattern switching, repeats/loop ranges, versioned binary format, flash audit, save/load/CRC, corruption recovery and migration harness.

Persistence must encode D047/D048 per-instrument engine identity, not obsolete project-wide family assumptions.

## Phase 7 - physical hardware UX freeze - BLOCKED

Requires physical FM-1 for STEP REC/control ergonomics, screen readability, live capture feel, install/boot/audio behavior and measured AC791N execution margin.

## Phase 8 - interoperability - PLANNED

Internal interchange specification, MIDI transfer investigation, Furnace import/export audit and converter prototype if technically and legally justified.

D003 remains authoritative: Furnace is a workflow/interoperability reference, not a full engine port.

## Phase 9 - optional extensions - DEFERRED

Possible later additions include WAVETABLE-16, approved PCM/sample paths, swing, microtiming, polymeter, additional VA/supersaw/wavetable families, extra DSP effects and licensed chip cores.

Every additional engine remains a bounded family package with CPU/RAM/parameter/licensing gates.

## Illustrative release progression

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
- physical-hardware claims require physical evidence;
- architecture changes are recorded before implementation;
- one active work package at a time;
- completed regressions remain active unless explicitly superseded.

`hardware_margin_claim=none` until physical hardware evidence exists.
