# WP4.3 NOISE Oscillator Contract

Status: **FROZEN BY D039 BEFORE NOISE PRODUCTION SOURCE**
Date: 2026-10-24
Package: WP4.3 / issue #33
Phase 4 branch: `phase4/8bit-engine`
Parent architecture: D037 / `WP4_1_ENGINE_CONTRACT.md`
Pitch baseline: D038 / `WP4_2_PULSE_CONTRACT.md`

This document is the normative contract for the second isolated Phase 4 oscillator primitive. It freezes NOISE state, seed, LFSR algorithm, pitch-to-clock mapping, render ordering, deterministic evidence and realtime bounds before `fmt_osc_noise.*` production source is created.

WP4.3 does **not** connect NOISE to `dinghy.c`. PULSE also remains isolated. The running full app stays on the Phase 3 proof sine and must remain byte-identical through WP4.3 closeout.

## 1. Scope and identity

D037 engine identity remains authoritative:

```text
mode 1 / engine 2 = NOISE
```

WP4.3 introduces exactly one new isolated primitive:

```text
fmt_osc_noise.*
```

It reuses `fmt_engine_pitch_inc_q7()` from D038. It does not create another tracker pitch representation or another pitch lookup table.

No TRIANGLE, WAVETABLE-8, envelope, final allocator, engine parameter page/cache, p-lock engine mapping or proof-sine migration belongs to WP4.3.

## 2. Exact state

The isolated NOISE state is exactly:

```c
typedef struct {
    uint32_t phase;
    uint32_t lfsr;
} FmtNoiseOsc;
```

Contract:

- `sizeof(FmtNoiseOsc) == 8`;
- `phase` is a wrapping unsigned 32-bit noise-clock accumulator;
- only the low 15 bits of `lfsr` are part of the LFSR state;
- no hidden/static mutable oscillator state;
- no pointer ownership;
- no embedded pitch, gain, envelope or tracker state;
- reset and retrigger set `phase = 0` and `lfsr = 0x5A3D`.

Eight future NOISE oscillator instances therefore require exactly 64 bytes of oscillator-specific mutable state before common voice/envelope/allocator fields.

## 3. Seed choice

The frozen reset seed is:

```text
FMT_NOISE_SEED = 0x5A3D
```

The seed is nonzero and lies on the same maximal 15-bit LFSR cycle as every other valid nonzero state. Changing the seed changes only the starting point of that cycle, not its period.

`0x5A3D` is chosen deliberately because its early output bits are mixed rather than beginning with the long constant run produced by seeds such as all-zero-like or all-one patterns. This makes deterministic retrigger less likely to begin with an obvious constant plateau while preserving the same long-cycle spectrum.

## 4. Exact 15-bit LFSR

The LFSR uses this exact right-shift recurrence:

```text
state = state & 0x7FFF
feedback = (bit0 XOR bit1)
next = (state >> 1) | (feedback << 14)
```

Equivalent C arithmetic:

```c
feedback = (state ^ (state >> 1)) & 1u;
next = (state >> 1) | (feedback << 14);
```

From every valid nonzero 15-bit state this recurrence has period exactly **32,767** LFSR steps before repeating.

The all-zero state is not part of the valid oscillator state cycle. Normal reset/retrigger and the frozen recurrence never enter zero.

WP4.3 defines only this long-period mode. It does not add a second short/metallic tap mode. Any future alternate tap mode changes musical identity and deterministic vectors and therefore requires an explicit later decision rather than a silent refactor.

## 5. Output mapping

NOISE produces one signed mono nominal-Q15 sample per render call.

The current LFSR bit is sampled before any state advance:

```text
if (lfsr & 1) sample = +32767
else          sample = -32767
```

Output is therefore exactly one of:

```text
+32767
-32767
```

There is no zero output state, interpolation, smoothing, filter or amplitude shaping inside the oscillator primitive. Envelope/level remains a later common layer under D037.

Reset seed `0x5A3D` has bit0 set, so the first sample after reset/retrigger is always `+32767`.

## 6. Q7 pitch to NOISE clock

NOISE receives the same resolved D037 Q7 pitch as every other Phase 4 engine.

The caller first derives the existing D038 32-bit pitch phase increment:

```c
pitch_inc = fmt_engine_pitch_inc_q7(pitch_q7);
```

NOISE then derives its own clock increment with a fixed +4-octave clock multiplier:

```text
noise_clock_inc = saturating_unsigned(pitch_inc * 16)
```

Canonical helper:

```c
uint32_t fmt_noise_clock_inc(uint32_t pitch_phase_inc);
```

Exact integer implementation rule:

