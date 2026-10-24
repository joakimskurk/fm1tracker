# WP5.3 FM4 Integer Ratio and Pitch Mapping Contract

Status: **FROZEN BY D051 BEFORE FM RATIO PRODUCTION SOURCE**  
Date: 2026-10-25  
Package: WP5.3 / issue #46  
Branch: `phase5/fm4-ratios`  
Parent architecture: D047 + D048  
DSP prerequisites: D049 + D050

This document is the normative D051 contract for the isolated FM4 operator-ratio layer. It freezes the ratio IDs, exact numeric meanings, canonical base-increment domain, 32-bit-only scaling arithmetic, rounding, high-register ceiling behavior, four-operator mapping API, deterministic reference vectors and resource limits before `fmt_fm_ratio.*` production source is created.

WP5.3 remains isolated from `dinghy.c`, the D048 family router and audible FM production. The full running application must therefore remain byte-identical to the accepted WP5.0 application throughout this package.

## 1. Scope

WP5.3 introduces one stateless transform layer:

```text
canonical D038 pitch increment
        |
        v
one base uint32 phase increment
        |
        v
D051 ratio ID x 4
        |
        v
four bounded uint32 operator phase increments
        |
        v
D050 algorithm router
```

WP5.3 owns only ratio selection semantics and base-increment scaling. It does not own operator phase, algorithm state, levels, modulation index, feedback, envelopes, voices, tracker parameters or p-locks.

Production source, when implementation is permitted, is limited to:

```text
firmware/src/fmt_fm_ratio.h
firmware/src/fmt_fm_ratio.c
```

## 2. Design reference, not emulation

Classic Yamaha FM instruments were consulted only as a behavioral reference for the distinction between keyboard-tracking ratio mode and fixed-frequency mode, and for the musical usefulness of both harmonic and irregular operator ratios.

D051 does **not** reproduce a Yamaha ratio table, coarse/fine control law, patch format or fixed-frequency mode.

FM1Tracker V1 instead uses a small curated set of stable ratio IDs. The reasons are:

- bounded constant-time mapping;
- compact instrument representation later;
- deterministic integer-only DSP preparation;
- useful tonal coverage without arbitrary runtime rational arithmetic;
- no need for a large coarse/fine parameter surface before the FM engine itself is integrated and measured.

No third-party source code, tables or firmware data are used.

## 3. Reused base-pitch domain

D051 reuses the accepted D038 helper without changing it:

```c
uint32_t fmt_engine_pitch_inc_q7(uint16_t pitch_q7);
```

The legal tracker pitch domain remains:

```text
pitch_q7 = 0..16383
```

The exact phase-increment result at the legal maximum is:

```text
FMT_FM_RATIO_BASE_INCREMENT_MAX = 1293741834
```

This value is frozen by D051 because it is the exact output of the existing D038 helper at `pitch_q7 = 16383`.

The ratio layer accepts a `uint32_t base_increment`, but sanitizes malformed values above this canonical maximum to `1293741834` before ratio scaling.

Thus the supported semantic base domain is:

```text
0..1293741834
```

D051 does not change D038 interpolation, pitch slides, tracker pitch representation or note semantics.

## 4. Stable ratio IDs

D051 freezes exactly 16 V1 ratio IDs.

All exact values are represented as unsigned Q8 ratios:

```text
ratio = ratio_q8 / 256
```

| ID | symbolic intent | exact ratio_q8 | exact ratio |
|---:|---|---:|---:|
| 0 | HALF | 128 | 0.5000000 |
| 1 | UNITY | 256 | 1.0000000 |
| 2 | FIVE_QUARTERS | 320 | 1.2500000 |
| 3 | INHARMONIC_1P414 | 362 | 1.4140625 |
| 4 | THREE_HALVES | 384 | 1.5000000 |
| 5 | TWO | 512 | 2.0000000 |
| 6 | FIVE_HALVES | 640 | 2.5000000 |
| 7 | THREE | 768 | 3.0000000 |
| 8 | SEVEN_HALVES | 896 | 3.5000000 |
| 9 | FOUR | 1024 | 4.0000000 |
| 10 | FIVE | 1280 | 5.0000000 |
| 11 | SIX | 1536 | 6.0000000 |
| 12 | SEVEN | 1792 | 7.0000000 |
| 13 | EIGHT | 2048 | 8.0000000 |
| 14 | TWELVE | 3072 | 12.0000000 |
| 15 | SIXTEEN | 4096 | 16.0000000 |

