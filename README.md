# FM1Tracker

FM1Tracker is a hardware-first tracker/sequencer firmware project for the M-VAVE FM-1.

The project takes workflow inspiration from trackers such as Furnace, but it is **not** a port of the Furnace engine or codebase. The goal is a compact deterministic instrument built around FM-1 hardware, integer DSP, tracker editing and immediate performance capture.

License: GPL-3.0-only unless otherwise noted. See `THIRD_PARTY.md`.

## Current status

**Phase 5 is in progress. WP5.0 through WP5.5 are complete. WP5.6 / D054 four-operator ADSR envelopes are technically complete and are in formal closeout. WP5.7 / #50 is planned but blocked until that closeout is green.**

Completed engine foundation:

- complete 8-BIT family: PULSE, NOISE, TRIANGLE, WAVETABLE-8;
- D047/D048 per-instrument mixed-engine routing;
- D049 deterministic fixed-point FM operator;
- D050 deterministic four-operator routing;
- D051 deterministic 16-ID integer FM ratio mapping;
- D052 deterministic four-LEVEL plus global-INDEX mapping;
- D053 bounded OP4 one-sample feedback;
- D054 four independent per-operator ADSR envelopes.

## FM primitive baseline

D049 operator: 4 B phase state, Q15 output and generated sine table.

D050 routing: four static algorithms, fixed OP4 -> OP3 -> OP2 -> OP1 evaluation and 16 B phase/routing state.

D051 ratio mapping: 16 stable Q8 ratios with deterministic clamp/rounding.

D052 control mapping: four LEVEL bytes + global INDEX byte with exact Q15/Q14 mappings.

D053 feedback: one OP4 self-feedback edge, exact one-sample history and 2 B persistent state.

D054 envelopes:

- four independent ADSR envelopes;
- 16 A/D/S/R control bytes per FM instrument;
- 20 B persistent envelope state per future FM voice;
- Q15.16 internal level;
- one generated immutable 1,024 B A/D/R time-step table;
- hard retrigger from zero;
- release from current level;
- true instant zero-time stages;
- envelope scales each D052 operator LEVEL before D049/D050 render;
- PI32v2 object 1,680 B text+rodata, 368 B under its 2,048 B ceiling.

Canonical WP5.6 implementation verification:

```text
head    401e666d21fa8290c7c622603e06e97b166c6b3e
run     40167933064
host    121092688126 SUCCESS
target  121092688295 SUCCESS
```

Evidence: `docs/WP5_6_VERIFICATION.md`.

Full running application remains exact WP5.0 because FM primitives are still isolated: 72,792 B image / 19,676 B normal RAM / 113,000 B pool, identity `FM-1_987`.

## Current work package

**WP5.6 closeout**, issue #49, branch `phase5/fm4-envelope`.

The documentation closeout head must pass the same host/target verification before #49 closes.

## Next work package

**WP5.7 - FM4 voice allocation and D048 router integration**, issue #50, expected branch `phase5/fm4-voice-integration`.

WP5.7 starts contract-first with D055. It must freeze FM voice count, state budget, ownership, release-tail policy, allocator tie-breaks and D048 event routing before production integration changes.

## FM4 sequence

1. WP5.1 fixed-point operator - COMPLETE
2. WP5.2 curated algorithms - COMPLETE
3. WP5.3 integer ratios/pitch mapping - COMPLETE
4. WP5.4 levels / modulation index - COMPLETE
5. WP5.5 bounded feedback - COMPLETE
6. WP5.6 envelope strategy - TECHNICALLY COMPLETE, CLOSEOUT ACTIVE
7. WP5.7 voice allocation/integration - PLANNED
8. WP5.8 parameter pages + p-lock mapping
9. WP5.9 mixed 8-BIT + FM4 validation
10. WP5.10 worst-case mixed-engine Phase 5 exit

## Product direction

**Tracker depth underneath, immediate performance/capture on top.**

Live capture, drums, transport-locked metronome, CHANNEL/GLOBAL performance FX/automation, song/order/save/load and physical hardware UX remain core planned work after engine foundations.

For a new session read `START_HERE.md`. Repository state wins over old chat context.

No physical FM-1 is currently available. `hardware_margin_claim=none`.
