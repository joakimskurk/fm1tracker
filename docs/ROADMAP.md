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
  - WP5.6 per-operator ADSR envelopes: TECHNICALLY COMPLETE, closeout active
  - WP5.7 FM voice/router integration: PLANNED, blocked on WP5.6 closeout
- live performance capture milestone: PLANNED after engine foundations
- Phase 6 song/order/persistence: PLANNED
- Phase 7 physical hardware UX freeze: BLOCKED until physical FM-1
- Phase 8 interoperability: PLANNED
- Phase 9 optional extensions: DEFERRED

## Architecture after D047/D048

FM1Tracker is one engine-agnostic tracker/sequencer. Instruments own family + engine identity. D048 common routing is static/bounded and the D046 all-8-BIT configuration remains the regression baseline.

## Phase 5 - mixed-engine routing + FM4

### WP5.0 - mixed-engine routing - COMPLETE

D047/D048 implemented and verified. Full target baseline: 72,792 B image / 19,676 B RAM / 113,000 B pool.

### WP5.1 - fixed-point FM operator - COMPLETE

D049 established the 4 B phase-state operator, signed phase modulation, generated Q15 sine table and deterministic integer render semantics.

### WP5.2 - curated FM algorithm routing - COMPLETE

D050 established four static topologies and fixed OP4 -> OP3 -> OP2 -> OP1 evaluation.

### WP5.3 - integer ratio table + pitch mapping - COMPLETE

D051 froze 16 stable Q8 ratios, deterministic high-register clamp behavior and a stateless four-output mapping.

### WP5.4 - levels / modulation index - COMPLETE

D052 froze four independent LEVEL bytes and one global INDEX byte with exact Q15/Q14 mappings.

### WP5.5 - bounded feedback - COMPLETE

D053 froze one OP4 self-feedback edge, exact one-sample history and a 2 B feedback state. Final closeout run `40165352623` passed host and target.

### WP5.6 - four independent operator envelopes - TECHNICALLY COMPLETE

D054 rejected a final post-mix envelope and froze the smallest V1 envelope architecture that gives real FM timbral evolution: four independent ADSR envelopes applied to the four D052 operator LEVEL values before D049/D050 rendering.

Frozen foundation:

```text
state per future FM voice     20 B
controls per FM instrument    16 B
A/D/R step table            1024 B
PI32v2 object               1680 B text+rodata
object ceiling              2048 B
```

Control 0 is a true instant stage transition. Controls 1..255 use a deterministic 16-steps-per-octave time law from roughly 1 ms through roughly 61.3 s. SUSTAIN uses the D052 LEVEL mapping. Hard retrigger starts from zero, release starts from current level, and D050 algorithm changes do not reset envelope state.

Canonical technical verification:

- source-absent run `40167537148`, job `121091550742`;
- implementation run `40167933064`, host `121092688126`, target `121092688295`;
- full target remains exact WP5.0.

Evidence: `WP5_6_VERIFICATION.md`.

Formal closeout requires the same verification on the documentation closeout head.

### WP5.7 - FM voice allocation/integration - NEXT AFTER WP5.6 CLOSEOUT

Issue #50. D055 must be frozen contract-first before integration.

This package connects D049-D054 to D048 without changing the tracker architecture. It must freeze FM voice count/state budget, allocator ownership, release-tail policy and exact cross-family event ordering before code changes.

Non-negotiable:

- no active cross-track primary stealing;
- all-8-BIT output remains exact;
- fixed bounded render/event work;
- no dynamic engine registry or heap;
- target image/RAM/pool measured before package closeout.

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
