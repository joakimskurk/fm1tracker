# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, `WP5_1_VERIFICATION.md`, `WP5_1_FM_OPERATOR_CONTRACT.md`, `WP5_0_VERIFICATION.md`, `D047_MIXED_ENGINE_ARCHITECTURE.md`, `WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md`, `CURRENT_DECISIONS.md`, `WORK_PACKAGES.md`, `EXECUTION_PROTOCOL.md`, `CURRENT_TEST_STATE.md`, `TEST_MATRIX.md`, and `ROADMAP.md` before changing code.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 and WP5.1 complete, WP5.2 contract-first work active - 2026-10-24**

Current work package: **WP5.2 - curated FM4 algorithm routing**  
Parent issue: **#42**  
Work-package issue: **#45**  
Current branch: `phase5/fm4-algorithms`  
WP5.1 canonical verification head: `52df09bf363ad5a9ad2d722a38734eaa580ecb90`  
WP5.1 canonical run: `40134823600`  
WP5.1 evidence: `docs/WP5_1_VERIFICATION.md`

Do not begin WP5.3 ratio/tuning work, WP5.4 final level/mod-index mapping, WP5.5 feedback, WP5.6 envelopes, WP5.7 voice integration, WP5.8 parameter pages or WP5.9 mixed audible integration while WP5.2 is active.

## Completed foundation

- Phase 1 clean Dinghy baseline: COMPLETE.
- Phase 2 tracker core: COMPLETE, 8 tracks x 64 steps x 16 patterns, exact 8 B `FmtStep`, integer transport and STEP REC.
- Phase 3 p-locks/effects/runtime ingress: COMPLETE, D027-D036.
- Phase 4 first complete 8-BIT family: COMPLETE, D037-D046.
- WP5.0 mixed-engine routing migration: COMPLETE, D047/D048.
- WP5.1 fixed-point FM operator primitive: COMPLETE, D049.

Canonical D046 exit: run `40124536375`, job `120963022289`, target 72,504 B image / 19,604 B RAM / 113,000 B pool.

Canonical WP5.0 exit: run `40129822006`, job `120978955451`, target 72,792 B image / 19,676 B RAM / 113,000 B pool.

Canonical WP5.1 exit: run `40134823600`, host job `120994003059`, target job `120994002842`.

## Accepted mixed-engine model

```text
tracker row/track
    -> instrument reference
    -> published instrument family + engine identity
    -> fixed per-track family router
    -> family-local engine/allocator
```

D048 remains authoritative:

- `FmtStep` remains 8 B;
- `FmtInstrumentHeader` remains 4 B;
- family compatibility values remain `0 = 8-BIT/chip`, `1 = FM/digital`;
- `FmtSongMeta.mode` is compatibility/default metadata, not a playback gate;
- instrument identity publication is 64 B;
- global family routing is exactly 8 B;
- static bounded dispatch only;
- one shared 2,048 B row-page cache remains;
- malformed/unsupported family or engine fails toward silence;
- non-applicable family-specific p-lock IDs are ignored, never reinterpreted.

## Completed WP5.1 / D049 operator baseline

Normative contract: `docs/WP5_1_FM_OPERATOR_CONTRACT.md`.  
Verification: `docs/WP5_1_VERIFICATION.md`.

Frozen and verified:

- `FmtFmOperator` = one `uint32_t phase`, exactly 4 B;
- one cycle = 2^32 phase units with natural unsigned wrap;
- sample-before-advance ordering;
- signed `int32_t` phase modulation in the same phase-unit scale;
- Q15 operator level `0..32767`;
- output `-32767..32767`;
- generated 257-entry positive quarter-wave Q15 table;
- table payload 514 B;
- table SHA-256 `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68`;
- 4,096-render SHA-256 `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66`;
- 65,536 half-cycle sign-symmetry probes;
- no loop/heap/float/64-bit/divide/modulo/store/router traversal in the primitive;
- pinned PI32v2 object text+rodata = 692 B;
- object SHA-256 `46868461ac6910d499975dbd93d06ee0af9db905d01c32583321eeaea0e33fad`.

WP5.1 remains intentionally isolated from `dinghy.c`. The running full application is therefore still exact WP5.0:

- image 72,792 B;
- normal RAM 19,676 B;
- pool 113,000 B;
- normal WAV `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`;
- representative WAV `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9`;
- BIN `a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125`;
- FWSC `cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073`.

## Active package - WP5.2 / issue #45

Goal: freeze and implement the smallest musically useful deterministic four-operator algorithm-routing layer on top of D049.

WP5.2 is **contract-first**. No production algorithm-routing source may be added until the D050 contract and reference vectors are committed.

### Exact next actions

1. inspect D049 operator semantics and the Phase 5 FM4 constraints;
2. define a small curated algorithm set rather than an unrestricted routing matrix;
3. freeze stable internal algorithm IDs and exact graph topology;
4. freeze exact per-sample operator evaluation order;
5. freeze the signed modulation accumulation/scaling boundary and overflow/saturation rules;
6. freeze carrier summing/output-normalization semantics;
7. freeze whether the routing layer needs any mutable state at all before feedback exists;
8. define deterministic graph fixtures and long-run hashes;
9. define target object/realtime ceilings;
10. write `docs/WP5_2_FM_ALGORITHM_CONTRACT.md` as D050 and a source-absent CI gate before production routing code.

### WP5.2 non-goals

- frequency ratio/tuning policy, WP5.3;
- final operator-level/modulation-index UX mapping, WP5.4;
- feedback, WP5.5;
- envelope strategy, WP5.6;
- FM voice allocation/D048 integration, WP5.7;
- FM parameter pages/p-lock IDs, WP5.8;
- simultaneous audible 8-BIT + FM4 integration, WP5.9.

## Important invariants

- eight logical tracks are never silently reduced;
- exact transport/effect timing remains authoritative;
- D034 remains an exact 32-entry SPSC queue;
- still-owned active cross-track primaries may not be stolen;
- D044 8-BIT parameter IDs remain stable;
- D048 per-instrument family/engine routing remains authoritative;
- D049 operator semantics remain the FM primitive compatibility baseline;
- no float in firmware DSP and no runtime 64-bit division.

## Hardware limitation

No physical FM-1 is available. Host/device-build evidence does not prove physical install/boot, sound quality, controls, screen feel or measured AC791N callback margin.

`hardware_margin_claim=none`

## Last updated

2026-10-24 - WP5.1/D049 closed green. WP5.2/#45 is the only active package and begins with D050 algorithm-contract design, not production routing code.
