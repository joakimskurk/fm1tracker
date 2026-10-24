# WP4.4 TRIANGLE Oscillator Contract

Status: **FROZEN BY D040 BEFORE TRIANGLE PRODUCTION SOURCE**
Date: 2026-10-24
Package: WP4.4 / issue #34
Phase 4 branch: `phase4/8bit-engine`
Parent architecture: D037 / `WP4_1_ENGINE_CONTRACT.md`
Pitch baseline: D038 / `WP4_2_PULSE_CONTRACT.md`

This document is the normative contract for the third isolated Phase 4 oscillator primitive. It freezes TRIANGLE state, exact 32-bit-phase-to-Q15 mapping, reset/retrigger semantics, deterministic vectors and realtime bounds before `fmt_osc_triangle.*` production source is created.

WP4.4 does **not** connect TRIANGLE to `dinghy.c`. PULSE and NOISE remain isolated. The running full app stays on the Phase 3 proof sine and must remain byte-identical through WP4.4 closeout.

## 1. Scope and identity

D037 engine identity remains authoritative:

```text
mode 1 / engine 3 = TRIANGLE
```

WP4.4 introduces exactly one new isolated primitive:

```text
fmt_osc_triangle.*
```

TRIANGLE reuses the canonical D038 `fmt_engine_pitch_inc_q7()` phase increment directly. It does not create another pitch lookup table, sample-rate representation or tracker-pitch domain.

No WAVETABLE-8, envelope, final allocator, engine parameter page/cache, p-lock engine mapping or proof-sine migration belongs to WP4.4.

## 2. Exact state

The isolated TRIANGLE state is exactly:

```c
typedef struct {
    uint32_t phase;
} FmtTriangleOsc;
```

Contract:

- `sizeof(FmtTriangleOsc) == 4`;
- `phase` is the common wrapping unsigned 32-bit D037 oscillator phase;
- no hidden/static mutable oscillator state;
- no pointer ownership;
- no embedded pitch, gain, envelope or tracker state;
- reset and retrigger set `phase = 0`.

Eight future TRIANGLE oscillator instances therefore require exactly 32 bytes of oscillator-specific mutable state before common voice/envelope/allocator fields.

## 3. Exact phase-to-sample mapping

TRIANGLE produces the complete D037 nominal-Q15 range without ever emitting `-32768`.

For current unsigned 32-bit phase:

```text
quadrant = phase >> 30                    // 0..3
offset   = (phase >> 15) & 0x7FFF        // 0..32767
```

The exact output mapping is:

```text
quadrant 0: sample =  offset
quadrant 1: sample =  32767 - offset
quadrant 2: sample = -offset
quadrant 3: sample = -32767 + offset
```

This gives the canonical phase points:

| phase | cycle position | sample |
|---:|---:|---:|
| `0x00000000` | 0 | `0` |
| `0x40000000` | 1/4 | `+32767` |
| `0x80000000` | 1/2 | `0` |
| `0xC0000000` | 3/4 | `-32767` |
| wrap to `0x00000000` | 1 | `0` |

The mapping has exact half-cycle antisymmetry:

```text
sample(phase + 0x80000000 mod 2^32) = -sample(phase)
```

and stays inside:

```text
-32767..32767
```

## 4. Quantization and endpoint policy

D040 intentionally uses 15 phase-derived bits per rising/falling quarter-slope.

Each quarter contains exactly the integer amplitude levels needed for the D037 Q15 range. This avoids multiplication, division and `-32768` while preserving exact zero crossings and exact positive/negative peaks.

Because phase has much higher resolution than output amplitude, adjacent phase values necessarily map to the same Q15 sample. At each peak/trough boundary the exact extrema are held across the quantization cells on both sides of the quadrant transition. This is deterministic quantization, not an extra oscillator state or plateau parameter.

Frozen boundary examples:

```text
phase 0x3FFF8000 -> +32767
phase 0x40000000 -> +32767
phase 0x7FFF8000 -> 0
phase 0x80000000 -> 0
phase 0xBFFF8000 -> -32767
phase 0xC0000000 -> -32767
phase 0xFFFF8000 -> 0
phase 0xFFFFFFFF -> 0
```

No interpolation or fractional amplitude beyond this integer mapping is allowed in WP4.4.

## 5. Exact render ordering

Canonical render behavior is:

```text
sample = triangle(current_phase)
phase = current_phase + phase_increment    // uint32 wrap
return sample
```

The **current phase is sampled before phase advance**, matching D038 PULSE and D039 NOISE ordering.

Consequences:

- reset/retrigger always produces a first sample of exactly `0`;
- the first post-reset motion for a positive pitch increment is on the rising slope;
- `phase_increment == 0` holds both phase and output indefinitely;
- unsigned 32-bit addition is the only phase-wrap mechanism;
- there is no special branch/event at wrap;
- no oscillator gain or envelope is applied inside TRIANGLE.

## 6. Frozen short vectors

From reset with quarter-cycle increment `0x40000000`:

```text
samples: 0, +32767, 0, -32767, repeat
phases after render: 0x40000000, 0x80000000, 0xC0000000, 0x00000000
```

From reset with eighth-cycle increment `0x20000000`:

```text
samples: 0, +16384, +32767, +16383, 0, -16384, -32767, -16383, repeat
```

The apparent one-count difference around the peaks is frozen integer quantization from the exact quadrant mapping; it must not be "corrected" by hidden rounding or smoothing.

## 7. API boundary

The intended production API is:

```c
void fmt_triangle_reset(FmtTriangleOsc *osc);
void fmt_triangle_retrigger(FmtTriangleOsc *osc);
int32_t fmt_triangle_render(FmtTriangleOsc *osc,
                            uint32_t phase_increment);
```

