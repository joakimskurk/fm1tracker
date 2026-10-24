# WP5.1 Fixed-Point FM Operator Contract

Status: **FROZEN BY D049 BEFORE FM OPERATOR PRODUCTION SOURCE**  
Date: 2026-10-24  
Package: WP5.1 / issue #44  
Branch: `phase5/fm4-operator`  
Parent architecture: D047 + D048 / `WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md`

This document is the normative contract for the first isolated FM4 DSP primitive. It freezes operator state, phase/modulation/level formats, sine generation, render order, deterministic vectors, realtime limits and verification boundaries before `fmt_fm_operator.*` production source is created.

WP5.1 does **not** link FM DSP into `dinghy.c`. The complete WP5.0 application, audio and target artifacts therefore remain byte-identical throughout this package. FM family routing remains unsupported/silent in the running application until a later explicit integration package.

## 1. Scope

WP5.1 introduces one reusable isolated primitive:

```text
fmt_fm_operator.*    one deterministic sine phase-modulation operator
```

The primitive owns only its carrier phase. It receives already-resolved numeric inputs and knows nothing about tracker rows, instruments, D048 routing, FM algorithms, ratios, envelopes, voice allocation or p-locks.

WP5.1 does not implement:

- four-operator algorithm graphs, WP5.2;
- musical ratio choices/pitch policy, WP5.3;
- final level/modulation-index mappings, WP5.4;
- feedback, WP5.5;
- FM envelope strategy, WP5.6;
- FM voice allocation/router integration, WP5.7;
- FM parameter pages/p-lock IDs, WP5.8;
- simultaneous production 8-BIT + FM4 playback, WP5.9.

## 2. Reused phase convention

D049 keeps the accepted D038-D045 oscillator phase convention:

```text
one cycle = 2^32 phase units
state = uint32_t phase
phase increment = uint32_t
wrap = natural unsigned 32-bit overflow
sample current phase before phase advance
```

This means existing `fmt_engine_pitch_inc_q7()` values can later feed an FM operator directly when ratio policy is added. WP5.1 itself does not call or modify the pitch helper and assigns no ratio semantics.

The operator state is exactly:

```c
typedef struct {
    uint32_t phase;
} FmtFmOperator;
```

Contract:

- `sizeof(FmtFmOperator) == 4`;
- no hidden/static mutable operator state;
- no pointer ownership;
- no embedded pitch, ratio, level, envelope, feedback or algorithm state;
- reset sets phase to zero;
- retrigger is the same primitive operation as reset;
- four operators later require exactly 16 B of operator phase state per FM voice before envelope/algorithm/allocator state.

## 3. Carrier waveform: generated quarter-wave sine

The FM carrier is a deterministic symmetric Q15 sine built from one immutable positive quarter-wave table.

Frozen table shape:

```text
entries = 257
entry type = int16_t
entry domain = 0..32767
quarter samples = endpoints i=0..256 inclusive
```

The offline generation rule is:

```text
value[i] = floor(sin((pi / 2) * i / 256) * 32767 + 0.5)
```

The generation rule may use host floating point because it runs only as an offline repository tool/test. No floating point enters firmware source or runtime DSP.

Frozen endpoint values:

```text
value[0]   = 0
value[1]   = 201
value[2]   = 402
value[128] = 23170
value[255] = 32766
value[256] = 32767
```

Frozen SHA-256 of the 257 values serialized as little-endian signed 16-bit integers:

```text
6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68
```

The checked-in generated table must reproduce this exact value sequence. No external waveform asset/code is imported.

Rationale:

- it is symmetric by construction, unlike the old D034 refined-parabola proof sine which has tiny signed-rounding asymmetry;
- 514 B immutable table data is small relative to available image budget;
- linear interpolation gives much finer phase behavior than the 32-step chip wavetable;
- one interpolation multiply per operator sample is bounded and simple;
- quarter-wave symmetry avoids a larger full-cycle table.

The D034 audition sine remains unchanged. D049 creates a separate FM primitive rather than silently altering historical audition behavior.

## 4. Exact sine lookup

