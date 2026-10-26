# FM1Tracker Work Packages

`PROJECT_CONTEXT.md` preserves product intent. `PROJECT_STATE.md` is the single source of truth for current execution. This file defines bounded package scope and exit criteria.

## Rules

1. One active package at a time.
2. A package closes only with durable verification or an explicit recorded waiver.
3. Do not pull later-package features forward.
4. Architectural decisions are recorded before implementation.
5. Realtime work remains bounded and deterministic.
6. Eight logical tracks may not be silently reduced.
7. Roadmap focus must not erase persistent product directions such as future virtual-analog/subtractive synthesis or drums/percussion.
8. Substantive user decisions are synchronized back into Git according to `EXECUTION_PROTOCOL.md`.
9. Pre-Phase-6 product constraints in `PRODUCT_REFINEMENT_DIRECTION.md`, `POLYPHONY_EDITOR_DIRECTION.md` and `CHIP_SOUND_DIRECTION.md` must be considered when freezing persistence, but do not themselves authorize later DSP/UI features.

## Phase 1-4 - COMPLETE

Canonical D046 exit: run `40124536375`, job `120963022289`.

## Phase 5 - mixed-engine routing + FM4 - COMPLETE at software level

Parent issue: #42.  
Final package: WP5.10 / #53.  
Closeout evidence: `WP5_10_CLOSEOUT_EVIDENCE.md`.

### WP5.0 - mixed-engine routing - COMPLETE - issue #43 - D048
### WP5.1 - fixed-point FM operator - COMPLETE - issue #44 - D049
### WP5.2 - curated FM4 algorithm routing - COMPLETE - issue #45 - D050
### WP5.3 - integer ratio table + pitch mapping - COMPLETE - issue #46 - D051
### WP5.4 - levels / modulation index - COMPLETE - issue #47 - D052
### WP5.5 - bounded feedback - COMPLETE - issue #48 - D053
### WP5.6 - FM envelope strategy - COMPLETE - issue #49 - D054
### WP5.7 - FM voice allocation/integration - COMPLETE - issue #50 - D055

D055 integrates D049-D054 as eight bounded family-local FM voices through D048 while preserving exact D043 ownership semantics. Final closeout `80587f12786122dcec819260ddad5dd5db4b1005`, run `40177151084` SUCCESS.

### WP5.8 - FM parameter pages + p-lock mapping - COMPLETE - issue #51 - D056

D056 integrates FM4 with the shared semantic parameter, sparse p-lock, row-cache and held-step editor architecture while preserving exact all-8BIT behavior.

Accepted closeout:

```text
head          849c8b27d8c1afa09f9c093b695985c64e0c6013
run           40203483720 SUCCESS
host          121199398344 SUCCESS
target        121199398538 SUCCESS
image         79804 B
normal RAM    27792 B
pool         113000 B
FWSC SHA      4d2805f428f5114531b4c3d379576d2b207cbc9a6c04fcbdd5263cfaa0cc84a0
```

Evidence: `WP5_8_CLOSEOUT_EVIDENCE.md`.

### WP5.9 - mixed 8-BIT + FM4 validation - COMPLETE - issue #52

Accepted closeout:

```text
code checkpoint  81d3464059270130747e26fcc950b7a2e1bb03e1
run              40210173573 SUCCESS
target           121219510315 SUCCESS
host             121219510342 SUCCESS
image            79836 B
normal RAM       27792 B
pool            113000 B
D056 delta       +32 B image / +0 B RAM / +0 B pool
```

The validation-first package exposed delayed cross-family defect 001 before repair. A separately frozen fix contract authorized only the narrow `firmware/src/dinghy.c` correction. All eight logical tracks remain intact. Full evidence: `WP5_9_CLOSEOUT_EVIDENCE.md`.

### WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit - COMPLETE - issue #53

Purpose: benchmark the bounded mixed-engine production path under realistic high-load conditions and make an evidence-based Phase 5 exit decision without silently reducing the eight logical tracker tracks.

The precontract audit and frozen benchmark contract established two required workloads:

