# WP4.5 WAVETABLE-8 Verification

Status: **COMPLETE — host + real PI32v2 + full target-build closeout**
Date: 2026-10-24
Package: WP4.5 / issue #35
Branch: `phase4/8bit-engine`
Normative decision: D041
Normative contract: `docs/WP4_5_WAVETABLE8_CONTRACT.md`

## Result

WP4.5 is complete at host/device-build level.

The isolated WAVETABLE-8 primitive is deterministic, bounded and real-PI32v2 compilable. It remains intentionally disconnected from `dinghy.c`, so the running FM1Tracker host/firmware artifacts remain exactly the frozen WP4.4/WP4.3/WP4.2/WP4.1/Phase 3 baseline.

Physical FM-1 install/audio/timbre/execution-margin verification remains a later integrated hardware gate because no physical FM-1 is currently available.

## Frozen contract checkpoint

- D041 contract commit: `c543c99df84ee304a1db4fb631c30d01dc8607d3`
- D041 decision/status freeze commit: `e797372c92e95d0b7f86030eeb7eabb8a585ddcf`
- D041 documentation-freeze run: `40094963637`
- D041 documentation-freeze job: `120875171230` — **SUCCESS**
- clean pre-source checkpoint: `64c9603795668b002cf2075bfaa72305384eb0fa`

D041 was committed and recorded before WAVETABLE-8 production source was created.

## Production source added

Exactly two new production files were added after the clean D041 pre-source checkpoint:

- `firmware/src/fmt_osc_wavetable8.h`
- `firmware/src/fmt_osc_wavetable8.c`

No pre-existing firmware source changed. In particular:

- `firmware/src/dinghy.c`: **0 delta**
- `fmt_engine_pitch.*`: **0 delta**
- `fmt_osc_pulse.*`: **0 delta**
- `fmt_osc_noise.*`: **0 delta**
- `fmt_osc_triangle.*`: **0 delta**

Therefore WP4.5 introduces no linked runtime/full-app delta.

## D041 behavior proved

Focused tests prove:

- `FmtWavetable8Osc` exactly **4 B**;
- exactly **8 tables × 32 samples × int8 = 256 immutable table bytes**;
- frozen table identities SINE, SAW8, SQUARE8, ORGAN, HOLLOW, NASAL, METAL and DIGI;
- all **256 table samples** are exercised in exact sequential phase order;
- row-major table blob SHA-256 is exactly `f95b092a5106a8b05acb57d2960b0162b48ea42e2f8217b8923b0b7346046aee`;
- no table contains `-128`; raw range is `-127..127`;
- Q15 expansion is exactly `raw * 258`, with extrema `±32766`;
- table selector `0..7` is exact; all malformed selectors `>=8` fall back to SINE/0;
- phase index is exactly `phase >> 27` with no interpolation;
- reset/retrigger phase zero and first sample zero for every table;
- current phase is sampled before D038 phase advance;
- zero increment holds phase/index/output;
- unsigned wrap returns index 31 to index 0 without a special event;
- guard words prove no mutation outside the 4-byte oscillator state;
- identical state/input replay is deterministic;
- all D038 pitch vectors remain unchanged.

Frozen 65,536-sample A4 FNV-1a32 signatures:

```text
SINE    40a7ddab
SAW8    7c509a67
SQUARE8 a573d449
ORGAN   1d6f9fcb
HOLLOW  b8048a69
NASAL   a3661307
METAL   27204689
DIGI    48f2989f
final_phase=dfb90000
```

## Focused verification run

GitHub Actions run `40095129004`:

- host job `120875655356` — **SUCCESS**
- PI32v2 job `120875655151` — **SUCCESS**

Key host evidence:

```text
WP4_5_ISOLATED_SOURCE_BOUNDARY_OK=1 new_modules=2 dinghy_delta=0 prior_engine_delta=0
WP4_5_WAVETABLE8_TESTS_OK=1
state_bytes=4 tables=8 length=32 table_bytes=256 sample=int8 q15_scale=258 interpolation=none
all_table_samples=256 selector_fallback=sine phase_index=top5 phase_order=sample_before_advance
WP4_5_TABLE_DATA_SHA_OK=1 bytes=256 sha256=f95b092a5106a8b05acb57d2960b0162b48ea42e2f8217b8923b0b7346046aee
long_hashes=40a7ddab,7c509a67,a573d449,1d6f9fcb,b8048a69,a3661307,27204689,48f2989f final_phase=dfb90000
WP4_5_WAVETABLE8_FOCUSED_GATE_OK=1
WP4_5_STATIC_AUDIT_OK=1 wavetable_loops=0 alloc=0 float=0 int64=0 store_traversal=0 immutable_table_bytes=256
```

