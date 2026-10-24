# FM1Tracker Current Decision Index

Date: 2026-10-25

`DECISIONS.md` remains the append-only historical decision log through D046. D047-D051 are accepted Phase 5 decisions.

## D047 - mixed-engine instrument architecture

Status: **ACCEPTED**  
Normative: `D047_MIXED_ENGINE_ARCHITECTURE.md`

One engine-agnostic tracker. Instruments own family + engine identity. Historical project-wide future family restriction is superseded.

## D048 - mixed-engine routing

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md`  
Evidence: `WP5_0_VERIFICATION.md`

Per-instrument family routing, 64 B identity publication, 8 B family route, bounded static dispatch, song mode non-gating, one shared row cache, fail-safe unsupported family behavior and exact D046 regression baseline.

## D049 - fixed-point FM operator

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_1_FM_OPERATOR_CONTRACT.md`  
Evidence: `WP5_1_VERIFICATION.md`

Frozen compatibility:

- one uint32 phase = 4 B state;
- 2^32 phase cycle / natural wrap / sample-before-advance;
- signed int32 phase modulation;
- Q15 level 0..32767;
- generated 257-entry quarter-wave table;
- table SHA `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68`;
- 4,096-render SHA `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66`;
- PI32v2 object 692 B.

## D050 - curated four-operator FM routing

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_2_FM_ALGORITHM_CONTRACT.md`  
Evidence: `WP5_2_PREIMPLEMENTATION_EVIDENCE.md`, `WP5_2_VERIFICATION.md`

Stable algorithm IDs:

```text
0 STACK4    OP4 -> OP3 -> OP2 -> OP1
1 FAN3      OP2 + OP3 + OP4 -> OP1
2 DUAL2     OP4 -> OP3 plus OP2 -> OP1
3 PARALLEL4 OP1 + OP2 + OP3 + OP4
```

Frozen compatibility:

- operator index 0..3 = OP1..OP4;
- fixed evaluation OP4 -> OP3 -> OP2 -> OP1;
- every operator renders exactly once/sample;
- routing state = four D049 operators = 16 B;
- no extra mutable history before feedback;
- invalid algorithm -> STACK4;
- mod depth 0..16383;
- edge mapping `sample * depth * 4`;
- multiple phase offsets accumulate modulo 2^32;
- carrier counts 1/2/4 with sign-safe power-of-two normalization;
- no runtime graph traversal;
- algorithm changes preserve operator phase continuity;
- PI32v2 router object 362 B, SHA `ed85fe6ed70402bd3b0727e59b1c8eee21f6d15f264ec8dc439603968a38f469`;
- full app remains exact WP5.0 while routing is isolated.

Canonical D050 run `40136425976`, host `120998804031`, target `120998804175`.

## D051 - curated FM4 ratio/pitch mapping

Status: **ACCEPTED, CONTRACT FROZEN, IMPLEMENTATION NOT YET AUTHORIZED**  
Normative: `WP5_3_FM_RATIO_CONTRACT.md`

D051 freezes one stateless integer mapping from the existing D038 base phase increment to four D050 operator increments.

Stable ratio IDs:

```text
0  = 0.5
1  = 1.0          UNITY / invalid-ID fallback
2  = 1.25
3  = 1.4140625    exact Q8 362/256
4  = 1.5
5  = 2.0
6  = 2.5
7  = 3.0
8  = 3.5
9  = 4.0
10 = 5.0
11 = 6.0
12 = 7.0
13 = 8.0
14 = 12.0
15 = 16.0
```

Frozen compatibility:

- exactly 16 V1 ratio IDs;
- unsigned Q8 ratio values;
- canonical base-increment domain `0..1293741834`;
- malformed larger base values clamp to `1293741834` before ratio scaling;
- output increment ceiling `0x7fffffff`;
- precomputed per-ratio base thresholds prevent accidental wrapped/folded operator increments;
- exact 32-bit split-Q8 round-half-up transform;
- no runtime division/modulo;
- no 64-bit integer arithmetic;
- invalid ratio ID -> UNITY;
- no persistent ratio state;
- `map4` performs exactly four mappings;
- exhaustive 16,384 pitch x 16 ratio reference SHA `5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c`;
- 27,615 legal pitch/ratio pairs intentionally saturate at the D051 ceiling;
- future PI32v2 ratio object ceiling 1,024 B;
- D051 ceiling is not an anti-aliasing or bandlimiting claim.

Production `fmt_fm_ratio.*` remains forbidden until the D051 source-absent reference workflow is green and its evidence is frozen in the repo.

D051 does not define free Yamaha-style coarse/fine editing, fixed-frequency mode, final operator levels/mod-index, feedback, envelopes, voice allocation, parameter pages/p-lock IDs or audible mixed-engine integration.

## Current precedence

- D047 governs mixed-engine architecture.
- D048 governs common family routing/ownership.
- D049 governs one FM operator.
- D050 governs four-operator connection/evaluation.
- D051 governs base-pitch to operator-ratio mapping only.
- WP5.4 must preserve D047-D051 and add only final level/mod-index mapping after WP5.3 is fully verified and closed.

No physical timing/audio-quality claim is permitted without physical FM-1 evidence.
