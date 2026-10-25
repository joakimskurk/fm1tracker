# FM1Tracker Project State

This is the **single source of truth for current execution state**.

Read `START_HERE.md` first.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.4 complete; WP5.5 ACTIVE; D053 frozen source-absent gate pending - 2026-10-25**

Current active package: **WP5.5 - bounded deterministic FM4 feedback**  
Active issue: **#48**  
Active branch: `phase5/fm4-feedback`  
Parent closeout: **WP5.4 / D052** at `d3501eca952f3e903d4dcc4faab2851d439f7ad0`

D053 is now frozen in `WP5_5_FM_FEEDBACK_CONTRACT.md` before production feedback source exists. No `fmt_fm_feedback.*` production source is permitted until the source-absent D053 gate is green and durable preimplementation evidence is recorded.

Do not begin WP5.6 envelopes, WP5.7 voice integration, WP5.8 tracker parameter pages, WP5.9 audible mixed integration or WP5.10 Phase 5 exit while WP5.5 is active.

## Completed Phase 5 baseline

- WP5.0 / D048 mixed-engine routing - COMPLETE.
- WP5.1 / D049 fixed-point FM operator - COMPLETE.
- WP5.2 / D050 curated four-operator routing - COMPLETE.
- WP5.3 / D051 integer ratio/pitch mapping - COMPLETE.
- WP5.4 / D052 operator level/modulation-index mapping - COMPLETE.

WP5.4 final closeout: `d3501eca952f3e903d4dcc4faab2851d439f7ad0`; final run `40163616219`, host `121080077905`, target `121080077699`, both SUCCESS.

## D053 frozen contract - preimplementation

Chosen V1 feedback model:

```text
previous OP4 level-scaled sample
    -> FEEDBACK byte 0..255
    -> exact byte-to-Q14 expansion
    -> sample * depth * 4 phase offset
    -> next OP4 D049 render
    -> current OP4 sample becomes history
```

Frozen properties:

- OP4 self-feedback only;
- exactly one-sample delay;
- exactly 2 B signed-16 history state;
- FEEDBACK is a distinct byte control, not D052 INDEX reuse;
- numeric feedback mapping equals D052 INDEX expansion `(v << 6) | (v >> 2)`;
- history updates even when FEEDBACK=0;
- reset/retrigger clears history;
- algorithm switch does not clear history;
- D049/D050/D051/D052 production sources remain unchanged;
- feedback production source, after authorization, is limited to `fmt_fm_feedback.h/.c`;
- full signed-16 history x all 256 controls is signed-32 safe.

Precontract audit: `docs/WP5_5_PRECONTRACT_AUDIT.md`.  
Normative contract: `docs/WP5_5_FM_FEEDBACK_CONTRACT.md`.

## Current exact action

Run and accept the source-absent D053 contract gate. It must prove:

1. feedback production source absent;
2. D049-D052 and `dinghy.c` unchanged from WP5.4 closeout;
3. exact 256-value control mapping;
4. exhaustive 16,777,216 history/control arithmetic surface;
5. canonical one-sample OP4 fixture;
6. all four test-only D050 feedback compositions;
7. algorithm-switch history continuity;
8. 65,536-sample extreme D051/D052 stress per algorithm;
9. inherited WP5.4 host regressions exact.

Only after that gate and durable preimplementation evidence may `fmt_fm_feedback.h/.c` be implemented.

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