Null-state behavior is fail-safe:

- reset/retrigger with `NULL` is a no-op;
- render with `NULL` returns 0 and mutates nothing.

The primitive receives only an already-resolved D038 phase increment. It does not inspect `FmtSong`, `FmtStep`, D028 locks, D031/D032 caches, D033 event state, D034 queue state, UI or persistence state.

## 8. Pitch ownership

TRIANGLE has no engine-specific pitch helper.

A future integrated voice resolves D037 Q7 pitch and computes:

```c
phase_increment = fmt_engine_pitch_inc_q7(pitch_q7);
```

only when pitch changes. `fmt_triangle_render()` receives that increment directly.

Therefore all frozen D038 pitch vectors remain authoritative without duplication or reinterpretation.

## 9. Realtime bounds

One TRIANGLE render is O(1) and contains only bounded integer work:

- one 32-bit phase read;
- two shifts/masks to derive quadrant/offset;
- a fixed bounded quadrant selection;
- one signed add/subtract to form Q15 output;
- one unsigned 32-bit phase addition;
- one state store.

There is no loop, divide, modulo, lookup-table search, allocation, lock traversal, screen access, persistence, blocking, recursion, floating point or 64-bit arithmetic in the render function.

Pitch conversion remains D038 work performed only when pitch changes.

## 10. Spectral / aliasing policy

D040 defines a direct piecewise-linear integer triangle with sharp slope discontinuities at the peaks/troughs.

WP4.4 adds no BLEP/polyBLEP/minBLEP, oversampling, FIR/IIR smoothing, interpolation or hidden post-filter.

A triangle naturally contains substantially less high-frequency energy than PULSE, but its corners still have a finite harmonic series and can alias in the upper register. That is accepted Phase 4 8-BIT behavior. Any later anti-aliasing changes state/CPU/sample vectors and requires an explicit superseding decision.

## 11. Resource budget and isolation

WP4.4 remains isolated and `fmt_osc_triangle.*` is not included by `dinghy.c`.

Required full-app delta at WP4.4 closeout:

| Resource | Required delta versus WP4.3 full app |
|---|---:|
| app image | **0 B** |
| normal RAM | **0 B** |
| pool RAM | **0 B** |
| default WAV | **byte-identical** |
| BIN/FWSC | **byte-identical** |

Isolated primitive cost/bounds:

- `FmtTriangleOsc`: exactly 4 B;
- eight future TRIANGLE phase states: exactly 32 B;
- no waveform table;
- no static mutable RAM;
- no pool storage;
- no dynamic allocation.

D038 PULSE and D039 NOISE remain untouched and isolated.

## 12. Required deterministic tests

Focused host tests must prove at least:

1. compile-time `FmtTriangleOsc == 4 B`;
2. reset/retrigger phase zero;
3. null reset/retrigger safety;
4. null render returns 0;
5. exact canonical phase points `0 / +32767 / 0 / -32767`;
6. exact boundary examples in Section 4;
7. exact quarter-cycle reference vector;
8. exact eighth-cycle reference vector;
9. exact half-cycle antisymmetry over representative and exhaustive high-16 phase values;
10. output never leaves `-32767..32767` over all 65,536 high-16 phase buckets;
11. zero increment holds phase/output;
12. unsigned phase wrap preserves mapping/order;
13. identical-state identical-input replay is deterministic;
14. guard words prove no mutation outside the 4-byte oscillator state;
15. all D038 frozen pitch vectors remain unchanged when used as TRIANGLE phase increments;
16. representative long-render hash from reset at fixed pitch.

Static/source audit must prove no float/double, runtime 64-bit divide, allocation, unbounded render loop or tracker-store traversal in the new production module.

The pinned real JieLi PI32v2 compiler must independently compile `fmt_osc_triangle.c` while the already-frozen D038 pitch object remains byte-identical.

## 13. Frozen long-run signature

The WP4.4 focused test uses:

```text
samples = 65,536
pitch_q7 = 8832 (A4)
phase_increment = 42,852,281 / 0x028DDFB9
reset phase = 0
```

FNV-1a32 uses the same Phase 4 style: fold each signed sample as a 32-bit word, then fold final phase.

Frozen result:

```text
final phase = 0xDFB90000
FNV-1a32 = 0xBC0F75ED
```

Any change to phase mapping, quantization, reset phase, sample/advance ordering or pitch integers changes this signature and requires explicit review.

## 14. Full-app regression obligation

WP4.4 closeout must rebuild the complete host and target app and prove it remains the WP4.3/WP4.2 baseline because TRIANGLE is still isolated:

- default host WAV SHA-256 `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`;
- `fm1tracker.bin` SHA-256 `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27`;
- `fm1tracker-0.1.fwsc` SHA-256 `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`;
- resources 66,960 B image / 13,296 B normal RAM / 113,000 B pool;
- package identity `FM-1_987`.

## 15. Non-goals

WP4.4 does not implement or decide:

- WAVETABLE-8 DSP;
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

## 16. Exit condition

WP4.4 may close only when:

- D040 is present in `DECISIONS.md` and this contract is committed before TRIANGLE production source;
- focused endpoint/symmetry/vector/hash tests are green;
- the new module compiles with the pinned real JieLi PI32v2 compiler;
- static realtime/source audit is green;
- the complete full app remains byte-identical to the WP4.3/WP4.2 baseline because TRIANGLE is isolated;
- durable `WP4_4_VERIFICATION.md` records exact target/object/full-app evidence;
- no WP4.5+ implementation has been pulled forward.

After that, WP4.5 / WAVETABLE-8 becomes the only active Phase 4 package.
