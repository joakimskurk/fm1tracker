# FM1Tracker - Start Here

Repository state is authoritative, but roadmap order is not the whole project memory. Read persistent product context before reducing the project to the active work package.

Read in this order:

1. `docs/PROJECT_CONTEXT.md`
2. `docs/PROJECT_STATE.md`
3. `docs/WP5_10_PRECONTRACT_AUDIT.md`
4. active `docs/WP5_10_WORST_CASE_MIXED_BENCHMARK_CONTRACT.md` once frozen
5. `docs/WP5_9_CLOSEOUT_EVIDENCE.md`
6. `docs/WP5_9_DEFECT_001_DELAYED_CROSS_FAMILY.md`
7. `docs/WP5_9_DEFECT_001_FIX_CONTRACT.md`
8. `docs/WP5_8_CLOSEOUT_EVIDENCE.md`
9. `docs/CURRENT_DECISIONS.md`
10. `docs/WORK_PACKAGES.md`
11. `docs/EXECUTION_PROTOCOL.md`
12. `docs/CURRENT_TEST_STATE.md`
13. `docs/TEST_MATRIX.md`
14. `docs/ROADMAP.md`
15. `THIRD_PARTY.md`

## Product context first

FM1Tracker is one engine-agnostic hardware tracker/sequencer, not an FM-only project. The intended product includes 8-BIT, FM, future virtual-analog/subtractive synthesis and drums/percussion in the same tracker environment. Exact DSP/resource contracts for not-yet-implemented engine families are future work and must be audited before implementation.

`docs/PROJECT_CONTEXT.md` is the persistent product-memory layer. Substantive user decisions and product clarifications must be synchronized back into Git according to `docs/EXECUTION_PROTOCOL.md`.

## Current execution

**WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit is ACTIVE.**

```text
issue                         #53
branch                        phase5/worst-case-benchmark
precontract audit             COMPLETE
benchmark contract            NEXT
parent docs handoff           c06550f72918964bd49467228688a7232d2c4cf9
last target-validated code    81d3464059270130747e26fcc950b7a2e1bb03e1
WP5.9 closeout run            40210173573 SUCCESS
WP5.9 target                  121219510315 SUCCESS
WP5.9 host                    121219510342 SUCCESS
image                         79836 B
normal RAM                    27792 B
pool                         113000 B
hardware_margin_claim         none
```

Audit: `docs/WP5_10_PRECONTRACT_AUDIT.md`.

No benchmark implementation or production optimization is legal until the WP5.10 benchmark contract and fail-closed freeze gate are committed.

## WP5.10 audit facts that must survive handoff

- The production `app_audio()` renders both 8-BIT and FM4 once per tracker frame.
- The tracker remains eight logical tracks, but 8-BIT and FM4 each own eight family-local voice slots.
- Detached release tails can make total non-idle DSP occupancy exceed eight, so the benchmark must prove actual occupancy at its measurement window.
- The contract must attempt a reachable 8 FM4 + 8 8-BIT non-idle saturation state and record the measured reachable maximum if full 8+8 cannot be reached through ordinary tracker semantics.
- The same callback can also process up to 32 audition-queue events and four manual/audition voices. Mixed-engine saturation and full-callback pressure must therefore be separate evidence cases.
- Existing WP4.10 test-only wrappers/digests and WP5.9 mixed source-injected fixtures are the preferred non-invasive benchmark mechanisms.
- Target HAL exposes 24 MHz TIMER4 through `fm1_ticks()`, but current automated target CI does not execute on physical hardware.
- Host timing is informational only. Emulator timing, if obtained, is emulator timing only.
- No physical FM-1 timing, audio-stability or free-RAM claim is permitted.

## Parent resource baselines

```text
D056 image                  79804 B
WP5.9 image                 79836 B
normal RAM                  27792 B
pool                       113000 B
WP5.9 delta vs D056         +32 B image / +0 B RAM / +0 B pool
hardware_margin_claim       none
```

WP5.9 closeout evidence: `docs/WP5_9_CLOSEOUT_EVIDENCE.md`.

## Exact next action

1. freeze `docs/WP5_10_WORST_CASE_MIXED_BENCHMARK_CONTRACT.md`;
2. add its fail-closed contract-freeze verification gate;
3. commit the contract checkpoint before benchmark code;
4. implement test-only mixed saturation and full-callback pressure evidence;
5. preserve inherited all-8BIT, D055, D056 and WP5.9 regressions;
6. measure uninstrumented target image/RAM/pool against the frozen baselines;
7. authorize production optimization only through a new bounded sub-contract if measured evidence proves it necessary;
8. record the Phase 5 exit decision.

WP5.10 does not implement future VA/subtractive or drum engines. Those remain explicit product directions and must survive the handoff.