```text
if pitch_phase_inc > (UINT32_MAX >> 4):
    return UINT32_MAX
else:
    return pitch_phase_inc << 4
```

There is no multiplication overflow and no wrap back to a lower noise clock at high pitch.

The ×16 mapping is intentional. NOISE pitch controls a state-update clock rather than a tonal oscillator fundamental. At 44.1 kHz it gives approximately:

- MIDI 0: 130.813 Hz noise clock;
- C4 / MIDI 60: 4,186.009 Hz;
- A4 / MIDI 69: 7,040.000 Hz;
- sufficiently high pitches: saturate toward one LFSR update per audio frame.

This gives low-note coarse/periodic digital texture, useful mid-register chip-noise rates and high-register hiss while retaining one shared tracker pitch domain.

Frozen clock vectors:

| Q7 pitch | D038 `pitch_inc` | D039 `noise_clock_inc` |
|---:|---:|---:|
| 0 | 796253 | 12740048 |
| 7680 / C4 | 25480118 | 407681888 |
| 8832 / A4 | 42852281 | 685636496 |
| 8896 / A4+0.5 | 44126346 | 706021536 |
| 16256 / MIDI127 | 1221665363 | 4294967295 |
| 16383 / max Q7 | 1293741834 | 4294967295 |

The clock helper runs when pitch changes, not as an expensive conversion inside each sample render.

## 7. Exact render ordering

Canonical render behavior is:

```text
sample = current LFSR bit0 mapped to +/-32767
next_phase = phase + noise_clock_inc      // uint32 wrap
if next_phase < phase:
    lfsr = one exact D039 LFSR step
phase = next_phase
return sample
```

The **current LFSR state is sampled before phase and LFSR advance**.

Consequences:

- reset/retrigger always emits the seed's positive first sample;
- zero clock increment holds phase, LFSR and output indefinitely;
- one render performs **zero or one** LFSR step, never more;
- unsigned phase overflow is the only NOISE clock event;
- no loop is needed to catch up multiple state advances;
- even `UINT32_MAX` clock increment still performs at most one LFSR step per rendered frame;
- the maximum NOISE state-work bound is independent of pitch.

This one-step-per-frame ceiling is more important than representing a hypothetical update rate above sample rate. High requested clocks intentionally saturate to the audio-frame ceiling.

## 8. Frozen short vectors

The first eight LFSR states beginning at reset seed are:

```text
5A3D, 6D1E, 768F, 3B47, 1DA3, 0ED1, 4768, 23B4
```

Their bit0 output signs are:

```text
+, -, +, +, +, +, -, -
```

With reset phase zero and `noise_clock_inc = 0x80000000`, each LFSR bit is held for exactly two samples. The frozen first sixteen output signs are:

```text
++--++++++++----
```

After those sixteen renders:

```text
phase = 0x00000000
lfsr = 0x11DA
```

With `noise_clock_inc = 0`, the first sample remains `+32767` forever and neither state word changes.

## 9. API boundary

The intended production API is:

```c
void fmt_noise_reset(FmtNoiseOsc *osc);
void fmt_noise_retrigger(FmtNoiseOsc *osc);
uint32_t fmt_noise_clock_inc(uint32_t pitch_phase_inc);
int32_t fmt_noise_render(FmtNoiseOsc *osc, uint32_t noise_clock_inc);
```

Null-state behavior is fail-safe:

- reset/retrigger with `NULL` is a no-op;
- render with `NULL` returns 0 and mutates nothing.

No API receives `FmtSong`, `FmtStep`, lock records, effect state, runtime-queue state, UI state or persistence state.

## 10. Realtime bounds

One NOISE render is O(1) and contains only:

- one LFSR output-bit test;
- one 32-bit phase addition;
- one unsigned-wrap comparison;
- zero or one fixed LFSR step;
- fixed state stores.

The render function contains no loop, divide, modulo, allocation, lock traversal, table search, screen access, persistence, blocking, recursion, floating point or 64-bit arithmetic.

`fmt_noise_clock_inc()` contains one comparison and either one left shift or one constant return. It is not a per-sample requirement.

## 11. Spectral and aliasing policy

D039 defines intentional **clocked chip noise**, not mathematically white full-band noise.

The LFSR bit is sample-and-held between noise-clock wraps. Therefore lower pitches intentionally produce obvious stepped/periodic digital texture and higher pitches produce increasingly dense hiss.

No interpolation, whitening stage, BLEP, oversampling, dithering, FIR/IIR filtering or hidden random source is part of WP4.3.

The 32,767-step periodic sequence and sample-rate/clock aliasing are accepted parts of the Phase 4 8-BIT identity. A future alternate noise spectrum or tap mode must be an explicit superseding decision with new deterministic evidence.

