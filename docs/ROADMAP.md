# FM1Tracker Roadmap

This roadmap is ordered to reduce risk on the FM-1 hardware. A phase is complete only when its exit criteria pass; later features should not bypass a failing earlier layer.

## Product north star — tracker depth, immediate capture

FM1Tracker should ultimately combine the depth and editability of a tracker with the immediacy of a performance instrument.

The target experience is **play first, capture when inspiration happens**:

- playback can continue while the user plays the currently selected track;
- pressing REC during playback arms live capture without stopping transport;
- changing tracks during playback is fast and does not interrupt unrelated tracks;
- drum performance participates in the same play/record workflow;
- a transport-locked metronome can be enabled when wanted;
- FX/parameter pages can be performed live in CHANNEL or GLOBAL scope;
- REC can capture selected parameter/effect gestures as bounded, tracker-editable automation/locks/events;
- the tracker remains the source of truth, so captured performances can always be edited precisely afterward.

The eight logical tracker tracks remain the baseline. A practical reference setup should be able to contain at least drums, bass, one polyphonic synth and one lead synth while leaving the remaining tracks flexible. These are roles, not hard-coded channel types.

Target 3–6 voices for at least one polyphonic instrument/track if measured CPU/RAM budgets allow it. Do not silently reduce the eight logical tracks to obtain polyphony; any compromise must be an explicit measured decision.

Detailed product and architecture direction: [`LIVE_PERFORMANCE_CAPTURE_VISION.md`](LIVE_PERFORMANCE_CAPTURE_VISION.md).

This direction does **not** change the current bounded Phase 2 sequence. Stopped STEP REC is the precise foundation; live performance capture is a later core layer built on transport, p-lock/event and engine foundations.

## Phase 0 — foundation

Goal: freeze the product boundary before firmware code grows around assumptions.

Status: **COMPLETE**

Deliverables:

- architecture contract
- provisional control map
- V0.1 data model
- V0.1 8-BIT / 16-BIT engine scope
- licensing/provenance policy
- implementation roadmap

Exit criteria:

- song model fits stated pool budget on paper;
- no V0.1 feature requires floating point or unbounded real-time work;
- STEP REC, REST and p-lock gestures have explicit behavior;
- full Furnace port is explicitly out of scope.

## Phase 1 — clean Dinghy baseline

Goal: make `fm1tracker` build and run as its own FM-1 firmware before tracker features are added.

Status: **COMPLETE — 2026-10-23**

Verified:

- Dinghy provenance is pinned and licensed material is documented;
- the minimal platform import matches pinned upstream blobs and Git modes;
- pristine and renamed host builds produce verified audio/screen artifacts;
- JieLi toolchain acquisition/execution works;
- FM1Tracker identity is fixed as `FM-1_987` after a current Firmware Hub collision check;
- renamed host artifacts and visible screen identity are verified;
- renamed host audio is bit-identical to the pristine baseline;
- protected Dinghy app/platform/HAL/loader/build-driver implementation remained unchanged through Phase 1;
- official JieLi AC79 SDK tag `AC79NN_SDK_V1.2.1_2023-12-13` is externally pinned to commit `d179b4484759423312073f5fbb232501aa491047` and its consumed package inputs are hash-verified;
- complete renamed device firmware/package build passes with inherited safety checks;
- clean reference usage is 55,088 B image, 12,368 B normal RAM and 30,720 B pool RAM;
- nominal callback period is 5.804988662 ms at 256 frames / 44.1 kHz;
- actual physical-hardware render time and install behavior remain untested because no FM-1 is currently available.

See `WP1_3_VERIFICATION.md`, `WP1_4_VERIFICATION.md`, `WP1_5_VERIFICATION.md` and `TEST_MATRIX.md` for exact verification levels.

Completed tasks:

