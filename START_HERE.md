# FM1Tracker - Start Here

If you are continuing after a pause, in a new chat, or as a new contributor, **do not reconstruct the plan from chat memory**. Repository state is authoritative.

Read in this order:

1. `docs/PROJECT_STATE.md` - current phase, active package and exact next action.
2. `docs/WP5_1_FM_OPERATOR_CONTRACT.md` - current D049 contract for the active package.
3. `docs/WP5_0_VERIFICATION.md` - most recent completed-package evidence.
4. `docs/D047_MIXED_ENGINE_ARCHITECTURE.md` - accepted engine-agnostic/per-instrument architecture.
5. `docs/WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md` - D048 routing semantics.
6. `docs/CURRENT_DECISIONS.md` - current supersession/decision index.
7. `docs/WORK_PACKAGES.md` - bounded implementation order and scope.
8. `docs/EXECUTION_PROTOCOL.md` - start/implement/verify/close procedure.
9. `docs/CURRENT_TEST_STATE.md` - current verification overlay.
10. `docs/TEST_MATRIX.md` - current regression index.
11. `docs/ROADMAP.md` - product and phase direction.
12. `THIRD_PARTY.md` - external code/assets, licenses and provenance.

## Current handoff

**WP5.0 / D048 is COMPLETE at host/device-build level.**

Canonical WP5.0 exit:

- production verification head `813792fb25632b4d7acda787422e51f713fad21e`
- aggregate run `40129822006`
- aggregate job `120978955451`
- closeout/handoff `6c0add8828474e9308710e02957231e7225eb121`
- evidence `docs/WP5_0_VERIFICATION.md`
- target 72,792 B image / 19,676 B normal RAM / 113,000 B pool
- representative D046 WAV retained `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9`
- normal D046 WAV retained `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`
- `hardware_margin_claim=none`

## Current active package

**WP5.1 - fixed-point FM operator primitive**, issue #44, branch `phase5/fm4-operator`.

**D049 is already frozen before production FM operator source.**

Normative contract: `docs/WP5_1_FM_OPERATOR_CONTRACT.md`.

Frozen essentials:

- operator state exactly 4 B, one `uint32_t phase`;
- one cycle = 2^32 phase units;
- signed int32 phase-modulation offset in the same phase domain;
- Q15 level 0..32767;
- generated 257-entry positive quarter-wave sine table;
- 22-bit linear interpolation;
- table SHA-256 `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68`;
- 4,096-render reference SHA-256 `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66`;
- production operator stays isolated from `dinghy.c` for WP5.1;
- full app must remain exact WP5.0 identity.

### Exact next action

Run `tests/run_wp5_1_contract_freeze.sh` through the D049 contract-freeze workflow and require a green result while `fmt_fm_operator.*` is still absent.

Only after that gate is green may implementation begin.

Then implement, in this order:

1. deterministic checked-in sine-table generator/data;
2. `fmt_fm_operator.h/.c` matching D049;
3. focused C host vectors;
4. source/realtime audit;
5. independent pinned JieLi target compile/object-size measurement;
6. exact WP5.0 full-app regression;
7. `WP5_1_VERIFICATION.md` before WP5.2.

Do not pull in algorithms, ratio tables, feedback, FM envelopes, FM voice allocation, FM p-lock pages or mixed production FM playback while WP5.1 is active.

## Product direction

Tracker depth underneath, immediate performance/capture on top.

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted core direction. Live capture, drums, metronome and CHANNEL/GLOBAL performance FX remain mandatory later core work, not optional extensions.

No physical FM-1 is available. Physical install/boot/audio/control/screen behavior and measured AC791N execution margin remain explicit later gates.
