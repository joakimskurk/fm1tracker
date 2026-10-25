# FM1Tracker

FM1Tracker is a hardware-first tracker/sequencer firmware project for the M-VAVE FM-1.

The project takes workflow inspiration from trackers such as Furnace, but it is **not** a port of the Furnace engine or codebase. The goal is a compact deterministic instrument built around FM-1 hardware, integer DSP, tracker editing and immediate performance capture.

License: GPL-3.0-only unless otherwise noted. See `THIRD_PARTY.md`.

## Current status

**Phase 5 is in progress. WP5.0 mixed-engine routing through WP5.5 bounded feedback are complete. WP5.6 bounded FM4 envelope strategy is ACTIVE and remains contract-first.**

Completed engine foundation:

- complete 8-BIT family: PULSE, NOISE, TRIANGLE, WAVETABLE-8;
- D047/D048 per-instrument mixed-engine routing;
- D049 deterministic fixed-point FM operator;
- D050 deterministic four-operator routing;
- D051 deterministic 16-ID integer FM ratio mapping;
- D052 deterministic four-LEVEL plus global-INDEX mapping;
- D053 deterministic bounded OP4 one-sample feedback.

Canonical WP5.5 closeout head: `216c7e34b6d761cadb683bd1fc356c216b98cc27`; run `40165352623`, host `121085156557`, target `121085156767`, both SUCCESS. Evidence: `docs/WP5_5_VERIFICATION.md`.

Full running application remains exact WP5.0: 72,792 B image / 19,676 B normal RAM / 113,000 B pool, identity `FM-1_987`.

## Current work package

**WP5.6 - bounded FM4 envelope strategy**, issue #49, branch `phase5/fm4-envelope`.

Current precontract direction:

- reject a final post-mix envelope because it only shapes final loudness;
- preserve D049-D053 unchanged;
- use the existing per-operator Q15 LEVEL surface as the envelope insertion point;
- working architecture candidate: four independent per-operator ADSR envelopes;
- candidate envelope output scales each D052 mapped LEVEL before the corresponding D049 render;
- candidate packed persistent state is 20 B per future FM voice;
- exact rate/time law, zero-time behavior, retrigger/release semantics and target ceilings must be frozen by D054 before implementation.

No production `fmt_fm_envelope.*` source may be added before D054 and its source-absent gate are accepted.

## FM4 sequence

1. WP5.1 fixed-point operator - COMPLETE
2. WP5.2 curated algorithms - COMPLETE
3. WP5.3 integer ratios/pitch mapping - COMPLETE
4. WP5.4 levels / modulation index - COMPLETE
5. WP5.5 bounded feedback - COMPLETE
6. WP5.6 envelope strategy - ACTIVE
7. WP5.7 voice allocation/integration
8. WP5.8 parameter pages + p-lock mapping
9. WP5.9 mixed 8-BIT + FM4 validation
10. WP5.10 worst-case mixed-engine Phase 5 exit

## Product direction

**Tracker depth underneath, immediate performance/capture on top.**

Live capture, drums, transport-locked metronome, CHANNEL/GLOBAL performance FX/automation, song/order/save/load and physical hardware UX remain core planned work after engine foundations.

For a new session read `START_HERE.md`. Repository state wins over old chat context.

No physical FM-1 is currently available. `hardware_margin_claim=none`.