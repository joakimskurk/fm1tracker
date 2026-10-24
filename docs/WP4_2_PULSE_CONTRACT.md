# WP4.2 PULSE Oscillator Contract

Status: **FROZEN BY D038 BEFORE PULSE PRODUCTION SOURCE**  
Date: 2026-10-24  
Package: WP4.2 / issue #32  
Phase 4 branch: `phase4/8bit-engine`  
Parent architecture: D037 / `WP4_1_ENGINE_CONTRACT.md`

This document is the normative contract for the first isolated Phase 4 oscillator primitive. It freezes PULSE behavior, pitch conversion, state, deterministic sample order and verification boundaries before `fmt_engine_pitch.*` or `fmt_osc_pulse.*` production source is created.

WP4.2 does **not** replace the current Phase 3 proof sine in `dinghy.c`. The full-app firmware/audio artifact must therefore remain byte-identical until a later explicit integration package changes that migration boundary.

## 1. Scope

WP4.2 introduces exactly two isolated reusable primitives:

```text
fmt_engine_pitch.*   shared Q7 pitch -> 32-bit phase increment
fmt_osc_pulse.*      hard-edge pulse oscillator
```

PULSE is D037 engine ID `1` inside `FmtInstrumentHeader.mode == 1` (`8-BIT`). WP4.2 does not wire mode/engine selection into the running full app.

No NOISE, TRIANGLE, WAVETABLE-8, envelope, final voice allocator, engine parameter-page mapping, p-lock engine cache or proof-sine replacement belongs to WP4.2.

## 2. Common pitch conversion becomes canonical here

D038 reuses the exact pitch behavior already proven by the Phase 3 proof lane rather than creating a second oscillator pitch model.

Input domain:

```text
uint16_t pitch_q7
0..16383
1 semitone = 128 units
integer MIDI note n = n << 7
sample rate = 44100 Hz
phase cycle = 2^32
```

The canonical API is:

```c
uint32_t fmt_engine_pitch_inc_q7(uint16_t pitch_q7);
```

For an integer semitone, the helper uses the existing 12-entry C9 increment table exactly:

```text
815363807, 863847862, 915214929, 969636441,
1027294024, 1088380105, 1153098554, 1221665363,
1294309365, 1371273005, 1452813141, 1539201906
```

For note `n`:

```text
integer_increment(n) = INC_C9[n % 12] >> (10 - n / 12)
```

The legal Q7 maximum can interpolate from MIDI 127 toward the internal MIDI-128 increment, so the helper must safely support the internal lookup point `128` even though stored tracker notes remain `0..127`.

The public/legal D037 engine-pitch domain still ends at `16383`; MIDI 128 is **not** a legal engine pitch. It exists only as the private upper lookup value needed when interpolating the legal maximum fractional pitch below MIDI 128.

Fractional semitones use the already-proven D031 interpolation rule from `fmt_playback_interp_inc()`:

```text
diff = hi - lo
q = diff >> 7
r = diff & 127
inc = lo + q * frac + ((r * frac + 64) >> 7)
```

No floating point and no runtime 64-bit division are permitted.

Frozen pitch vectors:

| `pitch_q7` | Meaning | Increment |
|---:|---|---:|
| 0 | MIDI 0 | 796253 |
| 128 | MIDI 1 | 843601 |
| 7680 | MIDI 60 / C4 | 25480118 |
| 8832 | MIDI 69 / A4 | 42852281 |
| 8896 | A4 + 0.5 semitone | 44126346 |
| 16256 | MIDI 127 | 1221665363 |
| 16383 | maximum D037 pitch | 1293741834 |

These exact integers are the compatibility contract. A future higher-accuracy pitch conversion may replace them only through a superseding decision and new deterministic audio evidence.

WP4.2 may move/copy this arithmetic into the canonical helper, but it must **not** alter the Phase 3 proof lane yet. Deduplicating `dinghy.c` onto the new helper is deferred to the later explicit engine-integration migration because even a mathematically equivalent refactor must not silently invalidate the frozen full-app binary/audio baseline.

## 3. PULSE state

The isolated oscillator state is exactly:

```c
typedef struct {
    uint32_t phase;
} FmtPulseOsc;
```

Contract:

- `sizeof(FmtPulseOsc) == 4`;
- no hidden/static mutable oscillator state;
- no pointer ownership;
- no sample-rate field;
- no embedded envelope, gain, pitch or pulse-width parameter;
- reset sets `phase = 0`;
- retrigger is the same oscillator-state operation as reset: `phase = 0`;
- normal NOTE on an already active future voice does not automatically call oscillator retrigger under D037;
- a newly reset/inactive future voice starts from phase zero.

