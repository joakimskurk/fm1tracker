# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, `WP5_2_PREIMPLEMENTATION_EVIDENCE.md`, `WP5_2_FM_ALGORITHM_CONTRACT.md`, `WP5_1_VERIFICATION.md`, `WP5_1_FM_OPERATOR_CONTRACT.md`, `WP5_0_VERIFICATION.md`, `D047_MIXED_ENGINE_ARCHITECTURE.md`, `CURRENT_DECISIONS.md`, `WORK_PACKAGES.md`, `EXECUTION_PROTOCOL.md`, `CURRENT_TEST_STATE.md`, `TEST_MATRIX.md`, and `ROADMAP.md` before changing code.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0/WP5.1 complete, WP5.2 D050 source-absent gate green, production routing implementation allowed - 2026-10-24**

Current work package: **WP5.2 - curated FM4 algorithm routing**  
Parent issue: **#42**  
Work-package issue: **#45**  
Current branch: `phase5/fm4-algorithms`  
WP5.1 closeout checkpoint: `bc71289690c8f9bd36e9cbbb378e0df3de1b9f25`  
D050 contract checkpoint: `f2d328f516bb66517d1e06d98aab1f3c09dcc68b`  
D050 source-absent run: `40135979431`  
D050 source-absent job: `120997461479`

Do not begin WP5.3 ratio/tuning work, WP5.4 final level/mod-index mapping, WP5.5 feedback, WP5.6 envelopes, WP5.7 voice integration, WP5.8 parameter pages or WP5.9 audible mixed integration while WP5.2 is active.

## Completed baseline

- Phase 1 platform baseline: COMPLETE.
- Phase 2 tracker core: COMPLETE.
- Phase 3 p-locks/effects/runtime ingress: COMPLETE.
- Phase 4 first complete 8-BIT family: COMPLETE, D037-D046.
- WP5.0 mixed-engine routing: COMPLETE, D047/D048.
- WP5.1 fixed-point FM operator primitive: COMPLETE, D049.

Canonical D046 exit: run `40124536375`, job `120963022289`.

Canonical WP5.0 exit: run `40129822006`, job `120978955451`, target 72,792 B image / 19,676 B RAM / 113,000 B pool.

Canonical WP5.1 exit: run `40134823600`, host job `120994003059`, target job `120994002842`, evidence `WP5_1_VERIFICATION.md`.

## D049 operator compatibility baseline

WP5.2 may not alter D049 semantics:

- `FmtFmOperator` exactly 4 B, one uint32 phase;
- 2^32 phase cycle with natural wrap;
- signed int32 phase modulation;
- Q15 level 0..32767;
- generated 257-entry quarter-wave table, 514 B payload;
- table SHA `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68`;
- 4,096-render SHA `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66`;
- isolated PI32v2 object text+rodata 692 B.

The running application remains exact WP5.0 because FM DSP is still isolated.

## Active package - WP5.2 / D050 / issue #45

Normative contract: `docs/WP5_2_FM_ALGORITHM_CONTRACT.md`.  
Pre-implementation evidence: `docs/WP5_2_PREIMPLEMENTATION_EVIDENCE.md`.

D050 source-absent gate is **GREEN**:

- run `40135979431`
- job `120997461479`
- head `f2d328f516bb66517d1e06d98aab1f3c09dcc68b`
- artifact ID `12577609841`
- artifact SHA-256 `d3465c39b17416d58c56d98c196a18ae1c4dbd6e07d2ab7cc4ca2b3992cab85`

Production `fmt_fm_algorithm.*` may now be implemented, but must remain isolated from `dinghy.c` and match D050 exactly.

### Frozen algorithm set

```text
ID 0 STACK4:    OP4 -> OP3 -> OP2 -> OP1 -> OUT
ID 1 FAN3:      OP2 + OP3 + OP4 -> OP1 -> OUT
ID 2 DUAL2:     OP4 -> OP3 plus OP2 -> OP1, two carriers
ID 3 PARALLEL4: OP1 + OP2 + OP3 + OP4, four carriers
```

Frozen routing rules:

- operator array index 0..3 = OP1..OP4;
- every algorithm evaluates OP4 -> OP3 -> OP2 -> OP1;
- all four operators render exactly once per sample;
- routing state is exactly four D049 operators, 16 B total, no extra mutable history;
- algorithm changes do not reset/pause phase;
- invalid algorithm IDs sanitize to STACK4;
- internal mod depth clamps to 0..16383;
- edge phase offset is exactly `sample * depth * 4`;
- multiple phase offsets accumulate modulo 2^32;
- carrier counts are only 1, 2 or 4;
- multi-carrier normalization uses sign-safe power-of-two truncation toward zero;
- explicit four-case static dispatch only;
- feedback state is absent until WP5.5;
- isolated router object `.text + .rodata` ceiling is 2,048 B;
- full app must remain exact WP5.0 while WP5.2 stays isolated.

Frozen 4,096-render hashes:

- STACK4 `25dfde1bb29de2c70305d6abafee91fd7480e69eaa71fd1a3fbde3c1e6e855cd`
- FAN3 `8d9ae5310369e3d5a36bcf8c44d5e74049d3d6e109362a844be2796b26330653`
- DUAL2 `c4c31cabf4b8e121591c4c1e0923aee67523eaf7d5b60b60ca1a545c13fc2269`
- PARALLEL4 `d316c3e5467cbfc606c788f669a3b19dba13535969dcce3b67b2821f0977a882`

### Exact next action

Implement the isolated D050 production routing primitive and focused verification:

1. add `firmware/src/fmt_fm_algorithm.h/.c` only;
2. keep `dinghy.c`, D048 router and D049 operator source unchanged;
3. implement exact 16 B state/reset/retrigger/null semantics;
4. implement algorithm/depth sanitizers, safe modulo phase-offset addition and sign-safe carrier normalization;
5. implement four unrolled algorithm cases with fixed OP4 -> OP3 -> OP2 -> OP1 evaluation;
6. add focused C tests reproducing all D050 vectors/hashes/stress and algorithm-switch phase continuity;
7. add source audit proving no loop/heap/float/int64/divide/modulo/store/router access in render;
8. compile/measure isolated PI32v2 router object <= 2,048 B;
9. re-prove D049 and full WP5.0 host/target identity;
10. only after green implementation CI write `WP5_2_VERIFICATION.md` and close WP5.2.

## WP5.2 explicitly out of scope

- ratio/tuning policy, WP5.3;
- final level/mod-index mapping, WP5.4;
- feedback, WP5.5;
- envelope strategy, WP5.6;
- FM voice allocation/D048 integration, WP5.7;
- FM parameter pages/p-lock IDs, WP5.8;
- simultaneous audible 8-BIT + FM4, WP5.9.

## Hardware limitation

No physical FM-1 is available. `hardware_margin_claim=none`.

## Last updated

2026-10-24 - D050 source-absent gate is green. Isolated production algorithm-routing implementation is the only next action.
