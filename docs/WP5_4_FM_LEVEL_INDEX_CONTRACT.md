# WP5.4 FM4 Operator Level and Modulation Index Contract

Status: **FROZEN BY D052 BEFORE FM CONTROL-MAPPING PRODUCTION SOURCE**  
Date: 2026-10-25  
Package: WP5.4 / issue #47  
Branch: `phase5/fm4-levels`  
Parent architecture: D047 + D048  
DSP prerequisites: D049 + D050 + D051

This document is the normative D052 contract for the isolated FM4 level/modulation-index control layer. It freezes the V1 control domains, exact integer mappings, topology-role semantics, malformed-input behavior, overflow proof, representative D051 composition vectors and resource limits before `fmt_fm_controls.*` production source is created.

WP5.4 remains isolated from `dinghy.c`, the D048 family router and audible FM production. The full running application must therefore remain byte-identical to the accepted WP5.0 application throughout this package.

## 1. Scope

WP5.4 introduces one stateless control transform:

```text
four operator LEVEL bytes 0..255
             +
one global INDEX byte 0..255
             |
             v
D052 integer mapping
             |
             +--> D050 level_q15[4]
             |
             `--> D050 mod_depth_q14
```

WP5.4 owns only this mapping. It does not own ratios, feedback, envelopes, voice allocation, mixed-engine routing, tracker parameter IDs, p-lock UI or final instrument defaults.

Production source, when implementation is permitted, is limited to:

```text
firmware/src/fmt_fm_controls.h
firmware/src/fmt_fm_controls.c
```

## 2. V1 control model

D052 freezes exactly five byte-sized internal controls:

```text
OP1_LEVEL  0..255
OP2_LEVEL  0..255
OP3_LEVEL  0..255
OP4_LEVEL  0..255
INDEX      0..255
```

These are internal DSP controls, not a frozen front-panel or tracker UI law. WP5.8 may later choose a perceptual knob/p-lock presentation while preserving the exact D052 DSP values passed into this layer.

D052 deliberately does not reproduce Yamaha 0..99 output-level behavior, logarithmic attenuation tables or any third-party patch format.

## 3. Why one global INDEX is retained

D050 already exposes one global `mod_depth_q14` plus four independent `level_q15` values. D052 retains that representation instead of expanding D050 to per-edge modulation depths.

The four frozen D050 algorithms have these source roles:

| Algorithm | Carriers | Modulation sources |
|---|---|---|
| STACK4 | OP1 | OP2, OP3, OP4 |
| FAN3 | OP1 | OP2, OP3, OP4 |
| DUAL2 | OP1, OP3 | OP2, OP4 |
| PARALLEL4 | OP1, OP2, OP3, OP4 | none |

No operator is simultaneously a carrier and a downstream modulation source in the same frozen topology. Therefore:

- carrier loudness is set independently by that carrier's LEVEL;
- each modulation source has its own LEVEL and can be balanced independently;
- INDEX supplies one global modulation-strength scale;
- PARALLEL4 ignores INDEX audibly because it has no modulation edges;
- per-edge INDEX would add API/state without evidence that V1 needs it.

A later package may supersede this only with measured musical/resource evidence. Existing D052 byte semantics must not be silently reinterpreted.

## 4. Control sanitization

Scalar mapping functions accept `uint32_t` so malformed callers can be tested.

The frozen sanitizer is saturation:

```text
sanitize(x) = min(x, 255)
```

There is no wrap and no invalid selector alias.

For the four-level array mapping:

- `out == NULL` -> no-op;
- `level_control == NULL` -> all four mapped levels are zero;
- otherwise exactly four values are mapped in OP1..OP4 index order.

Null level input therefore fails toward silence.

## 5. LEVEL byte -> D049 Q15 mapping

For sanitized byte `v`:

```text
level_q15 = (v << 7) | (v >> 1)
```

Domain/range:

```text
0..255 -> 0..32767
```

This mapping is exactly equal, for every legal byte, to nearest-integer scaling of:

```text
v * 32767 / 255
```

with the frozen integer reference:

```text
(v * 32767 + 127) / 255
```

The division exists only in offline/reference proof. Production uses the shift/OR form.

Anchors:

| LEVEL | Q15 |
|---:|---:|
| 0 | 0 |
| 1 | 128 |
| 32 | 4112 |
| 64 | 8224 |
| 96 | 12336 |
| 127 | 16319 |
| 128 | 16448 |
| 160 | 20560 |
| 192 | 24672 |
| 224 | 28784 |
| 254 | 32639 |
| 255 | 32767 |

`LEVEL=0` produces exact zero operator output under D049 while phase still advances according to D049. `LEVEL=255` maps to D049's exact full-scale level.

## 6. INDEX byte -> D050 Q14 modulation-depth mapping

For sanitized byte `v`:

```text
mod_depth_q14 = (v << 6) | (v >> 2)
```

Domain/range:

```text
0..255 -> 0..16383
```

This is a direct 8-bit to 14-bit bit-replication expansion. It is monotonic, endpoint-exact and differs by at most one Q14 unit from nearest-integer ideal linear scaling to `0..16383`.

No hidden nonlinear or perceptual law is frozen into the DSP layer.

Anchors:

| INDEX | Q14 |
|---:|---:|
| 0 | 0 |
| 1 | 64 |
| 3 | 192 |
| 4 | 257 |
| 32 | 2056 |
| 64 | 4112 |
| 96 | 6168 |
| 127 | 8159 |
| 128 | 8224 |
| 160 | 10280 |
| 192 | 12336 |
| 224 | 14392 |
| 254 | 16319 |
| 255 | 16383 |

`INDEX=0` makes every D050 modulation edge contribute zero phase offset. D050 still advances all four operators exactly once per render as frozen by D050.

## 7. No operator-level/index fusion

D052 does not pre-multiply LEVEL and INDEX into a new persistent parameter.

The mapped values remain separate:

```text
level_q15[4]
mod_depth_q14
```

D049 first applies each operator's Q15 level to its sine output. D050 then applies the global modulation depth only when that output is used on a modulation edge.

This preserves two musically distinct controls:

- source amount per operator via LEVEL;
- global FM intensity via INDEX.

Carrier LEVEL is never multiplied by INDEX merely because the same byte representation exists.

## 8. Overflow proof

D050's frozen edge transform is:

```text
scaled = sample * depth
phase_offset = scaled * 4
```

Under D049/D052:

```text
abs(sample) <= 32767
0 <= depth <= 16383
```

Worst positive magnitude:

```text
32767 * 16383 * 4 = 2147287044
```

Signed 32-bit positive maximum:

```text
INT32_MAX = 2147483647
margin    = 196603
```

Therefore the existing D050 single-edge calculation is safe at all 65,536 legal LEVEL/INDEX pairs.

FAN3 multi-source accumulation remains D050's explicit modulo-2^32 phase accumulation. D052 does not change that rule.

## 9. Frozen exhaustive mapping hashes

Serialize every mapped value little-endian uint16.

All 256 LEVEL mappings in ascending control order:

```text
SHA-256 a799f70bd763053e79c3d9de6acdda97523fe54632105756c4252dc33d51c815
```

All 256 INDEX mappings in ascending control order:

```text
SHA-256 cf791163cad612063084d0479cb00b8b86e4e2d19102c461bf548bcca4b69bc7
```

LEVEL payload followed by INDEX payload:

```text
SHA-256 26e55d6a5fcacb5f432ecc3620b41a1c18e23f277aea6a13cb72d988a1929b38
```

For overflow composition, iterate LEVEL 0..255 outer, INDEX 0..255 inner. For each pair:

1. map LEVEL to Q15;
2. resolve the maximum positive D049 sample from sine peak 32767 using D049's exact level arithmetic;
3. map INDEX to Q14;
4. evaluate D050's exact single-edge magnitude;
5. serialize the uint32 magnitude little-endian.

Total values: 65,536.  
Payload bytes: 262,144.  
Maximum: 2,147,287,044.  
Frozen SHA-256:

```text
cf642aee2276abf355a62b7fb0ff1d8ff496e49b9d0a3397a0acc88fff421de0
```

## 10. Representative D051 composition vectors

D052 does not alter D051 ratios. These vectors freeze composition at the D050 input boundary.

Vector A:

```text
pitch_q7 7680
ratios [1, 5, 3, 13]
phase_increment [25480118, 50960236, 36030479, 203840944]
LEVEL [255, 192, 128, 64]
level_q15 [32767, 24672, 16448, 8224]
INDEX 128
mod_depth_q14 8224
```

Vector B:

```text
pitch_q7 15360
ratios [0, 4, 9, 15]
phase_increment [407681904, 1223045711, 2147483647, 2147483647]
LEVEL [64, 128, 192, 255]
level_q15 [8224, 16448, 24672, 32767]
INDEX 255
mod_depth_q14 16383
```

Vector C:

```text
pitch_q7 0
ratios [1, 5, 3, 13]
phase_increment [796253, 1592506, 1125952, 6370024]
LEVEL [0, 64, 128, 255]
level_q15 [0, 8224, 16448, 32767]
INDEX 64
mod_depth_q14 4112
```

Serialized as four uint32 increments, four uint16 levels and one uint16 depth per vector, A then B then C:

```text
SHA-256 e1349b6d834de9655c2417e4a7a5aabdf64bed6138d8026bec10e23a4465486c
```

## 11. Public API contract

The implementation must expose the equivalent of:

```c
#define FMT_FM_CONTROL_MAX 255u

