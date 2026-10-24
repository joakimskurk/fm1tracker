# FM1Tracker V0.1 Architecture Contract

Status: **historical Phase 0 baseline, amended by D047 for engine routing**

This document defines the original boundaries used to build the first playable firmware. The tracker/platform/concurrency/data-budget rules remain useful. The original project-wide `8-BIT` versus `16-BIT` sound-mode restriction is **superseded by D047**.

Current normative mixed-engine direction: `D047_MIXED_ENGINE_ARCHITECTURE.md`.

Historical Phase 4 contracts D037-D046 remain authoritative for what the first 8-BIT engine family actually implemented and verified.

## 1. Product definition

FM1Tracker is a hardware-first tracker/sequencer for the M-VAVE FM-1.

It is not a desktop tracker squeezed onto a small screen and it is not a full Furnace port. The FM-1 keyboard, buttons and encoders are part of the tracker itself.

The core loop remains:

1. choose a track/instrument;
2. enter STEP REC;
3. play a note or chord according to the current recording policy;
4. advance automatically;
5. enter REST/TIE/OFF when needed;
6. hold/select a step and turn encoders to add per-step locks;
7. press PLAY and immediately hear the pattern.

D047 adds one product rule: the chosen instrument determines the sound-engine family. A project is not globally locked to one sound family.

## 2. Platform baseline

Target baseline: Dinghy/Felucca/SLOOP platform.

Relevant FM-1 constraints exposed by Dinghy:

- JieLi AC791N / pi32v2 toolchain;
- no floating-point firmware DSP;
- no runtime 64-bit division;
- 44.1 kHz audio callback, normally 256 stereo frames;
- nominal callback interval about 5.8 ms;
- 96 KiB normal RAM (`.data` + `.bss`);
- 336 KiB-class pool RAM for large buffers in the original platform description, with exact linker limits captured by package verification;
- 568 KiB-class code/constants region in the original platform description, with exact linker limits captured by package verification;
- 240 x 240 display;
- 27 note keys;
- 14 buttons;
- 7 encoders plus MASTER;
- USB MIDI and USB audio supplied by the platform;
- host mode available for screen/audio tests without physical hardware.

Hard rule: the audio ISR must not draw, access flash, wait, allocate dynamically or perform unbounded work.

## 3. Layering

The conceptual layering remains:

```text
UI / PANEL
    |
TRACKER EDITOR
    |
SEQUENCER / TRANSPORT
    |
INSTRUMENT ROUTER
    |
VOICE + ENGINE FAMILIES
    |
DINGHY PLATFORM / HAL
```

The `INSTRUMENT ROUTER` layer is the D047 amendment. Phase 4 effectively had one family behind that boundary; WP5.0 must make the boundary explicit before FM4 is linked.

### UI / panel

Owns button/key/encoder interpretation, screen state and dirty-region drawing. It never mutates audio voice internals directly.

### Tracker editor

Owns song/pattern/step edits, cursor state, STEP REC behavior, REST/TIE/OFF insertion and parameter-lock editing.

### Sequencer / transport

Owns musical time, pattern row dispatch, order playback and conversion of step data into bounded runtime actions/snapshots for the audio side.

The sequencer must not know how a PULSE oscillator or FM operator renders audio.

### Instrument router

Resolves tracker instrument identity to a bounded engine-family + engine adapter. It owns cross-family routing/primary-track ownership rules but not tracker timing.

D047 requires:

- per-instrument engine-family selection;
- simultaneous different families across logical tracks;
- at most one owned primary sequenced voice per track across all families;
- deterministic family switching;
- no active cross-track primary stealing;
- static bounded dispatch, not a desktop plugin system.

Exact runtime representation is WP5.0 work.

### Voice + engine families

Own synthesis state. Each family consumes compact, bounded semantic actions/snapshots and renders fixed-point audio. No tracker/UI logic belongs here.

The already-verified Phase 4 8-BIT family remains one such family. FM4 becomes another after WP5.0.

### Platform / HAL

Dinghy/Felucca/SLOOP code. Keep upstream changes traceable. Avoid changing the HAL unless the app cannot be implemented cleanly above it.

## 4. Concurrency contract

The main loop and audio ISR communicate through bounded, explicitly owned data.

General direction:

```text
main/editor -> song/cache publication -> sequencer/audio-owned runtime
main/audition -> bounded D034 queue -> audio
```

Rules:

- one writer per shared field;
- naturally aligned 32-bit publication where appropriate;
- no shared mutable structs that can be observed half-written;
- all queues have fixed capacity and defined overflow behavior;
- audio never allocates memory;
- expensive table/coefficient/cache resolution happens outside per-sample loops where possible;
- screen refresh is dirty-region based and main-loop only;
- adding an engine family may not add a parallel tracker clock or a private unbounded event system.

## 5. Sequencer scope

Current baseline:

- 8 logical tracks;
- patterns up to 64 steps;
- four 16-step pages;
- 16 pattern slots resident in pool RAM;
- 128-entry song/order direction for Phase 6;
- manual stopped STEP REC is already implemented;
- parameter locks are first-class data;
- tracker effects/events are engine-independent semantic data.

### D047 amendment

The original clauses:

```text
project-wide 8-BIT or 16-BIT sound mode
mixed 8/16 projects not required
```

are superseded.

The target model is:

```text
track / row -> instrument -> engine family -> engine
```

Different tracks may use different families at the same time. An explicit NOTE-row instrument change may also cross families when the normal instrument semantics request it.

