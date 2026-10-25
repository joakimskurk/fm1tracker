# FM1Tracker Current Decision Index

Date: 2026-10-25

`DECISIONS.md` remains the append-only historical decision log through D046. D047-D052 are accepted Phase 5 decisions.

## D047 - mixed-engine instrument architecture

Status: **ACCEPTED**  
Normative: `D047_MIXED_ENGINE_ARCHITECTURE.md`

One engine-agnostic tracker. Instruments own family + engine identity.

## D048 - mixed-engine routing

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md`  
Evidence: `WP5_0_VERIFICATION.md`

## D049 - fixed-point FM operator

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_1_FM_OPERATOR_CONTRACT.md`  
Evidence: `WP5_1_VERIFICATION.md`

## D050 - curated four-operator FM routing

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_2_FM_ALGORITHM_CONTRACT.md`  
Evidence: `WP5_2_PREIMPLEMENTATION_EVIDENCE.md`, `WP5_2_VERIFICATION.md`

Stable algorithms remain STACK4, FAN3, DUAL2 and PARALLEL4 with fixed OP4 -> OP3 -> OP2 -> OP1 evaluation.

## D051 - curated FM4 ratio/pitch mapping

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_3_FM_RATIO_CONTRACT.md`  
Evidence: `WP5_3_PREIMPLEMENTATION_EVIDENCE.md`, `WP5_3_VERIFICATION.md`

Stable 16-ID Q8 ratio mapping, deterministic clamp/rounding and output ceiling remain frozen.

## D052 - FM4 operator LEVEL / global INDEX mapping

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_4_FM_LEVEL_INDEX_CONTRACT.md`  
Evidence: `WP5_4_PREIMPLEMENTATION_EVIDENCE.md`, `WP5_4_VERIFICATION.md`

Frozen compatibility:

- exactly four LEVEL bytes plus one global INDEX byte, all 0..255;
- scalar malformed input saturates to 255;
- `level_q15 = (v << 7) | (v >> 1)`;
- `mod_depth_q14 = (v << 6) | (v >> 2)`;
- LEVEL and INDEX remain separate controls;
- null LEVEL input maps all four operators to zero level;
- transient mapped state exactly 10 B, persistent state 0 B;
- all 65,536 legal LEVEL x INDEX single-edge combinations are signed-32 safe under D049/D050;
- PI32v2 controls object 108 B, SHA `05867ebe120829d1e3b8bf6f0504f29d63e9643e4fa5eede4c6d858d31d25059`;
- full running application remains exact WP5.0 while D052 is isolated.

Canonical D052 source-absent gate: run `40140119411`, job `121009903030`.

Canonical D052 implementation verification: run `40143482844`, host `121019875458`, target `121019875309`.

## D053

Status: **NOT YET DEFINED**

WP5.5 / issue #48 must freeze D053 contract-first before any production feedback source is added.

D053 must preserve D047-D052 and decide only a bounded deterministic FM4 feedback topology/control/history model. Envelopes, voice integration and tracker-facing parameter IDs remain later packages.

## Current precedence

- D047 governs mixed-engine architecture.
- D048 governs common family routing/ownership.
- D049 governs one FM operator.
- D050 governs four-operator connection/evaluation.
- D051 governs base-pitch to operator-ratio mapping.
- D052 governs operator LEVEL and global INDEX mapping.
- D053 does not exist yet.

No physical timing/audio-quality claim is permitted without physical FM-1 evidence.
