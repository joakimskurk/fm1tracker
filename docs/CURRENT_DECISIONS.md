# FM1Tracker Current Decision Index

Date: 2026-10-24

`DECISIONS.md` remains the append-only historical decision log through D046. D047-D050 are accepted Phase 5 decisions. D051 does not exist yet and must be frozen by WP5.3 before production ratio source.

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

## Pending D051 boundary

WP5.3 / issue #46 must create D051 before any production ratio/pitch-mapping source.

D051 may define only:

- compact ratio set and stable ratio IDs;
- exact mathematical ratio meanings;
- integer base-phase-increment -> operator-phase-increment arithmetic;
- exact rounding and overflow/clamp behavior;
- supported base-increment domain;
- sub-unity/fractional ratio scope;
- deterministic pitch/ratio fixtures;
- mapping API/state requirement;
- source/realtime/PI32v2 object bounds.

Until D051 is accepted, no ratio list or arithmetic is authoritative.

D051 may not decide final level/mod-index UX, feedback, envelopes, voice allocation, parameter pages/p-lock IDs or audible mixed FM integration.

## Current precedence

- D047 governs mixed-engine architecture.
- D048 governs common family routing/ownership.
- D049 governs one FM operator.
- D050 governs four-operator connection/evaluation.
- WP5.3 must preserve D047-D050 and add only the ratio/pitch layer.

No physical timing/audio-quality claim is permitted without physical FM-1 evidence.
