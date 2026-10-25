# FM1Tracker Project State

This is the **single source of truth for current execution state**. Read `START_HERE.md` and `docs/PROJECT_CONTEXT.md` first so roadmap order is not mistaken for the whole product.

## Current phase

**Phase 5 - mixed-engine routing + FM4 - COMPLETE at software level - 2026-10-25**

Phase 5 software exit: **PASS**  
Final package: **WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit - COMPLETE**  
Closed issue: **#53**  
Branch: `phase5/worst-case-benchmark`  
Closeout evidence: `docs/WP5_10_CLOSEOUT_EVIDENCE.md`

WP5.10 benchmark checkpoint: `6f47d832b2fc4d83cd2ad03943e919a183123ee2`.  
WP5.10 closeout workflow checkpoint: `bcf339b5aa49cf66e8f19917bbb2c48ebc974c56`.  
Neither checkpoint changes production firmware.

The last target-validated production-code checkpoint remains:

`81d3464059270130747e26fcc950b7a2e1bb03e1`

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
- WP5.10 worst-case mixed-engine benchmark + Phase 5 software exit - COMPLETE.

## Accepted production resources

```text
D056 head                  849c8b27d8c1afa09f9c093b695985c64e0c6013
D056 image                 79804 B
production checkpoint      81d3464059270130747e26fcc950b7a2e1bb03e1
WP5.10 target image        79836 B
normal RAM                 27792 B
pool                      113000 B
D056 -> final image          +32 B
D056 -> final RAM              0 B
D056 -> final pool             0 B
WP5.9 -> final resources       0 B
hardware_margin_claim      none
```

Full evidence: `docs/WP5_8_CLOSEOUT_EVIDENCE.md`, `docs/WP5_9_CLOSEOUT_EVIDENCE.md` and `docs/WP5_10_CLOSEOUT_EVIDENCE.md`.

## WP5.10 accepted benchmark

### S1 - mixed saturation

The frozen workload reaches and retains simultaneously:

```text
logical_tracks_available = 8
fm_non_idle_slots         = 8
bit8_non_idle_slots       = 8
bit8_owned_tracks         = 8
warmup                    = 256 frames
measured                  = 4096 frames
repetitions               = 3 identical
```

### S2 - full callback pressure

From the same exact 8+8 state, a 256-frame callback begins with queue depth 32, consumes all 32 valid audition NOTE ON events, renders four audition voices and observes exactly 1024 audition `sine()` calls while retaining 8+8 engine occupancy. Three reset-to-run repetitions are identical.

Accepted benchmark run:

```text
run   40218677393 SUCCESS
job   121245277129 SUCCESS
```

## Pinned target closeout

```text
workflow checkpoint       bcf339b5aa49cf66e8f19917bbb2c48ebc974c56
run                       40220923958 SUCCESS
host                      121252114781 SUCCESS
target                    121252114877 SUCCESS
phase5 exit               121252369061 SUCCESS
AC79 SDK                   d179b4484759423312073f5fbb232501aa491047
production delta           0
image                      79836 B
normal RAM                 27792 B
pool                      113000 B
```

Built target hashes are recorded in `docs/WP5_10_CLOSEOUT_EVIDENCE.md`.

## Phase 5 exit boundary

```text
phase5_software_exit=pass
physical_audio_stability=unverified
physical_cpu_headroom=unverified
physical_free_ram_margin=unverified
target_runtime_timing_gate=deferred_without_execution_path
hardware_margin_claim=none
```

Host timing is informational only. No physical FM-1 exists for the required physical timing/audio/free-RAM evidence.

## Exact next action

Phase 5 is closed. Phase 6 - song/order/persistence - is the next planned phase, but no Phase 6 production implementation is authorized by this closeout.

Before implementation:

1. audit the existing tracker/pattern/runtime/persistence surfaces against the Phase 6 roadmap;
2. define the first bounded Phase 6 work package and tracking issue;
3. freeze its data-format/migration/corruption-recovery contract before production changes;
4. preserve per-instrument engine identity and the common multi-engine tracker model;
5. keep future virtual-analog/subtractive synthesis and drums/percussion representable rather than hard-coding an FM-only project format.
