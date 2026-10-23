# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and relevant source/history before changing code.

## Current phase

**Phase 2 - tracker core without synth ambition**

State: **READY TO START**

Phase 1 is complete. Phase 2 implementation has **not** started yet.

Phase 2 parent issue: #8

## Current milestone

**WP2.1 - compile-time tracker data model**

State: **READY TO START**

GitHub issue: #9

Current repository branch at Phase 1 closeout:

`phase1/fm1tracker-identity`

Before WP2.1 code changes, create/use a dedicated Phase 2 working branch from the completed Phase 1 head and mark WP2.1 `IN PROGRESS` here according to `EXECUTION_PROTOCOL.md`.

## Completed Phase 1 baseline

Pinned Dinghy upstream:

`d44df7db09b98ccad2f1233b8f83f5f09c04829e`

FM1Tracker identity:

- visible app name: `FM1TRACKER`;
- fixed package identity: `FM-1_987`;
- host artifacts: `build/host/fm1tracker.wav` and `fm1tracker-*.png`;
- device artifacts: `build/fm1tracker.bin` and versioned `fm1tracker-*.fwsc`.

Protected Dinghy app/platform/HAL/loader/build-driver implementation remained unchanged through Phase 1. The FM1Tracker identity is supplied by the thin wrapper/build layer introduced in WP1.4.

No tracker engine, transport, STEP REC implementation, p-lock editing, song mode, 8-BIT engine, 16-BIT FM4 engine or Furnace source has been added yet.

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

Durable evidence:

- `docs/WP1_3_VERIFICATION.md`
- `docs/WP1_4_VERIFICATION.md`
- `docs/WP1_5_VERIFICATION.md`
- `docs/TEST_MATRIX.md`
- `THIRD_PARTY.md`

Phase 1 issues #2 through #7 are closed/completed.

## WP2.1 objective

Implement only the already-approved V0.1 compile-time tracker data model.

Required deliverables:

1. compile-time constants for 8 tracks, 64 steps/pattern, 16 patterns and max four p-locks/step;
2. V0.1 song/pattern/track/step structures from `DATA_MODEL_V0.1.md`;
3. bounds-safe accessors appropriate to this layer;
4. static size/budget assertions;
5. host tests for structure sizes, defaults and access bounds;
6. host + device compile/build verification;
7. measured image/RAM/pool delta against the Phase 1 baseline.

## WP2.1 non-goals

- no transport clock;
- no playback scheduling;
- no STEP REC or REST behavior;
- no tracker UI;
- no p-lock editing behavior beyond the already-approved storage representation;
- no 8-BIT or FM4 synthesis work;
- no HAL, loader, rescue/update or USB redesign;
- no Furnace source import.

Do not begin WP2.2 in the same change.

## Important decisions / do not regress

- D016: fixed package identity is `FM-1_987`; release version lives in `APP_VER` and artifact filenames.
- Preserve the Phase 1 host/device regression gates as tracker code is introduced.
- Use the Phase 1 code/RAM/pool numbers above as the reference budget and record meaningful deltas.
- Keep the baseline tone/platform behavior unchanged during WP2.1; this package is data-model only.
- No floating point in FM-1 firmware DSP; avoid runtime 64-bit division.
- V0.1 remains 8 tracks, 64 steps/pattern, 16 patterns, max four p-locks/step unless a superseding decision is recorded first.
- STEP REC, REST and p-lock editing remain later bounded packages; do not pull them into WP2.1.
- No Furnace source/core import without a fresh exact-file/license audit.
- Physical hardware verification must remain distinct from host/device-build verification.

## Next recommended step

Start a new dedicated Phase 2 branch from the completed Phase 1 head, set WP2.1 / issue #9 to `IN PROGRESS`, then inspect `docs/DATA_MODEL_V0.1.md` and the current build/test integration points before adding the smallest compile-time data-model implementation. Do not add transport, UI, STEP REC or synth behavior in that work package.

## Last relevant checkpoints

- pristine code verification target: `21cb59ee455205a8243fee381e24eda517da2c23`
- WP1.4 branch start: `ff0a095b2384532f19724204b9059e88aac5a52c`
- WP1.4 verified implementation: `78a8f70879a2daec1ad5f99f95fbe5cfbdd5bba3`
- WP1.5 start: `57617e288972a476fd221ae379e7bb75853b7451`
- WP1.5 final-current-workflow verified code/workflow: `14e57e071daa40ca09576c54720b384b6ada609b`
- WP1.5 final Actions run: `39970562995`

## Last updated

2026-10-23 - WP1.5 and Phase 1 completed. Phase 2 / WP2.1 is prepared but not started; no tracker implementation has been written yet.
