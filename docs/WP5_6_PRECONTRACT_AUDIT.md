# WP5.6 Precontract Audit - FM4 envelope strategy

Date: 2026-10-25  
Package: WP5.6 / issue #49  
Branch: `phase5/fm4-envelope`  
Parent: WP5.5 / D053 closeout `216c7e34b6d761cadb683bd1fc356c216b98cc27`

Status: **PRECONTRACT - D054 NOT FROZEN**

This audit chooses the architecture direction before exact envelope arithmetic is frozen. Production envelope source is forbidden until D054 and its source-absent gate are accepted.

## Existing FM surfaces that must remain unchanged

D049:

- each operator owns only a 32-bit phase accumulator;
- each render receives `phase_increment`, signed phase modulation and one Q15 level;
- level scaling is already deterministic and bounded.

D050:

- exactly four operators;
- each operator renders exactly once per sample;
- fixed curated algorithms only;
- no dynamic graph traversal.

D052:

- four independent stable LEVEL controls already map to four Q15 operator levels;
- one global INDEX maps to Q14 edge modulation depth;
- mapped control state is transient.

D053:

- OP4 feedback history stores the already level-scaled OP4 sample;
- feedback therefore naturally follows any future OP4 envelope when that envelope scales OP4 LEVEL before D049 render;
- D053 does not need to know about envelope stages.

## Candidate A - one post-mix amplitude envelope

Placement:

```text
D049/D050 complete FM sample -> one envelope multiply -> output
```

Advantages:

- minimum state;
- one envelope update and one output multiply per sample;
- trivial algorithm independence.

Rejected for V1.

Reason: it only changes final loudness. The relative operator contributions and modulation edges inside D050 are already finished before the envelope is applied, so it cannot create the core FM behavior of modulators fading or building independently from carriers.

## Candidate B - one common pre-operator envelope

Placement:

```text
common ENV -> scale all four D052 LEVEL values -> D049/D050
```

Advantages:

- one envelope state;
- four level multiplies;
- because modulator amplitudes change before D050 edge modulation, it can create some coupled brightness change.

Not selected as the working V1 candidate.

Reason: every operator follows the same contour. Carrier and modulator evolution are locked together, and the usefulness depends strongly on algorithm topology. It cannot express the basic FM gesture of a short modulator transient over a sustained carrier, or a slow modulator swell over an already sounding carrier.

## Candidate C - four independent per-operator envelopes

Placement:

```text
OPn static LEVEL from D052
        x
OPn envelope level
        -> effective Q15 LEVEL
        -> D049 operator render
        -> D050 topology
```

**Selected as the working architecture candidate for D054.**

Why this is the smallest candidate that preserves clearly useful FM timbral evolution:

- operator identity remains stable across every D050 algorithm;
- no carrier/modulator role remapping is needed when algorithms change;
- each operator can have a transient, sustained or slow contour independently;
- the same mechanism controls audible carrier level and modulator strength through the existing D049/D050 path;
- OP4 envelope behavior composes automatically with D053 feedback history;
- D049-D053 APIs can remain unchanged;
- work remains fixed at four envelope updates plus four level scalings per rendered sample.

## State packing candidate

Do not use four padded envelope structs if arrays can preserve a smaller exact layout.

A viable state shape is:

```c
uint32_t level_accum[4];  /* high-resolution envelope accumulator */
uint8_t  stage[4];
```

Raw persistent state: **20 B per future FM voice**.

This is a candidate ceiling target, not yet a frozen ABI. A high-resolution accumulator is preferred because a plain Q15 integer level cannot represent both very long smooth times and per-sample updates without stepping.

D050 + D053 + candidate envelope state would therefore be:

```text
D050 operator phase state   16 B
D053 feedback history        2 B
candidate D054 envelope     20 B
--------------------------------
FM synthesis state          38 B per future voice before allocator metadata
```

The 20 B candidate must be proven by compile-time size assertions before D054 freezes.

## Stage-model direction

Working candidate: independent linear **ADSR** per operator.

Rationale:

