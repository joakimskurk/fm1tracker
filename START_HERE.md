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

WP5.0 / D048, WP5.1 / D049 and WP5.2 / D050 are COMPLETE.

WP5.3 / D051 is ACTIVE on issue #46 and branch `phase5/fm4-ratios`.

The D051 contract is frozen and its source-absent reference gate is GREEN:

- contract head `e3b1272a442091dd9cc140f3d6d357f6215438b5`
- run `40138575267`
- job `121005262360`
- artifact `wp5-3-d051-contract-freeze`
- artifact ID `12577928512`
- artifact SHA `f6ff75f9dd2128f3d79cffd467ee1b88b1422a5daa788516387b7e9c50e19014`
- production ratio source was absent
- D038/D049/D050/`dinghy.c` were unchanged
- all 16 ratio identities and clamp thresholds matched
- exhaustive 262,144-output SHA matched `5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c`
- inherited D050/D049/WP5.0 host regression remained exact

## Frozen D051 core

- ratios: `0.5, 1, 1.25, 1.4140625, 1.5, 2, 2.5, 3, 3.5, 4, 5, 6, 7, 8, 12, 16`;
- unsigned Q8 exact representation;
- UNITY ID 1 is invalid-ID fallback;
- canonical base maximum `1293741834`;
- output ceiling `0x7fffffff`;
- precomputed per-ratio clamp boundaries;
- 32-bit split-Q8 round-half-up transform;
- no runtime division/modulo, 64-bit integers or float;
- ratio mapping is stateless;
- future PI32v2 ratio-object ceiling 1,024 B;
- D051 ceiling is not an anti-aliasing claim.

## Exact next action

The isolated implementation is now authorized.

Proceed in this order:

1. preserve the green D051 pre-implementation evidence as a docs-only checkpoint;
2. add only `firmware/src/fmt_fm_ratio.h/.c` under production firmware;
3. add focused C verification that reproduces the frozen D051 anchors and exhaustive hash;
4. add a pinned PI32v2/full-target verification workflow;
5. keep `dinghy.c` unchanged;
6. require ratio-object text+rodata <= 1,024 B;
7. require D038/D049/D050 and WP5.0 regressions exact;
8. write `docs/WP5_3_VERIFICATION.md` and close #46 before WP5.4 begins.

Do not include final level/mod-index UX, feedback, envelopes, FM voice allocation, p-lock pages, oversampling policy or audible mixed FM playback in WP5.3.

No physical FM-1 is available. `hardware_margin_claim=none`.
