# FM1Tracker - Start Here

Repository state is authoritative. Do not reconstruct the plan from old chat context.

Read in this order:

1. `docs/PROJECT_STATE.md`
2. `docs/WP5_3_VERIFICATION.md`
3. `docs/WP5_3_FM_RATIO_CONTRACT.md`
4. `docs/WP5_2_VERIFICATION.md`
5. `docs/WP5_1_VERIFICATION.md`
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

WP5.0/D048, WP5.1/D049, WP5.2/D050 and WP5.3/D051 are complete.

WP5.3 canonical final verification:

- clean verification head `9c2b0a611b4e1d0c15c42aa8eb9259d368987340`
- run `40139154497`
- host job `121006986068`
- target job `121006985928`
- host artifact ID `12577038930`, SHA `2b632171295a72854b08b6451b122b670dac81304323e4c86af0ed460d3cccb1`
- target artifact ID `12578054556`, SHA `97a1a2f166c578a616d8f488e779b9905d630f20a2e5f1971542b8a2af0e29b3`
- PI32v2 `fmt_fm_ratio.o` 264 B, SHA `aa5bd8b9e1afa72054af2effeefbf468cfbb8b2b8d978e028540a30dc162956d`
- full target remains exact WP5.0
- evidence `docs/WP5_3_VERIFICATION.md`

D051 source-absent proof remains canonical at run `40138575267`, job `121005262360`.

## Frozen D051 core

Stable ratio IDs represent:

`0.5, 1.0, 1.25, 1.4140625, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 5.0, 6.0, 7.0, 8.0, 12.0, 16.0`

- unsigned Q8;
- base max `1293741834`;
- output ceiling `0x7fffffff`;
- invalid ID -> UNITY;
- deterministic 32-bit split-Q8 scaling;
- exhaustive reference SHA `5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c`;
- no runtime division/modulo/int64/float;
- D051 ceiling is not an anti-aliasing claim.

## Next package

**WP5.4 - FM4 operator level and modulation-index mapping**, issue #47.

Expected branch: `phase5/fm4-levels`.

D052 does **not** exist yet. Begin contract-first.

Exact first actions:

1. branch from the WP5.3 closeout checkpoint;
2. make #47 ACTIVE;
3. inspect D049 Q15 operator level semantics and D050 `level_q15[4]` / `mod_depth_q14` behavior across all four topologies;
4. decide the smallest musically useful carrier/modulator control model, including whether the current single global modulation depth is sufficient;
5. freeze stable internal control domains, zero/nominal/max, rounding, clamp and overflow/gain semantics;
6. define deterministic fixtures using all four D050 algorithms and representative D051 ratios;
7. freeze D052 plus source/realtime/target ceilings before production source;
8. pass a source-absent gate before implementation.

Do not pull feedback, envelopes, voice allocation/router integration, tracker-facing p-lock IDs/pages or audible mixed FM playback into WP5.4.

No physical FM-1 is available. `hardware_margin_claim=none`.
