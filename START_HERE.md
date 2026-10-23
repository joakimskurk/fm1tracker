# FM1Tracker — Start Here

If you are continuing this project after a pause, in a new chat, or as a new contributor, **do not reconstruct the plan from memory**.

Read in this order:

1. [`docs/PROJECT_STATE.md`](docs/PROJECT_STATE.md) — current phase, active work package, exact next actions and blockers.
2. [`docs/WORK_PACKAGES.md`](docs/WORK_PACKAGES.md) — roadmap broken into bounded implementation units.
3. [`docs/DECISIONS.md`](docs/DECISIONS.md) — accepted/provisional architectural and UX decisions.
4. [`docs/EXECUTION_PROTOCOL.md`](docs/EXECUTION_PROTOCOL.md) — how work packages are started, verified and closed.
5. [`docs/TEST_MATRIX.md`](docs/TEST_MATRIX.md) — verification obligations and regression state.
6. [`docs/ROADMAP.md`](docs/ROADMAP.md) — long-term phase direction.
7. [`THIRD_PARTY.md`](THIRD_PARTY.md) — external code/assets, licenses and provenance.

For the current Phase 1 platform import, also read:

- [`docs/UPSTREAM_DINGHY.md`](docs/UPSTREAM_DINGHY.md)
- [`docs/DINGHY_IMPORT_MANIFEST.md`](docs/DINGHY_IMPORT_MANIFEST.md)

Architecture/product references:

- [`docs/ARCHITECTURE_V0.1.md`](docs/ARCHITECTURE_V0.1.md)
- [`docs/DATA_MODEL_V0.1.md`](docs/DATA_MODEL_V0.1.md)
- [`docs/CONTROL_MAP_V0.1.md`](docs/CONTROL_MAP_V0.1.md)
- [`docs/ENGINE_SCOPE_V0.1.md`](docs/ENGINE_SCOPE_V0.1.md)

## Rule of precedence

When chat history and repository state disagree, treat repository state as authoritative unless the project owner explicitly changes the decision. Write the change back into the repository before building further work on it.

## Current execution rule

Only one work package should normally be active at a time. Finish its exit criteria, update project/test state, then advance.
