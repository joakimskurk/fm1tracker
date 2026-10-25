# WP5.10 Worst-case Mixed-engine Benchmark Contract

Status: **FROZEN - implementation may begin only after the contract-freeze gate is green**  
Date: 2026-10-25  
Issue: `#53`  
Branch: `phase5/worst-case-benchmark`  
Precontract audit: `docs/WP5_10_PRECONTRACT_AUDIT.md`  
Production baseline: `81d3464059270130747e26fcc950b7a2e1bb03e1`

## 1. Purpose

WP5.10 closes the Phase 5 mixed-engine stabilization work with a reproducible high-load benchmark, bounded realtime audit, inherited correctness regressions and exact uninstrumented target resource evidence.

This contract is benchmark-first and production-frozen. No production optimization is authorized by this contract.

`hardware_margin_claim=none`

## 2. Production freeze

Until a separately frozen optimization sub-contract exists, the following production surfaces must remain byte-for-byte identical to `81d3464059270130747e26fcc950b7a2e1bb03e1`:

- `firmware/src`
- `firmware/gen`
- `firmware/hal`
- `firmware/loader`
- `firmware/app.ld`
- `build.sh`
- `tools`

WP5.10 benchmark work may modify only tests, workflows, benchmark artifacts and documentation.

## 3. Frozen parent resource references

```text
D056 image                 = 79804 B
WP5.9 image                = 79836 B
normal RAM                 = 27792 B
vm40_pool                  = 113000 B
WP5.9 image delta vs D056  = +32 B
WP5.9 RAM delta vs D056    = +0 B
WP5.9 pool delta vs D056   = +0 B
hardware_margin_claim      = none
```

The final WP5.10 production build is uninstrumented. If production remains frozen, these WP5.9 production resource values are expected to remain exact.

## 4. Frozen realtime architecture facts

The benchmark assumes and verifies all of these facts rather than silently depending on them:

- tracker logical tracks = 8;
- 8-BIT voice slots = 8;
- FM4 voice slots = 8;
- manual/audition voices = 4;
- D034 realtime queue capacity = 32;
- target sample rate = 44,100 Hz;
- target audio half-buffer = 256 frames;
- target TIMER4 source = 24 MHz;
- tracker playback calls exactly one 8-BIT render and one FM4 render per audio frame;
- both family renderers use fixed eight-slot scans;
- a NOTE OFF detaches the family voice and clears route ownership without hard-silencing a valid release tail.

Any change in these facts fails the contract and requires re-audit before benchmark interpretation.

## 5. Benchmark instrumentation rule

Production source stays untouched.

Host benchmark instrumentation must follow the established WP4.10/WP5.9 pattern:

1. compile an exact test copy of the production entry translation unit;
2. generate test-only wrappers or host probes around selected functions;
3. observe counters, occupancy, state and audio without feeding benchmark state back into production logic;
4. prove the copied production entry source is byte-identical before instrumentation;
5. keep every observation hook outside production firmware.

Permanent firmware debug hooks are forbidden.

## 6. Scenario S1 - MIXED_SATURATION_8FM_8BIT

S1 is the required sustained mixed-engine saturation case.

### 6.1 Tracker fixture

Use all eight logical tracks. For each track `t = 0..7`:

- row 0: start one FM4 NOTE using a valid FM instrument;
- row 1: NOTE OFF;
- row 2: start one 8-BIT WAVETABLE-8 NOTE using a valid 8-BIT instrument;
- subsequent rows in the measurement window must not silence either family bank.

The FM base patch used for S1 must freeze:

- all four operator attack controls = 0;
- all four operator decay controls = 0;
- all four operator sustain controls = 255;
- all four operator release controls = 255;
- algorithm = `FMT_FM_ALGO_FAN3`;
- modulation index control = 255;
- feedback control = 255;
- all four operator levels non-zero;
- valid bounded ratio IDs for all four operators.

FAN3 is chosen because it renders all four operators and exercises the three-edge modulation accumulation path. The benchmark is not allowed to simplify FM4 to a cheaper single-operator or zero-modulation case.

The 8-BIT instrument used for S1 must be `FMT_8BIT_ENGINE_WAVETABLE8`, with non-zero gain and a sustained envelope for the full measurement window.

Notes may differ per track but must remain valid and deterministic.

### 6.2 Required occupancy

The benchmark begins its S1 measurement window only after test-only observation proves simultaneously:

```text
logical_tracks_available = 8
fm_non_idle_slots         = 8
bit8_non_idle_slots       = 8
bit8_owned_tracks         = 8
```

The eight FM4 slots are expected to be detached release tails created by the same eight logical tracks before row 2. The eight 8-BIT slots are the current row-2 track voices.

If exact `8 FM4 + 8 8-BIT` occupancy is not reached, S1 FAILS. Do not silently lower the workload.

### 6.3 Measurement window

After the first qualifying 8+8 frame:

- warmup: 256 frames;
- measured frames: 4096 exactly;
- the measurement is 16 complete 256-frame target-sized audio blocks;
- all 4096 measured frames must retain `fm_non_idle_slots=8` and `bit8_non_idle_slots=8`;
- all eight logical current routes must remain valid 8-BIT routes;
- no allocator invariant failure is allowed.

Run the complete S1 fixture three times from reset.

All three runs must produce identical:

- measured-frame count;
- occupancy histogram;
- tracker state digest;
- engine state digest;
- measured-window sample digest;
- final allocator ownership summary.

Wall-clock duration may be recorded, but it is informational only and is never an acceptance gate.

## 7. Scenario S2 - FULL_CALLBACK_PRESSURE

S2 proves the bounded non-engine work that can coexist with S1 saturation.