## 12. Resource budget and isolation

WP4.3 remains isolated and `fmt_osc_noise.*` is not included by `dinghy.c`.

Required full-app delta at WP4.3 closeout:

| Resource | Required delta versus WP4.2 full app |
|---|---:|
| app image | **0 B** |
| normal RAM | **0 B** |
| pool RAM | **0 B** |
| default WAV | **byte-identical** |
| BIN/FWSC | **byte-identical** |

Isolated primitive cost/bounds:

- `FmtNoiseOsc`: exactly 8 B;
- eight future NOISE states: exactly 64 B;
- no immutable waveform table;
- no static mutable RAM;
- no pool storage;
- no dynamic allocation.

The common D038 pitch helper and PULSE source remain isolated as already verified. WP4.3 does not alter their deterministic vectors or object behavior.

## 13. Required deterministic tests

Focused host tests must prove at least:

1. compile-time `FmtNoiseOsc == 8 B`;
2. exact reset/retrigger phase and seed;
3. null reset/retrigger safety;
4. null render returns 0;
5. exact D039 LFSR recurrence and first-state vector;
6. exact maximal period 32,767 steps returning to `0x5A3D` without zero;
7. exact output mapping `bit0 1 -> +32767`, `0 -> -32767`;
8. zero clock increment holds both state words/output;
9. `0x80000000` frozen sixteen-sample vector `++--++++++++----`;
10. unsigned phase wrap causes exactly one LFSR step;
11. no-wrap render causes zero LFSR steps;
12. `UINT32_MAX` clock still advances at most one LFSR state per audio frame;
13. exact frozen clock vectors in Section 6;
14. `fmt_noise_clock_inc()` is monotonic nondecreasing across the full legal Q7 pitch domain and saturates rather than wrapping;
15. identical-state identical-input replay is byte-for-byte deterministic;
16. guard words prove no mutation outside the 8-byte oscillator state;
17. representative long-render hash from a fixed pitch/reset state.

Static/source audit must prove no float/double, runtime 64-bit divide, allocation, unbounded render loop or tracker-store traversal in the new production module.

The pinned real JieLi PI32v2 compiler must independently compile `fmt_osc_noise.c` together with the already-frozen D038 pitch interface available to its tests.

## 14. Frozen long-run signature

The WP4.3 focused test uses:

```text
samples = 65,536
pitch_q7 = 8832 (A4)
pitch_inc = 42,852,281
noise_clock_inc = 685,636,496 / 0x28DDFB90
reset phase = 0
reset lfsr = 0x5A3D
```

FNV-1a32 uses the same style as WP4.2: fold each signed sample as a 32-bit word, then final `phase`, then final `lfsr`.

Frozen result:

```text
final phase = 0xFB900000
final lfsr = 0x5DC1
FNV-1a32 = 0xF50385B6
```

Any change to LFSR taps, seed, clock multiplier, saturation, sample/advance ordering or output mapping changes this signature and therefore requires explicit review.

## 15. Full-app regression obligation

WP4.3 closeout must rebuild the complete host and target app and prove it remains the WP4.2/WP4.1 baseline because NOISE is still isolated:

- default host WAV SHA-256 `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`;
- `fm1tracker.bin` SHA-256 `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27`;
- `fm1tracker-0.1.fwsc` SHA-256 `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`;
- resources 66,960 B image / 13,296 B normal RAM / 113,000 B pool;
- package identity `FM-1_987`.

## 16. Non-goals

WP4.3 does not implement or decide:

- alternate/short-period NOISE tap mode;
- TRIANGLE or WAVETABLE-8 DSP;
- common envelope timing/state;
- voice allocation/stealing;
- engine parameter IDs/pages;
- engine p-lock publication cache;
- D032/D033 adapter integration into an 8-BIT voice;
- multi-track engine rendering;
- replacement of `tracker_voice` / proof sine;
- live performance capture;
- persistence/file format;
- final hardware UX.

## 17. Exit condition

WP4.3 may close only when:

- D039 is present in `DECISIONS.md` and this contract is committed before NOISE production source;
- focused vectors, LFSR-period proof and long-run signature are green;
- the new module compiles with the pinned real JieLi PI32v2 compiler;
- static realtime/source audit is green;
- the complete full app remains byte-identical to the WP4.2/WP4.1 baseline because NOISE is isolated;
- durable `WP4_3_VERIFICATION.md` records exact target/object/full-app evidence;
- no WP4.4+ implementation has been pulled forward.

After that, WP4.4 / TRIANGLE becomes the only active Phase 4 package.
