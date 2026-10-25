# WP5.6 FM4 Envelope Contract - D054

Status: **FROZEN BEFORE PRODUCTION ENVELOPE SOURCE**  
Date: 2026-10-25  
Package: WP5.6 / issue #49  
Branch: `phase5/fm4-envelope`  
Parent closeout: WP5.5 / D053 `216c7e34b6d761cadb683bd1fc356c216b98cc27`

This is the normative D054 contract for the isolated FM4 envelope layer. Production envelope source is forbidden until the source-absent D054 gate passes and durable preimplementation evidence is recorded.

## 1. Scope

D054 adds four independent operator envelopes before D049/D050 rendering:

```text
D052 static OPn LEVEL Q15
          x
D054 OPn envelope Q15
          |
          v
D054 effective OPn LEVEL Q15
          |
          v
D049 operator -> D050 topology
```

D049-D053 remain unchanged.

D054 does not integrate FM voices into D048, does not allocate voices, does not define tracker pages/p-lock IDs and does not make FM audible in the running application.

## 2. Architecture decision

D054 freezes **four independent per-operator ADSR envelopes**.

Rejected alternatives:

- one post-D050 output envelope: only final loudness, insufficient FM timbral evolution;
- one common pre-operator envelope: bounded but locks every operator to one contour and cannot express independent modulator/carrier transients.

Per-operator ADSR is the smallest V1 architecture that clearly supports a short modulator transient over a sustained carrier, slow modulator swells and algorithm-independent operator identity.

## 3. Stable controls

Each operator owns four stable unsigned-byte controls:

```text
ATTACK  0..255
DECAY   0..255
SUSTAIN 0..255
RELEASE 0..255
```

Four operators therefore require exactly 16 control bytes.

Candidate public control layout is frozen as four four-byte arrays or an exactly 16 B equivalent. No control may silently widen persistent instrument state in WP5.6.

## 4. Envelope state

D054 persistent runtime state is exactly:

```c
uint32_t level_q15_16[4];
uint8_t  stage[4];
```

Exact size:

```text
FmtFmEnvelopeState = 20 B
```

Stage IDs are stable:

```text
OFF     = 0
ATTACK  = 1
DECAY   = 2
SUSTAIN = 3
RELEASE = 4
```

Unknown/corrupt stage values fail to `OFF` with zero level on the next advance.

D050 + D053 + D054 synthesis state before voice allocator metadata is therefore:

```text
D050 operator phases  16 B
D053 feedback          2 B
D054 envelopes        20 B
--------------------------
subtotal              38 B
```

## 5. Level representation

Internal envelope level is unsigned Q15.16:

```text
0x00000000 = zero
0x7fff0000 = full scale
```

Frozen constant:

```text
FMT_FM_ENVELOPE_LEVEL_MAX_Q15_16 = 0x7fff0000
```

The render-facing Q15 envelope level is:

```text
env_q15 = min(level_q15_16, 0x7fff0000) >> 16
```

Thus the exact legal render domain is `0..32767`.

## 6. SUSTAIN mapping

SUSTAIN uses the exact same byte-to-Q15 mapping already frozen for D052 LEVEL:

```text
v = clamp(control, 0, 255)
sustain_q15 = (v << 7) | (v >> 1)
sustain_q15_16 = sustain_q15 << 16
```

Frozen 256-value little-endian uint16 mapping SHA-256:

```text
a799f70bd763053e79c3d9de6acdda97523fe54632105756c4252dc33d51c815
```

Anchors:

```text
0   -> 0
1   -> 128
127 -> 16319
128 -> 16448
255 -> 32767
```

## 7. ATTACK / DECAY / RELEASE time mapping

The V1 FM sample domain is 44,100 samples/s, consistent with the accepted D038 phase-increment domain.

Control `0` means **instant transition** and has table step `0`.

Controls `1..255` use an exact integer 16-steps-per-octave time law.

For a control `c`:

```text
x      = c - 1
octave = x >> 4
frac   = x & 15

target_samples = ((44 * (16 + frac) * (1 << octave)) + 8) >> 4

step_q15_16 = ceil(0x7fff0000 / target_samples)
            = (0x7fff0000 + target_samples - 1) / target_samples
```

The division above is **offline generation only**. Production render code performs a direct table lookup and no division.