For a `uint32_t phase`:

```text
quadrant = phase >> 30
quarter_offset = phase & 0x3fffffff

if quadrant is even:
    coord = quarter_offset
else:
    coord = 0x40000000 - quarter_offset

index = coord >> 22
```

`coord` is therefore `0..0x40000000`, and `index` is `0..256`.

If `index == 256`, magnitude is exactly `table[256]`.

Otherwise:

```text
frac = coord & 0x003fffff
lo = table[index]
hi = table[index + 1]
delta = hi - lo
magnitude = lo + ((delta * frac + 0x00200000) >> 22)
```

All interpolation arithmetic is non-negative unsigned arithmetic and fits 32 bits. There is no signed-right-shift dependency in the interpolation path.

Sign is applied only after magnitude interpolation:

```text
quadrant 0/1 -> +magnitude
quadrant 2/3 -> -magnitude
```

Output domain is exactly bounded to `-32767..32767`. `-32768` is never produced.

Frozen sine vectors:

| phase | output |
|---|---:|
| `0x00000000` | 0 |
| `0x08000000` | 6393 |
| `0x10000000` | 12539 |
| `0x20000000` | 23170 |
| `0x30000000` | 30273 |
| `0x40000000` | 32767 |
| `0x60000000` | 23170 |
| `0x7fffffff` | 0 |
| `0x80000000` | 0 |
| `0xa0000000` | -23170 |
| `0xc0000000` | -32767 |
| `0xe0000000` | -23170 |
| `0xffffffff` | 0 |

Frozen non-node interpolation vectors:

| phase | output |
|---|---:|
| `0x01234567` | 915 |
| `0x05555555` | 4277 |
| `0x15555555` | 16383 |
| `0x23456789` | 24953 |
| `0x55555555` | 28377 |
| `0x76543210` | 7705 |
| `0x87654321` | -5915 |
| `0xabcdef01` | -28824 |
| `0xfedcba98` | -915 |

## 5. Phase-modulation input

The primitive receives one signed phase offset:

```c
int32_t phase_modulation
```

It uses the **same 32-bit phase-unit scale as the carrier accumulator**.

Semantics:

```text
sample_phase = current_phase + (uint32_t)phase_modulation
```

The addition is modulo `2^32`. There is no clamp and no special branch at wrap.

The full signed input domain is legal:

```text
-2147483648 .. 2147483647
```

This represents every distinct phase offset modulo one cycle. Larger conceptual modulation indices are later mapped into this periodic phase domain by WP5.4/WP5.2 logic, not by changing the primitive phase representation.

Frozen modulation vectors at unity level and zero increment:

| carrier phase | modulation | sample phase | output |
|---|---:|---|---:|
| `0x10000000` | `+0x10000000` | `0x20000000` | 23170 |
| `0x10000000` | `-0x10000000` | `0x00000000` | 0 |
| `0x70000000` | `+0x20000000` | `0x90000000` | -12539 |
| `0x00000000` | `+0x40000000` | `0x40000000` | 32767 |
| `0x00000000` | `-0x40000000` | `0xc0000000` | -32767 |
| `0x00000000` | `INT32_MIN` | `0x80000000` | 0 |
| `0x00000000` | `INT32_MAX` | `0x7fffffff` | 0 |

WP5.1 does not define how another operator output becomes this phase offset. That modulation-index mapping belongs to WP5.4 and algorithm routing belongs to WP5.2.

## 6. Operator level

The primitive receives a numeric level control as `uint32_t level_q15` and clamps it to:

```text
0..32767
```

This deliberately matches the existing D042 common-envelope Q15 maximum.

Frozen semantics:

- level 0 returns exactly 0;
- level >= 32767 is exact unity and returns the raw sine sample unchanged;
- values `1..32766` use symmetric magnitude scaling;
- negative samples are converted to positive magnitude before multiplication and sign is restored afterward;
- no signed negative right shift is required.

For nonzero, non-unity levels:

```text
magnitude = abs(raw_sample)
scaled = (magnitude * level + 16384) >> 15
output = sign(raw_sample) * scaled
```

