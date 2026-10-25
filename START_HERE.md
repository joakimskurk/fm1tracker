# FM1Tracker - Start Here

Repository state is authoritative. Do not reconstruct the plan from old chat context.

Read in this order:

1. `docs/PROJECT_CONTEXT.md`
2. `docs/PROJECT_STATE.md`
3. `docs/WP5_8_FM_PARAMETER_PAGE_CONTRACT.md`
4. `docs/WP5_8_PRECONTRACT_AUDIT.md`
5. `docs/WP5_7_VERIFICATION.md`
6. `docs/WP5_7_FM_VOICE_INTEGRATION_CONTRACT.md`
7. `docs/CURRENT_DECISIONS.md`
8. `docs/WORK_PACKAGES.md`
9. `docs/EXECUTION_PROTOCOL.md`
10. `docs/CURRENT_TEST_STATE.md`
11. `docs/TEST_MATRIX.md`
12. `docs/ROADMAP.md`
13. `THIRD_PARTY.md`

## Product context first

FM1Tracker is one engine-agnostic hardware tracker/sequencer, not an FM-only project. The intended product includes 8-BIT, FM, future virtual-analog/subtractive synthesis and drums/percussion in the same tracker environment. Exact DSP/resource contracts for not-yet-implemented engine families are future work and must be audited before implementation.

`docs/PROJECT_CONTEXT.md` is the persistent product-memory layer. Roadmap order must never replace product context.

## Current handoff

WP5.0/D048 through WP5.7/D055 are complete.

WP5.8 / D056 is the only active package. The D056 contract and production implementation are complete through parameter model, generalized row cache, store-backed FM resolver, row-boundary FM patch snapshots, page-aware held-step editor and mutable per-instrument FM base API.

Active issue: #51.  
Active branch: `phase5/fm4-parameters`.

Parent WP5.7 closeout checkpoint:

- head `80587f12786122dcec819260ddad5dd5db4b1005`
- run `40177151084` SUCCESS
- target 77,048 B image / 20,464 B normal RAM / 113,000 B pool
- exact all-8BIT SHA `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`

## WP5.8 closeout state

Closeout workflow run `40184529349` reached:

- `closeout-target` job `121142394191`: SUCCESS;
- `closeout-host` job `121142394408`: FAILURE.

Therefore WP5.8 is **not closed yet**. The target build is green, but the host closeout failure must be diagnosed and fixed without weakening fail-closed behavior. Do not start WP5.9 until both closeout jobs pass on an accepted checkpoint and the closeout documentation is committed.

## Exact next action

1. inspect the failing WP5.8 host closeout log and identify whether it is a real production regression or stale historical harness/source-boundary assumption;
2. fix the cause while preserving fail-closed CI behavior;
3. rerun the full WP5.8 closeout and require both host and pinned-target jobs green on the same accepted checkpoint;
4. record exact target metrics, firmware hash and closeout evidence;
5. synchronize `PROJECT_STATE.md`, `CURRENT_TEST_STATE.md`, `TEST_MATRIX.md`, `WORK_PACKAGES.md` and relevant context docs;
6. close issue #51 only after the accepted closeout/docs checkpoint;
7. only then open WP5.9 mixed 8-BIT + FM4 validation.

No physical FM-1 is available. `hardware_margin_claim=none`.
