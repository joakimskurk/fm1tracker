# FM1Tracker V0.1 Architecture Contract

Status: **Phase 0 design freeze candidate**

This document defines the boundaries that should remain stable while the first playable firmware is built. Anything marked **provisional** may change after host/hardware testing; everything else should be treated as a contract unless measurements force a revision.

## 1. Product definition

FM1Tracker is a hardware-first tracker/sequencer for the M-VAVE FM-1.

It is not a desktop tracker squeezed onto a small screen and it is not a full Furnace port. The FM-1 keyboard, buttons and encoders are part of the tracker itself.

V0.1 must make this loop fast:

1. choose a track/instrument;
2. enter STEP REC;
3. play a note or chord;
4. advance automatically;
5. press REST when silence is wanted;
6. hold/select a step and turn encoders to add per-step locks;
7. press PLAY and immediately hear the pattern.

## 2. Platform baseline

Target baseline: Dinghy/Felucca/SLOOP platform.

Relevant FM-1 constraints exposed by Dinghy:

- JieLi AC791N / pi32v2 toolchain
- no floating-point firmware DSP
- no runtime 64-bit division
- 44.1 kHz audio callback, normally 256 stereo frames
- 5.8 ms nominal callback interval; target less than half of that for FM1Tracker audio work
- 96 KiB normal RAM (`.data` + `.bss`)
- 336 KiB pool RAM for large buffers
- 568 KiB code + constants
- 240 x 240 display
- 27 note keys
- 14 buttons
- 7 encoders plus MASTER
- USB MIDI and USB audio supplied by the platform
- host mode available for screen/audio tests without physical hardware

Hard rule: the audio ISR must not draw, access flash, wait, or perform unbounded work.

## 3. Layering

The firmware should be split conceptually into five layers even if the first Dinghy build compiles as a small number of C translation units.

```text
UI / PANEL
    |
TRACKER EDITOR
    |
SEQUENCER / TRANSPORT
    |
VOICE + ENGINE LAYER
    |
DINGHY PLATFORM / HAL
```

### UI / panel

Owns button/key/encoder interpretation, screen state and dirty-region drawing. It never mutates audio voice internals directly.

### Tracker editor

Owns song/pattern/step edits, cursor state, STEP REC behavior, REST insertion and parameter-lock editing.

### Sequencer / transport

Owns musical time, pattern row dispatch, order playback and conversion of step data into bounded timestamped events for the audio side.

### Voice + engine layer

Owns synthesis state. It consumes compact events and renders fixed-point audio. No tracker/UI logic belongs here.

### Platform / HAL

Dinghy/Felucca/SLOOP code. Keep upstream changes traceable. Avoid changing the HAL unless the app cannot be implemented cleanly above it.

## 4. Concurrency contract

The main loop and audio ISR communicate through bounded, explicitly owned data.

Preferred direction:

```text
main loop/editor -> sequencer state -> event mailbox/ring -> audio ISR
```

Rules:

- one writer per shared field;
- 32-bit atomic stores where possible;
- no shared mutable structs that can be observed half-written;
- all queues have fixed capacity and defined overflow behavior;
- the audio ISR never allocates memory;
- expensive coefficient/table updates happen outside the per-sample inner loop;
- screen refresh is dirty-region based.

## 5. Sequencer scope

V0.1 target:

- 8 logical tracks
- patterns up to 64 steps
- four 16-step pages
- 16 pattern slots resident in pool RAM
- 128-entry song/order list
- project-wide 8-BIT or 16-BIT sound mode in V0.1
- file/data structures reserve room for future per-track engine families, but mixed 8/16 projects are not a V0.1 requirement
- manual STEP REC while stopped is required before live recording
- parameter locks are required before song mode

A logical track is a sequencer lane. Polyphony is an engine capability and is not assumed to equal the number of tracks.

## 6. Timing model

The transport must use integer/fixed-point timing only.

V0.1 defaults:

- step grid: 1/16 note
- 64 steps = 4 bars in 4/4 at the default grid
- tempo stored as integer BPM plus an optional fractional fixed-point field if measurement shows it is useful
- pattern playback must not drift because of integer truncation; use a phase/error accumulator rather than repeatedly rounding an interval

Provisional for post-V0.1:

- swing
- per-step microtiming
- alternate pattern lengths per track
- polymeter

