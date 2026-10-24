# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 3 — p-locks and tracker effects**

State: **IN PROGRESS — WP3.4 active — 2026-10-24**

Parent issue: #20

Current branch: `phase3/plocks-effects`

Phase 3 branch start / completed Phase 2 handoff:

`c3693d8c7bb3d5306794aa232d6d25bef50c6c99`

## Completed WP3.1

**WP3.1 — stable parameter ID registry** — COMPLETE — issue #19

Verified checkpoint: `5af8065a9c98884d22682075b600d9790cfa7940`

Evidence: `docs/WP3_1_VERIFICATION.md`

D027 freezes one-byte semantic parameter identity and exactly four current production-lockable IDs: VOICE LEVEL, FINE PITCH, TRACK LEVEL and TRACK PAN.

## Completed WP3.2

**WP3.2 — sparse four-lock-per-step storage** — COMPLETE — issue #21

Verified checkpoint: `da2f5afe9b1da6defc3857b91f52b40d7525f33b`

Evidence: `docs/WP3_2_VERIFICATION.md`

D028 freezes one owner-checked global sparse pool of 2,048 eight-byte lock records, max four active locks per individual step, deterministic sorted lists and explicit STEP_FULL / POOL_FULL / CORRUPT behavior.

## Completed WP3.3

**WP3.3 — hold-step + encoder p-lock editing** — COMPLETE — issue #22

WP3.3 verified source/workflow checkpoint:

`df5d7e07fc763909a141c99283cfd2be19cff847`

Final GitHub Actions:

- run `40054322180`
- job `120753812201` (`lock-editor-closeout`)
- result **SUCCESS**

Durable evidence: `docs/WP3_3_VERIFICATION.md`

D029 freezes ordinary-SEQ held-white-step target ownership, stale/begin/release KNOB1..4 delta discard, the four current D027 mappings and normalized `0..255` storage-only lock editing.

Final WP3.3 resources:

| Resource | Used | Limit | Free |
|---|---:|---:|---:|
| app image | 61,716 B | 581,564 B | 519,848 B |
| normal RAM | 12,476 B | 98,304 B | 85,828 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B |

Frozen default WAV remains:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

## Current milestone

**WP3.4 — p-lock replace/delete UX**

State: **IN PROGRESS — D030 frozen, focused editor gate green — 2026-10-24**

GitHub issue: #23

WP3.4 start head:

`8ffa2a3de00f89946aca37fa7015fd3940851c93`

### Frozen D030 contract

D030 is recorded in `docs/DECISIONS.md` and the control map before production source implementation.

The current prototype keeps D029's ordinary-SEQ held-white-step target and adds:

- SELECT encoder browse through existing locks in D028 ascending parameter-ID order;
- clamped browse, never wrapping;
- successful KNOB edit selects that semantic parameter;
- EDIT arms deletion only, with no mutation;
- only a later-poll SEL press confirms one selected-lock deletion;
- same-poll EDIT+SEL never deletes;
- EDIT release, target/context exit, PLAY/unrelated focus intent or storage error cancels the arm;
- encoder deltas while armed are drained but discarded;
- exactly one D028 `fmt_lock_remove()` after revalidation;
- deterministic post-delete selection at the deleted ordinal, then previous last, then NONE;
- no user-facing clear-all gesture in WP3.4;
- footer-only feedback; full lock visualization remains WP3.9;
- no playback/audio consumption of locks.

### Current implementation checkpoint

`FmtLockEditor` has been extended from 8 B to a compile-time asserted 10 B by adding only:

- selected semantic parameter ID;
- delete-armed flag.

The editor now provides bounded browse, arm, cancel and one-lock confirm/delete helpers over the unchanged D028 APIs. No storage layout, pool capacity, parameter ID or row format has changed.

Focused GitHub Actions after the first presentation-priority correction:

- run `40063442383`
- result **SUCCESS**

The focused suite proves:

- D028 sorted/clamped browse;
- zero-lock safe feedback;
- D029 KNOB write selects its parameter;
- arm/cancel are non-destructive;
- first/middle/last/only deletion;
- exactly-one-record free-list recovery;
- neighbor isolation;
- current-ID RANGE-value locks remain removable;
- NOT_FOUND/BAD_ADDRESS/CORRUPT safe handling;
- no clear-all editor action.

A small test-only portability cleanup followed the green run and does not change production semantics.

### Next implementation step

Connect D030 to the real `dinghy.c` main-loop input path:

- drain SELECT every poll alongside KNOB1..4;
- preserve D029 begin/release stale-delta discard;
- enforce same-poll EDIT+SEL rejection;
- cancel pending arm before PLAY/unrelated focus handling;
- suppress all encoder consumption on any poll that began with or creates a delete arm;
- process stable SELECT browse before K1..4 edits;
- add a real full-app panel/display fixture with zero unintended MIDI;
- preserve the frozen WP3.3 framebuffer regressions.

### Strict non-goals

- no playback/audio application of p-locks — WP3.5+;
- no tracker-effect semantics;
- no new parameter IDs unless separately justified;
- no lock storage-layout/capacity change;
- no clear-all user gesture in WP3.4;
- no full row lock visualization — WP3.9;
- no synth-engine work;
- no live-performance capture/automation;
- no persistence/file-format work;
- no final hardware control-map freeze.

## Important decisions / do not regress

- D005: 8 logical tracks × 64 steps × 16 patterns.
- D010: p-locks are first-class bounded sequencer data.
- D018: `FmtStep` remains exactly 8 B.
- D019: one pool-backed runtime `FmtSong`.
- D020-D026: completed Phase 2 timing/editor/display semantics remain regression requirements.
- D027: stable one-byte semantic parameter identity.
- D028: 2,048-record owner-checked global sparse lock pool, max four per step.
- D029: held-white-step snapshot + relative KNOB1..4 storage editing.
- D030: SELECT browse + armed EDIT -> later SEL one-lock deletion, no clear-all UI.
- Preserve the frozen WAV unless a deliberate later audio change records a superseding decision/test.
- No float in firmware DSP and no runtime 64-bit division.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted future core direction. Current Phase 3 locks are ordinary track/step-owned data. Do not silently force future GLOBAL/master automation into D028 ownership.

## Hardware limitation still open

No physical FM-1 is currently available. Host/device-build verification does not claim physical hold/browse/delete comfort, actual panel scan feel, display update feel, physical install/boot/audio behavior or measured AC791N execution timing.

## Exact next action

Integrate D030 into the real main loop, add full-app host evidence, then run the complete inherited/device/resource gate. Do not begin WP3.5.

## Last relevant checkpoints

- Phase 2 verified implementation/workflow: `7063be51e53c2657178084b3bef5ace813dafcd7`
- Phase 3 branch start: `c3693d8c7bb3d5306794aa232d6d25bef50c6c99`
- WP3.1 verified: `5af8065a9c98884d22682075b600d9790cfa7940`
- WP3.2 verified: `da2f5afe9b1da6defc3857b91f52b40d7525f33b`
- WP3.3 verified: `df5d7e07fc763909a141c99283cfd2be19cff847`
- WP3.4 start head: `8ffa2a3de00f89946aca37fa7015fd3940851c93`
- WP3.4 focused green run: `40063442383`

## Last updated

2026-10-24 — D030 frozen and focused WP3.4 editor tests green. Full-app D030 input integration is next.
