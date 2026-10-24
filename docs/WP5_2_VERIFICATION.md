# WP5.2 Verification - D050 Curated FM4 Algorithm Routing

Status: **COMPLETE - host + pinned PI32v2/device-build evidence green**  
Date: 2026-10-24  
Package: WP5.2 / issue #45  
Branch: `phase5/fm4-algorithms`  
Normative contract: `WP5_2_FM_ALGORITHM_CONTRACT.md`  
Pre-implementation evidence: `WP5_2_PREIMPLEMENTATION_EVIDENCE.md`

## 1. Canonical implementation verification

Canonical implementation head:

`bc054bc697304afe94fe18b2990396e6a1667dbf`

Canonical workflow:

- run: `40136425976`
- host job: `120998804031`
- target job: `120998804175`
- both jobs: SUCCESS

Artifacts:

- host artifact ID `12576975819`
- host artifact SHA-256 `3df772adb2054409aa65d75396878a4fe69817c7a524025dc3b1d5bbdf4b52fd`
- target artifact ID `12576824809`
- target artifact SHA-256 `0e6f7788fc8f4d5a979b0dde66bd222e0e175735d4d2ce04017a4baa95a34db7`

## 2. Contract-first history

D050 was frozen before production algorithm-routing source at:

`f2d328f516bb66517d1e06d98aab1f3c09dcc68b`

The source-absent contract/reference gate then passed before implementation:

- run `40135979431`
- job `120997461479`
- artifact ID `12577609841`
- artifact SHA-256 `d3465c39b17416d58c56d98c196a18ae1c4dbd6e07d2ab7cc4ca2b3992cab85`

The durable green pre-implementation checkpoint is:

`6ecdb64653dee06411eecd4e73571cf21b11c3d9`

This proves the D050 topology, modulation arithmetic, normalization and reference hashes existed independently of the later C implementation.

## 3. Implemented routing primitive

WP5.2 adds only the isolated FM4 routing layer:

- `firmware/src/fmt_fm_algorithm.h`
- `firmware/src/fmt_fm_algorithm.c`
- `tests/test_wp5_2_fm_algorithm.c`
- `tests/run_wp5_2_fm_algorithm_tests.sh`
- `.github/workflows/wp5_2_fm_algorithm_verify.yml`

`firmware/src/dinghy.c`, `fmt_engine_router.*` and the D049 operator source/data remain unchanged. FM algorithm routing is not linked into audible production playback yet.

Frozen stable algorithms:

```text
0 STACK4:    OP4 -> OP3 -> OP2 -> OP1 -> OUT
1 FAN3:      OP2 + OP3 + OP4 -> OP1 -> OUT
2 DUAL2:     OP4 -> OP3 plus OP2 -> OP1, two carriers
3 PARALLEL4: OP1 + OP2 + OP3 + OP4, four carriers
```

Every algorithm evaluates exactly OP4 -> OP3 -> OP2 -> OP1 and advances all four D049 operators exactly once per sample.

## 4. State and arithmetic evidence

Verified D050 semantics:

- `FmtFmAlgorithmState` = four D049 operators = exactly 16 B;
- no extra mutable routing history;
- reset/retrigger resets all four operator phases;
- null state/increment/level inputs fail safely to 0;
- invalid algorithm IDs sanitize to STACK4;
- algorithm changes do not reset or pause any operator phase;
- internal modulation depth clamps to `0..16383`;
- edge phase mapping is exactly `sample * depth * 4`;
- signed phase offsets accumulate modulo 2^32;
- carrier counts are only 1, 2 or 4;
- two/four-carrier normalization is sign-safe power-of-two truncation toward zero;
- render contains no runtime graph traversal.

Canonical host markers include:

`WP5_2_FM_ALGORITHM_VECTORS_OK=1 algorithms=4 state_bytes=16 order=op4_op3_op2_op1`

`WP5_2_FM_ALGORITHM_SWITCH_OK=1 phase_continuity=exact operators_per_sample=4`

`WP5_2_FM_ALGORITHM_STRESS_OK=1 algorithms=4 renders_each=65536 output_q15=bounded phase_advance=exact`

`WP5_2_FM_ALGORITHM_STATIC_AUDIT_OK=1 render_loops=0 graph_traversal=0 alloc=0 float=0 int64=0 divide=0 modulo=0 store_router_ui=0`

## 5. Frozen deterministic hashes

Exact 4,096-render little-endian int32 output hashes:

