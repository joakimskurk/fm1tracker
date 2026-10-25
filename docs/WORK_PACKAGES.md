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

Evidence `WP5_4_VERIFICATION.md`.

### WP5.5 - bounded feedback - COMPLETE - issue #48 - D053

D053 froze one bounded OP4 self-feedback edge, one-sample history and 2 B persistent state. Final closeout run `40165352623` passed host and target. Evidence `WP5_5_VERIFICATION.md`.

### WP5.6 - FM envelope strategy - TECHNICALLY COMPLETE, CLOSEOUT ACTIVE - issue #49 - D054

D054 froze:

- four independent per-operator ADSR envelopes;
- 16 A/D/S/R control bytes;
- 20 B persistent state per future FM voice;
- Q15.16 envelope level;
- exact 1,024 B immutable A/D/R step table;
- exact D052-compatible SUSTAIN mapping;
- hard retrigger, current-level release and true instant zero-time transitions;
- fixed per-sample work with no heap, traversal, runtime division/modulo, 64-bit integer or float.

Verified at technical checkpoint:

- source-absent run `40167537148`, job `121091550742`;
- implementation run `40167933064`, host `121092688126`, target `121092688295`;
- production source boundary exactly four allowed files;
- PI32v2 object 1,680 B text+rodata, SHA `22fa7937d7776fea37c8e758ca83ee0a3f5d6c506ebd4661a8290a4516e048d1`;
- full application remains exact WP5.0.

Evidence: `WP5_6_VERIFICATION.md`.

Exit remaining: same host/target verification must pass on the documentation closeout head, then #49 may close.

### WP5.7 - FM voice allocation/integration - PLANNED - issue #50

Blocked until WP5.6 closes.

Required order:

1. audit D045 8-BIT allocator and D048 route semantics against the complete D049-D054 FM voice state;
2. freeze voice count, ownership, primary/release-tail policy and allocator tie-breaks;
3. freeze D055 before production integration changes;
4. preserve D048 cross-family NOTE ordering and current-family non-NOTE semantics;
5. preserve no active cross-track primary stealing;
6. integrate D049-D054 reset/retrigger/release behavior;
7. prove exact all-8-BIT regression equivalence;
8. measure full target image/RAM/pool and close only with durable verification.

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
