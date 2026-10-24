# FM1Tracker - Start Here

Repository state is authoritative. Do not reconstruct the plan from old chat context.

Read in this order:

1. `docs/PROJECT_STATE.md`
2. `docs/WP5_1_FM_OPERATOR_CONTRACT.md`
3. `docs/WP5_1_PREIMPLEMENTATION_EVIDENCE.md`
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

WP5.0 / D048 is COMPLETE.

Canonical WP5.0 exit:

- run `40129822006`
- job `120978955451`
- production verification head `813792fb25632b4d7acda787422e51f713fad21e`
- closeout/handoff `6c0add8828474e9308710e02957231e7225eb121`
- target 72,792 B image / 19,676 B normal RAM / 113,000 B pool
- normal WAV `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`
- representative WAV `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9`

## Active package

**WP5.1 - fixed-point FM operator primitive**, issue #44, branch `phase5/fm4-operator`.

D049 is already frozen in `docs/WP5_1_FM_OPERATOR_CONTRACT.md`.

The contract-first gate is also already GREEN:

- run `40131135259`
- job `120982895786`
- head `010d1dff2f74e27450edc6784ae7792eca358e59`
- artifact `12574973209`
- artifact SHA-256 `ae74fcd6272eafffdf62a27ef0f9fd415941f1c8024901c4344cb19a0212f83b`
- evidence `docs/WP5_1_PREIMPLEMENTATION_EVIDENCE.md`

It proved all frozen D049 reference math while production FM operator source was still absent and the canonical WP5.0 WAV remained byte-identical.

## Exact next action

Do **not** rewrite or re-decide D049. Implementation is now allowed.

Proceed in this order:

1. add deterministic quarter-wave generator and checked-in generated table;
2. add isolated `fmt_fm_operator.h/.c` matching D049 exactly;
3. add focused C host vectors and source/realtime audit;
4. compile the isolated module with pinned JieLi PI32v2 and measure object sections;
5. prove the running application remains exact WP5.0 because the operator stays unlinked from `dinghy.c`;
6. write `WP5_1_VERIFICATION.md` and close WP5.1 before beginning WP5.2.

D049 frozen identifiers:

- operator state 4 B, one uint32 phase;
- table 257 int16 entries, 514 B immutable;
- table SHA-256 `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68`;
- long-vector SHA-256 `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66`;
- isolated target object text+immutable-data ceiling 4,096 B.

Do not include algorithms, ratios, feedback, FM envelopes, voice allocation, FM p-lock pages or mixed production FM playback in WP5.1.

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains core product direction after engine foundations.

No physical FM-1 is available. `hardware_margin_claim=none`.
