# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 3 — p-locks and tracker effects**

State: **IN PROGRESS — WP3.1 complete; WP3.2 ready to start — 2026-10-23**

Phase 3 parent issue: #20

Current branch: `phase3/plocks-effects`

Phase 3 branch start / completed Phase 2 handoff head:

`c3693d8c7bb3d5306794aa232d6d25bef50c6c99`

## Completed WP3.1 checkpoint

**WP3.1 — stable parameter ID registry**

State: **COMPLETE — 2026-10-23**

GitHub issue: #19

WP3.1 start head:

`c3693d8c7bb3d5306794aa232d6d25bef50c6c99`

Final verified source/workflow checkpoint:

`5af8065a9c98884d22682075b600d9790cfa7940`

GitHub Actions:

- focused green run: `40025671629`
- full closeout run: `40025790828`
- full job: `120661526848` (`param-registry-closeout`)
- result: **SUCCESS**

Durable evidence:

`docs/WP3_1_VERIFICATION.md`

### D027 frozen parameter identity

`FmtParamId` is exactly one byte.

- `0x00` = NONE, never assigned/lockable;
- `0x01..0x1F` = common voice namespace;
- `0x20..0x5F` = 8-BIT engine namespace;
- `0x60..0x9F` = 16-BIT engine namespace;
- `0xA0..0xBF` = track/mixer namespace;
- `0xC0..0xDF` = channel/send/effect namespace;
- `0xE0..0xEF` = future GLOBAL/master namespace, reserved/unassigned;
- `0xF0..0xFE` = future/reserved;
- `0xFF` = INVALID, never assignable.

Range membership does not imply assignment. Only authoritative registry entries are lockable.

WP3.1 assigns exactly four IDs:

- `0x01` `FMT_PARAM_VOICE_LEVEL`;
- `0x02` `FMT_PARAM_PITCH_FINE`;
- `0xA0` `FMT_PARAM_TRACK_LEVEL`;
- `0xA1` `FMT_PARAM_TRACK_PAN`.

`FmtStep.effect` remains a separate tracker-command identity domain.

### Verification result

The registry test exhaustively classifies all 256 possible byte values:

```text
FMT_PARAM_REGISTRY_TESTS_OK=1
id_bytes=1 assigned=4 exhaustive_values=256 none=0x00 invalid=0xFF global_reserved=0xE0_0xEF
```

The new registry source also compiles with the real pinned JieLi `pi32v2` target compiler.

WP3.1 intentionally does **not** link the registry into `dinghy.c`; WP3.2 is the first package that will consume parameter IDs in runtime p-lock data. Therefore the ordinary FM1Tracker device runtime remains byte-identical to Phase 2.

## Current verified runtime baseline after WP3.1

| Resource | Used | Limit | Free | Delta vs Phase 2 |
|---|---:|---:|---:|---:|
| app image | 59,252 B | 581,564 B | 522,312 B | 0 B |
| normal RAM | 12,464 B | 98,304 B | 85,840 B | 0 B |
| pool RAM | 96,616 B | 344,064 B | 247,448 B | 0 B |

Final device artifacts remain exactly Phase 2:

- `fm1tracker.bin`: `34d0e8c4344e091704608e08357964e7ae0b18673dd0b93f36c4e5bcc2cb4c7c`
- `fm1tracker-0.1.fwsc`: `3cfd0a2da3836720d2f921b04f29e93c16ca83ab18ab7242c99f2c333d890ca7`
- package identity: `FM-1_987`
- frozen host WAV: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

Evidence artifact:

- ID `12533180473`
- ZIP SHA-256 `fa28342bf1fcdf06fb168c8c19b5459afdc6e515bdced044c1f9761efecaee4b`

## Next milestone

**WP3.2 — sparse four-lock-per-step storage**

State: **READY TO START — no WP3.2 implementation has begun**

GitHub issue: #21

Before source code, WP3.2 must freeze D028 covering:

- concrete `FmtLock` record size/layout;
- exact capacity and pool-RAM cost;
- `FmtStep.lock_head` ownership and sentinel rules;
- allocator/free-list representation;
- deterministic add/replace/remove/clear storage primitives;
- duplicate-parameter policy;
- exact max-four-per-step enforcement;
- full-pool/invalid/corrupt-index behavior;
- reset/recovery invariants.

The old illustrative 4-byte lock record in `DATA_MODEL_V0.1.md` is not an ABI. D018 explicitly deferred the final record/allocator choice to Phase 3.

WP3.2 must not pull in WP3.3 encoder/edit gestures, WP3.4 user-facing lock UX or WP3.5+ playback/effect semantics.

## Important decisions / do not regress

- D005: 8 logical tracks × 64 steps × 16 patterns.
- D010: p-locks are first-class bounded sequencer data.
- D018: frozen 8-byte `FmtStep`; final lock allocator/layout deferred until Phase 3.
- D019: one pool-backed runtime `FmtSong`.
- D020–D026: completed Phase 2 transport/editor/display semantics remain regression requirements.
- D027: stable one-byte semantic parameter identity and namespaces.
- Preserve the frozen host WAV unless a later intentional audio change records a superseding decision/test.
- No floating point in firmware DSP and no runtime 64-bit division.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted future core direction. D027 reserves clean identity space for later CHANNEL/GLOBAL parameter/FX capture, but Phase 3 storage/editing must not silently implement or constrain the future live-capture workflow beyond documented requirements.

## Hardware limitation still open

No physical FM-1 is currently available. Host/device-build verification does not claim physical install/boot/audio, panel ergonomics, display readability/smoothness or measured AC791N execution timing.

## Exact next action

Start WP3.2 / issue #21 as its own package. Verify actual branch HEAD, mark WP3.2 IN PROGRESS with that start head, then freeze D028 before writing lock-storage source. Do not begin WP3.3.

## Last relevant checkpoints

- Phase 2 verified implementation/workflow: `7063be51e53c2657178084b3bef5ace813dafcd7`
- Phase 3 branch start: `c3693d8c7bb3d5306794aa232d6d25bef50c6c99`
- WP3.1 verified source/workflow: `5af8065a9c98884d22682075b600d9790cfa7940`
- WP3.1 final Actions run: `40025790828`

## Last updated

2026-10-23 — WP3.1 verified complete. WP3.2 / issue #21 is READY TO START; no WP3.2 source code exists.
