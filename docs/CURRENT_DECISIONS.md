# FM1Tracker Current Decision Index

Date: 2026-10-25

`DECISIONS.md` remains the append-only historical decision log through D046. D047-D053 are accepted Phase 5 decisions.

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
Audit: `WP5_5_PRECONTRACT_AUDIT.md`  
Evidence: `WP5_5_PREIMPLEMENTATION_EVIDENCE.md`, `WP5_5_VERIFICATION.md`

Frozen compatibility:

- one OP4 -> OP4 feedback edge only;
- exactly one sample of delay;
- `FmtFmFeedbackState` exactly 2 B signed-16 prior OP4 output;
- FEEDBACK is a distinct byte `0..255`;
- byte-to-Q14 mapping `(v << 6) | (v >> 2)`;
- feedback phase offset `previous_op4_q15 * depth_q14 * 4`;
- history updates every OP4 render even at FEEDBACK=0;
- reset/retrigger clears history;
- D050 algorithm changes retain history;
- no D049-D052 or `dinghy.c` modification in WP5.5;
- isolated production surface exactly `fmt_fm_feedback.h/.c`;
- full signed-int16 history x 256 controls is signed-32 safe;
- PI32v2 object 130 B and persistent state 2 B per future FM voice;
- full running application remains exact WP5.0 while D053 is isolated.

Canonical source-absent gate: run `40164610821`, job `121082987705`.

Canonical implementation verification: run `40164953532`, host `121083999893`, target `121083999790`.

## D054

Status: **NOT YET DEFINED**

WP5.6 / issue #49 must freeze D054 contract-first after WP5.5 formal closeout. D054 must choose a bounded deterministic FM4 envelope strategy without pulling WP5.7 voice/router integration forward.

## Current precedence

- D047 governs mixed-engine architecture.
- D048 governs common family routing/ownership.
- D049 governs one FM operator.
- D050 governs four-operator connection/evaluation.
- D051 governs base-pitch to operator-ratio mapping.
- D052 governs operator LEVEL and global INDEX mapping.
- D053 governs bounded one-sample OP4 feedback.
- D054 does not exist yet.

No physical timing/audio-quality claim is permitted without physical FM-1 evidence.
