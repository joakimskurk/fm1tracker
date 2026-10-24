# WP4.6 Common Envelope Contract

Status: **FROZEN BY D042 BEFORE ENVELOPE PRODUCTION SOURCE**
Date: 2026-10-24
Package: WP4.6 / issue #36
Phase 4 branch: `phase4/8bit-engine`
Parent architecture: D037 / `WP4_1_ENGINE_CONTRACT.md`
Event baseline: D033 + D037 NOTE/TIE/OFF/RETRIG adapter semantics

This document is the normative contract for the common Phase 4 amplitude-envelope primitive. It freezes envelope topology, exact fixed state, integer time/rate mapping, NOTE/TIE/OFF/RETRIG behavior, sample/update ordering, deterministic evidence and realtime bounds before `fmt_envelope.*` production source is created.

WP4.6 does **not** connect the envelope or any Phase 4 oscillator to `dinghy.c`. The running full app stays on the Phase 3 proof sine and must remain byte-identical through WP4.6 closeout.

## 1. Topology

D042 freezes one common **linear ADSR** envelope for the Phase 4 8-BIT voice layer:

```text
ATTACK -> DECAY -> SUSTAIN -> RELEASE -> IDLE
```

The topology is deliberately conventional because TIE already provides explicit tracker legato and D033 already provides explicit RETRIG. A simpler AR envelope would make sustained instrument shaping too limited; a more complex multisegment envelope would pull later UI/parameter work into Phase 4 and increase state/CPU without a current requirement.

The envelope controls **amplitude only** in WP4.6. Pitch, oscillator phase/state and engine-specific timbre remain outside this primitive.

## 2. Parameter snapshot

The primitive accepts one exact four-byte control snapshot:

```c
typedef struct {
    uint8_t attack;
    uint8_t decay;
    uint8_t sustain;
    uint8_t release;
} FmtEnvelopeParams;
```

Contract:

- `sizeof(FmtEnvelopeParams) == 4`;
- ATTACK/DECAY/RELEASE use the common 0..255 D042 rate domain;
- SUSTAIN uses the common 0..255 level domain;
- WP4.8, not WP4.6, owns stable parameter IDs/page placement and p-lock mapping;
- WP4.6 defines oscillator-independent numeric meaning only.

Frozen neutral/default envelope values are:

```text
ATTACK  = 24
DECAY   = 0
SUSTAIN = 255
RELEASE = 72
```

At 44.1 kHz these correspond to approximately a 3 ms full-scale attack slope, no decay away from full sustain, and a 24 ms full-scale release slope. This is intentionally close to the existing proof-lane click-suppression feel without claiming byte-identical envelope audio before the later integration package.

## 3. Exact state

The envelope state is exactly:

```c
typedef struct {
    uint32_t level_q16;
    uint32_t rate_q16;
    uint32_t decay_rate_q16;
    uint32_t sustain_q16;
    uint8_t stage;
    uint8_t reserved[3];
} FmtEnvelope;
```

Contract:

- `sizeof(FmtEnvelope) == 20`;
- `level_q16` is unsigned Q15.16 amplitude level;
- legal level range is `0 .. (32767 << 16)`;
- `rate_q16` is the active linear slope magnitude per rendered frame;
- `decay_rate_q16` is pre-resolved at NOTE/RETRIG so ATTACK -> DECAY does not need division or parameter lookup in the render loop;
- `sustain_q16` is the resolved sustain target;
- `stage` is one exact stage ID;
- `reserved[]` is always zero after reset/note/retrigger and is included so state layout remains explicit/aligned.

Frozen stage IDs:

```text
FMT_ENVELOPE_IDLE    = 0
FMT_ENVELOPE_ATTACK  = 1
FMT_ENVELOPE_DECAY   = 2
FMT_ENVELOPE_SUSTAIN = 3
FMT_ENVELOPE_RELEASE = 4
```

Eight future Phase 4 envelopes therefore require exactly **160 B** mutable state before allocator/common voice metadata.

## 4. Q15 level domain

Frozen full-scale internal level:

```text
FMT_ENVELOPE_LEVEL_MAX_Q16 = 32767 << 16 = 0x7FFF0000
```

`fmt_envelope_render()` returns one unsigned nominal-Q15 amplitude value:

```text
0..32767
```

A later integrated voice scales a signed oscillator sample with this level using the D037 bounded integer mix path. The envelope itself does not render audio.

SUSTAIN byte conversion is exact:

```text
sustain_q15 = (sustain * 257) >> 1
sustain_q16 = sustain_q15 << 16
```

