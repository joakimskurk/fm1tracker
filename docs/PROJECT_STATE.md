# FM1Tracker Project State

This is the **single source of truth for current execution state**.

Read `START_HERE.md` first.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.4 complete; WP5.5 ACTIVE; D053 accepted for implementation - 2026-10-25**

Current active package: **WP5.5 - bounded deterministic FM4 feedback**  
Active issue: **#48**  
Active branch: `phase5/fm4-feedback`  
Parent closeout: **WP5.4 / D052** at `d3501eca952f3e903d4dcc4faab2851d439f7ad0`

D053 is frozen and its source-absent gate is accepted. Production implementation is now authorized only for `firmware/src/fmt_fm_feedback.h/.c` under the frozen contract.

Do not begin WP5.6 envelopes, WP5.7 voice integration, WP5.8 tracker parameter pages, WP5.9 audible mixed integration or WP5.10 Phase 5 exit while WP5.5 is active.

## Completed Phase 5 baseline

- WP5.0 / D048 mixed-engine routing - COMPLETE.
- WP5.1 / D049 fixed-point FM operator - COMPLETE.
- WP5.2 / D050 curated four-operator routing - COMPLETE.
- WP5.3 / D051 integer ratio/pitch mapping - COMPLETE.
- WP5.4 / D052 operator level/modulation-index mapping - COMPLETE.

WP5.4 final closeout: `d3501eca952f3e903d4dcc4faab2851d439f7ad0`; final run `40163616219`, host `121080077905`, target `121080077699`, both SUCCESS.

## D053 accepted preimplementation checkpoint

Normative contract: `docs/WP5_5_FM_FEEDBACK_CONTRACT.md`.  
Precontract audit: `docs/WP5_5_PRECONTRACT_AUDIT.md`.  
Accepted evidence: `docs/WP5_5_PREIMPLEMENTATION_EVIDENCE.md`.

Canonical source-absent gate:

- head `10bf0e3bf8ef49253666bee999b2de95de10046d`;
- run `40164610821`;
- job `121082987705`;
- artifact ID `12587966076`;
- artifact SHA-256 `0eb34eb28a92f69eeeb653ddae2661ca62d72e8eaf887a881e227fbe26c7e4b9`;
- conclusion SUCCESS.

Frozen D053 V1:

```text
previous OP4 level-scaled sample
    -> FEEDBACK byte 0..255
    -> (v << 6) | (v >> 2) Q14 depth
    -> sample * depth * 4 phase offset
    -> next OP4 D049 render
    -> current OP4 sample becomes history
```

Properties:

- OP4 self-feedback only;
- exactly one-sample delay;
- exactly 2 B signed-16 history;
- FEEDBACK separate from D052 INDEX;
- history tracks OP4 even at FEEDBACK=0;
- reset/retrigger clears history;
- D050 algorithm switch retains history;
- full signed-16 history x all 256 controls signed-32 safe;
- D049-D052 and `dinghy.c` remain unchanged.

## Current exact action

Implement only:

```text
firmware/src/fmt_fm_feedback.h
firmware/src/fmt_fm_feedback.c
```

Then add focused host verification and a WP5.5 verify workflow that must prove:

1. 2 B state and exact D053 control/null/reset semantics;
2. canonical 4,096-render OP4 hash;
3. test-only four-algorithm composition and switch continuity exact;
4. extreme D051/D052 stress exact;
5. no loops, runtime division/modulo, int64, float, heap, table or mutable global state in D053 source;
6. D049-D052 and `dinghy.c` remain unchanged;
7. PI32v2 `fmt_fm_feedback.o` text+rodata <= 1,024 B;
8. inherited WP5.4 regressions exact;
9. full target remains exact WP5.0 while D053 is isolated.

## Full target identity at WP5.5 entry

```text
image       72792 B
normal RAM  19676 B
pool RAM   113000 B
identity   FM-1_987
BIN SHA    a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125
FWSC SHA   cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073
```

No physical FM-1 is available. `hardware_margin_claim=none`.
