# FM1Tracker - Start Here

Repository state is authoritative. Do not reconstruct the plan from old chat context.

Read in this order:

1. `docs/PROJECT_STATE.md`
2. `docs/WP5_2_PREIMPLEMENTATION_EVIDENCE.md`
3. `docs/WP5_2_FM_ALGORITHM_CONTRACT.md`
4. `docs/WP5_1_VERIFICATION.md`
5. `docs/WP5_1_FM_OPERATOR_CONTRACT.md`
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

WP5.0 / D048 and WP5.1 / D049 are COMPLETE.

WP5.2 / D050 is ACTIVE. The source-absent contract gate is now GREEN, so isolated production algorithm-routing implementation is allowed.

D050 canonical pre-implementation evidence:

- contract checkpoint `f2d328f516bb66517d1e06d98aab1f3c09dcc68b`
- run `40135979431`
- job `120997461479`
- artifact ID `12577609841`
- artifact SHA `d3465c39b17416d58c56d98c196a18ae1c4dbd6e07d2ab7cc4ca2b3992cab85`
- evidence `docs/WP5_2_PREIMPLEMENTATION_EVIDENCE.md`

Stable algorithms:

```text
0 STACK4:    OP4 -> OP3 -> OP2 -> OP1 -> OUT
1 FAN3:      OP2 + OP3 + OP4 -> OP1 -> OUT
2 DUAL2:     OP4 -> OP3 plus OP2 -> OP1, two carriers
3 PARALLEL4: OP1 + OP2 + OP3 + OP4, four carriers
```

Every algorithm evaluates OP4 -> OP3 -> OP2 -> OP1 and advances all four D049 operators exactly once per sample.

## Exact next action

Implement `firmware/src/fmt_fm_algorithm.h/.c` exactly to D050, still isolated from `dinghy.c`.

Required implementation verification:

1. exact 16 B state and reset/retrigger/null behavior;
2. algorithm fallback and mod-depth clamp;
3. exact edge modulation and modulo phase accumulation;
4. exact sign-safe one/two/four carrier normalization;
5. exact first-12 vectors and four 4,096-render hashes;
6. 65,536 renders per algorithm stress;
7. algorithm-switch phase continuity;
8. no loop/heap/float/int64/divide/modulo/store/router access in render;
9. pinned PI32v2 router object <= 2,048 B text+rodata;
10. D049, WP5.0 host and full target identity remain exact.

Do not include ratios, final level/mod-index UX, feedback, FM envelopes, voice allocation, p-lock pages or audible mixed FM playback in WP5.2.

No physical FM-1 is available. `hardware_margin_claim=none`.
