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

Four static algorithms, fixed OP4 -> OP3 -> OP2 -> OP1 evaluation, deterministic carrier normalization and 16 B routing state. PI32v2 router object 362 B. Canonical run `40136425976`, host `120998804031`, target `120998804175`. Evidence `WP5_2_VERIFICATION.md`.

### WP5.3 - integer ratio table + pitch mapping - COMPLETE - issue #46 - D051

Stable V1 ratios:

`0.5, 1.0, 1.25, 1.4140625, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 5.0, 6.0, 7.0, 8.0, 12.0, 16.0`

Verified:

- D051 frozen before source;
- source-absent reference run `40138575267` / job `121005262360`;
- unsigned Q8 split scaling with deterministic clamp/rounding;
- 262,144 exhaustive legal pitch/ratio outputs, SHA `5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c`;
- only `fmt_fm_ratio.h/.c` added under production firmware;
- PI32v2 ratio object 264 B, SHA `aa5bd8b9e1afa72054af2effeefbf468cfbb8b2b8d978e028540a30dc162956d`;
- canonical final run `40139154497`, host `121006986068`, target `121006985928`;
- full application remains exact WP5.0.

Evidence: `WP5_3_VERIFICATION.md`.

### WP5.4 - levels / modulation index - NEXT - issue #47

Goal: define, implement and verify the smallest bounded integer control layer that maps FM4 instrument controls to D050 `level_q15[4]` and modulation-depth semantics.

Required order:

1. inspect D049 Q15 level and D050 modulation/carrier behavior;
2. decide a musically useful carrier/modulator control model;
3. explicitly decide global versus bounded per-operator/per-edge modulation representation;
4. freeze stable internal control domains;
5. freeze exact integer mappings, zero/nominal/max semantics, rounding and clamp policy;
6. prove all four D050 algorithms and representative D051 ratio combinations;
7. freeze source/realtime/PI32v2 ceilings;
8. write/freeze D052 before production mapping source;
9. pass source-absent reference CI;
10. only then implement and verify;
11. write `WP5_4_VERIFICATION.md` before WP5.5 begins.

Allowed:

- immutable level/mod-index mapping tables;
- bounded integer transforms;
- stable internal DSP control domains;
- deterministic host vectors/hashes;
- pinned PI32v2 object measurement;
- inherited D049/D050/D051/WP5.0 regressions.

Out of scope:

- feedback, WP5.5;
- envelope strategy, WP5.6;
- FM voice/D048 integration, WP5.7;
- FM tracker pages/p-lock IDs, WP5.8;
- audible mixed 8-BIT + FM4, WP5.9.

D052 does not exist yet.

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
