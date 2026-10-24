# WP4.10 Verification - D046 Phase 4 Exit

Date: 2026-10-24  
Package: WP4.10 / issue #41  
Decision: D046  
Branch: `phase4/8bit-engine`  
Normative contract: `docs/WP4_10_PHASE4_EXIT_CONTRACT.md`

## Result

**PASS - Phase 4 is complete at host/device-build level.**

Canonical final verification:

- head: `2379d3e7e52d1cefe9f845f8287ad0ccda8c019a`
- workflow: `WP4.10 D046 Phase 4 exit`
- run: `40124536375`
- job: `120963022289`
- artifact: `wp4-10-d046-phase4-exit`
- artifact ID: `12571958995`
- artifact digest: `sha256:0cefb815415f12c88e45ad3477621e0805e67ff3b928ad142e2fd3c8835c8915`

The final aggregate exit marker is:

```text
WP4_10_EXIT_OK=1 representative_wav_sha256=33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9 inherited_regressions=pass realtime=pass d033_alloc_counts=pass repeat_identity=pass target=pinned identity=FM-1_987 phase4=exit_ready
```

Closeout interpretation:

```text
WP4_10_PHASE4_EXIT_OK=1 hardware_margin_claim=none
```

## Production freeze

D046 was verification-only. No production defect was found.

The final production tree under:

```text
firmware/src
firmware/hal
firmware/loader
firmware/app.ld
build.sh
tools
```

remains byte-identical to the frozen D045 production baseline:

`c93c39ad344760b4da0dfbc76829ca5fef886174`

The final D046 host observation layer lives only under `tests/` and generated `build/tests` copies. It does not change production behavior. The benchmark verifies that its `dinghy.c` test copy is byte-identical to production, then wraps verbatim copies of the D033 action evaluator and D043 allocator acquire function only to count returned outcomes. The counters do not feed back into tracker, event, allocator or engine state.

## Representative linked workload

The canonical D046 fixture covers:

- all 8 logical tracks;
- all 64 rows;
- all four 8-BIT engines: PULSE, NOISE, TRIANGLE and WAVETABLE-8;
- exactly 512 valid lock owners;
- the complete 2,048-record D028 pool at four locks per owner;
- D031, D032, D033 and D044 pressure together;
- D043 release-tail contention;
- D034 queue pressure and recovery while tracker playback continues.

Fixture shape:

```text
WP4_10_FIXTURE_SHAPE_OK=1 events=2778 steps=512 locks=2048 owners=512
WP4_10_FULL_POOL_LINKED_OK=1 tracks=8 rows=64 owners=512 locks=2048 engines=4
```

## Canonical 64-row render

Final canonical state/audio summary:

```text
D046_HOST_BENCH rows=64 ticks=2560 frames=353024 sample_fnv=abbd4fcd state_fnv=b5f80714 lock_used=2048 lock_free=0 queue_depth=0 overflow=16 seen=16 step=0 phase=26880
```

Exact D033 action and D043 allocation outcomes:

```text
WP4_10_D046_COUNTS actions_none=2958 actions_delay=23 actions_cut=37 actions_retrig=62 alloc_none=0 alloc_reuse=224 alloc_fresh_idle=36 alloc_steal_tail=68 active_cross_track_steals=0 alloc_invariant_fail=0
```

Representative WAV:

- frames: `353024`
- SHA-256: `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9`

A second complete 64-row render was run through the same linked path and produced byte-identical WAV, identical final state digest and identical D033/D043 counts:

```text
WP4_10_REPRESENTATIVE_REPEAT_OK=1 wav=byte_identical state_digest=identical action_alloc_counts=identical
```

## 4,096-row / 64-wrap linked run

Final long-run summary:

```text
D046_HOST_BENCH rows=4096 ticks=163840 frames=22579200 sample_fnv=a86caee1 state_fnv=53ae5fad lock_used=2048 lock_free=0 queue_depth=0 overflow=40 seen=40 step=0 phase=0
```

Exact long-run D033 action and D043 allocation outcomes:

```text
WP4_10_D046_COUNTS actions_none=188808 actions_delay=1472 actions_cut=2368 actions_retrig=3968 alloc_none=0 alloc_reuse=14336 alloc_fresh_idle=1800 alloc_steal_tail=4352 active_cross_track_steals=0 alloc_invariant_fail=0
```

The entire 4,096-row run was repeated. Sample digest, state digest and all action/allocation counts were identical:

```text
WP4_10_LONG_RUN_REPEAT_OK=1 sample_digest=identical state_digest=identical action_alloc_counts=identical
WP4_10_D033_ALLOC_COUNTS_OK=1 representative_repeat=identical long_repeat=identical active_cross_track_steals=0 allocation_invariants=pass
WP4_10_LONG_RUN_OK=1 rows=4096 wraps=64 ticks=163840 representative_wav=33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9
```

This explicitly proves that the D043 contention case never steals a still-owned active cross-track primary voice.

## Concurrent D034 pressure

The representative workload exercises queue-full, reject-newest overflow, audio-side fail-to-silence/discard and subsequent recovery while the eight-track tracker remains active.

```text
WP4_10_QUEUE_PRESSURE_OK=1 short_overflow=16 long_overflow=40 recovery=1 tracker_continues=1
```

## Realtime structural audit

Final D046 structural evidence includes:

