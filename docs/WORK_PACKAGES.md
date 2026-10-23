# FM1Tracker Work Packages

This document translates `ROADMAP.md` into bounded implementation units. The roadmap defines direction; this file defines what may be changed now.

## Rules

1. Work on one active package at a time unless a dependency genuinely requires otherwise.
2. A package is complete only when every exit criterion passes or a waiver is explicitly recorded in `DECISIONS.md`.
3. Do not pull later-phase features forward for convenience.
4. Firmware behavior changes require tests/benchmarks appropriate to that layer.
5. Update `PROJECT_STATE.md` when a package starts, blocks or completes.
6. Architectural deviations must be recorded in `DECISIONS.md` before they spread into code.
7. Third-party code/assets require provenance/license review in `THIRD_PARTY.md` before import.

---

# Phase 1 — clean Dinghy baseline

Status: **COMPLETE — 2026-10-23**

- WP1.1 upstream provenance freeze — COMPLETE
- WP1.2 minimal complete Dinghy import — COMPLETE
- WP1.3 pristine baseline verification — COMPLETE
- WP1.4 FM1Tracker identity rename — COMPLETE
- WP1.5 renamed baseline verification and budget capture — COMPLETE

Durable evidence: `WP1_3_VERIFICATION.md`, `WP1_4_VERIFICATION.md`, `WP1_5_VERIFICATION.md`.

Final Phase 1 baseline: 55,088 B image, 12,368 B normal RAM, 30,720 B pool RAM, identity `FM-1_987`.

---

# Phase 2 — tracker core

## WP2.1 — compile-time data model

**Goal:** freeze the approved V0.1 tracker structures/constants/accessors without runtime tracker behavior.

Status: **COMPLETE — 2026-10-23** — issue #9

Verified:
- 8 tracks, 64 steps/pattern, 16 patterns;
- `FmtStep` 8 B, `FmtPattern` 4,096 B, `FmtSong` 65,896 B;
- deterministic sentinels and bounds-safe accessors;
- model + conservative future lock budget 82,280 B / 98,304 B target;
- runtime delta 0 B image / 0 B normal RAM / 0 B pool.

Evidence: `WP2_1_VERIFICATION.md`.

## WP2.2 — bounds-safe pattern store

**Goal:** allocate one real V0.1 song/pattern store in the intended memory region and make runtime access/reset deterministic.

Status: **COMPLETE — 2026-10-23** — issue #10

Verified:
- one 65,896-byte `FmtSong` linked in `.pool` at `0x01C20000`;
- all 8,192 steps explicitly initialized to tracker sentinels;
- bounds/mutation/reset tests pass;
- host audio unchanged;
- resulting resources: 55,188 B image, 12,368 B normal RAM, 96,616 B pool RAM.

Evidence: `WP2_2_VERIFICATION.md`.

## WP2.3 — integer transport clock

**Goal:** implement deterministic integer/fixed-point transport timing for the fixed V0.1 1/16-note grid without audible playback or editor/UI behavior.

Status: **COMPLETE — 2026-10-23** — issue #11

Verified:
- D020 exact rational timing: phase threshold `661500 = 44100 * 15`, add integer BPM per frame;
- no float, runtime division or firmware 64-bit arithmetic in the transport path;
- deterministic stopped/reset/start/stop/resume semantics;
- exact `63 -> 0` wrap;
- bulk and frame-by-frame timing are equivalent;
- six one-hour representative-BPM runs plus max-uint16 BPM reference test show no cumulative rounding drift;
- host audio remains byte-identical because transport remains stopped/no row dispatch;
- `FmtTransport` is 8 B in `.bss`;
- resulting resources: 55,328 B image, 12,376 B normal RAM, 96,616 B pool RAM.

Evidence: `WP2_3_VERIFICATION.md`.

## WP2.4 — one-track audible playback

**Goal:** use the verified transport and existing baseline sine primitive to prove deterministic audible playback of one logical tracker track through a complete 64-step pattern loop.

Status: **COMPLETE — 2026-10-23** — issue #12

