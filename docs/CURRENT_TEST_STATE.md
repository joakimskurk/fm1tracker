# FM1Tracker Current Test State

Date: 2026-10-25  
Purpose: **current verification overlay**

Legend: ✅ verified/frozen, ⬜ planned/future, ⛔ physical unavailable.

## Completed baselines

- Phase 1-3: COMPLETE ✅
- D046 Phase 4 all-8BIT baseline: COMPLETE ✅ run `40124536375`, job `120963022289`
- D048 WP5.0 through D056 WP5.8: COMPLETE ✅
- WP5.9 mixed 8-BIT + FM4 validation: COMPLETE ✅ closeout run `40210173573`
- WP5.10 worst-case mixed-engine benchmark + Phase 5 software exit: COMPLETE ✅ closeout run `40220923958`

## Accepted production resources

| Check | D056 | WP5.9 | WP5.10 final |
|---|---:|---:|---:|
| image | 79,804 B | 79,836 B | 79,836 B |
| normal RAM | 27,792 B | 27,792 B | 27,792 B |
| pool | 113,000 B | 113,000 B | 113,000 B |

Target-validated production checkpoint remains `81d3464059270130747e26fcc950b7a2e1bb03e1`.

WP5.10 verification commits do not change production source.

## WP5.10 accepted state

Issue `#53`, branch `phase5/worst-case-benchmark`.

| Check | Status |
|---|---|
| precontract audit | ✅ `docs/WP5_10_PRECONTRACT_AUDIT.md` |
| benchmark contract | ✅ FROZEN `docs/WP5_10_WORST_CASE_MIXED_BENCHMARK_CONTRACT.md` |
| production source freeze | ✅ baseline `81d3464`, final delta 0 |
| contract-freeze gate | ✅ |
| mixed benchmark implementation | ✅ |
| dual-engine realtime audit | ✅ |
| inherited WP5.9/D056/D055/all-8BIT regressions | ✅ |
| S1 exact 8 FM4 + 8 8-BIT occupancy | ✅ |
| S1 warmup | ✅ 256 frames |
| S1 measured window | ✅ 4096 frames |
| S1 repetitions | ✅ 3 identical |
| S2 queue depth at callback entry | ✅ 32 |
| S2 queue after consume | ✅ 0 |
| S2 engine occupancy | ✅ exact 8+8 |
| S2 audition voices | ✅ 4 |
| S2 audition sine calls | ✅ 1024 in one 256-frame callback |
| S2 repetitions | ✅ 3 identical |
| uninstrumented pinned target footprint closeout | ✅ exact 79,836 / 27,792 / 113,000 B |
| optimization sub-contract | ✅ not required |
| Phase 5 software exit | ✅ PASS |
| host timing as physical gate | ⛔ forbidden |
| target runtime timing | ⬜ deferred without reproducible execution-return path |
| physical audio stability | ⛔ unverified |
| physical CPU headroom | ⛔ unverified |
| physical free-RAM margin | ⛔ unverified |

## Accepted runs

```text
benchmark checkpoint      6f47d832b2fc4d83cd2ad03943e919a183123ee2
benchmark run             40218677393 SUCCESS
benchmark job             121245277129 SUCCESS

closeout checkpoint       bcf339b5aa49cf66e8f19917bbb2c48ebc974c56
closeout run              40220923958 SUCCESS
closeout host             121252114781 SUCCESS
closeout target           121252114877 SUCCESS
phase5 software exit      121252369061 SUCCESS
```

Pinned AC79 SDK: `d179b4484759423312073f5fbb232501aa491047`.

Target hashes:

```text
fm1tracker.bin
17d8a9ad757a9d34908a17c03a0491c9e9be8aae0d1c1573edbf73766cb9d2e5

fm1tracker-0.1.fwsc
741036f46510f166a07bad2ba9a8dab5ed3e99917ce72fdbc697c736aec2a58d
```

## Evidence boundary

The closeout keeps distinct:

1. deterministic host workload shape/state/audio evidence;
2. host timing as informational only;
3. uninstrumented pinned-target build/resource evidence;
4. target/emulator TIMER4 telemetry only if a reproducible runtime-return path exists;
5. physical FM-1 realtime/audio/free-RAM evidence as unavailable.

Accepted exit state:

```text
phase5_software_exit=pass
physical_audio_stability=unverified
physical_cpu_headroom=unverified
physical_free_ram_margin=unverified
target_runtime_timing_gate=deferred_without_execution_path
hardware_margin_claim=none
```

Full closeout: `docs/WP5_10_CLOSEOUT_EVIDENCE.md`.
