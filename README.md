# FM1Tracker

FM1Tracker is a hardware-first tracker/sequencer firmware project for the M-VAVE FM-1.

The project takes workflow inspiration from trackers such as Furnace, but it is **not** a port of the Furnace engine or codebase. The goal is a compact deterministic instrument built around the FM-1 hardware, integer DSP, tracker editing and immediate performance capture.

License: GPL-3.0-only unless otherwise noted. See `THIRD_PARTY.md`.

## Current status

**Phase 5 is in progress. WP5.0 mixed-engine routing and WP5.1 fixed-point FM operator are complete. WP5.2 curated FM4 algorithm routing is active contract-first.**

Completed foundation includes:

- pinned Dinghy/Felucca/SLOOP platform baseline;
- 8 logical tracks x 64 steps x 16 patterns;
- exact integer transport;
- stopped STEP REC with NOTE / REST / TIE / OFF;
- compact tracker display;
- 2,048-record sparse p-lock pool;
- exact six-tick tracker effects;
- bounded 32-entry main-to-audio queue;
- complete PULSE, NOISE, TRIANGLE and WAVETABLE-8 family;
- common integer ADSR;
- deterministic eight-slot 8-BIT allocation;
- D046 full representative/realtime/target closeout;
- D047/D048 per-instrument mixed-engine routing;
- D049 deterministic fixed-point FM operator primitive.

## Mixed-engine architecture

FM1Tracker is **one engine-agnostic tracker/sequencer**.

Tracker rows/tracks reference instruments, and each instrument owns engine family + engine identity. A project is not globally locked to one sound family.

WP5.0 proved:

- `FmtSongMeta.mode` no longer gates valid playback;
- 64 B instrument family/engine publication;
- 8 B global family route state;
- static bounded family dispatch;
- deterministic fail-safe same-track cross-family replacement;
- non-applicable family-specific p-locks are ignored, never reinterpreted;
- D046 all-8-BIT output remains byte-identical.

Canonical WP5.0 exit: run `40129822006`, job `120978955451`, evidence `docs/WP5_0_VERIFICATION.md`.

## Fixed-point FM operator baseline

WP5.1 / D049 is complete and remains intentionally isolated from the running application.

Verified operator facts:

- 4 B state, one `uint32_t phase`;
- one cycle = 2^32 phase units;
- signed `int32_t` phase modulation;
- Q15 level `0..32767`;
- 257-entry generated positive quarter-wave table, 514 B payload;
- 65,536 exact half-cycle symmetry probes;
- deterministic 4,096-render SHA `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66`;
- no loop, heap, float, int64, divide or modulo in the primitive;
- pinned PI32v2 object text+rodata only 692 B;
- full application remains exact WP5.0.

Canonical WP5.1 exit:

- head `52df09bf363ad5a9ad2d722a38734eaa580ecb90`
- run `40134823600`
- host job `120994003059`
- target job `120994002842`
- evidence `docs/WP5_1_VERIFICATION.md`

## Active work package

**WP5.2 - curated FM4 algorithm routing**, issue #45, branch `phase5/fm4-algorithms`.

WP5.2 must freeze D050 before production routing code. It will define a small curated four-operator algorithm set, exact graph topology/evaluation order, modulation accumulation, carrier summing and deterministic graph vectors.

It does **not** include ratios, final level/mod-index UX, feedback, envelopes, voice integration, p-lock pages or audible mixed FM playback.

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

Core future work includes seamless track switching during capture, drums, transport-locked metronome, CHANNEL/GLOBAL performance FX/automation, bounded tracker-editable capture, song/order/save/load and physical hardware UX freeze.

## Start here

For a new session, read `START_HERE.md` first.

Authoritative execution files include:

- `docs/PROJECT_STATE.md`
- `docs/WP5_1_VERIFICATION.md`
- `docs/WP5_1_FM_OPERATOR_CONTRACT.md`
- `docs/WP5_0_VERIFICATION.md`
- `docs/D047_MIXED_ENGINE_ARCHITECTURE.md`
- `docs/CURRENT_DECISIONS.md`
- `docs/WORK_PACKAGES.md`
- `docs/EXECUTION_PROTOCOL.md`
- `docs/CURRENT_TEST_STATE.md`
- `docs/TEST_MATRIX.md`
- `docs/ROADMAP.md`

Repository state wins over old chat context.

No physical FM-1 is currently available, so physical install/boot/audio/control/screen feel and measured AC791N execution margin remain later explicit gates.

`hardware_margin_claim=none`
