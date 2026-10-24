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
8. Detailed completed-package evidence belongs in `WP*_VERIFICATION.md`; this file stays focused on execution order and boundaries.

---

# Phase 1 — clean Dinghy baseline

Status: **COMPLETE — 2026-10-23**

- WP1.1 upstream provenance freeze — COMPLETE
- WP1.2 minimal complete Dinghy import — COMPLETE
- WP1.3 pristine baseline verification — COMPLETE
- WP1.4 FM1Tracker identity rename — COMPLETE
- WP1.5 renamed baseline verification and budget capture — COMPLETE

Evidence: `WP1_3_VERIFICATION.md`, `WP1_4_VERIFICATION.md`, `WP1_5_VERIFICATION.md`.

Final Phase 1 baseline: 55,088 B image, 12,368 B normal RAM, 30,720 B pool RAM, identity `FM-1_987`.

---

# Phase 2 — tracker core

Status: **COMPLETE — 2026-10-23**

## Completed packages

- **WP2.1 — compile-time data model** — COMPLETE — issue #9  
  Frozen 8 tracks × 64 steps × 16 patterns, 8-byte `FmtStep`, deterministic sentinels/bounds. Evidence: `WP2_1_VERIFICATION.md`.

- **WP2.2 — bounds-safe pattern store** — COMPLETE — issue #10  
  One 65,896-byte `FmtSong` in `.pool` at `0x01C20000`. Evidence: `WP2_2_VERIFICATION.md`.

- **WP2.3 — integer transport clock** — COMPLETE — issue #11  
  D020 exact integer 1/16-note timing and deterministic 63 -> 0 wrap. Evidence: `WP2_3_VERIFICATION.md`.

- **WP2.4 — one-track audible playback** — COMPLETE — issue #12  
  D021 frame-exact one-lane playback and frozen deterministic host WAV. Evidence: `WP2_4_VERIFICATION.md`.

- **WP2.5 — 16-step page input** — COMPLETE — issue #13  
  D022 four clamped pages, 16 physical white step keys, no song mutation. Evidence: `WP2_5_VERIFICATION.md`.

- **WP2.6 — track selection prototype** — COMPLETE — issue #14  
  D023 eight black keys -> logical tracks 0..7, three black keys reserved. Evidence: `WP2_6_VERIFICATION.md`.

- **WP2.7 — STEP REC note entry** — COMPLETE — issue #15  
  D024 stopped-transport one-note-per-gesture editor with exact cursor advance. Evidence: `WP2_7_VERIFICATION.md`.

- **WP2.8 — REST / TIE / NOTE-OFF semantics** — COMPLETE — issue #16  
  D025 special-event editor/playback semantics: FX release=REST, SELECT=TIE, hold FX then SELECT=OFF; explicit playback `NONE/NOTE/TIE/OFF`; frozen default WAV preserved. Evidence: `WP2_8_VERIFICATION.md`.

- **WP2.9 — compact tracker screen** — COMPLETE — issue #17  
  D026 cursor-owned one-track eight-row tracker display, explicit cursor/playhead, REST/TIE/OFF notation, ten bounded dirty regions and max two redraws per main-loop poll. Evidence: `WP2_9_SCREEN_CONTRACT.md`, `WP2_9_VERIFICATION.md`.

- **WP2.10 — Phase 2 integration test and closeout** — COMPLETE — issue #18  
  Full 64-step/eight-track editor integration, REST/TIE/OFF, exact boundary/wrap behavior, complete-song isolation, full-app panel/display/HOME proof, inherited regressions and unchanged device binary. Final run `40015242947`. Evidence: `WP2_10_TEST_PLAN.md`, `PHASE2_VERIFICATION.md`.

Final verified Phase 2 footprint:

