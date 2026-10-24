# FM1Tracker - Start Here

Repository state is authoritative. Do not reconstruct the plan from old chat context.

Read in this order:

1. `docs/PROJECT_STATE.md`
2. `docs/WP5_3_FM_RATIO_CONTRACT.md`
3. `docs/WP5_2_VERIFICATION.md`
4. `docs/WP5_2_FM_ALGORITHM_CONTRACT.md`
5. `docs/WP5_1_VERIFICATION.md`
6. `docs/WP5_1_FM_OPERATOR_CONTRACT.md`
7. `docs/WP5_0_VERIFICATION.md`
8. `docs/D047_MIXED_ENGINE_ARCHITECTURE.md`
9. `docs/CURRENT_DECISIONS.md`
10. `docs/WORK_PACKAGES.md`
11. `docs/EXECUTION_PROTOCOL.md`
12. `docs/CURRENT_TEST_STATE.md`
13. `docs/TEST_MATRIX.md`
14. `docs/ROADMAP.md`
15. `THIRD_PARTY.md`

## Current handoff

WP5.0 / D048, WP5.1 / D049 and WP5.2 / D050 are COMPLETE.

Canonical WP5.2 exit:

- implementation head `bc054bc697304afe94fe18b2990396e6a1667dbf`
- closeout / WP5.3 parent `826a7dca7394d6e2c13f79fafa97bd05fb1986c8`
- run `40136425976`
- host job `120998804031`
- target job `120998804175`
- host artifact `12576975819`, SHA `3df772adb2054409aa65d75396878a4fe69817c7a524025dc3b1d5bbdf4b52fd`
- target artifact `12576824809`, SHA `0e6f7788fc8f4d5a979b0dde66bd222e0e175735d4d2ce04017a4baa95a34db7`
- router object 362 B text+rodata
- router object SHA `ed85fe6ed70402bd3b0727e59b1c8eee21f6d15f264ec8dc439603968a38f469`
- full application remains exact WP5.0
- evidence `docs/WP5_2_VERIFICATION.md`

## Active package

**WP5.3 - FM4 integer ratio table and pitch mapping**, issue #46, branch `phase5/fm4-ratios`.

D051 is now contract-frozen in `docs/WP5_3_FM_RATIO_CONTRACT.md`, but production ratio source is still absent and unauthorized until the source-absent gate is green.

Frozen D051 core:

- 16 stable ratio IDs: `0.5, 1, 1.25, 1.4140625, 1.5, 2, 2.5, 3, 3.5, 4, 5, 6, 7, 8, 12, 16`;
- unsigned Q8 exact ratio representation;
- UNITY ID 1 is invalid-ID fallback;
- canonical base maximum `1293741834`;
- output ceiling `0x7fffffff`;
- precomputed per-ratio clamp boundaries;
- 32-bit split-Q8 round-half-up transform;
- no runtime division/modulo, 64-bit integers or float;
- exhaustive reference SHA `5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c` over 262,144 legal pitch/ratio outputs;
- future PI32v2 ratio-object ceiling 1,024 B.

## Exact next action

Do not add `firmware/src/fmt_fm_ratio.h/.c` yet.

Proceed in this order:

1. commit the D051 contract, `tests/run_wp5_3_contract_freeze.sh`, workflow and handoff docs without production firmware changes;
2. compare that checkpoint against `826a7dca7394d6e2c13f79fafa97bd05fb1986c8` and prove zero `firmware/` delta;
3. run `.github/workflows/wp5_3_contract_freeze.yml`;
4. require exact ratio-table, arithmetic, clamp, map4, exhaustive hash and inherited D050/D049/WP5.0 green results;
5. freeze the green pre-implementation evidence in the repo;
6. only then implement isolated `fmt_fm_ratio.h/.c`;
7. keep `dinghy.c` unchanged until later FM voice integration.

Do not include final level/mod-index UX, feedback, envelopes, FM voice allocation, p-lock pages, oversampling policy or audible mixed FM playback in WP5.3.

No physical FM-1 is available. `hardware_margin_claim=none`.
