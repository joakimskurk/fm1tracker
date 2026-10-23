# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and relevant source/history before changing code.

## Current phase

**Phase 2 - tracker core without synth ambition**

State: **IN PROGRESS**

Phase 2 parent issue: #8

Current Phase 2 branch:

`phase2/tracker-core`

Phase 2 branch start / completed Phase 1 head:

`3ed4b65d7cda15c5831533f153f91e9014a9fde7`

## Current milestone

**WP2.2 - bounds-safe pattern store**

State: **IN PROGRESS**

GitHub issue: #10

WP2.2 start head:

`11b8b9bd48e9b4b53ed8008c160b40297e1d15c4`

### Exact objective

Allocate exactly one V0.1 `FmtSong` backing store in the intended FM-1 pool memory and make runtime song/pattern/track/step access deterministic and bounds-safe, using the compile-time model frozen by WP2.1.

### Memory placement decision for this package

The store belongs in linker section `.pool`, not ordinary `.bss`:

- `firmware/app.ld` defines `POOL` at `0x01C20000` with length `0x54000` = 344,064 B;
- `.pool` is `NOLOAD` and is zeroed by `fm1_cstart()` before `app_main()`;
- the existing graphics canvas already uses `__attribute__((section(".pool")))`;
- current measured pool use is 30,720 B;
- `sizeof(FmtSong)` is 65,896 B;
- allocating one store therefore predicts 96,616 B pool use before later p-lock storage, subject to device-build verification.

Zero-init alone is not a valid empty tracker song because a default `FmtStep` requires `note = 0xFF`, instrument/volume inherit `0xFF`, and `lock_head = 0xFFFF`. WP2.2 therefore owns an explicit bounded initialization/reset pass.

### Expected implementation files

- `firmware/src/fmt_store.h` - narrow store API;
- `firmware/src/fmt_store.c` - single `.pool` store, deterministic init/reset and bounds-safe access;
- `firmware/src/dinghy.c` - minimal integration only: compile the store and initialize it before the app loop;
- focused store tests under `tests/`;
- WP2.2 Actions verification workflow;
- documentation/status files only as required by the execution protocol.

### Required verification

1. host tests prove all 8,192 steps receive the WP2.1 empty/inherit/no-lock defaults;
2. first/last valid pattern/track/step addresses work and every out-of-range dimension is rejected;
3. mutation of one step does not alias neighboring steps and reset restores defaults;
4. existing host audio remains bit-identical to the Phase 1/WP2.1 WAV baseline;
5. device symbol/address inspection proves the store is physically inside `.pool`;
6. full device/package build and inherited Dinghy safety gates pass;
7. package identity remains `FM-1_987`;
8. actual image, normal-RAM and pool deltas are recorded against the Phase 1/WP2.1 baseline.

### WP2.2 non-goals

- no transport clock or pattern scheduling;
- no playback/event dispatch;
- no STEP REC or REST editing behavior;
- no TIE/NOTE-OFF editor semantics;
- no tracker UI, page input or track-selection UX;
- no p-lock allocator/insertion/replacement/deletion behavior;
- no stable parameter-ID registry;
- no synth-engine work;
- no persistence/serialization;
- no HAL, loader, rescue/update or USB redesign;
- no Furnace source import.

Do not begin WP2.3 in the same change.

## Completed WP2.1 checkpoint

**WP2.1 - compile-time tracker data model**

State: **COMPLETE - 2026-10-23**

GitHub issue: #9

Final verified implementation/workflow commit:

`3e922da4a33894bc938e3252e575a2908355f3a3`

GitHub Actions run `39985124933`, job `120522185302`: **SUCCESS**.

Durable evidence: `docs/WP2_1_VERIFICATION.md`.

Verified compile-time model:

- `FmtStep`: 8 B;
- `FmtPattern`: 4,096 B;
- `FmtSong`: 65,896 B;
- conservative future lock budget: 16,384 B;
- budgeted song + lock reserve: 82,280 B / 98,304 B editable-song target.

WP2.1 runtime delta was 0 B image, 0 B normal RAM and 0 B pool RAM. Host WAV, firmware binary and package remained byte-for-byte identical to Phase 1.

D018 remains in force: raw compiler structs are not persistence format, the final p-lock allocator/list ABI is not frozen, and stable parameter IDs remain later work.

## Phase 1 / regression baseline

Pinned Dinghy upstream:

`d44df7db09b98ccad2f1233b8f83f5f09c04829e`

FM1Tracker identity: `FM-1_987`, visible app name `FM1TRACKER`.

| Resource | Used | Limit | Free |
|---|---:|---:|---:|
| app image | 55,088 B | 581,564 B | 526,476 B |
| normal RAM (`.data + .bss`) | 12,368 B | 98,304 B | 85,936 B |
| pool RAM | 30,720 B | 344,064 B | 313,344 B |

Reference hashes:

- `fm1tracker.bin`: `7b313502f2ab222e6ecf19f970772a36f314bce7c880b505157d4d5a2e4dfbaf`;
- `fm1tracker-0.1.fwsc`: `a3afcc069e4a9fe161f8470f477a76720b411fc6f302ebcf862c272a9a683bc2`;
- host WAV: `dac1695b8e69f26334d548005e6cae505cf39ace297dcf6a57338e70fc386b9b`.

External SDK remains the official tag `AC79NN_SDK_V1.2.1_2023-12-13`, resolved commit `d179b4484759423312073f5fbb232501aa491047`, external/not vendored.

Physical FM-1 install/boot/audio/panel/update behavior and actual callback execution time remain **NOT TESTED** because hardware is not currently available.

## Important decisions / do not regress

- D005: 8 tracks, 64 steps/pattern, 16 patterns, max four sparse p-locks/step.
- D016: fixed package identity `FM-1_987`.
- D018: core type shape is frozen; allocation/p-lock allocator/persistence are separate concerns.
- WP2.2 places one actual `FmtSong` in `.pool` and initializes it deterministically; it does not implement musical sequencing behavior.
- Preserve Phase 1/WP2.1 host/device regression gates.
- No floating point in firmware DSP; avoid runtime 64-bit division.
- Physical hardware verification remains distinct from host/device-build verification.

## Next action

Implement and verify only WP2.2 according to issue #10. Close it only when pool placement, defaults, access boundaries, host regression, device build/safety and measured resource deltas all pass. Then set WP2.3 to READY TO START without implementing it.

## Last relevant checkpoints

- Phase 1 closeout / Phase 2 branch start: `3ed4b65d7cda15c5831533f153f91e9014a9fde7`
- WP2.1 final verified implementation/workflow: `3e922da4a33894bc938e3252e575a2908355f3a3`
- WP2.1 final Actions run: `39985124933`
- WP2.2 start: `11b8b9bd48e9b4b53ed8008c160b40297e1d15c4`

## Last updated

2026-10-23 - WP2.2 started on `phase2/tracker-core`. Scope is the real bounds-safe `.pool` song/pattern store only; WP2.3 transport has not started.
