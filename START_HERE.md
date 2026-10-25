# FM1Tracker - Start Here

Repository state is authoritative. Do not reconstruct the plan from old chat context.

Read in this order:

1. `docs/PROJECT_STATE.md`
2. `docs/WP5_7_PREIMPLEMENTATION_EVIDENCE.md`
3. `docs/WP5_7_FM_VOICE_INTEGRATION_CONTRACT.md`
4. `docs/WP5_7_PRECONTRACT_AUDIT.md`
5. `docs/WP5_6_VERIFICATION.md`
6. `docs/WP5_6_FM_ENVELOPE_CONTRACT.md`
7. `docs/WP5_5_VERIFICATION.md`
8. `docs/WP5_4_VERIFICATION.md`
9. `docs/WP5_3_VERIFICATION.md`
10. `docs/WP5_2_VERIFICATION.md`
11. `docs/WP5_1_VERIFICATION.md`
12. `docs/WP5_0_VERIFICATION.md`
13. `docs/D047_MIXED_ENGINE_ARCHITECTURE.md`
14. `docs/CURRENT_DECISIONS.md`
15. `docs/WORK_PACKAGES.md`
16. `docs/EXECUTION_PROTOCOL.md`
17. `docs/CURRENT_TEST_STATE.md`
18. `docs/TEST_MATRIX.md`
19. `docs/ROADMAP.md`
20. `THIRD_PARTY.md`

## Current handoff

WP5.0/D048 through WP5.6/D054 are complete.

WP5.7 / issue #50 is the only active package.

Active branch: `phase5/fm4-voice-integration`.

Parent closeout:

- head `886cb496f4e73a91c4ded16bf9e722b770a040f6`
- run `40168286355`
- host `121093715194` SUCCESS
- target `121093715000` SUCCESS

## D055 source-absent evidence

D055 contract checkpoint:

- head `01f8f6c8f81f9057909d99223372b0901296e3ec`
- run `40171940961`
- job `121104425642` SUCCESS
- artifact `12590656915`
- SHA `ab44e0a8553edaf02eefbf8c175b717a7d5ee32ae4c9ad44de77d94228f0d9c5`

Evidence: `docs/WP5_7_PREIMPLEMENTATION_EVIDENCE.md`.

The source-absent gate proved eight FM slots, 92 B per voice, 788 B engine state, the exact D043 allocator policy, D048 route ordering, zero active cross-track steals and inherited D054 through WP5.0 regressions before integration source existed.

## Production integration now authorized

Allowed production files only:

```text
firmware/src/fmt_fm_engine.h
firmware/src/fmt_fm_engine.c
firmware/src/fmt_fm_algorithm.h
firmware/src/fmt_fm_algorithm.c
firmware/src/dinghy.c
```

D050 may change only for the frozen externally-rendered-OP4 composition seam. D049, D051, D052, D053, D054, D043 allocator source and generated tables remain frozen.

## Exact next action

Implement and verify, in this order:

1. backward-compatible D050 composition seam;
2. exact 8-slot FM engine;
3. D048 FM family dispatch in `dinghy.c`;
4. focused host ownership/release/retrigger/cross-family fixtures;
5. exact all-8BIT WP5.0 WAV regression;
6. pinned PI32v2/full-target resource gate;
7. `WP5_7_VERIFICATION.md` and formal #50 closeout.

Do not start WP5.8 before WP5.7 is closed.

No physical FM-1 is available. `hardware_margin_claim=none`.