Frozen constants:

```text
FMT_FM_RATIO_COUNT = 16
FMT_FM_RATIO_UNITY = 1
```

Ratio ID 3 is intentionally not claimed to be mathematically exact square root of two. Its normative value is exactly `362/256 = 1.4140625`.

The palette deliberately contains:

- one sub-unity ratio, 0.5;
- unity;
- low non-integer ratios for alternate/inharmonic spectra;
- common small integer ratios;
- bright high integer ratios useful especially in lower registers.

No additional ratios may be inserted by renumbering these IDs. Future expansion must preserve IDs 0..15.

## 5. Invalid ratio behavior

Any ratio selector outside `0..15` sanitizes to:

```text
FMT_FM_RATIO_UNITY = 1
```

Invalid data therefore preserves the canonical base frequency instead of selecting HALF, producing silence, wrapping randomly or reading outside a table.

The sanitizer is pure and constant-time.

## 6. Operator phase-increment ceiling

The D049 phase accumulator is a 32-bit modulo accumulator, but D051 does not allow ratio scaling to turn high operator frequencies into accidental wrapped/folded base increments.

The frozen output ceiling is:

```text
FMT_FM_RATIO_INCREMENT_CEILING = 0x7fffffff = 2147483647
```

This is strictly below the exact half-cycle increment `0x80000000`.

Rationale:

- increments above half a cycle per sample represent already-folded discrete-time sine frequencies;
- exact `0x80000000` is a pathological Nyquist case for a sine starting at zero phase;
- deterministic saturation is easier to reason about than implicit unsigned wrap;
- this is a local oscillator-increment safety rule, not a claim that FM sidebands are bandlimited.

Important limitation:

**D051 is not an anti-aliasing system.** FM modulation can create sidebands above Nyquist even when every operator increment is below this ceiling. Audio-quality/oversampling policy, if required after hardware measurement, belongs to a later explicit decision.

## 7. Frozen precomputed clamp boundaries

Runtime division is forbidden. Each ratio therefore has one precomputed largest canonical base increment that may be scaled without crossing the D051 output ceiling after frozen rounding.

The mathematical offline definition is:

```text
max_base_for_ratio = min(
    FMT_FM_RATIO_BASE_INCREMENT_MAX,
    floor((FMT_FM_RATIO_INCREMENT_CEILING * 256 + 127) / ratio_q8)
)
```

The production source must contain the resulting constants, not evaluate this division at runtime.

Frozen table:

| ID | ratio | max base before output ceiling |
|---:|---:|---:|
| 0 | 0.5 | 1293741834 |
| 1 | 1.0 | 1293741834 |
| 2 | 1.25 | 1293741834 |
| 3 | 1.4140625 | 1293741834 |
| 4 | 1.5 | 1293741834 |
| 5 | 2.0 | 1073741823 |
| 6 | 2.5 | 858993458 |
| 7 | 3.0 | 715827882 |
| 8 | 3.5 | 613566756 |
| 9 | 4.0 | 536870911 |
| 10 | 5.0 | 429496729 |
| 11 | 6.0 | 357913941 |
| 12 | 7.0 | 306783378 |
| 13 | 8.0 | 268435455 |
| 14 | 12.0 | 178956970 |
| 15 | 16.0 | 134217727 |

If sanitized base increment is larger than the selected ratio's frozen maximum, output is exactly `0x7fffffff`.

## 8. Frozen 32-bit scaling arithmetic

No 64-bit intermediate is required or permitted in production ratio mapping.

For a sanitized base that does not exceed the selected precomputed boundary:

```text
whole = base_increment >> 8
remainder = base_increment & 255

scaled = whole * ratio_q8
       + ((remainder * ratio_q8 + 128) >> 8)
```

This is mathematically equivalent to round-half-up of:

```text
base_increment * ratio_q8 / 256
```

without constructing the full wide product.

Safety proof under D051:

- `whole * ratio_q8` is no larger than the accepted output ceiling because the precomputed boundary is checked first;
- `remainder <= 255`;
- `ratio_q8 <= 4096`;
- `remainder * ratio_q8 <= 1044480`;
- adding 128 remains safely inside uint32;
- the final result is `0..0x7fffffff`;
- no signed arithmetic is needed.

Runtime ratio mapping must contain:

