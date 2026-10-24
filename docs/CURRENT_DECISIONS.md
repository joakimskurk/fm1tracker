# FM1Tracker Current Decision Index

Date: 2026-10-24

`DECISIONS.md` remains the append-only historical decision log through D046. Historical entries are not rewritten when later decisions supersede constraints.

D047, D048 and D049 are the accepted Phase 5 architecture/implementation decisions currently in force. D047/D048 are implemented through WP5.0. D049 is implemented and verified through WP5.1. D050 does not exist yet and must be frozen by WP5.2 before production FM algorithm-routing source.

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

D049 freezes the isolated single-operator primitive:

- `FmtFmOperator` is exactly one `uint32_t phase`, 4 B;
- phase convention is one cycle = 2^32, natural unsigned wrap, sample-current-before-advance;
- phase modulation input is signed `int32_t` in the same phase-unit domain, added modulo 2^32;
- level input clamps to Q15 `0..32767`;
- carrier sine uses a generated 257-entry positive quarter-wave Q15 table;
- table interpolation uses 22 fractional phase bits and non-negative unsigned multiply/shift arithmetic;
- output is bounded to `-32767..32767`, never `-32768`;
- table binary SHA-256 is `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68`;
- frozen 4,096-render output SHA-256 is `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66`;
- render is O(1), with no loop/search/allocation/divide/modulo/float/64-bit arithmetic;
- isolated pinned PI32v2 object text+rodata is 692 B;
- production module remains isolated from `dinghy.c` through WP5.1;
- full app remains byte-identical to canonical WP5.0;
- physical timing/audio quality claims remain unavailable without hardware.

D049 does **not** decide:

- FM algorithm routing, WP5.2;
- ratio/pitch policy, WP5.3;
- final level/modulation-index controls, WP5.4;
- feedback, WP5.5;
- FM envelopes, WP5.6;
- voice allocation/router integration, WP5.7;
- FM parameter pages/p-lock IDs, WP5.8;
- simultaneous production mixed-family FM playback, WP5.9.

## Pending D050 boundary

WP5.2 / issue #45 must create D050 before any production FM algorithm-routing source.

D050 is expected to freeze only the curated four-operator routing layer:

- algorithm count and stable internal IDs;
- exact graph topology;
- exact per-sample evaluation order;
- modulation accumulation/scaling and overflow policy;
- carrier sum/output normalization;
- routing-layer state requirement before feedback;
- deterministic graph vectors/hashes;
- source/realtime/target object bounds.

Until D050 is accepted, no algorithm topology or numeric routing convention is authoritative.

## Current interpretation

- FM1Tracker is one engine-agnostic tracker/sequencer.
- Instruments own family + engine identity.
- D048 common routing remains authoritative while new family DSP is built in isolated packages.
- D049 is the FM operator compatibility baseline and does not yet make FM audible in the running firmware.
- WP5.2 must remain isolated and contract-first until D050 is frozen.
- The complete D046 8-BIT family remains the sound/regression baseline.
- No physical timing-margin claim is permitted without physical FM-1 evidence.

## Precedence rule

For historical checkpoints, use the decision/contract that governed that checkpoint.

For future sound-engine architecture after Phase 4, D047 supersedes D004. D048 is authoritative for common family routing. D049 is authoritative only for the isolated FM operator primitive and cannot override D047/D048 routing/ownership semantics. Future D050 may define algorithm routing only after explicit acceptance.
