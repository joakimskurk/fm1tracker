# WP4.8 Verification — D044 Four-Parameter Engine Pages + P-Lock Mapping

Date: 2026-10-24  
Package: WP4.8 / issue #39  
Branch: `phase4/8bit-engine`  
Decision: D044  
Normative contract: `docs/WP4_8_PARAMETER_PAGE_CONTRACT.md`

## Result

**WP4.8 is COMPLETE at isolated host/device-build level.**

D044 now has a deterministic isolated implementation that freezes the six semantic 8-BIT parameter identities, exact four-slot primary page for each current engine, normalized PULSE/WAVETABLE mappings and bounded row-local p-lock precedence without changing the running FM1Tracker firmware.

The parameter layer remains deliberately disconnected from `dinghy.c`, the linked D027 registry remains unchanged, no global 2048-byte row cache is allocated yet, and the Phase 3 proof sine remains the running audio path. Those deliberate linked changes belong to WP4.9.

## Production delta

Exactly two production files were added relative to the clean D044 pre-source checkpoint `5f46f94dc477ac0625417baec9dbfba0d3cb7877`:

- `firmware/src/fmt_8bit_params.h`
- `firmware/src/fmt_8bit_params.c`

No production delta was permitted in:

- `dinghy.c`;
- `fmt_param_registry.*`;
- D038 PULSE;
- D039 NOISE;
- D040 TRIANGLE;
- D041 WAVETABLE-8;
- D042 envelope;
- D043 allocator.

## Frozen implementation properties

Verified properties include:

- six D044 semantic IDs `0x20..0x25`;
- `Fmt8BitParamSet` exactly 6 B;
- bounded lock snapshot exactly 4 B;
- future `Fmt8BitRowParamCache` type exactly 2048 B;
- exactly four page slots for each supported engine;
- PULSE page `WIDTH/DECAY/SUSTAIN/RELEASE`;
- NOISE page `ATTACK/DECAY/SUSTAIN/RELEASE`;
- TRIANGLE page `ATTACK/DECAY/SUSTAIN/RELEASE`;
- WAVETABLE-8 page `TABLE/DECAY/SUSTAIN/RELEASE`;
- default page words `48ff0080`, `48ff0018`, `48ff0018`, `48ff0000`;
- all 256 normalized PULSE width values map monotonically into `32..224`;
- all 256 WAVETABLE values map exactly into eight 32-value selector bands;
- at most four row-lock snapshots are inspected;
- applicable `>255` values fall back to base without truncation;
- duplicate applicable IDs or `lock_count > 4` force complete structural fallback;
- unrelated/common/other-engine locks do not alter the active engine page;
- unsupported engines fail safely;
- linked D027 registry activation remains deferred to WP4.9;
- no global row-cache allocation exists in WP4.8.

## Focused deterministic verification

Permanent workflow:

- `.github/workflows/wp4_8_params_verify.yml`

Run:

- run `40104638904`
- host job `120903675437` — **SUCCESS**
- PI32v2 job `120903675295` — **SUCCESS**

Focused host evidence:

```text
WP4_8_REPLAY_FNV1A32=635311c5
WP4_8_D044_TESTS_OK=1
ids=6 base_bytes=6 lock_snapshot_bytes=4 cache_type_bytes=2048 page_slots=4
width_vectors=256 table_vectors=256 exhaustive_engine_slot_values=4096 max_lock_inspections=4
WP4_8_D044_FOCUSED_GATE_OK=1
```

The exhaustive suite covers 4 engines × 4 slots × all 256 byte values = 4096 exact resolver cases in addition to explicit fallback, duplicate, unsupported-engine, guard and registry-isolation tests.

Static audit proved:

```text
loops=3
max_lock_count=4
allocation=0
float=0
int64=0
divide/modulo=0
store_traversal=0
registry_activation=0
cache_global=0
```

Real pinned JieLi PI32v2 object:

```text
fmt_8bit_params.o SHA-256
cdee1d80274ed47a50391a69d0904a8cc28876c8cfd1e79b33374c7c4e6361f7
```

Focused target artifact:

- artifact ID `12564461209`
- artifact ZIP SHA-256 `9226c656891c5f33e671da5108c93da7eb4db4a62bdd7f04e68cca1ea4be3f28`

## Full closeout

Permanent workflow:

- `.github/workflows/wp4_8_closeout_verify.yml`
- `tests/run_wp4_8_closeout_ci.sh`

Canonical final run:

- run `40104773397`
- job `120904075249` — **SUCCESS**

Canonical closeout artifact:

- artifact ID `12564411814`
- size `1,120,160 B`
- artifact ZIP SHA-256 `a7fe658af8f5f857159c128e9e5d9832af240be3209498ef6bce972e990ede76`

An earlier closeout run `40104723586` / job `120903925381` also passed with the same D044 object hash, application hashes and resource counts. The canonical rerun above is the final reference.

The final gate reported:

```text
WP4_8_ISOLATED_PRODUCTION_DELTA_OK=1 files=2 linked_delta=0 registry_delta=0 prior_engine_delta=0
WP4_8_TARGET_OBJECT_OK=1 params_sha=cdee1d80274ed47a50391a69d0904a8cc28876c8cfd1e79b33374c7c4e6361f7
WP4_8_D044_REPLAY_HASH=635311c5
WP4_8_D044_EXIT_OK=1 params=isolated deterministic=1 ids=6 page_slots=4 cache_type_bytes=2048 registry_activation=0 pi32v2=1 full_app_identical=1 physical_hardware_pending=1
WP4_8_CLOSEOUT_OK=1
```

## Full-app identity

Because D044 remains isolated, the complete running application is still exactly the WP4.7/WP4.6 baseline:

```text
image          66,960 B
normal RAM     13,296 B
pool RAM      113,000 B
package        FM-1_987
```

Host WAV:

```text
453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440
286,464 frames
```

Firmware BIN:

```text
b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27
```

FWSC:

```text
7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c
```

Resource deltas versus WP4.7 are exactly:

```text
image       0 B
normal RAM  0 B
pool RAM    0 B
```

## Hardware limitation

No physical FM-1 is currently available. This checkpoint proves deterministic host behavior, real PI32v2 compilation and a pinned AC79/JieLi firmware build. It does not claim physical install/boot/audio behavior, hardware sound quality or measured on-device execution margin.

## Exit status

WP4.8 exit criteria are satisfied:

- D044 frozen before source — PASS;
- exact IDs/layouts/defaults — PASS;
- exhaustive WIDTH/TABLE mappings — PASS;
- bounded p-lock precedence/fallback — PASS;
- replay/guards/input immutability — PASS;
- static boundedness/isolation audit — PASS;
- real pinned PI32v2 compile — PASS;
- complete pinned AC79 target build — PASS;
- registry activation still deferred — PASS;
- no global cache allocation — PASS;
- full-app byte identity — PASS;
- physical FM-1 verification — PENDING LATER HARDWARE GATE.

## Next package

WP4.9 deliberately crosses the isolation boundary. Before linked integration code, freeze the integration contract for:

- activation of the six D044 IDs in the linked registry;
- allocation/ownership of the D044 2048 B row cache and instrument/base state;
- D038-D043 engine voice state and dispatch;
- D031/D032 final pitch/level plus D033 NOTE/RETRIG/CUT timing;
- D042 envelope NOTE/TIE/OFF/RETRIG behavior;
- D043 allocation/release-tail ownership;
- proof-sine migration and deterministic mixed-engine audio fixtures;
- linked image/RAM/realtime ceilings.

Do not move to the final eight-track benchmark/Phase 4 closeout until WP4.9 integrated host and target evidence is green.
