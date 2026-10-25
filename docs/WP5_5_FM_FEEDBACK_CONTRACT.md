# WP5.5 Bounded FM4 Feedback Contract

Status: **FROZEN BY D053 BEFORE FM FEEDBACK PRODUCTION SOURCE**  
Date: 2026-10-25  
Package: WP5.5 / issue #48  
Branch: `phase5/fm4-feedback`  
Parent closeout: `d3501eca952f3e903d4dcc4faab2851d439f7ad0`  
Parent decisions: D047-D052

This document is the normative D053 contract for the isolated FM4 feedback layer. It freezes topology, state, control mapping, one-sample ordering, reset/switch behavior, arithmetic bounds, deterministic reference vectors, resource ceilings and implementation boundaries before any `fmt_fm_feedback.*` production source exists.

## 1. Scope

WP5.5 introduces one isolated bounded feedback primitive around the D049 OP4 render surface.

It does not modify or integrate into:

- D049 operator source;
- D050 algorithm source/state;
- D051 ratio source;
- D052 level/index source;
- `dinghy.c`;
- D048 family routing;
- envelopes, WP5.6;
- FM voice allocation/integration, WP5.7;
- tracker parameter IDs/pages, WP5.8;
- audible mixed-family playback, WP5.9.

## 2. Stable topology

D053 V1 has exactly one feedback edge:

```text
OP4[n-1] -> feedback scale -> OP4 phase modulation at sample n
```

The feedback source and destination are both OP4.

There is no runtime operator selector and no per-algorithm feedback topology.

Rationale:

- D050 renders OP4 first in all four algorithms;
- OP4 has no current-sample upstream dependency in any frozen D050 algorithm;
- all four operators still advance once/sample;
- later integration needs only replace the unmodulated OP4 render surface with the D053 wrapper;
- algorithm switches can preserve both D050 phase continuity and D053 feedback history.

## 3. Feedback state

Frozen state:

```c
typedef struct {
    int16_t previous_op4_q15;
} FmtFmFeedbackState;
```

Requirements:

- `sizeof(FmtFmFeedbackState) == 2`;
- exactly one prior level-scaled OP4 sample;
- no algorithm ID;
- no control value cached in state;
- no second sample/history average;
- no pointer, heap ownership or hidden mutable global state.

D049 output is `-32767..32767`, so the prior OP4 output fits losslessly in `int16_t`.

Reset and retrigger set `previous_op4_q15 = 0`.

## 4. Stable FEEDBACK control

D053 introduces one distinct internal feedback control:

```text
FEEDBACK = 0..255
```

Malformed scalar values above 255 saturate to 255. There is no wrap.

The exact internal Q14 depth mapping is:

```text
v = min(control, 255)
feedback_depth_q14 = (v << 6) | (v >> 2)
```

This intentionally equals D052's byte-to-Q14 INDEX expansion for every legal byte, but FEEDBACK is a separate semantic parameter and does not reuse or reinterpret D052 INDEX.

Frozen anchors:

| FEEDBACK | Q14 depth |
|---:|---:|
| 0 | 0 |
| 1 | 64 |
| 64 | 4112 |
| 128 | 8224 |
| 192 | 12336 |
| 254 | 16319 |
| 255 | 16383 |
| >255 | 16383 |

The 256-value little-endian uint16 payload SHA-256 is:

```text
cf791163cad612063084d0479cb00b8b86e4e2d19102c461bf548bcca4b69bc7
```

## 5. Exact feedback phase arithmetic

For prior sample `p` and mapped depth `d`:

```text
feedback_phase_modulation = p * d * 4
```

The result is signed 32-bit phase units and is passed directly to D049 `phase_modulation`.

For every valid D049 history value and every legal FEEDBACK byte, arithmetic is signed-32 safe.

Maximum positive legal magnitude:

```text
32767 * 16383 * 4 = 2147287044
INT32_MAX margin = 196603
```

D053 additionally requires safety for the entire signed-16 state domain, including corrupted `-32768`:

```text
-32768 * 16383 * 4 = -2147352576
INT32_MIN margin = 131072
```

Exhaustive reference surface:

```text
history values = -32768..32767
FEEDBACK values = 0..255
pairs = 16,777,216
serialized result = little-endian int32
SHA-256 = 6eff2600842fa460d7663f025d0f9d13fc1176deabfcb3aa39e22e5fbdb852af
```

D051 ratios cannot enlarge feedback arithmetic because D049 output remains Q15 bounded independently of phase increment.

## 6. One-sample ordering

For one OP4 feedback render at sample `n`:

```text
1. read previous_op4_q15 from D053 state
2. map FEEDBACK byte to Q14 depth
3. calculate previous_sample * depth * 4
4. call D049 OP4 render exactly once with that signed phase modulation
5. receive current level-scaled OP4 sample
6. store current sample as previous_op4_q15
7. return current sample
```

The current output never feeds itself in the same sample. The delay is exactly one sample.

Feedback changes only D049 sample phase. It does not alter the carrier phase accumulator beyond D049's normal `phase += phase_increment` advance.