The production table is exactly 256 little-endian uint32 entries, 1,024 B.

Frozen table payload SHA-256:

```text
6e94aa6553b78919913d49a24c80ca7317c913fff344294a107137f85ffbc025
```

Selected anchors:

| control | target samples | target ms at 44.1 kHz | step Q15.16 | actual full-scale steps |
|---:|---:|---:|---:|---:|
| 0 | 0 | instant | 0 | 0 |
| 1 | 44 | 0.9977 | 48804958 | 44 |
| 16 | 85 | 1.9274 | 25263743 | 85 |
| 32 | 171 | 3.8776 | 12558001 | 171 |
| 64 | 682 | 15.4649 | 3148707 | 682 |
| 96 | 2728 | 61.8594 | 787177 | 2728 |
| 127 | 10560 | 239.4558 | 203354 | 10560 |
| 128 | 10912 | 247.4376 | 196795 | 10912 |
| 160 | 43648 | 989.7506 | 49199 | 43648 |
| 192 | 174592 | 3959.0023 | 12300 | 174587 |
| 224 | 698368 | 15836.0091 | 3075 | 698348 |
| 254 | 2613248 | 59257.3243 | 822 | 2612431 |
| 255 | 2703360 | 61300.6803 | 795 | 2701155 |

All 255 finite controls must have unique, monotonically non-increasing step values.

## 8. Static LEVEL x envelope LEVEL composition

Both inputs sanitize to `0..32767`.

Frozen effective-level rule:

```text
if static_q15 == 0 or env_q15 == 0:
    out = 0
else if env_q15 == 32767:
    out = static_q15
else:
    out = (static_q15 * env_q15 + 16384) >> 15
```

The full-envelope special case guarantees that an envelope at maximum preserves the D052 operator LEVEL exactly.

The 32-bit product is safe:

```text
32767 * 32766 + 16384 < 2^31
```

Exhaustive 256 D052 LEVEL controls x 256 envelope-byte levels, serialized as little-endian uint16 outputs in static-major order, has frozen SHA-256:

```text
196659105845f6183b0779183a583226129a5b7f35e8ac8ac811479b117e606b
```

## 9. Reset and retrigger

`reset`:

- all four levels = 0;
- all four stages = OFF.

V1 note retrigger is **hard retrigger from zero**.

For each operator on retrigger:

- if ATTACK > 0: level = 0, stage = ATTACK;
- if ATTACK = 0 and DECAY > 0: level = full scale, stage = DECAY;
- if ATTACK = 0 and DECAY = 0: level = exact SUSTAIN target, stage = SUSTAIN.

This gives true zero-time ATTACK/DECAY without loops.

A null controls pointer on retrigger fails to reset/off silence.

## 10. Note-off / RELEASE

For each non-OFF operator:

- RELEASE = 0 -> level = 0 and stage = OFF immediately;
- RELEASE > 0 and level > 0 -> stage = RELEASE with current level preserved;
- already zero/OFF remains zero/OFF.

A null controls pointer on note-off fails to reset/off silence.

Release therefore starts from the current envelope level and requires no release-start snapshot or division.

## 11. Sample ordering

D054 freezes the future WP5.7 call order:

1. apply note/retrigger/note-off event before rendering the sample;
2. read current D054 envelope Q15 levels;
3. combine them with D052 static Q15 levels;
4. render D049/D050/D053 with those effective levels;
5. advance D054 exactly once after that rendered sample.

Consequences:

- hard retrigger with finite ATTACK renders the first sample at zero;
- ATTACK=0, DECAY>0 renders the first sample at full scale;
- ATTACK=0, DECAY=0 renders the first sample at SUSTAIN;
- RELEASE=0 silences the first sample after note-off;
- finite release renders the note-off sample at its current level, then begins stepping down.

## 12. Advance semantics

Each operator advances once and only once per sample.

OFF:

- level forced to zero.

ATTACK:

- if ATTACK becomes zero, set full-scale and stage DECAY;
- otherwise add the frozen table step;
- use a pre-subtraction boundary check so uint32 addition cannot overflow;
- clamp exactly to full scale and enter DECAY when the next step reaches/crosses full scale.

DECAY:

- target is current SUSTAIN Q15.16;
- if level <= target or DECAY is zero, set target exactly and enter SUSTAIN;
- otherwise subtract the DECAY step;
- clamp exactly to target and enter SUSTAIN on crossing.