S2 starts from the same exact 8+8 mixed saturation state and must locate one target-sized 256-frame callback for which test-only host observation proves:

```text
queue_depth_at_callback_entry = 32
fm_non_idle_slots              = 8
bit8_non_idle_slots            = 8
```

The 32 queued events must be valid NOTE ON audition events. They are consumed by the ordinary D034 callback ingress. During that same callback:

- queue depth after consumption must be 0;
- four manual/audition voices must render;
- the audition `sine()` path must be observed exactly `4 * 256 = 1024` times;
- tracker engine occupancy must remain 8+8 for all 256 frames;
- no queue overflow is required for S2 itself;
- no allocator invariant failure is allowed.

Run S2 three times from reset and require identical callback sample digest, queue accounting and observation counters.

A separate inherited queue-overflow/recovery regression remains mandatory; S2 does not replace it.

## 8. Dual-engine realtime source audit

WP5.10 must add a static fail-closed realtime audit that proves at least:

- no heap allocation in the realtime mixed-engine path;
- callback contains exactly one 8-BIT render call and one FM4 render call per tracker frame;
- both engine renderers scan exactly eight fixed slots;
- FM4 rendering has no variable-length loop, sparse-store traversal, editor traversal, screen work or heap operation;
- 8-BIT rendering retains the same bounded properties;
- row boundary still wins over sub-row tick work;
- row and tick dispatch remain bounded to eight logical tracks;
- D034 callback ingress remains capped at 32 events;
- manual/audition voice count remains four;
- realtime code remains integer-only under the established no-FPU/no-libgcc rules.

## 9. Host evidence semantics

Host evidence is required for reproducibility and workload-shape proof.

Required host outputs:

- exact fixture identity/hash;
- S1 three-run deterministic digests and occupancy counts;
- S2 three-run deterministic digests and queue/audition counts;
- inherited WP5.9 mixed validation result;
- inherited all-8BIT / D055 / D056 correctness evidence;
- inherited queue overflow/recovery and allocator invariants;
- dual-engine realtime audit summary.

Host wall-clock timing may be printed as `informational_host_timing=1`. It may not trigger production optimization and may not be described as target CPU headroom.

## 10. Target and emulator timing semantics

`firmware/hal/fm1_time.h` exposes free-running 24 MHz TIMER4 through `fm1_ticks()`.

For 256 frames at 44.1 kHz, the nominal half-buffer interval is approximately:

```text
half_buffer_us             = 5804.99 us
nominal_timer4_ticks       = 139320 ticks approximately
```

However, current automated GitHub target closeout builds firmware but does not execute it on a physical FM-1.

Therefore:

- `target_runtime_timing_gate=deferred_without_execution_path`;
- a later test-only TIMER4 benchmark is allowed only if a reproducible execution-return path exists;
- online-emulator results must be labeled `emulator_timing`, never physical target timing;
- no host or emulator timing result may set `hardware_margin_claim` to anything other than `none`;
- absence of runtime target timing does not authorize speculative optimization.

## 11. Uninstrumented target closeout gate

The pinned JieLi/AC79 target build must use the ordinary uninstrumented production firmware.

If no optimization sub-contract was opened, closeout requires exactly:

```text
image       = 79836 B
normal RAM  = 27792 B
pool        = 113000 B
```

Any production footprint change fails closeout and must be explained before Phase 5 exit.

The workflow must also record package hashes and the pinned SDK/toolchain identity as evidence, but hash equality to WP5.9 is not assumed unless the build process proves deterministic identity.

## 12. Mandatory inherited regressions

WP5.10 must keep green:

1. exact all-8BIT production regression;
2. D055 default/no-lock FM behavior;
3. D056 locked-FM semantics;
4. WP5.9 simultaneous real-family mixed oracle;
5. WP5.9 same-track switch behavior;
6. WP5.9 delayed cross-family defect regression in both directions and unsupported-family fail-to-silence timing;
7. D043 ownership invariants and zero active cross-track primary stealing;
8. D034 queue overflow/recovery bounds;
9. deterministic repeated rendering.

## 13. Optimization trigger

WP5.10 is not an optimization package by default.

Production work must stop and a separate optimization sub-contract must be frozen before any production source change if and only if accepted evidence exposes a real constraint such as:

- a realtime path becomes structurally unbounded;
- the uninstrumented target resource result violates the frozen production baseline/ceiling unexpectedly;
- a reproducible target runtime path later demonstrates a deadline failure attributable to the production mixed-engine path.

Host wall-clock speed alone is not an optimization trigger. Emulator timing alone is not a physical-hardware optimization trigger.

## 14. Phase 5 exit decision

If S1, S2, realtime audit, inherited regressions and the uninstrumented target closeout all pass without an optimization trigger, WP5.10 records:

```text
phase5_software_exit=pass
physical_audio_stability=unverified
physical_cpu_headroom=unverified
physical_free_ram_margin=unverified
hardware_margin_claim=none
```

This closes mixed-engine software stabilization while carrying physical performance validation forward as explicit hardware-phase debt.

## 15. Contract acceptance

Before benchmark implementation starts, the contract-freeze gate must prove:

- production delta from `81d3464` is zero;
- architecture constants and release-tail prerequisite still match this contract;
- inherited WP5.9 mixed host gate remains green;
- S1 requires exact 8+8 occupancy for 4096 measured frames after 256 warmup frames;
- S2 requires queue depth 32, 8+8 engine occupancy and exactly 1024 audition sine renders in one 256-frame callback;
- three deterministic repetitions are required for both scenarios;
- target runtime timing remains deferred without a reproducible execution path;
- production optimization remains forbidden without a separate sub-contract;
- `hardware_margin_claim=none` remains explicit.

**WP5.10 benchmark implementation may begin only after this freeze gate is green.**
