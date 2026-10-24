# WP3.6 Tracker Effect Contract

Status: **FROZEN BY D032 BEFORE PRODUCTION SOURCE**  
Date: 2026-10-24  
Package: WP3.6 / issue #25  
Start head: `44f4a6573c8729735a82283294fcf4837cf70b24`

This document is the detailed contract referenced by D032. It defines the first tracker-effect command domain and proof-lane timing/pitch semantics. It does not implement WP3.7 retrig/note-delay/note-cut or WP3.8 general queue work.

## 1. Storage domain

WP3.6 uses the existing `FmtStep.effect` and `FmtStep.effect_value` bytes. Tracker-effect command IDs remain a separate domain from D027 parameter-lock IDs.

```text
0x00  NONE
0x01  ARPEGGIO
0x02  SLIDE_UP
0x03  SLIDE_DOWN
0x04  PORTAMENTO
0x05  VIBRATO
0x06..0xFF reserved / unknown in WP3.6
```

Unknown/reserved commands resolve to safe NONE behavior. `FmtStep` remains exactly 8 bytes.

## 2. Exact six-tick row clock

WP3.6 defines exactly six effect ticks per 1/16-note row.

D020 freezes:

```text
FMT_TRANSPORT_PHASE_PER_STEP = 661500
```

and:

```text
661500 / 6 = 110250
```

exactly. Effect ticks therefore use the existing transport phase, not a second free-running clock.

Tick 0 starts at D021 row dispatch. Internal effect-tick boundaries are:

```text
110250  -> tick 1
220500  -> tick 2
330750  -> tick 3
441000  -> tick 4
551250  -> tick 5
661500  -> next row / new tick 0
```

Each audio frame renders using the current effect state first. D020 transport advances at frame end. If no row boundary occurred and the phase crossed the next effect-tick threshold, the effect advances once and the change becomes audible on the next rendered frame. If the row boundary occurred, D021 new-row dispatch wins and initializes tick 0; there is no synthetic tick 6.

The actual D020 phase residual after a row crossing remains authoritative. Maximum BPM `65535` is lower than one effect-tick span `110250`, so one rendered frame can cross at most one effect-tick threshold. Stopped transport freezes effect-tick progress.

## 3. ARPEGGIO

Command `0x01`, argument `XY`.

High nibble `X` and low nibble `Y` are unsigned whole-semitone offsets `0..15`.

```text
tick 0  +0
tick 1  +X
tick 2  +Y
tick 3  +0
tick 4  +X
tick 5  +Y
```

Arpeggio is transient modulation only. It never changes persistent pitch state or a portamento target.

## 4. SLIDE_UP / SLIDE_DOWN

Commands `0x02` and `0x03`. Full argument byte `v` is an unsigned Q7 semitone delta per nonzero effect tick.

```text
movement per tick = v / 128 semitone
```

Movement occurs on ticks 1..5 only. `v=0` does not move. Pitch saturates to Q7 domain `0..16383`.

Slide modifies persistent pitch, so its result remains on later TIE/EMPTY rows until another command or note changes it. A slide command clears any remembered portamento target. A new ordinary NOTE resets persistent pitch to that note before the slide row proceeds.

No slide-speed memory exists: every slide row uses that row's `effect_value`.

## 5. PORTAMENTO

Command `0x04`. Full argument byte `v` is maximum unsigned Q7 semitone movement toward the remembered target on each tick 1..5.

- NOTE + PORTAMENTO while a proof note is logically active updates the target to `note * 128` but preserves current persistent pitch at row start.
- NOTE + PORTAMENTO with no active note starts directly at the target because no source pitch exists.
- TIE/EMPTY + PORTAMENTO continues toward an already remembered target.
- Movement occurs only on PORTAMENTO rows.
- A target may remain remembered across intervening non-portamento TIE/EMPTY rows, allowing a later PORTAMENTO row to resume.
- Reaching the target clears the pending target.
- NOTE without PORTAMENTO, SLIDE_UP/DOWN and OFF clear the remembered target.
- `v=0` may latch/retain a target but does not move.

No portamento speed/value memory exists. Each PORTAMENTO row supplies its own speed.

## 6. VIBRATO

Command `0x05`, argument `XY`.

High nibble `X` is speed `0..15` phase units per nonzero effect tick on a 64-phase triangle. Low nibble `Y` is depth `0..15`.

The triangle is defined by:

```text
phase 0   =   0
phase 16  = +32
phase 32  =   0
phase 48  = -32
phase 63  approaches 0 from below
```

