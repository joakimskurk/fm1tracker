# FM1Tracker Project State

This is the **single source of truth for current execution state**. Read `START_HERE.md` and `docs/PROJECT_CONTEXT.md` first so the active roadmap item is not mistaken for the whole product.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.9 complete; WP5.10 ACTIVE - 2026-10-25**

Active package: **WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit**  
Tracking issue: **#53**  
Branch: `phase5/worst-case-benchmark`  
Precontract audit: `docs/WP5_10_PRECONTRACT_AUDIT.md` - COMPLETE  
Benchmark contract: `docs/WP5_10_WORST_CASE_MIXED_BENCHMARK_CONTRACT.md` - FROZEN  
Contract gate: `tests/run_wp5_10_contract_freeze.sh` + `.github/workflows/wp5_10_contract_freeze.yml`

Last closed package: **WP5.9 - mixed 8-BIT + FM4 validation**  
Closed issue: **#52**  
Closeout evidence: `docs/WP5_9_CLOSEOUT_EVIDENCE.md`

Production remains frozen at `81d3464059270130747e26fcc950b7a2e1bb03e1`. Benchmark implementation must stay test/workflow/documentation-only unless a measured constraint later forces a separately frozen optimization sub-contract.

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

## Accepted resource baselines

```text
D056 head                849c8b27d8c1afa09f9c093b695985c64e0c6013
D056 image               79804 B
WP5.9 code checkpoint    81d3464059270130747e26fcc950b7a2e1bb03e1
WP5.9 run                40210173573 SUCCESS
WP5.9 target             121219510315 SUCCESS
WP5.9 host               121219510342 SUCCESS
WP5.9 image              79836 B
normal RAM               27792 B
pool                    113000 B
D056 -> WP5.9 image      +32 B
D056 -> WP5.9 RAM        +0 B
D056 -> WP5.9 pool       +0 B
hardware_margin_claim    none
```

Full parent evidence: `docs/WP5_8_CLOSEOUT_EVIDENCE.md` and `docs/WP5_9_CLOSEOUT_EVIDENCE.md`.

`81d3464059270130747e26fcc950b7a2e1bb03e1` remains the last target-validated production-code checkpoint. WP5.10 audit/contract commits do not supersede it as firmware validation evidence.

## Frozen WP5.10 benchmark

### S1 - mixed saturation

All eight logical tracks first create FM4 voices with full sustain and release 255, then NOTE OFF detaches all eight FM release tails, then all eight tracks start WAVETABLE-8 voices.

The measured S1 window is legal only after observation proves simultaneously:

```text
logical_tracks_available = 8
fm_non_idle_slots         = 8
bit8_non_idle_slots       = 8
bit8_owned_tracks         = 8
```

Then:

- warmup = 256 frames;
- measured window = 4096 frames exactly;
- all 4096 frames must remain 8+8;
- three complete reset-to-run repetitions must produce identical occupancy/state/sample evidence.

### S2 - full callback pressure

On the same 8+8 state, one 256-frame callback must begin with queue depth 32, consume the queue normally, render four audition voices and observe exactly 1024 audition `sine()` calls while retaining 8+8 engine occupancy. Three reset-to-run repetitions must be identical.

### Timing boundary

Host timing is informational only. Target HAL exposes 24 MHz TIMER4, but current automated target closeout does not execute on physical hardware.

`target_runtime_timing_gate=deferred_without_execution_path`

`hardware_margin_claim=none`

## Exact next action

1. require the WP5.10 contract-freeze workflow to pass on the frozen contract checkpoint;
2. implement S1/S2 through generated test-only source wrappers/probes without production changes;
3. add the dual-engine realtime audit;
4. run deterministic repeats and inherited WP5.9/D056/D055/all-8BIT regressions;
5. run the uninstrumented pinned target closeout and require exact image/RAM/pool evidence if production stayed frozen;
6. only if a real accepted constraint appears, stop and freeze a bounded optimization sub-contract;
7. otherwise record the Phase 5 software exit with physical performance debt explicit.
