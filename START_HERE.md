# FM1Tracker - Start Here

Repository state is authoritative. Do not reconstruct the plan from old chat context.

Read in this order:

1. `docs/PROJECT_STATE.md`
2. `docs/WP5_5_VERIFICATION.md`
3. `docs/WP5_5_PREIMPLEMENTATION_EVIDENCE.md`
4. `docs/WP5_5_FM_FEEDBACK_CONTRACT.md`
5. `docs/WP5_5_PRECONTRACT_AUDIT.md`
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

WP5.0/D048 through WP5.4/D052 are complete. WP5.5/D053 implementation and technical verification are complete. WP5.5 remains the active package only until the documentation closeout head passes one final verification run.

Active branch: `phase5/fm4-feedback`.  
Active issue: #48.  
Next planned issue: #49, WP5.6 envelope strategy.

Canonical D053 implementation verification:

- head `d1ddb4baab7fcb4934cbf98e46c968fcd5e1f45e`
- run `40164953532`
- host `121083999893` - SUCCESS
- target `121083999790` - SUCCESS
- host artifact `12587379892`, SHA `d57ad000b83e5aa7c155ff817a9feb74727998f7baa417ca73b11da735850408`
- target artifact `12587649156`, SHA `96183e229059cbc6ceb09b2057de5abf914d4d62ca0db89a8d1dea576ecb8645`
- PI32v2 feedback object 130 B, SHA `147c623c49fa8756f36937589d96f2da5139b6e3d1a09790e95d91c5f770256d`
- feedback state 2 B per future FM voice
- full target remains exact WP5.0

Evidence: `docs/WP5_5_VERIFICATION.md`.

## Frozen D053

- OP4 self-feedback only;
- one-sample delay;
- signed-16 previous OP4 output as 2 B state;
- FEEDBACK is a distinct 0..255 byte;
- Q14 mapping `(v << 6) | (v >> 2)`;
- phase offset `previous * depth * 4`;
- reset/retrigger clear history;
- algorithm changes retain history;
- FEEDBACK=0 keeps history tracking;
- D049-D052 and `dinghy.c` unchanged.

## Exact next action

The push containing this closeout must pass `.github/workflows/wp5_5_feedback_verify.yml` on the exact closeout head.

Only after both closeout-head jobs are green:

1. close #48;
2. branch `phase5/fm4-envelope` from that exact head;
3. activate #49;
4. audit common versus per-operator envelope architecture and freeze D054 before any production envelope source.

Do not start WP5.6 implementation before D054 source-absent evidence exists.

No physical FM-1 is available. `hardware_margin_claim=none`.
