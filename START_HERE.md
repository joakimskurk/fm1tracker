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

WP5.0/D048 through WP5.4/D052 are complete.

WP5.4 canonical verification:

- implementation head `8a283c9216e7f72bab0770f5b1a8a0255f9bd54d`
- run `40143482844`
- host job `121019875458`
- target job `121019875309`
- host artifact ID `12579074081`, SHA `8f98edfc38e09a1972f5ca168ad3b8690508782ca14b31179496ec4edb27de2c`
- target artifact ID `12579672157`, SHA `7b4d5e9d76555998ba3d099090f1e5ef592f775e25338f19b4c09de30b6bc01f`
- PI32v2 `fmt_fm_controls.o` 108 B, SHA `05867ebe120829d1e3b8bf6f0504f29d63e9643e4fa5eede4c6d858d31d25059`
- full target remains exact WP5.0
- evidence `docs/WP5_4_VERIFICATION.md`

D052 source-absent proof remains canonical at run `40140119411`, job `121009903030`.

## Frozen D052 core

- four LEVEL bytes plus one global INDEX byte, all 0..255;
- LEVEL -> Q15 by `(v << 7) | (v >> 1)`;
- INDEX -> Q14 by `(v << 6) | (v >> 2)`;
- scalar malformed values saturate at 255;
- null LEVEL input maps to silence;
- mapped transient struct is exactly 10 B;
- no persistent D052 state;
- all 65,536 legal LEVEL x INDEX edge combinations are signed-32 safe;
- D052 remains isolated from `dinghy.c` and audible FM production.

## Next package

**WP5.5 - bounded deterministic FM4 feedback**, issue #48.

Expected branch: `phase5/fm4-feedback`.

D053 does **not** exist yet. Begin contract-first.

Exact first actions:

1. branch from the WP5.4 closeout checkpoint;
2. make #48 ACTIVE only after #47 is closed;
3. inspect D049 phase modulation and D050 fixed evaluation order for the smallest feedback insertion point;
4. choose one bounded V1 feedback topology and fixed history ownership;
5. freeze feedback control domain, zero/nominal/max, clamp/rounding and malformed-input behavior;
6. freeze reset and algorithm-switch history semantics;
7. prove arithmetic safety against D051 ratio extremes and D052 max LEVEL/INDEX;
8. define deterministic vectors, switch/reset fixtures and long stress;
9. freeze state/object ceilings and D053 before production feedback source;
10. pass a source-absent gate before implementation.

Do not pull envelopes, voice allocation/router integration, tracker-facing p-lock IDs/pages or audible mixed FM playback into WP5.5.

No physical FM-1 is available. `hardware_margin_claim=none`.
