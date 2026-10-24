# FM1Tracker Roadmap

This roadmap is ordered to reduce risk on FM-1 hardware. A phase is complete only when its exit criteria pass; later features do not bypass a failing earlier layer.

## Product north star — tracker depth, immediate capture

FM1Tracker should combine deep tracker structure with the immediacy of a performance instrument.

The target experience is **play first, capture when inspiration happens**:

- playback can continue while the user plays the selected track;
- pressing REC during playback will later arm live capture without stopping transport;
- changing tracks during playback/capture is fast and must not interrupt unrelated tracks;
- drum performance follows the same play/record philosophy;
- a transport-locked metronome can be enabled;
- FX/parameter pages can be performed in CHANNEL or GLOBAL scope;
- REC can capture selected parameter/effect gestures as bounded tracker-editable data;
- the tracker remains the source of truth so captured performances remain precisely editable.

Eight logical tracker tracks remain the baseline. A practical reference setup should accommodate at least drums, bass, one polyphonic synth and one lead synth while retaining four flexible tracks. These are roles, not fixed channel types.

Target 3–6 voices for at least one polyphonic instrument/track if measured CPU/RAM budgets permit it. Do not silently reduce the eight logical tracks to obtain polyphony.

Detailed direction: `LIVE_PERFORMANCE_CAPTURE_VISION.md`.

---

## Phase 0 — foundation

Status: **COMPLETE**

Frozen:

- hardware-first tracker product boundary;
- 8 tracks / 64 steps / 16 patterns;
- V0.1 data model and engine direction;
- STEP REC and one-action REST as core workflows;
- p-locks as first-class data;
- full Furnace port explicitly out of scope.

---

## Phase 1 — clean Dinghy baseline

Status: **COMPLETE — 2026-10-23**

Verified:

- pinned Dinghy provenance/licensing;
- pristine and renamed host builds;
- fixed identity `FM-1_987`;
- exact external AC79 SDK pin/input hashes;
- renamed device firmware/package build;
- inherited safety checks;
- clean baseline resources: 55,088 B image, 12,368 B normal RAM, 30,720 B pool.

Physical install/audio/render-time testing remains separate because hardware is not currently available.

Evidence: `WP1_3_VERIFICATION.md`, `WP1_4_VERIFICATION.md`, `WP1_5_VERIFICATION.md`.

---

## Phase 2 — tracker core without synth ambition

Goal: prove deterministic editing, visible tracker state and transport using the simplest tone source.

Status: **COMPLETE — 2026-10-23**

Completed:

1. ✅ WP2.1 — compile-time V0.1 data model.
2. ✅ WP2.2 — one pool-backed runtime `FmtSong`.
3. ✅ WP2.3 — exact integer transport/no-drift clock.
4. ✅ WP2.4 — one-track audible frame-exact playback.
5. ✅ WP2.5 — D022 four-page white-key step addressing.
6. ✅ WP2.6 — D023 direct eight-track black-key selector prototype.
7. ✅ WP2.7 — D024 stopped STEP REC ordinary-note entry.
8. ✅ WP2.8 — D025 REST/TIE/OFF editor + playback semantics.
9. ✅ WP2.9 — D026 compact tracker screen.
10. ✅ WP2.10 — complete Phase 2 integration test and closeout.

Final verified Phase 2 state:

- app image: 59,252 B / 581,564 B;
- normal RAM: 12,464 B / 98,304 B;
- pool RAM: 96,616 B / 344,064 B;
- total Phase 2 cost versus Phase 1: +4,164 B image / +96 B normal RAM / +65,896 B pool;
- `FmtSong`: 65,896 B at `.pool` `0x01C20000`;
- complete 64-step D022 address traversal verified exactly once per step;
- eight D023 track selectors verified bijectively with edit isolation;
- stopped STEP REC ordinary notes plus REST/TIE/OFF advance exactly once modulo 64;
- exact automatic editor boundaries `63 -> 0` and `15 -> 16` verified;
- direct integration compares the complete final 65,896-byte song against the expected store;
- playback event kinds `NONE/NOTE/TIE/OFF` remain bounded at 4 B;
- exact 120 BPM loop remains 352,800 frames with no cumulative timing drift;
- tracker screen remains one selected track, eight cursor-owned rows, independent edit/playhead markers and max two dirty regions per poll;
- reviewed integrated framebuffer: `a993551ce922d90186080bf554b82477b4336e160a6b301c446f0d275569b6d7`;
- HOME return after the full workflow is pixel-identical to its equivalent control;
- deterministic default WAV remains `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`;
- package identity remains `FM-1_987`;
- WP2.10 required no production source change and the final firmware/package hashes are byte-identical to WP2.9.

