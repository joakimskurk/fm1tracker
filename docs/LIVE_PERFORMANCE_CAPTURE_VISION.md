# FM1Tracker Live Performance Capture Vision

Status: **accepted product direction; implementation packages deferred until the tracker, p-lock and engine foundations exist**

This document records a core product direction that must not be lost while the lower-level tracker is being built.

The guiding idea is:

> **Tracker depth underneath, immediate instrument workflow on top.**

FM1Tracker should make it unusually easy to move from playing to recording without stopping the musical flow. The tracker remains the source of truth, but the user should not need to think like a desktop tracker operator in order to get an idea down.

This is a core direction, not an optional late feature.

## 1. Two complementary recording workflows

FM1Tracker should ultimately support both:

### Precise STEP REC

Already established by D024/WP2.7:

- transport stopped;
- explicit cursor position;
- deterministic note/special-event entry;
- exact one-step advance;
- ideal for deliberate tracker programming.

### Live performance capture

Future core workflow:

1. playback is already running;
2. select/jump to a logical track without stopping transport;
3. play the keyboard normally and hear the instrument immediately;
4. press REC while playback continues;
5. subsequent performance is captured into editable tracker data;
6. press REC again to stop capture while playback continues;
7. keep playing, switch tracks, or edit what was captured.

The transition between audition and record should feel like arming a tape machine or groovebox, not entering a separate application mode.

**REC off:** play/audition/performance only.  
**REC on during playback:** play and capture.

Live capture must never weaken the deterministic stopped STEP REC workflow.

## 2. Seamless track switching

Track selection is part of performance, not only editing.

Requirements for the future live-capture layer:

- transport continues while changing the selected logical track;
- changing track must not reset the pattern position;
- changing track must not cut unrelated tracks;
- the newly selected track becomes the target for subsequent played/captured notes;
- the UI must make the current capture target unmistakable;
- accidental writes to the previously selected track are unacceptable.

The current eight logical tracks remain the baseline. This vision does **not** reduce the track count.

## 3. Musical role target without hard-coded track types

The eight logical tracks should stay generic. However, a practical performance setup should be able to cover at least:

- one drum-oriented track;
- one bass track;
- one polyphonic synth track;
- one lead synth track;
- four additional flexible logical tracks.

These are reference roles, not fixed channel types or fixed channel numbers.

The user should be able to reorganize the eight tracks freely.

## 4. Polyphony target

Polyphony remains an engine/voice-layer capability rather than a synonym for track count.

For the performance-oriented design, target **3–6 voices of polyphony for at least one polyphonic instrument/track if measured FM-1 CPU and RAM budgets permit it**.

Important constraints:

- do not silently reduce the eight logical tracker tracks to obtain polyphony;
- do not promise a fixed voice count before Phase 4/5 hardware profiling;
- voice stealing/allocation must remain deterministic;
- a polyphonic track must coexist with useful drum, bass and lead activity in the reference performance setup;
- if 3–6 voices cannot coexist with the measured real-time budget, record an explicit tradeoff decision rather than degrading transport or tracker determinism.

The existing V0.1 eight-voice target is therefore a starting budget, not the final performance-capacity promise.

## 5. Drum performance and recording

Drums must participate in the same immediate play/record philosophy.

Future drum workflow should allow:

- a drum-oriented track/instrument to expose multiple drum sounds across the keyboard;
- live audition while playback runs;
- REC-arm while playback continues;
- played drum hits captured into the tracker;
- deterministic quantization/timestamp policy;
- later tracker editing of the captured hits.

The exact drum data representation, engine and keyboard layout are not frozen here.

## 6. Metronome

A transport-derived metronome/click should be available for live capture.

Requirements:

- independently enable/disable it;
- derived from the same deterministic transport clock;
- must not drift from tracker playback;
- must not consume a normal tracker track unless a later explicit design deliberately chooses that behavior;
- useful count-in behavior may be added later, but count-in semantics are not frozen here.

## 7. Performance FX as an instrument

Effects should become a playable performance surface rather than only a menu of static settings.

The intended direction is closer to **Teenage Engineering OP-Z-style immediacy combined with tracker editability**:

- enter an FX performance context;
- effect actions/types may be distributed across physical keys where useful;
- holding/selecting an effect can make the encoders/pots control its parameters immediately;
- gestures should be playable rhythmically while the pattern continues;
- recorded gestures should become inspectable/editable tracker data rather than an opaque audio recording.