The largest multiply is below 2^31 and fits 32-bit arithmetic.

Frozen quarter-cycle level vectors (`raw_sample = 32767`):

| input level | sanitized | output |
|---:|---:|---:|
| 0 | 0 | 0 |
| 1 | 1 | 1 |
| 8192 | 8192 | 8192 |
| 16384 | 16384 | 16384 |
| 24576 | 24576 | 24575 |
| 32766 | 32766 | 32765 |
| 32767 | 32767 | 32767 |
| 65535 | 32767 | 32767 |

Negative input sine values must produce the exact sign-mirrored result.

WP5.1 operator level is a primitive amplitude scalar only. Final musical operator-level/modulation-index controls are explicitly deferred to WP5.4.

## 7. Exact render order

The canonical API is intended to be:

```c
void fmt_fm_operator_reset(FmtFmOperator *op);
void fmt_fm_operator_retrigger(FmtFmOperator *op);
uint32_t fmt_fm_operator_clamp_level_q15(uint32_t level_q15);
int32_t fmt_fm_operator_sine_q15(uint32_t phase);
int32_t fmt_fm_operator_render(FmtFmOperator *op,
                               uint32_t phase_increment,
                               int32_t phase_modulation,
                               uint32_t level_q15);
```

Render ordering is frozen:

```text
1. read current carrier phase
2. add signed modulation modulo 2^32 to form sample phase
3. evaluate deterministic sine at sample phase
4. apply clamped Q15 level
5. advance carrier phase by phase_increment using uint32 wrap
6. return sample
```

The current carrier phase is therefore sampled **before** phase advance, matching the accepted D038-D041 oscillator convention.

From reset, unity level, zero modulation and increment `0x40000000`, exact samples are:

```text
0, 32767, 0, -32767, repeat
```

Phases after each render are:

```text
0x40000000, 0x80000000, 0xc0000000, 0x00000000
```

`phase_increment == 0` holds carrier state indefinitely. Modulation may still change the instantaneous sample without mutating carrier phase.

Null behavior is fail-safe:

- reset/retrigger with `NULL` is a no-op;
- render with `NULL` returns 0;
- the sine helper is stateless and has no null input.

## 8. Deterministic long vector

A 4,096-render primitive stress vector is frozen before implementation.

Initial state:

```text
phase = 0x13579bdf
phase_increment = 42852281   // existing exact D038 A4 increment
```

For render index `i = 0..4095`:

```text
mod_u32 = (i * 0x1f123bb5) modulo 2^32
phase_modulation = reinterpret mod_u32 as int32_t
level_q15 = (i * 257) & 0x7fff
```

Each returned sample is serialized as little-endian signed 32-bit.

Frozen result:

```text
output SHA-256 = 499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66
final phase = 0xf1532bdf
first 12 samples = 0, 248, 437, 146, -609, -1277, -1165, -56, 1466, 2310, 1650, -361
```

This vector deliberately crosses positive/negative phase modulation, phase wrap and a wide level range.

## 9. Realtime bounds

One FM operator render is O(1).

Maximum primitive work per sample is bounded to:

- one phase-modulation addition;
- quadrant/index/mirror bit operations;
- two immutable table reads;
- one unsigned interpolation multiply;
- one level clamp;
- at most one unsigned level multiply;
- one phase advance;
- fixed branches only.

There is no loop, search, allocation, lock traversal, song/store access, queue access, screen work, persistence, blocking, recursion, division/modulo operation, floating point or 64-bit arithmetic in the render primitive.

The quarter table is direct indexed. Runtime `%` and `/` are not permitted in `fmt_fm_operator_render()` or the sine helper.

## 10. Ownership boundary

`FmtFmOperator` owns only carrier phase.

The primitive must not inspect or mutate:

- `FmtSong` / `FmtStep`;
- `FmtInstrumentHeader`;
- D048 route/publication state;
- p-lock storage/cache;
- tracker effects/events;
- voice allocator state;
- envelope state;
- UI/persistence.

Future layers own pitch/ratio, algorithm routing, modulation-index mapping, feedback, envelopes, voice allocation and family routing.