Eight simultaneous Phase 4 PULSE oscillators therefore require exactly 32 bytes of oscillator-specific mutable phase state before common voice/envelope/allocator fields.

## 4. Pulse-width domain

The oscillator receives one unsigned 8-bit pulse-width value.

Frozen legal domain:

```text
FMT_PULSE_WIDTH_MIN     = 32
FMT_PULSE_WIDTH_DEFAULT = 128
FMT_PULSE_WIDTH_MAX     = 224
```

Interpretation:

```text
high_fraction = width / 256 cycle
threshold = width << 24
```

So the legal range is exactly **12.5 % through 87.5 %**, with 50 % at 128.

This deliberately contains classic chip-style widths such as:

- 32 = 12.5 %;
- 64 = 25 %;
- 128 = 50 %;
- 192 = 75 %;
- 224 = 87.5 %.

Values below 32 clamp to 32. Values above 224 clamp to 224. There is no wraparound and no all-high/all-low DC mode in WP4.2.

WP4.8 will later decide which stable parameter ID/page control maps to this value. D038 freezes the oscillator-domain meaning now but assigns no D027 parameter ID.

## 5. Exact render ordering

The oscillator is mono and returns one signed nominal-Q15 sample per call.

Canonical behavior:

```text
width = clamp(width, 32, 224)
threshold = width << 24
sample = +32767 if current_phase < threshold else -32767
phase = current_phase + phase_increment   // uint32 wrap
return sample
```

The **current phase is sampled before phase advance**.

Consequences:

- reset/retrigger phase zero always produces `+32767` for every legal width;
- `phase_increment == 0` holds both phase and output indefinitely;
- unsigned 32-bit overflow is the only cycle wrap mechanism;
- there is no branch or special event at wrap;
- output values are exactly `+32767` or `-32767`, never `-32768`;
- the oscillator does not apply envelope/level scaling or stereo routing.

Frozen 50 % reference vector from reset with increment `0x40000000`:

```text
samples: +32767, +32767, -32767, -32767, repeat
phases after render: 0x40000000, 0x80000000, 0xC0000000, 0x00000000
```

Frozen 25 % reference vector (`width=64`) with the same increment:

```text
samples: +32767, -32767, -32767, -32767, repeat
```

Frozen 75 % reference vector (`width=192`) with the same increment:

```text
samples: +32767, +32767, +32767, -32767, repeat
```

## 6. API boundary

The intended production API is small and explicit:

```c
void fmt_pulse_reset(FmtPulseOsc *osc);
void fmt_pulse_retrigger(FmtPulseOsc *osc);
uint8_t fmt_pulse_clamp_width(uint32_t width);
int32_t fmt_pulse_render(FmtPulseOsc *osc,
                         uint32_t phase_increment,
                         uint8_t width);
```

Null-state behavior is fail-safe:

- reset/retrigger with `NULL` is a no-op;
- render with `NULL` returns 0 and mutates nothing.

`fmt_pulse_render()` may perform the two-comparison width clamp itself. That work is constant and bounded. A later integrated voice may pre-sanitize width at row/parameter-update time, but doing so must not change output semantics.

## 7. Hard-edge / aliasing policy

WP4.2 PULSE is intentionally a **band-unlimited hard-edge pulse**.

No BLEP, polyBLEP, minBLEP, oversampling, FIR/IIR edge smoothing or hidden post-filter is part of this primitive.

Reason:

- Phase 4 is explicitly the chip-style / 8-BIT palette;
- hard transitions provide deterministic, inexpensive and characterful behavior;
- the device has strict realtime constraints;
- anti-alias strategies would change the oscillator state size, sample vectors, CPU budget and sound identity and therefore deserve a separate explicit decision if later desired.

Aliasing at upper registers is therefore an accepted property of WP4.2, not an implementation defect. Any future anti-aliasing must supersede D038 rather than being slipped into a refactor.

## 8. Pitch and PULSE ownership

`fmt_engine_pitch_inc_q7()` is stateless common engine math.

`FmtPulseOsc` owns only phase.

A future integrated Phase 4 voice owns:

- resolved Q7 pitch;
- derived phase increment;
- sanitized engine parameters such as pulse width;
- gate/envelope and common level state;
- track ownership/allocation metadata.

The PULSE primitive does not inspect:

