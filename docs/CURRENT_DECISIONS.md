# FM1Tracker Current Decision Index

Date: 2026-10-25

`DECISIONS.md` remains the append-only historical decision log through D046. D047-D056 are accepted/current Phase 5 decisions.

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

## D055 - FM4 voice allocation and D048 integration

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_7_FM_VOICE_INTEGRATION_CONTRACT.md`  
Evidence: `WP5_7_VERIFICATION.md`

Final closeout head `80587f12786122dcec819260ddad5dd5db4b1005`, run `40177151084`, host `121120200847`, target `121120200983`, all SUCCESS.

## D056 - FM4 parameter pages and p-lock mapping

Status: **FROZEN BEFORE PRODUCTION IMPLEMENTATION - SOURCE-ABSENT GATE ACTIVE**  
Normative: `WP5_8_FM_PARAMETER_PAGE_CONTRACT.md`  
Audit: `WP5_8_PRECONTRACT_AUDIT.md`

D056 freezes:

- 27 stable semantic IDs in the existing 16BIT namespace `0x60..0x7B`, with `0x63` unassigned;
- seven stable four-knob logical pages;
- normalized 27 B FM parameter set and 432 B sixteen-instrument base bank;
- exact normalized mapping back to the D055 28 B patch;
- maximum four sparse locks per step unchanged;
- rejection of malformed >255 FM lock values rather than truncation;
- one generalized 8,704 B engine-row cache with the historical 2,048 B 8BIT view preserved as its first region;
- deterministic two-word FM override payload publication with a one-byte active-bank selector;
- no lock-store/editor reads in audio;
- D049-D055 DSP/voice source frozen;
- absolute ceilings 93,432 B image / 28,656 B normal RAM / 113,000 B pool.

No WP5.8 production activation is allowed until the D056 source-absent gate is green.

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
D056 -> tracker-facing FM parameter identity/pages, mutable base storage and p-lock publication.

No physical timing/audio-quality claim is permitted without physical FM-1 evidence.
