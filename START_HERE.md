# FM1Tracker - Start Here

Repository state is authoritative. Do not reconstruct the plan from old chat context.

Read in this order:

1. `docs/PROJECT_STATE.md`
2. `docs/WP5_7_FM_VOICE_INTEGRATION_CONTRACT.md`
3. `docs/WP5_7_PRECONTRACT_AUDIT.md`
4. `docs/WP5_6_VERIFICATION.md`
5. `docs/WP5_6_FM_ENVELOPE_CONTRACT.md`
6. `docs/WP5_5_VERIFICATION.md`
7. `docs/WP5_4_VERIFICATION.md`
8. `docs/WP5_3_VERIFICATION.md`
9. `docs/WP5_2_VERIFICATION.md`
10. `docs/WP5_1_VERIFICATION.md`
11. `docs/WP5_0_VERIFICATION.md`
12. `docs/D047_MIXED_ENGINE_ARCHITECTURE.md`
13. `docs/CURRENT_DECISIONS.md`
14. `docs/WORK_PACKAGES.md`
15. `docs/EXECUTION_PROTOCOL.md`
16. `docs/CURRENT_TEST_STATE.md`
17. `docs/TEST_MATRIX.md`
18. `docs/ROADMAP.md`
19. `THIRD_PARTY.md`

## Current handoff

WP5.0/D048 through WP5.6/D054 are complete.

WP5.7 / issue #50 is now the only active package.

Active branch: `phase5/fm4-voice-integration`.

Parent closeout:

- head `886cb496f4e73a91c4ded16bf9e722b770a040f6`
- run `40168286355`
- host `121093715194` SUCCESS
- target `121093715000` SUCCESS

## D055 direction

D055 is contract-first and source-absent.

The candidate architecture freezes:

- eight family-local FM slots;
- reuse of the existing D043 allocator policy;
- `FmtFmVoice = 92 B`;
- `FmtFmEngine = 788 B`;
- one immutable 28 B default FM bridge patch in ROM;
- no mutable FM instrument bank or FM p-lock/page cache in WP5.7;
- one backward-compatible D050 seam for externally rendered OP4;
- exact D048 family-switch ordering;
- default all-8BIT audio regression exact.

## Exact next action

Run the D055 source-absent gate.

Production integration is blocked until that gate proves:

1. `fmt_fm_engine.*` is absent;
2. D043/D048/D049-D054 and `dinghy.c` still equal the WP5.6 closeout baseline;
3. the 8-slot allocator model has zero active cross-track steals;
4. route ordering matches D048;
5. inherited D054 through WP5.0 host regressions remain exact.

Only after durable green evidence may WP5.7 integration source begin.

No physical FM-1 is available. `hardware_margin_claim=none`.
