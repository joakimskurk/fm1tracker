# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, `WP5_3_FM_RATIO_CONTRACT.md`, `WP5_2_VERIFICATION.md`, `WP5_2_FM_ALGORITHM_CONTRACT.md`, `WP5_1_VERIFICATION.md`, `WP5_1_FM_OPERATOR_CONTRACT.md`, `WP5_0_VERIFICATION.md`, `D047_MIXED_ENGINE_ARCHITECTURE.md`, `CURRENT_DECISIONS.md`, `WORK_PACKAGES.md`, `EXECUTION_PROTOCOL.md`, `CURRENT_TEST_STATE.md`, `TEST_MATRIX.md`, and `ROADMAP.md` before changing code.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0, WP5.1 and WP5.2 complete; WP5.3 D051 contract frozen, source-absent CI pending - 2026-10-25**

Current work package: **WP5.3 - FM4 integer ratio table and pitch mapping**  
Parent issue: **#42**  
Work-package issue: **#46**  
Current branch: `phase5/fm4-ratios`  
WP5.2 closeout baseline / current WP5.3 parent: `826a7dca7394d6e2c13f79fafa97bd05fb1986c8`  
WP5.2 canonical implementation head: `bc054bc697304afe94fe18b2990396e6a1667dbf`  
WP5.2 canonical run: `40136425976`  
WP5.2 evidence: `docs/WP5_2_VERIFICATION.md`  
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

## D049 operator baseline

- exact 4 B operator state;
- uint32 phase / 2^32 cycle / natural wrap;
- signed int32 phase modulation;
- Q15 level 0..32767;
- 257-entry generated quarter-wave, 514 B payload;
- table SHA `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68`;
- 4,096-render SHA `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66`;
- PI32v2 object text+rodata 692 B.

## D050 algorithm-routing baseline

Stable IDs:

```text
0 STACK4:    OP4 -> OP3 -> OP2 -> OP1 -> OUT
1 FAN3:      OP2 + OP3 + OP4 -> OP1 -> OUT
2 DUAL2:     OP4 -> OP3 plus OP2 -> OP1
3 PARALLEL4: OP1 + OP2 + OP3 + OP4
```

Frozen/verified:

- state exactly four D049 operators / 16 B;
- evaluation always OP4 -> OP3 -> OP2 -> OP1;
- every operator advances exactly once per sample;
- algorithm switching preserves phase continuity;
- internal mod depth 0..16383;
- edge mapping `sample * depth * 4`;
- multi-modulator offsets accumulate modulo 2^32;
- carrier counts 1/2/4 with sign-safe power-of-two normalization;
- no runtime graph traversal;
- PI32v2 router object text+rodata 362 B;
- router object SHA `ed85fe6ed70402bd3b0727e59b1c8eee21f6d15f264ec8dc439603968a38f469`.

The running application remains exact WP5.0 because D049/D050 are still isolated from `dinghy.c`.

## D051 ratio contract - frozen, not implemented

D051 now freezes the V1 ratio layer before production source exists.

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
- malformed larger base values clamp to that canonical maximum first;
- operator increment ceiling `0x7fffffff`;
- ratio scaling uses precomputed per-ID base boundaries plus 32-bit split Q8 round-half-up arithmetic;
- no runtime division/modulo, no 64-bit integer types, no float/double;
- invalid ratio ID -> UNITY;
- mapping is stateless;
- `map4` performs exactly four bounded mappings;
- exhaustive legal-pitch reference covers 16,384 pitches x 16 ratios = 262,144 outputs;
- frozen exhaustive SHA `5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c`;
- 27,615 exhaustive outputs intentionally saturate at the D051 ceiling;
- future PI32v2 `fmt_fm_ratio.o` text+rodata ceiling is 1,024 B.

D051 explicitly does **not** claim bandlimited FM. The ceiling prevents accidental oscillator-increment fold/wrap; later measured audio-quality policy remains separate.

## Active package - WP5.3 / issue #46

Current gate state: **D051 contract/test/workflow prepared for contract-first checkpoint. Production `fmt_fm_ratio.*` remains forbidden until the source-absent CI gate is green.**

### Exact next actions

1. commit the D051 contract, source-absent reference gate and handoff updates without production firmware changes;
2. verify the commit diff contains no `firmware/`, `firmware/gen` or production tool changes;
3. run `.github/workflows/wp5_3_contract_freeze.yml`;
4. require exact green exhaustive/hash/inherited results;
5. freeze that pre-implementation evidence in the repo;
6. only then create isolated `firmware/src/fmt_fm_ratio.h/.c`;
7. verify C behavior against the same D051 exhaustive reference and inherited D050/D049/WP5.0 baselines;
8. compile with pinned PI32v2 and require object text+rodata <= 1,024 B;
9. keep `dinghy.c` unchanged and full target application exact WP5.0;
10. write `docs/WP5_3_VERIFICATION.md` before WP5.4 begins.

## WP5.3 explicitly out of scope

- Yamaha-style free coarse/fine ratio editing or fixed-frequency mode;
- final operator level/mod-index UX mapping, WP5.4;
- feedback, WP5.5;
- envelope strategy, WP5.6;
- FM voice allocation/D048 integration, WP5.7;
- FM parameter pages/p-lock IDs, WP5.8;
- simultaneous audible 8-BIT + FM4, WP5.9;
- oversampling/bandlimiting decisions before measured need.

## Important invariants

Eight logical tracks remain. D048 routing/ownership, D049 operator semantics and D050 routing semantics are compatibility baselines. No float in firmware DSP, no runtime 64-bit division, no heap or unbounded realtime work.

## Hardware limitation

No physical FM-1 is available. `hardware_margin_claim=none`.

## Last updated

2026-10-25 - D051 ratio/pitch mapping contract frozen contract-first. Production ratio source remains absent and unauthorized until the D051 source-absent CI gate is green.
