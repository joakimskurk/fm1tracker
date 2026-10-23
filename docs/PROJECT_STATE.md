# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 3 — p-locks and tracker effects**

State: **IN PROGRESS — WP3.1 active — 2026-10-23**

Current branch: `phase3/plocks-effects`

Phase 3 branch start / completed Phase 2 handoff head:

`c3693d8c7bb3d5306794aa232d6d25bef50c6c99`

Completed Phase 2 verified implementation/workflow checkpoint:

`7063be51e53c2657178084b3bef5ace813dafcd7`

Durable Phase 2 closeout evidence:

`docs/PHASE2_VERIFICATION.md`

## Current milestone

**WP3.1 — stable parameter ID registry**

State: **IN PROGRESS — 2026-10-23**

GitHub issue: #19

WP3.1 start head:

`c3693d8c7bb3d5306794aa232d6d25bef50c6c99`

### Exact objective

Freeze and implement one authoritative, bounded parameter-identity registry that later p-lock storage, editing, tracker effects and engine mappings can use without inventing numeric IDs in multiple modules.

Before source implementation spreads, D027 must freeze:

- the scalar width/domain of `FmtParamId`;
- NONE/INVALID semantics;
- non-overlapping namespace/range rules;
- the small set of IDs that Phase 3 actually needs now;
- lookup/classification behavior for assigned, unassigned, reserved and invalid IDs;
- the rule that IDs are stable semantic identity, independent of UI page/encoder position and raw persistence structs.

### WP3.1 intended namespace direction

The registry is an 8-bit identity space. The current design target is:

- `0x00` — NONE, never an active lock parameter;
- `0x01..0x1F` — common voice parameters;
- `0x20..0x5F` — 8-BIT engine parameters;
- `0x60..0x9F` — 16-BIT engine parameters;
- `0xA0..0xBF` — track/mixer parameters;
- `0xC0..0xDF` — channel/send/effect parameters;
- `0xE0..0xEF` — future GLOBAL/master parameter namespace, reserved in WP3.1;
- `0xF0..0xFE` — future/reserved;
- `0xFF` — INVALID, never assignable.

Only the minimal Phase 3 IDs are to be assigned in WP3.1. Engine envelopes, oscillator-specific controls, FM operators and effect-specific parameters remain unassigned until their own packages.

### Expected WP3.1 source/test changes

Primary expected files:

- `firmware/src/fmt_param_registry.h`
- `firmware/src/fmt_param_registry.c`
- `tests/test_fmt_param_registry.c`
- `tests/run_param_registry_tests.sh`
- a dedicated WP3.1 verification workflow/evidence document
- `docs/DATA_MODEL_V0.1.md` and `docs/DECISIONS.md` where required to reflect the frozen registry contract.

No p-lock pool/storage object is created in WP3.1.

### Required verification

1. `FmtParamId` is exactly one byte on host/device builds;
2. namespace ranges are exact, non-overlapping and cover the intended 8-bit domain without collisions;
3. `NONE` and `INVALID` are never reported as assigned/lockable parameters;
4. every assigned ID is unique and belongs to its documented namespace;
5. unassigned IDs inside future namespaces remain safe/unassigned rather than acquiring accidental semantics;
6. lookup/metadata behavior is deterministic for all 256 possible byte values;
7. labels/metadata are independent of UI encoder/page positions and are not treated as persistence keys;
8. all Phase 2 regressions and the frozen default WAV remain green;
9. full JieLi device build, `FM-1_987` identity, HAL-only register access and resource checks pass;
10. device image/RAM/pool delta versus final Phase 2 is recorded;
11. no WP3.2 lock allocation/editing or later Phase 3 behavior is present.

### Strict WP3.1 non-goals

- no p-lock allocator/storage implementation — WP3.2;
- no hold-step/encoder lock editing — WP3.3;
- no lock replace/delete UX — WP3.4;
- no tracker-effect semantics/application — WP3.5+;
- no 8-BIT or FM4 engine implementation;
- no live-performance capture/automation implementation;
- no persistence/file-format implementation;
- no hardware control-map freeze;
- no global/master automation storage despite reserving an identity namespace for it.

## Final verified Phase 2 baseline

GitHub Actions closeout:

- run: `40015242947`
- job: `120625519388` (`phase2-closeout`)
- result: **SUCCESS**

| Resource | Used | Limit | Free |
|---|---:|---:|---:|
| app image | 59,252 B | 581,564 B | 522,312 B |
| normal RAM | 12,464 B | 98,304 B | 85,840 B |
| pool RAM | 96,616 B | 344,064 B | 247,448 B |

Final Phase 2 device artifacts:

- `fm1tracker.bin`: `34d0e8c4344e091704608e08357964e7ae0b18673dd0b93f36c4e5bcc2cb4c7c`
- `fm1tracker-0.1.fwsc`: `3cfd0a2da3836720d2f921b04f29e93c16ca83ab18ab7242c99f2c333d890ca7`
- package identity: `FM-1_987`
- frozen host WAV: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

Important Phase 2 contracts D018–D026 remain regression requirements.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted future core direction. WP3.1 should leave clean namespace room for later CHANNEL and GLOBAL parameter/FX capture, but must not implement live capture, automation, metronome or performance FX now.

## Hardware limitation still open

No physical FM-1 is currently available. Host/device-build verification does not claim physical install/boot/audio, panel ergonomics, display readability/smoothness or measured AC791N execution timing.

## Exact next action

Record D027 in `docs/DECISIONS.md` and align the parameter-ID section of `docs/DATA_MODEL_V0.1.md` before source code. Then implement the minimal authoritative registry and exhaustive 256-value host test. Do not begin WP3.2.

## Last relevant checkpoints

- Phase 2 verified implementation/workflow: `7063be51e53c2657178084b3bef5ace813dafcd7`
- Phase 2 final handoff / Phase 3 branch start: `c3693d8c7bb3d5306794aa232d6d25bef50c6c99`

## Last updated

2026-10-23 — Phase 3 branch created and WP3.1 / issue #19 marked IN PROGRESS. D027 must be recorded before registry source implementation.
