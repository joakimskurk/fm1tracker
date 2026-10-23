# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and relevant source/history before changing code.

## Current phase

**Phase 2 - tracker core without synth ambition**

State: **IN PROGRESS**

Phase 1 is complete. Phase 2 implementation starts with WP2.1 only.

Phase 2 parent issue: #8

Current Phase 2 branch:

`phase2/tracker-core`

Branch start / completed Phase 1 head:

`3ed4b65d7cda15c5831533f153f91e9014a9fde7`

## Current milestone

**WP2.1 - compile-time tracker data model**

State: **IN PROGRESS**

GitHub issue: #9

Exact objective:

Implement only the already-approved V0.1 compile-time tracker data model and its bounds/size tests. The baseline tone, UI, transport and platform behavior must remain unchanged.

Expected implementation files:

- a new FM1Tracker-owned data-model header/source under `firmware/src/`;
- a focused host/unit test under `tests/` plus a small runner if needed;
- Phase 2 CI/build verification wiring;
- project/test documentation only as evidence/status changes require.

Required verification:

1. host compile/unit tests for constants, structure sizes, defaults and access bounds;
2. existing renamed host baseline still produces the Phase 1 WAV hash;
3. device firmware/package build and inherited Dinghy safety checks still pass;
4. package identity remains `FM-1_987`;
5. image/RAM/pool deltas are measured against the Phase 1 baseline;
6. protected platform/HAL/loader/update code remains unchanged.

## Completed Phase 1 baseline

Pinned Dinghy upstream:

`d44df7db09b98ccad2f1233b8f83f5f09c04829e`

FM1Tracker identity:

- visible app name: `FM1TRACKER`;
- fixed package identity: `FM-1_987`;
- host artifacts: `build/host/fm1tracker.wav` and `fm1tracker-*.png`;
- device artifacts: `build/fm1tracker.bin` and versioned `fm1tracker-*.fwsc`.

Protected Dinghy app/platform/HAL/loader/build-driver implementation remained unchanged through Phase 1. The FM1Tracker identity is supplied by the thin wrapper/build layer introduced in WP1.4.

No tracker engine, transport, STEP REC implementation, p-lock editing, song mode, 8-BIT engine, 16-BIT FM4 engine or Furnace source existed at the Phase 1 closeout.

## Phase 1 verification baseline

Final-current-workflow verification:

- GitHub Actions run: `39970562995`;
- job: `120471807329`;
- verified code/workflow commit: `14e57e071daa40ca09576c54720b384b6ada609b`;
- result: **SUCCESS**.

External SDK build input:

- official repository: `https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK.git`;
- tag: `AC79NN_SDK_V1.2.1_2023-12-13`;
- resolved commit: `d179b4484759423312073f5fbb232501aa491047`;
- consumed SDK package inputs matched the frozen SHA-256 hashes in `THIRD_PARTY.md` / `WP1_5_VERIFICATION.md`;
- SDK remains external and is not vendored into FM1Tracker.

Verified clean renamed device baseline:

| Resource | Used | Limit | Free |
|---|---:|---:|---:|
| app image | 55,088 B | 581,564 B | 526,476 B |
| normal RAM (`.data + .bss`) | 12,368 B | 98,304 B | 85,936 B |
| pool RAM | 30,720 B | 344,064 B | 313,344 B |

Verified final artifacts:

- `fm1tracker.bin` - 55,088 B - SHA-256 `7b313502f2ab222e6ecf19f970772a36f314bce7c880b505157d4d5a2e4dfbaf`;
- `fm1tracker-0.1.fwsc` - 610,110 B - SHA-256 `a3afcc069e4a9fe161f8470f477a76720b411fc6f302ebcf862c272a9a683bc2`;
- host WAV - SHA-256 `dac1695b8e69f26334d548005e6cae505cf39ace297dcf6a57338e70fc386b9b`, identical to the pristine WP1.3 baseline;
- decoded package identity: `FM-1_987`.

Inherited build safety checks pass, including image/RAM/pool limits, `.ram_text` call restriction and HAL-only direct register access.

Nominal callback period:

`256 / 44100 = 5.804988662 ms`

This is the available callback period, **not measured firmware execution time**.

Physical FM-1 install/boot/audio/panel/update behavior and actual callback execution time remain **NOT TESTED** because hardware is not currently available.

Durable Phase 1 evidence:

- `docs/WP1_3_VERIFICATION.md`
- `docs/WP1_4_VERIFICATION.md`
- `docs/WP1_5_VERIFICATION.md`
- `docs/TEST_MATRIX.md`
- `THIRD_PARTY.md`

Phase 1 issues #2 through #7 are closed/completed.

## WP2.1 implementation contract

Required compile-time dimensions:

- 8 logical tracks;
- 64 steps per pattern;
- 16 resident pattern slots;
- 128 order entries;
- 16 instrument slots;
- maximum four parameter locks associated with one step;
- maximum lock-pool capacity remains 2048 semantically, but **WP2.1 does not choose or implement the final lock allocator**.

Required structures / representation at this layer:

- `FmtStep` with the approved 8-byte representation and 16-bit `lock_head` sentinel;
- `FmtPattern` as 8 x 64 `FmtStep` records;
- `FmtTrackConfig`;
- `FmtOrderEntry`;
- small compile-time song metadata/default representation only where the existing V0.1 document already defines semantics;
- note/effect/sentinel constants required to make those records deterministic.

Important boundary: `DATA_MODEL_V0.1.md` explicitly defers the final parameter-lock allocator/container until profiling. WP2.1 therefore preserves the `lock_head` representation and lock semantic limits but does not implement insertion/replacement/free-list behavior. Pattern/song backing storage allocation is WP2.2, not WP2.1.

## WP2.1 non-goals

- no transport clock;
- no playback scheduling;
- no STEP REC or REST behavior;
- no tracker UI;
- no p-lock editing or allocator behavior beyond the already-approved storage representation/sentinels;
- no 8-BIT or FM4 synthesis work;
- no HAL, loader, rescue/update or USB redesign;
- no Furnace source import;
- no persistent project serializer/file format.

Do not begin WP2.2 in the same change.

## Important decisions / do not regress

- D005: V0.1 remains 8 tracks, 64 steps/pattern, 16 patterns and max four sparse p-locks/step.
- D016: fixed package identity is `FM-1_987`; release version lives in `APP_VER` and artifact filenames.
- Preserve the Phase 1 host/device regression gates as tracker code is introduced.
- Use the Phase 1 code/RAM/pool numbers above as the reference budget and record meaningful deltas.
- Keep baseline tone/platform behavior unchanged during WP2.1; this package is data-model only.
- No floating point in FM-1 firmware DSP; avoid runtime 64-bit division.
- No Furnace source/core import without a fresh exact-file/license audit.
- Physical hardware verification must remain distinct from host/device-build verification.

## Next action

Implement the smallest FM1Tracker-owned V0.1 model types/constants and focused host tests, without allocating the 16-pattern store yet. Then run host regression + device build/safety/resource verification and close WP2.1 only if every exit criterion passes.

## Last relevant checkpoints

- Phase 1 closeout / Phase 2 branch start: `3ed4b65d7cda15c5831533f153f91e9014a9fde7`
- WP1.5 final-current-workflow verified code/workflow: `14e57e071daa40ca09576c54720b384b6ada609b`
- WP1.5 final Actions run: `39970562995`

## Last updated

2026-10-23 - Phase 2 branch `phase2/tracker-core` created from the completed Phase 1 head. WP2.1 is IN PROGRESS. Scope remains compile-time data model only; no transport/UI/STEP REC/synth implementation is allowed in this package.
