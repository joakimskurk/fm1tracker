# FM1Tracker Live Performance Capture Vision

Status: **accepted product direction; implementation packages deferred until tracker, p-lock and mixed-engine foundations exist**

This document records a core product direction that must not be lost while lower-level tracker and engine work is being built.

The guiding idea is:

> **Tracker depth underneath, immediate instrument workflow on top.**

FM1Tracker should make it unusually easy to move from playing to recording without stopping the musical flow. The tracker remains the source of truth, but the user should not need to think like a desktop tracker operator in order to get an idea down.

This is a core direction, not an optional late feature.

D047 strengthens this vision: the performance surface is not locked to a project-wide 8-BIT or 16-BIT sound mode. Each instrument chooses its engine family + engine, so the eight logical tracks may eventually combine chip, FM/digital, modern synth and drum engines in one performance if measured resources permit it.

Normative engine-routing direction: `D047_MIXED_ENGINE_ARCHITECTURE.md`.

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
3. play the keyboard normally and hear that track's selected instrument/engine immediately;
4. press REC while playback continues;
5. subsequent performance is captured into editable tracker data;
6. press REC again to stop capture while playback continues;
7. keep playing, switch tracks/engines through instrument selection, or edit what was captured.

The transition between audition and record should feel like arming a tape machine or groovebox, not entering a separate application mode.

**REC off:** play/audition/performance only.  
**REC on during playback:** play and capture.

Live capture must never weaken deterministic stopped STEP REC.

## 2. Seamless track switching

Track selection is part of performance, not only editing.

Requirements for the future live-capture layer:

- transport continues while changing selected logical track;
- changing track must not reset pattern position;
- changing track must not cut unrelated tracks;
- the newly selected track becomes the target for subsequent played/captured notes;
- the selected track's current instrument determines the engine family/engine heard and captured;
- the UI must make current capture target and instrument identity unmistakable;
- accidental writes to the previously selected track are unacceptable.

The current eight logical tracks remain the baseline. This vision does **not** reduce track count.

## 3. Musical role target without hard-coded track types

The eight logical tracks stay generic. A practical performance setup should be able to cover at least:

- one drum-oriented instrument/track;
- one bass instrument/track;
- one polyphonic synth instrument/track;
- one lead synth instrument/track;
- four additional flexible logical tracks.

These are reference roles, not fixed channel types or numbers.

Under D047, these roles may use different engine families simultaneously. For example, drums might use a future drum engine, bass FM4, poly a future modern synth engine, lead PULSE or FM4, while other tracks use NOISE/WAVETABLE-8/etc.

The user should be able to reorganize the eight tracks freely.

## 4. Polyphony target

Polyphony remains an engine/voice-layer capability rather than a synonym for track count.

For the performance-oriented design, target **3-6 voices of polyphony for at least one polyphonic instrument/track if measured FM-1 CPU and RAM budgets permit it**.

Important constraints:

- do not silently reduce the eight logical tracker tracks to obtain polyphony;
- do not promise a fixed voice count before mixed-engine hardware profiling;
- voice stealing/allocation must remain deterministic;
- one track may not steal another track's still-owned active primary voice;
- a polyphonic track must coexist with useful drum, bass and lead activity in the reference setup;
- mixed-family resource pressure must never silently substitute an engine or degrade quality unpredictably;
- if the 3-6 voice target cannot coexist with measured realtime budget, record an explicit tradeoff decision rather than degrading transport/tracker determinism.

The Phase 4 eight-voice engine-family budget is therefore a measured starting point, not the final performance-capacity promise.

## 5. Drum performance and recording

Drums participate in the same immediate play/record philosophy.

Future drum workflow should allow:

- a drum-oriented instrument to expose multiple drum sounds across the keyboard;
- live audition while playback runs;
- REC-arm while playback continues;
- played drum hits captured into tracker data;
- deterministic quantization/timestamp policy;
- later tracker editing of captured hits.

D047 allows a dedicated drum engine family later without turning the whole project into a drum/PCM mode.

The exact drum data representation, synthesis/sample engine and keyboard layout are not frozen here.

## 6. Metronome

A transport-derived metronome/click should be available for live capture.

Requirements:

- independently enable/disable it;
- derived from the same deterministic transport clock;
- must not drift from tracker playback;
- must not consume a normal tracker track unless a later explicit decision chooses that behavior;
- must remain independent of whichever engine families the musical tracks use;
- useful count-in behavior may be added later, but count-in semantics are not frozen here.

## 7. Performance FX as an instrument

Effects should become a playable performance surface rather than only a menu of static settings.

The intended direction is closer to OP-Z-style immediacy combined with tracker editability:

- enter an FX performance context;
- effect actions/types may be distributed across physical keys where useful;
- holding/selecting an effect can make encoders/pots control its parameters immediately;
- gestures should be playable rhythmically while pattern playback continues;
- recorded gestures should become inspectable/editable tracker data rather than opaque audio.

