# FM1Tracker

A hardware-first tracker/sequencer firmware for the **M-VAVE FM-1**.

FM1Tracker is inspired by classic trackers and by workflow ideas demonstrated by [Furnace](https://tildearrow.org/furnace/), but it is **not a port of the full Furnace desktop engine**. The goal is a compact, fixed-point tracker designed around the FM-1 panel: keyboard step entry, a dedicated rest workflow, parameter locks, 64-step patterns, and two curated sound worlds: **8-BIT** and **16-BIT**.

## Current status

**Phase 1 — clean Dinghy baseline is complete. Phase 2 has not started yet.**

The repository contains the pinned Dinghy-derived platform baseline plus a verified identity-only FM1Tracker layer:

- visible app name: `FM1TRACKER`;
- fixed FM-1 package identity: `FM-1_987`;
- host artifacts: `build/host/fm1tracker.wav` and `fm1tracker-*.png`;
- device artifacts: `build/fm1tracker.bin` and versioned `fm1tracker-*.fwsc`;
- the host WAV is bit-identical to the pristine Dinghy-derived baseline;
- protected HAL/loader/platform/test-tone implementation remained unchanged through Phase 1;
- the full renamed device build and inherited safety checks pass with the pinned official external JieLi SDK release;
- reference budgets are recorded in `docs/WP1_5_VERIFICATION.md`.

No tracker engine, STEP REC implementation, p-lock implementation, song mode, 8-BIT engine or 16-BIT FM4 engine has been added yet. The next bounded work package is **WP2.1 — compile-time tracker data model**.

For exact continuation state, start with [`START_HERE.md`](START_HERE.md) and [`docs/PROJECT_STATE.md`](docs/PROJECT_STATE.md).

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

A deliberately small chip-style palette built from inexpensive fixed-point synthesis blocks. Initial engines are pulse, noise, triangle and WAVETABLE-8. Authentic machine/chip profiles may be explored later, but are not a V0.1 requirement.

### 16-BIT

A broader early-digital palette centered initially on a compact fixed-point four-operator FM engine. PCM/sample playback is deliberately deferred until the tracker core and both initial synthesis modes are proven.

## Platform

The firmware baseline is pinned from [Dinghy](https://github.com/zvenson/dinghy), which provides the FM-1 update/rescue path, USB MIDI/audio, display, panel input, settings, host-mode testing and build-time hardware safety checks. Dinghy is GPL-3.0-only and is based on Felucca/SLOOP.

FM1Tracker follows the FM-1 platform constraints from the beginning:

- fixed-point DSP; no floating point in firmware
- no runtime 64-bit division
- bounded audio ISR work
- no screen/flash work in the audio ISR
- host-mode tests before hardware testing where practical
- explicit RAM/pool/flash budgets

## Host verification

Prerequisites: a C compiler, Python 3 and Pillow.

```sh
./tests/run_host.sh
```

Current outputs:

```text
build/host/fm1tracker.wav
build/host/fm1tracker-1000.png
build/host/fm1tracker-3000.png
```

The exact verified hashes and CI runs are recorded in:

- `docs/WP1_3_VERIFICATION.md`
- `docs/WP1_4_VERIFICATION.md`
- `docs/WP1_5_VERIFICATION.md`
- `docs/TEST_MATRIX.md`

## Device build

The verified build entry point is:

```sh
./build.sh --release 0.1
```

Required external inputs:

1. JieLi `pi32v2` toolchain:

```sh
tools/get_toolchain.sh
```

2. Exact official JieLi AC79 SDK release, kept outside this repository:

```sh
git clone --depth 1 --branch AC79NN_SDK_V1.2.1_2023-12-13 \
  https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK.git ~/fw-AC79_AIoT_SDK
```

The verified SDK tag resolves to commit:

`d179b4484759423312073f5fbb232501aa491047`

`JIELI_TOOLCHAIN` and `AC79_SDK` may point elsewhere if your copies live in different locations.

The AC79 SDK remains an external build dependency and is **not vendored** into this repository. `THIRD_PARTY.md` and `docs/WP1_5_VERIFICATION.md` record the exact provenance, hashes and verification state.

### Phase 1 reference build budget

The verified clean renamed baseline uses:

- app image/code: 55,088 B of 581,564 B;
- normal RAM (`.data + .bss`): 12,368 B of 98,304 B;
- pool RAM: 30,720 B of 344,064 B.

These are measured build outputs from the renamed device build. The nominal audio callback period is about 5.805 ms for 256 frames at 44.1 kHz, but actual hardware render time has **not** been measured because no physical FM-1 is currently available.

## Furnace relationship

Furnace is an important design and interoperability reference. Most of Furnace is GPL-2.0-or-later, while bundled components/emulation cores can have their own licenses. FM1Tracker therefore does **not** assume that a Furnace component is reusable merely because it exists in the Furnace tree. Any code or emulation core considered for inclusion requires an explicit license and resource audit first.

Longer term, interoperability through a documented export/subset format is preferable to embedding the full Furnace engine.

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

FM1Tracker firmware uses the GPL-3.0-only Dinghy/Felucca/SLOOP platform baseline. Third-party components retain their own notices and compatible licenses. See `THIRD_PARTY.md` and the provenance documents before adding external code.