The exact key-to-effect layout is deliberately deferred until the effect set and hardware ergonomics are real.

## 8. Channel and Global FX scopes

Performance FX should support two conceptual scopes:

### CHANNEL

The effect or parameter gesture targets the currently selected logical track/instrument.

Examples:

- filter/tone movement;
- engine parameter modulation;
- track-level effect amount;
- retrigger/stutter-like tracker performance actions;
- per-track send or effect parameters where implemented.

### GLOBAL

The gesture targets the master/global processing context rather than one track.

Examples may later include:

- master filter/tone;
- global delay/reverb/chorus parameters if those effects exist;
- master performance effects;
- global send/return controls.

Global automation must not be forced into a normal track p-lock representation if that produces ambiguous ownership. A dedicated bounded global automation/event representation may be required and must be designed explicitly before implementation.

## 9. Recording parameter and FX gestures

REC during playback should eventually be able to capture more than notes.

Desired interaction:

1. playback is running;
2. enter CHANNEL or GLOBAL FX/performance scope;
3. hold/select an effect or parameter context;
4. turn an encoder/pot or trigger an effect gesture;
5. with REC off, the gesture is temporary performance only;
6. press/arm REC;
7. subsequent gestures are captured into tracker-editable automation/lock data.

The implementation must remain bounded.

Do **not** record an unbounded stream of every ADC/encoder sample. Future implementation should prefer deterministic musical events such as:

- per-step parameter locks;
- thresholded parameter changes;
- bounded timestamped automation events;
- explicit effect on/off/value events;
- controlled interpolation between sparse points if needed.

Exact quantization, overwrite/merge/overdub and thinning rules require dedicated decisions after the Phase 3 p-lock/event model exists.

## 10. Tracker remains the source of truth

Live performance capture is not a separate linear recorder bolted onto the product.

The principle is:

**perform -> capture -> tracker data -> edit -> replay**

Captured material should remain understandable and editable in the tracker:

- notes become note events;
- drum hits become deterministic tracker events;
- parameter movement becomes locks/automation events;
- effect gestures become explicit effect/parameter events;
- the resulting pattern should reproduce the performance deterministically within the documented quantization/timing model.

## 11. Architectural fit

This direction fits the existing architecture well.

Existing foundations already point toward it:

- eight logical sequencer tracks are separate from voice/polyphony count;
- D020 gives a deterministic transport clock;
- D022/D023 give explicit cursor and track-selection state;
- D024 establishes precise stopped STEP REC as the first recording layer;
- Phase 3 already requires stable parameter IDs, sparse p-lock data and a bounded event queue;
- the architecture already requires one-writer shared state and bounded main-loop -> sequencer -> audio handoff;
- engine polyphony is already treated as an engine capability rather than a track-count property.

The main new architectural work later will be:

- a live capture/record-arm state that can coexist with running transport;
- deterministic mapping from real-time gestures to tracker positions/timestamps;
- safe track-target changes during capture;
- bounded note/parameter event ingestion;
- overwrite/merge/overdub rules;
- metronome/count-in behavior;
- CHANNEL versus GLOBAL automation ownership;
- measured voice-budget expansion for a useful polyphonic track.

## 12. Dependencies and order

Do not pull this work into WP2.8.

The clean implementation order remains:

1. finish Phase 2 tracker semantics and visible editor;
2. finish Phase 3 p-lock/effect/event foundations;
3. establish real 8-BIT and 16-BIT engine/voice costs in Phases 4 and 5;
4. then implement a dedicated live performance capture milestone;
5. validate the complete workflow on physical FM-1 before final V1 control-map freeze.

Live capture is therefore **core future work**, but not a reason to destabilize the current bounded work-package sequence.

## 13. Reference success scenario

A successful future FM1Tracker session should be able to feel like this:

1. press PLAY and hear the current pattern;
2. jump to the bass track and play ideas over it without recording;
3. press REC without stopping playback and capture the bass phrase;
4. jump to the drum track and finger-drum another pass;
5. enable the metronome if wanted;
6. jump to a poly track and record chords/voicings within the measured polyphony budget;
7. jump to lead and improvise;
8. enter CHANNEL FX and perform a filter/effect gesture;
9. switch to GLOBAL FX and manipulate master effects;
10. arm REC and capture selected parameter/effect gestures;
11. stop recording while playback continues;
12. open the tracker and precisely edit everything that was just performed.

That combination — **deep tracker structure with near-zero-friction idea capture and performance** — is the intended product identity.