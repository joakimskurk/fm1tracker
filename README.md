# FM1Tracker

FM1Tracker is a hardware-first tracker/sequencer firmware project for the M-VAVE FM-1.

The project takes workflow inspiration from trackers such as Furnace, but it is **not** a port of the Furnace engine or codebase. The goal is a compact deterministic instrument built around FM-1 hardware, integer DSP, tracker editing and immediate performance capture.

License: GPL-3.0-only unless otherwise noted. See `THIRD_PARTY.md`.

## Current status

**Phase 5 is in progress. WP5.0 mixed-engine routing, WP5.1 fixed-point FM operator, WP5.2 curated FM4 routing, WP5.3 integer ratio/pitch mapping and WP5.4 level/modulation-index mapping are complete. WP5.5 bounded feedback is next and must begin contract-first.**

Completed engine foundation:

- complete 8-BIT family: PULSE, NOISE, TRIANGLE, WAVETABLE-8;
- D047/D048 per-instrument mixed-engine routing;
- D049 deterministic fixed-point FM operator;
- D050 deterministic four-operator routing;
- D051 deterministic 16-ID integer FM ratio mapping;
- D052 deterministic four-LEVEL plus global-INDEX mapping.

## FM primitive baseline

D049 operator: 4 B state, Q15 output, generated quarter-wave sine table, PI32v2 object 692 B.

D050 routing: four static algorithms, fixed OP4 -> OP3 -> OP2 -> OP1 evaluation, 16 B routing state, PI32v2 object 362 B.

D051 ratio mapping: 16 stable Q8 ratios, deterministic clamp/rounding, PI32v2 object 264 B.

D052 control mapping:

- OP1_LEVEL..OP4_LEVEL and INDEX are stable 0..255 DSP bytes;
- LEVEL -> Q15 by `(v << 7) | (v >> 1)`;
- INDEX -> Q14 by `(v << 6) | (v >> 2)`;
- transient mapped state 10 B, persistent state 0 B;
- PI32v2 object 108 B;
- source remains isolated from `dinghy.c`.

Canonical WP5.4 final run: `40143482844`, host `121019875458`, target `121019875309`, evidence `docs/WP5_4_VERIFICATION.md`.

Full running application remains exact WP5.0: 72,792 B image / 19,676 B normal RAM / 113,000 B pool, identity `FM-1_987`.

## Next work package

**WP5.5 - bounded deterministic FM4 feedback**, issue #48, expected branch `phase5/fm4-feedback`.

WP5.5 starts contract-first. D053 must choose one bounded deterministic feedback topology/history model, freeze exact integer control/reset/switch semantics and prove arithmetic/state/object bounds before production feedback source exists.

WP5.5 does not include envelopes, voice integration, tracker-facing p-lock pages/IDs or audible mixed FM playback.

## FM4 sequence

1. WP5.1 fixed-point operator - COMPLETE
2. WP5.2 curated algorithms - COMPLETE
3. WP5.3 integer ratios/pitch mapping - COMPLETE
4. WP5.4 levels / modulation index - COMPLETE
5. WP5.5 bounded feedback - NEXT
6. WP5.6 envelope strategy
7. WP5.7 voice allocation/integration
8. WP5.8 parameter pages + p-lock mapping
9. WP5.9 mixed 8-BIT + FM4 validation
10. WP5.10 worst-case mixed-engine Phase 5 exit

## Product direction

**Tracker depth underneath, immediate performance/capture on top.**

Live capture, drums, transport-locked metronome, CHANNEL/GLOBAL performance FX/automation, song/order/save/load and physical hardware UX remain core planned work after engine foundations.

For a new session read `START_HERE.md`. Repository state wins over old chat context.

No physical FM-1 is currently available. `hardware_margin_claim=none`.
