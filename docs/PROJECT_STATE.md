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

WP2.1 and WP2.2 are complete.

## Current milestone

**WP2.3 - integer transport clock**

State: **IN PROGRESS — 2026-10-23**

GitHub issue: #11

WP2.3 start head:

`5d8fd495bdb4877cb15670801a86610ac18d7eaa`

### Exact objective

Implement only the deterministic V0.1 transport timing/state layer for the fixed 1/16-note grid in the 44.1 kHz frame domain. The package must prove exact `0..63` step wrap, deterministic reset/start/stop behavior and no cumulative timing drift.

### Timing decision

D020 is accepted before implementation.

For a 1/16-note grid:

`frames_per_step = 44100 * 15 / BPM`

WP2.3 therefore uses an integer phase threshold of `661500 = 44100 * 15`. Each rendered frame adds the current integer BPM to a 32-bit phase accumulator. Crossing the threshold subtracts 661500 and advances the current step exactly once.

Properties:

- no floating point;
- no runtime division;
- no runtime 64-bit arithmetic;
- exact fractional remainder is retained automatically;
- a 16-bit BPM cannot cross more than one step in one frame;
- the exact frame of each future step boundary is observable without implementing playback yet.

Runtime transport reset state is stopped, step 0, phase 0, 120 BPM. Stop preserves phase/step; start resumes. BPM 0 is rejected rather than overloaded as a stop command.

### Expected implementation files

- `firmware/src/fmt_transport.h` — bounded transport state/API/constants;
- `firmware/src/fmt_transport.c` — reset/start/stop/tempo/tick/advance implementation;
- `firmware/src/dinghy.c` — minimal integration only: one small runtime transport object, reset once, bounded advance from the audio callback while it remains stopped in WP2.3;
- `tests/test_fmt_transport.c` and runner — host timing/wrap/state/drift tests;
- `.github/workflows/wp2_3_transport_verify.yml` — regression/device/resource evidence;
- documentation/status files only as required for package closeout.

### Required verification

1. reset is deterministic: stopped, step 0, phase 0, 120 BPM;
2. stopped advance changes nothing;
3. start/stop/resume preserves exact phase and current step;
4. BPM 0 is rejected and a valid tempo change preserves phase progress;
5. step 63 wraps exactly to step 0;
6. representative tempos including fractional-frame durations match an independent host reference over long runs with no accumulated interval-rounding drift;
7. existing WP2.1 model tests and WP2.2 store tests remain green;
8. host WAV remains byte-identical to the Phase 1/WP2.1/WP2.2 baseline;
9. full device build and inherited safety checks remain green;
10. package identity remains `FM-1_987`;
11. actual image, normal-RAM and pool deltas are recorded against WP2.2.

### Strict non-goals

- no audible note playback or voice triggering — WP2.4;
- no row/event dispatch or sequencer-to-audio queue;
- no STEP REC, REST, TIE or NOTE-OFF editing;
- no 16-step page input, track-selection UI or tracker screen;
- no swing, polymeter or general microtiming;
- no p-lock allocator/effects;
- no synth-engine work;
- no persistence/serialization;
- no HAL/loader/update redesign;
- no Furnace source/core import.

Do not begin WP2.4 in the same package.

## Completed WP2.2 checkpoint

**WP2.2 - bounds-safe pattern store** — COMPLETE 2026-10-23

Issue #10. Durable evidence: `docs/WP2_2_VERIFICATION.md`.

Final verified implementation/workflow commit:

`e180ac8cb5ba7a68fd3932883c988ad9ed67003b`

Actions run `39986704633`, job `120527634095`: **SUCCESS**.

Verified runtime baseline carried into WP2.3:

| Resource | Used | Limit | Free |
|---|---:|---:|---:|
| app image | 55,188 B | 581,564 B | 526,376 B |
| normal RAM (`.data + .bss`) | 12,368 B | 98,304 B | 85,936 B |
| pool RAM | 96,616 B | 344,064 B | 247,448 B |

One 65,896-byte `FmtSong` is linked at `.pool` address `0x01C20000`. All 8,192 steps initialize explicitly to the frozen tracker sentinels. Host WAV SHA-256 remains `dac1695b8e69f26334d548005e6cae505cf39ace297dcf6a57338e70fc386b9b`.

## Completed WP2.1 checkpoint

**WP2.1 - compile-time tracker data model** — COMPLETE 2026-10-23

Issue #9. Evidence: `docs/WP2_1_VERIFICATION.md`.

Frozen V0.1 core remains 8 tracks, 64 steps/pattern, 16 patterns, `FmtStep` 8 B, `FmtPattern` 4,096 B and `FmtSong` 65,896 B. P-lock allocator ABI and persistence remain unfrozen.

## Important decisions / do not regress

- D005: V0.1 dimensions remain 8 tracks × 64 steps × 16 patterns.
- D016: fixed package identity remains `FM-1_987`.
- D018: core in-memory model is frozen; persistence and p-lock allocator are separate.
- D019: exactly one runtime `FmtSong` lives in `.pool` with deterministic explicit reset.
- D020: transport uses the exact BPM-per-frame rational phase accumulator described above.
- Preserve WP2.1/WP2.2 host/device regression gates.
- No floating point in firmware DSP and no runtime 64-bit division.
- Physical FM-1 install/boot/audio/panel/update behavior and actual callback execution time remain **NOT TESTED** because hardware is not currently available.

## Next action

Implement and verify only WP2.3 according to issue #11 and D020. Close WP2.3 only after timing/state tests, host regression, device build/safety and measured resource deltas all pass. Then mark WP2.4 READY TO START without implementing playback.

## Last relevant checkpoints

- Phase 1 closeout / Phase 2 branch start: `3ed4b65d7cda15c5831533f153f91e9014a9fde7`
- WP2.1 verified implementation/workflow: `3e922da4a33894bc938e3252e575a2908355f3a3`
- WP2.2 verified implementation/workflow: `e180ac8cb5ba7a68fd3932883c988ad9ed67003b`
- WP2.2 Actions run: `39986704633`
- WP2.3 start: `5d8fd495bdb4877cb15670801a86610ac18d7eaa`

## Last updated

2026-10-23 - WP2.3 started. Only deterministic integer transport timing/state is active; WP2.4 playback has not started.