# WP2.3 Verification — Integer Transport Clock

Date: 2026-10-23  
State: **COMPLETE**

GitHub issue: #11

## Scope

WP2.3 implements only deterministic transport timing/state for the V0.1 fixed 1/16-note grid. It does not dispatch rows, trigger notes, alter the tracker UI, implement STEP REC/REST, add p-lock behavior, change synth engines or persist transport state.

The final verified implementation/workflow commit is:

`c83ee9cd835503eb410dd8110c2049351a645578`

GitHub Actions:

- run: `39988630290`;
- job: `120534295734` (`integer-transport`);
- result: **SUCCESS**.

## Timing model

D020 freezes the WP2.3 timing formulation.

For 44.1 kHz and a 1/16-note grid:

```text
frames_per_step = 44100 * 15 / BPM
phase threshold = 44100 * 15 = 661500
```

The firmware transport therefore:

- stores a 32-bit phase accumulator;
- adds integer BPM units per audio frame;
- subtracts 661,500 on a step boundary;
- advances exactly one current step at a boundary;
- wraps `63 -> 0`;
- preserves the exact fractional remainder instead of rounding a frame interval.

Firmware transport source contains no `uint64_t`, `int64_t`, `float` or `double`. The real-time transport path uses no division.

## Runtime state

`FmtTransport` is exactly 8 bytes:

```text
uint32_t phase
uint16_t bpm
uint8_t  step
uint8_t  running
```

Reset state is deterministic:

- phase: 0;
- BPM: 120;
- current step: 0;
- running: false.

Stop preserves current step/phase. Start resumes them. BPM 0 is rejected. A valid BPM change preserves phase progress.

The runtime object is linked in ordinary RAM, not the editable-song pool:

```text
01c0b050 g       .bss         00000008 fmt_transport_runtime
```

The existing WP2.2 song store remains unchanged:

```text
01c20000 g       .pool        00010168 fmt_store_storage
```

## Host timing tests

`tests/test_fmt_transport.c` verifies:

- reset/default state and null safety;
- stopped advance is inert;
- deterministic start/stop/resume;
- BPM 0 rejection;
- valid tempo change preserves phase;
- the exact 120 BPM fractional boundary (`5512.5` frames/step) without interval rounding;
- exact step-63 to step-0 wrap;
- bulk advance and frame-by-frame tick produce identical state/boundary counts;
- six one-hour runs at 31, 60, 120, 137, 255 and 997 BPM match an independent host-only 64-bit rational reference exactly;
- one minute at the full 16-bit BPM maximum (`65535`) also matches the reference.

CI marker:

```text
FMT_TRANSPORT_TESTS_OK=1
phase_per_step=661500 default_bpm=120 state_bytes=8 long_run_hours=6 max_bpm=65535
```

The host-only test reference uses 64-bit math only to independently verify the firmware algorithm. The firmware transport implementation itself remains 32-bit.

## Regression tests

The frozen WP2.1 model suite remains green:

```text
FMT_MODEL_TESTS_OK=1
FmtStep=8 FmtPattern=4096 FmtSong=65896 lock_budget=16384 total_budgeted=82280
```

The WP2.2 store suite remains green:

```text
FMT_STORE_TESTS_OK=1
store_bytes=65896 steps=8192 patterns=16 tracks=8 steps_per_pattern=64
```

Host audio remains byte-for-byte unchanged because WP2.3 never starts the transport or dispatches rows:

`dac1695b8e69f26334d548005e6cae505cf39ace297dcf6a57338e70fc386b9b`

## Device build and safety

Pinned external inputs remain unchanged:

- JieLi toolchain package: `jieli-linux-toolchains-20260730.1`;
- clang: 4.0.1;
- AC79 SDK tag: `AC79NN_SDK_V1.2.1_2023-12-13`;
- resolved SDK commit: `d179b4484759423312073f5fbb232501aa491047`;
- consumed SDK package-input hashes remain verified.

Device build result:

```text
loader: image 8232 B at 0x1c0a800
loader: ota.bin 6954 B
ok .ram_text: 901 insns, no calls
ok image 55328 B of 581564; RAM .data+.bss 12376 B of 98304; pool 96616 B of 344064
ok register access: hal/ only (src/, loader/ clean)
package identity FM-1_987
```

## Measured resource delta

Compared with the completed WP2.2 baseline:

| Resource | WP2.2 | WP2.3 | Delta |
|---|---:|---:|---:|
| app image | 55,188 B | **55,328 B** | **+140 B** |
| normal RAM (`.data + .bss`) | 12,368 B | **12,376 B** | **+8 B** |
| pool RAM | 96,616 B | **96,616 B** | **0 B** |

The +8 B normal-RAM delta exactly equals `sizeof(FmtTransport)`. WP2.3 consumes no additional pool RAM.

Remaining resources after WP2.3:

- app image free: 526,236 B;
- normal RAM free: 85,928 B;
- pool RAM free: 247,448 B.

## Verified artifacts

- `fm1tracker.bin` SHA-256: `b1d0489f74393622b6f2d117b497c42e4659300185018ad82b6fdf52648c1e34`;
- `fm1tracker-0.1.fwsc` SHA-256: `22e45c217704468f87479243de14b1b99ad9dfdbe2033103d2dcbca7b1c3cd55`;
- package identity: `FM-1_987`;
- host WAV SHA-256: `dac1695b8e69f26334d548005e6cae505cf39ace297dcf6a57338e70fc386b9b`.

Evidence artifact:

- name: `wp2-3-integer-transport-evidence`;
- artifact ID: `12517195879`;
- ZIP SHA-256: `828d0010c1ffc364fae86f5466ee5c3e2c1b97ac55681510958b4e95c715efec`.

## Exit criteria

- deterministic 0..63 wrap: **PASS**;
- long-run no-drift/error-accumulator verification: **PASS**;
- deterministic reset/start/stop/resume: **PASS**;
- no playback/editor/UI behavior introduced: **PASS**;
- host audio regression unchanged: **PASS**;
- device build/safety gates: **PASS**;
- resource delta recorded: **PASS**.

Physical FM-1 timing remains **NOT TESTED** because hardware is not currently available. WP2.3 does not claim measured callback execution time.

## Closeout

WP2.3 is complete. WP2.4 — one-track audible playback — is next. WP2.4 has not been implemented as part of this checkpoint.
