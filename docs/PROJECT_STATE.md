# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and relevant source/history before changing code.

## Current phase

**Phase 2 - tracker core without synth ambition**

State: **IN PROGRESS**

Phase 2 parent issue: #8

Current branch: `phase2/tracker-core`

Phase 2 branch start / completed Phase 1 head:

`3ed4b65d7cda15c5831533f153f91e9014a9fde7`

WP2.1, WP2.2 and WP2.3 are complete. No WP2.4 implementation has started yet.

## Current milestone

**WP2.4 - one-track audible playback**

State: **READY TO START**

GitHub issue: #12

Goal from the approved Phase 2 sequence:

Use the verified WP2.3 transport and existing baseline sine voice to prove deterministic audible playback of one logical tracker track through a complete 64-step pattern loop. Keep the package strictly playback-only: no STEP REC/editor input, page UI, track-selection UX, general multitrack playback, p-locks, new synth engines, song mode or persistence.

Before WP2.4 code changes, follow `EXECUTION_PROTOCOL.md`: mark WP2.4 `IN PROGRESS`, record exact playback semantics/non-goals/files/tests, resolve the initial step-0 dispatch contract explicitly, then implement only the one-track playback proof.

## Completed WP2.3 checkpoint

**WP2.3 - integer transport clock**

State: **COMPLETE - 2026-10-23**

GitHub issue: #11

WP2.3 start head:

`5d8fd495bdb4877cb15670801a86610ac18d7eaa`

Final verified implementation/workflow commit:

`c83ee9cd835503eb410dd8110c2049351a645578`

GitHub Actions:

- run: `39988630290`;
- job: `120534295734` (`integer-transport`);
- result: **SUCCESS**.

Durable evidence:

`docs/WP2_3_VERIFICATION.md`

### Timing contract verified

D020 defines exact V0.1 1/16-note timing in the 44.1 kHz frame domain:

`frames_per_step = 44100 * 15 / BPM`

The firmware uses a 32-bit phase accumulator with threshold `661500 = 44100 * 15`, adding integer BPM per audio frame. A threshold crossing subtracts 661500 and advances exactly one step, wrapping `63 -> 0`.

Verified properties:

- no float in firmware transport;
- no runtime transport division;
- no firmware 64-bit arithmetic in the transport path;
- exact fractional remainder retained automatically;
- deterministic reset: stopped, step 0, phase 0, 120 BPM;
- stop preserves phase/step; start resumes;
- BPM 0 rejected;
- valid BPM edits preserve phase;
- bulk timing equals frame-by-frame timing;
- six one-hour representative-BPM runs and a max-uint16 BPM reference test show no cumulative interval-rounding drift.

### WP2.3 linked state

```text
01c0b050 g       .bss         00000008 fmt_transport_runtime
01c20000 g       .pool        00010168 fmt_store_storage
```

The 8-byte transport state is ordinary control RAM. The WP2.2 song store remains unchanged in `.pool`.

### WP2.3 measured resources

| Resource | WP2.2 | WP2.3 | Delta |
|---|---:|---:|---:|
| app image | 55,188 B | **55,328 B** | **+140 B** |
| normal RAM (`.data + .bss`) | 12,368 B | **12,376 B** | **+8 B** |
| pool RAM | 96,616 B | **96,616 B** | **0 B** |

Current free resources:

- app image: 526,236 B;
- normal RAM: 85,928 B;
- pool RAM: 247,448 B.

### WP2.3 artifacts

- `fm1tracker.bin`: SHA-256 `b1d0489f74393622b6f2d117b497c42e4659300185018ad82b6fdf52648c1e34`;
- `fm1tracker-0.1.fwsc`: SHA-256 `22e45c217704468f87479243de14b1b99ad9dfdbe2033103d2dcbca7b1c3cd55`;
- package identity: `FM-1_987`;
- host WAV: SHA-256 `dac1695b8e69f26334d548005e6cae505cf39ace297dcf6a57338e70fc386b9b`, still byte-identical to the Phase 1/WP2.1/WP2.2 baseline.

Actions evidence artifact:

- artifact ID `12517195879`;
- ZIP SHA-256 `828d0010c1ffc364fae86f5466ee5c3e2c1b97ac55681510958b4e95c715efec`.

