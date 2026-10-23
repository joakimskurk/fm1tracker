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
- `FmtTransport` is 8 B in `.bss` at `0x01C0B050`;
- resulting resources: 55,328 B image, 12,376 B normal RAM, 96,616 B pool RAM.

Evidence: `WP2_3_VERIFICATION.md`.

## WP2.4 — one-track audible playback

**Goal:** use the verified transport and existing baseline sine voice to prove deterministic audible playback of one logical tracker track through a complete 64-step pattern loop.

Status: **READY TO START** — issue #12

Deliverables:
- use the WP2.3 frame-accurate transport in playback;
- read one selected logical track from one 64-step pattern in the existing bounds-safe store;
- decode only the minimum note behavior needed for this playback proof;
- trigger events on exact transport frame boundaries;
- deterministic event trace/host playback fixture;
- verify exact 64-step loop timing;
- device build plus image/RAM/pool delta against WP2.3.

Exit:
- deterministic one-track 64-step fixture produces the expected note-event sequence;
- events land on the verified transport boundaries;
- step 63 loops to 0 with no cumulative timing drift;
- host playback evidence is deterministic and documented;
- WP2.1/WP2.2/WP2.3 regression tests remain green;
- device build/safety gates remain green;
- resource delta is recorded.

Non-goals:
- no STEP REC/editing;
- no 16-step page input or track-selection UX;
- no general multitrack playback;
- no tracker screen;
- no p-lock/effect work;
- no new 8-BIT/FM4 engine;
- no song/order mode or persistence.

Do not begin WP2.5 in the same package.

## WP2.5 — 16-step page input
White keys select steps 1–16 with four pages covering all 64 steps.

## WP2.6 — track selection prototype
Test the provisional eight-black-key track-selection mapping.

## WP2.7 — STEP REC note entry
Keyboard note entry advances the cursor deterministically.

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