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

WP5.2 / D050 is ACTIVE. The source-absent contract gate is green and the isolated production routing primitive has now been committed for verification.

D050 pre-implementation evidence:

- contract checkpoint `f2d328f516bb66517d1e06d98aab1f3c09dcc68b`
- green evidence checkpoint `6ecdb64653dee06411eecd4e73571cf21b11c3d9`
- run `40135979431`
- job `120997461479`
- artifact ID `12577609841`
- artifact SHA `d3465c39b17416d58c56d98c196a18ae1c4dbd6e07d2ab7cc4ca2b3992cab85`

Stable algorithms:

```text
0 STACK4:    OP4 -> OP3 -> OP2 -> OP1 -> OUT
1 FAN3:      OP2 + OP3 + OP4 -> OP1 -> OUT
2 DUAL2:     OP4 -> OP3 plus OP2 -> OP1, two carriers
3 PARALLEL4: OP1 + OP2 + OP3 + OP4, four carriers
```

## Exact next action

Do not design WP5.3 yet.

Follow the WP5.2 implementation CI for the current branch head. It must prove:

1. only `fmt_fm_algorithm.h/.c` were added under firmware/tools/gen after the green preimplementation checkpoint;
2. D049 operator and D048/router/app sources are unchanged;
3. exact D050 first-12 vectors and four long hashes pass;
4. 65,536 renders per algorithm and algorithm-switch phase continuity pass;
5. source audit proves no loops/graph traversal/heap/float/int64/divide/modulo/store/router access in render;
6. isolated PI32v2 router object stays <= 2,048 B text+rodata;
7. full target remains exact WP5.0 in resources, BIN and FWSC;
8. only after green host+target evidence write `WP5_2_VERIFICATION.md` and close WP5.2.

No ratios, final level/mod-index UX, feedback, envelopes, FM voice allocation, p-lock pages or audible mixed FM belong in WP5.2.

No physical FM-1 is available. `hardware_margin_claim=none`.