SUSTAIN:

- level is forced exactly to the current SUSTAIN target each advance.

RELEASE:

- RELEASE zero or level zero -> zero/OFF;
- otherwise subtract frozen RELEASE step;
- clamp exactly to zero/OFF on crossing.

No same-sample while-loop stage traversal is allowed. Each operator executes at most one stage branch per `advance` call. Retrigger resolves zero-time ATTACK/DECAY explicitly before the first render.

## 13. Algorithm/control continuity

D050 algorithm changes do not reset or alter D054 state.

D052 static LEVEL/INDEX changes do not reset D054 state.

D053 feedback changes do not reset D054 state.

SUSTAIN changes while in SUSTAIN become exact on the next post-render `advance`. Other live control changes affect the next relevant advance branch deterministically.

## 14. Null and malformed behavior

Required fail-safe behavior:

- null state read -> envelope level zero;
- null state mutator -> no-op;
- null controls on retrigger/note-off/advance -> reset/off silence;
- null static-level input when mapping four effective levels -> all outputs zero;
- null output pointer -> no-op;
- malformed scalar time/sustain controls above 255 saturate to 255;
- malformed Q15 level above 32767 saturates to 32767;
- corrupt internal level above `0x7fff0000` clamps before stage arithmetic;
- unknown stage -> zero/OFF.

## 15. Realtime bound

Per active FM voice, per rendered sample:

- exactly four fixed envelope stage updates;
- exactly four effective-level mappings;
- direct immutable table lookup only;
- fixed branches only;
- no heap;
- no graph traversal;
- no runtime `/` or `%`;
- no `uint64_t`, `int64_t` or `long long`;
- no float/double;
- no mutable global DSP state.

The production implementation should unroll the four-operator top-level calls. A bounded helper for one operator is allowed.

## 16. Resource ceilings

Frozen WP5.6 primitive ceilings:

```text
FmtFmEnvelopeState                  exactly 20 B
FmtFmEnvelopeControls               exactly 16 B or less
immutable step table                exactly 1024 B
PI32v2 envelope object text+rodata  <= 2048 B
new mutable global RAM              0 B
```

The full running application must remain exact WP5.0 while WP5.6 stays isolated.

## 17. Deterministic source-absent reference requirements

Before production source exists, the D054 gate must prove:

1. D049-D053 and `dinghy.c` unchanged from parent closeout;
2. envelope production source/generated table absent;
3. exact 20 B state model and stage IDs;
4. exact 16 B control model;
5. all 256 time controls and frozen table hash;
6. all 256 SUSTAIN mappings and frozen hash;
7. exhaustive 65,536 D052 LEVEL x envelope LEVEL outputs and frozen hash;
8. zero-time A/D/R semantics;
9. hard retrigger from zero;
10. release from current level;
11. maximum RELEASE terminates deterministically;
12. independent four-operator ADSR stress;
13. null/malformed fail-safe semantics;
14. inherited WP5.5/D053 host regression remains exact.

Canonical four-operator 65,536-sample reference stress uses:

```text
ATTACK  [1, 32, 64, 0]
DECAY   [64, 96, 128, 0]
SUSTAIN [255, 192, 128, 64]
RELEASE [96, 128, 160, 0]
static D052 Q15 LEVEL [32767, 24672, 16448, 8224]
note-off before sample 32768
```

For every sample serialize four effective levels as little-endian uint16 followed by four stage bytes, before the post-render advance.

Frozen stress SHA-256:

```text
8cdf12197813d5718b3b4c5a40375534f93ea006f61e5aa325f4b48af71e0849
```

At the end all four operators must be zero/OFF.

Maximum RELEASE control 255 from full scale must reach OFF after exactly:

```text
2701155 advance calls
```

## 18. Production source boundary

Only after accepted source-absent evidence may WP5.6 add the equivalent of:

```text
firmware/src/fmt_fm_envelope.h
firmware/src/fmt_fm_envelope.c
firmware/gen/fmt_fm_envelope_step_q15_16.inc
tools/gen_fmt_fm_envelope.py
```

No D049-D053 or `dinghy.c` modification belongs to WP5.6 primitive implementation.

No physical FM-1 is available. `hardware_margin_claim=none`.
