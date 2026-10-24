# FM1Tracker

FM1Tracker is a hardware-first tracker/sequencer firmware project for the M-VAVE FM-1.

The project takes workflow inspiration from trackers such as Furnace, but it is **not** a port of the Furnace engine or codebase. The goal is a compact deterministic instrument built around the FM-1 hardware, integer DSP, tracker editing and immediate performance capture.

License: GPL-3.0-only unless otherwise noted. See `THIRD_PARTY.md`.

## Current status

**Phase 5 is in progress. WP5.0 mixed-engine routing is complete. WP5.1 fixed-point FM operator primitive is active.**

Completed foundation:

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
- linked all-eight-track 8-BIT playback;
- D046 representative/full-pool/realtime/target closeout;
- D047/D048 per-instrument mixed-engine routing foundation.

## Mixed-engine architecture

FM1Tracker is **one engine-agnostic tracker/sequencer**.

The project is not globally locked to 8-BIT or 16-BIT. Tracker rows/tracks reference instruments, and each instrument owns engine family + engine identity.

WP5.0 proved this architecture without changing the verified 8-BIT sound output:

- `FmtSongMeta.mode` no longer gates valid playback;
- instrument family/engine publication is fixed at 64 B;
- global family route state is fixed at 8 B;
- family dispatch is static and bounded;
- cross-family replacement has deterministic fail-safe ordering;
- malformed/unsupported family/engine fails toward silence;
- non-applicable family-specific p-locks are ignored, never reinterpreted;
- D046 representative and normal WAVs remain byte-identical;
- no FM4 production DSP was mixed into the routing migration.

Canonical WP5.0 exit:

- head `813792fb25632b4d7acda787422e51f713fad21e`
- run `40129822006`
- job `120978955451`
- evidence `docs/WP5_0_VERIFICATION.md`
- target 72,792 B image / 19,676 B normal RAM / 113,000 B pool
- delta versus D046 +288 B / +72 B / +0 B
- BIN `a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125`
- FWSC `cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073`
- `hardware_margin_claim=none`

## Active work package

**WP5.1 - fixed-point FM operator primitive**, issue #44, branch `phase5/fm4-operator`.

Work remains contract-first. Before production FM operator code, WP5.1 must freeze:

- operator state and exact phase format;
- deterministic integer carrier lookup;
- signed modulation input format;
- level/output range;
- reset semantics;
- overflow/saturation behavior;
- exact deterministic host vectors;
- target/realtime/resource guards.

WP5.1 does not include algorithm routing, ratios, feedback, envelopes, voice allocation, FM parameter pages or mixed production FM playback.

## Planned FM4 sequence

1. WP5.1 fixed-point operator primitive
2. WP5.2 curated algorithm routing
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

Live performance capture is a core milestone, not an optional extension.

## Start here

For a new session, read `START_HERE.md` first.

Authoritative execution files:

- `docs/PROJECT_STATE.md`
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
