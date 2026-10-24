# WP4.1 Verification — D037 8-BIT Engine Interface Contract

Date: 2026-10-24  
Branch: `phase4/8bit-engine`  
Package: WP4.1 / issue #31  
Result: **PASS — host/device-build architecture closeout complete**

## Scope

WP4.1 is an architecture-only package. It freezes the Phase 4 8-BIT engine/voice/realtime interface and complete Phase 4 resource ceilings before production oscillator DSP begins.

Normative contract:

- `docs/WP4_1_ENGINE_CONTRACT.md`
- D037 in `docs/DECISIONS.md`
- contract commit `a740da66a0587b4e257629d7aba12781fad73f73`
- D037 decision/status synchronization commit `d9562019037abcce05552c17ddfee53e54fc882a`

Phase 4 clean branch start / final Phase 3 handoff:

`d14955f23a11044e8a348884feca24f8978fd700`

Final WP4.1 closeout workflow source checkpoint:

`01138c5f8a420efcc8e438ff3c2eb4090c086586`

## Final GitHub Actions evidence

Authoritative closeout:

- workflow: `WP4.1 D037 closeout verification`
- run: `40087703654`
- job: `120853898352`
- result: **SUCCESS**
- evidence artifact: `wp4-1-closeout-evidence`
- artifact ID: `12557981840`
- artifact size: `284,660` B
- artifact ZIP SHA-256: `212189d61f4f7002bcdee1b2304b5c21e588cc1e7c427a1f8e7bbd50cb017def`
- workflow head: `01138c5f8a420efcc8e438ff3c2eb4090c086586`

The immediately preceding run `40087571082` is not an accepted closeout run: its underlying D037/inherited test script reached `WP4_1_CLOSEOUT_OK=1`, but the workflow wrapper failed because `tee` attempted to open `build/wp4_1_closeout.log` before `build/` existed. Commit `01138c5f...` fixes only that wrapper by creating the directory first. The complete gate was then rerun from the beginning and passed.

## D037 repository freeze proof

The closeout gate verified:

`WP4_1_PRODUCTION_IDENTICAL_OK=1 baseline=d14955f23a11044e8a348884feca24f8978fd700`

and:

`WP4_1_D037_REPOSITORY_FREEZE_OK=1 voices=8 pitch=q7 params_cache_max=2048`

The production comparison covers:

- `firmware/src`
- `firmware/hal`
- `firmware/loader`
- `firmware/app.ld`
- `build.sh`
- `tools`

All are byte-identical to the clean Phase 4 branch start. WP4.1 therefore adds **zero production firmware/DSP source delta**.

The gate also verified that no early Phase 4 production translation unit exists for the reserved engine family:

`WP4_1_NO_ENGINE_SOURCE_OK=1`

No PULSE, NOISE, TRIANGLE or WAVETABLE-8 production implementation was pulled into WP4.1.

## Frozen D037 interface/budget facts

The gate checked the durable contract for:

- `8-BIT` mode/engine family with PULSE, NOISE, TRIANGLE and WAVETABLE-8 reserved IDs;
- fixed maximum of eight engine voice slots;
- reuse of D031/D032 unsigned Q7 semitone pitch `0..16383`;
- maximum resolved engine-parameter row cache of 2,048 B ordinary RAM;
- explicit complete Phase 4 ceilings:
  - app image: Phase 3 66,960 B + 65,536 B = **132,496 B maximum**;
  - normal RAM: Phase 3 13,296 B + 8,192 B = **21,488 B maximum**;
  - pool RAM: Phase 3 113,000 B + 16,384 B = **129,384 B maximum**;
- audio-owned mutable voice/engine state and bounded main→audio publication;
- at most eight voice renders per frame and at most eight allocator-slot inspections;
- D032/D033 timing/effect ownership retained rather than duplicated in the engine;
- explicit integer/Q15/32-bit-saturated mix boundary;
- no realtime allocator, D028 traversal, blocking, floating point, runtime 64-bit division or unbounded search.

## Complete inherited Phase 3 gate

WP4.1 deliberately reran `tests/run_wp3_10_closeout_ci.sh`, not a reduced smoke test.

The inherited exit proof passed again, including:

- WP3.9 full regression/device-build closeout;
- D028 pool and D031/D032 cache stress;
- D034 saturation/wrap/overflow stress and recovery;
- D035 screen-lock inspection/backlog convergence;
- combined full-app playback/audition/editor/UI/queue stress;
- complete D032/D033 timing matrix;
- 4,096-row / 64-wrap long timing proof;
- static realtime boundedness audit;
- pinned JieLi toolchain and AC79 SDK/input verification;
- full target build and package identity check.

Inherited signatures remained green, including:

- `WP3_10_FOCUSED_WORST_CASE_OK=1`
- `WP3_10_COMBINED_STRESS_OK=1 deterministic=1 two_overflow_epochs=1 recovery=1 midi_neutral=1 screen_queue_neutral=1`
- `WP3_10_TIMING_MATRIX_OK=1 d032_commands=5 values_each=256 d033_commands=3 values_each=256 no_tick6=1 hash=364d0c2c`
- `WP3_10_REALTIME_AUDIT_OK=1`
- `WP3_10_PHASE3_EXIT_OK=1 wp3_1_through_wp3_10=complete host_device_build=1 physical_hardware_pending=1`
- `WP3_10_CLOSEOUT_OK=1`

## Target build / resource identity

Because WP4.1 contains no production change, the real target build remained exactly the frozen final Phase 3 firmware:

- app image: **66,960 B / 581,564 B**
- normal RAM: **13,296 B / 98,304 B**
- pool RAM: **113,000 B / 344,064 B**
- WP4.1 production resource delta: **0 B / 0 B / 0 B**
- package identity: `FM-1_987`

Final artifacts remain:

- `fm1tracker.bin` SHA-256 `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27`
- `fm1tracker-0.1.fwsc` SHA-256 `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`

The gate emitted:

- `WP4_1_FINAL_RESOURCES image=66960 normal_ram=13296 pool_ram=113000 production_delta=0`
- `WP4_1_D037_EXIT_OK=1 architecture_frozen=1 production_engine_source=0 inherited_phase3=1 physical_hardware_pending=1`
- `WP4_1_CLOSEOUT_OK=1`

## Hardware limitation

WP4.1 makes no new sound and therefore does not require a physical FM-1 to validate oscillator quality. The already-open physical install/boot/audio/panel/LCD/execution-margin gates remain open and must not be represented as completed by this host/device-build proof.

## Exit

WP4.1 / D037 is complete at the host/device-build architecture level.

The next package is WP4.2 / PULSE. It may now introduce the first isolated Phase 4 oscillator primitive, but must obey D037 and must not opportunistically pull NOISE, TRIANGLE, WAVETABLE-8, envelope, final allocator, parameter-page or full proof-sine migration work forward.
