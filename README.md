# FM1Tracker

A hardware-first tracker/sequencer firmware for the **M-VAVE FM-1**.

FM1Tracker is inspired by classic trackers and by the workflow ideas demonstrated by [Furnace](https://tildearrow.org/furnace/), but it is **not intended to be a port of the full Furnace desktop engine**. The goal is a compact, fixed-point tracker designed around the FM-1 panel: keyboard step entry, a dedicated rest workflow, parameter locks, 64-step patterns, and two curated sound worlds: **8-BIT** and **16-BIT**.

## Core idea

- Play notes directly from the FM-1 keyboard.
- Enter notes into a pattern one step at a time with automatic step advance.
- Insert rests without leaving step-record mode.
- Use the 16 white keys as physical step selectors/pages in sequencer edit mode.
- Hold a step and turn an encoder to create per-step parameter locks.
- Keep tracker power — note, instrument, volume, effects and locks — without requiring the user to memorize hexadecimal commands.
- Build a small, deterministic engine specifically for the FM-1 instead of trying to embed the entire Furnace application.

## Planned sound modes

### 8-BIT

A deliberately small chip-style palette built from inexpensive fixed-point synthesis blocks. Initial candidates are pulse/PWM, wavetable, triangle and noise. Authentic machine/chip profiles may be explored later, but are not a V0.1 requirement.

### 16-BIT

A broader early-digital palette. Initial candidates are fixed-point FM, wavetable and compact PCM/sample playback. The name describes the musical/sonic era and feature set, not a promise that every internal signal path is literally 16-bit.

## Platform

The intended firmware base is [Dinghy](https://github.com/zvenson/dinghy), which provides the FM-1 update/rescue path, USB MIDI/audio, display, panel input, settings, host-mode testing and build-time hardware safety checks. Dinghy is GPL-3.0-only and is based on Felucca/SLOOP.

FM1Tracker will follow the FM-1 platform constraints from the beginning:

- fixed-point DSP; no floating point in firmware
- no 64-bit division in runtime firmware code
- bounded audio ISR work
- no screen/flash work in the audio ISR
- host-mode tests before hardware testing where practical
- explicit RAM/pool/flash budgets

## Furnace relationship

Furnace is an important design and interoperability reference. Most of Furnace is GPL-2.0-or-later, while some bundled components/emulation cores have their own licenses. FM1Tracker therefore does **not** assume that a Furnace component is reusable merely because it exists in the Furnace tree. Any code or emulation core considered for inclusion must receive an explicit license and resource audit first.

Longer term, interoperability through a documented export/subset format is preferable to embedding the full Furnace engine.

## Project status

**Phase 0 — architecture and control model.**

No production firmware has been committed yet. The next milestone is to freeze the V0.1 data model, control map and resource budgets, then import/rename a clean Dinghy baseline before implementing the sequencer.

## Design priorities

1. Fast musical entry on the physical hardware.
2. Step recording from the keyboard must feel immediate.
3. Parameter locks are a first-class feature, not an afterthought.
4. Tracker concepts should be visible and inspectable without making the UI cryptic.
5. Audio timing and memory limits win over feature count.
6. 8-BIT and 16-BIT share one sequencer and song model.
7. Legal provenance for every imported source file or emulation core must remain clear.

## References

- Furnace: https://tildearrow.org/furnace/
- Furnace source/license: https://github.com/tildearrow/furnace
- Dinghy: https://github.com/zvenson/dinghy
- SLOOP: https://github.com/isod89/sloop-fm1
- Felucca: https://github.com/hugelton/Felucca

## License

The firmware is intended to use the GPL-3.0-only Dinghy/Felucca/SLOOP platform, so FM1Tracker firmware will be distributed as **GPL-3.0-only** when that platform code is imported. Third-party components retain their own notices and compatible licenses. See the licensing documentation added during Phase 0 before adding external code.
