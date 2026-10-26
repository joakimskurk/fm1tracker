# FM1Tracker Project Context

This file preserves the product context that must survive chat/session changes. It complements `PROJECT_STATE.md` and `ROADMAP.md`; it is not a work-package checklist.

## Product identity

FM1Tracker is a hardware-first tracker/sequencer firmware for the M-VAVE FM-1.

The goal is one coherent instrument with tracker depth underneath and immediate musical capture on top. The tracker data model remains the source of truth while different sound-engine families coexist inside the same project.

FM1Tracker is **not** intended to become an FM-only tracker.

The refinement target is now also explicit: **playable first, characterful immediately, deep when opened**. The functioning tracker/engine skeleton should be sharpened into a stronger instrument rather than expanded by feature count alone. See `PRODUCT_REFINEMENT_DIRECTION.md`.

## Intended engine families

The product direction includes, in the same tracker environment:

- **8-BIT** synthesis / chip-style engine family;
- **FM** synthesis, currently represented by the FM4 engine family;
- **virtual-analog / subtractive-style synthesis** or an equivalent bounded synthesis family suited to the FM-1 hardware;
- **drums / percussion**, playable and recordable alongside melodic instruments.

The exact architecture, DSP design and resource budget for future virtual-analog and drum engines are intentionally not frozen here. They require their own audit and work-package contract before implementation.

## Mixed-engine principle

Different engine families are parts of one engine-agnostic tracker, not separate applications.

Per-instrument family + engine identity is the architectural direction already established by the mixed-engine work. Future engine families must integrate through the common bounded routing/parameter architecture rather than creating a parallel tracker model.

The intended user experience is that 8-BIT, FM, future virtual-analog/subtractive instruments and drums can coexist in the same musical project. Exact track/voice allocation details for not-yet-implemented engines remain subject to later resource validation.

## Performance direction

The instrument should feel playable before it feels editable.

Live performance capture is a mandatory core milestone, including transport-safe recording, track switching without misrouted writes, playable/capturable drums, metronome sync and bounded tracker-editable automation.

The current inherited HOME/sine-audition surface is a development scaffold, not the final product experience. Future HOME/performance work must use the actual selected instrument through the common engine architecture and should provide compact selected-instrument identity, eight-track activity/active-note feedback and a bounded Furnace-inspired spectrum visualization.

## Instrument and drum direction

A coherent instrument editor is a first-class product surface. It should expose musical instrument identity and engine-specific controls while keeping persistent base parameters distinct from tracker row overrides.

A Furnace-style drum keyboard map is now an explicit product requirement. The intended general model is:

```text
KEY / NOTE -> SOUND SLOT -> SOURCE
```

The source may later be sample-backed or synthesized under an explicit drum-engine contract. Tracker lanes remain generic: a normal tracker note plus instrument identity is resolved by the drum instrument's note map rather than by hard-coded drum-track types.

## Sonic baseline

Existing synthesis engines must be demonstrated through a deliberately curated, musically convincing factory bank. Engine primitives and development defaults alone are not an acceptable final sonic starting point.

The 8-BIT and FM4 families should be pushed musically before adding new synthesis families solely to increase feature count. Factory instruments should use the same deterministic parameter/persistence architecture as user instruments where practical.

## Phase 6 format consequence

Phase 6 remains the next implementation phase, but its format audit/contract must preserve the accepted refinement direction before persistence is frozen.

The future project format must be able to represent versioned/extensible instrument identity, human-readable instrument names, engine-specific payloads, future drum NOTE MAP data and stable future asset/sample references without raw runtime pointers or hard-coded host file paths. This requirement does not itself implement PCM/sample playback or the final UI.

## Non-negotiable product rules

- one coherent tracker/sequencer, not separate engine-specific apps;
- tracker data remains the source of truth;
- timing remains deterministic;
- realtime work remains bounded;
- eight logical tracks are never silently reduced;
- new engine families require explicit CPU/RAM/parameter/license review;
- architecture decisions precede implementation;
- physical hardware claims require physical evidence;
- HOME/performance must eventually represent the real selected instrument, not a legacy test voice;
- future drum support must preserve arbitrary note-to-sound mapping without hard-coded drum tracks;
- product refinement favors musical usefulness and coherence over feature count.

## Documentation rule

Roadmap order is not project memory.

When a work package changes product behavior, architecture, UX assumptions, resource constraints or future direction, the relevant context documents must be updated at the same checkpoint as code/test state. A new chat should be able to reconstruct both **what FM1Tracker is** and **what happens next** from the repository alone.
