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
  - WP5.2 curated four-operator routing: COMPLETE
  - WP5.3 integer ratio/pitch mapping: COMPLETE
  - WP5.4 level/modulation-index mapping: COMPLETE
  - WP5.5 bounded feedback: TECHNICALLY COMPLETE / CLOSEOUT
  - WP5.6 envelope strategy: NEXT after closeout
- live performance capture milestone: PLANNED after engine foundations
- Phase 6 song/order/persistence: PLANNED
- Phase 7 physical hardware UX freeze: BLOCKED until physical FM-1
- Phase 8 interoperability: PLANNED
- Phase 9 optional extensions: DEFERRED

## Architecture after D047/D048

FM1Tracker is one engine-agnostic tracker/sequencer. Instruments own family + engine identity. D048 common routing is static/bounded and the D046 all-8-BIT configuration remains the regression baseline.

## Phase 5 - mixed-engine routing + FM4

### WP5.0 - mixed-engine routing - COMPLETE

D047/D048 implemented and verified. Final target remains 72,792 B image / 19,676 B RAM / 113,000 B pool.

### WP5.1 - fixed-point FM operator - COMPLETE

D049 established the 4 B phase-state operator, signed phase modulation, generated Q15 sine table and deterministic integer render semantics. PI32v2 object: 692 B.

### WP5.2 - curated FM algorithm routing - COMPLETE

D050 established four static topologies and fixed OP4 -> OP3 -> OP2 -> OP1 evaluation. PI32v2 router object: 362 B.

### WP5.3 - integer ratio table + pitch mapping - COMPLETE

D051 froze 16 stable Q8 ratios, deterministic high-register clamp behavior and a stateless four-output mapping. PI32v2 ratio object: 264 B.

### WP5.4 - levels / modulation index - COMPLETE

D052 froze four LEVEL bytes plus one global INDEX byte, exact Q15/Q14 mapping and zero persistent state. PI32v2 controls object: 108 B.

### WP5.5 - bounded feedback - TECHNICALLY COMPLETE / CLOSEOUT

D053 establishes one deterministic OP4 self-feedback edge with exactly one-sample history.

Frozen core:

```text
history state  signed int16, 2 B
FEEDBACK       0..255
Q14 depth      (v << 6) | (v >> 2)
phase offset   previous_op4 * depth * 4
```

Reset/retrigger clear history. Algorithm switches retain it. FEEDBACK=0 leaves the phase unmodulated while continuing to track OP4 history.

Measured PI32v2 feedback object: 130 B text+rodata. Future D050 + D053 synthesis state before envelopes: 18 B. Full application remains exact WP5.0.

Canonical implementation run `40164953532`, host `121083999893`, target `121083999790`. Evidence: `WP5_5_VERIFICATION.md`.

### WP5.6 - envelope strategy - NEXT AFTER CLOSEOUT

Issue #49. D054 must be frozen contract-first before production envelope code.

The package must compare a common amplitude envelope against per-operator envelopes using both measured resource cost and the ability to create time-varying FM spectra. It must then freeze the smallest useful V1 stage architecture, integer rate/level model, reset/retrigger behavior, update cadence and exact deterministic vectors.

Preserve D049-D053 contracts. Do not add FM voice allocation, D048 integration or tracker-facing parameter semantics here.

### WP5.7 - FM voice allocation/integration

Connect FM into D048 family routing/ownership without active cross-track primary stealing.

### WP5.8 - FM parameter pages + p-lock mapping

Stable semantic IDs/pages and safe family-specific parameter behavior.

### WP5.9 - mixed 8-BIT + FM4 validation

Simultaneous families and same-track family switches with exact all-8-BIT regression equivalence.

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
