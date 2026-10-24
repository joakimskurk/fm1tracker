# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, `WP5_0_VERIFICATION.md`, `D047_MIXED_ENGINE_ARCHITECTURE.md`, `WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md`, `WP5_1_FM_OPERATOR_CONTRACT.md`, `CURRENT_DECISIONS.md`, `WORK_PACKAGES.md`, `EXECUTION_PROTOCOL.md`, `CURRENT_TEST_STATE.md`, `TEST_MATRIX.md`, and `ROADMAP.md` before changing code.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 complete, WP5.1 active, D049 contract frozen, pre-implementation CI gate pending - 2026-10-24**

Current work package: **WP5.1 - fixed-point FM operator primitive**  
Parent issue: **#42**  
Work-package issue: **#44**  
Current branch: `phase5/fm4-operator`  
WP5.0 closeout/handoff: `6c0add8828474e9308710e02957231e7225eb121`  
WP5.0 production verification head: `813792fb25632b4d7acda787422e51f713fad21e`

Do not begin WP5.2 algorithm routing, feedback, envelopes, voice allocation, parameter pages or mixed FM playback while WP5.1 is active.

## Completed foundation

- Phase 1 clean Dinghy baseline: COMPLETE.
- Phase 2 tracker core: COMPLETE, 8 tracks x 64 steps x 16 patterns, exact 8 B `FmtStep`, integer transport and STEP REC.
- Phase 3 p-locks/effects/runtime ingress: COMPLETE, D027-D036.
- Phase 4 first complete 8-BIT family: COMPLETE, D037-D046.
- WP5.0 mixed-engine routing migration: COMPLETE, D047/D048.

Canonical D046 Phase 4 exit: run `40124536375`, job `120963022289`, target 72,504 B image / 19,604 B RAM / 113,000 B pool.

Canonical WP5.0 exit: run `40129822006`, job `120978955451`, target 72,792 B image / 19,676 B RAM / 113,000 B pool, delta +288 / +72 / +0 B versus D046.

WP5.0 evidence: `docs/WP5_0_VERIFICATION.md`.

## Accepted mixed-engine model

```text
tracker row/track
    -> instrument reference
    -> published instrument family + engine identity
    -> fixed per-track family router
    -> family-local engine/allocator
```

Frozen D048 facts:

- `FmtStep` remains 8 B.
- `FmtInstrumentHeader` remains 4 B.
- family compatibility values remain `0 = 8-BIT/chip`, `1 = FM/digital`.
- `FmtSongMeta.mode` remains physically present but no longer gates playback/publication.
- instrument identity publication is fixed 64 B.
- global route ownership is exactly 8 B, one family byte per logical track.
- static bounded dispatch only, no runtime plugin registry or heap.
- one existing 2,048 B row-page cache remains shared.
- unsupported/malformed family or engine fails safely toward silence.
- non-applicable family-specific p-lock IDs are ignored, never reinterpreted.
- D046 all-8-BIT representative and normal WAVs remain byte-identical.

## Active package - WP5.1 / D049 / issue #44

Normative contract: `docs/WP5_1_FM_OPERATOR_CONTRACT.md`.

**D049 is frozen before production FM operator source.**

Frozen primitive:

- `FmtFmOperator` owns exactly one `uint32_t phase`, 4 B total.
- one cycle is 2^32 phase units with natural unsigned wrap.
- current carrier phase is sampled before phase advance.
- signed `int32_t` phase modulation uses the same phase-unit scale and adds modulo 2^32.
- operator level clamps to Q15 `0..32767`, matching the existing envelope maximum.
- carrier sine is a generated 257-entry positive quarter-wave Q15 table with 22-bit linear interpolation.
- table payload is 514 B immutable data.
- frozen table binary SHA-256: `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68`.
- frozen 4,096-render reference SHA-256: `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66`.
- no loop/search/allocation/divide/modulo/float/64-bit arithmetic is allowed in the render primitive.
- WP5.1 remains isolated from `dinghy.c`, so full WP5.0 WAV/BIN/FWSC/resources must remain byte-identical.
- isolated target object `.text + immutable table/rodata` ceiling is 4,096 B.

### Exact next action

Run the committed D049 contract-freeze CI gate and require it to prove:

- FM operator production source/header absent;
- no `dinghy.c` operator include;
- zero firmware/gen delta versus WP5.0 handoff;
- frozen table hash reproduced;
- frozen sine/interpolation/modulation/level/render-order vectors reproduced;
- 65,536 deterministic half-cycle symmetry probes pass;
- frozen 4,096-render hash/final phase reproduced;
- normal full-app WP5.0 WAV remains byte-identical.

Only after that gate is green may `fmt_fm_operator.*` and its generated table source be implemented.

## WP5.1 explicitly out of scope

- algorithm routing, WP5.2
- ratio table/pitch policy, WP5.3
- final operator-level/modulation-index mappings, WP5.4
- feedback, WP5.5
- FM envelope strategy, WP5.6
- FM voice allocation/router integration, WP5.7
- FM parameter pages/p-lock mapping, WP5.8
- simultaneous production 8-BIT + FM4 playback, WP5.9

## Phase 5 sequence after WP5.1

WP5.2 algorithms -> WP5.3 ratios -> WP5.4 levels/mod index -> WP5.5 feedback -> WP5.6 envelope -> WP5.7 voice/router integration -> WP5.8 parameter pages/p-locks -> WP5.9 mixed validation -> WP5.10 worst-case Phase 5 exit.

## Important invariants

- eight logical tracks are never silently reduced;
- exact integer transport and next-frame row-boundary ordering remain authoritative;
- semantic parameter IDs remain separate from tracker-effect IDs;
- p-lock storage/editor behavior stays bounded and corruption-safe;
- D032 remains one exact six-tick tracker clock;
- D034 remains an exact 32-entry SPSC queue;
- still-owned active cross-track primary voices may not be stolen;
- D044 8-BIT IDs `0x20..0x25` remain stable;
- D048 per-instrument family/engine routing remains authoritative;
- no float in firmware DSP and no runtime 64-bit division.

## Product direction

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted core direction: tracker depth underneath, immediate performance/capture on top. Mixed-engine routing and FM4 are engine foundations, not permission to pull live-capture work into WP5.1.

## Hardware limitation

No physical FM-1 is available. Host/device-build evidence does not prove physical install/boot, sound quality, controls, screen feel or measured AC791N callback margin.

`hardware_margin_claim=none`

## Last updated

2026-10-24 - WP5.0 is closed. D049/WP5.1 contract is frozen. The contract-first CI gate is the only next action before production FM operator implementation.