Verified:
- D021 dispatches step 0 before playback frame 0 and applies crossed rows beginning on the next rendered frame;
- pattern 0 / track 0 only, one dedicated monophonic proof sine voice;
- deterministic 16-note proof pattern with one note every four steps;
- EMPTY sustains by producing no new event; TIE/OFF cannot become accidental raw pitches;
- event timing remains exact inside 256-frame callback grouping;
- at 120 BPM, step 1 begins on frame 5513 and the complete 64-step loop returns on frame 352800;
- deterministic audible host WAV SHA-256 `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`;
- tracker audio is already non-zero during the first 100 ms before simulated manual key input;
- flash settings writes are deferred while the continuous proof lane is sounding, preventing the inherited flash-erase path from stalling tracker audio;
- final resources: 55,692 B image, 12,396 B normal RAM, 96,616 B pool RAM;
- delta vs WP2.3: +364 B image, +20 B normal RAM, +0 B pool;
- package identity and inherited device safety gates remain green.

Evidence: `WP2_4_VERIFICATION.md`.

## WP2.5 — 16-step page input

**Goal:** add bounded edit-cursor/page addressing so the FM-1's 16 white keys can select all 64 pattern steps through four pages without writing notes yet.

Status: **COMPLETE — 2026-10-23** — issue #13

Verified:
- D022 defines explicit PERFORMANCE versus SEQ/pattern context;
- physical white-key indexes `0,2,4,6,7,9,11,12,14,16,18,19,21,23,24,26` map bijectively to local steps `0..15`;
- four pages cover global steps `0..63` exactly once with no gaps or overlap;
- `OCT-` / `OCT+` page navigation clamps at pages 0/3 and preserves local step position;
- `SEQ` enters pattern context and `HOME` returns to normal performance context;
- white keys select cursor only; black keys are rejected in WP2.5 and reserved for WP2.6;
- performance audition/MIDI behavior remains intact outside SEQ context;
- a performance note released after entering SEQ still receives NOTE OFF, preventing stuck voices;
- a byte-for-byte `FmtSong` snapshot proves selection/navigation never mutates song data;
- frozen WP2.4 host WAV remains exactly `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`;
- `FmtPatternInput` is 3 B in `.bss` at `0x01C0B060`;
- final resources: 55,980 B image, 12,400 B normal RAM, 96,616 B pool RAM;
- delta vs WP2.4: +288 B image, +4 B normal RAM, +0 B pool;
- final verification run `39994230797` passed with package identity and inherited device safety gates green.

Evidence: `WP2_5_VERIFICATION.md`.

## WP2.6 — track selection prototype

**Goal:** prototype direct eight-track selection with black keys inside the explicit SEQ context while preserving D022 white-key page addressing and keeping playback/edit semantics narrow.

Status: **COMPLETE — 2026-10-23** — issue #14

Verified:
- D023 maps physical black-key indexes `1,3,5,8,10,13,15,17` bijectively to logical tracks `0..7`;
- remaining black-key indexes `20,22,25` are reserved and rejected;
- selection is active only in SEQ/pattern context;
- PERFORMANCE context keeps normal black-key audition/MIDI behavior;
- selected track survives HOME/SEQ context changes;
- track selection does not move page/local/global step;
- white-key step selection and page navigation preserve selected track;
- complete `FmtSong` remains byte-identical across cursor/track navigation;
- held PERFORMANCE notes still receive NOTE OFF when released after entering SEQ;
- WP2.4 proof playback remains fixed to logical track 0 and the frozen host WAV remains byte-identical;
- `FmtPatternInput` is 4 B at `.bss` `0x01C0B060`;
- final resources: 56,060 B image, 12,400 B normal RAM, 96,616 B pool RAM;
- delta vs WP2.5: +80 B image, +0 B normal RAM, +0 B pool;
- final verification run `39995856373` passed with package identity and inherited device safety gates green.

Evidence: `WP2_6_VERIFICATION.md`.

## WP2.7 — STEP REC note entry

**Goal:** write one deterministic ordinary note to the currently selected D022/D023 step and track while STEP REC is active, then advance the edit cursor exactly one logical step.