Final Phase 2 verification:

- checkpoint: `7063be51e53c2657178084b3bef5ace813dafcd7`;
- Actions run: `40015242947`;
- durable evidence: `PHASE2_VERIFICATION.md`.

Phase 2 exit criteria are satisfied at the host/device-build level. Physical FM-1 install/audio/timing and control/screen ergonomics remain later hardware gates.

---

## Phase 3 — parameter locks and tracker effects

Goal: make the sequencer genuinely tracker-like and hardware-native.

Status: **COMPLETE — WP3.1–WP3.10 host/device-build exit satisfied**

Parent issue: #20

Progress:

1. ✅ **WP3.1 — stable parameter ID registry**. D027. Final run `40025790828`.
2. ✅ **WP3.2 — sparse four-lock-per-step storage**. D028. Final run `40038453521`.
3. ✅ **WP3.3 — hold-step + encoder p-lock editing**. D029. Final run `40054322180`.
4. ✅ **WP3.4 — p-lock browse / single-lock delete UX**. D030. Final run `40064122343`.
5. ✅ **WP3.5 — basic pitch/volume p-lock playback**. D031. Final run `40069110111`.
6. ✅ **WP3.6 — arpeggio / slide / portamento / vibrato tracker effects**. D032 freezes commands in `FmtStep.effect/effect_value`, exact six-tick D020 timing, Q7 persistent/transient pitch composition and bounded fixed caches/state. Final run `40073680548`; evidence `WP3_6_EFFECT_CONTRACT.md`, `WP3_6_VERIFICATION.md`.
7. ✅ **WP3.7 — retrig / note delay / note cut**. D033 adds commands `0x06..0x08`, exact D032 six-tick scheduling and a 4 B fixed row-event state without a general queue. Final run `40077809743`; evidence `WP3_7_EVENT_CONTRACT.md`, `WP3_7_VERIFICATION.md`.
8. ✅ **WP3.8 — bounded event queue and overflow policy**. D034 freezes the exact 32-entry SPSC main->audio queue, reject-newest overflow and fail-to-silence audition recovery. Final run `40079824265`; evidence `WP3_8_QUEUE_CONTRACT.md`, `WP3_8_VERIFICATION.md`.
9. ✅ **WP3.9 — lock/effect visual feedback**. D035 preserves the ten-region / max-two-redraw surface, adds bounded `LK` and stored `FX` feedback, and keeps every audio/runtime fixture unchanged. Final run `40083510351`; evidence `WP3_9_VISUAL_CONTRACT.md`, `WP3_9_VERIFICATION.md`.
10. ✅ **WP3.10 — Phase 3 worst-case tests and exit gate**. D036 found no production defect after full-pool/cache/queue/UI/timing/combined stress and fail-fast realtime audit. Final run `40084706845`; evidence `WP3_10_TEST_PLAN.md`, `WP3_10_VERIFICATION.md`.

Final verified Phase 3 footprint after WP3.10: 66,960 B image, 13,296 B normal RAM and 113,000 B pool, with zero production delta versus WP3.9. The historical default WAV plus every frozen D031-D035 regression remain deterministic. Final exit run `40084706845`, job `120845144453`.

Phase 3 exit criteria are satisfied at host/device-build level: worst-case lock/effect data is inside budget, editing did not corrupt song/audio state, effect/event/queue behavior is bounded and deterministic, combined stress recovered safely, and the pinned target build remained byte-identical. Physical FM-1 verification remains later hardware work.

Phase 4 is active. **WP4.1 / #31 is COMPLETE** with D037 frozen before source and zero production delta; **WP4.2 / #32 PULSE is active and D038 is FROZEN** before oscillator source.

