# FM1Tracker Next Chat Handoff

Status: **ready to begin Phase 6 precontract work**  
Date: 2026-10-26

This file is a concise execution handoff. `START_HERE.md`, `PROJECT_CONTEXT.md` and `PROJECT_STATE.md` remain authoritative.

## Repository / branch

Repository: `joakimskurk/fm1tracker`  
Branch: `phase5/worst-case-benchmark`

Phase 5 is complete at software level. The last target-validated production-code checkpoint remains:

`81d3464059270130747e26fcc950b7a2e1bb03e1`

Post-closeout commits after that checkpoint are documentation/product-direction work and do not authorize new production behavior.

## What is complete

- Phase 1-4 complete.
- Phase 5 mixed-engine routing + FM4 complete at software level.
- WP5.10 / issue #53 closed.
- Worst-case mixed-engine benchmark and pinned target closeout passed.
- Eight logical tracker tracks preserved.
- Physical FM-1 timing/audio/free-RAM margin remains unverified because no physical unit is available.

## Accepted product direction that must survive Phase 6

Phase 6 persistence must not freeze the project into the current minimal runtime model.

Preserve:

1. one engine-agnostic tracker with 8-BIT, FM, future virtual-analog/subtractive and drums/percussion;
2. eight logical tracks without silently consuming neighboring tracks for polyphony;
3. versioned/extensible instrument identity and human-readable instrument names;
4. per-engine payloads that can grow without raw C-struct persistence;
5. future drum NOTE MAP data (`KEY / NOTE -> SOUND SLOT -> SOURCE`);
6. stable future sample/asset references without raw host paths or runtime pointers;
7. optional bounded multi-note/poly-row extension storage so monophonic rows remain compact;
8. four visible poly note lanes as the preferred first bounded editor target, without implementing poly playback yet;
9. independent lane NOTE/TIE/OFF semantics for future poly rows;
10. future SID-inspired chip refinement parameters such as character filter, drive/non-linearity and coherent channel/global FX;
11. factory instruments using the same versioned model as user instruments where practical;
12. deterministic bounded realtime behavior and explicit capacity/error handling.

Normative product-direction references:

- `PRODUCT_REFINEMENT_DIRECTION.md`
- `POLYPHONY_EDITOR_DIRECTION.md`
- `CHIP_SOUND_DIRECTION.md`
- `LIVE_PERFORMANCE_CAPTURE_VISION.md`

## Exact next work

**Do not start Phase 6 production persistence code immediately.**

The next chat should:

1. read `START_HERE.md` and all files it points to;
2. verify the branch HEAD before changing anything;
3. audit the existing pattern/song/order/runtime/storage surfaces relevant to Phase 6;
4. identify current assumptions that would block versioning, migration, corruption recovery, poly-row extensions, instrument payload growth, NOTE MAPs or stable asset references;
5. define the first bounded Phase 6 work package (expected to be a precontract/audit package) and create its tracking issue;
6. freeze the project format/versioning/migration/corruption-recovery contract before production implementation;
7. only then begin the first implementation package allowed by that contract.

## Explicit non-goals for the first Phase 6 package

Do not use the format work as permission to implement:

- PCM/sample playback;
- final drum DSP;
- polyphonic playback/editor code;
- SID/chip filter or FX DSP;
- HOME spectrum UI;
- final instrument editor;
- live performance capture.

Those require their own bounded contracts and measurements.

## Decision numbering

No D057 has been accepted yet. Assign the next decision number only when the first real Phase 6 architecture contract is accepted. Product-direction documents above are constraints for that decision, not substitutes for its audit/contract.

## Hardware evidence boundary

Keep these claims unchanged until physical evidence exists:

```text
physical_audio_stability=unverified
physical_cpu_headroom=unverified
physical_free_ram_margin=unverified
target_runtime_timing_gate=deferred_without_execution_path
hardware_margin_claim=none
```

## New-chat starter

A new chat can safely begin with:

> Continue FM1Tracker from repository `joakimskurk/fm1tracker`, branch `phase5/worst-case-benchmark`. Read `START_HERE.md` first and treat the repository as source of truth. Verify current HEAD, then continue exactly from the Phase 6 precontract action documented there. Do not reconstruct from old chat context and do not start production persistence code before the Phase 6 audit/contract is frozen.
