# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 3 — p-locks and tracker effects**

State: **IN PROGRESS — WP3.3 active — 2026-10-24**

Parent issue: #20

Current branch: `phase3/plocks-effects`

Phase 3 branch start / completed Phase 2 handoff:

`c3693d8c7bb3d5306794aa232d6d25bef50c6c99`

## Completed WP3.1

**WP3.1 — stable parameter ID registry** — COMPLETE — issue #19

Verified checkpoint:

`5af8065a9c98884d22682075b600d9790cfa7940`

Evidence: `docs/WP3_1_VERIFICATION.md`

D027 freezes one-byte semantic parameter identity and exactly four current production-lockable IDs:

- `0x01` `FMT_PARAM_VOICE_LEVEL`
- `0x02` `FMT_PARAM_PITCH_FINE`
- `0xA0` `FMT_PARAM_TRACK_LEVEL`
- `0xA1` `FMT_PARAM_TRACK_PAN`

Namespace membership alone never implies assignment.

## Completed WP3.2

**WP3.2 — sparse four-lock-per-step storage** — COMPLETE — issue #21

WP3.2 verified source/workflow checkpoint:

`da2f5afe9b1da6defc3857b91f52b40d7525f33b`

Final GitHub Actions:

- run `40038453521`
- job `120703577806` (`lock-store-closeout`)
- result **SUCCESS**

Durable evidence: `docs/WP3_2_VERIFICATION.md`

D028 freezes one global sparse 2,048-record pool with exact 8-byte owner-checked records, max four active locks per individual step, ascending parameter-ID lists, deterministic replacement/removal/clear and explicit STEP_FULL/POOL_FULL/CORRUPT handling.

Final WP3.2 footprint:

| Resource | Used | Limit | Free |
|---|---:|---:|---:|
| app image | 59,388 B | 581,564 B | 522,176 B |
| normal RAM | 12,468 B | 98,304 B | 85,836 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B |

Frozen default WAV remains:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

## Current milestone

**WP3.3 — hold-step + encoder p-lock editing**

State: **IN PROGRESS — 2026-10-24**

GitHub issue: #22

WP3.3 start head:

`361c5efa308ade56a35fd3b50e6a77c9e3ee41bb`

No WP3.3 source existed at the start checkpoint.

### Exact objective

Connect the verified D027 parameter registry and D028 sparse lock store to the first hardware-native p-lock editor gesture without yet applying locks to playback/audio and without adding WP3.4 delete/clear UX.

D029 must be written before source implementation and must freeze:

- exact step-hold/select gesture;
- SEQ/STEP REC context gating;
- exact KNOB1..4 -> D027 parameter mapping;
- first-lock initialization and relative encoder semantics;
- editor-domain value ranges/clamping for the four assigned IDs;
- whether the gesture writes storage only or also changes live/base parameters;
- target pattern/track/step ownership for the duration of the gesture;
- release/latch behavior;
- simultaneous step-key/encoder ordering;
- safe handling/feedback for D028 storage failures;
- minimal tracker-screen feedback for the active edit gesture.

### Hardware/input facts already verified at package start

The unchanged FM-1 HAL exposes seven encoders through `fm1_enc_take()`. It returns signed detent steps accumulated since the previous main-loop take; positive means clockwise. The decoder already makes one physical click equal one relative step and handles bounce/backtracking.

Current encoder indexes are:

- `ENC_SELECT = 0`
- `ENC_ALGORITHM = 1`
- `ENC_KNOB1 = 2`
- `ENC_KNOB2 = 3`
- `ENC_KNOB3 = 4`
- `ENC_KNOB4 = 5`
- `ENC_PRESETS = 6`

This means WP3.3 can use explicit relative detent deltas rather than inventing an absolute-knob pickup threshold.

Current D022 ordinary SEQ context already maps sixteen white physical note keys to the current 16-step page and the first eight black keys to D023 track selection. D024 STEP REC reinterprets all 27 note keys as pitch input, so WP3.3 must not silently steal the same keys there.

### Reviewed D029 direction before source

The intended prototype direction, to be frozen in D029 before implementation, is:

