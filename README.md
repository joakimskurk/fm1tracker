# FM1Tracker

FM1Tracker is a hardware-first tracker/sequencer firmware project for the M-VAVE FM-1.

The project takes workflow inspiration from trackers such as Furnace, but it is **not** a port of the Furnace engine or codebase. The goal is a compact deterministic instrument built around the FM-1 hardware, integer DSP, tracker editing and immediate performance capture.

License: GPL-3.0-only unless otherwise noted. See `THIRD_PARTY.md`.

## Current status

**Phase 5 is in progress. WP5.0 mixed-engine routing and WP5.1 fixed-point FM operator are complete. WP5.2 curated FM4 algorithm routing is active, with D050 frozen before production routing source.**

Completed foundation includes:

- pinned Dinghy/Felucca/SLOOP platform baseline;
- 8 logical tracks x 64 steps x 16 patterns;
- exact integer transport and stopped STEP REC;
- 2,048 sparse p-locks and exact six-tick tracker effects;
- bounded 32-entry realtime queue;
- complete PULSE, NOISE, TRIANGLE and WAVETABLE-8 family;
- common integer ADSR and deterministic eight-slot allocation;
- D047/D048 per-instrument mixed-engine routing;
- D049 deterministic fixed-point FM operator primitive.

## Mixed-engine architecture

FM1Tracker is **one engine-agnostic tracker/sequencer**.

Tracker rows/tracks reference instruments, and each instrument owns engine family + engine identity. A project is not globally locked to one sound family.

WP5.0 proved the bounded common router while keeping D046 8-BIT audio byte-identical.

Canonical WP5.0 exit: run `40129822006`, job `120978955451`, evidence `docs/WP5_0_VERIFICATION.md`.

## D049 FM operator baseline

WP5.1 is complete and intentionally isolated from the running application.

Verified operator facts:

- 4 B state, one uint32 phase;
- signed int32 phase modulation;
- Q15 level 0..32767;
- generated 257-entry quarter-wave table, 514 B payload;
- table SHA `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68`;
- 4,096-render SHA `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66`;
- pinned PI32v2 object text+rodata 692 B;
- full application remains exact WP5.0.

Canonical WP5.1 exit: run `40134823600`, host job `120994003059`, target job `120994002842`, evidence `docs/WP5_1_VERIFICATION.md`.

## Active work package - WP5.2 / D050

Issue #45, branch `phase5/fm4-algorithms`.

D050 freezes a four-algorithm subset:

```text
0 STACK4:    OP4 -> OP3 -> OP2 -> OP1 -> OUT
1 FAN3:      OP2 + OP3 + OP4 -> OP1 -> OUT
2 DUAL2:     OP4 -> OP3 plus OP2 -> OP1, two carriers
3 PARALLEL4: OP1 + OP2 + OP3 + OP4, four carriers
```

All algorithms evaluate OP4 -> OP3 -> OP2 -> OP1 and advance all four operators once per sample. Routing is a fixed four-case dispatch, not a runtime graph.

Production `fmt_fm_algorithm.*` is still forbidden until the committed source-absent D050 reference gate is green.

WP5.2 does not include ratios, final level/mod-index UX, feedback, envelopes, voice integration, p-lock pages or audible mixed FM playback.

## Planned FM4 sequence

1. WP5.1 fixed-point operator primitive - COMPLETE
2. WP5.2 curated algorithm routing - ACTIVE
3. WP5.3 integer ratio table
4. WP5.4 levels / modulation index
5. WP5.5 bounded feedback
6. WP5.6 envelope strategy
7. WP5.7 voice allocation/integration
8. WP5.8 semantic parameter pages + p-lock mapping
9. WP5.9 mixed 8-BIT + FM4 validation
10. WP5.10 worst-case mixed-engine benchmark / Phase 5 exit

## Product direction

**Tracker depth underneath, immediate performance/capture on top.**

The finished instrument should support precise stopped STEP REC and live capture while playback continues. Tracker data remains the source of truth.

Live performance capture, drums, transport-locked metronome, CHANNEL/GLOBAL performance FX/automation, song/order/save/load and physical hardware UX freeze remain core future work.

## Start here

For a new session, read `START_HERE.md` first. Repository state wins over old chat context.

No physical FM-1 is currently available, so physical install/boot/audio/control/screen feel and measured AC791N execution margin remain later explicit gates.

`hardware_margin_claim=none`
