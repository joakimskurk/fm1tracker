# WP5.2 FM4 Algorithm Routing Contract

Status: **FROZEN BY D050 BEFORE FM ALGORITHM PRODUCTION SOURCE**  
Date: 2026-10-24  
Package: WP5.2 / issue #45  
Phase 5 branch: `phase5/fm4-algorithms`  
Parent architecture: D047 / D048  
Primitive dependency: D049 / `WP5_1_FM_OPERATOR_CONTRACT.md`

This document is the normative contract for the isolated four-operator FM routing layer. It freezes the curated algorithm set, operator numbering, per-sample evaluation order, internal modulation scaling, carrier normalization, state and deterministic reference evidence before any `fmt_fm_algorithm.*` production source is created.

WP5.2 remains isolated from `dinghy.c`. It does not make FM audible in the running application.

## 1. Scope

WP5.2 adds exactly one synthesis layer above D049:

```text
four D049 operators
    -> fixed curated topology
    -> deterministic modulation routing
    -> bounded carrier sum
    -> one Q15 output sample
```

WP5.2 does not decide or implement:

- note-to-operator frequency ratios or tuning;
- final user-facing operator level or modulation-index mappings;
- feedback;
- envelopes;
- FM voice allocation;
- D048 family-router integration;
- tracker parameter IDs/pages;
- audible mixed-family FM playback.

Those remain WP5.3 through WP5.9 work.

## 2. Design rule - curated fixed switch, not a graph engine

D050 rejects a runtime routing matrix, graph interpreter, node list or dynamic connection registry.

The production primitive will use one bounded four-case dispatch with explicitly unrolled operator evaluation.

Reasons:

- exact per-sample cost is visible in source;
- no traversal or dynamic topology validation is needed in the audio path;
- all four operators can advance exactly once per sample;
- later algorithm changes can preserve phase continuity;
- four topologies cover the main musical shapes required before adding ratios, feedback and envelopes.

The stable algorithm count in D050 is exactly:

```text
FMT_FM_ALGORITHM_COUNT = 4
```

New algorithms may only be appended by a later explicit decision. IDs `0..3` never change meaning.

## 3. Operator numbering

Operator array order is:

```text
index 0 = OP1
index 1 = OP2
index 2 = OP3
index 3 = OP4
```

Signal-flow diagrams use arrows from modulator to destination.

All algorithms evaluate operators in this fixed chronological order:

```text
OP4 -> OP3 -> OP2 -> OP1
```

This is evaluation order, not always signal topology.

Every valid or sanitized algorithm renders **all four D049 operators exactly once per output sample**.

Consequences:

- every operator phase advances once per sample;
- switching algorithm does not itself reset or pause any operator phase;
- no algorithm has hidden lower oscillator cost through skipped operators;
- later realtime budgeting gets one stable four-operator baseline.

## 4. Stable algorithm IDs and topology

### Algorithm 0 - STACK4

Stable ID:

```text
FMT_FM_ALGO_STACK4 = 0
```

Topology:

```text
OP4 -> OP3 -> OP2 -> OP1 -> OUT
```

Carrier count: 1, OP1.

Evaluation:

1. render OP4 unmodulated;
2. convert OP4 output to phase modulation and render OP3;
3. convert OP3 output to phase modulation and render OP2;
4. convert OP2 output to phase modulation and render OP1;
5. output OP1 directly.

### Algorithm 1 - FAN3

Stable ID:

```text
FMT_FM_ALGO_FAN3 = 1
```

Topology:

```text
OP2 --\
OP3 ---> OP1 -> OUT
OP4 --/
```

Carrier count: 1, OP1.

Evaluation:

1. render OP4 unmodulated;
2. render OP3 unmodulated;
3. render OP2 unmodulated;
4. convert OP4, OP3 and OP2 outputs to phase offsets;
5. add those offsets modulo one 32-bit phase cycle;
6. render OP1 with the accumulated phase modulation;
7. output OP1 directly.

### Algorithm 2 - DUAL2

Stable ID:

```text
FMT_FM_ALGO_DUAL2 = 2
```

Topology:

```text
OP4 -> OP3 --\
              +-> normalized OUT
OP2 -> OP1 --/
```

Carrier count: 2, OP3 and OP1.

Evaluation:

1. render OP4 unmodulated;
2. convert OP4 output to phase modulation and render OP3;
3. render OP2 unmodulated;
4. convert OP2 output to phase modulation and render OP1;
5. sum OP3 + OP1;
6. normalize two carriers by exact power-of-two scaling toward zero.

