# FM1Tracker V0.1 Engine Scope

Status: **current engine-family direction amended by D037-D047**

FM1Tracker should prove a small number of musically useful, deterministic engines before expanding feature count. The important architecture rule after D047 is that engines are **per instrument**, not project-wide modes.

Current normative mixed-engine direction: `D047_MIXED_ENGINE_ARCHITECTURE.md`.

## 1. Shared engine-family contract

Every engine family integrated into tracker playback must provide a bounded adapter for the common tracker semantics it supports.

At minimum, an engine must define:

- NOTE / OFF behavior and TIE policy;
- integer/fixed-point pitch input;
- deterministic reset/retrigger behavior;
- bounded render cost;
- deterministic voice ownership/allocation policy;
- stable semantic parameter IDs for lockable controls;
- a compact primary performance page suitable for the FM-1 controls;
- explicit behavior for unsupported/malformed instrument/parameter data;
- measured image/RAM/pool cost;
- deterministic host/device-build evidence.

All real-time firmware DSP is fixed-point/integer. No float, runtime 64-bit division, heap allocation, dynamic plugin discovery or unbounded per-sample work is allowed.

The tracker/sequencer owns musical time. An engine may not introduce a parallel tracker clock or private unbounded event stream.

## 2. Instrument routing after D047

The target relationship is:

```text
track / row -> instrument -> engine family -> engine
```

The project itself is not `8-BIT` or `16-BIT`.

Different tracks may eventually run different engine families simultaneously, and an explicit instrument change on a NOTE may cross families once WP5.0 freezes the mixed router.

The current `FmtInstrumentHeader.mode` byte is kept as the compatibility family discriminator until WP5.0 decides whether to retain/alias/rename the source field. `FmtInstrumentHeader.engine` remains the engine-within-family identity.

`8-BIT`, `FM/DIGITAL`, `MODERN`, `DRUM` and similar labels may be useful browser categories, but they are not project types.

## 3. First proven family - 8-BIT/chip

Phase 4 implements and verifies the first complete engine family.

### PULSE

Frozen implementation highlights:

- 32-bit phase-only oscillator;
- hard-edge pulse output;
- bounded WIDTH domain;
- common Q7 pitch helper;
- deterministic reset/retrigger;
- no anti-aliasing path hidden in the realtime loop.

Current D044 primary page:

```text
WIDTH | DECAY | SUSTAIN | RELEASE
```

### NOISE

Frozen implementation highlights:

- deterministic 15-bit maximal LFSR;
- one bounded state advance maximum per rendered frame;
- saturated pitch-derived noise clock;
- deterministic reset seed.

Current D044 primary page:

```text
ATTACK | DECAY | SUSTAIN | RELEASE
```

### TRIANGLE

Frozen implementation highlights:

- 32-bit phase-only integer triangle;
- exact bounded Q15-style output;
- direct common pitch reuse;
- no table/interpolation cost.

Current D044 primary page:

```text
ATTACK | DECAY | SUSTAIN | RELEASE
```

### WAVETABLE-8

Frozen implementation highlights:

- eight immutable 32-sample signed-8-bit tables;
- top-five-bit zero-order-hold lookup;
- deterministic table fallback;
- 256 B immutable table bank.

Current D044 primary page:

```text
TABLE | DECAY | SUSTAIN | RELEASE
```

### Common Phase 4 family components

- D042 common integer linear ADSR;
- D043 deterministic eight-slot owner/release-tail allocator;
- D044 six stable 8-BIT semantic parameter IDs;
- D045 linked all-eight-track adapter;
- D046 representative/full-pool/long-run/realtime closeout.

The Phase 4 all-8-BIT implementation remains the regression baseline after mixed routing is introduced. D047 does not require changing it before WP4.10 closes.

## 4. Next family - FM4 / early digital

FM4 is the next engine family to build after WP5.0 establishes the common mixed-engine router.

Required direction:

- four fixed-point operators;
- small curated algorithm set rather than an unrestricted matrix;
- precomputed/integer operator ratios;
- explicit modulation/index/level mapping;
- bounded feedback;
- deterministic pitch/retrigger/NOTE/OFF semantics;
- measured envelope strategy;
- deterministic voice allocation/integration;
- stable parameter IDs;
- primary page initially targeted around:

```text
ALGO | MOD | FEED | LEVEL
```

A secondary focused editor may expose operator ratios/levels if hardware/resources justify it.

The first milestone should prefer a musically useful compact FM4 over superficial DX-style completeness that compromises realtime bounds.

### Phase 5 ordering

FM4 production integration may not bypass the router work:

```text
WP5.0 mixed-engine routing contract + migration
WP5.1 FM operator primitive
WP5.2 curated algorithms
WP5.3 ratio table
WP5.4 levels/modulation index
WP5.5 bounded feedback
WP5.6 FM envelope strategy
WP5.7 FM voice allocation/integration
WP5.8 FM parameter pages + p-lock mapping
WP5.9 mixed 8-BIT + FM4 validation
WP5.10 mixed worst-case Phase 5 exit
```

