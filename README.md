# FM1Tracker

FM1Tracker is a hardware-first tracker/sequencer firmware project for the M-VAVE FM-1.

The project takes workflow inspiration from trackers such as Furnace, but it is **not** a port of the Furnace engine or codebase. The goal is a compact deterministic instrument built around FM-1 hardware, integer DSP, tracker editing and immediate performance capture.

License: GPL-3.0-only unless otherwise noted. See `THIRD_PARTY.md`.

## Current status

**Phase 5 is in progress. WP5.0 mixed-engine routing, WP5.1 fixed-point FM operator, WP5.2 curated FM4 routing and WP5.3 integer ratio/pitch mapping are complete. WP5.4 level/modulation-index mapping is next and must begin contract-first.**

Completed engine foundation:

- complete 8-BIT family: PULSE, NOISE, TRIANGLE, WAVETABLE-8;
- D047/D048 per-instrument mixed-engine routing;
- D049 deterministic fixed-point FM operator;
- D050 deterministic four-operator routing;
- D051 deterministic 16-ID integer FM ratio mapping.

## Mixed-engine architecture

FM1Tracker is one engine-agnostic tracker/sequencer. Tracker rows/tracks reference instruments, and instruments own engine family + engine identity.

WP5.0 canonical exit: run `40129822006`, job `120978955451`, evidence `docs/WP5_0_VERIFICATION.md`.

## FM primitive baseline

D049 operator:

- 4 B state;
- uint32 phase / signed int32 phase modulation;
- Q15 level/output;
- generated 257-entry quarter-wave table;
- PI32v2 object 692 B.

D050 routing:

```text
0 STACK4    OP4 -> OP3 -> OP2 -> OP1
1 FAN3      OP2 + OP3 + OP4 -> OP1
2 DUAL2     OP4 -> OP3 plus OP2 -> OP1
3 PARALLEL4 OP1 + OP2 + OP3 + OP4
```

- fixed OP4 -> OP3 -> OP2 -> OP1 evaluation;
- every operator advances once/sample;
- 16 B routing state;
- PI32v2 router object 362 B.

D051 ratio mapping:

- 16 stable ratios from 0.5x to 16x;
- exact Q8 integer mapping;
- deterministic clamp/rounding semantics;
- exhaustive 262,144-output verification;
- PI32v2 ratio object 264 B;
- source remains isolated from `dinghy.c`.

Canonical WP5.3 final run: `40139154497`, host `121006986068`, target `121006985928`, evidence `docs/WP5_3_VERIFICATION.md`.

Full running application remains exact WP5.0: 72,792 B image / 19,676 B normal RAM / 113,000 B pool, identity `FM-1_987`.

## Next work package

**WP5.4 - FM4 operator level and modulation-index mapping**, issue #47, expected branch `phase5/fm4-levels`.

WP5.4 starts contract-first. D052 must decide the smallest musically useful bounded control model for D050 `level_q15[4]` and modulation-depth semantics, including whether one global modulation depth is sufficient or a bounded per-operator/per-edge model is justified.

WP5.4 does not include feedback, envelopes, voice integration, tracker-facing p-lock pages/IDs or audible mixed FM playback.

## FM4 sequence

1. WP5.1 fixed-point operator - COMPLETE
2. WP5.2 curated algorithms - COMPLETE
3. WP5.3 integer ratios/pitch mapping - COMPLETE
4. WP5.4 levels / modulation index - NEXT
5. WP5.5 bounded feedback
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