typedef struct {
    uint16_t level_q15[4];
    uint16_t mod_depth_q14;
} FmtFmMappedControls;

uint8_t fmt_fm_control_sanitize(uint32_t control);
uint16_t fmt_fm_level_q15(uint32_t control);
uint16_t fmt_fm_index_q14(uint32_t control);
void fmt_fm_controls_map(const uint8_t level_control[4],
                         uint8_t index_control,
                         FmtFmMappedControls *out);
```

Required semantics:

- `FmtFmMappedControls` is transient prepared input, not persistent oscillator/voice state;
- expected size is exactly 10 B;
- scalar mappings saturate malformed values above 255;
- `controls_map` performs exactly four LEVEL mappings plus one INDEX mapping;
- `out == NULL` is a no-op;
- null LEVEL pointer maps all four levels to zero;
- no mutable/static runtime state exists.

## 12. Realtime/source restrictions

Production mapping must contain:

- no heap;
- no float/double;
- no `uint64_t`, `int64_t` or `long long`;
- no runtime `/` or `%`;
- no loops with data-dependent bounds;
- no table search;
- no router/store/UI access;
- no change to D049, D050, D051 or `dinghy.c`.

A fixed four-iteration mapping loop is allowed, but an explicitly unrolled four-element map is preferred if it produces smaller/equal PI32v2 code.

## 13. Resource ceilings

Because D052 is a tiny isolated mapping primitive:

```text
PI32v2 fmt_fm_controls.o text+rodata ceiling = 1024 B
persistent DSP state added                 = 0 B
mapped transient struct                    = 10 B
```

The full target application must remain exact WP5.0 while this source remains unlinked.

If the isolated object exceeds the ceiling, stop and review instead of raising the ceiling casually.

## 14. Required source-absent gate

Before production `fmt_fm_controls.h/.c` exists, CI must prove:

1. this D052 document is frozen;
2. both production control-mapping files are absent;
3. D049, D050, D051 and `dinghy.c` are unchanged from the WP5.3 closeout base;
4. all 256 LEVEL mappings are exact;
5. all 256 INDEX mappings are exact;
6. the 65,536 LEVEL/INDEX overflow surface is exact and signed-32 safe;
7. topology carrier/modulator role sets are disjoint as specified;
8. representative D051 composition vectors are exact;
9. inherited D051, D050, D049 and WP5.0 host regressions remain green.

Only then may production mapping source be added.

## 15. Explicit non-goals

D052 does not define:

- feedback amount or feedback topology, WP5.5;
- envelopes, rates or level evolution, WP5.6;
- FM voice allocation or D048 integration, WP5.7;
- tracker-facing FM parameter IDs/pages/p-locks, WP5.8;
- audible mixed 8-BIT + FM4 integration, WP5.9;
- final factory patch defaults;
- physical timing/audio-quality margin.

No physical FM-1 is available. `hardware_margin_claim=none`.