- `FmtSong`;
- `FmtStep`;
- D028 lock records;
- D031/D032 caches;
- D033 event state;
- D034 queue state;
- UI/screen/editor state.

It receives only already-resolved numeric inputs.

## 9. Realtime bounds

One PULSE render is O(1) and contains only:

- at most two width-bound comparisons;
- one 32-bit threshold shift;
- one phase comparison;
- one 32-bit addition;
- one state store.

There is no loop, table search, allocation, divide, modulo, lock traversal, screen access, persistence, blocking, recursion, floating point or 64-bit arithmetic in the render function.

The shared pitch helper runs only when pitch changes, never once per sample. Its table indexing/interpolation is O(1).

## 10. WP4.2 resource budget

Because WP4.2 remains isolated and is not included by `dinghy.c`, the required full-firmware delta at WP4.2 closeout is:

| Resource | Required WP4.2 full-app delta |
|---|---:|
| app image | **0 B** |
| normal RAM | **0 B** |
| pool RAM | **0 B** |
| default WAV | **byte-identical** |
| BIN/FWSC | **byte-identical** |

The isolated primitive itself has these frozen costs/bounds:

- `FmtPulseOsc`: exactly 4 B per oscillator instance;
- eight oscillator phase states: exactly 32 B when later instantiated;
- canonical pitch table: exactly 12 × 4 = 48 B immutable table data;
- no static mutable RAM;
- no pool storage;
- no dynamic allocation.

The actual linked image/RAM cost is measured when a later package intentionally connects the engine into the full app. That later integration must still fit the D037 Phase 4 ceilings.

## 11. Required WP4.2 deterministic tests

Focused host tests must prove at least:

1. compile-time `FmtPulseOsc == 4 B`;
2. reset/retrigger phase zero;
3. null reset/retrigger safety;
4. null render returns 0;
5. width clamps exactly `0..31 -> 32` and `225..UINT32_MAX -> 224`;
6. exact classic widths 32/64/128/192/224;
7. frozen 25/50/75 % vectors above;
8. phase wrap using unsigned overflow;
9. zero increment holds phase/output;
10. deterministic identical-state identical-output replay;
11. all frozen pitch vectors in Section 2;
12. monotonic integer-semitone phase increments over legal MIDI 0..127, plus the exact maximum-Q7 vector whose interpolation exercises the private MIDI-128 upper lookup;
13. monotonic Q7 increments over the full `0..16383` input domain;
14. pitch helper never returns zero in the legal D037 domain;
15. representative long-render hash from a fixed initial state, pitch and width;
16. no mutation outside `FmtPulseOsc.phase`.

Static/source audit must prove no float/double, no runtime 64-bit divide, no allocation and no unbounded loop in the new production modules.

The pinned real JieLi PI32v2 compiler must independently compile both new translation units.

## 12. Full-app regression obligation

Even though the new modules are isolated, WP4.2 closeout must prove that the running firmware remains the frozen WP4.1/Phase 3 baseline:

- complete production app still builds with the pinned toolchain/SDK;
- default host WAV remains SHA-256 `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`;
- `fm1tracker.bin` remains SHA-256 `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27`;
- `fm1tracker-0.1.fwsc` remains SHA-256 `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`;
- resources remain 66,960 B image / 13,296 B normal RAM / 113,000 B pool;
- package identity remains `FM-1_987`.

This is intentional. A PULSE primitive can be complete and target-compiled before the proof lane is migrated.

## 13. Non-goals

WP4.2 does not implement or decide:

- NOISE, TRIANGLE or WAVETABLE-8 DSP;
- common envelope timing/state;
- voice allocation/stealing;
- engine parameter IDs/pages;
- engine p-lock publication cache;
- track pan audibility;
- D032/D033 adapter integration into an 8-BIT voice;
- multi-track engine rendering;
- replacement of `tracker_voice` / proof sine;
- live performance capture;
- persistence/file format;
- final hardware UX.

## 14. WP4.2 exit condition

WP4.2 may close only when:

- D038 is present in `DECISIONS.md` and this contract is committed before production oscillator source;
- focused host vectors and long-run deterministic hash are green;
- both new modules compile with the pinned JieLi target compiler;
- static realtime/source audit is green;
- the full app remains byte-identical to the WP4.1 target/audio baseline because PULSE is still isolated;
- durable `WP4_2_VERIFICATION.md` evidence records exact target/object/full-app results;
- no WP4.3+ implementation has been pulled forward.

After that, WP4.3 / NOISE becomes the only active Phase 4 package.
