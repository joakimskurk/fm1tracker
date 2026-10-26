# FM1Tracker Chip Sound Direction

Status: **accepted sonic product direction before future 8-BIT refinement work**  
Date: 2026-10-26

This document records the intended refinement of the current 8-BIT/chip family. It does not authorize implementation outside a bounded audited work package.

## 1. Problem

The existing 8-BIT family successfully proves deterministic PULSE, NOISE, TRIANGLE and WAVETABLE-8 engines, but those primitives alone do not yet provide the intended musical identity.

A bare oscillator plus envelope is not a strong enough factory experience.

## 2. Sonic target

The preferred direction is **SID-inspired rather than SID-clone**.

FM1Tracker should pursue the musical qualities associated with classic SID-style synthesis while keeping its own engine-agnostic architecture and modern performance goals.

Important character targets include:

- strong pulse-width character and movement;
- useful saw/triangle/noise-style chip sources where bounded implementation permits;
- oscillator interactions such as sync/ring-style behavior where musically valuable and affordable;
- expressive envelopes;
- a resonant multimode or similarly characterful filter stage;
- deliberate non-linearity/drive rather than only mathematically clean oscillator output;
- a modern effects path around the chip voice.

The goal is not cycle-accurate C64 compatibility and not the right to claim an authentic SID implementation.

## 3. Modern chip signal path

The intended conceptual signal path is:

```text
CHIP SOURCE / OSCILLATOR
        -> ENVELOPE
        -> CHARACTER FILTER
        -> DRIVE / SATURATION
        -> CHANNEL FX / SENDS
        -> MIXER
        -> GLOBAL FX
```

Exact ordering and DSP require measurement.

The chip family should ultimately feel like a modern instrument built around chip character, rather than an emulator presented without contemporary shaping tools.

## 4. Filter priority

A musically useful filter is a high-priority refinement because it changes the usefulness of the existing oscillators dramatically.

Desired controls include a bounded subset of:

- cutoff;
- resonance;
- low-pass / band-pass / high-pass or a compact multimode selection;
- drive/input level interaction;
- optional envelope/modulation amount when a later modulation contract supports it.

The filter must use fixed-point/integer DSP, bounded state/work and explicit stability/range tests.

Authentic 6581/8580 electrical modelling is not required. If later pursued, it requires a separate provenance/license/CPU/resource audit.

## 5. Effects direction

The chip family should benefit from the same future coherent effects architecture as other engines.

Useful modern effects around chip sounds may include, subject to explicit resource work packages:

- drive/saturation;
- bit-depth/sample-rate style degradation where musically distinct from the source engine;
- chorus/doubling;
- delay;
- reverb;
- track filter/tone;
- performance retrigger/stutter and tracker-native timing effects.

Do not bolt a private unbounded FX engine onto 8-BIT. Channel/global effect ownership should remain coherent with the accepted live-performance FX direction.

## 6. Factory bank consequence

The default/factory bank must demonstrate this refined character immediately.

Target chip patches should include examples such as:

- subby/resonant chip bass;
- PWM bass;
- sharp sync-like lead where supported;
- hollow pulse lead;
- filtered arpeggio/pluck;
- noisy chip percussion;
- resonant sweep/stab;
- intentionally unstable/gritty digital character sounds.

The factory bank is part of product quality, not demo filler.

## 7. Phase 6 consequence

Phase 6 persistence must preserve extensible per-engine payloads so future chip/filter/drive/FX parameters can be added without changing the fundamental project model.

The format must not freeze the current minimal 8-BIT parameter set as the permanent limit of the chip family.

## 8. Non-goals

This direction does not:

- implement an actual MOS 6581/8580 emulator;
- promise cycle accuracy;
- authorize copyrighted ROM/table reuse;
- require all modern FX to run per voice;
- bypass CPU/RAM benchmarks;
- replace the existing tested 8-BIT regression baseline before a migration/compatibility contract exists.

## 9. Product rule

The chip family should sound **characterful before nostalgic**: recognizably chip-derived, immediately musical, and shaped with modern filtering/effects rather than limited to bare retro oscillator primitives.
