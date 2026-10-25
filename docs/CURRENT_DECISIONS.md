# FM1Tracker Current Decision Index

Date: 2026-10-25

`DECISIONS.md` remains the append-only historical decision log through D046. D047-D054 are accepted Phase 5 decisions.

## D047 - mixed-engine instrument architecture

Status: **ACCEPTED**  
Normative: `D047_MIXED_ENGINE_ARCHITECTURE.md`

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

## D051 - curated FM4 ratio/pitch mapping

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_3_FM_RATIO_CONTRACT.md`  
Evidence: `WP5_3_PREIMPLEMENTATION_EVIDENCE.md`, `WP5_3_VERIFICATION.md`

## D052 - FM4 operator LEVEL / global INDEX mapping

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_4_FM_LEVEL_INDEX_CONTRACT.md`  
Evidence: `WP5_4_PREIMPLEMENTATION_EVIDENCE.md`, `WP5_4_VERIFICATION.md`

## D053 - bounded one-sample OP4 feedback

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_5_FM_FEEDBACK_CONTRACT.md`  
Evidence: `WP5_5_PREIMPLEMENTATION_EVIDENCE.md`, `WP5_5_VERIFICATION.md`

D053 freezes one OP4 self-feedback edge, one-sample delay, 2 B history state, distinct FEEDBACK byte and bounded integer arithmetic. Final closeout run `40165352623` passed host and target.

## D054 - four independent FM4 ADSR envelopes

Status: **ACCEPTED, IMPLEMENTED, VERIFIED AT TECHNICAL CHECKPOINT**  
Normative: `WP5_6_FM_ENVELOPE_CONTRACT.md`  
Audit: `WP5_6_PRECONTRACT_AUDIT.md`  
Evidence: `WP5_6_PREIMPLEMENTATION_EVIDENCE.md`, `WP5_6_VERIFICATION.md`

Frozen compatibility:

- four independent per-operator ADSR envelopes;
- envelope scales each D052 Q15 LEVEL before D049 render;
- exactly 20 B persistent envelope state per future FM voice;
- exactly 16 B control model per FM instrument;
- Q15.16 level state with full scale `0x7fff0000`;
- stable OFF/ATTACK/DECAY/SUSTAIN/RELEASE IDs 0..4;
- hard retrigger from zero;
- release from current level;
- true instant A/D/R when time control is 0;
- controls 1..255 use one exact immutable 1,024 B time-step table;
- SUSTAIN uses the D052 LEVEL mapping;
- per-sample bounded update with no heap, runtime division/modulo, 64-bit integer or floating point;
- D050 algorithm changes do not reset envelope state;
- D049-D053 and `dinghy.c` unchanged;
- production source remains isolated until WP5.7.

Accepted source-absent gate: run `40167537148`, job `121091550742`.

Canonical implementation verification: run `40167933064`, host `121092688126`, target `121092688295`.

Measured PI32v2 envelope object: 1,680 B text+rodata, including exactly 1,024 B immutable table. Object SHA `22fa7937d7776fea37c8e758ca83ee0a3f5d6c506ebd4661a8290a4516e048d1`.

Formal package closeout still requires the same verification on the documentation closeout head.

## Current precedence

- D047 governs mixed-engine architecture.
- D048 governs common family routing/ownership.
- D049 governs one FM operator.
- D050 governs four-operator connection/evaluation.
- D051 governs base-pitch to operator-ratio mapping.
- D052 governs operator LEVEL and global INDEX mapping.
- D053 governs bounded one-sample OP4 feedback.
- D054 governs isolated four-operator ADSR envelope semantics.

D055 does not exist yet. WP5.7 / #50 must freeze it before FM voice/router integration changes.

No physical timing/audio-quality claim is permitted without physical FM-1 evidence.
