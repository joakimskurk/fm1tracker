# FM1Tracker - Start Here

Repository state is authoritative. Do not reconstruct the plan from old chat context.

Read in this order:

1. `docs/PROJECT_STATE.md`
2. `docs/WP5_3_PREIMPLEMENTATION_EVIDENCE.md`
3. `docs/WP5_3_FM_RATIO_CONTRACT.md`
4. `docs/WP5_2_VERIFICATION.md`
5. `docs/WP5_1_VERIFICATION.md`
6. `docs/WP5_0_VERIFICATION.md`
7. `docs/D047_MIXED_ENGINE_ARCHITECTURE.md`
8. `docs/CURRENT_DECISIONS.md`
9. `docs/WORK_PACKAGES.md`
10. `docs/EXECUTION_PROTOCOL.md`
11. `docs/CURRENT_TEST_STATE.md`
12. `docs/TEST_MATRIX.md`
13. `docs/ROADMAP.md`
14. `THIRD_PARTY.md`

## Current handoff

WP5.0/D048, WP5.1/D049 and WP5.2/D050 are complete.

WP5.3/D051 is active on issue #46, branch `phase5/fm4-ratios`.

The D051 source-absent gate is green:

- contract head `e3b1272a442091dd9cc140f3d6d357f6215438b5`
- pre-implementation evidence checkpoint `35f583b8717e167376e1c6418f426d1e1f95ff5c`
- run/job `40138575267` / `121005262360`
- artifact ID `12577928512`
- artifact SHA `f6ff75f9dd2128f3d79cffd467ee1b88b1422a5daa788516387b7e9c50e19014`

## D051 core

Stable ratio IDs represent:

`0.5, 1.0, 1.25, 1.4140625, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 5.0, 6.0, 7.0, 8.0, 12.0, 16.0`

- Q8 integer representation;
- base max `1293741834`;
- output ceiling `0x7fffffff`;
- invalid ID -> UNITY;
- no runtime division/modulo/int64/float;
- exhaustive reference SHA `5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c`;
- PI32v2 object ceiling 1,024 B.

## Current implementation checkpoint state

Isolated production implementation plus verification assets are prepared for commit:

- `firmware/src/fmt_fm_ratio.h`
- `firmware/src/fmt_fm_ratio.c`
- `tests/test_wp5_3_fm_ratio.c`
- `tests/run_wp5_3_fm_ratio_tests.sh`
- `.github/workflows/wp5_3_fm_ratio_verify.yml`

No `dinghy.c`, D038, D049 or D050 change is allowed.

## Exact next action

1. commit the isolated D051 implementation over `35f583b...`;
2. verify production diff is exactly the two ratio files;
3. run WP5.3 host + pinned target CI;
4. require exhaustive C output SHA exact;
5. require ratio object <= 1,024 B;
6. require full target exact WP5.0;
7. if green, write `WP5_3_VERIFICATION.md` and close #46 before WP5.4.

Do not include final level/mod-index mapping, feedback, envelopes, voice allocation, p-lock pages, oversampling policy or audible mixed FM playback in WP5.3.

No physical FM-1 is available. `hardware_margin_claim=none`.