### Algorithm 3 - PARALLEL4

Stable ID:

```text
FMT_FM_ALGO_PARALLEL4 = 3
```

Topology:

```text
OP4 --\
OP3 ---\
OP2 ----+-> normalized OUT
OP1 ---/
```

Carrier count: 4.

Evaluation:

1. render OP4 unmodulated;
2. render OP3 unmodulated;
3. render OP2 unmodulated;
4. render OP1 unmodulated;
5. sum all four outputs;
6. normalize four carriers by exact power-of-two scaling toward zero.

PARALLEL4 is intentionally additive rather than phase-modulated. It gives the compact FM4 family a clean multi-oscillator endpoint without a separate additive engine.

## 5. Algorithm state

The routing layer owns no mutable history beyond the four D049 operator phases.

Frozen state:

```c
typedef struct {
    FmtFmOperator op[4];
} FmtFmAlgorithmState;
```

Required:

- `sizeof(FmtFmAlgorithmState) == 16`;
- no cached previous operator outputs;
- no feedback sample;
- no envelope state;
- no ratio/frequency state;
- no algorithm field inside mutable state;
- no heap/pointers/ownership metadata.

Reset and retrigger reset all four D049 operators to phase zero.

Feedback history is explicitly deferred to WP5.5.

## 6. Render inputs

The intended isolated API receives already-resolved numeric inputs:

```c
int32_t fmt_fm_algorithm_render(FmtFmAlgorithmState *state,
                                uint8_t algorithm,
                                const uint32_t phase_increment[4],
                                const uint16_t level_q15[4],
                                uint16_t mod_depth_q14);
```

Meaning:

- `phase_increment[0..3]` is one D049 phase increment per operator;
- WP5.3 later decides how note pitch and operator ratios produce these increments;
- `level_q15[0..3]` is one already-resolved D049 operator level per operator;
- D049 still clamps each value to `0..32767`;
- WP5.4 later decides user-facing level/mod-index mappings;
- `mod_depth_q14` is an internal global edge-depth value for routing only;
- D050 freezes its numeric domain, not its future UI mapping.

Null behavior:

- null `state`, `phase_increment` or `level_q15` returns 0;
- null failure mutates no operator phase.

## 7. Internal modulation-depth domain

Frozen domain:

```text
0..16383
```

Values above 16383 clamp to 16383.

The internal name is `mod_depth_q14`. This is not yet a user control scale.

For one D049 operator sample `s` in `-32767..32767`:

```text
d = clamp(mod_depth_q14, 0, 16383)
edge_phase_offset = s * d * 4
```

All arithmetic is signed 32-bit and safe by construction.

Exact maximum magnitude:

```text
32767 * 16383 * 4 = 2147287044
```

which remains below `INT32_MAX = 2147483647`.

Frozen positive full-scale anchors:

| depth | edge phase offset |
|---:|---:|
| 0 | 0 |
| 1 | 131068 |
| 4096 | 536854528 (`0x1fffc000`) |
| 8192 | 1073709056 (`0x3fff8000`) |
| 16383 | 2147287044 (`0x7ffd0004`) |
| 65535 input | clamps to 2147287044 |

Negative D049 samples must produce the exact sign mirror.

This internal scale allows a full-scale modulator to approach a half-cycle peak phase offset at maximum depth without 64-bit math. WP5.4 may map user controls onto this domain but may not silently change D050 routing arithmetic.

## 8. Multiple-modulator accumulation

FAN3 requires three phase offsets to reach OP1.

Phase offsets are periodic quantities. D050 therefore chooses **modulo 2^32 accumulation**, not signed saturation.

For signed offsets `a` and `b`:

```text
bits = (uint32)a + (uint32)b
result = the exact signed int32 value represented by those 32 bits
```

The implementation must convert the bit pattern back to signed form without relying on an implementation-defined out-of-range unsigned-to-signed cast.

Frozen wrap anchors:

```text
INT32_MAX + 1       -> INT32_MIN
INT32_MIN + (-1)    -> INT32_MAX
0x70000000 + 0x30000000 -> 0xA0000000 interpreted as -1610612736
```

Modulo accumulation is mathematically appropriate because phase offsets differing by a whole 32-bit cycle address the same carrier phase.

## 9. Carrier sum and normalization

D049 operator outputs are individually bounded to `-32767..32767`.

D050 only uses carrier counts 1, 2 or 4 so output normalization requires no divide.

### One carrier

Return the carrier sample unchanged.

### Two carriers

