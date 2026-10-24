# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, `WP5_2_FM_ALGORITHM_CONTRACT.md`, `WP5_1_VERIFICATION.md`, `WP5_1_FM_OPERATOR_CONTRACT.md`, `WP5_0_VERIFICATION.md`, `D047_MIXED_ENGINE_ARCHITECTURE.md`, `CURRENT_DECISIONS.md`, `WORK_PACKAGES.md`, `EXECUTION_PROTOCOL.md`, `CURRENT_TEST_STATE.md`, `TEST_MATRIX.md`, and `ROADMAP.md` before changing code.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0/WP5.1 complete, WP5.2 D050 contract frozen, source-absent gate pending - 2026-10-24**

Current work package: **WP5.2 - curated FM4 algorithm routing**  
Parent issue: **#42**  
Work-package issue: **#45**  
Current branch: `phase5/fm4-algorithms`  
WP5.1 closeout checkpoint: `bc71289690c8f9bd36e9cbbb378e0df3de1b9f25`  
WP5.1 canonical run: `40134823600`

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

WP5.2 must not alter D049 semantics:

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

**D050 is frozen before production FM algorithm-routing source.**

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
- phase increments and operator levels are already-resolved inputs;
- internal mod depth clamps to 0..16383;
- edge phase offset is exactly `sample * depth * 4`;
- multi-modulator phase offsets accumulate modulo 2^32;
- carrier counts are only 1, 2 or 4;
- multi-carrier output normalization uses sign-safe power-of-two truncation toward zero;
- routing uses an explicit four-case static dispatch, not runtime graph traversal;
- feedback state is absent until WP5.5;
- isolated router object `.text + .rodata` ceiling is 2,048 B;
- full app must remain exact WP5.0 while WP5.2 stays isolated.

Frozen 4,096-render hashes:

- STACK4 `25dfde1bb29de2c70305d6abafee91fd7480e69eaa71fd1a3fbde3c1e6e855cd`
- FAN3 `8d9ae5310369e3d5a36bcf8c44d5e74049d3d6e109362a844be2796b26330653`
- DUAL2 `c4c31cabf4b8e121591c4c1e0923aee67523eaf7d5b60b60ca1a545c13fc2269`
- PARALLEL4 `d316c3e5467cbfc606c788f669a3b19dba13535969dcce3b67b2821f0977a882`

All four algorithms finish the frozen 4,096-sample fixture with identical phases:

```text
OP1 0x4476A040
OP2 0x95C7F080
OP3 0xE71940C0
OP4 0x386A9000
```

### Exact next action

Run the committed D050 source-absent CI gate and require it to prove:

1. `fmt_fm_algorithm.h/.c` are absent;
2. D049 operator source/data are unchanged from WP5.1 closeout;
3. all four stable IDs/topologies/evaluation order reproduce the contract;
4. mod-depth anchors and exact sign mirrors pass;
5. modulo-2^32 phase-offset accumulation anchors pass;
6. carrier normalization anchors pass;
7. all frozen first-12 output vectors pass;
8. all four 4,096-render hashes/final phases pass;
9. 65,536 renders per algorithm keep output bounded and phase advancement exact;
10. invalid IDs reproduce STACK4 exactly;
11. D049 production vectors remain exact;
12. normal WP5.0 WAV and inherited representative baseline remain exact.

Only after this gate is green may `fmt_fm_algorithm.*` production source be added.

## WP5.2 explicitly out of scope

- ratio/tuning policy, WP5.3;
- final level/mod-index mapping, WP5.4;
- feedback, WP5.5;
- envelope strategy, WP5.6;
- FM voice allocation/D048 integration, WP5.7;
- FM parameter pages/p-lock IDs, WP5.8;
- simultaneous audible 8-BIT + FM4, WP5.9.

## Important invariants

- eight logical tracks are never silently reduced;
- tracker timing/p-lock timing remain deterministic;
- D034 remains an exact 32-entry queue;
- no active cross-track primary stealing;
- D048 remains authoritative for family routing/ownership;
- D049 remains authoritative for each operator;
- no float in firmware DSP and no runtime 64-bit division.

## Hardware limitation

No physical FM-1 is available. Host/device-build evidence does not prove physical install/boot, sound quality, controls, screen feel or measured AC791N callback margin.

`hardware_margin_claim=none`

## Last updated

2026-10-24 - D050 algorithm-routing contract and source-absent reference gate prepared. The contract gate is the only next action before production WP5.2 routing implementation.
