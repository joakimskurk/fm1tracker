# FM1Tracker Current Decision Index

Date: 2026-10-25

`DECISIONS.md` remains the append-only historical decision log through D046. D047-D056 are accepted/current Phase 5 decisions. Persistent product direction is additionally summarized in `PROJECT_CONTEXT.md` so future work-package focus cannot erase intended engine families.

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

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**  
Normative: `WP5_8_FM_PARAMETER_PAGE_CONTRACT.md`  
Audit: `WP5_8_PRECONTRACT_AUDIT.md`  
Evidence: `WP5_8_CLOSEOUT_EVIDENCE.md`

D056 establishes:

- 27 stable semantic IDs in the existing 16BIT namespace `0x60..0x7B`, with `0x63` unassigned;
- seven stable four-control logical pages;
- normalized 27 B FM parameter set and 432 B sixteen-instrument mutable base bank;
- exact normalized mapping back to the D055 28 B patch;
- maximum four sparse locks per step unchanged;
- rejection of malformed >255 FM lock values rather than truncation;
- one generalized 8,704 B engine-row cache with the historical 2,048 B 8BIT view preserved as its first region;
- deterministic double-buffered FM override publication with one-byte selector per track/row;
- 232 B current row-boundary FM patch snapshot state and no per-row full-patch cache;
- no sparse lock-store/editor traversal in the per-sample renderer;
- page-aware held-step FM editing with main/UI-only page state;
- D049-D055 DSP/voice source frozen;
- exact WP5.0 all-8BIT regression equivalence.

Accepted closeout:

```text
head                     849c8b27d8c1afa09f9c093b695985c64e0c6013
run                      40203483720 SUCCESS
host                     121199398344 SUCCESS
target                   121199398538 SUCCESS
image                    79804 B
normal RAM               27792 B
pool                     113000 B
FWSC SHA-256             4d2805f428f5114531b4c3d379576d2b207cbc9a6c04fcbdd5263cfaa0cc84a0
```

The frozen D056 normal-RAM ceiling is 28,656 B, so the accepted target is 864 B below that package ceiling. This is a package-budget fact, not a physical-hardware margin claim.

## Product-context decision discipline

The active work package never implicitly removes future product directions. `PROJECT_CONTEXT.md` explicitly preserves 8-BIT, FM, future virtual-analog/subtractive synthesis and drums/percussion as intended families in one tracker environment. Future families require their own resource/architecture decisions before implementation.

Substantive user decisions and product clarifications are synchronized into Git under `EXECUTION_PROTOCOL.md`; roadmap order alone is not project memory.

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
D056 -> tracker-facing FM parameter identity/pages, mutable base storage, row publication and editor integration.

No D057 has been accepted yet. WP5.9 must audit and freeze its validation contract before assigning any new architecture decision.

No physical timing/audio-quality claim is permitted without physical FM-1 evidence.