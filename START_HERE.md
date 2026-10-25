# FM1Tracker - Start Here

Repository state is authoritative. Do not reconstruct the plan from old chat context.

Read in this order:

1. `docs/PROJECT_STATE.md`
2. `docs/WP5_5_PREIMPLEMENTATION_EVIDENCE.md`
3. `docs/WP5_5_FM_FEEDBACK_CONTRACT.md`
4. `docs/WP5_5_PRECONTRACT_AUDIT.md`
5. `docs/WP5_4_VERIFICATION.md`
6. `docs/WP5_4_FM_LEVEL_INDEX_CONTRACT.md`
7. `docs/WP5_3_VERIFICATION.md`
8. `docs/WP5_2_VERIFICATION.md`
9. `docs/WP5_1_VERIFICATION.md`
10. `docs/WP5_0_VERIFICATION.md`
11. `docs/D047_MIXED_ENGINE_ARCHITECTURE.md`
12. `docs/CURRENT_DECISIONS.md`
13. `docs/WORK_PACKAGES.md`
14. `docs/EXECUTION_PROTOCOL.md`
15. `docs/CURRENT_TEST_STATE.md`
16. `docs/TEST_MATRIX.md`
17. `docs/ROADMAP.md`
18. `THIRD_PARTY.md`

## Current handoff

WP5.0/D048 through WP5.4/D052 are complete. WP5.5 / #48 is ACTIVE on `phase5/fm4-feedback`.

Parent closeout: `d3501eca952f3e903d4dcc4faab2851d439f7ad0`.

D053 is now accepted source-absent for implementation.

Canonical D053 gate:

- head `10bf0e3bf8ef49253666bee999b2de95de10046d`
- run `40164610821`
- job `121082987705`
- artifact ID `12587966076`
- artifact SHA `0eb34eb28a92f69eeeb653ddae2661ca62d72e8eaf887a881e227fbe26c7e4b9`
- evidence `docs/WP5_5_PREIMPLEMENTATION_EVIDENCE.md`

## Frozen D053

- topology: OP4 self-feedback only;
- delay: exactly one sample;
- history: one signed-16 prior OP4 output, exactly 2 B;
- FEEDBACK: distinct `0..255` byte;
- mapping: `(v << 6) | (v >> 2)` to Q14;
- arithmetic: prior sample x depth x 4;
- reset/retrigger: clear history;
- algorithm switch: retain history;
- FEEDBACK=0: zero phase offset but continue tracking OP4 history;
- no D049-D052 production changes.

## Exact next action

Implement only:

```text
firmware/src/fmt_fm_feedback.h
firmware/src/fmt_fm_feedback.c
```

Then verify the C implementation against the frozen D053 hashes, run inherited WP5.4 regressions, compile the isolated object with the pinned PI32v2 toolchain and prove the full target remains exact WP5.0.

Do not modify D049-D052 or `dinghy.c` in WP5.5. Do not pull envelopes, voice allocation/router integration, tracker-facing p-lock IDs/pages or audible mixed FM playback into this package.

No physical FM-1 is available. `hardware_margin_claim=none`.
