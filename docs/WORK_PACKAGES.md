# FM1Tracker Work Packages

This document translates `ROADMAP.md` into small, bounded units of work. The roadmap defines direction; work packages define what may be changed now.

## Rules

1. Work on one active package at a time unless a dependency genuinely requires otherwise.
2. A package is complete only when every exit criterion is satisfied or explicitly waived in `DECISIONS.md`.
3. Do not pull later-phase features into an earlier package for convenience.
4. Every package that changes firmware behavior must add or update tests/benchmarks appropriate to that layer.
5. Update `PROJECT_STATE.md` when a package starts, is blocked, or completes.
6. Architectural deviations must be recorded in `DECISIONS.md` before they spread into code.
7. Third-party code/assets require provenance and license review in `THIRD_PARTY.md` before import.

---

# Phase 1 — clean Dinghy baseline

## WP1.1 — upstream provenance freeze

**Goal:** make the exact platform origin reproducible before importing firmware code.

Deliverables:
- pinned Dinghy commit recorded;
- upstream license/provenance recorded;
- excluded non-GPL branding/assets documented;
- required external JieLi SDK version documented;
- exact import manifest with pinned upstream blob IDs.

Exit:
- another developer can identify exactly what upstream revision FM1Tracker is based on;
- no third-party file can enter the tree without a known license/source;
- WP1.2 can perform a mechanical/reviewable import without redefining scope.

Status: **COMPLETE — 2026-10-23**

## WP1.2 — minimal complete Dinghy import

**Goal:** import the smallest complete upstream tree that builds both host and device firmware.

Deliverables:
- platform/HAL/loader/build tools;
- host test harness;
- required generated font + OFL license;
- upstream source headers preserved;
- Dinghy logo/branding assets omitted;
- import checked against `DINGHY_IMPORT_MANIFEST.md`.

Exit:
- imported files correspond to the pinned upstream commit;
- no FM1Tracker tracker/synth modifications yet;
- import manifest can be reviewed against upstream.

Status: **COMPLETE — 2026-10-23**

## WP1.3 — pristine baseline verification

**Goal:** prove the imported upstream baseline works before renaming or feature changes.

Deliverables:
- host build attempt/results;
- host WAV/screen output if build environment permits;
- firmware build attempt/results;
- exact failures recorded if external toolchain/SDK prevents completion.

Exit:
- pristine baseline is either verified or has a precisely documented external dependency blocker.

Status: **COMPLETE — 2026-10-23**

Verification record: `WP1_3_VERIFICATION.md`. Host build/run and artifacts are verified. The JieLi toolchain is verified. Device firmware build remains environment-blocked only by the separately required AC79 SDK checkout; no physical FM-1 test has been performed.

## WP1.4 — FM1Tracker identity rename

**Goal:** make the firmware identify as FM1Tracker without changing musical behavior.

Deliverables:
- app name/build artifact rename;
- package identity selected after checking the current FM-1 ecosystem;
- project-owned minimal visual identity;
- no inherited Dinghy logo.

Exit:
- host screen/build artifacts identify FM1Tracker;
- package identity does not collide with known firmware.

Status: **COMPLETE — 2026-10-23**

Result: fixed identity `FM-1_987`; host screen/artifacts identify FM1Tracker; host WAV is bit-identical to WP1.3; protected Dinghy/HAL/loader/build implementation remains unchanged. Full renamed device package execution is explicitly deferred to WP1.5 under D017 because the external AC79 SDK checkout is still unavailable. Evidence: `WP1_4_VERIFICATION.md`.

## WP1.5 — renamed baseline verification and budget capture

**Goal:** establish the reference numbers every later phase must preserve or consciously spend.

Deliverables:
- host run after rename;
- firmware build after rename;
- code/RAM/pool usage recorded;
- audio callback baseline timing recorded where measurable;
- reproducible setup instructions updated.

Exit:
- clean renamed baseline is reproducible;
- memory/code/timing baseline is in repo;
- Phase 2 can begin without changing platform plumbing.

Status: **READY TO START — 2026-10-23**

First action: audit what WP1.4 evidence can be reused, then attempt the renamed device build with the verified JieLi toolchain and a separately obtained AC79 SDK checkout if available. If the SDK remains unavailable, record the exact blocker and complete every host/static budget measurement that does not require fabricating or vendoring SDK material.

---

# Phase 2 — tracker core

## WP2.1 — compile-time data model
Implement V0.1 structs/constants/accessors with static size assertions and no UI/audio changes.

## WP2.2 — bounds-safe pattern store
Allocate pattern/song state in the intended memory region and test all access boundaries.

## WP2.3 — integer transport clock
Implement deterministic step clock and host tests for wrap/drift.

## WP2.4 — one-track audible playback
Use the simplest baseline tone to prove note events and 64-step looping.

## WP2.5 — 16-step page input
White keys select 1–16 with four pages covering 64 steps.

## WP2.6 — track selection prototype
Test the provisional eight-black-key track selection mapping.

## WP2.7 — STEP REC note entry
Keyboard note entry advances the cursor deterministically.

## WP2.8 — REST / TIE / NOTE-OFF semantics
Implement and host-test the explicit event semantics before adding more effects.

## WP2.9 — tracker screen
Add compact tracker rows and dirty-region rendering.

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

Each optional feature receives its own package only after profiling proves room exists. Candidates: WAVETABLE-16, PCM, swing, microtiming, polymeter, live recording, mixed engines, FX, specific licensed chip cores.

Nothing in Phase 9 is allowed to become a hidden prerequisite for V1.
