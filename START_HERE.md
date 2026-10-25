# FM1Tracker - Start Here

Repository state is authoritative. Do not reconstruct the plan from old chat context.

Read in this order:

1. `docs/PROJECT_STATE.md`
2. `docs/WP5_7_VERIFICATION.md`
3. `docs/WP5_7_FM_VOICE_INTEGRATION_CONTRACT.md`
4. `docs/WP5_7_PREIMPLEMENTATION_EVIDENCE.md`
5. `docs/CURRENT_DECISIONS.md`
6. `docs/WORK_PACKAGES.md`
7. `docs/EXECUTION_PROTOCOL.md`
8. `docs/CURRENT_TEST_STATE.md`
9. `docs/TEST_MATRIX.md`
10. `docs/ROADMAP.md`
11. `THIRD_PARTY.md`

## Current handoff

WP5.0/D048 through WP5.6/D054 are complete.

WP5.7 / D055 implementation is technically complete and this branch is at formal documentation closeout.

Active issue: #50.  
Active branch: `phase5/fm4-voice-integration`.

Canonical implementation checkpoint:

- head `75ff8136881799ad652df380a53002c5efa70516`
- run `40173076046` SUCCESS
- host `121107852886` SUCCESS
- target `121107852943` SUCCESS
- host artifact `12590982004`
- target artifact `12590199901`

Verified target footprint:

```text
image       77048 B
normal RAM  20464 B
pool       113000 B
```

Default all-8BIT WAV remains exact WP5.0: `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`.

## Exact next action

1. require the normal WP5.7 host+target workflow to pass on this documentation closeout head;
2. close issue #50 only after that run is green;
3. create the WP5.8 issue and branch from the accepted closeout head;
4. begin WP5.8 with a contract-first audit of existing parameter-page, p-lock ID/storage and family-routing semantics;
5. do not implement mutable FM parameter storage until the WP5.8 decision is frozen and its source-absent gate is green.

No physical FM-1 is available. `hardware_margin_claim=none`.