Engine-specific controls may differ between PULSE, FM4, modern synth, drums, etc., but the performance interaction should feel coherent because the selected instrument determines the current parameter surface.

The exact key-to-effect layout is deliberately deferred until effect set and hardware ergonomics are real.

## 8. CHANNEL and GLOBAL FX scopes

Performance FX should support two conceptual scopes.

### CHANNEL

The effect or parameter gesture targets the currently selected logical track/instrument.

Examples:

- filter/tone movement where the current engine exposes it;
- engine parameter modulation;
- track-level effect amount;
- retrigger/stutter-like tracker performance actions;
- per-track send/effect parameters where implemented.

The selected instrument determines which engine-specific semantic parameter IDs are applicable. A parameter from another engine family must never be reinterpreted merely because it occupies the same knob position.

### GLOBAL

The gesture targets the master/global processing context rather than one track.

Examples may later include:

- master filter/tone;
- global delay/reverb/chorus parameters if those effects exist;
- master performance effects;
- global send/return controls.

Global automation must not be forced into normal track p-lock storage if that produces ambiguous ownership. A dedicated bounded global automation/event representation may be required and must be designed explicitly before implementation.

## 9. Recording parameter and FX gestures

REC during playback should eventually capture more than notes.

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

D027 semantic parameter identity remains critical under mixed engines: recorded automation must identify the actual semantic parameter, not a physical knob number or page position.

Exact quantization, overwrite/merge/overdub and thinning rules require dedicated decisions after Phase 5 has established mixed-engine voice/parameter costs.

## 10. Tracker remains the source of truth

Live performance capture is not a separate linear recorder bolted onto the product.

The principle is:

**perform -> capture -> tracker data -> edit -> replay**

Captured material should remain understandable and editable:

- notes become note events;
- explicit instrument identity determines the engine used on replay;
- drum hits become deterministic tracker events;
- parameter movement becomes semantic locks/automation events;
- effect gestures become explicit effect/parameter events;
- resulting patterns reproduce the performance deterministically within documented quantization/timing/resource rules.

## 11. Architectural fit

This direction fits the existing architecture well.

Existing foundations already support it:

- eight logical sequencer tracks are separate from voice/polyphony count;
- `FmtStep.instrument` and track default instrument already separate sequencer lanes from sound identity;
- `FmtInstrumentHeader` already carries family/mode + engine bytes;
- D020 gives a deterministic transport clock;
- D022/D023 give explicit cursor and track-selection state;
- D024 establishes precise stopped STEP REC;
- D027-D031 provide stable semantic parameter identity and bounded p-lock storage/publication;
- D032/D033 provide engine-independent tracker timing/actions;
- D034 provides bounded main-to-audio audition ingress;
- D037-D046 prove one complete 8-BIT engine family;
- D047 freezes the future per-instrument mixed-engine router direction.

The main architectural work before live capture is therefore:

1. close Phase 4 under D046;
2. implement WP5.0 mixed-engine routing without regressing the 8-BIT family;
3. add/measure FM4 and mixed-family voice/resource behavior;
4. then add live record-arm and capture semantics on top of that stable engine/instrument model.

The main new live-capture work will be:

- live record-arm state coexisting with running transport;
- deterministic mapping from realtime gestures to tracker positions/timestamps;
- safe track-target changes during capture;
- bounded note/parameter event ingestion;
- overwrite/merge/overdub rules;
- metronome/count-in behavior;
- CHANNEL versus GLOBAL automation ownership;
- measured voice-budget expansion for useful polyphony.

## 12. Dependencies and order

The clean implementation order is now:

1. Phase 2 tracker semantics/editor - **complete**;
2. Phase 3 p-lock/effect/event foundations - **complete**;
3. Phase 4 first 8-BIT engine family - close D046 first;
4. Phase 5 begins with WP5.0 mixed-engine routing, then FM4 and mixed-family validation;
5. implement the dedicated live performance capture milestone;
6. validate complete workflow on physical FM-1 before final V1 control-map freeze.

Live capture is core future work, but not a reason to destabilize the current bounded work-package sequence.

## 13. Reference success scenario

A successful future FM1Tracker session should be able to feel like this:

1. press PLAY and hear the current mixed-engine pattern;
2. jump to an FM bass track and play ideas without recording;
3. press REC without stopping playback and capture the phrase;
4. jump to a drum-engine track and finger-drum another pass;
5. enable metronome if wanted;
6. jump to a poly track using an appropriate measured engine and record chords/voicings;
7. jump to a chip or modern lead and improvise;
8. enter CHANNEL FX and perform an engine-appropriate parameter/effect gesture;
9. switch to GLOBAL FX and manipulate master effects;
10. arm REC and capture selected semantic parameter/effect gestures;
11. stop recording while playback continues;
12. open the tracker and precisely edit everything that was just performed.

That combination - **deep tracker structure, free instrument-engine choice, and near-zero-friction idea capture/performance** - is the intended product identity.
