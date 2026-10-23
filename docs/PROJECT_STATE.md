# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 3 — p-locks and tracker effects**

State: **IN PROGRESS — WP3.2 complete; WP3.3 ready to start — 2026-10-24**

Parent issue: #20

Current branch: `phase3/plocks-effects`

Phase 3 branch start / completed Phase 2 handoff:

`c3693d8c7bb3d5306794aa232d6d25bef50c6c99`

## Completed WP3.1

**WP3.1 — stable parameter ID registry** — COMPLETE — issue #19

Verified checkpoint:

`5af8065a9c98884d22682075b600d9790cfa7940`

Evidence:

`docs/WP3_1_VERIFICATION.md`

D027 freezes one-byte semantic parameter identity, namespace ranges and exactly four initial production-lockable IDs:

- `0x01` `FMT_PARAM_VOICE_LEVEL`
- `0x02` `FMT_PARAM_PITCH_FINE`
- `0xA0` `FMT_PARAM_TRACK_LEVEL`
- `0xA1` `FMT_PARAM_TRACK_PAN`

Namespace membership alone never implies assignment.

## Completed WP3.2

**WP3.2 — sparse four-lock-per-step storage** — COMPLETE — issue #21

WP3.2 start head:

`dc0f4280f27e8a1a1a4608faef8d59cfceb68a1e`

Final verified source/workflow checkpoint:

`da2f5afe9b1da6defc3857b91f52b40d7525f33b`

Final GitHub Actions:

- run `40038453521`
- job `120703577806` (`lock-store-closeout`)
- result **SUCCESS**

Durable evidence:

`docs/WP3_2_VERIFICATION.md`

### D028 frozen storage contract

The full editable song shares one global sparse pool of 2,048 records. This is a deliberate global capacity, not four locks on all 8,192 resident steps simultaneously.

`FmtLock` is exactly 8 B:

```text
uint16 next       offset 0
uint16 value      offset 2
uint16 owner      offset 4
uint8  param_id   offset 6
uint8  state      offset 7
```

The pool is exactly:

```text
2048 * 8 B = 16,384 B
```

Allocator runtime state is exactly 4 B: a 16-bit free head plus 16-bit used count.

Any individual step may own at most four active locks. Active step lists are strictly sorted by ascending D027 parameter ID. Setting an existing parameter replaces in place and consumes no record. A fifth prevalidated unique lock returns `STEP_FULL`; global exhaustion returns `POOL_FULL`.

Each active record carries a linear step owner `0..8191`, so an in-range index pointing to another step is rejected as corruption. Mutating operations validate the bounded list before changes; malformed/cross-owned/unsorted/overlong chains fail without partial mutation.

The earlier planning claim that 2,048 records covered four locks on every song step was corrected: 16 patterns × 8 tracks × 64 steps × 4 would require 32,768 records. V0.1 intentionally remains a sparse global pool.

### Final WP3.2 linked resources

```text
01c20000 .pool 00010168 fmt_store_storage
01c30168 .pool 00004000 fmt_lock_store_storage
01c0b080 .bss  00000004 fmt_lock_store_runtime
```

Final footprint:

| Resource | Used | Limit | Free | Delta vs WP3.1 |
|---|---:|---:|---:|---:|
| app image | 59,388 B | 581,564 B | 522,176 B | +136 B |
| normal RAM | 12,468 B | 98,304 B | 85,836 B | +4 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B | +16,384 B |

Editable core plus lock records:

```text
FmtSong core              65,896 B
lock records              16,384 B
---------------------------------
editable core + locks     82,280 B
```

This remains 16,024 B below the frozen 96 KiB editable-song target.

Final artifacts:

- `fm1tracker.bin`: `b717a504f96601393e493f52142e3120204051c5e11b733b0113309e0219ac2b`
- `fm1tracker-0.1.fwsc`: `36c054c0f6c0cc8e86dd869fadf8d0093a560f5851a9ebd9b2405552642d5966`
- identity `FM-1_987`
- frozen WAV remains `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

Final evidence artifact:

- ID `12538271532`
- ZIP SHA-256 `ecb0985b24201e3506d1512ead85e406e4226bebc7915d945cd5812e5dbd1830`

## Next milestone

**WP3.3 — hold-step + encoder p-lock editing**

State: **READY TO START — no WP3.3 implementation has begun**

GitHub issue: #22

Before source code, WP3.3 must record the exact editor/gesture contract. It must freeze at least:

- step-hold/select gesture;
- SEQ/STEP REC context gating;
- encoder-to-D027 parameter mapping;
- deterministic editor value scaling;
- target step/track ownership during a gesture;
- release/latch behavior;
- storage-error handling;
- minimal editing feedback.

WP3.3 is the first package allowed to call D028 `fmt_lock_set` from the hardware/editor path. It must not add lock delete/clear UX from WP3.4 or apply locks to playback/audio from WP3.5+.

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

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted future core direction. Current Phase 3 storage/editor work is track/step-owned. Do not silently force future GLOBAL/master automation into D028 track lock ownership.

## Hardware limitation still open

No physical FM-1 is currently available. Host/device-build verification does not claim physical install/boot/audio, panel ergonomics, display readability/smoothness or measured AC791N execution timing.

## Exact next action

Start WP3.3 / issue #22 as its own package. Verify actual branch HEAD, mark WP3.3 IN PROGRESS with that start head, then freeze the editor gesture/value contract before source. Do not begin WP3.4.

## Last relevant checkpoints

- Phase 2 verified implementation/workflow: `7063be51e53c2657178084b3bef5ace813dafcd7`
- Phase 3 branch start: `c3693d8c7bb3d5306794aa232d6d25bef50c6c99`
- WP3.1 verified: `5af8065a9c98884d22682075b600d9790cfa7940`
- WP3.2 verified: `da2f5afe9b1da6defc3857b91f52b40d7525f33b`
- WP3.2 final run: `40038453521`

## Last updated

2026-10-24 — WP3.2 verified complete. WP3.3 / issue #22 is READY TO START; no WP3.3 source exists.
