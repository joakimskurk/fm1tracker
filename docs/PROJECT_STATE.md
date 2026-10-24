# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 3 — p-locks and tracker effects**

State: **IN PROGRESS — WP3.3 complete; WP3.4 ready to start — 2026-10-24**

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

WP3.3 start head:

`361c5efa308ade56a35fd3b50e6a77c9e3ee41bb`

Final verified source/workflow checkpoint:

`df5d7e07fc763909a141c99283cfd2be19cff847`

Final GitHub Actions:

- run `40054322180`
- job `120753812201` (`lock-editor-closeout`)
- result **SUCCESS**

Durable evidence: `docs/WP3_3_VERIFICATION.md`

### D029 editor contract

In ordinary SEQ only, hold one white step key to snapshot pattern 0, selected track and global step. While that exact key remains held, KNOB1..4 write or replace locks on the fixed target. Other step keys, black track selectors and OCT page navigation cannot redirect the target. HOME or STEP REC entry cancels; PLAY may toggle while the hold remains active.

KNOB1..4 are drained every poll. Inactive, hold-start and release/cancel-poll deltas are discarded so stale encoder motion cannot leak into a new target.

Current mapping and editor references:

| Knob | Parameter | Reference |
|---|---|---:|
| KNOB1 | VOICE LEVEL | 255 |
| KNOB2 | FINE PITCH | 128 |
| KNOB3 | TRACK LEVEL | 255 |
| KNOB4 | TRACK PAN | 128 |

The editor domain is normalized `0..255`, one detent per unit with saturation. These values do not yet define dB, cents, pan law or engine coefficients.

`FmtLockEditor` is exactly 8 B ordinary RAM and allocates no pool storage.

Frozen unit marker:

```text
FMT_LOCK_EDITOR_TESTS_OK=1
state_bytes=8 pattern=0 knobs=4 value_domain=0_255 references=255,128,255,128
targets=64x8 snapshot=1 replace_in_place=1 pool_full=1 corrupt=1 range_reject=1
```

Full-app marker:

```text
WP3_3_LOCK_EDITOR_CONTEXT_HOST_OK=1 midi_events=0 stale_begin_release_deltas=fixture focus_suppression=fixture
```

Reviewed raw framebuffer hashes:

- 1000 ms: `224c0c05bdc29360e8829fd6f69ea21b68d38dc3a369e33d1373e461ea678fd0`
- 3000 ms: `95bbee218132222d18d4593d9e5427cc25c0efcd340ec768a3c9df2ec9751212`

The reviewed screens keep `CUR19 / T2 / PG2` fixed through suppressed focus-stealing attempts, first showing `K1 VOICE 254 LOCK`, then an in-place replacement to `K1 VOICE 255 LOCK`.

### Final WP3.3 resources

| Resource | Used | Limit | Free | Delta vs WP3.2 |
|---|---:|---:|---:|---:|
| app image | 61,716 B | 581,564 B | 519,848 B | +2,328 B |
| normal RAM | 12,476 B | 98,304 B | 85,828 B | +8 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B | +0 B |

Final linked symbols:

```text
01c0b078 .bss  00000008 fmt_lock_editor_runtime
01c0b088 .bss  00000004 fmt_lock_store_runtime
01c20000 .pool 00010168 fmt_store_storage
01c30168 .pool 00004000 fmt_lock_store_storage
```

Final artifacts:

- `fm1tracker.bin`: `ee3daf66216e1f8ba4a04183bc36aca13236b693acd548a1dd8de90f48800d40`
- `fm1tracker-0.1.fwsc`: `422e8283025419ec1991f0e13d52321e7dc837fab8218c80e969c69101e97922`
- package identity: `FM-1_987`
- frozen host WAV: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

Final evidence artifact:

- ID `12544667629`
- ZIP SHA-256 `06e00463f14d4d09cea2573cea55b37ed7ba6eee74eb64209f4c4b1efeb20516`

## Next milestone

**WP3.4 — p-lock replace/delete UX**

State: **READY TO START — no WP3.4 implementation has begun**

GitHub issue: #23

Before source, WP3.4 must freeze a new decision covering lock selection/browse, delete-one versus clear-all, destructive-action safeguards, interaction with D029 hold, context/target ownership and minimal feedback. Full row lock visualization remains WP3.9.

WP3.4 may use the verified D028 remove/clear primitives but must not apply locks to playback/audio. Playback application remains WP3.5+.

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

No physical FM-1 is currently available. Host/device-build verification does not claim physical encoder/white-key comfort, actual panel scan feel, display update feel, physical install/boot/audio behavior or measured AC791N execution timing.

## Exact next action

Start WP3.4 / issue #23 as its own package. Verify actual branch HEAD, mark WP3.4 IN PROGRESS with that start head, freeze D030 for delete/clear/browse UX before source, and do not begin WP3.5.

## Last relevant checkpoints

- Phase 2 verified implementation/workflow: `7063be51e53c2657178084b3bef5ace813dafcd7`
- Phase 3 branch start: `c3693d8c7bb3d5306794aa232d6d25bef50c6c99`
- WP3.1 verified: `5af8065a9c98884d22682075b600d9790cfa7940`
- WP3.2 verified: `da2f5afe9b1da6defc3857b91f52b40d7525f33b`
- WP3.3 verified: `df5d7e07fc763909a141c99283cfd2be19cff847`
- WP3.3 final run: `40054322180`

## Last updated

2026-10-24 — WP3.3 verified complete. WP3.4 / #23 is READY TO START; no WP3.4 source exists.
