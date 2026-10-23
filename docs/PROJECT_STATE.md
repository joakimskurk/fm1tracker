# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 3 — p-locks and tracker effects**

State: **IN PROGRESS — WP3.2 active — 2026-10-24**

Phase 3 parent issue: #20

Current branch: `phase3/plocks-effects`

Phase 3 branch start / completed Phase 2 handoff head:

`c3693d8c7bb3d5306794aa232d6d25bef50c6c99`

## Completed WP3.1 checkpoint

**WP3.1 — stable parameter ID registry** — COMPLETE — issue #19

Final verified source/workflow checkpoint:

`5af8065a9c98884d22682075b600d9790cfa7940`

Full verification:

- Actions run `40025790828`
- job `120661526848` (`param-registry-closeout`)
- result **SUCCESS**
- evidence `docs/WP3_1_VERIFICATION.md`

D027 freezes `FmtParamId` as one byte with `0x00` NONE, `0xFF` INVALID and four currently assigned/lockable IDs: `VOICE_LEVEL`, `PITCH_FINE`, `TRACK_LEVEL`, `TRACK_PAN`. Namespace membership alone never implies assignment.

WP3.1 runtime remained byte-identical to Phase 2.

## Current milestone

**WP3.2 — sparse four-lock-per-step storage**

State: **IN PROGRESS — 2026-10-24**

GitHub issue: #21

WP3.2 start head:

`dc0f4280f27e8a1a1a4608faef8d59cfceb68a1e`

### Exact objective

Freeze and implement the first concrete bounded p-lock storage layer while preserving the 8-byte `FmtStep`, consuming D027 semantic parameter IDs without renumbering them, and enforcing at most four active locks on any one step.

Before source code, D028 must freeze the concrete lock record, global capacity, ownership, free-list/allocation rules, deterministic storage primitives, corruption behavior and reset invariants.

### Important capacity clarification

`FMT_LOCK_POOL_MAX = 2048` is a **global sparse song pool**, not enough to place four locks on every one of the 8,192 resident song steps simultaneously.

The old `DATA_MODEL_V0.1.md` example that described 2,048 records as the worst case for every step omitted the 16 pattern slots from its arithmetic. WP3.2 must correct that wording rather than silently treating it as a larger allocation requirement.

The intended V0.1 rule remains:

- any individual step may own at most four locks;
- the complete song shares a fixed global pool of 2,048 lock records;
- pool exhaustion is an explicit deterministic storage result;
- capacity may only change later through a documented budget/architecture decision.

### D028 design target to freeze before source

The reviewed implementation target is:

- `FmtLock` is exactly 8 B;
- 2,048 records = exactly 16,384 B pool RAM;
- each active record contains a 16-bit next index, 16-bit value, 16-bit linear step owner, one-byte D027 parameter ID and one-byte runtime state;
- owner IDs cover all 8,192 song steps; `0xFFFF` means no owner;
- `FmtStep.lock_head` remains the 16-bit first-record index and `0xFFFF` means no list;
- the same 16-bit `next` field is reused by the free list while a record is free;
- allocator runtime state is only a 16-bit free head plus 16-bit used count in ordinary RAM;
- reset builds the free list in ascending record-index order;
- active step lists are strictly sorted by ascending `FmtParamId`, not insertion order;
- setting an existing parameter replaces its value in place without allocation;
- adding a new parameter rejects a fifth lock before allocation;
- invalid/unassigned D027 IDs are rejected;
- pool-full is distinct from step-full;
- remove/clear return records to the free list exactly once;
- all mutating operations validate the complete bounded step chain before changing state;
- out-of-range indexes, wrong owner, non-active records, unsorted/duplicate IDs, invalid parameters or chains longer than four are corruption and cause no partial mutation.

D028 is not accepted until written to `DECISIONS.md` and `DATA_MODEL_V0.1.md` is aligned.

### Expected WP3.2 source/test changes

Primary expected files:

- `firmware/src/fmt_lock_store.h`
- `firmware/src/fmt_lock_store.c`
- `firmware/src/fmt_store.c` / reset integration
- `firmware/src/dinghy.c` only to link/reset the bounded storage layer, not to expose editing behavior
- `tests/test_fmt_lock_store.c`
- focused max-four harness if needed to test a fifth prevalidated ID without assigning a fifth production D027 ID
- `tests/run_lock_store_tests.sh`
- WP3.2 CI/evidence files
- `docs/DECISIONS.md`
- `docs/DATA_MODEL_V0.1.md`

