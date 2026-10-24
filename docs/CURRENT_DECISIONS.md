# FM1Tracker Current Decision Index

Date: 2026-10-24

`DECISIONS.md` remains the append-only historical decision log through D046. Historical entries are not rewritten when later decisions supersede constraints.

D047-D050 are the current accepted Phase 5 architecture/implementation decisions. D047/D048 are implemented through WP5.0. D049 is implemented and verified through WP5.1. D050 is frozen before WP5.2 production algorithm-routing source.

## D047 - mixed-engine instrument architecture

Normative document: `D047_MIXED_ENGINE_ARCHITECTURE.md`

Status: **ACCEPTED**

D047 supersedes D004's future project-wide 8-BIT/16-BIT restriction and older statements that mixed engine families are Phase 9-only architecture. It does not invalidate historical D005-D046 behavior or evidence.

## D048 - mixed-engine routing implementation contract

Normative document: `WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md`

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**

Verification: `WP5_0_VERIFICATION.md`, run `40129822006`, job `120978955451`.

Frozen D048 interpretation:

- tracker steps reference instruments;
- each instrument owns family + engine identity;
- `FmtSongMeta.mode` is compatibility/default metadata, not a playback gate;
- fixed 64 B instrument identity publication;
- fixed 8 B global per-track family route;
- family-local allocators own local voices;
- cross-family NOTE clears/hard-silences old primary before new dispatch and commits only on success;
- OFF/CUT clear global ownership;
- TIE/EMPTY/RETRIG do not allocate/change family;
- dispatch is static/bounded with no runtime plugin registry or heap;
- one existing 2,048 B row-page cache remains shared;
- non-applicable family-specific p-locks are ignored, never reinterpreted;
- complete D046 all-8-BIT behavior remains the regression baseline.

## D049 - fixed-point FM operator primitive

Normative document: `WP5_1_FM_OPERATOR_CONTRACT.md`

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**

Package: WP5.1 / issue #44.  
Verification: `WP5_1_VERIFICATION.md`, run `40134823600`, host job `120994003059`, target job `120994002842`.

D049 freezes:

- `FmtFmOperator` exactly one `uint32_t phase`, 4 B;
- one phase cycle = 2^32 with natural unsigned wrap;
- sample-current-before-advance;
- signed `int32_t` phase modulation in the same phase-unit domain;
- Q15 level `0..32767`;
- generated 257-entry positive quarter-wave Q15 carrier table;
- 22-bit interpolation;
- output `-32767..32767`;
- table SHA-256 `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68`;
- 4,096-render SHA-256 `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66`;
- no loop/search/allocation/divide/modulo/float/64-bit arithmetic in the primitive;
- pinned PI32v2 object text+rodata 692 B;
- full application remains exact WP5.0 because the operator is isolated.

## D050 - curated four-operator FM routing

Normative document: `WP5_2_FM_ALGORITHM_CONTRACT.md`

Status: **ACCEPTED / FROZEN BEFORE PRODUCTION FM ALGORITHM SOURCE**

Package: WP5.2 / issue #45.

D050 freezes a fixed four-algorithm subset rather than an unrestricted routing matrix:

```text
ID 0 STACK4:    OP4 -> OP3 -> OP2 -> OP1 -> OUT
ID 1 FAN3:      OP2 + OP3 + OP4 -> OP1 -> OUT
ID 2 DUAL2:     OP4 -> OP3 plus OP2 -> OP1, two carriers
ID 3 PARALLEL4: OP1 + OP2 + OP3 + OP4, four carriers
```

Frozen routing semantics:

- operator array index 0..3 means OP1..OP4;
- every algorithm evaluates exactly OP4 -> OP3 -> OP2 -> OP1;
- all four D049 operators render exactly once per sample in every algorithm;
- routing state is exactly four `FmtFmOperator` states, 16 B, with no extra mutable history;
- algorithm change itself never resets or pauses phase;
- invalid algorithm IDs sanitize to STACK4;
- caller supplies already-resolved operator phase increments and D049 levels;
- WP5.3 later owns ratio/tuning mapping;
- WP5.4 later owns user-facing level/mod-index mapping;
- internal routing depth is `0..16383`, exact edge formula `sample * depth * 4`;
- edge conversion stays inside signed 32-bit by construction;
- multiple phase offsets accumulate modulo 2^32, never by signed saturation;
- carrier counts are deliberately only 1, 2 or 4;
- two/four-carrier normalization uses sign-safe power-of-two truncation toward zero;
- algorithm dispatch is an explicit bounded four-case implementation, not graph traversal;
- no feedback state exists before WP5.5;
- router-only target object `.text + .rodata` ceiling is 2,048 B;
- WP5.2 remains isolated from `dinghy.c`, so full WP5.0 target/audio identity must remain exact.

Frozen 4,096-render hashes:

- STACK4 `25dfde1bb29de2c70305d6abafee91fd7480e69eaa71fd1a3fbde3c1e6e855cd`
- FAN3 `8d9ae5310369e3d5a36bcf8c44d5e74049d3d6e109362a844be2796b26330653`
- DUAL2 `c4c31cabf4b8e121591c4c1e0923aee67523eaf7d5b60b60ca1a545c13fc2269`
- PARALLEL4 `d316c3e5467cbfc606c788f669a3b19dba13535969dcce3b67b2821f0977a882`

D050 does **not** decide:

- frequency ratios/tuning, WP5.3;
- final level/mod-index controls, WP5.4;
- feedback, WP5.5;
- envelope strategy, WP5.6;
- FM voice allocation/router integration, WP5.7;
- FM parameter pages/p-lock IDs, WP5.8;
- simultaneous production mixed-family FM playback, WP5.9.

## Current interpretation

- FM1Tracker is one engine-agnostic tracker/sequencer.
- Instruments own family + engine identity.
- D048 remains authoritative for common family routing.
- D049 is the exact FM single-operator primitive compatibility baseline.
- D050 is the exact isolated four-operator connection/evaluation baseline.
- WP5.2 production routing code is forbidden until the D050 source-absent reference gate is green.
- The complete D046 8-BIT family remains the sound/regression baseline.
- No physical timing-margin claim is permitted without physical FM-1 evidence.

## Precedence rule

For historical checkpoints, use the decision/contract that governed that checkpoint.

For future sound-engine architecture after Phase 4, D047 supersedes D004. D048 governs common family routing/ownership. D049 governs individual FM operator semantics. D050 governs only the isolated four-operator routing layer and cannot override D047-D049 or pull later FM packages forward.