- app image: 59,252 B / 581,564 B
- normal RAM: 12,464 B / 98,304 B
- pool RAM: 96,616 B / 344,064 B
- total delta vs Phase 1: +4,164 B image / +96 B normal RAM / +65,896 B pool
- song store: 65,896 B at `.pool` `0x01C20000`
- `FmtStepRecord`: 3 B
- `FmtPatternInput`: 4 B
- `FmtTransport`: 8 B
- `FmtTrackerScreen`: 44 B
- `FmtPlayback`: 1 B
- integrated reviewed framebuffer: `a993551ce922d90186080bf554b82477b4336e160a6b301c446f0d275569b6d7`
- frozen default WAV: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`
- package identity: `FM-1_987`

WP2.10 changed no production source and added zero production resource cost versus WP2.9.

Evidence: `PHASE2_VERIFICATION.md`.

---

# Phase 3 — p-locks and tracker effects

Status: **IN PROGRESS — WP3.1–WP3.9 complete; WP3.10 active**

Parent issue: #20

## Completed packages

- **WP3.1 — stable parameter ID registry** — COMPLETE — issue #19
  D027 freezes one-byte semantic parameter identity. Final run `40025790828`. Evidence: `WP3_1_VERIFICATION.md`.

- **WP3.2 — sparse four-lock-per-step storage** — COMPLETE — issue #21
  D028 freezes the owner-checked 2,048-record global sparse pool. Final run `40038453521`. Evidence: `WP3_2_VERIFICATION.md`.

- **WP3.3 — hold-step + encoder p-lock editing** — COMPLETE — issue #22
  D029 freezes held-step target ownership and KNOB1..4 normalized editing. Final run `40054322180`. Evidence: `WP3_3_VERIFICATION.md`.

- **WP3.4 — p-lock browse / single-lock delete UX** — COMPLETE — issue #23
  D030 freezes sorted SELECT browse plus deliberate one-lock delete confirmation. Final run `40064122343`. Evidence: `WP3_4_VERIFICATION.md`.

- **WP3.5 — basic pitch/volume p-lock playback** — COMPLETE — issue #24
  D031 applies VOICE LEVEL, FINE PITCH and TRACK LEVEL through a fixed 256 B proof-lane row cache; TRACK PAN remains storage-only. Final run `40069110111`. Evidence: `WP3_5_VERIFICATION.md`.

- **WP3.6 — arpeggio / slide / portamento / vibrato tracker effects** — COMPLETE — issue #25
  D032 assigns `NONE/ARP/SLIDE_UP/SLIDE_DOWN/PORTA/VIBRATO` in `FmtStep.effect/effect_value`, freezes an exact six-tick D020-derived row clock, persistent Q7 slide/porta pitch, transient ARP/vibrato, D031 FINE precedence, a 256 B effect-row cache and 12 B bounded runtime state. Final run `40073680548`. Final resources: 65,528 B image / 13,004 B normal RAM / 113,000 B pool. Evidence: `WP3_6_EFFECT_CONTRACT.md`, `WP3_6_VERIFICATION.md`.

- **WP3.7 — retrig / note delay / note cut** — COMPLETE — issue #26
  D033 extends the same command domain with RETRIG `0x06`, NOTE DELAY `0x07`, NOTE CUT `0x08`, exact D032 six-tick scheduling and a 4 B fixed row-event state. No general queue was pulled forward. Final clean-tree run `40077809743`, job `120824870575`. Final resources: 65,940 B image / 13,008 B normal RAM / 113,000 B pool. Evidence: `WP3_7_EVENT_CONTRACT.md`, `WP3_7_VERIFICATION.md`.

- **WP3.8 — bounded event queue and overflow policy** — COMPLETE — issue #27
  D034 freezes an 8-byte event envelope and exact 32-entry / 268-byte SPSC main->audio queue, reject-newest overflow, wrap-safe sequence counters and click-safe fail-to-silence audition recovery. Physical-key and USB-MIDI audition no longer mutate manual audio voices from main. Final clean-tree run `40079824265`, job `120830854974`. Final resources: 66,308 B image / 13,280 B normal RAM / 113,000 B pool. Evidence: `WP3_8_QUEUE_CONTRACT.md`, `WP3_8_VERIFICATION.md`.

- **WP3.9 — lock/effect visual feedback** — COMPLETE — issue #28
  D035 extends the existing ten-region / max-two-redraw tracker surface with `LK` p-lock summaries, stored D032/D033 effect mnemonics/values and `STORE` detail for TRACK PAN while preserving main-only bounded drawing. `FmtTrackerScreen` is exactly 62 B. Final run `40083510351`, job `120841687772`. Final resources: 66,960 B image / 13,296 B normal RAM / 113,000 B pool. Evidence: `WP3_9_VISUAL_CONTRACT.md`, `WP3_9_VERIFICATION.md`.

Current verified baseline after WP3.9:

- app image: 66,960 B / 581,564 B
- normal RAM: 13,296 B / 98,304 B
- pool RAM: 113,000 B / 344,064 B
- D031 playback row cache: 256 B ordinary RAM
- D032 effect row cache: 256 B ordinary RAM
- D032 effect runtime: 12 B
- D033 row-event runtime: 4 B
- D034 runtime queue: 268 B ordinary RAM + 4 B overflow acknowledgement
- D035 `FmtTrackerScreen`: 62 B ordinary RAM, ten regions / max two redraws
- default WAV: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`
- D031 locked WAV: `8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8`
- package identity: `FM-1_987`

