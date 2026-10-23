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

Verification record: `WP1_3_VERIFICATION.md`. Host build/run and artifacts are verified. The JieLi toolchain is verified. The pristine device build was environment-blocked at this package boundary; the equivalent protected baseline is fully device-built after the identity layer in WP1.5.

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

Result: fixed identity `FM-1_987`; host screen/artifacts identify FM1Tracker; host WAV is bit-identical to WP1.3; protected Dinghy/HAL/loader/build implementation remains unchanged. Evidence: `WP1_4_VERIFICATION.md`.

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

Status: **COMPLETE — 2026-10-23**

Verified result:
- official external AC79 SDK tag/commit pinned and hash-checked;
- renamed host baseline passes and remains audio-identical to WP1.3;
- renamed firmware/package build passes with identity `FM-1_987`;
- inherited build safety checks pass;
- app image: 55,088 B / 581,564 B;
- normal RAM: 12,368 B / 98,304 B;
- pool RAM: 30,720 B / 344,064 B;
- nominal 256-frame/44.1 kHz callback period: 5.804988662 ms;
- actual physical-hardware render time remains unmeasured because no FM-1 is currently available.

Evidence: `WP1_5_VERIFICATION.md`.

**Phase 1 is COMPLETE.**

---

# Phase 2 — tracker core

## WP2.1 — compile-time data model

**Goal:** implement the already-approved V0.1 tracker structures/constants/accessors without changing UI or audio behavior.

Deliverables:
- compile-time constants for 8 tracks, 64 steps/pattern, 16 patterns and max four p-locks/step;
- V0.1 song/pattern/track/step structures from `DATA_MODEL_V0.1.md`;
- bounds-safe accessors appropriate to this layer;
- static size/budget assertions;
- host tests for sizes, defaults and access bounds;
- measured memory delta versus the Phase 1 baseline where buildable.

Exit:
- data model compiles on host/device;
- sizes and bounds are deterministic and tested;
- no UI/audio/transport behavior is introduced;
- the implementation stays inside the documented pool/RAM budget.

Status: **COMPLETE — 2026-10-23**

Verified result:
- `FmtStep` = 8 B, `FmtPattern` = 4,096 B, compile-time `FmtSong` core shape = 65,896 B;
- dimensions/defaults/sentinels and pattern/track/step bounds are host-tested;
- conservative future lock-storage reservation keeps the budgeted model at 82,280 B within the 96 KiB editable-song target;
- no pattern/song backing store or lock allocator is instantiated yet;
- device build, package identity and inherited safety checks pass;
- host WAV, firmware binary and firmware package remain bit-identical to Phase 1;
- measured runtime resource delta: image **0 B**, normal RAM **0 B**, pool RAM **0 B**.

Evidence: `WP2_1_VERIFICATION.md`.

## WP2.2 — bounds-safe pattern store

**Goal:** allocate pattern/song state in the intended memory region and make all pattern/track/step access deterministic and bounds-safe at runtime.

Deliverables:
- exactly one runtime `FmtSong` backing store;
- store placed in the linker `.pool` region rather than ordinary `.bss`;
- deterministic explicit initialization/reset of every step to the WP2.1 empty/inherit/no-lock sentinels;
- narrow bounds-safe song/pattern/step store API;
- host tests for all defaults, valid boundaries, out-of-range rejection, mutation isolation and reset;
- device symbol/address inspection plus actual image/RAM/pool delta.

Exit:
- all 8,192 steps initialize deterministically;
- first/last valid accesses work and every out-of-range dimension is rejected safely;
- reset after mutation restores defaults;
- linked device symbol proves the store is in `.pool`;
- host audio remains unchanged;
- build/safety gates remain green;
- normal RAM does not absorb the song store.

Status: **COMPLETE — 2026-10-23**

Verified result:
- one 65,896-byte `FmtSong` is linked at `.pool` address `0x01C20000`;
- all 8,192 steps are explicitly initialized to tracker sentinels;
- store bounds, mapping, mutation isolation and reset are host-tested;
- host WAV remains byte-identical to Phase 1/WP2.1;
- device build/package identity and inherited safety checks pass;
- app image: 55,188 B (**+100 B** vs WP2.1);
- normal RAM: 12,368 B (**0 B delta**);
- pool RAM: 96,616 B (**+65,896 B**, exactly `sizeof(FmtSong)`);
- remaining physical pool: 247,448 B;
- no transport/playback/editor/UI/p-lock/synth/persistence behavior added.

Evidence: `WP2_2_VERIFICATION.md`.

## WP2.3 — integer transport clock

**Goal:** implement deterministic integer/fixed-point transport timing for the V0.1 1/16-note grid without audible playback or editor/UI behavior.

Required direction:
- advance against the 44.1 kHz frame domain;
- use an accumulated remainder/error rather than repeatedly rounding a samples-per-step interval;
- make stopped/reset/start state deterministic;
- wrap exactly from step 63 to step 0;
- host-test representative BPM values and long-run timing error/drift;
- preserve WP2.2 store and host/device regression gates.

Non-goals:
- no audible note/voice triggering;
- no sequencer-to-audio event dispatch;
- no STEP REC/REST/TIE/NOTE-OFF editing;
- no page/track UI or tracker screen;
- no swing/polymeter/general microtiming;
- no p-lock/synth/persistence work.

Status: **READY TO START** — GitHub issue #11

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
