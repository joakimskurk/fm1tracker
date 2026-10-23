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

Status: **IN PROGRESS — WP2.1 through WP2.8 complete; WP2.9 next**

Completed:

1. ✅ WP2.1 — compile-time V0.1 data model.
2. ✅ WP2.2 — one pool-backed runtime `FmtSong`.
3. ✅ WP2.3 — exact integer transport/no-drift clock.
4. ✅ WP2.4 — one-track audible frame-exact playback.
5. ✅ WP2.5 — D022 four-page white-key step addressing.
6. ✅ WP2.6 — D023 direct eight-track black-key selector prototype.
7. ✅ WP2.7 — D024 stopped STEP REC ordinary-note entry.
8. ✅ WP2.8 — D025 REST/TIE/OFF editor + playback semantics.
9. ⬜ WP2.9 — compact tracker screen, issue #17, READY TO START.
10. ⬜ WP2.10 — complete Phase 2 computer-free integration test.

Current verified baseline after WP2.8:

- app image: 56,748 B / 581,564 B;
- normal RAM: 12,420 B / 98,304 B;
- pool RAM: 96,616 B / 344,064 B;
- `FmtSong`: 65,896 B at `.pool` `0x01C20000`;
- STEP REC ordinary notes plus REST/TIE/OFF all advance exactly once modulo 64;
- explicit playback event kinds `NONE/NOTE/TIE/OFF` remain a 4-byte event;
- deterministic default WAV: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`;
- package identity: `FM-1_987`.

WP2.8 verified run: `40004436981`. Evidence: `WP2_8_VERIFICATION.md`.

Phase 2 exit criteria:

- a 64-step melody can be entered without a computer;
- REST/TIE/OFF are deterministic and visible;
- playback loops without cumulative timing drift;
- edits while stopped are bounds-safe;
- compact tracker display clearly distinguishes cursor/edit target from playback position;
- complete Phase 2 host/device regression passes.

### Immediate next package — WP2.9

Before code, freeze:

- visible tracker row count/window;
- track/page/header contents;
- edit cursor vs playhead markers;
- ordinary note / REST / TIE / OFF notation;
- STEP REC visual state;
- bounded dirty-region redraw policy.

No audio semantics should change in this display-only package.

---

## Phase 3 — parameter locks and tracker effects

Goal: make the sequencer genuinely tracker-like and hardware-native.

Planned:

- stable parameter ID registry;
- bounded sparse max-four-lock storage;
- hold/select step + encoder lock editing;
- lock replace/delete UX;
- pitch/volume controls;
- arpeggio, slide, portamento, vibrato;
- retrig, note delay, note cut;
- bounded event queue/overflow policy;
- visible lock/effect feedback;
- worst-case tests.

Exit criteria:

- worst-case lock data inside budget;
- no lock edit corrupts audio/song state;
- effect behavior host-tested and documented;
- all event work bounded.

---

## Phase 4 — 8-BIT engine

Goal: a musically complete chip-style palette.

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