# FM1Tracker

A hardware-first tracker/sequencer firmware for the **M-VAVE FM-1**.

FM1Tracker is inspired by classic trackers and workflow ideas demonstrated by Furnace, but it is **not a port of the full Furnace desktop engine**. It is a compact, deterministic, fixed-point instrument built around the FM-1 panel: keyboard STEP REC, REST/TIE/OFF entry, parameter locks, tracker effects, 64-step patterns, eight logical tracks and a growing set of native synthesis engines.

## Product direction

FM1Tracker is **one tracker with freely selectable instrument engines**, not separate 8-BIT and 16-BIT projects.

Each instrument owns an engine family + engine identity. Different tracks may therefore eventually combine chip-style, FM/digital, modern synth and drum engines in the same project, subject to measured FM-1 CPU/RAM/voice budgets.

The current 8-BIT/chip family is the first complete engine family. FM4 is next. The accepted architecture change is frozen in:

- `docs/D047_MIXED_ENGINE_ARCHITECTURE.md`

D047 supersedes the original project-wide 8-BIT/16-BIT restriction for future work, but deliberately does **not** rewrite or invalidate the already-verified Phase 4 implementation.

## Current status - 2026-10-24

- Phase 1 - clean Dinghy baseline: **COMPLETE**
- Phase 2 - tracker core: **COMPLETE**
- Phase 3 - p-locks/effects/runtime ingress: **COMPLETE**
- Phase 4 - first engine family, 8-BIT/chip: **WP4.1-WP4.9 COMPLETE; WP4.10 closeout active**

The linked production tracker currently has:

- 8 logical tracks x 64 steps x 16 patterns;
- exact integer transport;
- keyboard STEP REC;
- REST / TIE / NOTE OFF semantics;
- bounded tracker screen;
- 2,048-record sparse p-lock pool, max four locks per step;
- ARP / slide / portamento / vibrato / retrig / note delay / note cut;
- bounded 32-entry main-to-audio audition queue;
- PULSE, NOISE, TRIANGLE and WAVETABLE-8 engines;
- common integer ADSR;
- deterministic release-tail voice allocation;
- engine-aware four-parameter p-lock pages;
- all eight tracks linked through the real 8-BIT engine.

WP4.9's verified linked target uses:

- 72,504 B image;
- 19,604 B normal RAM;
- 113,000 B pool RAM;
- package identity `FM-1_987`.

WP4.10 / D046 is verification-only Phase 4 closeout. The representative benchmark and structural realtime audit have been implemented, but the first aggregate exit workflow run `40120541899` is red. Phase 4 is therefore **not yet marked complete**. The immediate task is to isolate that aggregate failure, obtain a complete green closeout and write `docs/WP4_10_VERIFICATION.md`.

For exact continuation state, always start with:

1. `START_HERE.md`
2. `docs/PROJECT_STATE.md`
3. `docs/D047_MIXED_ENGINE_ARCHITECTURE.md`

## Engine architecture

The tracker/sequencer remains engine-agnostic. The target relationship is:

```text
track / tracker row
        |
        v
    instrument
        |
        +-- engine family
        +-- engine
        +-- engine parameters/state
```

The current `FmtStep.instrument` indirection and 4-byte `FmtInstrumentHeader` already provide the basic data-model boundary required for this direction.

After Phase 4 closes, Phase 5 begins with **WP5.0 - mixed-engine routing contract + migration** before FM4 production integration. WP5.0 will remove the current song-level project-mode gate while preserving all proven tracker timing, p-lock, effect, queue and 8-BIT behavior.

The planned next engine family is compact fixed-point four-operator FM. Later engine packages may explore modern virtual analog, supersaw/unison, broader wavetable/digital synthesis, drums, PCM or authentic chip cores when resource/licensing evidence supports them.

Architectural support for future engines does not make every engine a V1 requirement.

## Live performance direction

The long-term product identity is:

**tracker depth underneath, immediate instrument workflow on top.**

The accepted future core includes:

- PLAY continuing while REC arms/disarms live capture;
- fast capture-target track switching;
- drum performance/capture;
- metronome/count-in;
- CHANNEL and GLOBAL performance/automation scopes;
- bounded capture of notes, p-locks and effect gestures into editable tracker data;
- mixed engine families across the eight logical tracks;
- measured polyphony without silently reducing tracker track count.

See `docs/LIVE_PERFORMANCE_CAPTURE_VISION.md`.

## Platform

The firmware baseline is pinned from Dinghy/Felucca/SLOOP and targets JieLi AC791N / pi32v2.

Core realtime rules include:

- fixed-point/integer firmware DSP;
- no floating point in firmware audio work;
- no runtime 64-bit division;
- bounded audio callback work;
- no screen/flash work in the audio ISR;
- no heap/dynamic plugin architecture in realtime paths;
- deterministic overflow/failure policies;
- host-mode tests before physical hardware testing where practical;
- explicit RAM/pool/image budgets.

The fixed package/device identity remains:

`FM-1_987`

The verified external AC79 SDK pin remains:

`d179b4484759423312073f5fbb232501aa491047`

## Build

Host verification uses the repository test scripts under `tests/`.

The device build entry point remains:

```sh
./build.sh --release 0.1
```

The official JieLi toolchain and pinned AC79 SDK remain external build inputs and are not vendored into the repository. See `THIRD_PARTY.md` and the verification documents for exact provenance.

## Hardware limitation

No physical FM-1 is currently available for this project.

Host and pinned device-build evidence can prove deterministic behavior, code/resource budgets and structural realtime bounds, but it does **not** prove physical install/boot behavior, sound quality, control ergonomics or actual AC791N callback execution margin.

Those remain explicit later hardware gates rather than inferred claims.

## Furnace relationship

Furnace remains an important design/interoperability reference, not the firmware architecture to transplant. Third-party cores require explicit license, provenance and resource audits before inclusion.

Longer term, documented interchange/conversion is preferred over embedding the full Furnace engine.

## Design priorities

1. Fast musical entry on physical FM-1 hardware.
2. Tracker/sequencer behavior stays deterministic regardless of sound engine.
3. Instruments choose engines; projects are not locked to one sound family.
4. Parameter locks are first-class semantic data.
5. Audio timing and memory limits win over feature count.
6. Eight logical tracks remain stable while polyphony/engine cost is measured independently.
7. New engines must fit a bounded static router rather than creating a desktop-style plugin system.
8. Legal provenance for every imported source/core remains explicit.

## License

FM1Tracker firmware uses the GPL-3.0-only Dinghy/Felucca/SLOOP platform baseline. Third-party components retain their own compatible notices/licenses. See `THIRD_PARTY.md` before adding external code.
