# FM1Tracker V0.1 Engine Scope

Status: **design freeze candidate**

The first playable firmware should prove the tracker workflow, not win a chip-emulation feature-count contest. Both sound modes must therefore start with a deliberately small set of engines that are cheap enough to run comfortably inside the FM-1 audio budget.

## 1. Shared engine contract

Every engine must expose:

- note on / note off / optional tie
- integer velocity/level
- integer pitch representation
- deterministic reset behavior
- bounded render cost
- a stable parameter-ID table for p-locks
- a four-parameter primary performance page
- optional engine-specific secondary pages

All DSP is fixed-point/integer on device.

The host build may use helper code for tests, but behavior relevant to the firmware must be reproducible without floating point.

## 2. 8-BIT mode

8-BIT means a curated chip-style synthesis palette. It does not imply authentic emulation of a particular console unless a later engine is explicitly named and verified as such.

### V0.1 required engines

#### PULSE

Core:

- band-unlimited pulse/square initially acceptable if aliasing remains musically tolerable and CPU savings matter
- selectable duty values, with continuous PWM only if cheap enough
- simple amplitude envelope
- optional pitch envelope after core is stable

Primary encoder page:

```text
DUTY | DECAY | FINE | LEVEL
```

Candidate p-lock parameters:

```text
DUTY
LEVEL
PITCH_FINE
ENV_DECAY
```

#### TRIANGLE

Core:

- integer phase accumulator
- triangle transfer function/table
- amplitude envelope

Primary page:

```text
SHAPE | DECAY | FINE | LEVEL
```

`SHAPE` may initially be fixed/reserved if a useful cheap variant is not found.

#### WAVETABLE-8

Core:

- small mono single-cycle tables stored as constants
- integer table lookup
- optional interpolation only after profiling
- small built-in wave bank, no user import in V0.1

Primary page:

```text
WAVE | DECAY | FINE | LEVEL
```

#### NOISE

Core:

- deterministic LFSR or equivalent integer pseudo-noise source
- at least two tonal/color modes if cheap
- amplitude envelope

Primary page:

```text
TYPE | DECAY | TONE | LEVEL
```

### 8-BIT polyphony target

Start with **8 voices maximum**, dynamically assigned across the eight logical tracks. The exact per-engine cost must be measured before promising eight simultaneous expensive voices.

Voice stealing must be deterministic: prefer oldest/released voice according to a documented policy.

## 3. 16-BIT mode

16-BIT means a broader early-digital sound palette. It is a musical category, not a guarantee about every arithmetic width.

### V0.1 required engine

#### FM4

A compact fixed-point four-operator FM engine.

Required:

- four operators
- small curated algorithm set rather than an unbounded matrix
- per-operator ratio from precomputed integer tables
- per-operator level
- feedback on a bounded subset/operator if CPU allows
- shared/common amplitude envelope first; per-operator envelopes only if profiling proves feasible

Primary page:

```text
ALGO | MOD | FEED | LEVEL
```

Secondary EDIT page may expose operator ratios/levels.

The first milestone should favor a musically useful simplified FM4 over a superficially complete DX-style implementation that misses the ISR budget.

### V0.1 optional engine

#### WAVETABLE-16

A larger/broader wavetable oscillator than WAVETABLE-8, potentially with morph/phase-distortion-style shaping if the CPU budget permits.

It is optional for the first audible 16-BIT milestone. FM4 alone is sufficient to validate the mode split.

### Deferred PCM

PCM/sample playback is intentionally deferred until after the tracker, p-locks and both synthesis modes are stable.

Reasons:

- sample storage competes with pool/flash resources;
- import/format conversion adds non-audio scope;
- sample start/end/loop parameters add UI complexity;
- voice streaming/storage policy should be designed from measured remaining resources.

## 4. Envelope policy

V0.1 should begin with the cheapest envelope that still makes instruments musical.

Preferred implementation:

- integer stage machine
- control-rate update where audibly safe
- precomputed/time-mapped coefficient or increment tables
- no division in the audio callback

The UI may present familiar ADSR terminology even if an early engine internally uses a simpler decay/release model, but the displayed controls must match the actual behavior.

## 5. Pitch policy

Use 32-bit phase accumulators and precomputed note-increment tables.

Required:

- MIDI-style note 0..127 input domain
- octave switching without runtime expensive division
- fine pitch parameter with deterministic integer mapping
- effect pitch slides use accumulators rather than repeated lossy note re-quantization

## 6. Mixer policy

V0.1 mixer:

- per-track level
- per-track pan if CPU/memory cost is negligible
- master headroom strategy documented and measured
- explicit saturation/clipping behavior

Delay/reverb are not required for the first playable build. A tracker with excellent entry/editing and dry sound is more valuable than an overloaded DSP build.

## 7. Engine quality gates

An engine is not accepted into V0.1 merely because it makes sound.

It must pass:

1. host deterministic render test;
2. note/pitch sanity across the intended keyboard range;
3. note-off/retrigger test;
4. p-lock parameter update test;
5. worst-case simultaneous voice render benchmark;
6. hardware ISR timing measurement;
7. no forbidden float/64-bit divide symbols in the firmware build;
8. no zipper/click artifacts beyond those intentionally characteristic of the engine.

## 8. Resource stop rules

If an engine pushes normal worst-case audio rendering above the target half-budget, simplify in this order:

1. reduce control-rate work;
2. replace runtime math with tables/shifts;
3. reduce expensive interpolation/modulation;
4. reduce simultaneous voice count for that engine;
5. reduce algorithm complexity;
6. defer the engine.

Do not sacrifice transport timing or p-lock determinism to preserve an overly ambitious synth feature.

## 9. Authentic chip emulation policy

Future authentic profiles/cores are welcome only after an explicit audit covering:

- upstream license compatibility with GPL-3.0-only firmware;
- source provenance and notices;
- code/constant size;
- RAM/pool cost;
- worst-case CPU cost;
- endianness/alignment assumptions;
- use of float, dynamic allocation, 64-bit division or unavailable libc functions;
- whether the implementation is sufficiently accurate to use the chip/system name.

Until then, product/UI naming stays generic: PULSE, TRIANGLE, WAVETABLE, NOISE, FM4.
