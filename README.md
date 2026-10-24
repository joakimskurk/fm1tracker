# FM1Tracker

FM1Tracker is a hardware-first tracker/sequencer firmware project for the M-VAVE FM-1.

The project takes workflow inspiration from trackers such as Furnace, but it is **not** a port of the Furnace engine or codebase. The goal is a compact, deterministic instrument designed around the FM-1 hardware, integer DSP, tracker editing and immediate performance capture.

License: GPL-3.0-only unless otherwise noted. See `THIRD_PARTY.md` for provenance and external components.

## Current status

**Phase 4 is complete at host/device-build level. Phase 5 / WP5.0 is active.**

Completed foundation:

- clean pinned Dinghy/Felucca/SLOOP platform baseline;
- 8 logical tracks x 64 steps x 16 patterns;
- exact integer transport;
- stopped STEP REC with NOTE / REST / TIE / OFF;
- compact tracker display;
- 2,048-record sparse p-lock pool;
- tracker effects with exact six-tick timing;
- bounded 32-entry main-to-audio queue;
- PULSE, NOISE, TRIANGLE and WAVETABLE-8 engines;
- common integer ADSR;
- deterministic eight-slot tracker voice allocation;
- linked all-eight-track 8-BIT playback;
- full representative/full-pool/realtime/pinned-target Phase 4 closeout.

Canonical Phase 4 exit:

- production baseline: `c93c39ad344760b4da0dfbc76829ca5fef886174`
- verification head: `2379d3e7e52d1cefe9f845f8287ad0ccda8c019a`
- run: `40124536375`
- job: `120963022289`
- evidence: `docs/WP4_10_VERIFICATION.md`
- representative WAV SHA-256: `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9`
- target footprint: 72,504 B image / 19,604 B normal RAM / 113,000 B pool
- zero active cross-track steals in the D046 representative/long-run ownership evidence
- no physical-hardware execution-margin claim: `hardware_margin_claim=none`

No physical FM-1 is currently available, so install/boot/audio/control/screen feel and measured AC791N execution margin remain later explicit hardware gates.

## Current architecture direction

D047 establishes FM1Tracker as **one engine-agnostic tracker/sequencer**.

The project is not globally locked to 8-BIT or 16-BIT. Instruments own engine family + engine identity, and tracker rows/tracks reference instruments.

The first complete all-8-BIT family remains the regression baseline while the common mixed-family router is introduced.

## Active work package

**WP5.0 - mixed-engine routing contract + migration**

Before FM4 production implementation, WP5.0 must freeze and prove:

- per-instrument family + engine routing;
- removal of `FmtSongMeta.mode` as a project-wide family gate while preserving compatibility;
- simultaneous different engine families across logical tracks;
- deterministic same-track cross-family instrument switching;
- one owned primary sequenced voice per logical track across families;
- no active cross-track primary stealing;
- bounded static dispatch and ownership representation;
- safe engine-specific parameter/p-lock behavior;
- exact all-8-BIT D046 regression equivalence through the new router.

Do not jump directly to FM4 operators before WP5.0 closes.

## Planned FM4 sequence

After WP5.0:

1. four-operator fixed-point primitive
2. curated algorithm routing
3. integer ratio table
4. operator levels / modulation index
5. bounded feedback
6. envelope strategy
7. voice allocation/integration
8. semantic parameter pages + p-lock mapping
9. mixed 8-BIT + FM4 validation
10. worst-case mixed-engine benchmark / Phase 5 exit

## Product direction

The long-term interaction goal is:

**tracker depth underneath, immediate performance/capture on top.**

The finished instrument should support stopped STEP REC for precise programming and live capture while playback continues. Tracker data remains the source of truth.

Core future work includes:

- seamless track switching during live capture;
- drums;
- transport-locked metronome;
- CHANNEL and GLOBAL performance FX/automation;
- bounded tracker-editable parameter/FX capture;
- song/order/save/load;
- physical hardware UX freeze.

Live performance capture is a core milestone, not an optional extension.

## Start here

For a new session or contributor, read [`START_HERE.md`](START_HERE.md) first.

The authoritative execution files are:

- `docs/PROJECT_STATE.md`
- `docs/D047_MIXED_ENGINE_ARCHITECTURE.md`
- `docs/CURRENT_DECISIONS.md`
- `docs/WORK_PACKAGES.md`
- `docs/EXECUTION_PROTOCOL.md`
- `docs/CURRENT_TEST_STATE.md`
- `docs/TEST_MATRIX.md`
- `docs/ROADMAP.md`
- `docs/WP4_10_VERIFICATION.md`

Repository state wins over old chat context.
