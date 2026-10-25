# FM1Tracker - Start Here

Repository state is authoritative. Do not reconstruct the plan from old chat context.

Read in this order:

1. `docs/PROJECT_STATE.md`
2. `docs/WP5_4_VERIFICATION.md`
3. `docs/WP5_4_FM_LEVEL_INDEX_CONTRACT.md`
4. `docs/WP5_4_PREIMPLEMENTATION_EVIDENCE.md`
5. `docs/WP5_3_VERIFICATION.md`
6. `docs/WP5_2_VERIFICATION.md`
7. `docs/WP5_1_VERIFICATION.md`
8. `docs/WP5_0_VERIFICATION.md`
9. `docs/D047_MIXED_ENGINE_ARCHITECTURE.md`
10. `docs/CURRENT_DECISIONS.md`
11. `docs/WORK_PACKAGES.md`
12. `docs/EXECUTION_PROTOCOL.md`
13. `docs/CURRENT_TEST_STATE.md`
14. `docs/TEST_MATRIX.md`
15. `docs/ROADMAP.md`
16. `THIRD_PARTY.md`

## Current handoff

WP5.0/D048 through WP5.4/D052 are complete. WP5.5 / #48 is ACTIVE.

Active branch: `phase5/fm4-feedback`  
Parent closeout: `d3501eca952f3e903d4dcc4faab2851d439f7ad0`

WP5.4 final closeout-head verification:

- run `40163616219`
- host job `121080077905` - SUCCESS
- target job `121080077699` - SUCCESS
- host artifact ID `12587775120`, SHA `1aa8c6f6bc33580fe4cfabb30bc4882207abdab2b73e37c7ec2f04ea92a13f8b`
- target artifact ID `12586739798`, SHA `fe83801b6ed68e1409019a8c18b5dc2a164405e1a5c19ad16386333b81a6fe20`

Canonical WP5.4 implementation evidence remains in `docs/WP5_4_VERIFICATION.md`.

## Active package

**WP5.5 - bounded deterministic FM4 feedback**, issue #48.

D053 does **not** exist yet. Begin with actual D049/D050/D052 source audit. Do not create production feedback source before D053 is frozen and a source-absent gate passes.

Exact first actions:

1. inspect D049 phase modulation arithmetic and D050 fixed OP4 -> OP3 -> OP2 -> OP1 evaluation;
2. identify the smallest feedback insertion point with no graph traversal or unbounded history;
3. choose one bounded V1 feedback topology;
4. freeze feedback control range and exact integer transform;
5. freeze one-sample history ownership, reset and algorithm-switch semantics;
6. prove worst-case arithmetic using D051 ratios and D052 maximum LEVEL/INDEX;
7. define deterministic vectors, reset/switch fixtures and long stress;
8. freeze state/object/realtime ceilings in D053;
9. pass a source-absent D053 gate;
10. only then implement.

Do not pull envelopes, voice allocation/router integration, tracker-facing p-lock IDs/pages or audible mixed FM playback into WP5.5.

No physical FM-1 is available. `hardware_margin_claim=none`.
