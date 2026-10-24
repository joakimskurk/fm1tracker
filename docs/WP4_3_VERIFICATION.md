# WP4.3 NOISE Verification

Status: **COMPLETE — host + real PI32v2 + full target-build closeout**
Date: 2026-10-24
Package: WP4.3 / issue #33
Branch: `phase4/8bit-engine`
Normative decision: D039
Normative contract: `docs/WP4_3_NOISE_CONTRACT.md`

## Result

WP4.3 is complete at host/device-build level.

The isolated NOISE primitive is deterministic, bounded and real-PI32v2 compilable. It remains intentionally disconnected from `dinghy.c`, so the running FM1Tracker host/firmware artifacts remain exactly the frozen WP4.2/WP4.1/Phase 3 baseline.

Physical FM-1 install/audio/spectral-character/execution-margin verification remains a later integrated hardware gate because no physical FM-1 is currently available.

## Frozen contract checkpoint

- D039 contract commit: `9cf3fd7d0a227c9555c34c98e37154ea038ff4cb`
- D039 documentation-freeze run: `40093443893`
- D039 documentation-freeze job: `120870682189` — **SUCCESS**
- clean pre-source checkpoint: `eb396e0af0b53f44a99c06a44aa4f085e0a99f4e`

D039 was committed and recorded before NOISE production source was created.

## Production source added

Exactly two new production files were added after the clean D039 pre-source checkpoint:

- `firmware/src/fmt_osc_noise.h`
- `firmware/src/fmt_osc_noise.c`

No pre-existing firmware source changed. In particular:

- `firmware/src/dinghy.c`: **0 delta**
- `fmt_engine_pitch.*`: **0 delta**
- `fmt_osc_pulse.*`: **0 delta**

Therefore WP4.3 introduces no linked runtime/full-app delta.

## D039 behavior proved

Focused tests prove:

- `FmtNoiseOsc` exactly **8 B**;
- reset/retrigger phase `0`, seed `0x5A3D`;
- exact 15-bit right-shift recurrence with `bit0 XOR bit1` feedback into bit14;
- exact nonzero LFSR period **32,767**;
- current-state bit0 sampled before state advance;
- exact output `+32767` / `-32767`;
- zero clock holds phase/LFSR/output;
- each audio render advances the LFSR **zero or one time only**;
- clock is D038 pitch increment ×16 with unsigned saturation, never wraparound;
- clock mapping is monotonic nondecreasing over legal Q7 `0..16383`;
- frozen half-clock sign vector `++--++++++++----`;
- guard words prove no mutation outside the 8-byte oscillator state;
- identical state/input replay is deterministic.

Frozen 65,536-sample A4 signature:

```text
pitch_q7=8832
pitch_inc=42852281
noise_clock_inc=685636496
final_phase=fb900000
final_lfsr=5dc1
FNV-1a32=f50385b6
```

## Focused verification run

GitHub Actions run `40093607376`:

- host job `120871167653` — **SUCCESS**
- PI32v2 job `120871167611` — **SUCCESS**

Host gate proved:

```text
WP4_3_ISOLATED_SOURCE_BOUNDARY_OK=1 new_modules=2 dinghy_delta=0 pulse_pitch_delta=0
WP4_3_NOISE_TESTS_OK=1
state_bytes=8 seed=5a3d lfsr_period=32767 output=bit0_q15 max_steps_per_frame=1
clock=pitch_inc_x16_saturating q7=0..16383 half_clock_vector=++--++++++++----
long_hash_fnv1a32=f50385b6 samples=65536 pitch_q7=8832 final_phase=fb900000 final_lfsr=5dc1
WP4_3_NOISE_FOCUSED_GATE_OK=1
WP4_3_STATIC_AUDIT_OK=1 noise_loops=0 alloc=0 float=0 int64=0 store_traversal=0 max_lfsr_steps_per_frame=1
```

Static audit therefore found no render loop, dynamic allocation, floating point, 64-bit integer arithmetic or tracker/store traversal in the NOISE production module.

## Real PI32v2 object proof

Pinned JieLi toolchain: `jieli-linux-toolchains-20260730.1`.

Exact object SHA-256:

- inherited D038 pitch helper: `6203ee8b5ab68a1000066c25a52b674dbed9a61ff30eb7579bf29a07abe69f20`
- D039 NOISE: `7620be3ab279f664201a26e4b49dfadf52a9a1391c777cc96909e1e78a2c1fd7`

The pitch object is byte-identical to WP4.2.

Focused target artifact:

- artifact ID `12559373364`
- ZIP SHA-256 `7a8e9aaa593df417ef745a86c9ff2b3e0d04d1da8532493d4e49f7740f241650`

## Final WP4.3 closeout

Permanent closeout workflow source checkpoint:

`bc06227071f6c5de6f54222a28d86d1273b66d24`

Final GitHub Actions:

- run `40093740514`
- job `120871569195` (`wp4-3-closeout`)
- result **SUCCESS**

Closeout evidence artifact:

- ID `12559858213`
- ZIP SHA-256 `04cb3a9d66aa98c0b528d1eeb68a036889bf0e877cc3907995a5228c90946f2c`
- size `1,120,004` bytes

Key exit markers:

```text
WP4_3_ISOLATED_PRODUCTION_DELTA_OK=1 files=2 linked_delta=0 pulse_pitch_delta=0
WP4_3_TARGET_OBJECTS_OK=1 pitch_sha=6203ee8b5ab68a1000066c25a52b674dbed9a61ff30eb7579bf29a07abe69f20 noise_sha=7620be3ab279f664201a26e4b49dfadf52a9a1391c777cc96909e1e78a2c1fd7
WP4_3_AC79_INPUTS_OK=1
WP4_3_PACKAGE_IDENTITY=FM-1_987
WP4_3_FINAL_RESOURCES image=66960 normal_ram=13296 pool_ram=113000 full_app_delta=0
WP4_3_FINAL_HOST_WAV_SHA256=453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440
WP4_3_FINAL_BIN_SHA256=b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27
WP4_3_FINAL_FWSC_SHA256=7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c
WP4_3_D039_EXIT_OK=1 noise=isolated deterministic=1 period32767=1 pi32v2=1 full_app_identical=1 physical_hardware_pending=1
WP4_3_CLOSEOUT_OK=1
```

## Full-app identity remains frozen

Because NOISE remains isolated/unlinked, the complete app remains exactly:

| Resource | WP4.3 | Delta vs WP4.2 |
|---|---:|---:|
| app image | 66,960 B | 0 B |
| normal RAM | 13,296 B | 0 B |
| pool RAM | 113,000 B | 0 B |

Artifacts remain byte-identical:

- host WAV SHA-256 `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`
- `fm1tracker.bin` SHA-256 `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27`
- `fm1tracker-0.1.fwsc` SHA-256 `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`
- package identity `FM-1_987`

## Scope discipline

WP4.3 did not pull forward:

- alternate/short-period NOISE mode;
- TRIANGLE;
- WAVETABLE-8;
- common envelope;
- final allocator/stealing;
- engine parameter page/p-lock mapping;
- full D032/D033 engine adapter integration;
- proof-sine replacement;
- eight-track benchmark.

## Exit decision

WP4.3 / D039 satisfies its documented exit criteria at host/device-build level and may be closed.

Next package: **WP4.4 — TRIANGLE**. Its exact phase/sample mapping and deterministic test contract must be frozen before TRIANGLE production source is created.