### Required verification

1. exact `FmtLock` size/layout and 2,048-record capacity;
2. exact pool placement and 16,384 B record-storage cost;
3. all 8,192 possible step owners map uniquely and safely;
4. empty/reset state has every record on the free list exactly once;
5. add/get/replace/remove/clear are deterministic and bounded;
6. duplicate parameter set replaces in place and does not consume capacity;
7. max four locks per step is mechanically enforced;
8. actual D027 unassigned/invalid IDs are rejected;
9. global pool exhaustion and one-record recovery/reuse are exact;
10. invalid/cross-owned/corrupt indexes fail without neighbor/pool mutation;
11. normal song fields and neighboring steps remain byte-identical across lock edits except intended `lock_head` changes;
12. all Phase 2 + WP3.1 regressions and frozen WAV remain green;
13. full JieLi device build, fixed identity and safety checks pass;
14. exact image/RAM/pool delta is recorded;
15. no WP3.3 editor gesture or WP3.5 playback/effect application exists.

### Strict non-goals

- no encoder/hold-step lock editing — WP3.3;
- no user-facing replace/delete UX — WP3.4;
- no playback/audio application of locks — WP3.5+;
- no tracker-effect implementation;
- no 8-BIT or FM4 engine implementation;
- no live-performance capture/automation implementation;
- no persistence/file-format implementation;
- no hardware control-map freeze.

## Current verified runtime baseline before WP3.2

| Resource | Used | Limit | Free |
|---|---:|---:|---:|
| app image | 59,252 B | 581,564 B | 522,312 B |
| normal RAM | 12,464 B | 98,304 B | 85,840 B |
| pool RAM | 96,616 B | 344,064 B | 247,448 B |

Frozen artifacts/regressions before WP3.2:

- `fm1tracker.bin`: `34d0e8c4344e091704608e08357964e7ae0b18673dd0b93f36c4e5bcc2cb4c7c`
- `fm1tracker-0.1.fwsc`: `3cfd0a2da3836720d2f921b04f29e93c16ca83ab18ab7242c99f2c333d890ca7`
- package identity: `FM-1_987`
- frozen host WAV: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

If the 8-byte / 2,048-record D028 target is accepted unchanged, expected storage-only resource direction before measured code cost is:

- lock records: +16,384 B pool;
- allocator runtime state: +4 B ordinary RAM;
- expected total pool use before linker/code measurement: 113,000 B;
- expected remaining pool: 231,064 B;
- editable `FmtSong` + lock-record budget: 82,280 B, still inside the frozen 96 KiB editable-song target.

## Important decisions / do not regress

- D005: 8 logical tracks × 64 steps × 16 patterns.
- D010: p-locks are first-class bounded sequencer data.
- D018: frozen 8-byte `FmtStep`; p-lock allocator/layout explicitly deferred to Phase 3.
- D019: one pool-backed runtime `FmtSong`.
- D020–D026: completed Phase 2 transport/editor/display semantics remain regression requirements.
- D027: stable one-byte semantic parameter identity and namespaces.
- Preserve the frozen host WAV unless a later intentional audio change records a superseding decision/test.
- No floating point in firmware DSP and no runtime 64-bit division.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted future core direction. WP3.2 stores ordinary per-step track-owned locks only. It must not silently define GLOBAL/master automation ownership or live-capture storage.

## Hardware limitation still open

No physical FM-1 is currently available. Host/device-build verification does not claim physical install/boot/audio, panel ergonomics, display readability/smoothness or measured AC791N execution timing.

## Exact next action

Record D028 and correct the lock-capacity arithmetic/wording in `DATA_MODEL_V0.1.md`. Only after that checkpoint may `fmt_lock_store` source be created. Do not begin WP3.3.

## Last relevant checkpoints

- Phase 2 verified implementation/workflow: `7063be51e53c2657178084b3bef5ace813dafcd7`
- Phase 3 branch start: `c3693d8c7bb3d5306794aa232d6d25bef50c6c99`
- WP3.1 verified source/workflow: `5af8065a9c98884d22682075b600d9790cfa7940`
- WP3.2 start head: `dc0f4280f27e8a1a1a4608faef8d59cfceb68a1e`

## Last updated

2026-10-24 — WP3.2 / issue #21 marked IN PROGRESS at `dc0f4280...`. D028 must be frozen before lock-storage source implementation.
