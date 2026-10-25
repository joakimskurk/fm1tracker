# FM1Tracker Current Decision Index

Date: 2026-10-25

`DECISIONS.md` remains the append-only historical decision log through D046. D047-D053 are accepted and verified Phase 5 decisions. D054 is frozen source-absent and accepted for implementation.

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

Canonical final closeout: run `40165352623`, host `121085156557`, target `121085156767`.

## D054 - four independent FM4 ADSR envelopes

Status: **FROZEN SOURCE-ABSENT, ACCEPTED FOR IMPLEMENTATION**  
Normative: `WP5_6_FM_ENVELOPE_CONTRACT.md`  
Audit: `WP5_6_PRECONTRACT_AUDIT.md`  
Evidence: `WP5_6_PREIMPLEMENTATION_EVIDENCE.md`

Frozen compatibility:

- four independent per-operator ADSR envelopes;
- envelope scales each D052 Q15 LEVEL before D049 render;
- exactly 20 B persistent envelope state per future FM voice;
- exactly 16 B envelope control model;
- Q15.16 internal level, full scale `0x7fff0000`;
- stable OFF/ATTACK/DECAY/SUSTAIN/RELEASE stage IDs 0..4;
- ATTACK/DECAY/RELEASE time controls use one immutable 1,024 B table;
- 16 exact steps per octave from approximately 1 ms through approximately 61.3 s;
- control 0 is true instant transition;
- SUSTAIN uses the exact D052 LEVEL byte-to-Q15 mapping;
- hard retrigger from zero;
- note-off releases from current level;
- D050 algorithm changes do not reset envelope state;
- per-sample bounded update, four state updates + four level scalings per FM voice;
- no heap, runtime division/modulo, 64-bit integer or floating-point requirement;
- D049-D053 and `dinghy.c` remain unchanged in WP5.6 primitive work.

Accepted source-absent gate:

- head `151401f164a17c3459f965971f2855bb05831c4b`;
- run `40167537148`;
- job `121091550742` SUCCESS;
- artifact ID `12588534519`, SHA `eaea6ae76ee55779f45b4e2692c226e0790e0ebeb22326a9e722359fe76eec36`.

Production envelope source is now permitted only inside the D054 boundary.

## Current precedence

- D047 governs mixed-engine architecture.
- D048 governs common family routing/ownership.
- D049 governs one FM operator.
- D050 governs four-operator connection/evaluation.
- D051 governs base-pitch to operator-ratio mapping.
- D052 governs operator LEVEL and global INDEX mapping.
- D053 governs bounded one-sample OP4 feedback.
- D054 governs isolated four-operator ADSR envelope semantics.

No physical timing/audio-quality claim is permitted without physical FM-1 evidence.