Static audit therefore found no render loop, dynamic allocation, floating point, 64-bit integer arithmetic or tracker/store traversal in the WAVETABLE-8 production module.

## Real PI32v2 object proof

Pinned JieLi toolchain: `jieli-linux-toolchains-20260730.1`.

Exact object SHA-256:

- inherited D038 pitch helper: `6203ee8b5ab68a1000066c25a52b674dbed9a61ff30eb7579bf29a07abe69f20`
- D041 WAVETABLE-8: `4924fc7ff9069d51ab7cf8259af109b32da16758d25504e22b183af7e29c1194`

The pitch object is byte-identical to WP4.2-WP4.4.

Focused target artifact:

- artifact ID `12560861001`
- ZIP SHA-256 `f0b3b4cb3c7fd9fdf7f6d9f73e855d58834cd81163b967a4222e2b382bd95658`
- size `1,774` bytes

## Final WP4.5 closeout

Permanent closeout workflow source checkpoint:

`f32453ab9b7e0a967ea5ade78839c071cdd3f400`

Final GitHub Actions:

- run `40095198147`
- job `120875856198` (`wp4-5-closeout`)
- result **SUCCESS**

Closeout evidence artifact:

- ID `12559813913`
- ZIP SHA-256 `f134aaa40e7f18f2ea54f85512d5b825e6bc6b9eaa645be2077171af4d66127c`
- size `1,120,630` bytes

Key exit markers:

```text
WP4_5_ISOLATED_PRODUCTION_DELTA_OK=1 files=2 linked_delta=0 prior_engine_delta=0
WP4_5_TARGET_OBJECTS_OK=1 pitch_sha=6203ee8b5ab68a1000066c25a52b674dbed9a61ff30eb7579bf29a07abe69f20 wavetable8_sha=4924fc7ff9069d51ab7cf8259af109b32da16758d25504e22b183af7e29c1194
WP4_5_AC79_INPUTS_OK=1
WP4_5_PACKAGE_IDENTITY=FM-1_987
WP4_5_FINAL_RESOURCES image=66960 normal_ram=13296 pool_ram=113000 full_app_delta=0
WP4_5_FINAL_HOST_WAV_SHA256=453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440
WP4_5_FINAL_BIN_SHA256=b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27
WP4_5_FINAL_FWSC_SHA256=7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c
WP4_5_FINAL_PITCH_OBJECT_SHA256=6203ee8b5ab68a1000066c25a52b674dbed9a61ff30eb7579bf29a07abe69f20
WP4_5_FINAL_WAVETABLE8_OBJECT_SHA256=4924fc7ff9069d51ab7cf8259af109b32da16758d25504e22b183af7e29c1194
WP4_5_TABLE_BLOB_SHA256=f95b092a5106a8b05acb57d2960b0162b48ea42e2f8217b8923b0b7346046aee
WP4_5_D041_EXIT_OK=1 wavetable8=isolated deterministic=1 tables=8 samples=256 pi32v2=1 full_app_identical=1 physical_hardware_pending=1
WP4_5_CLOSEOUT_OK=1
```

## Full-app identity remains frozen

Because WAVETABLE-8 remains isolated/unlinked, its 256 B immutable table data exists only in the independently compiled module object and does not alter the complete app.

| Resource | WP4.5 full app | Delta vs WP4.4 |
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

WP4.5 did not pull forward:

- editable/imported wavetable data;
- wavetable morphing/interpolation;
- common envelope;
- final allocator/stealing;
- engine parameter page/p-lock mapping;
- full D032/D033 engine adapter integration;
- proof-sine replacement;
- eight-track benchmark.

## Exit decision

WP4.5 / D041 satisfies its documented exit criteria at host/device-build level and may be closed.

Next package: **WP4.6 — common envelope primitive**. Its exact state, time-domain mapping, gate/retrigger/release semantics and bounded per-sample behavior must be frozen before envelope production source is created.
