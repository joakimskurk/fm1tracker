# WP5.10 Closeout Evidence

Date: 2026-10-25  
Package: **WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit**  
Issue: **#53**  
Branch: `phase5/worst-case-benchmark`

## Decision

WP5.10 is **COMPLETE** and the **Phase 5 software exit passes**.

No production optimization was required. All eight logical tracker tracks remain intact. The target-validated production source remains the WP5.9 code checkpoint:

`81d3464059270130747e26fcc950b7a2e1bb03e1`

WP5.10 audit, benchmark, workflow and documentation commits are verification infrastructure and do not supersede that production-code checkpoint.

## Frozen benchmark evidence

Accepted benchmark checkpoint:

`6f47d832b2fc4d83cd2ad03943e919a183123ee2`

Accepted benchmark run:

```text
run   40218677393 SUCCESS
job   121245277129 SUCCESS
```

The benchmark reuses the frozen contract in `docs/WP5_10_WORST_CASE_MIXED_BENCHMARK_CONTRACT.md` and requires production source to remain byte-for-byte equal to `81d3464` across the frozen production boundary.

### S1 - mixed saturation

Accepted workload:

```text
logical tracks             8
FM4 non-idle slots         8
8-BIT non-idle slots       8
8-BIT owned tracks         8
warmup                     256 frames
measured window            4096 frames
repetitions                3
deterministic repeats      PASS
```

All 4096 measured frames retain exact 8 FM4 + 8 8-BIT occupancy in every repetition.

### S2 - full callback pressure

Accepted workload:

```text
engine occupancy           exact 8 FM4 + 8 8-BIT
callback                   256 frames
queue depth at entry       32
queue depth after consume  0
audition voices            4
audition sine calls        1024
repetitions                3
deterministic repeats      PASS
```

The host closeout also re-runs the frozen contract gate, the dual-engine realtime audit and inherited WP5.9/D056/D055/all-8BIT regressions.

Host wall-clock timing is informational only and is not a physical performance gate.

## Pinned target closeout

Closeout workflow checkpoint:

`bcf339b5aa49cf66e8f19917bbb2c48ebc974c56`

Closeout workflow:

```text
run                     40220923958 SUCCESS
closeout-host           121252114781 SUCCESS
closeout-target         121252114877 SUCCESS
phase5-software-exit    121252369061 SUCCESS
```

Pinned AC79 SDK commit:

`d179b4484759423312073f5fbb232501aa491047`

The target job first proves `production_delta=0` against `81d3464`, then builds the ordinary uninstrumented production target with the pinned JieLi/AC79 toolchain and requires the exact accepted footprint.

### Target footprint

```text
image                    79836 B
normal RAM               27792 B
pool                    113000 B
image limit            2097152 B
image margin           2017316 B
D056 -> WP5.10 image       +32 B
D056 -> WP5.10 RAM           0 B
D056 -> WP5.10 pool          0 B
WP5.9 -> WP5.10 image        0 B
WP5.9 -> WP5.10 RAM          0 B
WP5.9 -> WP5.10 pool         0 B
production delta             0
```

The image margin above is a linker/image-space result only. It is not a claim about physical realtime headroom or physical free RAM.

### Built artifact hashes

```text
fm1tracker.bin
17d8a9ad757a9d34908a17c03a0491c9e9be8aae0d1c1573edbf73766cb9d2e5

fm1tracker-0.1.fwsc
741036f46510f166a07bad2ba9a8dab5ed3e99917ce72fdbc697c736aec2a58d
```

## Phase 5 software exit

The final dependent closeout job only runs after both host and target jobs pass. Accepted exit state:

```text
WP5_10_PHASE5_SOFTWARE_EXIT_OK=1
phase5_software_exit=pass
physical_audio_stability=unverified
physical_cpu_headroom=unverified
physical_free_ram_margin=unverified
target_runtime_timing_gate=deferred_without_execution_path
hardware_margin_claim=none
```

No physical FM-1 is available, so this closeout intentionally makes no physical audio-stability, callback-margin, CPU-headroom or free-RAM claim. Emulator or host timing must not be relabeled as physical evidence.

## Phase 5 exit conclusion

Phase 5 closes at software level with:

- common mixed-engine routing complete;
- FM4 engine family integrated and parameterized;
- mixed 8-BIT + FM4 behavior validated;
- exact worst-case 8+8 family-local occupancy demonstrated through ordinary tracker/release-tail semantics;
- full bounded callback pressure demonstrated with queue and audition load;
- deterministic repeated host evidence green;
- dual-engine realtime audit green;
- inherited regressions green;
- ordinary pinned target build green at the exact WP5.9 footprint;
- no optimization sub-contract required;
- all eight logical tracker tracks preserved;
- physical performance debt explicit.

Phase 6 song/order/persistence remains the next planned phase. Future virtual-analog/subtractive synthesis and drums/percussion remain persistent product directions and require their own audits/contracts before production implementation.

`hardware_margin_claim=none`
