# FM1Tracker - Start Here

Repository state is authoritative, but roadmap order is not the whole project memory. Read persistent product context before reducing the project to the active work package.

Read in this order:

1. `docs/PROJECT_CONTEXT.md`
2. `docs/PROJECT_STATE.md`
3. `docs/WP5_10_PRECONTRACT_AUDIT.md`
4. `docs/WP5_10_WORST_CASE_MIXED_BENCHMARK_CONTRACT.md`
5. `docs/WP5_9_CLOSEOUT_EVIDENCE.md`
6. `docs/CURRENT_DECISIONS.md`
7. `docs/WORK_PACKAGES.md`
8. `docs/EXECUTION_PROTOCOL.md`
9. `docs/CURRENT_TEST_STATE.md`
10. `docs/TEST_MATRIX.md`
11. `docs/ROADMAP.md`
12. `THIRD_PARTY.md`

## Product context first

FM1Tracker is one engine-agnostic hardware tracker/sequencer. The product direction includes 8-BIT, FM, future virtual-analog/subtractive synthesis and drums/percussion in the same tracker environment. Future engine families require their own audits/contracts.

## Current execution

**WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit is ACTIVE.**

```text
issue                         #53
branch                        phase5/worst-case-benchmark
precontract audit             COMPLETE
benchmark contract            FROZEN
production baseline           81d3464059270130747e26fcc950b7a2e1bb03e1
WP5.9 closeout run            40210173573 SUCCESS
image                         79836 B
normal RAM                    27792 B
pool                         113000 B
hardware_margin_claim         none
```

No production source change is authorized. Benchmark implementation is legal only after the contract-freeze gate is green.

## Frozen WP5.10 workload

S1 must reach and retain exact mixed saturation:

```text
8 logical tracks
8 non-idle FM4 release-tail slots
8 non-idle 8-BIT current slots
256 warmup frames
4096 measured frames
3 identical reset-to-run repetitions
```

S2 starts from the same 8+8 state and requires one 256-frame callback with queue depth 32 at entry and four audition voices, producing exactly 1024 audition `sine()` calls. It also repeats three times identically.

Host wall-clock data is informational only. Target TIMER4 runtime timing remains deferred until a reproducible execution-return path exists. Emulator timing is never a physical FM-1 margin claim.

## Exact next action

1. get `tests/run_wp5_10_contract_freeze.sh` green in CI;
2. implement S1/S2 with generated test-only wrappers/probes;
3. add the final dual-engine realtime audit;
4. run inherited WP5.9/D056/D055/all-8BIT regressions;
5. run uninstrumented pinned-target resource closeout;
6. open an optimization sub-contract only if accepted evidence proves it necessary;
7. otherwise record Phase 5 software exit with physical validation debt explicit.
