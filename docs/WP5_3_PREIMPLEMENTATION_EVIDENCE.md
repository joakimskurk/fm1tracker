# WP5.3 Pre-Implementation Evidence - D051 FM4 Ratio/Pitch Mapping

Status: **PASS - isolated production ratio source is now allowed under D051**  
Date: 2026-10-25  
Package: WP5.3 / issue #46  
Branch: `phase5/fm4-ratios`  
Normative contract: `WP5_3_FM_RATIO_CONTRACT.md`

## Contract checkpoint

D051 was frozen before any production `fmt_fm_ratio.*` source at commit:

`e3b1272a442091dd9cc140f3d6d357f6215438b5`

Its parent is the canonical WP5.2 closeout:

`826a7dca7394d6e2c13f79fafa97bd05fb1986c8`

The compare from the WP5.2 closeout to the D051 contract checkpoint contained exactly six documentation/test/workflow files and no `firmware/`, `firmware/gen` or production-tool changes.

## Canonical source-absent gate

- run: `40138575267`
- job: `121005262360`
- head: `e3b1272a442091dd9cc140f3d6d357f6215438b5`
- conclusion: SUCCESS
- artifact: `wp5-3-d051-contract-freeze`
- artifact ID: `12577928512`
- artifact SHA-256: `f6ff75f9dd2128f3d79cffd467ee1b88b1422a5daa788516387b7e9c50e19014`

## Proven before implementation

The canonical gate proved all of the following before production ratio source existed:

- `firmware/src/fmt_fm_ratio.h/.c` were absent;
- D038 pitch code was unchanged from the WP5.2 closeout;
- D049 operator code/data were unchanged;
- D050 algorithm-routing code was unchanged;
- `dinghy.c` was unchanged;
- exactly 16 stable D051 ratio IDs were reproduced;
- UNITY ID 1 is the deterministic invalid-ID fallback;
- the exact Q8 ratio table matched its frozen SHA;
- the exact per-ratio clamp-boundary table matched its frozen SHA;
- canonical base maximum `1293741834` was reproduced from D038;
- output ceiling `2147483647` was reproduced;
- 32-bit split-Q8 round-half-up arithmetic matched all anchors;
- ratios 0..4 never clamp in the legal tracker pitch domain;
- all first-clamp anchors for IDs 5..15 matched exactly;
- malformed base input clamps to the canonical base maximum before ratio scaling;
- null ratio-ID map4 semantics resolve to UNITY for all four outputs;
- the ratio layer requires no persistent state;
- exhaustive legal-pitch verification covered 16,384 pitch values x 16 ratio IDs = 262,144 outputs;
- exhaustive payload size was exactly 1,048,576 B;
- exactly 27,615 legal pitch/ratio outputs intentionally saturated at the D051 ceiling;
- exhaustive output SHA matched exactly;
- inherited D050 algorithm routing remained exact;
- inherited D049 operator behavior remained exact;
- inherited WP5.0 host application WAV remained exact;
- no physical timing-margin claim was made.

Canonical markers:

`WP5_3_D051_PRODUCTION_ABSENT_OK=1 ratio_source=absent d038_delta=0 d049_delta=0 d050_delta=0 dinghy_delta=0`

`WP5_3_D051_RATIO_TABLE_OK=1 ids=16 unity=1 q8_sha=5d98aebf644051ac555efcf1ceabf4015f26aaa2884722b515c73b5518659a60 boundaries_sha=c39e8210aadfc1bd347fbb86a7c2058ae8681c123892044c316cf48a55a15255`

`WP5_3_D051_ARITHMETIC_OK=1 base_max=1293741834 ceiling=2147483647 q8_round=half_up runtime_div=0 runtime_int64=0`

`WP5_3_D051_CLAMP_OK=1 ratios_never_clip=0..4 high_ratio_first_clip=exact malformed_base=canonical_max invalid_ratio=unity`

`WP5_3_D051_MAP4_OK=1 outputs=4 state_bytes=0 null_ratio_ids=unity`

`WP5_3_D051_EXHAUSTIVE_OK=1 pitches=16384 ratios=16 outputs=262144 bytes=1048576 clipped=27615 sha256=5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c`

`WP5_2_FM_ALGORITHM_HOST_GATE_OK=1 d050=exact algorithms=4 hashes=exact stress=pass d049=exact full_app_wav=wp5_0_identical`

`WP5_3_D051_CONTRACT_FREEZE_OK=1 production=absent ratios=16 exhaustive=pass d050=exact d049=exact full_app_wav=wp5_0_identical hardware_margin_claim=none`

## Frozen D051 identities

The V1 ratio IDs are now stable:

```text
0  0.5
1  1.0          UNITY
2  1.25
3  1.4140625    exact 362/256
4  1.5
5  2.0
6  2.5
7  3.0
8  3.5
9  4.0
10 5.0
11 6.0
12 7.0
13 8.0
14 12.0
15 16.0
```

Frozen reference hashes:

- Q8 ratio table: `5d98aebf644051ac555efcf1ceabf4015f26aaa2884722b515c73b5518659a60`
- clamp-boundary table: `c39e8210aadfc1bd347fbb86a7c2058ae8681c123892044c316cf48a55a15255`
- combined tables: `17d978a26a632bf5f28dc7efcc519f84dd270a2a1c19baac9739dd8e8605f28c`
- exhaustive 262,144-output reference: `5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c`

## Implementation permission

The D051 source-absent gate is green. WP5.3 may now add only the isolated production ratio primitive:

```text
firmware/src/fmt_fm_ratio.h
firmware/src/fmt_fm_ratio.c
```

The implementation must match D051 exactly, remain unlinked from `dinghy.c`, preserve D038/D049/D050, pass the same exhaustive reference, compile under pinned PI32v2 within the 1,024 B text+rodata ceiling and leave the full target application exact WP5.0.

WP5.4+ level/mod-index mapping, feedback, envelopes, voice/router integration, parameter pages and audible mixed-engine integration remain forbidden in WP5.3.

No physical FM-1 was available. `hardware_margin_claim=none`.
