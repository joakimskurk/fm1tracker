# FM1Tracker Roadmap

FM1Tracker is a hardware-first tracker/sequencer firmware for the M-VAVE FM-1.

`PROJECT_CONTEXT.md` defines persistent product intent. `PROJECT_STATE.md` defines current execution. `WORK_PACKAGES.md` defines bounded scope. This file defines phase/product direction.

## Product north star

**Tracker depth underneath, immediate capture on top.**

The instrument should feel playable before it feels editable. Tracker data remains the source of truth, timing stays deterministic, realtime work stays bounded, and eight logical tracks are never silently reduced.

FM1Tracker is one engine-agnostic tracker environment. It is not intended to become an FM-only tracker.

## Intended engine families

The product direction includes, in the same tracker/project environment:

- 8-BIT synthesis / chip-style engines;
- FM synthesis, currently FM4;
- future virtual-analog / subtractive-style synthesis or an equivalent bounded synthesis family suited to the hardware;
- drums / percussion.

Future virtual-analog and drum implementations require their own CPU/RAM/parameter/license audit and frozen work-package contract before production code. Their exact DSP and allocation architecture is intentionally not specified yet.

## Current status

- Phase 1 platform foundation: COMPLETE
- Phase 2 tracker core: COMPLETE
- Phase 3 p-locks/effects/runtime ingress: COMPLETE
- Phase 4 first 8-BIT engine family: COMPLETE
- Phase 5 mixed-engine routing + FM4: IN PROGRESS
  - WP5.0 mixed-engine routing: COMPLETE
  - WP5.1 fixed-point FM operator: COMPLETE
  - WP5.2 curated FM4 routing: COMPLETE
  - WP5.3 integer ratio/pitch mapping: COMPLETE
  - WP5.4 level/modulation-index mapping: COMPLETE
  - WP5.5 bounded feedback: COMPLETE
  - WP5.6 per-operator ADSR envelopes: COMPLETE
  - WP5.7 FM voice/router integration: COMPLETE
  - WP5.8 FM parameter pages + p-lock mapping: COMPLETE
  - WP5.9 mixed 8-BIT + FM4 validation: NEXT
  - WP5.10 worst-case mixed-engine benchmark + Phase 5 exit: PLANNED
- live performance capture milestone: PLANNED after engine foundations
- later engine-family expansion, including virtual-analog/subtractive and drums: PLANNED, contracts not yet frozen
- Phase 6 song/order/persistence: PLANNED
- Phase 7 physical hardware UX freeze: BLOCKED until physical FM-1
- Phase 8 interoperability: PLANNED
- Phase 9 optional extensions: DEFERRED

## Architecture after D056

FM1Tracker is one engine-agnostic tracker/sequencer. Instruments own family + engine identity. D048 common routing is static/bounded. FM4 has eight family-local runtime voices using exact D043 ownership semantics. D056 extends the shared semantic parameter/p-lock path without introducing a parallel tracker model. The D046/WP5.0 all-8BIT configuration remains the exact regression baseline.

Accepted D056 target checkpoint:

```text
image       79804 B
normal RAM  27792 B
pool       113000 B
FM engine     788 B
```

The target is 864 B below the frozen WP5.8 normal-RAM package ceiling. This is not a physical free-RAM claim, but it makes resource measurement a first-class constraint for WP5.9 and WP5.10.

No active cross-track primary stealing is permitted.

## Phase 5 remaining work

### WP5.9 - mixed 8-BIT + FM4 validation

Validate the engines we already have under genuinely mixed use before worst-case benchmarking. Coverage must include simultaneous 8-BIT + FM4 tracks, same-track family switches, ownership/state cleanup, D056 p-lock behavior, deterministic repeat renders and exact all-8BIT equivalence. Audit and freeze the validation contract before implementation changes.

WP5.9 is validation scope, not an engine-expansion package. It must not pull virtual-analog/subtractive or drum implementation forward.

### WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit

Representative worst-case mixed workload, realtime audit, inherited regressions and exact target closeout. Any resource conflict must become an explicit architecture decision; eight logical tracks may not be silently reduced.

## Mandatory core milestone - live performance capture + performance FX

PLAY continues while performing, REC arms/disarms capture without stopping transport, track switching cannot misroute writes, drums are playable/capturable, metronome is transport-locked, and CHANNEL/GLOBAL automation remains bounded and tracker-editable.

Reference: `LIVE_PERFORMANCE_CAPTURE_VISION.md`.

## Future engine-family expansion

After the current mixed-engine/FM foundation is closed and resource behavior is measured, additional bounded engine families can be introduced through the same common tracker architecture. Virtual-analog/subtractive synthesis and drums/percussion are explicit product directions, not separate applications. Each receives its own preimplementation audit and work package before code is added.

## Phase 6 - song/order/persistence

Order list, pattern switching, repeats/loops, versioned binary format, save/load/CRC, corruption recovery and migration. Persistence must encode D047/D048 per-instrument engine identity so future engine families remain representable without a parallel project format.

## Phase 7 - physical hardware UX freeze

Requires physical FM-1 for controls, install/boot/audio/display and measured callback margin.

## Phase 8 - interoperability

Internal interchange, MIDI transfer investigation and Furnace import/export audit. D003 remains authoritative: Furnace is a workflow/interoperability reference, not a full engine port.

## Phase 9 - optional extensions

Additional engines/features only after explicit CPU/RAM/parameter/license review. This phase is not a license to forget engine families already named in `PROJECT_CONTEXT.md`; roadmap placement and implementation order must be decided from measured constraints and product priorities.

## Non-negotiable rules

- one coherent multi-engine tracker/sequencer;
- tracker remains source of truth;
- timing remains deterministic;
- realtime work remains bounded;
- no silent reduction from eight tracks;
- architecture decisions precede implementation;
- one active package at a time;
- physical claims require physical evidence;
- roadmap order does not replace persistent product context;
- substantive user decisions must be synchronized back into Git.

`hardware_margin_claim=none` until physical hardware evidence exists.