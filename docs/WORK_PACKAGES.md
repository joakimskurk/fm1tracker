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

Canonical D046 exit: run `40124536375`, job `120963022289`.

## Phase 5 - mixed-engine routing + FM4 - IN PROGRESS

Parent issue: #42.

### WP5.0 - mixed-engine routing - COMPLETE - issue #43 - D048
### WP5.1 - fixed-point FM operator - COMPLETE - issue #44 - D049
### WP5.2 - curated FM4 algorithm routing - COMPLETE - issue #45 - D050
### WP5.3 - integer ratio table + pitch mapping - COMPLETE - issue #46 - D051
### WP5.4 - levels / modulation index - COMPLETE - issue #47 - D052
### WP5.5 - bounded feedback - COMPLETE - issue #48 - D053
### WP5.6 - FM envelope strategy - COMPLETE - issue #49 - D054

### WP5.7 - FM voice allocation/integration - TECHNICALLY COMPLETE, CLOSEOUT ACTIVE - issue #50 - D055

D055 integrates the complete D049-D054 FM4 path as eight bounded family-local voices through D048 while preserving exact D043 ownership semantics.

Canonical implementation verification:

- head `75ff8136881799ad652df380a53002c5efa70516`;
- run `40173076046`;
- host `121107852886` SUCCESS;
- target `121107852943` SUCCESS;
- `FmtFmVoice` 92 B;
- `FmtFmEngine` 788 B;
- full target 77,048 B image / 20,464 B RAM / 113,000 B pool;
- all-8BIT production WAV exact WP5.0;
- mixed `FM4 -> 8-BIT -> FM4` fixture deterministic;
- zero active cross-track primary steals.

Evidence: `WP5_7_VERIFICATION.md`.

Exit remaining: the same workflow must pass on the documentation closeout head, then #50 may close.

### WP5.8 - FM parameter pages + p-lock mapping - PLANNED

Must begin only after #50 closes. Contract-first scope:

1. audit existing semantic parameter IDs, page model and sparse p-lock storage;
2. freeze stable FM parameter IDs/page layout and family-specific invalid-ID behavior;
3. freeze mutable FM instrument parameter storage and how it replaces the temporary WP5.7 immutable bridge patch;
4. preserve max four p-locks per step and exact row-boundary application timing;
5. keep editor/storage work out of the realtime sample loop;
6. freeze memory/image ceilings and a source-absent gate before production parameter integration;
7. prove exact all-8BIT regression equivalence.

### WP5.9 - mixed 8-BIT + FM4 validation - PLANNED

### WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit - PLANNED

## Mandatory core milestone - live performance capture + performance FX

PLANNED after engine foundations. Reference `LIVE_PERFORMANCE_CAPTURE_VISION.md`.

## Phase 6 - song/order/persistence - PLANNED
## Phase 7 - physical hardware UX freeze - BLOCKED
## Phase 8 - interoperability - PLANNED
## Phase 9 - optional extensions - DEFERRED

`hardware_margin_claim=none` until physical hardware evidence exists.
