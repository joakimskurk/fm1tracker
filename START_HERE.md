# FM1Tracker - Start Here

Repository state is authoritative, but roadmap order is not the whole project memory. Read persistent product context before reducing the project to the active work package.

Read in this order:

1. `docs/PROJECT_CONTEXT.md`
2. `docs/PROJECT_STATE.md`
3. `docs/WP5_8_CLOSEOUT_EVIDENCE.md`
4. `docs/CURRENT_DECISIONS.md`
5. `docs/WORK_PACKAGES.md`
6. `docs/EXECUTION_PROTOCOL.md`
7. `docs/CURRENT_TEST_STATE.md`
8. `docs/TEST_MATRIX.md`
9. `docs/ROADMAP.md`
10. active work-package contract/audit documents
11. `THIRD_PARTY.md`

## Product context first

FM1Tracker is one engine-agnostic hardware tracker/sequencer, not an FM-only project. The intended product includes 8-BIT, FM, future virtual-analog/subtractive synthesis and drums/percussion in the same tracker environment. Exact DSP/resource contracts for not-yet-implemented engine families are future work and must be audited before implementation.

`docs/PROJECT_CONTEXT.md` is the persistent product-memory layer. Substantive user decisions and product clarifications must be synchronized back into Git according to `docs/EXECUTION_PROTOCOL.md`.

## Current handoff

Phase 5 mixed-engine/FM foundation is complete through **WP5.8 / D056**.

Accepted WP5.8 closeout:

```text
code/workflow checkpoint   849c8b27d8c1afa09f9c093b695985c64e0c6013
run                        40203483720 SUCCESS
host                       121199398344 SUCCESS
target                     121199398538 SUCCESS
image                       79804 B
normal RAM                  27792 B
pool                       113000 B
FWSC sha256                4d2805f428f5114531b4c3d379576d2b207cbc9a6c04fcbdd5263cfaa0cc84a0
all-8BIT WAV sha256        17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076
hardware_margin_claim      none
```

Evidence: `docs/WP5_8_CLOSEOUT_EVIDENCE.md`.

The first closeout run's red host job was a CI `tee` directory-order race, not a production/test failure. The wrapper was fixed without weakening any gate; the accepted rerun passed host and pinned target on the same checkpoint.

## What D056 leaves us with

- one shared semantic parameter architecture;
- 27 stable FM parameter IDs and seven pages;
- max four sparse p-locks per step unchanged;
- generalized 8,704 B row cache with the historical 2,048 B 8BIT view preserved;
- bounded 432 B mutable FM instrument-base bank;
- 232 B current row-boundary FM patch snapshot state, no per-row full-patch cache;
- no sparse store/editor traversal in the per-sample FM renderer;
- D049-D055 synthesis/voice behavior frozen;
- exact WP5.0 all-8BIT regression equivalence.

The WP5.8 normal-RAM result is 27,792 B, leaving only 864 B to the deliberately frozen D056 package ceiling of 28,656 B. This is not a claim about total physical free RAM, but it is a mandatory planning constraint for the next validation/benchmark packages.

## Exact next action

1. close issue #51 with `docs/WP5_8_CLOSEOUT_EVIDENCE.md` as durable evidence;
2. activate **WP5.9 - mixed 8-BIT + FM4 validation** as the only active package;
3. perform a precontract audit before adding or changing production behavior;
4. freeze the simultaneous-family and same-track-switch validation surface, resource expectations and fail-closed regressions;
5. only then implement any WP5.9-specific fixture/harness changes.

WP5.9 must validate the current 8-BIT + FM4 architecture. It must not pull future virtual-analog/subtractive or drum-engine implementation forward. Those engine families remain part of the product direction and must survive every handoff.

No physical FM-1 is available. `hardware_margin_claim=none`.