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
- Phase 5 mixed-engine routing + FM4: COMPLETE at software level
  - WP5.0 mixed-engine routing: COMPLETE
  - WP5.1 fixed-point FM operator: COMPLETE
  - WP5.2 curated FM4 routing: COMPLETE
  - WP5.3 integer ratio/pitch mapping: COMPLETE
  - WP5.4 level/modulation-index mapping: COMPLETE
  - WP5.5 bounded feedback: COMPLETE
  - WP5.6 per-operator ADSR envelopes: COMPLETE
  - WP5.7 FM voice/router integration: COMPLETE
  - WP5.8 FM parameter pages + p-lock mapping: COMPLETE
  - WP5.9 mixed 8-BIT + FM4 validation: COMPLETE
  - WP5.10 worst-case mixed-engine benchmark + Phase 5 exit: COMPLETE
- live performance capture milestone: PLANNED after engine foundations
- later engine-family expansion, including virtual-analog/subtractive and drums: PLANNED, contracts not yet frozen
- Phase 6 song/order/persistence: PLANNED - next phase
- Phase 7 physical hardware UX freeze: BLOCKED until physical FM-1
- Phase 8 interoperability: PLANNED
- Phase 9 optional extensions: DEFERRED

## Architecture after Phase 5

FM1Tracker is one engine-agnostic tracker/sequencer. Instruments own family + engine identity. D048 common routing is static/bounded. FM4 has eight family-local runtime voices using exact D043 ownership semantics. D056 extends the shared semantic parameter/p-lock path without introducing a parallel tracker model. The D046/WP5.0 all-8BIT configuration remains the exact regression baseline.

The final Phase 5 target-validated production source remains:

`81d3464059270130747e26fcc950b7a2e1bb03e1`

Final target footprint:

```text
image       79836 B
normal RAM  27792 B
pool       113000 B
```

WP5.10 demonstrates exact 8 FM4 + 8 8-BIT family-local non-idle occupancy under the frozen mixed-saturation workload while preserving eight logical tracker tracks. The full callback-pressure case also adds queue depth 32 and four audition voices. Deterministic repeated host evidence, realtime audits, inherited regressions and the pinned ordinary target build are green.

No production optimization was required for the Phase 5 software exit.

```text
phase5_software_exit=pass
physical_audio_stability=unverified
physical_cpu_headroom=unverified
physical_free_ram_margin=unverified
target_runtime_timing_gate=deferred_without_execution_path
hardware_margin_claim=none
```

Full evidence: `WP5_10_CLOSEOUT_EVIDENCE.md`.

No active cross-track primary stealing is permitted.

## Phase 5 closeout

Phase 5 delivered the common mixed-engine routing foundation and integrated FM4 alongside the existing 8-BIT family without creating a parallel tracker model. The final validation and benchmark packages demonstrated mixed family behavior, exact worst-case family-local occupancy, bounded callback pressure and unchanged final target resources relative to WP5.9.

Physical audio stability, callback/CPU headroom and physical free-RAM margin remain intentionally unverified until hardware exists.

## Mandatory core milestone - live performance capture + performance FX

PLAY continues while performing, REC arms/disarms capture without stopping transport, track switching cannot misroute writes, drums are playable/capturable, metronome is transport-locked, and CHANNEL/GLOBAL automation remains bounded and tracker-editable.

Reference: `LIVE_PERFORMANCE_CAPTURE_VISION.md`.

## Future engine-family expansion

After the mixed-engine/FM foundation, additional bounded engine families can be introduced through the same common tracker architecture. Virtual-analog/subtractive synthesis and drums/percussion are explicit product directions, not separate applications. Each receives its own preimplementation audit and work package before code is added.

## Phase 6 - song/order/persistence

Order list, pattern switching, repeats/loops, versioned binary format, save/load/CRC, corruption recovery and migration. Persistence must encode D047/D048 per-instrument engine identity so future engine families remain representable without a parallel project format.

Phase 6 is the next planned phase. Its first implementation package must be preceded by an audit and frozen contract. Do not infer a storage format directly from roadmap prose.

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
