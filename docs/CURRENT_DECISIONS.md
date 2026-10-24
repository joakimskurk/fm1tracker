# FM1Tracker Current Decision Index

Date: 2026-10-25

`DECISIONS.md` remains the append-only historical decision log through D046. D047-D051 are accepted Phase 5 decisions.

## D047 - mixed-engine instrument architecture

Status: **ACCEPTED**  
Normative: `D047_MIXED_ENGINE_ARCHITECTURE.md`

One engine-agnostic tracker. Instruments own family + engine identity.

## D048 - mixed-engine routing

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md`  
Evidence: `WP5_0_VERIFICATION.md`

Per-instrument family routing, bounded static dispatch, song mode non-gating, shared row cache, fail-safe unsupported-family behavior and exact D046 regression baseline.

## D049 - fixed-point FM operator

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_1_FM_OPERATOR_CONTRACT.md`  
Evidence: `WP5_1_VERIFICATION.md`

Frozen compatibility includes 4 B phase state, uint32 phase, signed int32 phase modulation, Q15 level, generated quarter-wave sine table and exact deterministic render semantics. PI32v2 object 692 B.

## D050 - curated four-operator FM routing

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_2_FM_ALGORITHM_CONTRACT.md`  
Evidence: `WP5_2_PREIMPLEMENTATION_EVIDENCE.md`, `WP5_2_VERIFICATION.md`

Stable algorithms:

```text
0 STACK4    OP4 -> OP3 -> OP2 -> OP1
1 FAN3      OP2 + OP3 + OP4 -> OP1
2 DUAL2     OP4 -> OP3 plus OP2 -> OP1
3 PARALLEL4 OP1 + OP2 + OP3 + OP4
```

Frozen compatibility includes fixed OP4 -> OP3 -> OP2 -> OP1 evaluation, exactly one render/operator/sample, 16 B routing state, mod depth 0..16383, modulo-2^32 phase accumulation and deterministic carrier normalization. PI32v2 router object 362 B.

## D051 - curated FM4 ratio/pitch mapping

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_3_FM_RATIO_CONTRACT.md`  
Evidence: `WP5_3_PREIMPLEMENTATION_EVIDENCE.md`, `WP5_3_VERIFICATION.md`

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

- unsigned Q8 ratios;
- canonical base-increment domain `0..1293741834`;
- malformed larger base values clamp before scaling;
- output ceiling `0x7fffffff`;
- precomputed per-ratio clamp thresholds;
- exact 32-bit split-Q8 round-half-up transform;
- no runtime division/modulo or 64-bit integer arithmetic;
- invalid ratio ID -> UNITY;
- stateless four-output mapping;
- exhaustive 16,384 x 16 reference SHA `5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c`;
- 27,615 legal pitch/ratio pairs intentionally saturate at the D051 ceiling;
- PI32v2 ratio object 264 B, SHA `aa5bd8b9e1afa72054af2effeefbf468cfbb8b2b8d978e028540a30dc162956d`;
- full app remains exact WP5.0 while the primitive is isolated.

Canonical D051 source-absent gate: run `40138575267`, job `121005262360`.

Canonical D051 implementation verification: run `40139154497`, host `121006986068`, target `121006985928`.

D051's increment ceiling is not an anti-aliasing or bandlimiting claim.

## D052

Status: **NOT YET DEFINED**

WP5.4 / issue #47 must freeze D052 contract-first before any production level/modulation-index mapping source is added.

D052 must preserve D047-D051 and decide only the bounded control mapping into D050 `level_q15[4]` and modulation-depth semantics. Feedback, envelopes, voice integration and tracker-facing parameter IDs remain later packages.

## Current precedence

- D047 governs mixed-engine architecture.
- D048 governs common family routing/ownership.
- D049 governs one FM operator.
- D050 governs four-operator connection/evaluation.
- D051 governs base-pitch to operator-ratio mapping.
- D052 does not exist yet.

No physical timing/audio-quality claim is permitted without physical FM-1 evidence.
