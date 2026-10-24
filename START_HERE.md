# FM1Tracker - Start Here

If you are continuing this project after a pause, in a new chat, or as a new contributor, **do not reconstruct the plan from memory**.

Read in this order:

1. [`docs/PROJECT_STATE.md`](docs/PROJECT_STATE.md) - current phase, active work package, exact next actions and blockers.
2. [`docs/D047_MIXED_ENGINE_ARCHITECTURE.md`](docs/D047_MIXED_ENGINE_ARCHITECTURE.md) - accepted post-Phase-4 architecture direction: one engine-agnostic tracker with per-instrument engine families; no project-wide 8-BIT/16-BIT lock.
3. [`docs/CURRENT_DECISIONS.md`](docs/CURRENT_DECISIONS.md) - compact current supersession index. `DECISIONS.md` remains the historical decision log through D046.
4. [`docs/WORK_PACKAGES.md`](docs/WORK_PACKAGES.md) - roadmap broken into bounded implementation units.
5. [`docs/DECISIONS.md`](docs/DECISIONS.md) - historical accepted/provisional architectural and UX decisions. D047 supersedes D004's project-wide-mode restriction for future work without rewriting historical evidence.
6. [`docs/EXECUTION_PROTOCOL.md`](docs/EXECUTION_PROTOCOL.md) - how work packages are started, verified and closed.
7. [`docs/CURRENT_TEST_STATE.md`](docs/CURRENT_TEST_STATE.md) - current in-flight D046 status plus D047/WP5.0 verification obligations.
8. [`docs/TEST_MATRIX.md`](docs/TEST_MATRIX.md) - detailed historical regression inventory. Final D046 results are folded into it when WP4.10 closes.
9. [`docs/ROADMAP.md`](docs/ROADMAP.md) - long-term phase direction.
10. [`THIRD_PARTY.md`](THIRD_PARTY.md) - external code/assets, licenses and provenance.

## Current handoff

WP4.9 / D045 is verified in [`docs/WP4_9_VERIFICATION.md`](docs/WP4_9_VERIFICATION.md).

WP4.10 / #41 remains the **only active work package**. D046 is frozen in [`docs/WP4_10_PHASE4_EXIT_CONTRACT.md`](docs/WP4_10_PHASE4_EXIT_CONTRACT.md). The representative linked benchmark and structural realtime audit have now been implemented as verification-only work while the D045 production tree remains frozen. The first aggregate Phase 4 exit workflow run `40120541899` is red, so Phase 4 is **not closed yet**. The exact next execution task is to isolate/fix the aggregate exit-gate failure, rerun the complete D046 closeout green, write `docs/WP4_10_VERIFICATION.md`, and only then mark Phase 4 complete.

D047 changes **future architecture only**. Do not modify Phase 4 production code to implement mixed engines while WP4.10 is active. After a green Phase 4 closeout, Phase 5 begins with **WP5.0 mixed-engine routing contract + migration** before FM4 production integration.

Platform provenance remains available in:

- [`docs/UPSTREAM_DINGHY.md`](docs/UPSTREAM_DINGHY.md)
- [`docs/DINGHY_IMPORT_MANIFEST.md`](docs/DINGHY_IMPORT_MANIFEST.md)

Architecture/product references:

- [`docs/D047_MIXED_ENGINE_ARCHITECTURE.md`](docs/D047_MIXED_ENGINE_ARCHITECTURE.md) - current sound-engine architecture direction and supersession rules
- [`docs/ARCHITECTURE_V0.1.md`](docs/ARCHITECTURE_V0.1.md) - original Phase 0 architecture baseline; project-wide sound-mode clauses are superseded by D047
- [`docs/DATA_MODEL_V0.1.md`](docs/DATA_MODEL_V0.1.md) - existing compact tracker/instrument data model; D047 preserves current layouts while changing future mode semantics
- [`docs/CONTROL_MAP_V0.1.md`](docs/CONTROL_MAP_V0.1.md)
- [`docs/ENGINE_SCOPE_V0.1.md`](docs/ENGINE_SCOPE_V0.1.md) - engine-family scope after D047
- [`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md`](docs/LIVE_PERFORMANCE_CAPTURE_VISION.md)

## Rule of precedence

When chat history and repository state disagree, treat repository state as authoritative unless the project owner explicitly changes the decision. Write the change back into the repository before building further work on it.

For the sound-engine architecture specifically, D047 supersedes D004 and any older statement that mixed 8-BIT/16-BIT engines are a Phase 9-only feature. Historical contracts remain authoritative for the checkpoints they actually verified.

For current verification status while WP4.10 is in flight, use `CURRENT_TEST_STATE.md` together with `PROJECT_STATE.md`; `TEST_MATRIX.md` remains the exhaustive historical regression inventory until final Phase 4 closeout is written.

## Current execution rule

Only one work package should normally be active at a time. Finish its exit criteria, update project/test state, then advance. D047 is already accepted direction, but its production migration begins only after WP4.10 closes Phase 4.
