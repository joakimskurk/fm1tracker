# FM1Tracker - Start Here

If you are continuing after a pause, in a new chat, or as a new contributor, **do not reconstruct the plan from chat memory**. Repository state is authoritative.

Read in this order:

1. `docs/PROJECT_STATE.md` - current phase, active work package and exact next action.
2. `docs/WP5_0_VERIFICATION.md` - most recent completed-package evidence.
3. `docs/D047_MIXED_ENGINE_ARCHITECTURE.md` - accepted engine-agnostic/per-instrument architecture.
4. `docs/WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md` - D048 routing semantics now implemented.
5. `docs/CURRENT_DECISIONS.md` - current supersession index.
6. `docs/WORK_PACKAGES.md` - bounded implementation order and scope.
7. `docs/EXECUTION_PROTOCOL.md` - start/implement/verify/close procedure.
8. `docs/CURRENT_TEST_STATE.md` - current verification overlay and WP5.1 obligations.
9. `docs/TEST_MATRIX.md` - current regression index.
10. `docs/ROADMAP.md` - product and phase direction.
11. `THIRD_PARTY.md` - external code/assets, licenses and provenance.

## Current handoff

**WP5.0 / D048 is COMPLETE at host/device-build level.**

Canonical WP5.0 exit:

- production verification head: `813792fb25632b4d7acda787422e51f713fad21e`
- aggregate run: `40129822006`
- aggregate job: `120978955451`
- evidence: `docs/WP5_0_VERIFICATION.md`
- target: 72,792 B image / 19,676 B normal RAM / 113,000 B pool
- delta versus D046: +288 B / +72 B / +0 B
- representative D046 WAV retained: `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9`
- normal D046 WAV retained: `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`
- BIN SHA-256: `a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125`
- FWSC SHA-256: `cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073`
- `hardware_margin_claim=none`

D047/D048 now mean:

- FM1Tracker is one engine-agnostic tracker/sequencer.
- Tracker rows/tracks reference instruments.
- Each instrument owns engine family + engine identity.
- `FmtSongMeta.mode` is no longer an audio eligibility gate.
- global family routing is fixed, static and bounded.
- the complete all-8-BIT D046 configuration remains the regression baseline.

## Current active package

**WP5.1 - fixed-point FM operator primitive**, issue #44, branch `phase5/fm4-operator`.

The next action is **contract/test freeze, not FM implementation yet**.

Required order:

1. inspect existing accepted fixed-point pitch/phase/oscillator patterns;
2. write and freeze D049/WP5.1 contract;
3. freeze exact operator state, phase/modulation/level/output formats and overflow rules;
4. freeze deterministic host vectors;
5. define target/realtime/resource guards;
6. only then add production FM operator source.

Do not pull in algorithms, ratio tables, feedback, FM envelopes, FM voice allocation, FM p-lock pages or mixed production FM playback while WP5.1 is active.

## Product direction

Tracker depth underneath, immediate performance/capture on top.

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted core direction. Live capture, drums, metronome and CHANNEL/GLOBAL performance FX remain mandatory later core work, not optional extensions.

## Historical checkpoints

- Phase 2 complete: `7063be51e53c2657178084b3bef5ace813dafcd7`
- Phase 3 closeout: run `40084706845`, job `120845144453`
- Phase 4 / D046 closeout: run `40124536375`, job `120963022289`
- Phase 5 WP5.0 / D048 closeout: run `40129822006`, job `120978955451`

No physical FM-1 is available. Physical install/boot/audio/control/screen behavior and measured AC791N execution margin remain explicit later gates.
