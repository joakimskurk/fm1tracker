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
Evidence: `WP5_2_VERIFICATION.md`

## D051 - curated FM4 ratio/pitch mapping

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_3_FM_RATIO_CONTRACT.md`  
Evidence: `WP5_3_VERIFICATION.md`

## D052 - FM4 operator LEVEL / global INDEX mapping

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_4_FM_LEVEL_INDEX_CONTRACT.md`  
Evidence: `WP5_4_VERIFICATION.md`

## D053 - bounded one-sample OP4 feedback

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_5_FM_FEEDBACK_CONTRACT.md`  
Evidence: `WP5_5_VERIFICATION.md`

## D054 - four independent FM4 ADSR envelopes

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_6_FM_ENVELOPE_CONTRACT.md`  
Evidence: `WP5_6_VERIFICATION.md`

Final closeout run `40168286355`, host `121093715194`, target `121093715000`.

## D055 - FM4 voice allocation and D048 integration

Status: **ACCEPTED, IMPLEMENTED, TECHNICALLY VERIFIED; DOCUMENTATION CLOSEOUT ACTIVE**  
Normative: `WP5_7_FM_VOICE_INTEGRATION_CONTRACT.md`  
Audit: `WP5_7_PRECONTRACT_AUDIT.md`  
Evidence: `WP5_7_PREIMPLEMENTATION_EVIDENCE.md`, `WP5_7_VERIFICATION.md`

D055 freezes and verifies:

- eight family-local FM slots;
- exact D043 allocator reuse;
- no active cross-track primary stealing;
- exact 92 B FM voice and 788 B FM engine state;
- hard NOTE retrigger and RETRIG reset;
- detached OFF/CUT release tails;
- one backward-compatible D050 OP4 composition seam;
- D048 cross-family silence/clear/attempt/commit ordering;
- exact default all-8BIT regression requirement;
- bounded target ceiling with accepted result 77,048 B image / 20,464 B RAM / 113,000 B pool;
- mutable FM instrument parameter storage deferred to WP5.8.

Canonical implementation run `40173076046`, host `121107852886`, target `121107852943`, all SUCCESS.

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

The next decision will belong to WP5.8 and must be frozen only after the parameter-page/p-lock audit.

No physical timing/audio-quality claim is permitted without physical FM-1 evidence.