1. ✅ import a pinned Dinghy baseline with history/provenance recorded;
2. ✅ preserve upstream GPL/credits/headers;
3. ✅ add full GPL-3.0-only license text;
4. ✅ rename application identity/name/build artifacts to FM1Tracker;
5. ✅ choose a currently unused FM-1 package identity (`FM-1_987`);
6. ✅ verify unchanged pristine/renamed host behavior;
7. ✅ verify renamed firmware build safety checks;
8. ✅ create/maintain `THIRD_PARTY.md` with exact upstream and SDK provenance;
9. ✅ add automated pristine verification without redistributing SDK material;
10. ✅ add automated identity-only host regression verification;
11. ✅ complete renamed device-build/resource baseline and reproducible setup closeout.

Phase 1 exit criteria are satisfied at the host/firmware-build level. Physical FM-1 testing remains an explicit later hardware verification layer rather than a claimed Phase 1 result.

## Phase 2 — tracker core without synth ambition

Goal: prove editing and transport with the simplest possible tone source.

Status: **IN PROGRESS — WP2.1 through WP2.7 complete; WP2.8 is next**

Tasks:

1. ✅ WP2.1 — compile-time V0.1 data model, constants, bounds-safe accessors, static size assertions and host/device compile checks. Evidence: `WP2_1_VERIFICATION.md`.
2. ✅ WP2.2 — one actual 65,896 B `FmtSong` in `.pool`, explicit initialization of all 8,192 steps, runtime bounds/reset tests and measured resource delta. Evidence: `WP2_2_VERIFICATION.md`.
3. ✅ WP2.3 — exact integer/fixed-point transport clock in the 44.1 kHz frame domain, deterministic 0..63 wrap, reset/start/stop/resume semantics and long-run no-drift verification. Evidence: `WP2_3_VERIFICATION.md`.
4. ✅ WP2.4 — deterministic one-track audible playback with exact sub-block row timing, frame-0 step dispatch and exact 64-step loop. Evidence: `WP2_4_VERIFICATION.md`.
5. ✅ WP2.5 — D022 SEQ-context white-key 16-step page input across four clamped pages, with no song mutation and frozen audible playback preserved. Evidence: `WP2_5_VERIFICATION.md`.
6. ✅ WP2.6 — D023 provisional eight-black-key track-selection prototype in SEQ context, with reserved keys, no song mutation and frozen playback preserved. Evidence: `WP2_6_VERIFICATION.md`.
7. ✅ WP2.7 — D024 stopped-transport STEP REC note entry with deterministic lowest-note monophonic gesture policy, exact selected-row note writes and modulo-64 cursor advance. Evidence: `WP2_7_VERIFICATION.md`.
8. ⬜ WP2.8 — REST / TIE / NOTE-OFF semantics. Issue #16 is READY TO START; no implementation yet.
9. ⬜ WP2.9 — compact tracker screen with bounded/dirty rendering.
10. ⬜ WP2.10 — Phase 2 integration test: enter and play a complete 64-step melody without a computer.

Current verified Phase 2 runtime baseline after WP2.7:

- app image: 56,548 B / 581,564 B;
- normal RAM: 12,404 B / 98,304 B;
- pool RAM: 96,616 B / 344,064 B;
- `FmtSong` store: 65,896 B at `.pool` address `0x01C20000`;
- `FmtStepRecord`: 2 B at `.bss` address `0x01C0B060`;
- `FmtPatternInput`: 4 B at `.bss` address `0x01C0B062`;
- `FmtTransport`: 8 B at `.bss` address `0x01C0B068`;
- `FmtPlayback`: 1 B at `.bss` address `0x01C0B070`;
- four pages × sixteen local positions cover all 64 pattern steps exactly;
- eight designated black keys select logical tracks 0..7; three remaining black keys are reserved;
- STEP REC writes one ordinary note per fully released gesture while transport is stopped;
- simultaneous key downs use deterministic lowest-note selection;
- successful note writes advance exactly one global step modulo 64;
- transport phase threshold: 661,500;
- deterministic 120 BPM full-pattern loop: 352,800 frames;
- deterministic audible host WAV SHA-256 remains `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`;
- physical FM-1 STEP REC/page/track ergonomics, audio behavior and actual callback execution time remain unverified.

