# FM1Tracker - Start Here

Repository state is authoritative. Do not reconstruct the plan from old chat context.

Read in this order:

1. `docs/PROJECT_STATE.md`
2. `docs/WP5_6_VERIFICATION.md`
3. `docs/WP5_6_FM_ENVELOPE_CONTRACT.md`
4. `docs/WP5_6_PREIMPLEMENTATION_EVIDENCE.md`
5. `docs/WP5_5_VERIFICATION.md`
6. `docs/WP5_4_VERIFICATION.md`
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

WP5.0/D048 through WP5.5/D053 are COMPLETE. WP5.6/D054 is technically complete and is in formal closeout.

Active package: **WP5.6 closeout**.  
Active issue: **#49**.  
Active branch: `phase5/fm4-envelope`.

Canonical implementation verification:

```text
head    401e666d21fa8290c7c622603e06e97b166c6b3e
run     40167933064
host    121092688126 SUCCESS
target  121092688295 SUCCESS
```

D054 result:

- four independent per-operator ADSR envelopes;
- exactly 20 B state per future FM voice;
- exactly 16 B controls per FM instrument;
- immutable 1,024 B A/D/R step table;
- PI32v2 object 1,680 B text+rodata, 368 B under ceiling;
- D049-D053 unchanged;
- full running firmware remains exact WP5.0.

## Exact next action

Run the same WP5.6 host/target verification on the documentation closeout head. Only after both jobs are green may #49 close and WP5.7 / #50 become active on `phase5/fm4-voice-integration`.

WP5.7 must start D055 contract-first. Do not integrate FM voices into D048 before D055 is frozen.

No physical FM-1 is available. `hardware_margin_claim=none`.
