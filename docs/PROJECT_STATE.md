# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 3 — p-locks and tracker effects**

State: **IN PROGRESS — WP3.4 complete; WP3.5 ready to start — 2026-10-24**

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

Verified source/workflow checkpoint:

`df5d7e07fc763909a141c99283cfd2be19cff847`

Final run `40054322180`, job `120753812201` — **SUCCESS**.  
Evidence: `docs/WP3_3_VERIFICATION.md`

D029 freezes ordinary-SEQ held-white-step target ownership, stale/begin/release KNOB1..4 delta discard, the four current D027 mappings and normalized `0..255` storage-only lock editing.

## Completed WP3.4

**WP3.4 — p-lock browse / single-lock delete UX** — COMPLETE — issue #23

WP3.4 start head:

`8ffa2a3de00f89946aca37fa7015fd3940851c93`

Final verified source/workflow checkpoint:

`178ba57a19adc8e25106f1c63e0df2f16478fb33`

Final GitHub Actions:

- run `40064122343`
- job `120783658593` (`lock-delete-closeout`)
- result **SUCCESS**

Durable evidence: `docs/WP3_4_VERIFICATION.md`

### D030 behavior

D030 preserves the D029 held-white-step target and adds bounded browse/delete behavior:

- SELECT encoder browses existing locks in D028 ascending semantic parameter-ID order;
- browse clamps at first/last and never wraps;
- successful KNOB edit selects that parameter;
- EDIT arms one selected lock for deletion but does not mutate storage;
- only a later-poll SEL press confirms;
- same-poll EDIT+SEL never deletes;
- EDIT release, target/context exit, PLAY or unrelated focus/navigation intent cancels a pending arm;
- SELECT/K1..4 deltas are drained/discarded while armed;
- confirmation revalidates and calls D028 remove exactly once;
- exactly one record returns to the free list;
- deterministic post-delete selection moves to old ordinal, new last or NONE;
- zero-lock / NOT_FOUND / BAD_ADDRESS / CORRUPT are safe and non-mutating;
- current-ID RANGE values may be browsed and deleted without reinterpretation;
- no user-facing clear-all gesture exists in WP3.4;
- feedback stays in the tracker footer;
- locks remain storage-only and are not consumed by playback/audio.

`FmtLockEditor` is now exactly **10 B** ordinary RAM: the two new bytes are semantic selection and delete-arm state.

Reviewed WP3.4 raw framebuffers:

- armed 1000 ms: `6ce56a09073b4d70ec5ff7001ae787966f17d6bf610f1db2e239df152e14205d`
- deleted 3000 ms: `f0185637136d5b609ebae1adbfbba7a62989a9246cb63093e6263242040e6cc6`

Full-app marker:

```text
WP3_4_LOCK_DELETE_CONTEXT_HOST_OK=1 midi_events=0 same_poll_confirm=fixture play_cancel=fixture unrelated_key_cancel=fixture valid_delete=fixture
```

### Final WP3.4 resources

| Resource | Used | Limit | Free | Delta vs WP3.3 |
|---|---:|---:|---:|---:|
| app image | 64,228 B | 581,564 B | 517,336 B | +2,512 B |
| normal RAM | 12,480 B | 98,304 B | 85,824 B | +4 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B | +0 B |

Final linked symbols:

```text
01c0b078 .bss  0000000a fmt_lock_editor_runtime
01c0b08c .bss  00000004 fmt_lock_store_runtime
01c20000 .pool 00010168 fmt_store_storage
01c30168 .pool 00004000 fmt_lock_store_storage
```

Final artifacts:

- `fm1tracker.bin`: `0f828e989888158b09cdb137f7e429f97c42f4c3e30d8d814abd349d9971ed42`
- `fm1tracker-0.1.fwsc`: `de23d8ff997d60178b3b7b9e1490c90696b5c22f3660d187edaa40cf08598a57`
- package identity: `FM-1_987`
- evidence artifact ID `12548328977`
- evidence ZIP SHA-256 `e2da7f2e514cd107765cf8835f883e46bc32eef6e2eb9bc3cf8de721aac104d2`

Frozen pre-p-lock-playback WAV remains byte-identical:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

## Next milestone

**WP3.5 — basic pitch/volume p-lock playback**

State: **READY TO START — issue #24 — no WP3.5 production implementation has begun**

Before source, D031 must freeze at least:

- which current D027 IDs are applied in WP3.5 and which remain storage-only;
- exact normalized-value -> playback/DSP interpretation;
- VOICE LEVEL / TRACK LEVEL composition if both are accepted;
- FINE PITCH center/range and integer/fixed-point mapping;
- whether TRACK PAN belongs now or later;
- row-boundary application timing under D020/D021;
- unlocked-row/base-value restore or persistence semantics;
- NOTE/TIE/OFF/EMPTY interaction;
- out-of-domain lock behavior;
- bounded sequencer/audio ownership/handoff model;
- deterministic audio evidence and whether the old frozen WAV is preserved or explicitly superseded.

Do not write WP3.5 audio/playback source before D031 exists.

## Important decisions / do not regress

- D005: 8 logical tracks × 64 steps × 16 patterns.
- D010: p-locks are first-class bounded sequencer data.
- D018: `FmtStep` remains exactly 8 B.
- D019: one pool-backed runtime `FmtSong`.
- D020-D026: completed Phase 2 timing/editor/display semantics remain regression requirements.
- D027: stable one-byte semantic parameter identity.
- D028: 2,048-record owner-checked global sparse lock pool, max four per step.
- D029: held-white-step snapshot + relative KNOB1..4 storage editing.
- D030: SELECT browse + armed EDIT -> later SEL one-lock deletion; no clear-all panel UX.
- Preserve the frozen pre-playback-lock WAV unless D031 explicitly supersedes the affected audio regression with deterministic replacement evidence.
- No float in firmware DSP and no runtime 64-bit division.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted future core direction. Current Phase 3 locks are ordinary track/step-owned data. Do not silently force future GLOBAL/master automation into D028 ownership.

## Hardware limitation still open

No physical FM-1 is currently available. Host/device-build verification does not claim physical p-lock hold/browse/delete comfort, actual panel scan feel, display update feel, physical install/boot/audio behavior or measured AC791N execution timing.

## Exact next action

Start WP3.5 / issue #24 as its own package from the actual current branch HEAD, mark it IN PROGRESS, inspect the existing playback/audio ownership path, freeze D031 before source, then implement only the smallest bounded basic p-lock playback slice. Do not begin WP3.6.

## Last relevant checkpoints

- Phase 2 verified implementation/workflow: `7063be51e53c2657178084b3bef5ace813dafcd7`
- Phase 3 branch start: `c3693d8c7bb3d5306794aa232d6d25bef50c6c99`
- WP3.1 verified: `5af8065a9c98884d22682075b600d9790cfa7940`
- WP3.2 verified: `da2f5afe9b1da6defc3857b91f52b40d7525f33b`
- WP3.3 verified: `df5d7e07fc763909a141c99283cfd2be19cff847`
- WP3.4 verified: `178ba57a19adc8e25106f1c63e0df2f16478fb33`
- WP3.4 final run: `40064122343`

## Last updated

2026-10-24 — WP3.4 verified complete. WP3.5 / issue #24 is READY TO START; no WP3.5 source exists.