This is exactly the nearest-integer mapping of `0..255` onto `0..32767` without division.

Frozen sustain vectors:

| control | Q15 |
|---:|---:|
| 0 | 0 |
| 1 | 128 |
| 64 | 8224 |
| 128 | 16448 |
| 160 | 20560 |
| 192 | 24672 |
| 254 | 32639 |
| 255 | 32767 |

## 5. ATTACK/DECAY/RELEASE rate domain

Time controls are **full-scale linear slope controls**, not exact wall-clock duration promises for every stage distance.

Control `0` means instantaneous transition.

For control `1..255`:

```text
mantissa = control & 15
exponent = control >> 4
rate_q16 = RATE_BASE_Q16[mantissa] >> exponent
if rate_q16 == 0: rate_q16 = 1
```

Frozen 16-entry base-rate table:

```text
48804958, 45689748, 42948363, 41296503,
39043966, 37024451, 35203576, 34086002,
32536639, 31122002, 29825252, 29019164,
27888547, 26842727, 25872508, 25263743
```

The table is immutable `uint32_t[16]`, exactly **64 B**.

A full-scale ramp from 0 to `0x7FFF0000` takes `ceil(MAX/rate)` rendered frames. Decay and release use the same slope but their actual duration is proportional to the distance from current level to target. This is intentional classic linear-rate envelope behavior.

Frozen full-scale rate/time vectors:

| control | rate Q16 | frames | approx at 44.1 kHz |
|---:|---:|---:|---:|
| 0 | instant | 0 | 0 |
| 1 | 45689748 | 47 | 1.07 ms |
| 15 | 25263743 | 85 | 1.93 ms |
| 16 | 24402479 | 88 | 2.00 ms |
| 24 | 16268319 | 133 | 3.02 ms |
| 32 | 12201239 | 177 | 4.01 ms |
| 64 | 3050309 | 705 | 15.99 ms |
| 80 | 1525154 | 1409 | 31.95 ms |
| 96 | 762577 | 2817 | 63.88 ms |
| 112 | 381288 | 5633 | 127.73 ms |
| 128 | 190644 | 11265 | 255.44 ms |
| 160 | 47661 | 45057 | 1.022 s |
| 192 | 11915 | 180229 | 4.087 s |
| 224 | 2978 | 721095 | 16.351 s |
| 240 | 1489 | 1442189 | 32.703 s |
| 255 | 770 | 2788855 | 63.239 s |

The production path contains no runtime division for this mapping.

## 6. NOTE / gate-on semantics

D042 resolves the D037 open envelope question as follows:

**Ordinary NOTE hard-retriggers the envelope from zero but does not restart oscillator phase.**

This is the tracker distinction:

- NOTE = new articulation, envelope restarts;
- TIE = legato continuation, envelope does not restart;
- D033 RETRIG = oscillator/state restart **and** envelope restart.

Canonical NOTE behavior with a non-null parameter snapshot:

1. `level_q16 = 0`;
2. resolve `sustain_q16`;
3. resolve/store `decay_rate_q16`;
4. if ATTACK == 0, jump immediately to full scale and begin DECAY/SUSTAIN according to DECAY/SUSTAIN;
5. otherwise enter ATTACK using resolved attack rate.

A NOTE on an already active voice therefore may introduce an intentional articulation discontinuity to zero. Use TIE for continuous legato.

A newly allocated/reset voice and an ordinary NOTE use the same envelope note-on operation.

NOTE DELAY calls this same NOTE operation only when the delayed NOTE actually fires at the frozen D033 tick.

## 7. RETRIG semantics

`fmt_envelope_retrigger()` is exactly the same hard envelope restart as NOTE:

```text
level -> 0
stage -> ATTACK / resolved zero-time successor
```

D037 still owns the difference: D033 RETRIG also resets oscillator phase/state, whereas ordinary NOTE does not.

RETRIG with no active voice remains a no-op at the adapter level under D037 and therefore never calls the envelope primitive.

## 8. TIE and EMPTY semantics

TIE and EMPTY/REST make **no envelope call**.

They therefore do not:

- reset level;
- restart ATTACK;
- alter DECAY progress;
- alter SUSTAIN;
- cancel RELEASE;
- change stage/rate.

This preserves D037 legato and natural continuation semantics.

WP4.8 may later define what happens when envelope parameter controls themselves are changed while a voice is active. D042 does not silently add live stage retiming.

## 9. OFF / NOTE CUT / release semantics