Exit criteria:

- a 64-step melody can be entered from the FM-1 keyboard without a computer;
- REST advances exactly one step;
- playback loops without cumulative timing drift;
- edits during stop mode are deterministic and bounds-safe.

The stopped deterministic note-write portion of the final editing criterion is now verified in WP2.7. Special-event editing remains WP2.8, the visible tracker remains WP2.9, and the full computer-free melody workflow remains WP2.10.

## Phase 3 — parameter locks and tracker effects

Goal: make the sequencer genuinely tracker-like and hardware-native.

Tasks:

- choose/profile the final sparse lock representation;
- max four p-locks/step;
- hold/select step + encoder lock gesture;
- lock replace/delete UI;
- stable parameter ID table;
- first tracker effects: arpeggio, slide, portamento, vibrato, retrig, note delay, note cut, volume slide;
- timestamped/bounded event handoff if NOTE DELAY requires sub-step offsets;
- visible lock/effect markers on screen/LEDs.

Exit criteria:

- worst-case lock data remains inside budget;
- no lock edit can corrupt current audio state;
- effect behavior is documented and host-tested;
- queue overflow behavior is deterministic.

## Phase 4 — 8-BIT engine

Goal: make 8-BIT mode musically complete enough for real patterns.

Order:

1. PULSE
2. NOISE
3. TRIANGLE
4. WAVETABLE-8

Tasks:

- fixed-point oscillators;
- envelope primitive;
- 8-voice target with measured stealing policy;
- primary four-parameter pages;
- p-lock parameter mappings;
- pitch and retrigger integration;
- hardware timing benchmarks.

Exit criteria:

- representative eight-track pattern remains below audio timing budget;
- no forbidden float/runtime 64-bit divide paths;
- p-locks can alter engine parameters without instability;
- pitch is correct across intended keyboard range.

## Phase 5 — 16-BIT FM4 engine

Goal: establish a distinct, deeper second sound mode without destabilizing the tracker core.

Tasks:

- four fixed-point operators;
- curated algorithm table;
- ratio table;
- modulation index/levels;
- bounded feedback;
- shared/common envelope first;
- deterministic voice allocation;
- p-lock mappings;
- hardware worst-case benchmark.

Exit criteria:

- clearly distinct 16-BIT sonic palette;
- stable pitch/modulation behavior;
- normal worst-case render target remains below half audio callback budget or voice/algorithm limits are explicitly reduced.

## Core milestone after Phases 4/5 — live performance capture and performance FX

Goal: make FM1Tracker fast enough for idea capture and performance that the tracker depth becomes almost invisible until the user wants to edit.

Status: **PLANNED CORE MILESTONE — package IDs intentionally deferred until the Phase 3–5 event/engine budgets are measured**

This milestone is not optional. It builds on the precise stopped STEP REC workflow rather than replacing it.

Required direction:

- playback continues while the keyboard auditions the selected logical track;
- REC can arm/disarm note capture while playback continues;
- track selection can change the live capture target without stopping transport or cutting unrelated tracks;
- drum-oriented performance and capture use the same workflow;
- a transport-derived metronome can be enabled independently;
- captured notes/hits become deterministic, editable tracker events;
- CHANNEL performance scope targets the selected track/instrument;
- GLOBAL performance scope targets master/global FX/control state;
- effects may use a keyboard-distributed performance layout where musically useful, inspired by OP-Z immediacy but represented as explicit tracker data;
- holding/selecting an effect/parameter context plus turning encoders should work as a performance gesture;
- with REC armed, selected encoder/effect gestures are captured as bounded p-lock/automation/effect events rather than an unbounded raw control stream;
- overwrite/merge/overdub, quantization and automation-thinning rules must be explicitly decided before implementation;
- global automation gets explicit ownership and is not silently forced into one normal track's p-locks;
- preserve all eight logical tracks;
- benchmark a reference configuration containing at least drums, bass, poly synth and lead synth plus four flexible tracks;
- target 3–6 voices for at least one polyphonic instrument/track if measured hardware budget allows it, with deterministic voice allocation/stealing.

