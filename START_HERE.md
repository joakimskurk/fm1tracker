# FM1Tracker - Start Here

Repository state is authoritative. Do not reconstruct the plan from old chat context.

Read in this order:

1. `docs/PROJECT_STATE.md`
2. `docs/WP5_2_FM_ALGORITHM_CONTRACT.md`
3. `docs/WP5_1_VERIFICATION.md`
4. `docs/WP5_1_FM_OPERATOR_CONTRACT.md`
5. `docs/WP5_0_VERIFICATION.md`
6. `docs/D047_MIXED_ENGINE_ARCHITECTURE.md`
7. `docs/CURRENT_DECISIONS.md`
8. `docs/WORK_PACKAGES.md`
9. `docs/EXECUTION_PROTOCOL.md`
10. `docs/CURRENT_TEST_STATE.md`
11. `docs/TEST_MATRIX.md`
12. `docs/ROADMAP.md`
13. `THIRD_PARTY.md`

## Current handoff

WP5.0 / D048 and WP5.1 / D049 are COMPLETE.

Canonical WP5.1 exit:

- verification head `52df09bf363ad5a9ad2d722a38734eaa580ecb90`
- closeout checkpoint `bc71289690c8f9bd36e9cbbb378e0df3de1b9f25`
- run `40134823600`
- host job `120994003059`
- target job `120994002842`
- evidence `docs/WP5_1_VERIFICATION.md`
- isolated D049 operator object text+rodata 692 B
- full application remains exact WP5.0

## Active package

**WP5.2 - curated FM4 algorithm routing**, issue #45, branch `phase5/fm4-algorithms`.

D050 is already frozen in `docs/WP5_2_FM_ALGORITHM_CONTRACT.md`.

The four stable algorithms are:

```text
0 STACK4:    OP4 -> OP3 -> OP2 -> OP1 -> OUT
1 FAN3:      OP2 + OP3 + OP4 -> OP1 -> OUT
2 DUAL2:     OP4 -> OP3 plus OP2 -> OP1, two carriers
3 PARALLEL4: OP1 + OP2 + OP3 + OP4, four carriers
```

Every algorithm evaluates OP4 -> OP3 -> OP2 -> OP1 and advances all four D049 operators exactly once per sample.

## Exact next action

Do **not** add `fmt_fm_algorithm.*` production source yet.

Run the committed D050 source-absent contract gate and require it to prove:

1. algorithm source/header absent;
2. D049 source/data unchanged from WP5.1 closeout;
3. all four topology/evaluation fixtures exact;
4. internal mod-depth and modulo phase-accumulation anchors exact;
5. carrier normalization exact;
6. all first-12 vectors exact;
7. all four 4,096-render hashes/final phases exact;
8. 65,536 renders per algorithm keep Q15 output bounded and phase advancement exact;
9. invalid IDs are exact STACK4 fallback;
10. D049 production vectors and full WP5.0 host baseline remain green.

Only after that gate is green may the production algorithm-routing primitive be implemented.

D050 frozen long hashes:

- STACK4 `25dfde1bb29de2c70305d6abafee91fd7480e69eaa71fd1a3fbde3c1e6e855cd`
- FAN3 `8d9ae5310369e3d5a36bcf8c44d5e74049d3d6e109362a844be2796b26330653`
- DUAL2 `c4c31cabf4b8e121591c4c1e0923aee67523eaf7d5b60b60ca1a545c13fc2269`
- PARALLEL4 `d316c3e5467cbfc606c788f669a3b19dba13535969dcce3b67b2821f0977a882`

Do not include ratios, final level/mod-index UX, feedback, FM envelopes, voice allocation, p-lock pages or audible mixed FM playback in WP5.2.

No physical FM-1 is available. `hardware_margin_claim=none`.
