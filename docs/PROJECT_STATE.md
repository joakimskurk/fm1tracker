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

WP2.1 and WP2.2 are complete. No WP2.3 implementation has started yet.

## Current milestone

**WP2.3 - integer transport clock**

State: **READY TO START**

GitHub issue: #11

Goal from the approved Phase 2 sequence:

Implement a deterministic integer/fixed-point transport clock for the V0.1 1/16-note grid, including exact 0..63 step wrap and long-run drift/error tests. It must remain timing/state only: no audible playback, event dispatch, STEP REC, REST, tracker UI or synth behavior.

Before WP2.3 code changes, follow `EXECUTION_PROTOCOL.md`: mark WP2.3 `IN PROGRESS`, record exact objective/non-goals/files/tests, then implement only the transport package.

## Completed WP2.2 checkpoint

**WP2.2 - bounds-safe pattern store**

State: **COMPLETE - 2026-10-23**

GitHub issue: #10

WP2.2 start head:

`11b8b9bd48e9b4b53ed8008c160b40297e1d15c4`

Final verified implementation/workflow commit:

`e180ac8cb5ba7a68fd3932883c988ad9ed67003b`

GitHub Actions:

- run: `39986704633`;
- job: `120527634095` (`pattern-store`);
- result: **SUCCESS**.

Durable evidence:

`docs/WP2_2_VERIFICATION.md`

### What WP2.2 implemented

- one actual 65,896-byte `FmtSong` backing store;
- store linked in `.pool`, not normal `.bss`;
- explicit deterministic reset of all 8,192 steps to the WP2.1 EMPTY/inherit/no-lock sentinels;
- narrow song/pattern/step store API;
- bounds-safe first/last/out-of-range access;
- mutation-isolation and reset-after-mutation tests;
- no transport, playback, UI, STEP REC, p-lock allocator, synth or persistence behavior.

### Verified physical linker placement

Linked symbol:

```text
01c20000 g       .pool        00010168 fmt_store_storage
```

Therefore:

- store address: `0x01C20000`;
- store size: 65,896 B;
- store end: `0x01C30168`;
- `.pool` region ends at `0x01C74000` exclusive.

### WP2.2 measured resources

| Resource | WP2.1 baseline | WP2.2 | Delta |
|---|---:|---:|---:|
| app image | 55,088 B | **55,188 B** | **+100 B** |
| normal RAM (`.data + .bss`) | 12,368 B | **12,368 B** | **0 B** |
| pool RAM | 30,720 B | **96,616 B** | **+65,896 B** |

Remaining physical pool after WP2.2: **247,448 B**.

The pool delta exactly equals `sizeof(FmtSong)`. The song store therefore consumes no ordinary RAM.

### WP2.2 verified artifacts

- `fm1tracker.bin`: SHA-256 `c952d8cfc0f8d9ae1b18750f40753663d0c14cdaa1d493e43c9502572af6d578`;
- `fm1tracker-0.1.fwsc`: SHA-256 `859892524248ee64e1ebb85c3808468f087665b39e603876b97b754e28550bd0`;
- package identity: `FM-1_987`;
- host WAV: SHA-256 `dac1695b8e69f26334d548005e6cae505cf39ace297dcf6a57338e70fc386b9b`, still byte-identical to Phase 1/WP2.1.

Actions evidence artifact:

- artifact ID `12516445200`;
- ZIP SHA-256 `70b03fdf9d4548f6e3139c1ecbfb18414e0ef1c4d16cddbd9d0c4939f3e0114f`.

Inherited build safety checks remain green, including `.ram_text`, memory/image limits and HAL-only direct register access.

## Completed WP2.1 checkpoint

**WP2.1 - compile-time tracker data model**

State: **COMPLETE - 2026-10-23**

GitHub issue: #9

Final verified implementation/workflow commit:

`3e922da4a33894bc938e3252e575a2908355f3a3`

Actions run `39985124933`, job `120522185302`: **SUCCESS**.

Evidence: `docs/WP2_1_VERIFICATION.md`.

Frozen core dimensions/shape:

- 8 tracks;
- 64 steps/pattern;
- 16 pattern slots;
- `FmtStep` 8 B;
- `FmtPattern` 4,096 B;
- `FmtSong` 65,896 B;
- max four p-locks/step semantically;
- 16-bit `lock_head`, `0xFFFF` = no lock;
- conservative future lock budget 16,384 B, not an allocator ABI.

## Regression baseline carried forward to WP2.3

FM1Tracker identity:

- visible app name: `FM1TRACKER`;
- fixed package identity: `FM-1_987`.

Current verified runtime baseline is the completed WP2.2 build:

| Resource | Used | Limit | Free |
|---|---:|---:|---:|
| app image | 55,188 B | 581,564 B | 526,376 B |
| normal RAM (`.data + .bss`) | 12,368 B | 98,304 B | 85,936 B |
| pool RAM | 96,616 B | 344,064 B | 247,448 B |

Host audio regression hash remains:

`dac1695b8e69f26334d548005e6cae505cf39ace297dcf6a57338e70fc386b9b`

External SDK remains:

- official repository: `https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK.git`;
- tag: `AC79NN_SDK_V1.2.1_2023-12-13`;
- resolved commit: `d179b4484759423312073f5fbb232501aa491047`;
- external/not vendored.

Physical FM-1 install/boot/audio/panel/update behavior and actual callback execution time remain **NOT TESTED** because hardware is not currently available.

## Important decisions / do not regress

- D005: V0.1 remains 8 tracks, 64 steps/pattern, 16 patterns and max four sparse p-locks/step.
- D016: package identity remains `FM-1_987`.
- D018: core in-memory model is frozen; p-lock allocator ABI and persistence are not.
- D019: one runtime `FmtSong` lives in `.pool` and receives explicit deterministic initialization.
- WP2.3 must use integer/fixed-point timing and a phase/error accumulator; repeated rounded step intervals are not acceptable.
- No floating point in firmware DSP; avoid runtime 64-bit division.
- Preserve the WP2.2 store placement/bounds tests and host/device regression gates.
- No Furnace source/core import without a fresh exact-file/license audit.
- Physical hardware verification remains distinct from host/device-build verification.

## WP2.3 strict non-goals

- no audible playback/voice triggering - WP2.4;
- no sequencer-to-audio event queue/dispatch;
- no STEP REC, REST, TIE or NOTE-OFF editing;
- no 16-step page input or track-selection UI;
- no tracker screen;
- no p-lock allocation/effects;
- no swing, polymeter or general microtiming;
- no synth-engine work;
- no persistence;
- no HAL/loader/update redesign.

## Next action

Start WP2.3 issue #11 as its own package. Mark it `IN PROGRESS` before code, implement only deterministic integer transport timing/wrap/drift tests, re-run host/device regression and record its resource delta. Do **not** begin WP2.4 in the same package.

## Last relevant checkpoints

- Phase 1 closeout / Phase 2 branch start: `3ed4b65d7cda15c5831533f153f91e9014a9fde7`
- WP2.1 final verified implementation/workflow: `3e922da4a33894bc938e3252e575a2908355f3a3`
- WP2.1 final Actions run: `39985124933`
- WP2.2 start: `11b8b9bd48e9b4b53ed8008c160b40297e1d15c4`
- WP2.2 final verified implementation/workflow: `e180ac8cb5ba7a68fd3932883c988ad9ed67003b`
- WP2.2 final Actions run: `39986704633`

## Last updated

2026-10-23 - WP2.2 completed and verified. WP2.3 / issue #11 is READY TO START; no transport implementation has been written yet.
