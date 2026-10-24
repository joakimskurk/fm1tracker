# WP4.4 TRIANGLE Verification

Status: **COMPLETE — host + real PI32v2 + full target-build closeout**
Date: 2026-10-24
Package: WP4.4 / issue #34
Branch: `phase4/8bit-engine`
Normative decision: D040
Normative contract: `docs/WP4_4_TRIANGLE_CONTRACT.md`

## Result

WP4.4 is complete at host/device-build level.

The isolated TRIANGLE primitive is deterministic, bounded and real-PI32v2 compilable. It remains intentionally disconnected from `dinghy.c`, so the running FM1Tracker host/firmware artifacts remain exactly the frozen WP4.3/WP4.2/WP4.1/Phase 3 baseline.

Physical FM-1 install/audio/spectral-character/execution-margin verification remains a later integrated hardware gate because no physical FM-1 is currently available.

## Frozen contract checkpoint

- D040 contract commit: `79167dd9e76fdac09801fe8fc59bb7c9bb94a72e`
- D040 decision/status freeze commit: `a53db6454ee7bc7129bae4859b5a484ff3474a45`
- D040 documentation-freeze run: `40094239423` — **SUCCESS**
- clean pre-source checkpoint: `c592af611b28b0ae10ce9c277b9276f3efa350c5`

D040 was committed and recorded before TRIANGLE production source was created.

## Production source added

Exactly two new production files were added after the clean D040 pre-source checkpoint:

- `firmware/src/fmt_osc_triangle.h`
- `firmware/src/fmt_osc_triangle.c`

No pre-existing firmware source changed. In particular:

- `firmware/src/dinghy.c`: **0 delta**
- `fmt_engine_pitch.*`: **0 delta**
- `fmt_osc_pulse.*`: **0 delta**
- `fmt_osc_noise.*`: **0 delta**

Therefore WP4.4 introduces no linked runtime/full-app delta.

## D040 behavior proved

Focused tests prove:

- `FmtTriangleOsc` exactly **4 B**;
- reset/retrigger phase `0` and first sample exactly `0`;
- direct reuse of D038 pitch increment without parallel pitch math;
- exact mapping from quadrant and 15-bit quarter-slope offset;
- canonical quarter-cycle points exactly `0, +32767, 0, -32767`;
- eighth-cycle vector exactly `0,+16384,+32767,+16383,0,-16384,-32767,-16383`;
- sample-before-advance ordering;
- exact half-cycle antisymmetry;
- output always inside `-32767..32767`;
- all **131,072 quantized phase bins** exercised across the 65,536 high-16 phase buckets and both bit-15 states;
- zero increment holds phase/output;
- unsigned phase wrap remains deterministic;
- guard words prove no mutation outside the 4-byte oscillator state;
- identical state/input replay is deterministic;
- all D038 pitch vectors remain unchanged.

Frozen 65,536-sample A4 signature:

```text
pitch_q7=8832
phase_increment=42852281
final_phase=dfb90000
FNV-1a32=bc0f75ed
```

## Focused verification run

GitHub Actions run `40094383287`:

- host job `120873465692` — **SUCCESS**
- PI32v2 job `120873465390` — **SUCCESS**

Host gate proved:

```text
WP4_4_ISOLATED_SOURCE_BOUNDARY_OK=1 new_modules=2 dinghy_delta=0 prior_engine_delta=0
WP4_4_TRIANGLE_TESTS_OK=1
state_bytes=4 q15=-32767..32767 canonical=0,+32767,0,-32767 phase_order=sample_before_advance
quantized_phase_bins=131072 half_cycle_antisymmetry=1
long_hash_fnv1a32=bc0f75ed samples=65536 pitch_q7=8832 final_phase=dfb90000
WP4_4_TRIANGLE_FOCUSED_GATE_OK=1
WP4_4_STATIC_AUDIT_OK=1 triangle_loops=0 alloc=0 float=0 int64=0 store_traversal=0
```

Static audit therefore found no render loop, dynamic allocation, floating point, 64-bit integer arithmetic or tracker/store traversal in the TRIANGLE production module.

## Real PI32v2 object proof

Pinned JieLi toolchain: `jieli-linux-toolchains-20260730.1`.

Exact object SHA-256:

- inherited D038 pitch helper: `6203ee8b5ab68a1000066c25a52b674dbed9a61ff30eb7579bf29a07abe69f20`
- D040 TRIANGLE: `c63be23d052c4dae44da6f12bf5c3511bc36e443d8b964720874107815eaa3d3`

The pitch object is byte-identical to WP4.2/WP4.3.

Focused target artifact:

- artifact ID `12560421748`
- ZIP SHA-256 `f89910e027ec6508857b4de2402152213e078f70af8b2920f3752a5740dc79f8`

## Final WP4.4 closeout

Permanent closeout workflow source checkpoint:

`7f9469adef0983d92884c7907d254d6c9c50538b`

Final GitHub Actions:

- run `40094496353`
- job `120873805827` (`wp4-4-closeout`)
- result **SUCCESS**

Closeout evidence artifact:

- ID `12560555371`
- ZIP SHA-256 `322c8619bb94d70e87d1ffb8ce8ca44b0cbe6bd6cf6d19574c1f07152dc3e51a`
- size `1,120,028` bytes

Key exit markers:

```text
WP4_4_ISOLATED_PRODUCTION_DELTA_OK=1 files=2 linked_delta=0 prior_engine_delta=0
WP4_4_TARGET_OBJECTS_OK=1 pitch_sha=6203ee8b5ab68a1000066c25a52b674dbed9a61ff30eb7579bf29a07abe69f20 triangle_sha=c63be23d052c4dae44da6f12bf5c3511bc36e443d8b964720874107815eaa3d3
WP4_4_AC79_INPUTS_OK=1
WP4_4_PACKAGE_IDENTITY=FM-1_987
WP4_4_FINAL_RESOURCES image=66960 normal_ram=13296 pool_ram=113000 full_app_delta=0
WP4_4_FINAL_HOST_WAV_SHA256=453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440
WP4_4_FINAL_BIN_SHA256=b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27
WP4_4_FINAL_FWSC_SHA256=7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c
WP4_4_FINAL_PITCH_OBJECT_SHA256=6203ee8b5ab68a1000066c25a52b674dbed9a61ff30eb7579bf29a07abe69f20
WP4_4_FINAL_TRIANGLE_OBJECT_SHA256=c63be23d052c4dae44da6f12bf5c3511bc36e443d8b964720874107815eaa3d3
WP4_4_D040_EXIT_OK=1 triangle=isolated deterministic=1 exhaustive_bins=131072 pi32v2=1 full_app_identical=1 physical_hardware_pending=1
WP4_4_CLOSEOUT_OK=1
```

## Full-app identity remains frozen

Because TRIANGLE remains isolated/unlinked, the complete app remains exactly:

| Resource | WP4.4 | Delta vs WP4.3 |
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

WP4.4 did not pull forward:

- WAVETABLE-8;
- common envelope;
- final allocator/stealing;
- engine parameter page/p-lock mapping;
- full D032/D033 engine adapter integration;
- proof-sine replacement;
- eight-track benchmark.

## Exit decision

WP4.4 / D040 satisfies its documented exit criteria at host/device-build level and may be closed.

Next package: **WP4.5 — WAVETABLE-8**. Its exact table count/length/sample format/index/interpolation/editability/test contract must be frozen before WAVETABLE-8 production source is created.