Status: **READY TO START** — issue #15

Required direction:
- define the exact REC/STEP REC entry/exit and transport-stopped contract before code;
- use the existing selected track and 64-step cursor as the write target;
- one valid note gesture writes exactly one ordinary note event to exactly one step/track;
- successful note entry advances exactly one logical step with explicit step-63 boundary behavior;
- performance audition remains intact outside STEP REC;
- ambiguous chord/multi-key behavior must be explicitly resolved or safely deferred, not left order-dependent;
- host-test write target, neighbor isolation, one-step advance, wrap and context gating;
- preserve prior regressions and measure device resource delta.

Non-goals:
- no REST/TIE/OFF editing — WP2.8;
- no tracker screen — WP2.9;
- no general multitrack playback, p-locks, new engine or persistence.

## WP2.8 — REST / TIE / NOTE-OFF semantics
Implement and host-test explicit event semantics before more effects.

## WP2.9 — tracker screen
Add compact tracker rows and bounded/dirty rendering.

## WP2.10 — Phase 2 integration test
Enter and play a complete 64-step melody without a computer.

---

# Phase 3 — p-locks and tracker effects

## WP3.1 — stable parameter ID registry
## WP3.2 — sparse four-lock-per-step storage
## WP3.3 — hold-step + encoder lock editing
## WP3.4 — lock replace/delete UX
## WP3.5 — basic pitch/volume effects
## WP3.6 — arpeggio/slide/porta/vibrato
## WP3.7 — retrig/note delay/note cut
## WP3.8 — bounded event queue and overflow policy
## WP3.9 — lock/effect visual feedback
## WP3.10 — Phase 3 worst-case tests

---

# Phase 4 — 8-BIT engine

## WP4.1 — engine interface contract
## WP4.2 — PULSE
## WP4.3 — NOISE
## WP4.4 — TRIANGLE
## WP4.5 — WAVETABLE-8
## WP4.6 — envelope primitive
## WP4.7 — deterministic voice allocation/stealing
## WP4.8 — four-parameter pages + p-lock mapping
## WP4.9 — pitch/retrigger integration
## WP4.10 — eight-track hardware benchmark

---

# Phase 5 — 16-BIT FM4 engine

## WP5.1 — FM operator primitive
## WP5.2 — curated algorithm routing
## WP5.3 — ratio table
## WP5.4 — levels/modulation index
## WP5.5 — bounded feedback
## WP5.6 — common envelope
## WP5.7 — deterministic voice allocation
## WP5.8 — parameter pages + p-lock mapping
## WP5.9 — pitch/modulation validation
## WP5.10 — worst-case hardware benchmark

---

# Phase 6 — song/order + persistence

## WP6.1 — order-list data model
## WP6.2 — safe pattern switching
## WP6.3 — repeats/loop ranges
## WP6.4 — project binary format v1
## WP6.5 — flash-region audit
## WP6.6 — save path
## WP6.7 — load + CRC validation
## WP6.8 — corrupted-project recovery
## WP6.9 — migration/version harness
## WP6.10 — power-cycle round-trip test

---

# Phase 7 — hardware UX freeze

## WP7.1 — STEP REC/REST hardware test
## WP7.2 — step-page ergonomics
## WP7.3 — track-selection ergonomics
## WP7.4 — p-lock ergonomics
## WP7.5 — chord-entry behavior
## WP7.6 — tracker-screen readability
## WP7.7 — final V1 control-map freeze

---

# Phase 8 — interoperability

## WP8.1 — internal project interchange spec
## WP8.2 — MIDI pattern/project transfer investigation
## WP8.3 — Furnace documented export audit
## WP8.4 — one-way supported-subset converter prototype
## WP8.5 — round-trip feasibility decision

---

# Phase 9 — optional expansion

Each optional feature gets its own package only after profiling proves room exists. Candidates include WAVETABLE-16, PCM, swing, microtiming, polymeter, live recording, mixed engines, FX and specifically licensed chip cores.

Nothing in Phase 9 may become a hidden V1 prerequisite.
