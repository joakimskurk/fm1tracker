# FM1Tracker Project State

This is the **single source of truth for current execution state**. Read `START_HERE.md`, `docs/NEXT_CHAT_HANDOFF.md` and `docs/PROJECT_CONTEXT.md` first so roadmap order is not mistaken for the whole product.

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

Post-closeout documentation/product-direction commits do not change that production checkpoint.

## Persistent product direction

FM1Tracker remains one coherent engine-agnostic tracker with 8-BIT, FM, future virtual-analog/subtractive synthesis and drums/percussion in the same project environment. Future VA/drum DSP and resource architecture are not frozen yet and must receive their own audits/contracts. See `PROJECT_CONTEXT.md`.

The accepted pre-Phase-6 refinement constraints also include:

- polyphony stays inside one logical track rather than consuming neighboring tracker tracks;
- four visible note lanes are the preferred first bounded poly editor target;
- monophonic rows should remain compact, with optional bounded/versioned poly-row extension storage rather than blindly widening every `FmtStep`;
- future drum instruments require NOTE MAP representation (`KEY / NOTE -> SOUND SLOT -> SOURCE`);
- future project persistence must preserve human-readable instrument names, extensible per-engine payloads and stable asset/sample references;
- the 8-BIT family is intended to evolve in a SID-inspired, non-clone direction with character filter, drive/non-linearity and coherent modern FX under later bounded contracts;
- none of those product directions authorize their implementation during the first Phase 6 persistence contract package.

References: `PRODUCT_REFINEMENT_DIRECTION.md`, `POLYPHONY_EDITOR_DIRECTION.md`, `CHIP_SOUND_DIRECTION.md`, `LIVE_PERFORMANCE_CAPTURE_VISION.md`.

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

Phase 5 is closed. **Phase 6 - song/order/persistence - is the next planned phase, but no Phase 6 production implementation is authorized yet.**

The next work must be a bounded Phase 6 precontract/audit step:

1. audit the existing tracker/pattern/song/order/runtime/storage surfaces against the Phase 6 roadmap and current product-direction documents;
2. identify assumptions that could block explicit versioning, migration, corruption recovery, future poly-row extensions, instrument payload growth, NOTE MAPs or stable asset references;
3. define the first bounded Phase 6 work package and tracking issue;
4. freeze the project-format/versioning/migration/corruption-recovery contract before production changes;
5. keep the format engine-agnostic and preserve per-instrument engine identity;
6. preserve optional bounded multi-note/poly-row extension capability while keeping ordinary monophonic rows compact;
7. preserve extensible instrument names/payloads, future drum NOTE MAPs, future chip filter/drive/FX parameters and stable future sample/asset references without raw host paths or runtime pointers;
8. preserve all eight logical tracks and deterministic bounded realtime behavior;
9. do not implement PCM, poly playback/editor code, SID/chip filter/FX DSP, HOME spectrum UI, final instrument editor or live capture as part of the format precontract package.

No D057 has been accepted yet. The next decision number belongs to the first accepted Phase 6 architecture contract, not to roadmap prose alone.
