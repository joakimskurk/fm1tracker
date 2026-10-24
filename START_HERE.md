# FM1Tracker - Start Here

If you are continuing this project after a pause, in a new chat, or as a new contributor, **do not reconstruct the plan from memory**.

Read in this order:

1. [`docs/PROJECT_STATE.md`](docs/PROJECT_STATE.md) - current phase, active work package, exact next actions and blockers.
2. [`docs/D047_MIXED_ENGINE_ARCHITECTURE.md`](docs/D047_MIXED_ENGINE_ARCHITECTURE.md) - accepted architecture direction: one engine-agnostic tracker with per-instrument engine families.
3. [`docs/CURRENT_DECISIONS.md`](docs/CURRENT_DECISIONS.md) - compact current supersession index. `DECISIONS.md` remains the historical decision log through D046.
4. [`docs/WORK_PACKAGES.md`](docs/WORK_PACKAGES.md) - roadmap broken into bounded implementation units.
5. [`docs/DECISIONS.md`](docs/DECISIONS.md) - historical accepted/provisional architectural and UX decisions.
6. [`docs/EXECUTION_PROTOCOL.md`](docs/EXECUTION_PROTOCOL.md) - how work packages are started, verified and closed.
7. [`docs/CURRENT_TEST_STATE.md`](docs/CURRENT_TEST_STATE.md) - final D046 state plus active D047/WP5.0 verification obligations.
8. [`docs/TEST_MATRIX.md`](docs/TEST_MATRIX.md) - detailed historical regression inventory.
9. [`docs/ROADMAP.md`](docs/ROADMAP.md) - long-term phase direction.
10. [`docs/WP4_10_VERIFICATION.md`](docs/WP4_10_VERIFICATION.md) - most recent completed-package evidence and canonical Phase 4 closeout.
11. [`THIRD_PARTY.md`](THIRD_PARTY.md) - external code/assets, licenses and provenance.

## Current handoff

**Phase 4 is complete at host/device-build level.** WP4.10 / D046 closed the complete first 8-BIT engine family without any production-source change after the D045 production freeze.

Canonical Phase 4 exit:

- production baseline: `c93c39ad344760b4da0dfbc76829ca5fef886174`
- verification head: `2379d3e7e52d1cefe9f845f8287ad0ccda8c019a`
- run: `40124536375`
- job: `120963022289`
- evidence: [`docs/WP4_10_VERIFICATION.md`](docs/WP4_10_VERIFICATION.md)
- representative WAV SHA-256: `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9`
- 4,096-row / 64-wrap repeat-identical long run
- zero active cross-track steals
- pinned target resources/artifacts unchanged
- `hardware_margin_claim=none`

## Current active work package

**WP5.0 - mixed-engine routing contract + migration** is now the only active package.

D047 is the normative architecture direction. FM1Tracker is one engine-agnostic tracker/sequencer where instrument identity selects engine family + engine. The historical project-wide 8-BIT/16-BIT restriction is superseded for future work.

WP5.0 must freeze the routing/migration contract before production code changes. In particular it must define and test:

- per-instrument family + engine as the authoritative routing identity;
- removal of `FmtSongMeta.mode` as a project-wide playback eligibility gate while preserving compatibility;
- simultaneous different engine families across tracks;
- deterministic same-track switching across engine families;
- one owned primary sequenced voice per logical track across all families;
- no active cross-track primary stealing;
- bounded static dispatch and ownership representation;
- the common publication/parameter boundary;
- exact all-8-BIT D046 regression equivalence through the new router.

**Do not jump to FM4 operator/oscillator implementation before WP5.0 is frozen and its focused pre-migration tests exist.**

Platform provenance remains available in:

- [`docs/UPSTREAM_DINGHY.md`](docs/UPSTREAM_DINGHY.md)
- [`docs/DINGHY_IMPORT_MANIFEST.md`](docs/DINGHY_IMPORT_MANIFEST.md)

Architecture/product references:

- [`docs/D047_MIXED_ENGINE_ARCHITECTURE.md`](docs/D047_MIXED_ENGINE_ARCHITECTURE.md) - current sound-engine architecture direction and supersession rules
- [`docs/ARCHITECTURE_V0.1.md`](docs/ARCHITECTURE_V0.1.md) - original Phase 0 architecture baseline; project-wide sound-mode clauses are superseded by D047
- [`docs/DATA_MODEL_V0.1.md`](docs/DATA_MODEL_V0.1.md) - compact tracker/instrument data model retained through D047
- [`docs/CONTROL_MAP_V0.1.md`](docs/CONTROL_MAP_V0.1.md)
- [`docs/ENGINE_SCOPE_V0.1.md`](docs/ENGINE_SCOPE_V0.1.md)
- [`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md`](docs/LIVE_PERFORMANCE_CAPTURE_VISION.md)

## Rule of precedence

When chat history and repository state disagree, treat repository state as authoritative unless the project owner explicitly changes the decision. Write the change back into the repository before building further work on it.

For the sound-engine architecture specifically, D047 supersedes D004 and any older statement that mixed 8-BIT/16-BIT engines are a Phase 9-only feature. Historical contracts remain authoritative for the checkpoints they actually verified.

## Current execution rule

Only one work package should normally be active at a time. Finish its contract, implementation, exit criteria and state updates before advancing.

The next action is **WP5.0 contract/test planning**, not FM4 production implementation.

No physical FM-1 was available for Phase 4 closeout. Physical install/boot/audio/control/screen validation and measured AC791N execution margin remain explicit later gates.