## 5. Future modern/drum engine direction

D047 deliberately prevents the tracker from being redesigned again when later engines arrive.

Possible future bounded engine packages include:

- virtual analog;
- supersaw/unison;
- broader wavetable/digital synthesis;
- dedicated drum synthesis;
- phase-distortion-style digital synthesis;
- PCM/sample playback if storage/transfer/runtime budgets support it;
- specifically licensed authentic chip cores.

These are **architecturally allowed**, not automatically V1-required.

Each new engine still needs its own explicit parameter/resource/realtime contract and deterministic verification.

## 6. Polyphony and voice-budget policy

Eight logical tracker tracks are stable sequencer lanes. They are not a voice-count promise.

Rules:

- engine families may have different voice costs;
- at most one owned primary sequenced voice per logical track remains the mixed-router baseline until a later polyphonic-track package explicitly extends it;
- release tails may exist only under an explicit bounded policy;
- one track may not steal another track's still-owned active primary voice;
- the accepted future target is 3-6 voices for at least one polyphonic instrument if measured FM-1 CPU/RAM permits;
- do not silently reduce the eight logical tracks to obtain polyphony;
- do not silently substitute an engine, reduce synthesis quality or drop random notes when a configuration exceeds budget;
- resource-limit behavior must be explicit and deterministic.

WP5.0 must choose the cross-family ownership representation from measured options rather than forcing every engine into the current `Fmt8BitVoice` layout.

## 7. Envelope policy

D042 proves one cheap common linear ADSR implementation for the 8-BIT family.

A later family may reuse it or define another bounded envelope representation if required by its synthesis model. Envelope state/rates still must obey:

- integer/fixed-point arithmetic;
- bounded stage work;
- no division in the sample loop;
- deterministic NOTE/TIE/RETRIG/OFF semantics;
- displayed controls matching actual behavior.

FM4's exact envelope strategy is intentionally WP5.6 work rather than silently assumed from the 8-BIT family.

## 8. Pitch policy

Common tracker pitch semantics remain engine-independent.

Current foundation:

- MIDI-style note `0..127`;
- D031/D032 Q7 semitone domain for fine/effect composition;
- exact integer transport/effect timing;
- expensive conversion work occurs on note/pitch changes, not unnecessarily per sample.

Each engine family must document how the common resolved pitch becomes its internal oscillator/operator phase increments without changing tracker semantics.

## 9. Mixer policy

The mixer remains bounded and explicit.

Current Phase 4 linked engine:

- per-track common gain from D031 VOICE LEVEL x TRACK LEVEL;
- signed bounded mix/clamp;
- TRACK PAN remains storage-only.

Future work may add pan/sends/global processing only through explicit packages with measured CPU/RAM and ownership rules.

Delay/reverb/modern FX are not prerequisites for validating a new sound engine.

## 10. Parameter/p-lock policy

D027 semantic parameter identity remains the common storage contract.

Rules:

- existing IDs are never renumbered for mixed-engine routing;
- new engine parameters receive explicit IDs in their owning package;
- primary page position does not define parameter identity;
- a lock irrelevant to the resolved engine is ignored safely;
- an engine change never reinterprets one engine's lock ID as another parameter;
- parameter caches/publication remain bounded and audio never traverses D028 lists.

## 11. Engine quality gates

An engine is not accepted merely because it makes sound.

It must pass the applicable gates:

1. deterministic host render/state tests;
2. pitch/note sanity over intended range;
3. NOTE/OFF/TIE/retrigger behavior;
4. p-lock parameter application and safe irrelevant-lock handling;
5. deterministic voice ownership/allocation;
6. simultaneous-engine/family integration tests once linked;
7. worst-case render/resource benchmark;
8. pinned target compile/build;
9. no forbidden float/runtime-64-bit-divide/allocation/unbounded realtime work;
10. physical hardware callback margin before final hardware signoff;
11. no unacceptable zipper/click artifacts beyond deliberate engine character.

Host wall-clock timing is diagnostic only and is not AC791N execution-margin proof.

## 12. Resource stop rules

If an engine or mixed configuration exceeds measured budget, simplify in this order where musically appropriate:

1. reduce control-rate work;
2. replace runtime math with tables/shifts;
3. reduce expensive interpolation/modulation;
4. reduce that engine's explicit polyphony/voice target;
5. reduce algorithm complexity;
6. defer the engine/configuration.

Do not sacrifice transport timing, p-lock determinism, active-track ownership or silent engine substitution to preserve an overly ambitious synth feature.

## 13. Authentic chip/core policy

Future authentic profiles/cores require explicit audit of:

- GPL-3.0-only compatibility;
- provenance/notices;
- image/constants size;
- RAM/pool cost;
- worst-case CPU cost;
- endian/alignment assumptions;
- float/dynamic allocation/runtime 64-bit division/libc requirements;
- accuracy sufficient to use the claimed system/chip name.

Until then, generic product names such as PULSE, TRIANGLE, WAVETABLE, NOISE and FM4 remain appropriate.