```text
sum = carrier_a + carrier_b      // range -65534..65534
output = sign_safe_truncate_toward_zero(sum / 2)
```

Implementation uses magnitude + right shift, not runtime division and not implementation-defined signed right shift.

Frozen anchors:

```text
-65534 -> -32767
-3     -> -1
-1     -> 0
 1     -> 0
 3     -> 1
 65534 -> 32767
```

### Four carriers

```text
sum = c1 + c2 + c3 + c4         // range -131068..131068
output = sign_safe_truncate_toward_zero(sum / 4)
```

Frozen anchors:

```text
-131068 -> -32767
-3      -> 0
-1      -> 0
 1      -> 0
 3      -> 0
 131068 -> 32767
```

Thus every algorithm output remains inside the D049 nominal Q15 domain without limiter state or clipping.

## 10. Invalid algorithm behavior

Algorithm values `0..3` are valid.

Every other value sanitizes deterministically to:

```text
FMT_FM_ALGO_STACK4 = 0
```

No invalid value may index a table, skip operator advancement or produce uninitialized routing.

The sanitize helper is part of the D050 compatibility contract.

## 11. Algorithm changes and phase continuity

The routing primitive stores no algorithm in state and resets no phase when the requested algorithm changes.

Because all four operators render exactly once per sample in every algorithm:

- an algorithm change changes only current signal connections;
- all four phase accumulators continue monotonically according to their supplied increments;
- no hidden retrigger occurs;
- later tracker/parameter packages may decide *when* an algorithm value becomes visible, but not change this primitive continuity rule without superseding D050.

## 12. Exact reference fixture

The D050 source-absent reference model uses this non-musical deterministic fixture. These increments are test constants only and do not establish WP5.3 ratio policy.

Initial OP1..OP4 phases:

```text
0x10203040
0x50607080
0x90A0B0C0
0xD0E0F000
```

OP1..OP4 phase increments:

```text
0x01234567
0x02345678
0x03456789
0x0456789A
```

OP1..OP4 D049 levels:

```text
32767, 28000, 22000, 18000
```

Global internal modulation depth:

```text
7000
```

### First 12 output samples

STACK4 / ID 0:

```text
31709, 32144, 32478, 32686, 32765, 32732,
32615, 32445, 32245, 32031, 31808, 31583
```

FAN3 / ID 1:

```text
13322, 12487, 11858, 11434, 11223, 11214,
11408, 11786, 12337, 13043, 13887, 14841
```

DUAL2 / ID 2:

```text
19166, 17927, 16580, 15148, 13662, 12161,
10686, 9284, 8003, 6886, 5971, 5288
```

PARALLEL4 / ID 3:

```text
3298, 3168, 3078, 3028, 3017, 3043,
3106, 3203, 3331, 3488, 3670, 3874
```

## 13. Frozen 4,096-sample algorithm hashes

Using the exact fixture in Section 12 continuously for 4,096 renders from the initial state, serialize each signed output as little-endian int32.

Frozen SHA-256 values:

| algorithm | SHA-256 |
|---|---|
| STACK4 | `25dfde1bb29de2c70305d6abafee91fd7480e69eaa71fd1a3fbde3c1e6e855cd` |
| FAN3 | `8d9ae5310369e3d5a36bcf8c44d5e74049d3d6e109362a844be2796b26330653` |
| DUAL2 | `c4c31cabf4b8e121591c4c1e0923aee67523eaf7d5b60b60ca1a545c13fc2269` |
| PARALLEL4 | `d316c3e5467cbfc606c788f669a3b19dba13535969dcce3b67b2821f0977a882` |

All four algorithms must finish with identical phases because every operator advances once per sample:

```text
OP1 = 0x4476A040
OP2 = 0x95C7F080
OP3 = 0xE71940C0
OP4 = 0x386A9000
```

This phase-equality invariant is part of D050.

## 14. Broad reference stress

The pre-implementation reference gate must additionally exercise at least 65,536 renders per algorithm with deterministic varying:

- operator increments;
- operator levels including values above 32767 so D049 clamps are exercised;
- modulation-depth values across the uint16 domain so D050 clamp behavior is exercised.

It must prove:

- every output remains `-32767..32767`;
- each operator phase after every render equals old phase + supplied increment modulo 2^32;
- no algorithm skips or double-advances an operator;
- invalid algorithm IDs reproduce STACK4 output and state exactly.

The stress pattern is verification data only and does not define musical ratio/control mappings.

## 15. Intended production API

The intended public boundary is:

