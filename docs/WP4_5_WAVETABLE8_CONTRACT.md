# WP4.5 WAVETABLE-8 Oscillator Contract

Status: **FROZEN BY D041 BEFORE WAVETABLE-8 PRODUCTION SOURCE**
Date: 2026-10-24
Package: WP4.5 / issue #35
Phase 4 branch: `phase4/8bit-engine`
Parent architecture: D037 / `WP4_1_ENGINE_CONTRACT.md`
Pitch baseline: D038 / `WP4_2_PULSE_CONTRACT.md`

This document is the normative contract for the fourth isolated Phase 4 oscillator primitive. It freezes the concrete meaning of WAVETABLE-8, immutable table contents, phase/index/sample mapping, malformed selector behavior, deterministic evidence and realtime/image bounds before `fmt_osc_wavetable8.*` production source is created.

WP4.5 does **not** connect WAVETABLE-8 to `dinghy.c`. PULSE, NOISE and TRIANGLE remain isolated. The running full app stays on the Phase 3 proof sine and must remain byte-identical through WP4.5 closeout.

## 1. Scope and identity

D041 defines WAVETABLE-8 concretely as:

```text
8 curated immutable wave tables
32 samples per table
signed 8-bit sample storage
nearest/zero-order-hold table lookup
```

The Phase 4 palette ordering assigns WAVETABLE-8 the fourth 8-BIT engine identity after PULSE, NOISE and TRIANGLE.

WP4.5 introduces exactly one new isolated primitive:

```text
fmt_osc_wavetable8.*
```

It reuses the canonical D038 `fmt_engine_pitch_inc_q7()` phase increment directly. It does not create another pitch lookup table or tracker-pitch domain.

No editable/user wavetable storage, morphing, interpolation, envelope, final allocator, parameter page/cache, p-lock engine mapping or proof-sine migration belongs to WP4.5.

## 2. Exact oscillator state

The isolated oscillator state is exactly:

```c
typedef struct {
    uint32_t phase;
} FmtWavetable8Osc;
```

Contract:

- `sizeof(FmtWavetable8Osc) == 4`;
- reset and retrigger set `phase = 0`;
- no table selector is stored in oscillator state;
- no hidden/static mutable oscillator state;
- no pointer ownership;
- no embedded pitch, gain, envelope or tracker state.

Eight future WAVETABLE-8 oscillator instances therefore require exactly 32 bytes of oscillator-specific mutable state before common voice/envelope/allocator fields.

## 3. Table dimensions and selector domain

Frozen constants:

```text
FMT_WAVETABLE8_TABLE_COUNT  = 8
FMT_WAVETABLE8_TABLE_LENGTH = 32
```

Frozen table identities:

| ID | Name | Character |
|---:|---|---|
| 0 | SINE | smooth fundamental |
| 1 | SAW8 | harmonic ramp / saw-like |
| 2 | SQUARE8 | harmonic square-like |
| 3 | ORGAN | octave-rich organ-like |
| 4 | HOLLOW | odd/hollow resonant |
| 5 | NASAL | dense low-harmonic formant-like |
| 6 | METAL | high odd-harmonic metallic |
| 7 | DIGI | phase-bent digital |

A selector `0..7` selects that exact table.

Any malformed selector `>=8` falls back to table **0 / SINE**. There is no modulo/wrap selection because an invalid future parameter value must fail to a stable known timbre rather than silently choosing a different table.

WP4.8 will later assign the stable engine parameter/page mapping for table selection. D041 freezes only the oscillator-domain meaning.

## 4. Exact immutable signed-8-bit table data

The production source contains exactly these row-major `int8_t[8][32]` values:

```text
SINE:
  0, 25, 49, 71, 90, 106, 117, 125,
  127, 125, 117, 106, 90, 71, 49, 25,
  0, -25, -49, -71, -90, -106, -117, -125,
  -127, -125, -117, -106, -90, -71, -49, -25

SAW8:
  0, 3, 16, 27, 28, 33, 48, 57,
  56, 64, 83, 87, 79, 96, 127, 103,
  0, -103, -127, -96, -79, -87, -83, -64,
  -56, -57, -48, -33, -28, -27, -16, -3

SQUARE8:
  0, 127, 97, 115, 102, 113, 103, 112,
  103, 112, 103, 113, 102, 115, 97, 127,
  0, -127, -97, -115, -102, -113, -103, -112,
  -103, -112, -103, -113, -102, -115, -97, -127

ORGAN:
  0, 75, 103, 107, 123, 127, 101, 81,
  98, 110, 80, 35, 15, 1, -28, -36,
  0, 36, 28, -1, -15, -35, -80, -110,
  -98, -81, -101, -127, -123, -107, -103, -75

HOLLOW:
  0, 43, 73, 97, 121, 127, 90, 27,
  -4, 27, 90, 127, 121, 97, 73, 43,
  0, -43, -73, -97, -121, -127, -90, -27,
  4, -27, -90, -127, -121, -97, -73, -43

NASAL:
  0, 78, 123, 127, 108, 87, 70, 52,
  34, 24, 29, 38, 35, 15, -8, -14,
  0, 14, 8, -15, -35, -38, -29, -24,
  -34, -52, -70, -87, -108, -127, -123, -78

METAL:
  0, 127, 79, 26, 3, 10, 102, 118,
  74, 118, 102, 10, 3, 26, 79, 127,
  0, -127, -79, -26, -3, -10, -102, -118,
  -74, -118, -102, -10, -3, -26, -79, -127

DIGI:
  0, 99, 124, 122, 123, 126, 117, 84,
  72, 117, 127, 86, 50, 1, -53, -58,
  0, 58, 53, -1, -50, -86, -127, -117,
  -72, -84, -117, -126, -123, -122, -124, -99
```