A logical track is still a sequencer lane. Polyphony and engine cost are engine/runtime capabilities and do not change the eight logical-track model.

## 6. Timing model

The transport uses integer/fixed-point timing only.

Current frozen timing foundation:

- 1/16-note grid baseline;
- D020 row phase `661500`;
- D032 effect tick phase `110250`;
- exactly six effect ticks per row with no second tracker clock;
- exact residual-phase/no-drift behavior.

Future swing, general microtiming, alternate track lengths and polymeter remain explicit later work.

Mixed engines must consume this common musical time rather than creating independent sequencer clocks.

## 7. Song data memory budget

The editable song representation belongs in pool RAM, not ordinary `.bss`.

Current proven core:

```text
FmtSong core              65,896 B
D028 lock records         16,384 B
---------------------------------
editable core + locks     82,280 B
```

The existing 8-byte step must not be enlarged merely to support mixed engines; its `instrument` byte already supplies the necessary indirection.

Engine runtime state belongs outside the editable-song step matrix and must be budgeted per family/package.

## 8. Audio resource policy

Common rules:

- integer/fixed-point levels, envelopes and coefficients;
- bounded oscillator/operator state;
- precomputed constant tables where useful;
- saturating/clamped mixing at explicit points;
- no hidden dynamic allocation in the audio callback;
- no float;
- no runtime 64-bit division;
- no unbounded engine registry/plugin traversal.

The original aspirational target of less than roughly half of a 256-frame callback remains a useful hardware goal, but actual execution margin requires physical FM-1 measurement. Host wall time is not hardware proof.

A feature that breaks the measured hardware budget must be simplified, moved to a cheaper update rate, constrained by an explicit deterministic policy, or deferred. It must not silently reduce tracker track count or substitute a different engine.

## 9. Engine-family architecture

### First proven family - 8-BIT/chip

Phase 4 implements:

- PULSE;
- NOISE;
- TRIANGLE;
- WAVETABLE-8;
- common integer ADSR;
- deterministic eight-slot allocator;
- four-control primary pages and p-lock publication.

This family is complete enough to be the regression baseline for WP5.0.

### Next family - FM/digital

The next engine is compact fixed-point FM4:

- four operators;
- curated algorithms;
- integer ratio table;
- modulation levels/index;
- bounded feedback;
- measured envelope/voice strategy;
- engine-specific p-lock page(s).

FM4 does not get linked directly into the Phase 4 engine. WP5.0 first freezes the cross-family router.

### Future families

The architecture must permit later explicit work packages for engines such as:

- modern virtual analog;
- supersaw/unison;
- broader wavetable/digital synthesis;
- drum synthesis;
- PCM/sample engines where storage/profile permits;
- separately licensed/verified authentic chip cores.

These are possibilities, not automatic V1 requirements.

### Category versus project mode

Names such as `8-BIT`, `FM/DIGITAL`, `MODERN` and `DRUM` may be useful browser categories. They are not project types.

## 10. Parameter-lock architecture

P-locks remain first-class song data.

Rules:

- sparse D028 storage;
- maximum four locks on one step;
- stable one-byte semantic parameter IDs;
- parameter identity is independent of UI page position;
- engine-specific scaling is defined by the owning parameter/engine contract;
- changing instrument/engine must ignore non-applicable locks safely and never reinterpret an unrelated ID;
- existing assigned IDs are never renumbered because the engine router becomes mixed-family.

The D027 historical namespace partitions remain valid until explicitly superseded. New engines require explicit collision-safe assignments.

## 11. Tracker effect architecture

The compact step effect slot is independent of engine family.

Current assigned semantics include:

- ARPEGGIO;
- PITCH SLIDE UP/DOWN;
- PORTAMENTO;
- VIBRATO;
- RETRIG;
- NOTE DELAY;
- NOTE CUT.

A new engine family must adapt these semantic actions where applicable without inventing a parallel tracker command domain.

## 12. Persistence

Songs require a dedicated persistence strategy, not raw compiler-struct dumps.

Phase 6 rules remain:

- never erase/write song flash from the audio ISR;
- explicit safe save flow;
- versioned format;
- magic, payload length and CRC;
- safe handling of corrupt/future data;
- explicit byte order and serialization.

D047 adds a requirement: the serialized format must store engine family + engine per instrument and must not impose a project-wide engine-family restriction.

The current runtime `FmtSongMeta.mode` field is no longer the target global playback gate. Its persistent meaning, if any, is decided explicitly in Phase 6 after WP5.0.

## 13. Host-first testing

Host/device-build verification should continue to cover:

- deterministic transport timing;
- STEP REC/special-event behavior;
- p-lock storage/edit/application;
- effect decoding/timing;
- queue overflow/recovery;
- engine primitive vectors;
- screen-state snapshots;
- worst-case engine/resource behavior;
- mixed-family routing once WP5.0 begins;
- same-track cross-family instrument changes;
- no active cross-track steal;
- complete 8-BIT regression equivalence under the mixed router.

Golden WAV/hash tests remain appropriate for stable deterministic engine paths.

## 14. Explicit non-goals / boundaries

Still non-goals unless promoted by explicit work packages:

- full Furnace `.fur` compatibility;
- full Furnace engine/UI port;
- dozens of authentic chip emulators by default;
- sample editor;
- arbitrary runtime plugin architecture;
- unbounded polyphony;
- silent dynamic reduction of logical track count;
- live song save while unsafe audio/flash interaction exists;
- full desktop-style tracker command set.

**Mixed engine families are no longer a non-goal.** D047 makes them core architecture beginning with WP5.0.
