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

### WP5.5 - bounded feedback - TECHNICALLY COMPLETE / CLOSEOUT - issue #48 - D053

D053 froze and verified:

- one OP4 self-feedback edge;
- exactly one-sample history;
- exactly 2 B feedback state per future FM voice;
- one distinct FEEDBACK byte 0..255;
- exact Q14 mapping `(v << 6) | (v >> 2)`;
- signed phase offset `previous * depth * 4`;
- reset/retrigger clear history;
- D050 algorithm switches retain history;
- full signed-int16 x 256 control arithmetic proof;
- isolated production source only `fmt_fm_feedback.h/.c`.

Verified:

- source-absent run `40164610821`, job `121082987705`;
- implementation run `40164953532`, host `121083999893`, target `121083999790`;
- PI32v2 object 130 B, SHA `147c623c49fa8756f36937589d96f2da5139b6e3d1a09790e95d91c5f770256d`;
- full application remains exact WP5.0;
- D049-D052 and `dinghy.c` unchanged.

Evidence: `WP5_5_VERIFICATION.md`.

Final closeout-head rerun is required before #48 closes.

### WP5.6 - FM envelope strategy - PLANNED / NEXT - issue #49

Goal: choose the smallest bounded deterministic envelope architecture that still enables useful FM timbral evolution, using measured state and realtime cost before WP5.7 integration.

Required order:

1. audit actual D049-D053 envelope needs;
2. compare common amplitude envelope versus per-operator envelopes;
3. choose bounded V1 architecture from musical capability and measured cost;
4. freeze stage, time, level, reset/retrigger and zero-time semantics;
5. freeze sample/tick update cadence and realtime bounds;
6. define deterministic vectors and long stress;
7. freeze state and PI32v2 ceilings;
8. freeze D054 before production source;
9. pass source-absent gate;
10. only then implement and verify;
11. write `WP5_6_VERIFICATION.md` before WP5.7.

Out of scope:

- FM voice/D048 integration, WP5.7;
- FM tracker pages/p-lock IDs, WP5.8;
- audible mixed 8-BIT + FM4, WP5.9.

D054 does not exist yet.

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