## Next package

- **WP3.10 — Phase 3 worst-case tests and exit gate** — IN PROGRESS — issue #29.
  D036 is frozen in `WP3_10_TEST_PLAN.md`. Stress the complete D027-D035 stack and close Phase 3; production source remains unchanged unless a frozen stress test exposes a real defect.

Remaining Phase 3 order:

- WP3.10 Phase 3 worst-case tests and exit gate

---

# Phase 4 — 8-BIT engine

- WP4.1 engine interface contract
- WP4.2 PULSE
- WP4.3 NOISE
- WP4.4 TRIANGLE
- WP4.5 WAVETABLE-8
- WP4.6 envelope primitive
- WP4.7 deterministic voice allocation/stealing
- WP4.8 four-parameter pages + p-lock mapping
- WP4.9 pitch/retrigger integration
- WP4.10 eight-track hardware benchmark

---

# Phase 5 — 16-BIT FM4 engine

- WP5.1 FM operator primitive
- WP5.2 curated algorithm routing
- WP5.3 ratio table
- WP5.4 levels/modulation index
- WP5.5 bounded feedback
- WP5.6 common envelope
- WP5.7 deterministic voice allocation
- WP5.8 parameter pages + p-lock mapping
- WP5.9 pitch/modulation validation
- WP5.10 worst-case hardware benchmark

---

# Core milestone after Phases 4/5 — live performance capture and performance FX

Status: **PLANNED CORE — package IDs deferred until Phase 3–5 measurement exists**

This milestone is mandatory product work, not optional Phase 9 expansion. Detailed direction: `LIVE_PERFORMANCE_CAPTURE_VISION.md`.

Future decomposition must cover at least:

- running-transport REC arm/disarm without breaking stopped STEP REC;
- deterministic live note capture to the selected logical track;
- fast capture-target track switching while transport continues;
- drum-oriented live capture;
- transport-derived metronome/count-in policy;
- quantization/timestamp and overwrite/merge/overdub rules;
- CHANNEL versus GLOBAL performance/automation scope;
- keyboard-distributed FX performance gestures where useful;
- encoder/pot capture into bounded p-lock/automation/effect events;
- global automation ownership/storage where normal track locks are inappropriate;
- eight-track reference configuration containing drums, bass, poly synth and lead synth while retaining four flexible tracks;
- measured 3–6 voice target for at least one polyphonic instrument/track if hardware budget permits;
- deterministic voice allocation and host/hardware stress tests.

Do not assign concrete storage formats/package IDs until Phase 3 p-lock/event structures and Phase 4/5 engine budgets are measured.

---

# Phase 6 — song/order + persistence

- WP6.1 order-list data model
- WP6.2 safe pattern switching
- WP6.3 repeats/loop ranges
- WP6.4 project binary format v1
- WP6.5 flash-region audit
- WP6.6 save path
- WP6.7 load + CRC validation
- WP6.8 corrupted-project recovery
- WP6.9 migration/version harness
- WP6.10 power-cycle round-trip test

---

# Phase 7 — hardware UX freeze

- WP7.1 STEP REC/REST/TIE/OFF hardware test
- WP7.2 step-page ergonomics
- WP7.3 track-selection ergonomics
- WP7.4 p-lock ergonomics
- WP7.5 chord-entry behavior
- WP7.6 tracker-screen readability
- WP7.7 final V1 control-map freeze

The final hardware UX freeze must also validate the live-performance milestone: seamless PLAY/REC, track switching during playback/capture, drum/poly workflow, metronome and CHANNEL/GLOBAL FX/automation gestures.

---

# Phase 8 — interoperability

- WP8.1 internal project interchange spec
- WP8.2 MIDI pattern/project transfer investigation
- WP8.3 Furnace documented export audit
- WP8.4 one-way supported-subset converter prototype
- WP8.5 round-trip feasibility decision

---

# Phase 9 — optional expansion

Optional features get dedicated packages only after profiling proves room exists. Candidates include WAVETABLE-16, PCM/sample playback beyond specifically approved core performance needs, swing, additional microtiming, polymeter, mixed engines, additional DSP FX algorithms and specifically licensed chip cores.

**Live performance capture is not optional Phase 9 work.**

Nothing in Phase 9 may become a hidden V1 prerequisite.
