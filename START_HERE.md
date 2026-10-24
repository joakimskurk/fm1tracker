# FM1Tracker - Start Here

Repository state is authoritative. Do not reconstruct the plan from old chat context.

Read in this order:

1. `docs/PROJECT_STATE.md`
2. `docs/WP5_1_VERIFICATION.md`
3. `docs/WP5_1_FM_OPERATOR_CONTRACT.md`
4. `docs/WP5_0_VERIFICATION.md`
5. `docs/D047_MIXED_ENGINE_ARCHITECTURE.md`
6. `docs/WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md`
7. `docs/CURRENT_DECISIONS.md`
8. `docs/WORK_PACKAGES.md`
9. `docs/EXECUTION_PROTOCOL.md`
10. `docs/CURRENT_TEST_STATE.md`
11. `docs/TEST_MATRIX.md`
12. `docs/ROADMAP.md`
13. `THIRD_PARTY.md`

## Current handoff

WP5.0 / D048 and WP5.1 / D049 are COMPLETE.

Canonical WP5.0 exit:

- production verification head `813792fb25632b4d7acda787422e51f713fad21e`
- run `40129822006`
- job `120978955451`
- target 72,792 B image / 19,676 B RAM / 113,000 B pool
- evidence `docs/WP5_0_VERIFICATION.md`

Canonical WP5.1 exit:

- verification head `52df09bf363ad5a9ad2d722a38734eaa580ecb90`
- run `40134823600`
- host job `120994003059`
- target job `120994002842`
- evidence `docs/WP5_1_VERIFICATION.md`
- isolated D049 operator object text+rodata 692 B
- operator object SHA `46868461ac6910d499975dbd93d06ee0af9db905d01c32583321eeaea0e33fad`
- full application remains exact WP5.0

D049 operator compatibility identifiers:

- state exactly 4 B, one uint32 phase;
- table 257 int16 entries / 514 B payload;
- table SHA `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68`;
- 4,096-render SHA `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66`;
- no audible FM production integration yet.

## Active package

**WP5.2 - curated FM4 algorithm routing**, issue #45, branch `phase5/fm4-algorithms`.

WP5.2 begins contract-first. There is no accepted D050 algorithm contract yet.

## Exact next action

Do not start production FM algorithm-routing code yet.

Proceed in this order:

1. inspect D049 operator semantics and existing FM4 scope constraints;
2. choose a small curated four-operator algorithm set rather than an unrestricted matrix;
3. freeze stable internal algorithm IDs and exact graph topology;
4. freeze exact per-sample operator evaluation order;
5. freeze modulation accumulation/scaling and overflow/saturation semantics;
6. freeze carrier summing/output normalization;
7. decide whether the pre-feedback routing layer needs mutable state;
8. define deterministic graph fixtures, long-run hashes and target/realtime ceilings;
9. write `docs/WP5_2_FM_ALGORITHM_CONTRACT.md` as D050;
10. add a source-absent reference CI gate and require green evidence before implementation.

Do not include ratios, final level/mod-index UX, feedback, FM envelopes, voice allocation, p-lock pages or audible mixed FM playback in WP5.2.

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains core product direction after engine foundations.

No physical FM-1 is available. `hardware_margin_claim=none`.
