# WP5.1 Verification - D049 Fixed-Point FM Operator Primitive

Status: **COMPLETE - host + pinned PI32v2/device-build evidence green**  
Date: 2026-10-24  
Package: WP5.1 / issue #44  
Branch: `phase5/fm4-operator`  
Normative contract: `WP5_1_FM_OPERATOR_CONTRACT.md`  
Pre-implementation evidence: `WP5_1_PREIMPLEMENTATION_EVIDENCE.md`

## 1. Canonical verification

Canonical implementation head:

`52df09bf363ad5a9ad2d722a38734eaa580ecb90`

Canonical workflow:

- run: `40134823600`
- host job: `120994003059`
- target job: `120994002842`
- both jobs: SUCCESS

Artifacts:

- host artifact ID `12576217981`
- host artifact SHA-256 `dec781b3af375c25860bbfec9bb60b2ce88b08c1639fe50d731fabc3111d1b81`
- target artifact ID `12576337348`
- target artifact SHA-256 `c6dc617b2a448e5f0304319874a44d08565294a90a9fd0501667f92fc6a67cb7`

The first target attempt failed only because the workflow had not installed Pillow before the unchanged full-app build. The isolated PI32v2 operator compile was already green. Commit `52df09bf...` adds that CI dependency only; no firmware source changed in the fix.

## 2. Contract-first history

D049 was frozen before production operator source at commit:

`f9b97edeb66483199fdf5bbfc964839f31841dc0`

The source-absent reference gate then passed before implementation:

- run `40131135259`
- job `120982895786`
- head `010d1dff2f74e27450edc6784ae7792eca358e59`
- artifact `12574973209`
- artifact SHA-256 `ae74fcd6272eafffdf62a27ef0f9fd415941f1c8024901c4344cb19a0212f83b`

The durable green pre-implementation checkpoint is:

`b01a70f94b91dcc4d7afffaff7c30b4d4486eb2a`

This proves the frozen mathematics existed independently of the later C implementation.

## 3. Implemented primitive

WP5.1 adds only the isolated FM operator layer:

- `tools/gen_wp5_1_fm_sine.py`
- `firmware/gen/fmt_fm_sine_q15.inc`
- `firmware/src/fmt_fm_operator.h`
- `firmware/src/fmt_fm_operator.c`
- `tests/test_wp5_1_fm_operator.c`
- `tests/run_wp5_1_fm_operator_tests.sh`
- `.github/workflows/wp5_1_fm_operator_verify.yml`

`firmware/src/dinghy.c` and `firmware/src/fmt_engine_router.c` remain unchanged from the WP5.0 application path. The FM operator is not linked into audible production playback yet.

Frozen/verified primitive semantics:

- `FmtFmOperator` = exactly one `uint32_t phase`, 4 B;
- one full cycle = 2^32 phase units;
- unsigned 32-bit overflow is phase wrap;
- current carrier phase is sampled before phase advance;
- signed `int32_t` phase modulation is added modulo 2^32;
- operator level clamps to Q15 `0..32767`;
- output domain is `-32767..32767`;
- carrier uses a 257-entry positive quarter-wave `int16_t` Q15 table;
- table payload = 514 B;
- table interpolation uses 22 fractional bits;
- no loop, heap, float/double, 64-bit arithmetic, divide, modulo, song/store/router traversal in the primitive render path.

## 4. Generator and deterministic data identity

The offline generator reproduces the checked-in table byte-for-byte.

Frozen table binary SHA-256:

`6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68`

Canonical host markers include:

`WP5_1_FM_SINE_GENERATOR_OK=1 entries=257 payload_bytes=514 sha256=6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68`

`WP5_1_GENERATED_TABLE_IDENTITY_OK=1 checked_in=generator_exact payload_bytes=514`

## 5. Focused D049 host vectors

Canonical host verification passed:

