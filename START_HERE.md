# FM1Tracker - Start Here

Repository state is authoritative. Do not reconstruct the plan from old chat context.

Read in this order:

1. `docs/PROJECT_STATE.md`
2. `docs/WP5_5_PRECONTRACT_AUDIT.md`
3. `docs/WP5_5_FM_FEEDBACK_CONTRACT.md`
4. `docs/WP5_4_VERIFICATION.md`
5. `docs/WP5_4_FM_LEVEL_INDEX_CONTRACT.md`
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

WP5.0/D048 through WP5.4/D052 are complete. WP5.5 / #48 is ACTIVE on `phase5/fm4-feedback`.

Parent closeout: `d3501eca952f3e903d4dcc4faab2851d439f7ad0`.

D053 is frozen contract-first in `docs/WP5_5_FM_FEEDBACK_CONTRACT.md`, but production feedback source is still forbidden until the source-absent gate is green and evidence is recorded.

## Frozen D053 candidate

- topology: OP4 self-feedback only;
- delay: exactly one sample;
- history: one signed-16 prior OP4 output, exactly 2 B;
- FEEDBACK: distinct `0..255` byte;
- mapping: `(v << 6) | (v >> 2)` to Q14;
- arithmetic: prior sample x depth x 4;
- reset/retrigger: clear history;
- algorithm switch: retain history;
- FEEDBACK=0: zero phase offset but continue tracking OP4 history;
- no D049-D052 production changes;
- no production feedback source yet.

## Exact next action

Run `.github/workflows/wp5_5_contract_freeze.yml` through the branch push and accept only if all source-absent D053 reference checks pass.

The gate includes the full signed-16 x 256 feedback arithmetic surface, four canonical algorithm fixtures, switch continuity, D051/D052 extreme stress and inherited WP5.4 regressions.

Only after accepted evidence may WP5.5 add:

```text
firmware/src/fmt_fm_feedback.h
firmware/src/fmt_fm_feedback.c
```

Do not pull envelopes, voice allocation/router integration, tracker-facing p-lock IDs/pages or audible mixed FM playback into WP5.5.

No physical FM-1 is available. `hardware_margin_claim=none`.