OFF and D033 NOTE CUT call the common gate-off operation at their exact frozen timing point.

Gate-off receives one already-resolved RELEASE control byte.

Behavior:

- if envelope is already IDLE/zero: remain IDLE;
- RELEASE == 0: set level 0 and IDLE immediately;
- otherwise: preserve the **current** level, resolve release rate, enter RELEASE;
- RELEASE always falls from the current level; it never jumps back to sustain/full scale first.

If OFF/CUT occurs during ATTACK or DECAY, RELEASE therefore starts from the exact current level at that frame.

## 10. Zero-time stage rules

Zero-time controls are collapsed deterministically without adding dummy one-frame stages.

NOTE/RETRIG:

- A=0, D=0 -> level becomes SUSTAIN immediately, stage SUSTAIN;
- A=0, D>0 -> level becomes full scale immediately, stage DECAY unless sustain is already full scale;
- A>0 -> ATTACK starts from zero;
- when ATTACK reaches full scale and D=0 (or sustain=full), transition directly to SUSTAIN;
- SUSTAIN=0 is legal; DECAY may therefore fall all the way to zero while gate remains logically on.

OFF/CUT:

- R=0 -> level zero/IDLE immediately.

Zero-time stages do not create hidden delay samples.

## 11. Exact per-frame update ordering

`fmt_envelope_render()` uses **advance-before-return**, matching the existing proof-lane amplitude-ramp feel.

For one rendered frame:

```text
if ATTACK:
    level += attack_rate, clamp to MAX
    if MAX reached: transition to DECAY or SUSTAIN
elif DECAY:
    level -= decay_rate, clamp to sustain
    if sustain reached: transition to SUSTAIN
elif SUSTAIN:
    hold level
elif RELEASE:
    level -= release_rate, clamp to zero
    if zero reached: transition to IDLE
elif IDLE:
    hold zero
return level_q16 >> 16
```

Consequences:

- nonzero ATTACK produces a positive first returned level;
- zero ATTACK may produce full/sustain immediately before first render;
- release begins reducing amplitude on the first render after OFF/CUT;
- terminal levels are exact despite integer rate quantization;
- each render performs at most **one nonzero-ramp stage transition**;
- zero-duration successor stages are collapsed by a fixed conditional, never a loop.

## 12. Exact default-shape compatibility intent

Frozen default params:

```text
A=24, D=0, S=255, R=72
```

produce:

- full-scale attack in 133 frames (~3.02 ms);
- full sustain;
- full-scale release in approximately 1057 frames (~23.97 ms).

The current proof sine uses a simple +48/-6 integer ramp rather than ADSR. D042 intentionally preserves its short click-suppression timescale, not its exact byte/sample sequence. Exact full-app audio remains unchanged in WP4.6 because the new envelope is isolated.

## 13. API boundary

Intended production API:

```c
void fmt_envelope_reset(FmtEnvelope *env);
uint32_t fmt_envelope_rate_q16(uint8_t control);
uint16_t fmt_envelope_sustain_q15(uint8_t sustain);
void fmt_envelope_note_on(FmtEnvelope *env,
                          const FmtEnvelopeParams *params);
void fmt_envelope_retrigger(FmtEnvelope *env,
                            const FmtEnvelopeParams *params);
void fmt_envelope_gate_off(FmtEnvelope *env,
                           uint8_t release_control);
uint16_t fmt_envelope_render(FmtEnvelope *env);
```

Null behavior:

- reset with `NULL`: no-op;
- note_on/retrigger with null env or params: no-op;
- gate_off with null env: no-op;
- render with null env: returns 0.

The primitive does not inspect oscillator state, tracker storage, D031/D032 caches, D033 runtime state, D034 queue, UI or persistence state.

## 14. Realtime bounds

One envelope render is fixed O(1) integer work:

- one stage switch;
- at most one 32-bit add/subtract;
- bounded compare/clamp;
- at most one fixed stage transition;
- one Q16 -> Q15 shift.

There is no loop, divide, modulo, allocation, lock traversal, table search, screen access, persistence, blocking, recursion, floating point or 64-bit arithmetic in `fmt_envelope_render()`.

Rate resolution at NOTE/RETRIG/OFF uses one 16-entry direct index plus right shift, never division.

## 15. Resource budget and isolation

Frozen isolated costs:

```text
FmtEnvelopeParams = 4 B
FmtEnvelope       = 20 B
8 envelopes       = 160 B mutable state
rate table        = 16 * 4 = 64 B immutable data
```