Peak depth is exactly `Y / 32` semitone. The transient Q7 modulation is mathematically:

```text
triangle * depth / 8
```

with deterministic truncation toward zero. At maximum depth 15 the peak is 60 Q7 units = 15/32 semitone.

Phase advances by `X` modulo 64 on ticks 1..5. Vibrato phase continues across consecutive VIBRATO rows carrying TIE/EMPTY. It resets to phase 0 when entering VIBRATO from another command, on a new NOTE, and on OFF. Speed/depth always come from the current row and are never remembered implicitly.

Vibrato is transient modulation only and never changes persistent pitch.

## 7. Persistent pitch and D031 precedence

WP3.6 introduces persistent proof-lane pitch in Q7 semitone units. Final oscillator pitch is always:

```text
final_q7 = clamp(
    persistent_pitch_q7
    + (D031_FINE - 128)
    + transient_effect_mod_q7,
    0,
    16383)
```

Consequences:

- D031 FINE remains row-local and never mutates persistent slide/portamento pitch.
- ARPEGGIO and VIBRATO are applied after FINE and never mutate persistent pitch.
- D031 VOICE LEVEL and TRACK LEVEL behavior is unchanged.
- With NONE, no prior slide/porta state and FINE=128, pitch must be exactly the historical D031 integer-note pitch.

## 8. NOTE / TIE / EMPTY / OFF

**NOTE without PORTAMENTO**

- logical note becomes active;
- persistent pitch resets to `note * 128`;
- remembered portamento target is cleared;
- the row's ARP/SLIDE/VIBRATO command then applies.

**NOTE with PORTAMENTO**

- uses the target rules above;
- WP3.6 does not reset proof oscillator phase.

**TIE / EMPTY**

- do not reset persistent pitch;
- if a logical note is active, row effects may modulate/move it;
- if no logical note is active, effects cannot resurrect sound.

**OFF**

- clears logical note-active state;
- clears remembered portamento target;
- clears transient modulation and vibrato continuation;
- gate release remains D025/D031 behavior.

A new ordinary NOTE after a slide therefore resets the accumulated slide pitch. ARP/VIB never alter accumulated pitch.

## 9. Bounded main-to-audio effect snapshot

WP3.6 mirrors D031 with one naturally aligned 32-bit effect word per proof-lane row:

```text
bits 0..7    resolved command
bits 8..15   effect_value
bits 16..23  resolver flags / diagnostics
bits 24..31  reserved zero
```

The fixed cache is:

```text
64 rows * 4 bytes = 256 bytes ordinary RAM
```

Main/startup resolution reads only pattern 0 / track 0 `FmtStep.effect/effect_value`. Unknown commands publish NONE plus an invalid-command flag. Audio reads one complete word at row dispatch.

This cache is not the WP3.8 general event queue. WP3.6 exposes no user-facing effect editor; tests may seed effect fields before rebuilding the cache. Any later effect editor must refresh the affected row after mutation.

## 10. Hard real-time bounds

`FmtPlaybackEvent` remains exactly 4 bytes and D020/D021 row ordering remains unchanged.

The proof-lane effect runtime state must be a fixed struct of **no more than 16 bytes**.

Per rendered frame:

- existing D020 transport work;
- at most one effect-threshold comparison;
- only at an actual effect-tick boundary, fixed integer state/pitch arithmetic.

At row dispatch audio performs one fixed 32-bit effect-cache read plus fixed state setup.

Forbidden in the audio path:

- allocator work;
- D028 linked-list traversal;
- variable-count event loops;
- retries;
- screen drawing;
- flash work;
- float;
- runtime 64-bit division.

## 11. Regression/evidence requirements

WP3.6 must preserve byte-for-byte when tracker effects are NONE:

- historical default WAV `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`;
- D031 locked-audio WAV `8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8`.

New deterministic evidence must cover independently and in combination:

- ARPEGGIO six-tick sequence;
- slide up/down accumulation and saturation;
- portamento target, resume, stop and target-reached behavior;
- vibrato phase/speed/depth behavior;
- NOTE/TIE/EMPTY/OFF behavior;
- D031 FINE precedence;
- invalid-command fallback;
- stored song/effect data unchanged by playback;
- all Phase 2 and WP3.1-WP3.5 regressions;
- full JieLi build and measured resource delta.

## Non-goals

WP3.6 does not include:

- retrig;
- note delay;
- note cut;
- a general event queue;
- effect-entry UI or full effect visualization;
- a new synth engine;
- persistence;
- live performance capture;
- final hardware control-map freeze.