- p-lock editing exists only in ordinary D022 SEQ/pattern context, not PERFORMANCE or STEP REC;
- press and hold one white step key to arm p-lock editing for that exact global step and the selected track as they existed when the hold began;
- the held white key still selects that step on press;
- while that key remains physically held, KNOB1..4 are the only encoders that write locks;
- target pattern remains the current Phase 2/3 editor pattern 0;
- track and global step are snapshotted at gesture start so later track/page/cursor activity cannot redirect writes;
- releasing the held white key ends the lock gesture immediately;
- holding a different white step while a lock gesture is already active does not retarget the existing gesture; it may update ordinary cursor selection only after the current gesture ends, according to the frozen input order;
- STEP REC entry, HOME or any context exit cancels the lock gesture without writing anything extra;
- encoder movement writes storage only in WP3.3; it does not change proof playback, audition voices or base instrument state;
- no delete/clear gesture exists in WP3.3.

The exact parameter/value mapping still must be recorded in D029 before code.

### Required verification

1. exact gesture state is small, bounded and deterministic;
2. only ordinary SEQ can begin or retain a p-lock gesture;
3. held white key snapshots the intended pattern/track/global-step target;
4. target cannot migrate after track/page/cursor changes;
5. KNOB1..4 map only to the four authoritative D027 IDs;
6. each signed encoder detent produces deterministic bounded value movement;
7. first movement on an unlocked parameter creates exactly one lock;
8. later movement on that parameter replaces in place without consuming another record;
9. simultaneous encoder deltas are processed in fixed encoder order and remain independent;
10. zero movement creates no lock;
11. invalid context/release cancels safely;
12. D028 STEP_FULL/POOL_FULL/CORRUPT outcomes cause no unintended song mutation;
13. no lock is applied to playback/audio in this package;
14. all Phase 2 + WP3.1 + WP3.2 regressions and frozen WAV remain green;
15. full JieLi device build/resource delta is recorded;
16. no WP3.4 delete/clear or WP3.5 playback semantics exist.

### Strict non-goals

- no user-facing lock deletion/clear — WP3.4;
- no expanded replace/delete UX — WP3.4;
- no playback/audio application of p-locks — WP3.5+;
- no tracker-effect semantics;
- no new parameter IDs unless separately justified;
- no 8-BIT or FM4 engine work;
- no live-performance capture/automation;
- no persistence/file-format work;
- no final hardware control-map freeze.

## Important decisions / do not regress

- D005: 8 logical tracks × 64 steps × 16 patterns.
- D010: p-locks are first-class bounded sequencer data.
- D018: `FmtStep` remains exactly 8 B.
- D019: one pool-backed runtime `FmtSong`.
- D020–D026: completed Phase 2 timing/editor/display semantics remain regression requirements.
- D027: stable one-byte parameter identity.
- D028: 2,048-record owner-checked global sparse lock pool, max four per step.
- Preserve the frozen WAV unless a deliberate later audio change records a superseding decision/test.
- No float in firmware DSP and no runtime 64-bit division.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted future core direction. WP3.3 edits track/step-owned p-lock data only. It must not define future GLOBAL/master automation ownership.

## Hardware limitation still open

No physical FM-1 is currently available. Host/device-build verification does not claim physical encoder/white-key comfort, display readability/smoothness, install/boot/audio behavior or measured AC791N execution timing.

## Exact next action

Freeze D029 in `DECISIONS.md`, align the provisional parameter-lock section in `CONTROL_MAP_V0.1.md`, and only then create the WP3.3 editor/input source. Do not begin WP3.4.

## Last relevant checkpoints

- Phase 2 verified implementation/workflow: `7063be51e53c2657178084b3bef5ace813dafcd7`
- Phase 3 branch start: `c3693d8c7bb3d5306794aa232d6d25bef50c6c99`
- WP3.1 verified: `5af8065a9c98884d22682075b600d9790cfa7940`
- WP3.2 verified: `da2f5afe9b1da6defc3857b91f52b40d7525f33b`
- WP3.3 start head: `361c5efa308ade56a35fd3b50e6a77c9e3ee41bb`

## Last updated

2026-10-24 — WP3.3 / issue #22 marked IN PROGRESS at `361c5efa...`. D029 must be frozen before editor source implementation.