No production-time generation is permitted. These 256 bytes are the source of truth.

The row-major signed-byte blob SHA-256 is frozen as:

```text
f95b092a5106a8b05acb57d2960b0162b48ea42e2f8217b8923b0b7346046aee
```

All values lie in `-127..127`; `-128` is deliberately unused.

## 5. 8-bit sample to nominal-Q15 mapping

One signed table byte is expanded with the exact integer rule:

```text
sample_q15 = raw_int8 * 258
```

Therefore:

```text
-127 -> -32766
   0 ->      0
+127 -> +32766
```

This stays strictly inside the D037 nominal-Q15 range without a clamp, division or asymmetric `-128` case. The one-count headroom relative to `±32767` is intentional and negligible; changing the expansion factor changes every deterministic audio vector and therefore requires a superseding decision.

## 6. Exact phase-to-index mapping

WAVETABLE-8 uses the top five bits of the common 32-bit phase accumulator:

```text
index = phase >> 27       // 0..31
```

There is **no interpolation** between adjacent table samples in WP4.5.

Each table sample therefore spans exactly `2^27` phase units. This explicit zero-order-hold behavior is part of the 8-BIT sound identity and makes render work and table access fully bounded.

A later interpolation/morphing feature would change CPU cost and sound and must not be slipped into an unrelated refactor.

## 7. Exact render ordering

Canonical behavior:

```text
table = selector if selector < 8 else 0
index = current_phase >> 27
raw = tables[table][index]
sample = raw * 258
phase = current_phase + phase_increment       // uint32 wrap
return sample
```

The **current phase is sampled before phase advance**, matching PULSE, NOISE and TRIANGLE.

Consequences:

- reset/retrigger phase zero always reads table index 0;
- every frozen table has sample 0 at index 0, so first sample after reset/retrigger is exactly `0`;
- `phase_increment == 0` holds phase/index/output indefinitely;
- unsigned 32-bit addition is the only wrap mechanism;
- there is no special branch/event at cycle wrap;
- no oscillator gain or envelope is applied inside WAVETABLE-8.

## 8. Exact sequential table vectors

With reset phase zero and increment `0x08000000`, each render advances exactly one table index. The first 32 rendered raw values must therefore equal the selected row in Section 4 in exact order, followed by wrap to index 0.

Focused tests must run this sequential sweep for **all eight tables**, so every one of the 256 immutable samples participates in deterministic behavior evidence.

Malformed table selectors such as `8`, `9`, `255` and `UINT32_MAX` must produce the exact same output/state evolution as selector `0`.

## 9. API boundary

The intended production API is:

```c
void fmt_wavetable8_reset(FmtWavetable8Osc *osc);
void fmt_wavetable8_retrigger(FmtWavetable8Osc *osc);
uint8_t fmt_wavetable8_sanitize_table(uint32_t table);
int32_t fmt_wavetable8_render(FmtWavetable8Osc *osc,
                              uint32_t phase_increment,
                              uint32_t table);
```

Null-state behavior is fail-safe:

- reset/retrigger with `NULL` is a no-op;
- render with `NULL` returns 0 and mutates nothing.

The primitive receives only an already-resolved D038 phase increment plus table selector. It does not inspect `FmtSong`, `FmtStep`, locks, effects, runtime queue, UI or persistence state.

## 10. Pitch ownership

WAVETABLE-8 has no engine-specific pitch helper.

A future integrated voice resolves D037 Q7 pitch and computes:

```c
phase_increment = fmt_engine_pitch_inc_q7(pitch_q7);
```

only when pitch changes. `fmt_wavetable8_render()` receives that increment directly.

All D038 frozen pitch vectors remain authoritative and unchanged.

## 11. Realtime bounds

One WAVETABLE-8 render is O(1) and contains only:

- one table-selector comparison/fallback;
- one phase shift to obtain 5-bit index;
- one bounded immutable table lookup;
- one signed constant multiply by 258;
- one unsigned 32-bit phase addition;
- one state store.