---

## Phase 4 — 8-BIT engine

Goal: a musically complete chip-style palette.

Status: **IN PROGRESS — WP4.1/D037 COMPLETE; WP4.2 / #32 PULSE active; D038 FROZEN; isolated implementation/tests next**

Parent issue: #30
Active package: WP4.2 / #32 — D038 is frozen in `WP4_2_PULSE_CONTRACT.md`; implement/test the isolated shared pitch helper and PULSE primitive without full-app integration. WP4.1 / #31 is complete. Branch: `phase4/8bit-engine`.

Required starting engines:

1. PULSE
2. NOISE
3. TRIANGLE
4. WAVETABLE-8

Also:

- integer envelope primitive;
- deterministic voice allocation/stealing;
- primary parameter pages + p-lock mapping;
- pitch/retrigger integration;
- measured eight-track hardware benchmark.

Eight voices are the initial budget target, not a promise about final polyphonic-track capacity.

---

## Phase 5 — 16-BIT FM4 engine

Goal: a distinct deeper early-digital mode without destabilizing the tracker.

Planned:

- fixed-point four-operator primitive;
- curated algorithms;
- ratio table;
- modulation levels/index;
- bounded feedback;
- common envelope first;
- deterministic voice allocation;
- parameter pages/p-lock mapping;
- pitch/modulation validation;
- worst-case benchmark.

---

## Core milestone after Phases 4/5 — live performance capture + performance FX

Status: **PLANNED CORE, not optional**

This is where the accepted product north star becomes implemented, after p-lock/event structures and real engine budgets are known.

Must cover:

- PLAY continues while REC arms/disarms capture;
- live notes -> deterministic tracker events;
- fast target-track switching;
- drum-oriented live capture;
- metronome/count-in policy;
- quantization/timestamp and overwrite/merge/overdub rules;
- CHANNEL and GLOBAL performance/automation scope;
- keyboard-distributed FX gestures;
- encoder/pot capture into bounded locks/automation/events;
- explicit global automation ownership where needed;
- reference drums+bass+poly+lead setup across the existing eight logical tracks;
- measured 3–6 voice poly target if hardware permits;
- deterministic voice allocation and stress tests.

No unbounded stream of every ADC sample is acceptable. Captured performance must remain tracker-editable data.

---

## Phase 6 — song/order mode and persistence

Goal: complete songs and safe persistence.

Planned:

- 128-entry order list;
- safe pattern switching;
- repeats/loop range;
- versioned project format with magic/length/CRC;
- flash-region audit;
- explicit save/load flow;
- corruption recovery;
- migration/version harness;
- power-cycle round trip.

---

## Phase 7 — hardware UX freeze

Physical FM-1 required.

Validate specifically:

- REST/TIE/OFF gestures;
- white-key step pages;
- black-key track selection;
- p-lock ergonomics;
- chord-entry behavior;
- tracker-screen readability;
- final live PLAY/REC flow;
- drum/poly workflow;
- metronome;
- CHANNEL/GLOBAL performance FX.

Exit: final V1 control map, no frequent awkward gestures, core workflows learnable without repeated manual lookup.

---

## Phase 8 — interoperability

Goal: desktop connectivity without turning firmware into Furnace.

Candidates:

- internal interchange spec;
- MIDI pattern/project transfer;
- Furnace documented export audit;
- one-way supported-subset converter;
- round-trip feasibility decision.

---

## Phase 9 — optional expansion

Only after measurement:

- WAVETABLE-16;
- PCM/sample playback beyond specifically approved core performance needs;
- swing;
- additional microtiming;
- polymeter/per-track lengths;
- mixed engines;
- additional DSP FX algorithms;
- specifically licensed authentic chip cores.

**Live performance capture is not optional Phase 9 work.**

---

## Release philosophy

Prefer small playable releases. Illustrative sequence:

```text
0.1 host tracker skeleton
0.2 FM-1 step recorder
0.3 visible/p-lock tracker
0.4 8-BIT playable
0.5 16-BIT FM4
0.6 live performance capture
0.7 song/save
1.0 hardware UX freeze
```

Version numbers remain illustrative until release strategy is explicitly frozen.
