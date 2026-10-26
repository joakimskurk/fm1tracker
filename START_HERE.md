# FM1Tracker - Start Here

Repository state is authoritative, but roadmap order is not the whole project memory. Read persistent product context before reducing the project to the next work package.

Read in this order:

1. `docs/PROJECT_CONTEXT.md`
2. `docs/PRODUCT_REFINEMENT_DIRECTION.md`
3. `docs/POLYPHONY_EDITOR_DIRECTION.md`
4. `docs/CHIP_SOUND_DIRECTION.md`
5. `docs/PROJECT_STATE.md`
6. `docs/WP5_10_CLOSEOUT_EVIDENCE.md`
7. `docs/CURRENT_DECISIONS.md`
8. `docs/WORK_PACKAGES.md`
9. `docs/EXECUTION_PROTOCOL.md`
10. `docs/CURRENT_TEST_STATE.md`
11. `docs/TEST_MATRIX.md`
12. `docs/ROADMAP.md`
13. `docs/WP5_10_PRECONTRACT_AUDIT.md`
14. `docs/WP5_10_WORST_CASE_MIXED_BENCHMARK_CONTRACT.md`
15. `THIRD_PARTY.md`

## Product context first

FM1Tracker is one engine-agnostic hardware tracker/sequencer. The product direction includes 8-BIT, FM, future virtual-analog/subtractive synthesis and drums/percussion in the same tracker environment. Future engine families require their own audits/contracts.

The accepted refinement direction is now explicit: the existing working skeleton must be sharpened into a more immediate musical instrument. HOME/performance must eventually audition the real selected instrument rather than the legacy sine scaffold, expose compact eight-track activity plus a bounded spectrum visualization, and gain a coherent instrument-editor surface. Drum instruments must support arbitrary note-to-sound mapping through a future NOTE MAP architecture.

Polyphony is also explicit product direction: one logical track may contain several note lanes rather than consuming neighboring tracker tracks. The preferred first bounded editor target is four visible note lanes, with sparse/versioned extension storage rather than blindly widening every `FmtStep`. See `POLYPHONY_EDITOR_DIRECTION.md`.

The existing 8-BIT family must also be sharpened sonically. The accepted target is SID-inspired rather than SID-clone: stronger pulse/chip character, a bounded character filter, drive/non-linearity and coherent modern channel/global FX around the chip source. See `CHIP_SOUND_DIRECTION.md`.

## Current execution

**Phase 5 - mixed-engine routing + FM4 is COMPLETE at software level.**

**WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit is COMPLETE.**

```text
final Phase 5 issue             #53
branch                          phase5/worst-case-benchmark
production checkpoint           81d3464059270130747e26fcc950b7a2e1bb03e1
benchmark checkpoint            6f47d832b2fc4d83cd2ad03943e919a183123ee2
benchmark run                   40218677393 SUCCESS
closeout workflow checkpoint    bcf339b5aa49cf66e8f19917bbb2c48ebc974c56
closeout run                    40220923958 SUCCESS
image                           79836 B
normal RAM                      27792 B
pool                           113000 B
phase5_software_exit            pass
hardware_margin_claim           none
```

WP5.10 verification infrastructure did not change production firmware. `81d3464` remains the last target-validated production-code checkpoint.

## Accepted WP5.10 workload

S1 reaches and retains exact mixed saturation:

```text
8 logical tracks
8 non-idle FM4 release-tail slots
8 non-idle 8-BIT current slots
256 warmup frames
4096 measured frames
3 identical reset-to-run repetitions
```

S2 starts from the same 8+8 state and requires one 256-frame callback with queue depth 32 at entry and four audition voices, producing exactly 1024 audition `sine()` calls. It also repeats three times identically.

The ordinary pinned target build remains exact WP5.9 footprint: 79,836 B image, 27,792 B normal RAM and 113,000 B pool.

Host wall-clock data is informational only. Target TIMER4 runtime timing remains deferred until a reproducible execution-return path exists. Emulator timing is never a physical FM-1 margin claim.

```text
physical_audio_stability=unverified
physical_cpu_headroom=unverified
physical_free_ram_margin=unverified
target_runtime_timing_gate=deferred_without_execution_path
hardware_margin_claim=none
```

## Exact next action

Phase 6 - song/order/persistence - is the next planned phase. Do not start persistence production code from roadmap prose alone.

First:

1. audit the existing pattern, transport, engine-identity and storage surfaces;
2. define the first bounded Phase 6 work package and tracking issue;
3. freeze the project-format/versioning/migration/corruption-recovery contract before implementation;
4. keep the format engine-agnostic so future virtual-analog/subtractive synthesis and drums/percussion remain representable;
5. explicitly preserve extensible instrument identity/name, engine-specific payloads, future drum NOTE MAP data, future chip filter/drive/FX parameters and stable future sample/asset references without hard-coded host paths;
6. explicitly preserve an optional bounded multi-note/poly-row extension so monophonic rows remain compact while one logical track can later store several note lanes;
7. preserve eight logical tracks and deterministic bounded realtime behavior;
8. do not treat the refinement requirements as permission to implement PCM, poly playback, spectrum UI, chip filter/FX or a final instrument editor before their own bounded contracts exist.
