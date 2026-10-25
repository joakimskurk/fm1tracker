# FM1Tracker Project State

This is the **single source of truth for current execution state**. Read `START_HERE.md` and `docs/PROJECT_CONTEXT.md` first so the active roadmap item is not mistaken for the whole product.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.9 complete; WP5.10 ACTIVE - 2026-10-25**

Active package: **WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit**  
Tracking issue: **#53**  
Branch: `phase5/worst-case-benchmark`  
Precontract audit: `docs/WP5_10_PRECONTRACT_AUDIT.md` - COMPLETE  
Benchmark contract: `docs/WP5_10_WORST_CASE_MIXED_BENCHMARK_CONTRACT.md` - NEXT

Last closed package: **WP5.9 - mixed 8-BIT + FM4 validation**  
Closed issue: **#52**  
Closeout evidence: `docs/WP5_9_CLOSEOUT_EVIDENCE.md`

No WP5.10 benchmark implementation or production optimization is authorized until the benchmark contract and its fail-closed freeze gate are committed.

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

## WP5.10 precontract audit result

The audit inspected the real linked production callback and inherited benchmark infrastructure.

Frozen findings:

- `app_audio()` renders both family engines once per tracker frame, then advances the shared tracker/control path;
- 8-BIT and FM4 each own a fixed eight-slot voice bank while the tracker remains eight logical tracks;
- detached release tails mean a realistic mixed load must prove actual family-local occupancy rather than assume eight logical tracks equal eight total DSP voices;
- the contract must attempt a reachable 8 FM4 + 8 8-BIT non-idle saturation window and record the measured reachable maximum if ordinary tracker semantics cannot reach the full 8+8 state;
- the same callback can additionally process up to 32 queued audition events and four manual/audition voices, so engine saturation and full-callback pressure must be separate evidence cases;
- WP4.10 test-only wrapper/digest instrumentation and WP5.9 source-injected mixed fixtures are the preferred non-invasive mechanisms;
- existing realtime audits must be extended to both final engine renderers;
- target HAL exposes a free-running 24 MHz TIMER4 via `fm1_ticks()`, but current automated target closeout does not execute firmware on physical hardware;
- host timing is informational only, emulator timing must be labeled emulator timing, and no such number is a physical FM-1 margin claim;
- uninstrumented production resource evidence stays anchored to D056/WP5.9;
- optimization remains forbidden unless the frozen benchmark exposes a real measured constraint.

`hardware_margin_claim=none`

## Exact next action

1. freeze `docs/WP5_10_WORST_CASE_MIXED_BENCHMARK_CONTRACT.md` from the completed audit;
2. add a fail-closed contract-freeze verification gate;
3. commit that contract checkpoint before benchmark implementation;
4. implement test-only mixed saturation/full-callback benchmark evidence without production changes;
5. run inherited regressions and pinned target resource closeout;
6. only if measured evidence exposes a real constraint, stop and freeze a separate bounded optimization sub-contract;
7. record the Phase 5 exit decision.
