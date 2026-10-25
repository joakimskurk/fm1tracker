# FM1Tracker Project Context

This file preserves the product context that must survive chat/session changes. It complements `PROJECT_STATE.md` and `ROADMAP.md`; it is not a work-package checklist.

## Product identity

FM1Tracker is a hardware-first tracker/sequencer firmware for the M-VAVE FM-1.

The goal is one coherent instrument with tracker depth underneath and immediate musical capture on top. The tracker data model remains the source of truth while different sound-engine families coexist inside the same project.

FM1Tracker is **not** intended to become an FM-only tracker.

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

## Non-negotiable product rules

- one coherent tracker/sequencer, not separate engine-specific apps;
- tracker data remains the source of truth;
- timing remains deterministic;
- realtime work remains bounded;
- eight logical tracks are never silently reduced;
- new engine families require explicit CPU/RAM/parameter/license review;
- architecture decisions precede implementation;
- physical hardware claims require physical evidence.

## Documentation rule

Roadmap order is not project memory.

When a work package changes product behavior, architecture, UX assumptions, resource constraints or future direction, the relevant context documents must be updated at the same checkpoint as code/test state. A new chat should be able to reconstruct both **what FM1Tracker is** and **what happens next** from the repository alone.
