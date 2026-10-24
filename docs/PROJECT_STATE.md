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

State: **IN PROGRESS — 2026-10-24**

GitHub issue: #23

WP3.4 start head:

`8ffa2a3de00f89946aca37fa7015fd3940851c93`

No WP3.4 production implementation existed at this checkpoint.

### Exact objective

Add deliberate browse/select/delete behavior on top of D029's fixed held-step target, using the already-verified D028 lookup/iteration/remove primitives, without applying locks to playback/audio and without changing lock storage format/capacity.

D030 must be recorded in `DECISIONS.md` before WP3.4 source changes and must freeze:

- exact existing-lock browse/selection behavior;
- exact deliberate one-lock deletion gesture;
- whether clear-all is exposed at all in this package;
- destructive-action arming/confirmation/cancellation;
- interaction with D029 held-white-step ownership and KNOB1..4 edits;
- stale encoder protection for any newly consumed encoder stream;
- zero-lock / missing-selection behavior;
- D028 BAD_ADDRESS / CORRUPT / NOT_FOUND handling;
- fixed input precedence against white/black keys, OCT, PLAY, REC and HOME;
- bounded footer feedback only, leaving full lock visualization to WP3.9.

### Reviewed D030 direction before source

The intended prototype direction to freeze before implementation is:

- WP3.4 remains ordinary-SEQ-only and requires an active D029 held-white-step target;
- `ENC_SELECT` browses only the locks already present on that snapshotted target, in D028's strict ascending parameter-ID order;
- browse is clamped, not wrapping, and max-four bounded;
- the currently edited KNOB1..4 parameter becomes the selected lock after a successful D029 set/replace;
- `BTN_EDIT` does not delete immediately; it arms deletion of the currently selected lock;
- a later `BTN_SEL` press confirms deletion only while that arm is still valid;
- releasing EDIT, target/context cancellation, unrelated focus-changing intent or a storage error cancels the destructive arm before mutation;
- EDIT+SEL arriving together in the same poll may not delete: confirmation must occur after a previously armed stable poll;
- no clear-all gesture is exposed in WP3.4. With at most four locks per step, deliberate one-by-one removal is the safer prototype; D028 `fmt_lock_clear()` remains available internally for later UX decisions/tests;
- after a successful delete, the held-step target remains active and selection deterministically moves to the next remaining lock, otherwise the previous last lock, otherwise NONE;
- deletion changes only D028 lock storage and the target step's `lock_head` as required; row note/instrument/volume/effect fields and playback/base parameters remain untouched;
- SELECT encoder deltas are drained every poll and discarded while inactive and on begin/release/cancel polls, matching D029's stale-delta safety model;
- full per-row lock markers remain WP3.9.

### Required verification

1. D030 is recorded before source implementation;
2. browse order exactly matches D028 sorted parameter order and clamps at first/last lock;
3. zero-lock browse is a safe no-op with explicit bounded feedback;
4. a successful KNOB edit selects that parameter for later delete;
5. EDIT alone never mutates storage;
6. same-poll EDIT+SEL never deletes;
7. only a later SEL confirm while an existing arm is valid removes exactly one selected lock;
8. EDIT release/cancel/unrelated focus intent prevents latent deletion;
9. selected-lock deletion restores exactly one free record and cannot mutate neighboring steps/locks;
10. deleting first/middle/last/only lock leaves valid sorted storage and deterministic new selection;
11. NOT_FOUND/CORRUPT/BAD_ADDRESS are safe and produce no partial mutation;
12. SELECT encoder stale/begin/release deltas cannot browse a new target accidentally;
13. all D029 target/focus/KNOB semantics remain green;
14. no clear-all user gesture exists in WP3.4;
15. no lock is applied to playback/audio;
16. all Phase 2 + WP3.1 + WP3.2 + WP3.3 regressions and frozen WAV remain green;
17. full JieLi device build/resource delta is recorded.

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
- Preserve the frozen WAV unless a deliberate later audio change records a superseding decision/test.
- No float in firmware DSP and no runtime 64-bit division.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted future core direction. Current Phase 3 locks are ordinary track/step-owned data. Do not silently force future GLOBAL/master automation into D028 ownership.

## Hardware limitation still open

No physical FM-1 is currently available. Host/device-build verification does not claim physical hold/browse/delete comfort, actual panel scan feel, display update feel, physical install/boot/audio behavior or measured AC791N execution timing.

## Exact next action

Freeze D030 in `docs/DECISIONS.md`, align `docs/CONTROL_MAP_V0.1.md`, then implement the smallest bounded WP3.4 extension. Do not begin WP3.5.

## Last relevant checkpoints

- Phase 2 verified implementation/workflow: `7063be51e53c2657178084b3bef5ace813dafcd7`
- Phase 3 branch start: `c3693d8c7bb3d5306794aa232d6d25bef50c6c99`
- WP3.1 verified: `5af8065a9c98884d22682075b600d9790cfa7940`
- WP3.2 verified: `da2f5afe9b1da6defc3857b91f52b40d7525f33b`
- WP3.3 verified: `df5d7e07fc763909a141c99283cfd2be19cff847`
- WP3.4 start head: `8ffa2a3de00f89946aca37fa7015fd3940851c93`

## Last updated

2026-10-24 — WP3.4 / issue #23 marked IN PROGRESS at `8ffa2a3d...`. D030 must be frozen before source implementation.