- STACK4: `25dfde1bb29de2c70305d6abafee91fd7480e69eaa71fd1a3fbde3c1e6e855cd`
- FAN3: `8d9ae5310369e3d5a36bcf8c44d5e74049d3d6e109362a844be2796b26330653`
- DUAL2: `c4c31cabf4b8e121591c4c1e0923aee67523eaf7d5b60b60ca1a545c13fc2269`
- PARALLEL4: `d316c3e5467cbfc606c788f669a3b19dba13535969dcce3b67b2821f0977a882`

Common final phases after the frozen fixture:

- OP1 `0x4476A040`
- OP2 `0x95C7F080`
- OP3 `0xE71940C0`
- OP4 `0x386A9000`

The production C implementation reproduced all frozen first-12 vectors, all four hashes and the common final phases exactly.

## 6. Broad deterministic stress

Each algorithm passed 65,536 deterministic renders with varying:

- operator phase increments;
- operator levels, including D049 clamp exercise;
- modulation depth across the uint16 input domain.

For all 262,144 renders:

- output remained inside `-32767..32767`;
- every operator phase advanced by exactly its supplied increment;
- no operator was skipped or double-advanced;
- invalid algorithm fallback remained exact STACK4 behavior.

## 7. Isolated PI32v2 object evidence

Pinned JieLi PI32v2 compile passed.

Canonical object SHA-256:

`ed85fe6ed70402bd3b0727e59b1c8eee21f6d15f264ec8dc439603968a38f469`

Measured `.text + .rodata` = **362 B**, against the D050 ceiling of 2,048 B.

Exact sections:

- `.text.fmt_fm_algorithm_reset` = 34 B
- `.text.fmt_fm_algorithm_retrigger` = 34 B
- `.text.fmt_fm_algorithm_sanitize` = 6 B
- `.text.fmt_fm_algorithm_clamp_mod_depth_q14` = 10 B
- `.text.fmt_fm_algorithm_render` = 278 B

There is no router-specific immutable rodata section.

The routing object therefore consumes about 17.7 percent of its isolated 2,048 B ceiling.

Canonical marker:

`WP5_2_PI32V2_OBJECT_OK=1`

## 8. Inherited D049 and WP5.0/D046 regressions

The WP5.2 host gate reran the complete accepted D049 operator gate and inherited WP5.0/D046 host regressions.

Verified unchanged:

- D049 table generator and table SHA;
- D049 single-operator vectors and 4,096-render hash;
- WP5.0 mixed-engine router core;
- D046 representative 8-track / 64-row / 2,048-lock workload;
- representative WAV `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9`;
- normal WAV `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`.

## 9. Full application remains exact WP5.0

Because D050 remains isolated from `dinghy.c`, the complete running application remains byte-identical to the canonical WP5.0 target.

Pinned target:

- image: **72,792 B**
- normal RAM: **19,676 B**
- pool: **113,000 B**
- BIN SHA-256: `a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125`
- FWSC SHA-256: `cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073`
- package identity: `FM-1_987`
- AC79 SDK commit: `d179b4484759423312073f5fbb232501aa491047`

Canonical markers:

`WP5_2_FULL_APP_TARGET_IDENTITY_OK=1`

`WP5_2_TARGET_GATE_OK=1 algorithm=pi32v2 full_app=wp5_0_identical hardware_margin_claim=none`

## 10. Exit result

WP5.2 exit criteria are satisfied at host/device-build level:

- D050 frozen before implementation: PASS
- source-absent reference gate: PASS
- stable algorithm IDs/topologies: PASS
- exact evaluation order: PASS
- deterministic modulation/normalization: PASS
- exact vectors/hashes/final phases: PASS
- broad stress: PASS
- algorithm-switch phase continuity: PASS
- structural realtime/source audit: PASS
- pinned PI32v2 compile: PASS
- router object budget: PASS, 362 B / 2,048 B
- D049 regression: PASS
- WP5.0/D046 host regressions: PASS
- full target identity unchanged: PASS
- durable evidence: PASS

WP5.2 is therefore **COMPLETE**.

WP5.3 may begin contract-first work on integer ratio/pitch mapping. D049 operator semantics and D050 routing semantics remain compatibility baselines.

## 11. Physical hardware boundary

No physical FM-1 was available.

WP5.2 does not claim physical sound quality, install/boot/control behavior or measured AC791N callback margin.

`hardware_margin_claim=none`
