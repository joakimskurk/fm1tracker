# FM1Tracker - Start Here

Repository state is authoritative. Do not reconstruct the plan from old chat context.

Read in this order:

1. `docs/PROJECT_STATE.md`
2. `docs/WP5_2_VERIFICATION.md`
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

WP5.0 / D048, WP5.1 / D049 and WP5.2 / D050 are COMPLETE.

Canonical WP5.2 exit:

- implementation head `bc054bc697304afe94fe18b2990396e6a1667dbf`
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

There is no accepted D051 ratio contract yet.

## Exact next action

Do not add production ratio code yet.

Proceed in this order:

1. inspect canonical `fmt_engine_pitch.*` and the D049/D050 numeric boundaries;
2. select a compact musically useful ratio set rather than arbitrary runtime rational math;
3. freeze stable ratio IDs and exact meanings;
4. freeze integer transform arithmetic, rounding and overflow/clamp behavior;
5. freeze supported base-increment/high-register behavior;
6. explicitly decide sub-unity/fractional ratio scope;
7. define deterministic low/mid/high and boundary fixtures;
8. define one-base-increment -> four-operator mapping API/state;
9. define source/realtime and PI32v2 object ceilings;
10. write D051 `docs/WP5_3_FM_RATIO_CONTRACT.md` and a source-absent reference gate;
11. require that gate green before production ratio implementation.

Do not include final level/mod-index UX, feedback, envelopes, FM voice allocation, p-lock pages or audible mixed FM playback in WP5.3.

No physical FM-1 is available. `hardware_margin_claim=none`.