Inherited build safety checks remain green, including `.ram_text`, image/RAM/pool limits and HAL-only direct register access.

WP2.3 deliberately leaves the transport stopped in the app. No row dispatch or audible tracker playback has been added; that boundary belongs to WP2.4.

## Completed WP2.2 checkpoint

**WP2.2 - bounds-safe pattern store** — COMPLETE 2026-10-23

Issue #10. Evidence: `docs/WP2_2_VERIFICATION.md`.

Verified one 65,896-byte `FmtSong` at `.pool` address `0x01C20000`, explicit initialization of all 8,192 steps, runtime bounds/reset safety and resulting pool use 96,616 B.

## Completed WP2.1 checkpoint

**WP2.1 - compile-time tracker data model** — COMPLETE 2026-10-23

Issue #9. Evidence: `docs/WP2_1_VERIFICATION.md`.

Frozen V0.1 core remains 8 tracks, 64 steps/pattern, 16 patterns, `FmtStep` 8 B, `FmtPattern` 4,096 B and `FmtSong` 65,896 B. P-lock allocator ABI and persistence remain unfrozen.

## Regression baseline carried into WP2.4

FM1Tracker identity:

- visible app name: `FM1TRACKER`;
- fixed package identity: `FM-1_987`.

| Resource | Used | Limit | Free |
|---|---:|---:|---:|
| app image | 55,328 B | 581,564 B | 526,236 B |
| normal RAM (`.data + .bss`) | 12,376 B | 98,304 B | 85,928 B |
| pool RAM | 96,616 B | 344,064 B | 247,448 B |

Host audio regression hash before audible tracker playback is:

`dac1695b8e69f26334d548005e6cae505cf39ace297dcf6a57338e70fc386b9b`

External SDK remains official tag `AC79NN_SDK_V1.2.1_2023-12-13`, resolved commit `d179b4484759423312073f5fbb232501aa491047`, external/not vendored.

Physical FM-1 install/boot/audio/panel/update behavior and actual callback execution time remain **NOT TESTED** because hardware is not currently available.

## Important decisions / do not regress

- D005: V0.1 dimensions remain 8 tracks × 64 steps × 16 patterns.
- D016: fixed package identity remains `FM-1_987`.
- D018: core in-memory model is frozen; persistence and p-lock allocator are separate.
- D019: exactly one runtime `FmtSong` lives in `.pool` with deterministic explicit reset.
- D020: transport uses the exact BPM-per-frame rational phase accumulator above.
- Preserve WP2.1/WP2.2/WP2.3 regression coverage as playback is added.
- No floating point in firmware DSP and no runtime 64-bit division.
- Physical hardware verification remains distinct from host/device-build verification.

## WP2.4 strict non-goals

- no STEP REC or note editing;
- no 16-step page input;
- no track-selection UX or general multitrack playback;
- no tracker screen;
- no p-lock allocator/effects;
- no new 8-BIT/FM4 engine implementation;
- no song/order mode;
- no persistence;
- no HAL/loader/update redesign.

## Next action

Start WP2.4 issue #12 as its own package. Before code, mark it `IN PROGRESS` and explicitly define the one-track playback fixture, note-event semantics needed by this package, and initial step-0 dispatch behavior. Verify exact transport-boundary event timing, deterministic 64-step looping, host playback evidence, device safety and resource delta. Do **not** begin WP2.5 in the same package.

## Last relevant checkpoints

- Phase 1 closeout / Phase 2 branch start: `3ed4b65d7cda15c5831533f153f91e9014a9fde7`
- WP2.1 verified implementation/workflow: `3e922da4a33894bc938e3252e575a2908355f3a3`
- WP2.2 verified implementation/workflow: `e180ac8cb5ba7a68fd3932883c988ad9ed67003b`
- WP2.2 Actions run: `39986704633`
- WP2.3 start: `5d8fd495bdb4877cb15670801a86610ac18d7eaa`
- WP2.3 verified implementation/workflow: `c83ee9cd835503eb410dd8110c2049351a645578`
- WP2.3 Actions run: `39988630290`

## Last updated

2026-10-23 - WP2.3 completed and verified. WP2.4 / issue #12 is READY TO START; no audible tracker playback implementation has been written yet.
