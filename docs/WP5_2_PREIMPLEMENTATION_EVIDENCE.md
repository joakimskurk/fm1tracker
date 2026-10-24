# WP5.2 Pre-Implementation Evidence - D050 Curated FM4 Algorithm Routing

Status: **PASS - production algorithm routing is now allowed under D050**  
Date: 2026-10-24  
Package: WP5.2 / issue #45  
Branch: `phase5/fm4-algorithms`  
Normative contract: `WP5_2_FM_ALGORITHM_CONTRACT.md`

## Contract checkpoint

D050 was frozen before any production `fmt_fm_algorithm.*` source at commit:

`f2d328f516bb66517d1e06d98aab1f3c09dcc68b`

The compare from WP5.1 closeout `bc71289690c8f9bd36e9cbbb378e0df3de1b9f25` to the D050 checkpoint contained only documentation, test harness and workflow changes. `firmware/`, `firmware/gen` and `tools/` were unchanged.

## Canonical source-absent gate

- run: `40135979431`
- job: `120997461479`
- head: `f2d328f516bb66517d1e06d98aab1f3c09dcc68b`
- conclusion: SUCCESS
- artifact: `wp5-2-d050-contract-freeze`
- artifact ID: `12577609841`
- artifact SHA-256: `d3465c39b17416d58c56d98c196a18ae1c4dbd6e07d2ab7cc4ca2b3992cab85`

## Proven before implementation

The gate proved:

- production `firmware/src/fmt_fm_algorithm.h/.c` were absent;
- D049 operator source/data were unchanged from WP5.1 closeout;
- exactly four stable algorithms were reproduced;
- every algorithm used fixed OP4 -> OP3 -> OP2 -> OP1 evaluation order;
- every algorithm advanced all four D049 operators exactly once per sample;
- internal modulation depth `0..16383` and `sample * depth * 4` anchors passed;
- signed multi-modulator phase offsets accumulated modulo 2^32 exactly;
- one/two/four-carrier normalization anchors passed with sign-safe truncation toward zero;
- invalid algorithm IDs reproduced STACK4 output and state exactly;
- exact frozen first-12 vectors passed for all algorithms;
- exact 4,096-render hashes and common final phases passed;
- 65,536 renders per algorithm kept output inside `-32767..32767` and phase advancement exact;
- the accepted D049 production implementation remained exact;
- WP5.0 router and D046 representative host regressions remained green;
- normal host WAV remained exact WP5.0.

Canonical markers:

`WP5_2_D050_PRODUCTION_ABSENT_OK=1 algorithm_source=absent d049_delta=0 dinghy_delta=0`

`WP5_2_D050_REFERENCE_TOPOLOGIES_OK=1 algorithms=4 order=op4_op3_op2_op1 operators_per_sample=4`

`WP5_2_D050_MODULATION_OK=1 depth=0..16383 accumulation=mod2p32 carriers=1_2_4 normalization=pow2_toward_zero`

`WP5_2_D050_HASHES_OK=1 stack4=25dfde1bb29de2c70305d6abafee91fd7480e69eaa71fd1a3fbde3c1e6e855cd fan3=8d9ae5310369e3d5a36bcf8c44d5e74049d3d6e109362a844be2796b26330653 dual2=c4c31cabf4b8e121591c4c1e0923aee67523eaf7d5b60b60ca1a545c13fc2269 parallel4=d316c3e5467cbfc606c788f669a3b19dba13535969dcce3b67b2821f0977a882 final_phases=4476a040,95c7f080,e71940c0,386a9000`

`WP5_2_D050_STRESS_OK=1 algorithms=4 renders_each=65536 phase_advance=exact output_q15=bounded invalid_fallback=stack4`

`WP5_2_D050_CONTRACT_FREEZE_OK=1 production=absent d049=unchanged algorithms=4 reference=pass full_app_wav=wp5_0_identical hardware_margin_claim=none`

## Frozen algorithm identities

- `0 STACK4`: OP4 -> OP3 -> OP2 -> OP1 -> OUT
- `1 FAN3`: OP2 + OP3 + OP4 -> OP1 -> OUT
- `2 DUAL2`: OP4 -> OP3 plus OP2 -> OP1, two carriers
- `3 PARALLEL4`: OP1 + OP2 + OP3 + OP4, four carriers

Frozen 4,096-render SHA-256 values:

- STACK4 `25dfde1bb29de2c70305d6abafee91fd7480e69eaa71fd1a3fbde3c1e6e855cd`
- FAN3 `8d9ae5310369e3d5a36bcf8c44d5e74049d3d6e109362a844be2796b26330653`
- DUAL2 `c4c31cabf4b8e121591c4c1e0923aee67523eaf7d5b60b60ca1a545c13fc2269`
- PARALLEL4 `d316c3e5467cbfc606c788f669a3b19dba13535969dcce3b67b2821f0977a882`

Common final phases:

- OP1 `0x4476A040`
- OP2 `0x95C7F080`
- OP3 `0xE71940C0`
- OP4 `0x386A9000`

## Implementation permission

The D050 source-absent gate is green. WP5.2 may now add the isolated production algorithm-routing primitive, provided it matches D050 exactly and remains unlinked from `dinghy.c`.

WP5.3+ ratios, final level/mod-index mappings, feedback, envelopes, voice/router integration, parameter pages and audible mixed FM remain forbidden in WP5.2.

No physical FM-1 was available. `hardware_margin_claim=none`.
