# FM1Tracker Project State

This is the **single source of truth for current execution state**. Read `START_HERE.md` and `docs/PROJECT_CONTEXT.md` first so the active roadmap item is not mistaken for the whole product.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.9 complete; WP5.10 next - 2026-10-25**

Last closed package: **WP5.9 - mixed 8-BIT + FM4 validation**  
Closed issue: **#52**  
Closeout branch: `phase5/mixed-validation`  
Precontract audit: `docs/WP5_9_PRECONTRACT_AUDIT.md`  
Closeout evidence: `docs/WP5_9_CLOSEOUT_EVIDENCE.md`

Next package: **WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit**. WP5.10 is not implementation-active until its precontract audit and benchmark contract are frozen according to `docs/EXECUTION_PROTOCOL.md`.

## Persistent product direction

FM1Tracker remains one coherent engine-agnostic tracker with 8-BIT, FM, future virtual-analog/subtractive synthesis and drums/percussion in the same project environment. Future VA/drum DSP and resource architecture are not frozen yet and must receive their own audits/contracts. See `PROJECT_CONTEXT.md`.

## Completed Phase 5 packages

- WP5.0 / D048 mixed-engine routing - COMPLETE.
- WP5.1 / D049 fixed-point FM operator - COMPLETE.
- WP5.2 / D050 curated four-operator routing - COMPLETE.
- WP5.3 / D051 integer ratio/pitch mapping - COMPLETE.
- WP5.4 / D052 level/modulation-index mapping - COMPLETE.
- WP5.5 / D053 bounded OP4 feedback - COMPLETE.
- WP5.6 / D054 four independent FM4 ADSR envelopes - COMPLETE.
- WP5.7 / D055 FM voice allocation and D048 integration - COMPLETE.
- WP5.8 / D056 FM parameter pages and p-lock mapping - COMPLETE.
- WP5.9 mixed 8-BIT + FM4 validation - COMPLETE.

## Accepted WP5.8 parent baseline

```text
head                     849c8b27d8c1afa09f9c093b695985c64e0c6013
run                      40203483720 SUCCESS
host                     121199398344 SUCCESS
target                   121199398538 SUCCESS
image                    79804 B
normal RAM               27792 B
pool                    113000 B
FWSC SHA-256             4d2805f428f5114531b4c3d379576d2b207cbc9a6c04fcbdd5263cfaa0cc84a0
all-8BIT WAV             17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076
hardware_margin_claim    none
```

Full evidence: `docs/WP5_8_CLOSEOUT_EVIDENCE.md`.

## Accepted WP5.9 closeout

The WP5.9 audit began as a validation-gap package. Its new delayed cross-family fixture exposed a real target cleanup defect. The defect was documented before repair, then a separate frozen fix contract authorized only the narrow `firmware/src/dinghy.c` correction.

Final accepted production checkpoint and closeout:

```text
code checkpoint          81d3464059270130747e26fcc950b7a2e1bb03e1
run                      40210173573 SUCCESS
target                   121219510315 SUCCESS
host                     121219510342 SUCCESS
image                    79836 B
normal RAM               27792 B
pool                    113000 B
D056 -> WP5.9 image      +32 B
D056 -> WP5.9 RAM        +0 B
D056 -> WP5.9 pool       +0 B
hardware_margin_claim    none
```

Full evidence: `docs/WP5_9_CLOSEOUT_EVIDENCE.md`.

`81d3464059270130747e26fcc950b7a2e1bb03e1` remains the last target-validated WP5.9 production-code checkpoint. Documentation/handoff commits after it do not supersede it as firmware validation evidence.

## WP5.10 frozen direction before implementation

WP5.10 is the Phase 5 exit package. Per `docs/WORK_PACKAGES.md`, it must:

- begin with a precontract audit;
- freeze a realistic worst-case mixed-engine benchmark/load definition before optimization;
- preserve all eight logical tracker tracks;
- measure target/resource effects against the frozen D056/WP5.9 baseline;
- distinguish target/resource evidence from physical-audio stability;
- permit optimization only if the frozen benchmark exposes a real measured constraint;
- record an explicit Phase 5 exit decision.

No physical FM-1 is available. `hardware_margin_claim=none`.

## Exact next action

1. close WP5.9 issue `#52` with `docs/WP5_9_CLOSEOUT_EVIDENCE.md` as durable evidence;
2. create/activate WP5.10 from this documentation-only handoff;
3. perform the WP5.10 precontract audit before implementation changes;
4. freeze its benchmark/load contract and fail-closed verification path;
5. only then implement the benchmark and evaluate whether any production optimization is justified.
