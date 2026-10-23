# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and relevant source/history before changing code.

## Current phase

**Phase 2 - tracker core without synth ambition**

State: **IN PROGRESS**

Phase 1 is complete. WP2.1 is complete. No WP2.2 implementation has started yet.

Phase 2 parent issue: #8

Current Phase 2 branch:

`phase2/tracker-core`

Branch start / completed Phase 1 head:

`3ed4b65d7cda15c5831533f153f91e9014a9fde7`

## Current milestone

**WP2.2 - bounds-safe pattern store**

State: **READY TO START**

Goal from the approved work-package sequence:

Allocate pattern/song state in the intended memory region and make runtime pattern/track/step access deterministic and bounds-safe, using the compile-time model frozen by WP2.1.

WP2.2 must remain a separate change/package. Do not mix in the transport clock, playback, STEP REC, REST, tracker UI, p-lock allocator/editing or synth engines.

## Completed WP2.1 checkpoint

**WP2.1 - compile-time tracker data model**

State: **COMPLETE — 2026-10-23**

GitHub issue: #9

Final verified implementation/workflow commit:

`3e922da4a33894bc938e3252e575a2908355f3a3`

GitHub Actions:

- run: `39985124933`;
- job: `120522185302` (`compile-time-model`);
- result: **SUCCESS**.

Durable evidence:

`docs/WP2_1_VERIFICATION.md`

WP2.1 implemented only compile-time model/constants/accessors/tests. It did not instantiate song/pattern storage or introduce runtime tracker behavior.

Verified compile-time model:

- `FmtStep`: 8 B;
- `FmtPattern`: 4,096 B;
- `FmtSong` core shape: 65,896 B;
- conservative future lock budget: 16,384 B;
- budgeted core + lock reserve: 82,280 B / 98,304 B editable-song target;
- remaining budget under that target before later model additions: 16,024 B.

Verified WP2.1 runtime/resource impact versus Phase 1:

| Resource | Phase 1 | WP2.1 | Delta |
|---|---:|---:|---:|
| app image | 55,088 B | 55,088 B | 0 B |
| normal RAM (`.data + .bss`) | 12,368 B | 12,368 B | 0 B |
| pool RAM | 30,720 B | 30,720 B | 0 B |

Host WAV, firmware binary and firmware package remain byte-for-byte identical to the Phase 1 baseline.

WP2.1 model/access tests verify constants, sentinels, exact sizes, defaults, null safety and rejection of out-of-range pattern/track/step indexes.

Protected platform/HAL/loader/build tooling remained unchanged. The only change to the existing baseline app file is the include of `fmt_model.h` so the device compiler also enforces the model assertions.

Important WP2.1 boundary retained for later work:

- no p-lock allocator/list representation frozen;
- `lock_head` remains 16-bit with `0xFFFF` as no-lock sentinel;
- no global song/pattern backing store allocated yet;
- raw compiler structs are not a persistence format;
- stable parameter IDs/effect registry beyond `NONE` remain later work.

## Completed Phase 1 baseline

Pinned Dinghy upstream:

`d44df7db09b98ccad2f1233b8f83f5f09c04829e`

FM1Tracker identity:

- visible app name: `FM1TRACKER`;
- fixed package identity: `FM-1_987`;
- host artifacts: `build/host/fm1tracker.wav` and `fm1tracker-*.png`;
- device artifacts: `build/fm1tracker.bin` and versioned `fm1tracker-*.fwsc`.

Phase 1 final reference budget:

| Resource | Used | Limit | Free |
|---|---:|---:|---:|
| app image | 55,088 B | 581,564 B | 526,476 B |
| normal RAM (`.data + .bss`) | 12,368 B | 98,304 B | 85,936 B |
| pool RAM | 30,720 B | 344,064 B | 313,344 B |

Reference hashes:

- `fm1tracker.bin`: `7b313502f2ab222e6ecf19f970772a36f314bce7c880b505157d4d5a2e4dfbaf`;
- `fm1tracker-0.1.fwsc`: `a3afcc069e4a9fe161f8470f477a76720b411fc6f302ebcf862c272a9a683bc2`;
- host WAV: `dac1695b8e69f26334d548005e6cae505cf39ace297dcf6a57338e70fc386b9b`.

External SDK build input remains:

- official repository: `https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK.git`;
- tag: `AC79NN_SDK_V1.2.1_2023-12-13`;
- resolved commit: `d179b4484759423312073f5fbb232501aa491047`;
- SDK remains external and is not vendored.

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

## Important decisions / do not regress

- D005: V0.1 remains 8 tracks, 64 steps/pattern, 16 patterns and max four sparse p-locks/step.
- D016: fixed package identity is `FM-1_987`; release version lives in `APP_VER` and artifact filenames.
- D018: WP2.1 froze the in-memory core shape, not backing-store allocation, p-lock allocator ABI or persistence format.
- Preserve Phase 1 host/device regression gates as tracker code becomes runtime state.
- Use the Phase 1 code/RAM/pool numbers as the reference budget and record meaningful deltas for each package.
- No floating point in FM-1 firmware DSP; avoid runtime 64-bit division.
- No Furnace source/core import without a fresh exact-file/license audit.
- Physical hardware verification must remain distinct from host/device-build verification.

## Next action

Start WP2.2 as its own work package on `phase2/tracker-core`:

1. read `DATA_MODEL_V0.1.md`, `ARCHITECTURE_V0.1.md`, D018 and `WP2_1_VERIFICATION.md`;
2. mark WP2.2 `IN PROGRESS` before implementation;
3. decide/document the intended runtime memory placement for the pattern/song store within the already measured pool constraints;
4. implement only the bounds-safe store/allocation layer and focused tests;
5. re-run host/device safety and capture the real image/RAM/pool delta;
6. do **not** begin WP2.3 transport in the same package.

## Last relevant checkpoints

- Phase 1 closeout / Phase 2 branch start: `3ed4b65d7cda15c5831533f153f91e9014a9fde7`
- WP2.1 final verified implementation/workflow: `3e922da4a33894bc938e3252e575a2908355f3a3`
- WP2.1 final Actions run: `39985124933`
- WP2.1 final job: `120522185302`

## Last updated

2026-10-23 - WP2.1 completed with a zero-byte runtime/resource delta and bit-identical Phase 1 outputs. WP2.2 is READY TO START but has not been implemented.