Dependencies:

- Phase 2 deterministic transport/editor semantics;
- Phase 3 stable parameter IDs, bounded p-lock storage and bounded event handoff;
- measured Phase 4/5 engine and voice costs.

Exit criteria:

- PLAY -> REC -> capture -> REC-off works without stopping transport;
- switching the selected capture track during playback is deterministic and glitch-safe;
- live note and drum capture replay as editable tracker data;
- metronome remains phase-locked to the same transport;
- recorded parameter/effect gestures reproduce deterministically within documented quantization rules;
- CHANNEL and GLOBAL automation ownership is unambiguous;
- eight logical tracks are preserved;
- the drums/bass/poly/lead reference setup is demonstrated within measured CPU/RAM limits, or any necessary polyphony compromise is recorded explicitly rather than hidden.

Detailed direction: [`LIVE_PERFORMANCE_CAPTURE_VISION.md`](LIVE_PERFORMANCE_CAPTURE_VISION.md).

## Phase 6 — song/order mode and persistence

Goal: move from one pattern to complete songs.

Tasks:

- 128-entry order list;
- repeats/loop range;
- pattern switching at safe boundaries;
- project serializer with magic/version/length/CRC;
- audit and allocate FM1Tracker flash region;
- explicit save/load UI;
- corrupted-project recovery;
- migration tests.

Exit criteria:

- power-cycle/save/load round trip preserves a test song;
- malformed project data fails safely;
- save path never blocks the audio ISR unexpectedly.

## Phase 7 — hardware UX freeze

Goal: validate the provisional mappings on a physical FM-1 and remove awkward gestures.

Test specifically:

- FX as REST;
- white keys as 16 steps;
- black keys as track selectors;
- hold-step + knob p-lock ergonomics;
- current lowest-note/full-release chord-entry behavior;
- OCT buttons as page navigation outside STEP REC and pitch octave inside STEP REC;
- readability of the tracker view on 240 x 240;
- PLAY -> live REC without stopping musical flow;
- fast track switching while playback/capture continues;
- drum, bass, poly and lead reference workflow;
- metronome discoverability and usefulness;
- CHANNEL versus GLOBAL FX performance mode;
- keyboard-distributed FX gestures and encoder capture ergonomics.

Exit criteria:

- final V1 control map documented;
- no frequently used action needs an uncomfortable multi-button gesture;
- STEP REC can be used without consulting documentation after initial learning;
- live audition/capture can be understood and used without fighting the tracker UI.

## Phase 8 — interoperability

Goal: connect FM1Tracker to desktop workflows without turning the firmware into Furnace.

Candidates:

- MIDI dump/import of patterns/projects;
- host-side project converter;
- Furnace documented export/subset investigation;
- optional one-way Furnace -> FM1Tracker conversion for supported notes/instruments/effects;
- later round-trip only if semantics can be preserved honestly.

Every imported format is a compatibility layer outside the real-time synth core.

## Phase 9 — optional expansion

Only after measurement:

- WAVETABLE-16
- PCM/sample playback beyond any specifically approved drum/performance need
- swing
- microtiming beyond the bounded live-capture/event model
- per-track lengths/polymeter
- mixed 8-BIT/16-BIT tracks
- additional delay/chorus/reverb or other DSP algorithms beyond the core performance-control architecture
- specifically named chip emulation cores that pass license/resource audits

**Live performance capture itself is not optional Phase 9 work.** It is the core milestone defined above. Phase 9 may expand what can be captured or processed, not whether seamless live capture exists.

## Release philosophy

Prefer small playable releases:

```text
0.1 host tracker skeleton
0.2 FM-1 step recorder
0.3 p-lock tracker
0.4 8-BIT playable
0.5 16-BIT FM4
0.6 live performance capture
0.7 song/save
1.0 hardware UX freeze
```

Version numbers are illustrative until the release strategy is explicitly frozen.