- ATTACK is required for pad/swell behavior;
- DECAY + SUSTAIN allows a modulator to provide an initial bright transient then settle while the note remains held;
- RELEASE is required for bounded note-off behavior before WP5.7 voice integration;
- a simpler AR model loses the transient-to-sustain FM gesture;
- adding extra multi-stage DX-style envelopes in V1 would multiply controls/state and is deferred unless evidence shows ADSR cannot support the intended tracker workflow.

Exact controls are not frozen yet, but the current candidate is four bytes per operator:

```text
ATTACK DECAY SUSTAIN RELEASE
```

16 stable envelope-control bytes per FM instrument if accepted later by WP5.8.

## Level-domain direction

Envelope state should use a higher-resolution nonnegative internal accumulator. The render-facing envelope level is reduced deterministically to Q15, then multiplied by the already mapped D052 Q15 LEVEL to produce the effective D049 operator level.

Required final mapping properties:

- zero envelope output produces exact silence for that operator;
- maximum envelope output preserves the D052 LEVEL as closely as the frozen integer rule permits;
- no signed overflow;
- no float/double;
- no runtime 64-bit division;
- one bounded multiply/shift path per operator.

Exact rounding is still open and must be frozen in D054.

## Time/rate direction

A per-sample envelope update is the preferred baseline because it avoids control-rate stair-stepping and keeps behavior independent of tracker tick density.

A plain Q15 accumulator is insufficient for long smooth times. D054 should evaluate a 32-bit fixed-point accumulator plus a precomputed 8-bit-control -> 32-bit step table.

Candidate properties to test before freeze:

- control `0` means an explicit instant transition, not merely the fastest finite rate;
- controls `1..255` form a monotonic musically useful time curve;
- table generation happens offline/test-side;
- firmware render path performs no division;
- one constant table may be shared by A/D/R if one time law is musically acceptable;
- table size target <= 1,024 B for 256 x 32-bit steps;
- long-time endpoint must remain smooth with per-sample updates.

The exact time range and exponential/curved mapping are **not frozen** by this audit.

## Retrigger/reset/release direction

To freeze in D054 after vector review:

- reset must return all four envelopes to deterministic OFF/zero state;
- note-on retrigger policy must be explicit: hard restart from zero versus restart from current level;
- note-off must enter RELEASE from the current level without allocating new state;
- zero-time A/D/R stages must collapse deterministically without loops or repeated same-sample traversal;
- sustain must hold exactly until note-off;
- algorithm switches must not implicitly reset envelope state, matching D053 history continuity unless later voice semantics explicitly demand a note retrigger.

Working preference: hard note retrigger from zero for V1, because it is deterministic and matches D049/D053 retrigger reset semantics. This remains unaccepted until D054 vectors are frozen.

## Realtime bound

Candidate per rendered sample per FM voice:

- exactly four envelope state updates;
- exactly four envelope-to-D052 level scalings;
- fixed stage checks only;
- no heap;
- no traversal;
- no runtime division/modulo;
- no dynamic graph selection beyond existing bounded D050 algorithm choice.

WP5.6 remains isolated, so full application target identity must remain exact WP5.0 until WP5.7 integrates FM voices.

## D054 questions still open

D054 MUST freeze before implementation:

1. exact accumulator representation and exact state size;
2. exact ADSR stage enum and same-sample zero-time transition semantics;
3. exact 0..255 SUSTAIN -> envelope-level mapping;
4. exact 0..255 A/D/R -> step/time table law and endpoint range;
5. exact effective-level multiply and rounding rule;
6. hard-retrigger versus current-level retrigger;
7. release completion and OFF definition;
8. exact persistent-state ceiling;
9. exact constant-table and PI32v2 object ceilings;
10. deterministic reference vectors, exhaustive control mapping checks and long-run stress.

## Source boundary

Before D054 source-absent acceptance there must be no:

```text
firmware/src/fmt_fm_envelope.h
firmware/src/fmt_fm_envelope.c
firmware/gen/fmt_fm_envelope_*.inc
```

D049-D053 and `dinghy.c` remain frozen during WP5.6 primitive work.

No physical FM-1 is available. `hardware_margin_claim=none`.