Tracker effects such as NOTE DELAY can provide controlled off-grid timing before a general microtiming field is added.

## 7. Song data memory budget

The song representation belongs in pool RAM, not ordinary `.bss`.

Target upper bound for the first implementation: **<= 96 KiB pool RAM for all editable song data**, leaving the majority of the 336 KiB pool available for engine state, delay/sample buffers and screen helpers.

A proposed 8-byte step gives:

```text
8 bytes * 64 steps * 8 tracks = 4096 bytes per pattern
4096 * 16 patterns            = 65536 bytes
```

A bounded sparse parameter-lock pool and song metadata should keep the initial song model comfortably below the 96 KiB target.

Do not optimize the step below 8 bytes until profiling shows a real need; predictable access is more valuable than bit-packing every field.

## 8. Audio resource policy

### Common rules

- Q15/Q12 or equivalent fixed-point levels and coefficients
- 32-bit phase accumulators for oscillators
- precomputed constant tables in flash where useful
- control-rate envelopes/LFO/coefficient updates may run every 16 or 32 frames where audibly safe
- saturating/clamped mixing at explicit points
- no hidden dynamic voice allocation in the audio callback

### CPU target

During normal playback, FM1Tracker-owned audio work should target **< 2.9 ms per 256-frame block** on hardware, leaving headroom for USB/key scanning/platform work.

A feature that breaks this budget must be simplified, moved to control rate or rejected.

## 9. Sound-mode architecture

Both modes use the same sequencer, pattern and parameter-lock system.

### 8-BIT

Initial family candidates:

- pulse / PWM
- triangle
- small wavetable
- noise

These are original chip-style engines unless a separately audited emulation core is deliberately introduced. Do not claim authentic NES/Game Boy/AY/etc. emulation unless that specific behavior is implemented and verified.

### 16-BIT

Initial family candidates:

- fixed-point 4-operator FM
- larger wavetable / phase-distortion-style digital oscillator

Compact PCM/sample playback is desirable but is **not required for the first playable milestone** because it adds storage/import/voice-stealing concerns that should not block the tracker core.

## 10. Parameter-lock architecture

P-locks are first-class song data.

V0.1:

- sparse lock storage
- maximum 4 locks on one step
- parameter IDs are stable numeric IDs, not UI page positions
- values use a normalized integer domain or engine-specific integer domain documented per parameter
- changing instrument/engine must have deterministic behavior for locks that no longer apply: ignore safely, never reinterpret an unrelated ID

The UI may expose four encoder lanes at once, but storage is not tied to exactly four physical encoders.

## 11. Tracker effect architecture

The step contains one compact tracker effect slot in addition to parameter locks.

Initial effect candidates:

- ARPEGGIO
- PITCH SLIDE UP/DOWN
- PORTAMENTO
- VIBRATO
- RETRIG
- NOTE DELAY
- NOTE CUT
- VOLUME SLIDE

The UI should show names/values; hexadecimal effect notation may be displayed as a compact secondary representation but must not be required knowledge.

## 12. Persistence

Dinghy settings storage is only suitable for small preferences. Songs require a dedicated flash region or an external/USB transfer strategy.

V0.1 persistence rules:

- never erase/write song flash from the audio ISR;
- save only from an explicit safe state, preferably stopped/silent;
- version every persistent structure;
- include magic, format version, payload length and CRC;
- preserve a migration path or fail safely to an empty song;
- choose FM1Tracker's flash range only after the Dinghy baseline is imported and the current platform map is audited.

Do not hard-code a new flash range before that audit.

## 13. Host-first testing

Before hardware testing, host mode should cover:

- deterministic transport timing;
- STEP REC note/rest advancement;
- pattern wrap and page boundaries;
- parameter-lock insertion/replacement/removal;
- effect decoding;
- voice render smoke tests;
- screen-state snapshots;
- worst-case event queue behavior.

Golden WAV tests are appropriate for stable oscillator/envelope primitives.

## 14. Explicit non-goals for V0.1

- full Furnace `.fur` compatibility
- full Furnace engine or UI port
- dozens of authentic chip emulators
- sample editor
- arbitrary plugin architecture
- unbounded polyphony
- live song save while audio continues
- mixed 8-BIT/16-BIT tracks
- full desktop-style tracker command set

These can only be reconsidered after the core tracker is measured on real hardware.
