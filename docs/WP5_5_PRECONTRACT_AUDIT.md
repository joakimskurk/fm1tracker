# WP5.5 Precontract Audit

Status: **COMPLETE - INPUT TO D053 FREEZE**  
Date: 2026-10-25  
Package: WP5.5 / issue #48  
Branch: `phase5/fm4-feedback`  
Parent closeout: `d3501eca952f3e903d4dcc4faab2851d439f7ad0`

## Purpose

This audit identifies the smallest feedback model that composes with frozen D049-D052 without changing those production sources or pulling later FM integration work into WP5.5.

## D049 observations

`fmt_fm_operator_render()` owns only a 32-bit carrier phase and receives one signed phase offset.

Frozen ordering is:

1. read current phase;
2. add signed `phase_modulation` modulo 2^32;
3. evaluate sine;
4. apply Q15 level;
5. advance carrier phase;
6. return the level-scaled sample.

The phase-modulation input is therefore already the correct insertion surface for self-feedback. Feedback does not need to mutate the carrier accumulator, add a second oscillator state or alter D049.

D049 output is bounded to `-32767..32767`, so one prior output fits losslessly in signed 16-bit state.

## D050 observations

Every D050 algorithm evaluates all four operators exactly once per sample and always begins with OP4.

```text
STACK4:    OP4 -> OP3 -> OP2 -> OP1
FAN3:      OP4, OP3, OP2 -> OP1
DUAL2:     OP4 -> OP3 and OP2 -> OP1
PARALLEL4: OP4 + OP3 + OP2 + OP1
```

OP4 is therefore the only operator that has all of these properties simultaneously:

- first chronological render in every frozen algorithm;
- no current-sample upstream dependency in any algorithm;
- present in every algorithm;
- phase continuity across algorithm changes is already guaranteed by D050;
- one self-feedback edge can be inserted without traversal, recursion or graph interpretation.

D050 also explicitly deferred feedback history to WP5.5 and forbids cached previous outputs in `FmtFmAlgorithmState`. D053 should therefore add separate feedback state rather than silently enlarge or reinterpret D050 state.

## D052 observations

D052 already freezes one byte-to-Q14 phase-depth expansion for INDEX:

```text
(v << 6) | (v >> 2)
```

for `v = 0..255`, producing `0..16383`.

Using the same numeric expansion for a separate FEEDBACK byte gives:

- consistent phase-depth resolution;
- no table;
- no runtime division;
- identical signed-32 arithmetic bound to a D050 modulation edge;
- no reinterpretation of D052 INDEX, because FEEDBACK remains a distinct parameter.

## Chosen D053 direction

The smallest sufficient V1 model is:

```text
previous OP4 level-scaled sample
    -> FEEDBACK 0..255 mapped to Q14
    -> sample * depth * 4
    -> signed phase offset into next OP4 render
    -> current OP4 output replaces history
```

Properties:

- topology: OP4 self-feedback only;
- delay: exactly one sample;
- history: exactly one signed 16-bit sample, 2 B;
- history source: D049 level-scaled OP4 output;
- feedback scaling: exact D052 INDEX-style 8-bit to Q14 expansion;
- first render after reset: zero feedback;
- feedback amount 0: no phase offset, while history still tracks current OP4 output;
- algorithm change: retain history, because OP4 remains the same continuously advancing operator;
- feedback reset/retrigger: history becomes zero;
- no averaging, multi-sample delay, per-edge history or arbitrary feedback graph.

## Arithmetic audit

For a valid D049 OP4 sample and maximum Q14 depth:

```text
32767 * 16383 * 4 = 2147287044
INT32_MAX - 2147287044 = 196603
```

Even a corrupted full signed-16 history value remains safe:

```text
-32768 * 16383 * 4 = -2147352576
-2147352576 - INT32_MIN = 131072
```

Thus every signed-16 history value x every legal FEEDBACK byte fits signed 32-bit arithmetic.

D051 frequency ratio does not enlarge this bound. Ratio changes only the unsigned 32-bit phase increment; D049 output remains Q15 bounded and phase addition remains modulo 2^32.

## Rejected alternatives

D053 rejects for V1:

- feedback on an algorithm-dependent carrier/modulator chosen at runtime;
- multiple feedback edges;
- arbitrary operator selection;
- two-sample averaging;
- feedback directly into the phase accumulator;
- dynamic graph traversal;
- feedback state inside D050 `FmtFmAlgorithmState`;
- reusing or overloading D052 INDEX as the FEEDBACK control.

These alternatives either add state/branching, make behavior algorithm-dependent, complicate continuity, or blur frozen ownership boundaries without evidence that V1 needs them.

## Implementation boundary

WP5.5 production source, after a green source-absent D053 gate, is limited to:

```text
firmware/src/fmt_fm_feedback.h
firmware/src/fmt_fm_feedback.c
```

D049, D050, D051, D052 and `dinghy.c` remain byte-for-byte unchanged in WP5.5. The feedback primitive remains isolated from the running application until the later FM integration package.
