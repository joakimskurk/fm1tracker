# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, `WP5_2_VERIFICATION.md`, `WP5_2_FM_ALGORITHM_CONTRACT.md`, `WP5_1_VERIFICATION.md`, `WP5_1_FM_OPERATOR_CONTRACT.md`, `WP5_0_VERIFICATION.md`, `D047_MIXED_ENGINE_ARCHITECTURE.md`, `CURRENT_DECISIONS.md`, `WORK_PACKAGES.md`, `EXECUTION_PROTOCOL.md`, `CURRENT_TEST_STATE.md`, `TEST_MATRIX.md`, and `ROADMAP.md` before changing code.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0, WP5.1 and WP5.2 complete; WP5.3 contract-first work active - 2026-10-24**

Current work package: **WP5.3 - FM4 integer ratio table and pitch mapping**  
Parent issue: **#42**  
Work-package issue: **#46**  
Current branch: `phase5/fm4-ratios`  
WP5.2 canonical implementation head: `bc054bc697304afe94fe18b2990396e6a1667dbf`  
WP5.2 canonical run: `40136425976`  
WP5.2 evidence: `docs/WP5_2_VERIFICATION.md`

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

## Active package - WP5.3 / issue #46

Goal: freeze and implement the bounded integer mapping from one canonical base phase increment to four operator phase increments through stable musically useful ratio IDs.

WP5.3 is **contract-first**. No production ratio source may be added until D051 and a source-absent reference gate are committed and green.

### Exact next actions

1. inspect `fmt_engine_pitch.*`, D049 phase/increment conventions and D050 input requirements;
2. choose a compact musically useful ratio set, not arbitrary rational runtime math;
3. freeze stable internal ratio IDs and exact mathematical meaning;
4. freeze integer transform arithmetic, rounding and clamp/overflow behavior;
5. freeze the supported base-increment domain and high-register edge policy;
6. decide explicitly whether sub-unity/fractional ratios belong in V1 FM4;
7. freeze deterministic low/mid/high pitch fixtures and boundary vectors;
8. freeze one-base-increment to four-operator mapping API/state requirements;
9. define source/realtime and pinned PI32v2 object ceilings;
10. write `docs/WP5_3_FM_RATIO_CONTRACT.md` as D051 and a source-absent CI gate before implementation.

## WP5.3 explicitly out of scope

- final operator level/mod-index UX mapping, WP5.4;
- feedback, WP5.5;
- envelope strategy, WP5.6;
- FM voice allocation/D048 integration, WP5.7;
- FM parameter pages/p-lock IDs, WP5.8;
- simultaneous audible 8-BIT + FM4, WP5.9.

## Important invariants

Eight logical tracks remain. D048 routing/ownership, D049 operator semantics and D050 routing semantics are compatibility baselines. No float in firmware DSP, no runtime 64-bit division, no heap or unbounded realtime work.

## Hardware limitation

No physical FM-1 is available. `hardware_margin_claim=none`.

## Last updated

2026-10-24 - WP5.2/D050 closed green. WP5.3/#46 is the only active package and begins with D051 ratio-contract design, not production ratio code.
