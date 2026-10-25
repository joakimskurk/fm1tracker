# FM1Tracker - Start Here

Repository state is authoritative. Do not reconstruct the plan from old chat context.

Read in this order:

1. `docs/PROJECT_STATE.md`
2. `docs/WP5_6_PRECONTRACT_AUDIT.md`
3. `docs/WP5_5_VERIFICATION.md`
4. `docs/WP5_5_FM_FEEDBACK_CONTRACT.md`
5. `docs/WP5_4_VERIFICATION.md`
6. `docs/WP5_3_VERIFICATION.md`
7. `docs/WP5_2_VERIFICATION.md`
8. `docs/WP5_1_VERIFICATION.md`
9. `docs/WP5_0_VERIFICATION.md`
10. `docs/D047_MIXED_ENGINE_ARCHITECTURE.md`
11. `docs/CURRENT_DECISIONS.md`
12. `docs/WORK_PACKAGES.md`
13. `docs/EXECUTION_PROTOCOL.md`
14. `docs/CURRENT_TEST_STATE.md`
15. `docs/TEST_MATRIX.md`
16. `docs/ROADMAP.md`
17. `THIRD_PARTY.md`

## Current handoff

WP5.0/D048 through WP5.5/D053 are COMPLETE.

Active package: **WP5.6 - bounded FM4 envelope strategy**.  
Active issue: **#49**.  
Active branch: `phase5/fm4-envelope`.  
Parent closeout head: `216c7e34b6d761cadb683bd1fc356c216b98cc27`.

WP5.5 final closeout run `40165352623` passed host `121085156557` and target `121085156767`.

## WP5.6 contract-first boundary

No production envelope source exists yet and none may be added before D054 is frozen and its source-absent gate is accepted.

Current audit direction:

- reject a final post-mix envelope because it only shapes final loudness;
- preserve D049-D053 unchanged;
- use the existing per-operator Q15 LEVEL surface as the envelope insertion point;
- working candidate: four independent per-operator ADSR envelopes;
- envelope output scales each D052 mapped LEVEL before the corresponding D049 operator render;
- candidate packed persistent state is 20 B per future FM voice, still to be proven/frozen;
- exact accumulator, rate/time law, zero-time behavior, retrigger/release semantics and target ceilings are not frozen until D054.

## Exact next action

Finish `docs/WP5_6_PRECONTRACT_AUDIT.md`, select and quantify the V1 envelope arithmetic, then freeze D054 and source-absent reference tests before implementation.

Do not begin WP5.7 integration or add `fmt_fm_envelope.*` before the D054 gate is green.

No physical FM-1 is available. `hardware_margin_claim=none`.
