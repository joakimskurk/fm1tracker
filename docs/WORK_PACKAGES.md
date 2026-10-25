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

Evidence `WP5_0_VERIFICATION.md`.

### WP5.1 - fixed-point FM operator - COMPLETE - issue #44 - D049

Evidence `WP5_1_VERIFICATION.md`.

### WP5.2 - curated FM4 algorithm routing - COMPLETE - issue #45 - D050

Evidence `WP5_2_VERIFICATION.md`.

### WP5.3 - integer ratio table + pitch mapping - COMPLETE - issue #46 - D051

Evidence `WP5_3_VERIFICATION.md`.

### WP5.4 - levels / modulation index - COMPLETE - issue #47 - D052

D052 froze:

- four independent LEVEL bytes and one global INDEX byte, all 0..255;
- exact Q15/Q14 integer mappings;
- saturating malformed scalar input;
- null-level fail-to-silence semantics;
- 10 B transient mapped state and zero persistent state;
- signed-32-safe D049/D050 single-edge bounds across all 65,536 legal LEVEL x INDEX pairs.

Verified:

- source-absent run `40140119411` / job `121009903030`;
- canonical implementation run `40143482844`, host `121019875458`, target `121019875309`;
- only `fmt_fm_controls.h/.c` added under production firmware;
- PI32v2 object 108 B, SHA `05867ebe120829d1e3b8bf6f0504f29d63e9643e4fa5eede4c6d858d31d25059`;
- full application remains exact WP5.0.

Evidence: `WP5_4_VERIFICATION.md`.

### WP5.5 - bounded feedback - NEXT - issue #48

Goal: define, implement and verify the smallest deterministic bounded feedback primitive/topology that composes with D049-D052 without pulling envelopes or voice/router integration forward.

Required order:

1. inspect D049 phase-modulation arithmetic and D050 fixed topology/evaluation order;
2. choose one bounded V1 feedback insertion/topology;
3. freeze internal feedback control domain, zero/nominal/max, clamp/rounding and malformed-input policy;
4. freeze one-sample history/state ownership plus reset and algorithm-switch behavior;
5. prove arithmetic safety against D051 ratio extremes and D052 maximum LEVEL/INDEX;
6. define deterministic vectors, reset/switch fixtures and long stress;
7. freeze persistent-state/realtime/PI32v2 ceilings;
8. write/freeze D053 before production feedback source;
9. pass source-absent reference CI;
10. only then implement and verify;
11. write `WP5_5_VERIFICATION.md` before WP5.6 begins.

Allowed:

- one explicitly frozen bounded feedback topology;
- compact fixed-size feedback history;
- deterministic integer transforms;
- deterministic host vectors/hashes and long stress;
- pinned PI32v2 state/object measurement;
- inherited D049-D052/WP5.0 regressions.

Out of scope:

- envelopes, WP5.6;
- FM voice/D048 integration, WP5.7;
- FM tracker pages/p-lock IDs, WP5.8;
- audible mixed 8-BIT + FM4, WP5.9.

D053 does not exist yet.

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