```c
#define FMT_FM_ALGORITHM_COUNT 4u
#define FMT_FM_MOD_DEPTH_MAX_Q14 16383u

#define FMT_FM_ALGO_STACK4    0u
#define FMT_FM_ALGO_FAN3      1u
#define FMT_FM_ALGO_DUAL2     2u
#define FMT_FM_ALGO_PARALLEL4 3u

typedef struct {
    FmtFmOperator op[4];
} FmtFmAlgorithmState;

void fmt_fm_algorithm_reset(FmtFmAlgorithmState *state);
void fmt_fm_algorithm_retrigger(FmtFmAlgorithmState *state);
uint8_t fmt_fm_algorithm_sanitize(uint32_t algorithm);
uint16_t fmt_fm_algorithm_clamp_mod_depth_q14(uint32_t depth);
int32_t fmt_fm_algorithm_render(FmtFmAlgorithmState *state,
                                uint8_t algorithm,
                                const uint32_t phase_increment[4],
                                const uint16_t level_q15[4],
                                uint16_t mod_depth_q14);
```

The exact source names may only change before implementation if the contract and reference gate are updated together. Semantics may not drift silently.

## 16. Realtime bounds

One valid render performs exactly:

- four D049 operator renders;
- zero dynamic loops or graph traversal;
- at most three edge modulation conversions;
- at most two modular phase-offset additions;
- one carrier sum and one fixed power-of-two normalization for multi-carrier algorithms;
- no heap/allocation;
- no recursion;
- no runtime divide/modulo;
- no float/double;
- no 64-bit arithmetic;
- no song/store/p-lock/router/UI/persistence access.

All work is O(1) with a four-case static dispatch.

Host wall-clock timing is diagnostic only and is not AC791N callback-margin evidence.

## 17. WP5.2 isolated resource contract

WP5.2 remains unlinked from `dinghy.c`.

Therefore the full application must remain exact WP5.0/WP5.1 identity:

- image 72,792 B;
- normal RAM 19,676 B;
- pool 113,000 B;
- normal WAV SHA-256 `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`;
- BIN SHA-256 `a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125`;
- FWSC SHA-256 `cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073`.

The routing state itself is exactly 16 B when instantiated later.

The isolated `fmt_fm_algorithm.o` target object ceiling is:

```text
.text + immutable .rodata <= 2048 B
```

Expected static mutable/global RAM in the isolated module is 0 B.

D049's 692 B operator object is a dependency baseline, not charged again against this router-only 2,048 B ceiling.

## 18. Required pre-implementation gate

Before production `fmt_fm_algorithm.h/.c` exists, CI must prove:

1. D050 contract is committed;
2. production algorithm source/header are absent;
3. D049 operator source/data are unchanged from WP5.1 closeout;
4. all four stable IDs/topologies are reproduced by a host-only reference model;
5. all modulation-depth anchors pass;
6. modular phase-offset wrap anchors pass;
7. carrier normalization anchors pass;
8. all first-12 fixture vectors pass;
9. all four 4,096-sample SHA-256 values and final phases pass;
10. broad 65,536-render-per-algorithm stress passes;
11. invalid IDs reproduce STACK4 exactly;
12. D049 production vectors remain green;
13. full normal WP5.0 WAV remains byte-identical.

Only after this gate is green may production FM algorithm-routing source be added.

## 19. Required production verification

After implementation, WP5.2 must prove:

- `FmtFmAlgorithmState == 16 B`;
- reset/retrigger/null safety;
- algorithm sanitizer exactness;
- mod-depth clamp exactness;
- exact graph/evaluation order for all four algorithms;
- every operator advances once per sample;
- algorithm switching preserves phase continuity;
- exact first-12 vectors;
- exact 4,096-render hashes/final phases;
- broad deterministic stress;
- no loop/heap/float/int64/divide/modulo/unbounded traversal in render;
- pinned PI32v2 compile;
- router object `.text + .rodata <= 2048 B`;
- D049 regression remains exact;
- full WP5.0 target/WAV/BIN/FWSC remains exact because routing is still isolated.

## 20. Exit condition

WP5.2 may close only when:

- D050 existed and passed source-absent reference CI before production routing source;
- production routing matches D050 exactly;
- all deterministic host and target gates pass;
- exact object size is recorded and accepted;
- `WP5_2_VERIFICATION.md` records durable evidence;
- no WP5.3+ ratio, level mapping, feedback, envelope, voice integration or parameter-page implementation has been pulled forward.

After that, WP5.3 ratio-table work becomes the only active Phase 5 package.

No physical FM-1 is available. `hardware_margin_claim=none`.
