# FM1Tracker Roadmap

FM1Tracker is a hardware-first tracker/sequencer firmware for the M-VAVE FM-1.

`PROJECT_STATE.md` defines current execution. `WORK_PACKAGES.md` defines bounded scope. This file defines phase/product direction.

## Product north star

**Tracker depth underneath, immediate capture on top.**

The instrument should feel playable before it feels editable. Tracker data remains the source of truth, timing stays deterministic, realtime work stays bounded, and eight logical tracks are never silently reduced.

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
  - WP5.7 FM voice/router integration: TECHNICALLY COMPLETE, closeout active
  - WP5.8 FM parameter pages + p-lock mapping: NEXT
- live performance capture milestone: PLANNED after engine foundations
- Phase 6 song/order/persistence: PLANNED
- Phase 7 physical hardware UX freeze: BLOCKED until physical FM-1
- Phase 8 interoperability: PLANNED
- Phase 9 optional extensions: DEFERRED

## Architecture after D055

FM1Tracker is one engine-agnostic tracker/sequencer. Instruments own family + engine identity. D048 common routing is static/bounded. FM4 now has eight family-local runtime voices using exact D043 ownership semantics. The D046/WP5.0 all-8BIT configuration remains the exact regression baseline.

WP5.7 target checkpoint:

```text
image       77048 B
normal RAM  20464 B
pool       113000 B
FM engine     788 B
```

No active cross-track primary stealing is permitted.

## Phase 5 remaining work

### WP5.8 - FM parameter pages + p-lock mapping

Define stable semantic FM parameter IDs/pages and bounded mutable instrument parameter storage. Reuse the existing sparse p-lock architecture and preserve exact row-boundary timing. Invalid IDs must be family-safe and deterministic. This package replaces the temporary immutable WP5.7 FM bridge patch with parameterized controls, but does not broaden into final mixed-family stress validation.

### WP5.9 - mixed 8-BIT + FM4 validation

Simultaneous families and same-track family switches with exact all-8BIT regression equivalence.

### WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit

Representative mixed workload, realtime audit, inherited regressions and exact target closeout.

## Mandatory core milestone - live performance capture + performance FX

PLAY continues while performing, REC arms/disarms capture without stopping transport, track switching cannot misroute writes, drums are playable/capturable, metronome is transport-locked, and CHANNEL/GLOBAL automation remains bounded and tracker-editable.

Reference: `LIVE_PERFORMANCE_CAPTURE_VISION.md`.

## Phase 6 - song/order/persistence

Order list, pattern switching, repeats/loops, versioned binary format, save/load/CRC, corruption recovery and migration. Persistence must encode D047/D048 per-instrument engine identity.

## Phase 7 - physical hardware UX freeze

Requires physical FM-1 for controls, install/boot/audio/display and measured callback margin.

## Phase 8 - interoperability

Internal interchange, MIDI transfer investigation and Furnace import/export audit. D003 remains authoritative: Furnace is a workflow/interoperability reference, not a full engine port.

## Phase 9 - optional extensions

Additional engines/features only after explicit CPU/RAM/parameter/license review.

## Non-negotiable rules

- tracker remains source of truth;
- timing remains deterministic;
- realtime work remains bounded;
- no silent reduction from eight tracks;
- architecture decisions precede implementation;
- one active package at a time;
- physical claims require physical evidence.

`hardware_margin_claim=none` until physical hardware evidence exists.