- no `/` division;
- no `%` modulo;
- no `uint64_t`/`int64_t` or `long long`;
- no float/double;
- no heap;
- no table search or graph traversal.

A direct indexed read of the 16-entry immutable ratio/boundary tables is allowed.

## 9. High-register behavior

High ratios intentionally stop preserving their mathematical ratio once the operator would cross the D051 ceiling.

First legal Q7 pitch positions that produce a clamped result for selected high ratios are frozen as regression anchors:

| ratio | first clamped pitch_q7 | note + fraction |
|---:|---:|---:|
| 2.0 | 15970 | 124.765625 |
| 2.5 | 15476 | 120.906250 |
| 3.0 | 15071 | 117.742188 |
| 3.5 | 14730 | 115.078125 |
| 4.0 | 14434 | 112.765625 |
| 5.0 | 13940 | 108.906250 |
| 6.0 | 13535 | 105.742188 |
| 7.0 | 13194 | 103.078125 |
| 8.0 | 12898 | 100.765625 |
| 12.0 | 11999 | 93.742188 |
| 16.0 | 11362 | 88.765625 |

Ratios 0.5, 1.0, 1.25, 1.4140625 and 1.5 never clamp anywhere in the current legal tracker pitch domain.

D051 chooses explicit saturation instead of silent ratio substitution, octave folding or engine substitution.

## 10. Public API contract

The implementation must expose the equivalent of:

```c
uint8_t fmt_fm_ratio_sanitize(uint32_t ratio_id);
uint16_t fmt_fm_ratio_q8(uint32_t ratio_id);
uint32_t fmt_fm_ratio_apply(uint32_t base_increment,
                            uint32_t ratio_id);
void fmt_fm_ratio_map4(uint32_t base_increment,
                       const uint8_t ratio_id[4],
                       uint32_t out_increment[4]);
```

Required semantics:

- `fmt_fm_ratio_sanitize()` returns the input for 0..15, otherwise UNITY;
- `fmt_fm_ratio_q8()` uses sanitized ID semantics;
- `fmt_fm_ratio_apply()` clamps malformed base to the canonical D051 base maximum, then applies the selected ratio and D051 output ceiling;
- `fmt_fm_ratio_map4()` performs exactly four bounded mappings, OP1 through OP4 in index order;
- `out_increment == NULL` makes `map4` a no-op;
- `ratio_id == NULL` means UNITY for all four outputs;
- no persistent/mutable ratio DSP state exists.

The four outputs are ready to be passed to D050's existing:

```c
const uint32_t phase_increment[4]
```

WP5.3 does not alter D050.

## 11. Deterministic anchor vectors

Canonical D038 base anchors:

| pitch_q7 | base increment |
|---:|---:|
| 0 | 796253 |
| 128 | 843601 |
| 7680 | 25480118 |
| 15360 | 815363807 |
| 16000 | 1088380105 |
| 16383 | 1293741834 |

Selected D051 outputs:

| pitch_q7 | 0.5x | 1x | 1.4140625x | 2x | 4x | 8x | 16x |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 0 | 398127 | 796253 | 1125952 | 1592506 | 3185012 | 6370024 | 12740048 |
| 128 | 421801 | 843601 | 1192905 | 1687202 | 3374404 | 6748808 | 13497616 |
| 7680 | 12740059 | 25480118 | 36030479 | 50960236 | 101920472 | 203840944 | 407681888 |
| 15360 | 407681904 | 815363807 | 1152975383 | 1630727614 | 2147483647 | 2147483647 | 2147483647 |
| 16000 | 544190053 | 1088380105 | 1539037492 | 2147483647 | 2147483647 | 2147483647 | 2147483647 |
| 16383 | 646870917 | 1293741834 | 1829431812 | 2147483647 | 2147483647 | 2147483647 | 2147483647 |

Four-operator mapping anchors:

```text
pitch_q7 7680
base 25480118
IDs [1, 5, 3, 13]
OUT [25480118, 50960236, 36030479, 203840944]

pitch_q7 15360
base 815363807
IDs [0, 4, 9, 15]
OUT [407681904, 1223045711, 2147483647, 2147483647]

pitch_q7 16383
base 1293741834
IDs [1, 3, 5, 15]
OUT [1293741834, 1829431812, 2147483647, 2147483647]
```

Invalid ratio IDs must match UNITY exactly.

Malformed `base_increment > 1293741834` must first sanitize to `1293741834` before ratio mapping.

## 12. Frozen table/reference hashes