## 7. Zero and history-tracking behavior

`FEEDBACK = 0` produces phase modulation exactly 0.

History still updates to every current OP4 output while FEEDBACK is zero. Therefore raising FEEDBACK later uses the immediately preceding real OP4 sample rather than an artificially cleared history.

This is frozen behavior.

## 8. Reset, retrigger and algorithm-switch behavior

Reset/retrigger:

```text
previous_op4_q15 -> 0
```

The first OP4 render after feedback reset is therefore identical to an unmodulated D049 OP4 render for the same phase/increment/level.

Algorithm changes do **not** reset D053 history.

Rationale:

- D050 does not reset operator phases on algorithm change;
- OP4 renders first and once/sample in all algorithms;
- retaining OP4 history preserves one continuous oscillator/feedback process while only downstream routing changes.

Later integration must reset D053 state when the FM voice itself is reset/retriggered, but must not reset D053 merely because D050 algorithm ID changes.

## 9. Null behavior

Frozen fail-safe behavior for the intended production API:

- reset/retrigger with null feedback state: no-op;
- phase-modulation query with null feedback state: return 0;
- OP4 wrapper with null OP4 pointer: return 0 and do not mutate feedback history;
- OP4 wrapper with null feedback state but valid OP4: perform exactly one ordinary D049 render with zero phase modulation, so OP4 phase still advances normally.

## 10. Intended production API

The intended D053 public boundary is:

```c
#define FMT_FM_FEEDBACK_CONTROL_MAX 255u

typedef struct {
    int16_t previous_op4_q15;
} FmtFmFeedbackState;

void fmt_fm_feedback_reset(FmtFmFeedbackState *state);
void fmt_fm_feedback_retrigger(FmtFmFeedbackState *state);
uint8_t fmt_fm_feedback_sanitize(uint32_t control);
uint16_t fmt_fm_feedback_depth_q14(uint32_t control);
int32_t fmt_fm_feedback_phase_modulation(const FmtFmFeedbackState *state,
                                         uint32_t feedback_control);
int32_t fmt_fm_feedback_render_op4(FmtFmFeedbackState *state,
                                   FmtFmOperator *op4,
                                   uint32_t phase_increment,
                                   uint32_t level_q15,
                                   uint32_t feedback_control);
```

The wrapper calls D049 exactly once. It does not call D050 or inspect an algorithm ID.

## 11. Canonical OP4 fixture

Use the D050 OP4 fixture:

```text
initial phase    0xD0E0F000
phase increment  0x0456789A
level_q15        18000
FEEDBACK         192 -> depth 12336
initial history  0
```

First six causal steps are frozen:

| n | history before | phase modulation | output/history after |
|---:|---:|---:|---:|
| 0 | 0 | 0 | -16477 |
| 1 | -16477 | -813041088 | -14123 |
| 2 | -14123 | -696885312 | -16632 |
| 3 | -16632 | -820689408 | -16072 |
| 4 | -16072 | -793056768 | -17085 |
| 5 | -17085 | -843042240 | -17264 |

First 12 outputs:

```text
-16477, -14123, -16632, -16072, -17085, -17264,
-17664, -17880, -17994, -17951, -17698, -17154
```

After 4,096 renders:

```text
OP4 phase = 0x386A9000
history = 13151
output SHA-256 = 6eee497f61dec5556882cc7e66ef0b493529dcf1ff56d8e06193886eaa792e49
```

## 12. Test-only D050 composition fixture

WP5.5 production source does not modify D050. The D053 source-absent and implementation tests use a test-only composition model where the four D050 algorithms are unchanged except that their first OP4 render uses the D053 one-sample feedback rule.

Use the frozen D050 fixture:

```text
initial phases:
OP1 0x10203040
OP2 0x50607080
OP3 0x90A0B0C0
OP4 0xD0E0F000

increments:
OP1 0x01234567
OP2 0x02345678
OP3 0x03456789
OP4 0x0456789A

levels: 32767, 28000, 22000, 18000
D050 global mod depth: 7000
D053 FEEDBACK: 192
initial feedback history: 0
```

Frozen first 12 outputs:

STACK4:

```text
31709, 32339, 32237, 32501, 32563, 32671,
32732, 32764, 32759, 32710, 32608, 32444
```

FAN3:

```text
13322, 14313, 9242, 7971, 4630, 2391,
-101, -2283, -4239, -5886, -7139, -7896
```

DUAL2:

```text
19166, 17261, 17505, 16364, 15924, 15103,
14378, 13571, 12724, 11818, 10843, 9786
```

PARALLEL4:

```text
3298, 3541, 2564, 2352, 1749, 1357,
917, 531, 183, -113, -341, -480
```

Frozen 4,096-output SHA-256 values:

| algorithm | SHA-256 |
|---|---|
| STACK4 | `25f86f13f10413f5be3ff4b50160cafeac97d758933fb5d660225525a4eb9077` |
| FAN3 | `02e8b4556fb26a4134a6c257f1030b03c6808b8da2e597351fd19ef6ba9920fa` |
| DUAL2 | `f23d8e23e09ee1f4c4fd5d7247c125627b0624ac98fceb9bf98c8b1a6ff55a93` |
| PARALLEL4 | `60ff90a19ccfc517213aa4fcce1a209240d546bbd3db5eecde9c695612501050` |

All four finish with the same D050 phases and the same D053 OP4 history:

```text
OP1 0x4476A040
OP2 0x95C7F080
OP3 0xE71940C0
OP4 0x386A9000
feedback history = 13151
```

## 13. Algorithm-switch continuity fixture

Render 4,096 samples continuously from the Section 12 initial state with:

```text
samples 0..1023    STACK4
samples 1024..2047 FAN3
samples 2048..3071 DUAL2
samples 3072..4095 PARALLEL4
```

Do not reset operator phase or feedback history at switches.

Frozen output SHA-256:

```text
5ba3042bc21eacccc103de9da579aa1b5a4db2d78aa5541130b66b2fce75ac67
```

A test-only model that incorrectly clears feedback history at each algorithm switch produces a different SHA and must be rejected:

```text
51f0959b5e6840ed9dacc2f56eb9c2ea8c501ef5cb32080d0290a181e1e235aa
```

## 14. D051/D052 extreme stress

The source-absent reference gate must run at least 65,536 samples per D050 algorithm using the previously frozen D051/D052 prepared extreme vector:

```text
increments = 407681904, 1223045711, 2147483647, 2147483647
levels     = 8224, 16448, 24672, 32767
D050 INDEX depth = 16383
D053 FEEDBACK = 255 -> 16383
initial phases = 0,0,0,0
initial feedback history = 0
```

Frozen 65,536-output SHA-256 values:

| algorithm | SHA-256 |
|---|---|
| STACK4 | `d91af85e62f5791881a7b2b3bc560228ab820268c3fe7ca0762f9ae6d6922c0d` |
| FAN3 | `1037755115ed94cc460da484851a8f86fc49f8d19b6b9e76f55fa00c9bd65406` |
| DUAL2 | `9da3125c952f75280296266236fd7a61afe93f6816354ce145d4d433fc0c6e46` |
| PARALLEL4 | `ed1c7ae3b47c83773e455757267da3692454d988e6c9e01dcba09acd02ea8d7a` |

Final feedback history is `2517` for all four algorithms.

Final phases are:

```text
OP1 0xBB700000
OP2 0x324F0000
OP3 0xFFFF0000
OP4 0xFFFF0000
```

Every final output must remain in D049 Q15 range `-32767..32767`.

## 15. Realtime/source bounds

D053 production feedback work is O(1).

Per OP4 render it may add at most:

- one feedback control sanitize;
- one fixed bit-expansion to Q14;
- one signed 32-bit multiply by depth;
- one multiply-by-four operation;
- one D049 render call;
- one 16-bit history store.

Forbidden:

- loops or searches;
- graph traversal;
- recursion;
- heap allocation;
- locks/queues/store/song/UI access;
- runtime `/` or `%`;
- int64 arithmetic;
- float/double;
- tables for feedback mapping;
- more than one stored feedback sample.

## 16. Production boundary

After the source-absent gate passes, WP5.5 may add only:

```text
firmware/src/fmt_fm_feedback.h
firmware/src/fmt_fm_feedback.c
```

D049, D050, D051, D052, generated sine data and `dinghy.c` must remain unchanged through WP5.5.

The feedback primitive remains isolated from the full application. Test-only composition with D050 is allowed and required.

## 17. Resource contract

Persistent D053 state:

```text
2 B per future FM voice
```

Future D050 + D053 synthesis state before envelopes:

```text
16 B D050 operator phases + 2 B feedback = 18 B
```

Pinned PI32v2 isolated `fmt_fm_feedback.o` ceiling:

```text
text + rodata <= 1024 B
```

No feedback table payload is permitted.

Because D053 remains unlinked from `dinghy.c` in WP5.5, full-app closeout must remain exact WP5.0:

```text
image       72792 B
normal RAM  19676 B
pool RAM   113000 B
identity   FM-1_987
BIN SHA    a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125
FWSC SHA   cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073
```

## 18. Exit gate before implementation

Before `fmt_fm_feedback.h/.c` may exist, CI must prove:

- this D053 contract is present and frozen;
- both feedback production files are absent;
- D049-D052, generated sine and `dinghy.c` are unchanged from the WP5.4 closeout;
- exhaustive 256-value feedback mapping is exact;
- exhaustive 65,536 signed-16 history values x 256 feedback values is signed-32 safe and matches the frozen SHA;
- D049/D050 no-feedback reference hashes remain exact;
- canonical OP4 feedback fixture is exact;
- all four test-only feedback algorithm hashes are exact;
- algorithm-switch history continuity hash is exact;
- four 65,536-sample extreme stress hashes are exact;
- inherited WP5.4 host gate remains exact;
- `hardware_margin_claim=none`.

Only a green source-absent D053 gate authorizes production feedback source.