- exact sine anchor/non-node vectors;
- 65,536 half-cycle sign-symmetry probes;
- exact positive/negative and `INT32_MIN`/`INT32_MAX` modulation vectors;
- level clamp and sign-mirror vectors;
- reset/retrigger/null safety;
- sample-before-advance ordering;
- unsigned phase wrap;
- zero-increment hold;
- deterministic 4,096-render vector.

Canonical markers:

`WP5_1_FM_OPERATOR_VECTORS_OK=1 state_bytes=4 symmetry_probes=65536`

`WP5_1_FM_OPERATOR_RENDER_OK=1 sample_before_advance=1 modulation=full_int32 level_q15=0..32767`

`WP5_1_FM_OPERATOR_LONG_OK=1 samples=4096 final_phase=f1532bdf`

Frozen 4,096-render SHA-256:

`499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66`

Static source gate:

`WP5_1_FM_OPERATOR_STATIC_AUDIT_OK=1 render_loops=0 alloc=0 float=0 int64=0 divide=0 modulo=0 store_router_access=0`

## 6. Isolated PI32v2 object evidence

Pinned JieLi toolchain compile passed.

Canonical object SHA-256:

`46868461ac6910d499975dbd93d06ee0af9db905d01c32583321eeaea0e33fad`

Measured `.text + .rodata` = **692 B**, against the D049 ceiling of 4,096 B.

Exact sections:

- `.text.fmt_fm_operator_reset` = 8 B
- `.text.fmt_fm_operator_retrigger` = 8 B
- `.text.fmt_fm_operator_clamp_level_q15` = 10 B
- `.text.fmt_fm_operator_sine_q15` = 76 B
- `.text.fmt_fm_operator_render` = 76 B
- `.rodata.fmt_fm_sine_quarter_q15` = 514 B

Marker:

`WP5_1_PI32V2_OBJECT_OK=1`

The operator is therefore only 16.9 percent of its isolated 4,096 B ceiling.

## 7. Full application remains exact WP5.0

Because WP5.1 is intentionally isolated from `dinghy.c`, the complete running application must remain byte-identical to WP5.0.

Host normal WAV remains:

`17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`

D046 representative workload remains:

`33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9`

Inherited representative evidence still proves:

- 8 tracks x 64 rows;
- 2,048 locks;
- four 8-BIT engines;
- 4,096 rows / 64 wraps;
- deterministic repeat identity;
- zero active cross-track primary steals;
- queue overflow/recovery remains bounded.

Pinned full target remains exactly:

- image: **72,792 B**
- normal RAM: **19,676 B**
- pool: **113,000 B**
- BIN SHA-256: `a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125`
- FWSC SHA-256: `cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073`
- package identity: `FM-1_987`
- AC79 SDK commit: `d179b4484759423312073f5fbb232501aa491047`

Canonical marker:

`WP5_1_TARGET_GATE_OK=1 operator=pi32v2 full_app=wp5_0_identical hardware_margin_claim=none`

## 8. Exit criteria result

WP5.1 exit criteria are satisfied at host/device-build level:

- D049 frozen before implementation: PASS
- deterministic generated carrier data: PASS
- exact operator state/formats: PASS
- exact host vectors and long hash: PASS
- reset/null/wrap/modulation/level behavior: PASS
- realtime/source structural guards: PASS
- pinned PI32v2 isolated compile: PASS
- isolated object budget: PASS, 692 B / 4,096 B
- inherited D049/WP5.0/D046 regressions: PASS
- full application identity unchanged: PASS
- durable verification evidence: PASS

WP5.1 is therefore **COMPLETE**.

WP5.2 may now begin contract-first work on curated four-operator algorithm routing. D049 remains the primitive compatibility baseline.

## 9. Physical hardware boundary

No physical FM-1 was available.

WP5.1 does not claim:

- physical audio quality;
- measured AC791N callback margin;
- physical install/boot/control/display behavior.

`hardware_margin_claim=none`
