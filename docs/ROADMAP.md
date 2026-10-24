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
  - WP5.3 integer ratio/pitch mapping: ACTIVE
- live performance capture milestone: PLANNED after engine foundations
- Phase 6 song/order/persistence: PLANNED
- Phase 7 physical hardware UX freeze: BLOCKED until physical FM-1
- Phase 8 interoperability: PLANNED
- Phase 9 optional extensions: DEFERRED

## Architecture after D047/D048

FM1Tracker is one engine-agnostic tracker/sequencer. Instruments own family + engine identity. D048 common routing is static/bounded and the D046 all-8-BIT configuration remains the regression baseline.

## Phase 4 - first engine family - COMPLETE

PULSE, NOISE, TRIANGLE, WAVETABLE-8, common ADSR, deterministic allocation, parameter pages and linked eight-track playback.

Canonical D046 run `40124536375`, job `120963022289`.

## Phase 5 - mixed-engine routing + FM4

### WP5.0 - mixed-engine routing - COMPLETE

D047/D048 implemented and verified. Final target remains 72,792 B image / 19,676 B RAM / 113,000 B pool.

### WP5.1 - fixed-point FM operator - COMPLETE

D049 established the 4 B phase-state operator, signed phase modulation, generated Q15 sine table and deterministic integer render semantics. PI32v2 object: 692 B.

### WP5.2 - curated FM algorithm routing - COMPLETE

D050 established four static topologies:

```text
STACK4    OP4 -> OP3 -> OP2 -> OP1
FAN3      OP2 + OP3 + OP4 -> OP1
DUAL2     OP4 -> OP3 plus OP2 -> OP1
PARALLEL4 OP1 + OP2 + OP3 + OP4
```

All four operators advance once/sample in fixed OP4 -> OP3 -> OP2 -> OP1 evaluation order. PI32v2 router object: 362 B. Full application remains exact WP5.0.

Canonical run `40136425976`, host `120998804031`, target `120998804175`.

### WP5.3 - integer ratio table + pitch mapping - ACTIVE

D051 contract-first work must freeze:

- compact musically useful ratio set;
- stable ratio IDs;
- exact mathematical meanings;
- integer base-increment -> operator-increment arithmetic;
- rounding and overflow/clamp policy;
- supported base-increment/high-register domain;
- fractional/sub-unity scope;
- deterministic pitch/ratio fixtures;
- bounded mapping API/state;
- target object/realtime ceilings.

No production ratio source before a green D051 source-absent gate.

### WP5.4 - levels / modulation index

Final bounded user-facing level and modulation-index mappings.

### WP5.5 - bounded feedback

Explicit deterministic feedback topology and stability/overflow stress.

### WP5.6 - envelope strategy

Freeze common versus per-operator strategy from measured cost.

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