1. S1 exact mixed saturation: eight non-idle FM4 release-tail slots plus eight non-idle current 8-BIT slots, 256-frame warmup, 4096-frame measured window, three deterministic repetitions.
2. S2 full callback pressure: same exact 8+8 state, queue depth 32 at callback entry, four audition voices, exactly 1024 audition `sine()` calls in one 256-frame callback, three deterministic repetitions.

Accepted benchmark:

```text
checkpoint       6f47d832b2fc4d83cd2ad03943e919a183123ee2
run              40218677393 SUCCESS
job              121245277129 SUCCESS
production delta 0
```

Accepted pinned target/Phase 5 closeout:

```text
workflow checkpoint       bcf339b5aa49cf66e8f19917bbb2c48ebc974c56
run                       40220923958 SUCCESS
host                      121252114781 SUCCESS
target                    121252114877 SUCCESS
phase5 software exit      121252369061 SUCCESS
AC79 SDK                   d179b4484759423312073f5fbb232501aa491047
production checkpoint     81d3464059270130747e26fcc950b7a2e1bb03e1
image                     79836 B
normal RAM                 27792 B
pool                      113000 B
production delta          0
```

No optimization sub-contract was required. All eight logical tracks are preserved. Full evidence: `WP5_10_CLOSEOUT_EVIDENCE.md`.

Phase 5 exit state:

```text
phase5_software_exit=pass
physical_audio_stability=unverified
physical_cpu_headroom=unverified
physical_free_ram_margin=unverified
target_runtime_timing_gate=deferred_without_execution_path
hardware_margin_claim=none
```

The Phase 5 software exit is complete. Physical timing/audio/free-RAM validation remains blocked until a physical FM-1 is available.

## Mandatory core milestone - live performance capture + performance FX

PLANNED after engine foundations. Reference `LIVE_PERFORMANCE_CAPTURE_VISION.md`. Drums remain a required playable/capturable direction even though their engine implementation was not part of Phase 5.

## Future engine-family expansion

Virtual-analog/subtractive synthesis and drums/percussion are explicit FM1Tracker product directions in the same tracker environment. They require dedicated CPU/RAM/parameter/license audits and frozen work packages before production implementation.

## Phase 6 - song/order/persistence - NEXT, PRECONTRACT REQUIRED

No Phase 6 production code is authorized by the Phase 5 closeout.

The first Phase 6 package must be a bounded audit/precontract package. It must inspect existing tracker/pattern/song/order/runtime/storage assumptions and freeze a versioned project representation before implementation.

The contract must cover at minimum:

- order/pattern semantics and deterministic bounds;
- explicit project-format identity and versioning;
- migration and unknown/newer-block behavior;
- validation, CRC/checksums and corruption recovery/failure behavior;
- save/load boundaries and failure atomicity appropriate to available storage;
- preservation of per-instrument engine family + engine identity;
- human-readable instrument names and extensible per-engine payloads;
- optional bounded multi-note/poly-row extension representation while ordinary mono rows stay compact;
- future four-lane poly editor semantics without implementing poly playback yet;
- future drum NOTE MAP representation;
- stable future sample/asset references without raw host paths or runtime pointers;
- room for future chip character-filter/drive/FX parameters without freezing the current minimal 8-BIT payload forever;
- factory instruments fitting the same versioned model where practical;
- preservation of all eight logical tracks and bounded realtime behavior.

The first Phase 6 package must explicitly exclude implementation of PCM/sample playback, drum DSP, poly playback/editor code, SID/chip filter/FX DSP, HOME spectrum UI, final instrument editor and live performance capture.

No D057 is accepted yet. The first Phase 6 architecture decision receives the next number only after the audit/contract is accepted.

Reference handoff: `NEXT_CHAT_HANDOFF.md`.

## Phase 7 - physical hardware UX freeze - BLOCKED
## Phase 8 - interoperability - PLANNED
## Phase 9 - optional extensions - DEFERRED

`hardware_margin_claim=none` until physical FM-1 evidence exists.