There is no pool storage or dynamic allocation.

WP4.6 remains isolated and `fmt_envelope.*` is not included by `dinghy.c`.

Required full-app delta at WP4.6 closeout:

| Resource | Required delta versus WP4.5 full app |
|---|---:|
| app image | **0 B** |
| normal RAM | **0 B** |
| pool RAM | **0 B** |
| default WAV | **byte-identical** |
| BIN/FWSC | **byte-identical** |

The isolated module may contain its 64 B immutable rate table without affecting the linked full app.

## 16. Required deterministic tests

Focused host tests must prove at least:

1. compile-time `FmtEnvelopeParams == 4 B` and `FmtEnvelope == 20 B`;
2. exact stage IDs and reset state;
3. exact 16-entry base-rate table behavior through all 256 time-control values;
4. rate is nonincreasing as control increases, with control 0 instantaneous and every nonzero control producing nonzero rate;
5. all frozen rate/time vectors in Section 5;
6. all sustain vectors in Section 4 and monotonic sustain mapping over 0..255;
7. NOTE hard restart from zero and ordinary NOTE-vs-TIE semantics at the primitive/adapter boundary;
8. RETRIG exactly matches NOTE envelope restart;
9. OFF/CUT release starts from current level;
10. all zero-time combinations, including A0/D0/R0 and sustain extrema;
11. monotonic ATTACK, DECAY and RELEASE;
12. exact terminal full/sustain/zero levels and stage transitions;
13. default A24/D0/S255/R72 attack/release frame counts;
14. release from half/current level is proportional to distance and never jumps upward;
15. identical state/input replay is deterministic;
16. guard words prove no mutation outside the 20-byte envelope state;
17. one long sequence traversing ATTACK, DECAY, SUSTAIN and RELEASE with frozen hash/state.

Static/source audit must prove no float/double, runtime division/modulo, allocation, render loop or tracker-store traversal in the new production module.

The pinned real JieLi PI32v2 compiler must independently compile `fmt_envelope.c`.

## 17. Frozen long-run signature

The deterministic long sequence uses:

```text
params: A=64, D=96, S=160
release: R=112
note_on
render 4096 frames
OFF/gate_off
render 4096 frames
```

Frozen stage/rate facts:

```text
attack_rate_q16 = 3050309
attack -> decay on attack render 705, returned Q15 = 32767
decay_rate_q16 = 762577
sustain_q15 = 20560
decay -> sustain on total render 1755, returned Q15 = 20560
release_rate_q16 = 381288
release -> idle on release render 3534, returned Q15 = 0
final level_q16 = 0
final stage = IDLE
```

FNV-1a32 folds each returned Q15 level for the first 4096 renders, then each returned level for the 4096 release renders, then final `level_q16`, then final `stage`.

Frozen result:

```text
FNV-1a32 = 0x40A44A2E
```

Any change to topology, hard-retrigger semantics, rate table, sustain mapping, update ordering or terminal clamp changes this evidence and requires explicit review.

## 18. Full-app regression obligation

WP4.6 closeout must rebuild the complete host and target app and prove it remains the WP4.5 baseline because the envelope is still isolated:

- default host WAV SHA-256 `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`;
- `fm1tracker.bin` SHA-256 `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27`;
- `fm1tracker-0.1.fwsc` SHA-256 `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`;
- resources 66,960 B image / 13,296 B normal RAM / 113,000 B pool;
- package identity `FM-1_987`.

## 19. Non-goals

WP4.6 does not implement or decide:

- live envelope parameter retiming/modulation during an already-running stage;
- final voice allocation/stealing;
- engine parameter IDs/pages;
- engine p-lock publication cache;
- full D032/D033 adapter integration into an 8-BIT voice;
- multi-track engine rendering;
- replacement of `tracker_voice` / proof sine;
- live performance capture;
- persistence/file format;
- final hardware UX.

## 20. Exit condition

WP4.6 may close only when:

- D042 is present in `DECISIONS.md` and this contract is committed before envelope production source;
- stage/time/gate/retrigger/release/zero-time vectors and frozen long-run hash are green;
- the new module compiles with the pinned real JieLi PI32v2 compiler;
- static realtime/source audit is green;
- the complete full app remains byte-identical to the WP4.5 baseline because the envelope is isolated;
- durable `WP4_6_VERIFICATION.md` records exact target/object/full-app evidence;
- no WP4.7+ implementation has been pulled forward.

After that, WP4.7 / deterministic voice allocation and stealing becomes the only active Phase 4 package.
