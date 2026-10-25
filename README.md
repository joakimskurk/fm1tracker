# FM1Tracker

FM1Tracker is a hardware-first tracker/sequencer firmware project for the M-VAVE FM-1.

The project takes workflow inspiration from trackers such as Furnace, but it is **not** a port of the Furnace engine or codebase. The goal is a compact deterministic instrument built around FM-1 hardware, integer DSP, tracker editing and immediate performance capture.

License: GPL-3.0-only unless otherwise noted. See `THIRD_PARTY.md`.

## Current status

**Phase 5 is in progress. WP5.0 through WP5.5 are technically complete; WP5.5 is at final documentation closeout and WP5.6 envelope strategy is next after the closeout head is green.**

Completed engine foundation:

- complete 8-BIT family: PULSE, NOISE, TRIANGLE, WAVETABLE-8;
- D047/D048 per-instrument mixed-engine routing;
- D049 deterministic fixed-point FM operator;
- D050 deterministic four-operator routing;
- D051 deterministic 16-ID integer FM ratio mapping;
- D052 deterministic four-LEVEL plus global-INDEX mapping;
- D053 deterministic one-sample OP4 feedback.

## FM primitive baseline

D049 operator: 4 B state, Q15 output, generated quarter-wave sine table, PI32v2 object 692 B.

D050 routing: four static algorithms, fixed OP4 -> OP3 -> OP2 -> OP1 evaluation, 16 B routing state, PI32v2 object 362 B.

D051 ratio mapping: 16 stable Q8 ratios, deterministic clamp/rounding, PI32v2 object 264 B.

D052 control mapping: four LEVEL bytes and one global INDEX byte, 10 B transient mapped state, PI32v2 object 108 B.

D053 feedback:

- OP4 self-feedback only;
- one-sample delay;
- FEEDBACK 0..255 mapped to Q14;
- 2 B prior-OP4 history state per future FM voice;
- PI32v2 feedback object 130 B;
- source remains isolated from `dinghy.c`.

Canonical WP5.5 implementation run: `40164953532`, host `121083999893`, target `121083999790`, evidence `docs/WP5_5_VERIFICATION.md`.

Full running application remains exact WP5.0: 72,792 B image / 19,676 B normal RAM / 113,000 B pool, identity `FM-1_987`.

## Next work package

**WP5.6 - bounded FM4 envelope strategy**, issue #49, expected branch `phase5/fm4-envelope`, blocked until WP5.5 formal closeout completes.

WP5.6 starts contract-first. D054 must compare common versus per-operator envelope strategies using measured cost and musical capability, then freeze exact integer stage/rate/retrigger/update semantics before production envelope source exists.

WP5.6 does not include voice allocation, D048 integration, tracker-facing p-lock pages/IDs or audible mixed FM playback.

## FM4 sequence

1. WP5.1 fixed-point operator - COMPLETE
2. WP5.2 curated algorithms - COMPLETE
3. WP5.3 integer ratios/pitch mapping - COMPLETE
4. WP5.4 levels / modulation index - COMPLETE
5. WP5.5 bounded feedback - TECHNICALLY COMPLETE / CLOSEOUT
6. WP5.6 envelope strategy - NEXT
7. WP5.7 voice allocation/integration
8. WP5.8 parameter pages + p-lock mapping
9. WP5.9 mixed 8-BIT + FM4 validation
10. WP5.10 worst-case mixed-engine Phase 5 exit

## Product direction

**Tracker depth underneath, immediate performance/capture on top.**

Live capture, drums, transport-locked metronome, CHANNEL/GLOBAL performance FX/automation, song/order/save/load and physical hardware UX remain core planned work after engine foundations.

For a new session read `START_HERE.md`. Repository state wins over old chat context.

No physical FM-1 is currently available. `hardware_margin_claim=none`.