There is no loop, divide, modulo, dynamic allocation, lock traversal, table search, screen access, persistence, blocking, recursion, floating point or 64-bit arithmetic in the render function.

## 12. Spectral / aliasing policy

D041 intentionally defines a **32-sample zero-order-hold wavetable oscillator**.

The low table resolution and hard sample transitions are part of the Phase 4 8-BIT identity. WP4.5 adds no linear interpolation, cubic interpolation, BLEP, oversampling, FIR/IIR smoothing or hidden post-filter.

Aliasing and imaging in upper registers are therefore accepted properties, just as hard-edge PULSE and clocked NOISE have intentional digital spectra. Any later interpolation or anti-aliasing strategy requires an explicit superseding decision and new deterministic evidence.

## 13. Immutable-data and state budget

Frozen isolated costs:

```text
8 tables * 32 samples * 1 byte = 256 B immutable table data
FmtWavetable8Osc = 4 B mutable state per oscillator
8 future oscillator states = 32 B mutable state
```

There is:

- no static mutable table RAM;
- no pool storage;
- no runtime table generation;
- no dynamic allocation;
- no user-editable table memory in WP4.5.

Because the module remains isolated and unlinked, required **full-app** image/RAM/pool delta at WP4.5 closeout is still exactly zero versus WP4.4.

## 14. Required deterministic tests

Focused host tests must prove at least:

1. compile-time `FmtWavetable8Osc == 4 B`;
2. exact table count 8 and length 32;
3. reset/retrigger phase zero and first output zero for every table;
4. null reset/retrigger safety and null render returns 0;
5. selector `0..7` identity and every selector `>=8` fallback to table 0;
6. exact all-256-sample sequential sweeps from Section 4;
7. exact raw-to-Q15 expansion `raw * 258` including `±127 -> ±32766`;
8. no frozen table contains `-128`;
9. row-major table-data SHA-256 matches Section 4 source-of-truth value in a verification helper/script;
10. zero increment holds phase/index/output;
11. unsigned wrap returns from index 31 to index 0 without a special event;
12. all D038 pitch vectors remain unchanged;
13. identical-state identical-input replay is deterministic;
14. guard words prove no mutation outside the 4-byte oscillator state;
15. long-run deterministic FNV signatures for all eight tables.

Static/source audit must prove no float/double, runtime 64-bit divide, allocation, render loop or tracker-store traversal in the new production module.

The pinned real JieLi PI32v2 compiler must independently compile `fmt_osc_wavetable8.c` while the frozen D038 pitch object remains byte-identical.

## 15. Frozen long-run signatures

The long-run test uses for every table:

```text
samples = 65,536
pitch_q7 = 8832 (A4)
phase_increment = 42,852,281 / 0x028DDFB9
reset phase = 0
Q15 expansion = raw * 258
```

FNV-1a32 folds each signed Q15 sample as a 32-bit word, then final phase.

The final phase is always:

```text
0xDFB90000
```

Frozen per-table FNV-1a32 values:

| ID | Table | FNV-1a32 |
|---:|---|---:|
| 0 | SINE | `40a7ddab` |
| 1 | SAW8 | `7c509a67` |
| 2 | SQUARE8 | `a573d449` |
| 3 | ORGAN | `1d6f9fcb` |
| 4 | HOLLOW | `b8048a69` |
| 5 | NASAL | `a3661307` |
| 6 | METAL | `27204689` |
| 7 | DIGI | `48f2989f` |

Any change to table bytes, expansion, selector fallback, phase/index mapping, interpolation policy, reset phase or sample/advance ordering changes this evidence and requires explicit review.

## 16. Full-app regression obligation

WP4.5 closeout must rebuild the complete host and target app and prove it remains the WP4.4/WP4.3 baseline because WAVETABLE-8 is still isolated:

- default host WAV SHA-256 `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`;
- `fm1tracker.bin` SHA-256 `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27`;
- `fm1tracker-0.1.fwsc` SHA-256 `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`;
- resources 66,960 B image / 13,296 B normal RAM / 113,000 B pool;
- package identity `FM-1_987`.

## 17. Non-goals

WP4.5 does not implement or decide:

- user-imported/editable wavetable data;
- wavetable morphing/interpolation;
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

## 18. Exit condition

WP4.5 may close only when:

- D041 is present in `DECISIONS.md` and this contract is committed before WAVETABLE-8 production source;
- all 256 table samples, selector fallback, vectors and eight long-run hashes are green;
- the new module compiles with the pinned real JieLi PI32v2 compiler;
- static realtime/source audit is green;
- the complete full app remains byte-identical to the WP4.4/WP4.3 baseline because WAVETABLE-8 is isolated;
- durable `WP4_5_VERIFICATION.md` records exact target/object/full-app evidence;
- no WP4.6+ implementation has been pulled forward.

After that, WP4.6 / common envelope becomes the only active Phase 4 package.