## 11. Generated-data provenance

The quarter-wave values are mathematical generated data, not copied from an external implementation or asset.

The repository must contain a deterministic generator for the checked-in table. Verification must regenerate the 257-value binary sequence and assert SHA-256 `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68`.

The firmware build must consume the checked-in generated values and must not require Python or host floating point at build/runtime.

## 12. WP5.1 resource contract

WP5.1 remains isolated and is not included by `dinghy.c`.

Therefore the full-app closeout requirement is exact WP5.0 identity:

| Resource/artifact | Required WP5.1 full-app result |
|---|---|
| app image | 72,792 B |
| normal RAM | 19,676 B |
| pool RAM | 113,000 B |
| normal WAV | SHA-256 `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076` |
| representative WAV | SHA-256 `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9` |
| BIN | SHA-256 `a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125` |
| FWSC | SHA-256 `cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073` |

Isolated primitive bounds:

- `FmtFmOperator` exactly 4 B;
- four operator phase states exactly 16 B when later instantiated per FM voice;
- quarter table exactly 514 B immutable payload;
- no static mutable RAM;
- no pool allocation;
- target object `.text + immutable table/rodata` must remain <= 4,096 B;
- exact target object sections are recorded at closeout.

Actual linked FM-family cost and polyphony are deliberately measured in WP5.7/WP5.10, not guessed here.

## 13. Required focused tests

After implementation, host/target tests must prove at minimum:

1. `sizeof(FmtFmOperator) == 4`;
2. reset/retrigger phase zero;
3. null reset/retrigger safety;
4. null render returns 0;
5. level clamp `0..32767`, values above clamp to 32767;
6. regenerated quarter-table binary hash exactly matches D049;
7. all frozen sine anchor vectors;
8. all frozen non-node interpolation vectors;
9. exact sign symmetry over a broad deterministic phase sweep;
10. all frozen phase-modulation vectors;
11. all frozen level vectors and negative sign mirrors;
12. current-phase-before-advance quarter-cycle vector;
13. unsigned phase wrap near `UINT32_MAX`;
14. zero increment holds phase;
15. changing modulation with zero increment changes sample but not carrier state;
16. identical initial state + inputs produce identical samples/state;
17. frozen 4,096-render SHA-256 and final phase;
18. no mutation outside `FmtFmOperator.phase`;
19. no float/double, heap, runtime divide/modulo or 64-bit arithmetic in production primitive;
20. independent pinned JieLi PI32v2 compilation of the module;
21. isolated object size/sections inside the D049 ceiling;
22. full application remains exact WP5.0 audio/resource/BIN/FWSC identity;
23. semantically applicable WP5.0 router/D046 regressions remain green.

## 14. Contract-first gate

Before `fmt_fm_operator.*` production files exist, a focused repository gate must prove:

- this D049 contract exists and is marked frozen;
- production FM operator source/header are absent;
- `dinghy.c` has no FM operator include;
- the table-generation rule reproduces the frozen table hash;
- all frozen reference vectors and long-run hash reproduce in a host-only reference model;
- WP5.0 canonical normal WAV/BIN/FWSC/resource identities remain the baseline.

This gate becomes historical evidence after production source is added and is not required to stay runnable as an "operator absent" regression.

## 15. Physical hardware boundary

No physical FM-1 is available.

WP5.1 may prove deterministic math, target compilation, object/resource size and full-app non-regression. It may not claim audible FM quality on hardware or actual AC791N callback margin.

`hardware_margin_claim=none`

## 16. WP5.1 exit condition

WP5.1 may close only when:

- D049 was committed before production FM operator source;
- the contract-first reference gate is durably green;
- production primitive matches every frozen vector;
- generated quarter-table provenance/hash is reproduced;
- source/realtime audit is green;
- pinned target compile/object-size evidence is green;
- full app remains byte-identical to canonical WP5.0 because FM is still isolated;
- `WP5_1_VERIFICATION.md` records exact evidence;
- no WP5.2+ functionality has been pulled forward.

After that, WP5.2 curated algorithm routing becomes the only active package.
