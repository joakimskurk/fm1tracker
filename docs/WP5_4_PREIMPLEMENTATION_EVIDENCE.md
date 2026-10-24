# WP5.4 Preimplementation Evidence

Status: **ACCEPTED SOURCE-ABSENT GATE**  
Date: 2026-10-25  
Package: WP5.4 / issue #47  
Branch: `phase5/fm4-levels`

## Frozen checkpoint

D052 was frozen before any FM control-mapping production source existed.

- contract commit: `18ea7f9af97e32e3903c7bbf1b2fbeef6dcc762e`
- parent WP5.3 closeout: `ed3a9765ba9e401f4d30f514a357dd0d592e6a7e`
- workflow: `.github/workflows/wp5_4_contract_freeze.yml`
- run: `40140119411`
- job: `121009903030`
- conclusion: `success`
- artifact id: `12577269868`
- artifact name: `wp5-4-d052-contract-freeze`
- artifact digest: `sha256:d147dfd682c10e9400a9f65533ab6cdb2dc9db85b872035fe75495ac799cef03`

## Source-absent proof

At the accepted checkpoint:

- `firmware/src/fmt_fm_controls.h` was absent;
- `firmware/src/fmt_fm_controls.c` was absent;
- no production source referenced `fmt_fm_controls`;
- D049 operator source was unchanged from the WP5.3 closeout;
- D050 algorithm source was unchanged from the WP5.3 closeout;
- D051 ratio source was unchanged from the WP5.3 closeout;
- `firmware/src/dinghy.c` was unchanged from the WP5.3 closeout.

The accepted marker was:

```text
WP5_4_D052_PRODUCTION_ABSENT_OK=1 controls_source=absent d049_delta=0 d050_delta=0 d051_delta=0 dinghy_delta=0
```

## D052 mapping proof

The source-absent gate exhaustively proved the frozen D052 reference behavior before implementation:

- all 256 legal LEVEL byte values map exactly to Q15;
- all 256 legal INDEX byte values map exactly to Q14;
- malformed scalar values above 255 saturate instead of wrapping;
- all 65,536 LEVEL x INDEX combinations remain signed-32 safe through the D049 peak and D050 single-edge arithmetic;
- D050 carrier and modulator role sets are disjoint for all four frozen algorithms;
- the three frozen D051 -> D052 composition vectors are exact.

Frozen reference hashes:

```text
LEVEL payload            a799f70bd763053e79c3d9de6acdda97523fe54632105756c4252dc33d51c815
INDEX payload            cf791163cad612063084d0479cb00b8b86e4e2d19102c461bf548bcca4b69bc7
LEVEL + INDEX payload    26e55d6a5fcacb5f432ecc3620b41a1c18e23f277aea6a13cb72d988a1929b38
edge-safety payload      cf642aee2276abf355a62b7fb0ff1d8ff496e49b9d0a3397a0acc88fff421de0
D051 composition payload e1349b6d834de9655c2417e4a7a5aabdf64bed6138d8026bec10e23a4465486c
```

Worst legal D050 single-edge magnitude:

```text
2147287044
```

Signed-32 positive margin:

```text
196603
```

## Inherited proof

The D052 source-absent gate recursively re-ran the accepted D051 host gate, which in turn re-proved inherited D050, D049 and WP5.0 host behavior.

Final accepted source-absent marker:

```text
WP5_4_D052_CONTRACT_FREEZE_OK=1 production=absent controls=5 level_map=exact index_map=exact edge_safety=pass d051=exact d050=exact d049=exact full_app_wav=wp5_0_identical hardware_margin_claim=none
```

## Permission to implement

This evidence authorizes only the production surface frozen by D052:

```text
firmware/src/fmt_fm_controls.h
firmware/src/fmt_fm_controls.c
```

Implementation must remain isolated from `dinghy.c`, D048 routing and audible FM production. WP5.4 is not complete until focused host verification, PI32v2 object verification, inherited regressions and exact WP5.0 full-application target identity are all green.

No physical FM-1 was available. `hardware_margin_claim=none`.