```text
WP4_10_PRODUCTION_FREEZE_OK=1 baseline=c93c39ad344760b4da0dfbc76829ca5fef886174
WP4_10_NO_ALLOCATOR_OK=1
WP4_10_AUDIO_OWNERSHIP_OK=1 d034_consumers=1 engine_render_calls=1 editor_store_traversal=0 screen_calls=0
WP4_10_TRACKER_FRAME_BOUND_OK=1 row_tracks=8 tick_due_checks_max=8 row_boundary_wins=1 second_tracker_clock=0
WP4_10_ROW_HELPERS_BOUND_OK=1 variable_loops=0 queue_traversal=0 lock_store_traversal=0
WP4_10_QUEUE_BOUND_OK=1 capacity=32 callback_cap=32 overflow_discard_o1=1 spin_retry=0 irq_masking=0
WP4_10_ENGINE_BOUND_OK=1 render_slots=8 mutable_store_reads=0 lock_list_reads=0
WP4_10_SCREEN_OWNERSHIP_OK=1 main_presentation_only=1 bounded_lock_reads=1
WP4_10_INTEGER_REALTIME_OK=1 callback_64bit_types=0 leaf_64bit_types=0 float_double=0
WP4_10_SINGLE_CLOCK_OK=1 row_phase=661500 tick_phase=110250 ticks_per_row=6
WP4_10_REALTIME_AUDIT_OK=1
```

These are structural boundedness proofs. They are not host-speed claims about AC791N execution margin.

## Inherited regressions

The aggregate D046 gate reuses the complete authoritative WP4.9 closeout rather than duplicating or weakening it. All still-applicable Phase 2, Phase 3 and Phase 4 regressions remain green, including the canonical linked 8-BIT audio fixtures and D045 realtime/static ownership audit.

Historical proof-sine assertions superseded by D045 remain explicitly excluded as intended by the D046 contract.

The normal WP4.9 linked default-app WAV remains separately frozen and unchanged:

`17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`

## Pinned target identity and resources

Pinned inputs:

- JieLi toolchain: `jieli-linux-toolchains-20260730.1`
- AC79 SDK commit: `d179b4484759423312073f5fbb232501aa491047`
- package identity: `FM-1_987`

Final linked target footprint:

| Resource | Final | D037 ceiling | Remaining |
|---|---:|---:|---:|
| Image | 72,504 B | 132,496 B | 59,992 B |
| Normal RAM | 19,604 B | 21,488 B | 1,884 B |
| Pool RAM | 113,000 B | 129,384 B | 16,384 B |

Artifact hashes remain exactly the D045 baseline:

- `fm1tracker.bin`: `214fba8ec0af4be3120bc91dafbaa24e7d4d8a5a6de7395de2473f12374c4215`
- `fm1tracker-0.1.fwsc`: `20f0bbe556edcf6a00118f9983335913dda2f29fc64d3c87997bfd6756bcf119`

No target resource or artifact drift occurred during WP4.10.

## Closeout harness history

The first aggregate closeout attempt, run `40120541899`, was red. Investigation proved a CI harness dependency issue rather than a firmware failure: the aggregate workflow did not install Pillow even though the inherited WP4.9 target closeout requires it.

After adding the same Pillow setup used by the authoritative WP4.9 workflow, run `40123735687` reached the end of the D046 script successfully but the workflow still reported red because a nested target rebuild recreated `build/` and removed the top-level `tee` log before post-run grep checks. The log was moved to `$RUNNER_TEMP` and copied back after the build.

Run `40124184543` then proved the aggregate wiring green. Before declaring Phase 4 complete, the D046 contract was audited line by line. That audit found that the benchmark had final state/sample hashes but did not yet expose the contract's required exact D033 action counts, D043 allocation outcome counts, zero-active-cross-track-steal count or actual repeat-run identity. Test-only observation instrumentation and repeat runs were added, with production still frozen.

The canonical closeout is therefore the later contract-complete run `40124536375`, not the earlier green wiring run.

## Physical hardware limitation

No physical FM-1 is available for this closeout.

Therefore WP4.10 does **not** claim:

- physical install or boot success;
- physical audio quality;
- control or screen ergonomics;
- measured AC791N callback/cycle execution margin;
- thermal or long-duration physical-hardware behavior.

Host wall-clock speed is non-normative and is not converted into a hardware CPU percentage.

`hardware_margin_claim=none`

These physical checks remain explicit later hardware gates.

## Phase 4 exit

Every D046 host/device-build exit criterion is satisfied:

1. production remains frozen to D045;
2. complete 2,048-lock representative workload passes;
3. all 8 tracks and all 4 engines pass representative and long-run execution;
4. D031/D032/D033/D044 timing and precedence remain deterministic;
5. D043 reports zero active cross-track steals;
6. D034 pressure/overflow/recovery coexists safely with tracker playback;
7. realtime work remains structurally bounded;
8. inherited non-obsolete regressions pass;
9. pinned target resources and artifact identity remain exact;
10. repeat runs are identical;
11. physical-hardware limitations are stated explicitly.

**WP4.10 / D046 is COMPLETE. Phase 4 is COMPLETE at host/device-build level.**

The next work package is **WP5.0 - mixed-engine routing contract + migration**, under the already accepted D047 architecture decision. No FM4 production implementation should begin until WP5.0 has frozen its routing/migration contract.
