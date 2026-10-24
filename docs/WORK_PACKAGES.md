# FM1Tracker Work Packages

`PROJECT_STATE.md` is the single source of truth for current execution. This file defines bounded package scope and exit criteria.

## Rules

1. One active package at a time.
2. A package closes only with durable verification or an explicit recorded waiver.
3. Do not pull later-package features forward.
4. Architectural decisions are recorded before implementation.
5. Realtime work remains bounded and deterministic.
6. Eight logical tracks may not be silently reduced.

## Phase 1-4 - COMPLETE

- Phase 1 clean platform baseline: COMPLETE
- Phase 2 tracker core: COMPLETE
- Phase 3 p-locks/effects/runtime ingress: COMPLETE
- Phase 4 first complete 8-BIT family: COMPLETE, D037-D046

Canonical D046 exit: run `40124536375`, job `120963022289`.

## Phase 5 - mixed-engine routing + FM4 - IN PROGRESS

Parent issue: #42.

### WP5.0 - mixed-engine routing - COMPLETE - issue #43 - D048

Per-instrument family/engine routing implemented and verified. Canonical run `40129822006`, job `120978955451`. Evidence `WP5_0_VERIFICATION.md`.

### WP5.1 - fixed-point FM operator - COMPLETE - issue #44 - D049

4 B phase-state operator, generated Q15 sine table, deterministic phase modulation and exact PI32v2 evidence. Canonical run `40134823600`, host `120994003059`, target `120994002842`. Evidence `WP5_1_VERIFICATION.md`.

### WP5.2 - curated FM4 algorithm routing - COMPLETE - issue #45 - D050

Stable IDs/topologies:

```text
0 STACK4    OP4 -> OP3 -> OP2 -> OP1
1 FAN3      OP2 + OP3 + OP4 -> OP1
2 DUAL2     OP4 -> OP3 plus OP2 -> OP1
3 PARALLEL4 OP1 + OP2 + OP3 + OP4
```

Verified:

- fixed OP4 -> OP3 -> OP2 -> OP1 evaluation;
- all four operators advance exactly once/sample;
- 16 B routing state, no extra history;
- bounded signed modulation and modulo-2^32 phase accumulation;
- deterministic carrier normalization;
- source-absent D050 evidence before implementation;
- four exact long hashes and 65,536-render-per-algorithm stress;
- algorithm-switch phase continuity;
- PI32v2 router object 362 B text+rodata;
- full application remains exact WP5.0.

Canonical implementation head `bc054bc697304afe94fe18b2990396e6a1667dbf`, run `40136425976`, host `120998804031`, target `120998804175`. Evidence `WP5_2_VERIFICATION.md`.

### WP5.3 - integer ratio table + pitch mapping - ACTIVE - issue #46

Goal: convert one already-resolved base phase increment into four operator increments through a compact stable ratio set, using bounded integer arithmetic only.

Required order:

1. inspect `fmt_engine_pitch.*`, D049 phase conventions and D050 input boundary;
2. select a compact musically useful ratio set;
3. freeze stable ratio IDs and exact mathematical meanings;
4. freeze integer arithmetic and rounding;
5. freeze overflow/clamp and supported base-increment domain;
6. explicitly decide sub-unity/fractional ratio scope;
7. define deterministic low/mid/high and edge fixtures;
8. define four-operator mapping API and any state requirement;
9. define realtime/source and PI32v2 object ceilings;
10. write/freeze D051 before production source;
11. pass a source-absent reference gate;
12. only then implement and verify;
13. write `WP5_3_VERIFICATION.md` before WP5.4 begins.

Allowed:

- immutable ratio descriptors/tables;
- integer base-increment -> operator-increment transform;
- stable internal ratio IDs;
- deterministic rounding/clamp policy;
- host fixtures/hashes;
- pinned target object measurement;
- inherited D049/D050/WP5.0 regressions.

Out of scope:

- final operator level/mod-index UX, WP5.4;
- feedback, WP5.5;
- envelope strategy, WP5.6;
- FM voice/D048 integration, WP5.7;
- FM pages/p-lock IDs, WP5.8;
- audible mixed 8-BIT + FM4, WP5.9.

### WP5.4 - levels / modulation index - PLANNED

### WP5.5 - bounded feedback - PLANNED

### WP5.6 - FM envelope strategy - PLANNED

### WP5.7 - FM voice allocation/integration - PLANNED

### WP5.8 - FM parameter pages + p-lock mapping - PLANNED

### WP5.9 - mixed 8-BIT + FM4 validation - PLANNED

### WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit - PLANNED

## Mandatory core milestone - live performance capture + performance FX

PLANNED after engine foundations. Reference `LIVE_PERFORMANCE_CAPTURE_VISION.md`.

## Phase 6 - song/order/persistence - PLANNED

## Phase 7 - physical hardware UX freeze - BLOCKED

Requires physical FM-1.

## Phase 8 - interoperability - PLANNED

## Phase 9 - optional extensions - DEFERRED

`hardware_margin_claim=none` until physical hardware evidence exists.
