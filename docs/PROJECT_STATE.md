# FM1Tracker Project State

This is the **single source of truth for current execution state**.

Read `START_HERE.md` first.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.3 D051 isolated implementation prepared, verification CI pending - 2026-10-25**

Current work package: **WP5.3 - FM4 integer ratio table and pitch mapping**  
Issue: **#46**  
Branch: `phase5/fm4-ratios`  
WP5.2 closeout baseline: `826a7dca7394d6e2c13f79fafa97bd05fb1986c8`  
D051 contract checkpoint: `e3b1272a442091dd9cc140f3d6d357f6215438b5`  
D051 green pre-implementation evidence checkpoint: `35f583b8717e167376e1c6418f426d1e1f95ff5c`  
D051 source-absent run/job: `40138575267` / `121005262360`  
Normative: `docs/WP5_3_FM_RATIO_CONTRACT.md`  
Pre-implementation evidence: `docs/WP5_3_PREIMPLEMENTATION_EVIDENCE.md`

## Completed baseline

- Phase 1-4 complete.
- WP5.0 / D048 mixed-engine routing complete.
- WP5.1 / D049 fixed-point FM operator complete.
- WP5.2 / D050 curated four-operator routing complete.

## D051 frozen compatibility

Ratios by stable ID:

`0.5, 1.0, 1.25, 1.4140625, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 5.0, 6.0, 7.0, 8.0, 12.0, 16.0`

- unsigned Q8;
- UNITY ID 1 invalid-ID fallback;
- base maximum `1293741834`;
- output ceiling `0x7fffffff`;
- precomputed clamp boundaries;
- 32-bit split-Q8 round-half-up;
- no runtime division/modulo, int64 or float;
- stateless mapping;
- exhaustive 262,144-output SHA `5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c`;
- PI32v2 ratio object ceiling 1,024 B.

## Current implementation candidate

The only new production files allowed and prepared are:

- `firmware/src/fmt_fm_ratio.h`
- `firmware/src/fmt_fm_ratio.c`

Verification assets prepared:

- `tests/test_wp5_3_fm_ratio.c`
- `tests/run_wp5_3_fm_ratio_tests.sh`
- `.github/workflows/wp5_3_fm_ratio_verify.yml`

The C implementation is intentionally isolated. `dinghy.c`, D038, D049 and D050 must remain unchanged. The full application must remain exact WP5.0.

## Exact next actions

1. commit the isolated implementation/test/workflow over `35f583b...`;
2. prove source boundary contains only `fmt_fm_ratio.h/.c` under production firmware;
3. run WP5.3 host and pinned-target verification;
4. require C exhaustive SHA exact and inherited D050/D049/WP5.0 exact;
5. require PI32v2 object text+rodata <= 1,024 B;
6. require target image 72,792 B / RAM 19,676 B / pool 113,000 B and exact WP5.0 BIN/FWSC hashes;
7. if green, write `docs/WP5_3_VERIFICATION.md`, close #46 and only then activate WP5.4.

## Out of scope

No final level/mod-index mapping, feedback, envelopes, FM voice allocation/router integration, parameter pages/p-lock IDs, audible mixed FM integration or oversampling policy in WP5.3.

## Hardware boundary

No physical FM-1 is available. `hardware_margin_claim=none`.
