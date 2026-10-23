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

WP2.1, WP2.2 and WP2.3 are complete.

## Current milestone

**WP2.4 - one-track audible playback**

State: **IN PROGRESS - 2026-10-23**

GitHub issue: #12

WP2.4 start head:

`64c4b7d260dca2471d7658a9a59bec23a24454a5`

### Exact objective

Use the verified WP2.3 transport and existing baseline integer sine primitive to prove deterministic audible playback of exactly one logical tracker lane through a complete 64-step pattern loop. Playback reads pattern 0 / track 0 from the existing pool-backed store and must change note state at the exact D020 frame boundaries, not merely at 256-frame callback boundaries.

### Playback timing/row contract

D021 is accepted before implementation:

- after transport reset, step 0 is decoded exactly once before rendered frame 0;
- transport is then started;
- each tracker audio frame is rendered using the current row state;
- `fmt_transport_tick_frame()` runs at the end of that rendered frame;
- when the tick advances the step, that new row is decoded immediately and applies beginning with the next frame;
- at 120 BPM, step 1 begins on frame 5513 and the first `63 -> 0` loop returns to step 0 on frame 352800;
- only ordinary notes `0..127` trigger the WP2.4 proof voice;
- EMPTY is no event and sustains the proof voice;
- TIE/OFF/other special values are no event in WP2.4; their full semantics remain WP2.8;
- no general event queue is introduced.

The proof voice is one dedicated monophonic use of the existing baseline sine primitive. It is not a new 8-BIT/16-BIT engine and does not alter the existing manual four-voice audition path.

### Expected implementation files

- `firmware/src/fmt_playback.h` / `fmt_playback.c` - narrow immediate one-lane row-dispatch helper around pattern 0 / track 0 and the WP2.3 transport;
- `firmware/src/dinghy.c` - minimal dedicated proof sine state, deterministic playback fixture seed, start at frame 0 and per-frame tracker rendering/tick;
- `tests/test_fmt_playback.c` plus runner - exact event trace/frame-boundary/loop/store tests;
- `.github/workflows/wp2_4_playback_verify.yml` - scope, host playback, prior regressions, device build/safety/resource evidence;
- documentation/status files only as required for package closeout.

### Required verification

1. step 0 dispatches once at frame 0;
2. step 1 starts on the exact D020 next-frame boundary for representative fractional timing;
3. pattern 0 / track 0 reads from the real bounds-safe store;
4. EMPTY produces no note event and preserves the current proof note;
5. special TIE/OFF values do not accidentally become raw pitches in WP2.4;
6. exact step 63 -> 0 loop event occurs at the expected frame without callback quantization or accumulated drift;
7. event trace remains deterministic when frames are grouped into normal 256-frame host callback blocks;
8. WP2.1 model, WP2.2 store and WP2.3 transport tests remain green;
9. host run produces a deterministic audible tracker WAV and documented new hash;
10. existing manual input path remains buildable and no UI/editor behavior is added;
11. full JieLi device build, package identity and inherited safety checks remain green;
12. image, normal-RAM and pool deltas are recorded against WP2.3.

### Strict non-goals

- no STEP REC or note editing;
- no REST/TIE/NOTE-OFF editor semantics;
- no 16-step page input;
- no track-selection UX or general multitrack playback;
- no tracker screen;
- no p-lock allocator/effects;
- no new 8-BIT/FM4 engine implementation;
- no song/order mode;
- no persistence;
- no HAL/loader/update redesign;
- no Furnace source/core import;
- do not begin WP2.5 in this package.

## Completed WP2.3 checkpoint

**WP2.3 - integer transport clock** - COMPLETE 2026-10-23

Issue #11. Durable evidence: `docs/WP2_3_VERIFICATION.md`.

WP2.3 start head:

`5d8fd495bdb4877cb15670801a86610ac18d7eaa`

Final verified implementation/workflow commit:

`c83ee9cd835503eb410dd8110c2049351a645578`

GitHub Actions run `39988630290`, job `120534295734`: **SUCCESS**.

D020 timing remains:

`frames_per_step = 44100 * 15 / BPM`

with 32-bit phase threshold `661500`, exact fractional remainder, deterministic reset/start/stop and exact 63 -> 0 wrap.

WP2.3 runtime baseline carried into WP2.4:

| Resource | Used | Limit | Free |
|---|---:|---:|---:|
| app image | 55,328 B | 581,564 B | 526,236 B |
| normal RAM (`.data + .bss`) | 12,376 B | 98,304 B | 85,928 B |
| pool RAM | 96,616 B | 344,064 B | 247,448 B |

Linked state:

```text
01c0b050 g       .bss         00000008 fmt_transport_runtime
01c20000 g       .pool        00010168 fmt_store_storage
```

Pre-playback host WAV SHA-256:

`dac1695b8e69f26334d548005e6cae505cf39ace297dcf6a57338e70fc386b9b`

## Completed earlier checkpoints

- WP2.2 / issue #10: one 65,896-byte `FmtSong` at `.pool` `0x01C20000`, all 8,192 steps explicitly reset, bounds-safe runtime store. Evidence: `docs/WP2_2_VERIFICATION.md`.
- WP2.1 / issue #9: frozen 8-track × 64-step × 16-pattern compile-time core, 8-byte `FmtStep`, 4,096-byte `FmtPattern`, 65,896-byte `FmtSong`. Evidence: `docs/WP2_1_VERIFICATION.md`.

## Important decisions / do not regress

- D005: V0.1 dimensions remain 8 tracks × 64 steps × 16 patterns.
- D016: fixed package identity remains `FM-1_987`.
- D018: core in-memory model is frozen; persistence and p-lock allocator are separate.
- D019: exactly one runtime `FmtSong` lives in `.pool` with deterministic explicit reset.
- D020: transport uses the exact BPM-per-frame rational phase accumulator.
- D021: step 0 dispatch/frame timing and WP2.4 one-lane note semantics are frozen as described above.
- Preserve WP2.1/WP2.2/WP2.3 regression coverage.
- No floating point in firmware DSP and no runtime 64-bit division.
- Physical FM-1 install/boot/audio/panel/update behavior and actual callback execution time remain **NOT TESTED** because hardware is not currently available.

## Next action

Implement only WP2.4 according to issue #12 and D021. Close it only after exact event timing, deterministic audible host evidence, prior regression suites, device safety and measured resource deltas pass. Then make WP2.5 the only READY package without implementing its page-input behavior.

## Last relevant checkpoints

- Phase 1 closeout / Phase 2 branch start: `3ed4b65d7cda15c5831533f153f91e9014a9fde7`
- WP2.1 verified implementation/workflow: `3e922da4a33894bc938e3252e575a2908355f3a3`
- WP2.2 verified implementation/workflow: `e180ac8cb5ba7a68fd3932883c988ad9ed67003b`
- WP2.3 verified implementation/workflow: `c83ee9cd835503eb410dd8110c2049351a645578`
- WP2.3 Actions run: `39988630290`
- WP2.4 start: `64c4b7d260dca2471d7658a9a59bec23a24454a5`

## Last updated

2026-10-23 - WP2.4 started. One-track audible playback is the only active implementation package; WP2.5 has not started.