Ratio Q8 table serialized as 16 little-endian uint16 values:

```text
SHA-256 5d98aebf644051ac555efcf1ceabf4015f26aaa2884722b515c73b5518659a60
```

Clamp-boundary table serialized as 16 little-endian uint32 values:

```text
SHA-256 c39e8210aadfc1bd347fbb86a7c2058ae8681c123892044c316cf48a55a15255
```

Combined Q8 table followed by clamp-boundary table:

```text
SHA-256 17d978a26a632bf5f28dc7efcc519f84dd270a2a1c19baac9739dd8e8605f28c
```

Exhaustive legal-pitch mapping reference:

- iterate `pitch_q7 = 0..16383`;
- resolve each base increment using the exact D038 integer helper semantics;
- for each pitch, map ratio IDs 0..15 in ascending ID order;
- serialize every output as little-endian uint32;
- total outputs = 262144;
- total payload = 1048576 B.

Frozen exhaustive SHA-256:

```text
5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c
```

The reference contains 27615 saturated pitch/ratio outputs at the D051 ceiling.

## 13. Realtime and resource bounds

The ratio layer is intended to run when pitch/ratio state is resolved or updated. It is not required to be recomputed for every FM sample.

Even if called from a realtime path, cost remains statically bounded.

Frozen source/runtime constraints:

- immutable tables only;
- no mutable global state;
- no heap;
- no float/double;
- no 64-bit integer types;
- no runtime division/modulo;
- no unbounded loops/search/traversal;
- `map4` does exactly four mappings;
- no tracker/store/router/UI dependency;
- no D049/D050 mutation.

Pinned PI32v2 implementation ceiling:

```text
fmt_fm_ratio.o .text + .rodata <= 1024 B
```

This is a ceiling, not a target. Actual object size must be recorded at WP5.3 closeout.

## 14. Source-absent verification gate

Before `fmt_fm_ratio.h/.c` may exist, CI must prove:

1. this D051 document is present with frozen status;
2. production ratio source is absent;
3. D049 and D050 source remain unchanged from the WP5.2 closeout baseline `826a7dca7394d6e2c13f79fafa97bd05fb1986c8`;
4. the Q8 ratio table and boundary table hashes match D051;
5. all frozen anchor vectors match;
6. all first-clamp pitch anchors match;
7. exhaustive 16384 x 16 legal-pitch mapping matches SHA `5b7b862b...`;
8. invalid-ID and malformed-base semantics are deterministic;
9. inherited D050/D049/WP5.0 host regressions remain exact.

Only a green source-absent gate authorizes production ratio implementation.

## 15. Implementation verification gate

After source is allowed, WP5.3 must additionally prove:

- only `fmt_fm_ratio.h/.c` are added under production firmware for this isolated primitive;
- C implementation matches all D051 vectors and exhaustive reference behavior;
- static audit proves no forbidden arithmetic/state/dependencies;
- D049 and D050 vectors/hashes remain exact;
- pinned PI32v2 compile succeeds;
- object text+rodata is <= 1024 B and exact size/hash are recorded;
- full target application remains exact WP5.0 because ratio source is still isolated;
- `dinghy.c` remains unchanged;
- `hardware_margin_claim=none` until physical hardware exists.

## 16. Explicitly out of scope

D051/WP5.3 does not decide or implement:

- user-facing free coarse/fine Yamaha-style ratio editing;
- fixed-frequency operator mode;
- final operator-level mapping or modulation-index UX, WP5.4;
- feedback, WP5.5;
- FM envelope strategy, WP5.6;
- FM voice allocation or D048 family-router integration, WP5.7;
- FM parameter pages or p-lock IDs, WP5.8;
- simultaneous audible 8-BIT + FM4 integration, WP5.9;
- oversampling/bandlimiting policy;
- physical callback-margin claims.

## 17. D051 compatibility rule

After D051 is accepted:

- IDs 0..15 and their exact Q8 values are stable semantic identities;
- invalid ratio ID -> UNITY is stable;
- canonical base maximum `1293741834` is stable unless a later explicit pitch-contract decision supersedes D038/D051;
- output ceiling `0x7fffffff` and clamp-before-scale semantics are stable;
- round-half-up Q8 mapping semantics are stable;
- no future UI is allowed to reinterpret these IDs as unrelated ratios.

Any later expansion must preserve these semantics or explicitly supersede D051 with a migration plan.
