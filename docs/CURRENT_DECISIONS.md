# FM1Tracker Current Decision Index

Date: 2026-10-25

`DECISIONS.md` remains the append-only historical decision log through D046. D047-D055 are accepted/current Phase 5 decisions.

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

## D054 - four independent FM4 ADSR envelopes

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_6_FM_ENVELOPE_CONTRACT.md`  
Evidence: `WP5_6_PREIMPLEMENTATION_EVIDENCE.md`, `WP5_6_VERIFICATION.md`

Final closeout run `40168286355`, host `121093715194`, target `121093715000`.

## D055 - FM4 voice allocation and D048 integration

Status: **FROZEN BEFORE PRODUCTION INTEGRATION - SOURCE-ABSENT GATE PENDING**  
Normative: `WP5_7_FM_VOICE_INTEGRATION_CONTRACT.md`  
Audit: `WP5_7_PRECONTRACT_AUDIT.md`

D055 freezes:

- eight family-local FM slots;
- direct D043 allocator reuse;
- no active cross-track primary stealing;
- exact 92 B FM voice and 788 B FM engine targets;
- immutable 28 B WP5.7 bridge patch in ROM;
- no mutable FM instrument bank/page cache until WP5.8;
- hard NOTE retrigger of D049/D053/D054;
- detached OFF/CUT release tails;
- one backward-compatible D050 OP4 composition seam;
- D048 cross-family silence/clear/attempt/commit ordering;
- +1024 B normal-RAM and +8192 B image ceilings for WP5.7;
- exact default all-8BIT regression requirement.

No production integration change is allowed until the D055 source-absent gate is green.

## Current precedence

D047 -> architecture.  
D048 -> family route.  
D049 -> one FM operator.  
D050 -> topology.  
D051 -> ratios/pitch.  
D052 -> LEVEL/INDEX.  
D053 -> OP4 feedback.  
D054 -> four envelopes.  
D055 -> FM voice ownership/allocator/render composition and D048 integration.

No physical timing/audio-quality claim is permitted without physical FM-1 evidence.
