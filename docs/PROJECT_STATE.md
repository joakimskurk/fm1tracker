# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, `WP5_3_PREIMPLEMENTATION_EVIDENCE.md`, `WP5_3_FM_RATIO_CONTRACT.md`, `WP5_2_VERIFICATION.md`, `WP5_1_VERIFICATION.md`, `WP5_0_VERIFICATION.md`, `D047_MIXED_ENGINE_ARCHITECTURE.md`, `CURRENT_DECISIONS.md`, `WORK_PACKAGES.md`, `EXECUTION_PROTOCOL.md`, `CURRENT_TEST_STATE.md`, `TEST_MATRIX.md`, and `ROADMAP.md` before changing code.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0, WP5.1 and WP5.2 complete; WP5.3 D051 pre-implementation gate GREEN, isolated implementation authorized - 2026-10-25**

Current work package: **WP5.3 - FM4 integer ratio table and pitch mapping**  
Parent issue: **#42**  
Work-package issue: **#46**  
Current branch: `phase5/fm4-ratios`  
WP5.2 closeout baseline: `826a7dca7394d6e2c13f79fafa97bd05fb1986c8`  
D051 contract checkpoint: `e3b1272a442091dd9cc140f3d6d357f6215438b5`  
D051 source-absent run: `40138575267`  
D051 source-absent job: `121005262360`  
D051 evidence: `docs/WP5_3_PREIMPLEMENTATION_EVIDENCE.md`  
WP5.3 normative contract: `docs/WP5_3_FM_RATIO_CONTRACT.md`

Do not begin WP5.4 final level/mod-index mapping, WP5.5 feedback, WP5.6 envelopes, WP5.7 voice integration, WP5.8 parameter pages or WP5.9 audible mixed integration while WP5.3 is active.

## Completed baseline

- Phase 1 platform baseline: COMPLETE.
- Phase 2 tracker core: COMPLETE.
- Phase 3 p-locks/effects/runtime ingress: COMPLETE.
- Phase 4 / D046 first complete 8-BIT family: COMPLETE.
- WP5.0 / D048 mixed-engine routing: COMPLETE.
- WP5.1 / D049 fixed-point FM operator: COMPLETE.
- WP5.2 / D050 curated four-operator routing: COMPLETE.

Canonical WP5.0 exit: run `40129822006`, job `120978955451`, target 72,792 B image / 19,676 B RAM / 113,000 B pool.

Canonical WP5.1 exit: run `40134823600`, host `120994003059`, target `120994002842`.

Canonical WP5.2 exit: run `40136425976`, host `120998804031`, target `120998804175`.

## D051 ratio contract

Stable ratio IDs 0..15:

```text
0  0.5
1  1.0    UNITY / invalid-ID fallback
2  1.25
3  1.4140625 exact Q8 value 362/256
4  1.5
5  2.0
6  2.5
7  3.0
8  3.5
9  4.0
10 5.0
11 6.0
12 7.0
13 8.0
14 12.0
15 16.0
```

Frozen numeric policy:

- ratio representation unsigned Q8;
- canonical base-increment maximum `1293741834` from D038 at `pitch_q7=16383`;
- malformed larger base values clamp to that maximum first;
- operator increment ceiling `0x7fffffff`;
- precomputed per-ID base boundaries plus 32-bit split-Q8 round-half-up arithmetic;
- no runtime division/modulo, 64-bit integer types or float/double;
- invalid ratio ID -> UNITY;
- mapping is stateless;
- `map4` performs exactly four bounded mappings;
- exhaustive reference covers 16,384 pitches x 16 ratios = 262,144 outputs;
- exhaustive SHA `5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c`;
- 27,615 exhaustive outputs intentionally saturate at the D051 ceiling;
- PI32v2 `fmt_fm_ratio.o` text+rodata ceiling is 1,024 B.

D051 explicitly does not claim bandlimited FM.

## D051 pre-implementation gate

Canonical source-absent evidence is GREEN:

- run `40138575267`
- job `121005262360`
- head `e3b1272a442091dd9cc140f3d6d357f6215438b5`
- artifact `wp5-3-d051-contract-freeze`
- artifact ID `12577928512`
- artifact SHA `f6ff75f9dd2128f3d79cffd467ee1b88b1422a5daa788516387b7e9c50e19014`
- source absent and D038/D049/D050/`dinghy.c` unchanged
- Q8/boundary hashes exact
- all clamp anchors exact
- exhaustive 262,144-output hash exact
- inherited D050/D049/WP5.0 host regressions exact
- `hardware_margin_claim=none`

## Active package - exact next actions

Isolated production ratio implementation is now authorized under D051.

1. preserve the green pre-implementation evidence as a docs-only checkpoint;
2. add only `firmware/src/fmt_fm_ratio.h/.c` under production firmware;
3. add focused host C verification against the already frozen D051 reference;
4. add pinned PI32v2 object measurement and full-target identity verification;
5. keep `dinghy.c` unchanged;
6. require `fmt_fm_ratio.o` text+rodata <= 1,024 B;
7. require D038/D049/D050 and WP5.0 exact regressions;
8. write `docs/WP5_3_VERIFICATION.md` and close #46 before WP5.4 starts.

## WP5.3 explicitly out of scope

- Yamaha-style free coarse/fine ratio editing or fixed-frequency mode;
- final operator level/mod-index UX mapping, WP5.4;
- feedback, WP5.5;
- envelope strategy, WP5.6;
- FM voice allocation/D048 integration, WP5.7;
- FM parameter pages/p-lock IDs, WP5.8;
- simultaneous audible 8-BIT + FM4, WP5.9;
- oversampling/bandlimiting decisions before measured need.

## Hardware limitation

No physical FM-1 is available. `hardware_margin_claim=none`.

## Last updated

2026-10-25 - canonical D051 source-absent gate GREEN. Isolated `fmt_fm_ratio.*` implementation is authorized, but no application integration is allowed in WP5.3.
