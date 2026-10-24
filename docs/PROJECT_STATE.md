# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, `WP5_1_FM_OPERATOR_CONTRACT.md`, `WP5_1_PREIMPLEMENTATION_EVIDENCE.md`, `WP5_0_VERIFICATION.md`, `D047_MIXED_ENGINE_ARCHITECTURE.md`, `WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md`, `CURRENT_DECISIONS.md`, `WORK_PACKAGES.md`, `EXECUTION_PROTOCOL.md`, `CURRENT_TEST_STATE.md`, `TEST_MATRIX.md`, and `ROADMAP.md` before changing code.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 complete, WP5.1 active, D049 frozen and pre-implementation gate GREEN - 2026-10-24**

Current work package: **WP5.1 - fixed-point FM operator primitive**  
Parent issue: **#42**  
Work-package issue: **#44**  
Current branch: `phase5/fm4-operator`  
WP5.0 closeout/handoff: `6c0add8828474e9308710e02957231e7225eb121`  
D049 contract commit: `f9b97edeb66483199fdf5bbfc964839f31841dc0`  
D049 canonical pre-implementation head: `010d1dff2f74e27450edc6784ae7792eca358e59`

Do not begin WP5.2 algorithm routing, feedback, envelopes, voice allocation, parameter pages or mixed FM playback while WP5.1 is active.

## Completed foundation

- Phase 1 clean Dinghy baseline: COMPLETE.
- Phase 2 tracker core: COMPLETE, 8 tracks x 64 steps x 16 patterns, exact 8 B `FmtStep`, integer transport and STEP REC.
- Phase 3 p-locks/effects/runtime ingress: COMPLETE, D027-D036.
- Phase 4 first complete 8-BIT family: COMPLETE, D037-D046.
- WP5.0 mixed-engine routing migration: COMPLETE, D047/D048.

Canonical D046 exit: run `40124536375`, job `120963022289`, target 72,504 B image / 19,604 B RAM / 113,000 B pool.

Canonical WP5.0 exit: run `40129822006`, job `120978955451`, target 72,792 B image / 19,676 B RAM / 113,000 B pool, delta +288 / +72 / +0 B versus D046.

## Accepted mixed-engine model

```text
tracker row/track
    -> instrument reference
    -> published instrument family + engine identity
    -> fixed per-track family router
    -> family-local engine/allocator
```

D048 remains authoritative: `FmtStep` 8 B, `FmtInstrumentHeader` 4 B, 64 B instrument publication, 8 B global family route, static bounded dispatch, song mode non-gating, one shared 2,048 B row cache, malformed/unsupported family or engine fails toward silence, non-applicable family-specific p-locks are ignored rather than reinterpreted.

## Active package - WP5.1 / D049 / issue #44

Normative contract: `docs/WP5_1_FM_OPERATOR_CONTRACT.md`.  
Pre-implementation evidence: `docs/WP5_1_PREIMPLEMENTATION_EVIDENCE.md`.

D049 freezes:

- `FmtFmOperator` is exactly one `uint32_t phase`, 4 B;
- one cycle = 2^32 phase units with natural unsigned wrap;
- sample current carrier phase before phase advance;
- signed `int32_t` phase modulation uses the same phase-unit scale modulo 2^32;
- operator level clamps to Q15 `0..32767`;
- carrier uses a generated 257-entry positive quarter-wave Q15 table with 22-bit linear interpolation;
- table payload is 514 B immutable data;
- table binary SHA-256 `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68`;
- frozen 4,096-render SHA-256 `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66`;
- render primitive has no loop/search/allocation/divide/modulo/float/64-bit arithmetic;
- WP5.1 remains isolated from `dinghy.c`, so the full WP5.0 app must remain byte-identical;
- isolated target object text+immutable data ceiling is 4,096 B.

### D049 pre-implementation gate - GREEN

Canonical run `40131135259`, job `120982895786`, artifact `12574973209`, artifact SHA-256 `ae74fcd6272eafffdf62a27ef0f9fd415941f1c8024901c4344cb19a0212f83b`.

The gate proved the production operator was absent, zero firmware/gen delta versus WP5.0, the frozen table hash, all sine/interpolation/modulation/level vectors, 65,536 sign-symmetry probes, the 4,096-render hash/final phase and byte-identical WP5.0 normal WAV.

### Exact next action

Production implementation is now allowed, but remains isolated:

1. add deterministic generator and checked-in quarter-wave table;
2. add `fmt_fm_operator.h/.c` matching D049 exactly;
3. add focused C host vectors and source audit;
4. independently compile the module with the pinned JieLi PI32v2 toolchain and measure object sections;
5. prove full WP5.0 WAV/resources/BIN/FWSC remain exact because `dinghy.c` is unchanged;
6. write `WP5_1_VERIFICATION.md` before WP5.2 begins.

## WP5.1 explicitly out of scope

- algorithm routing, WP5.2
- ratio table/pitch policy, WP5.3
- final operator-level/modulation-index mappings, WP5.4
- feedback, WP5.5
- FM envelope strategy, WP5.6
- FM voice allocation/router integration, WP5.7
- FM parameter pages/p-lock mapping, WP5.8
- simultaneous production 8-BIT + FM4 playback, WP5.9

## Important invariants

Eight logical tracks remain. Exact transport, D032 six-tick timing, D034 32-entry queue, D043 ownership, D044 8-BIT IDs and D048 per-instrument routing may not regress. No float in firmware DSP and no runtime 64-bit division.

## Product direction

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted core direction: tracker depth underneath, immediate performance/capture on top.

## Hardware limitation

No physical FM-1 is available. Host/device-build evidence does not prove physical install/boot, sound quality, controls, screen feel or measured AC791N callback margin.

`hardware_margin_claim=none`

## Last updated

2026-10-24 - D049 pre-implementation gate is canonically green. The isolated FM operator implementation is the only next action.
