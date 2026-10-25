# FM1Tracker - Start Here

Repository state is authoritative, but roadmap order is not the whole project memory. Read persistent product context before reducing the project to the active work package.

Read in this order:

1. `docs/PROJECT_CONTEXT.md`
2. `docs/PROJECT_STATE.md`
3. `docs/WP5_9_CLOSEOUT_EVIDENCE.md`
4. `docs/WP5_9_DEFECT_001_DELAYED_CROSS_FAMILY.md`
5. `docs/WP5_9_DEFECT_001_FIX_CONTRACT.md`
6. `docs/WP5_8_CLOSEOUT_EVIDENCE.md`
7. `docs/CURRENT_DECISIONS.md`
8. `docs/WORK_PACKAGES.md`
9. `docs/EXECUTION_PROTOCOL.md`
10. `docs/CURRENT_TEST_STATE.md`
11. `docs/TEST_MATRIX.md`
12. `docs/ROADMAP.md`
13. active WP5.10 audit/contract as it is created
14. `THIRD_PARTY.md`

## Product context first

FM1Tracker is one engine-agnostic hardware tracker/sequencer, not an FM-only project. The intended product includes 8-BIT, FM, future virtual-analog/subtractive synthesis and drums/percussion in the same tracker environment. Exact DSP/resource contracts for not-yet-implemented engine families are future work and must be audited before implementation.

`docs/PROJECT_CONTEXT.md` is the persistent product-memory layer. Substantive user decisions and product clarifications must be synchronized back into Git according to `docs/EXECUTION_PROTOCOL.md`.

## Current execution

**WP5.9 - mixed 8-BIT + FM4 validation is CLOSED.**

```text
issue                         #52
closeout branch               phase5/mixed-validation
last target-validated code    81d3464059270130747e26fcc950b7a2e1bb03e1
closeout run                  40210173573 SUCCESS
target                        121219510315 SUCCESS
host                          121219510342 SUCCESS
image                         79836 B
normal RAM                    27792 B
pool                         113000 B
D056 -> WP5.9 delta           +32 B image / +0 B RAM / +0 B pool
hardware_margin_claim         none
```

Full evidence: `docs/WP5_9_CLOSEOUT_EVIDENCE.md`.

The audit began as validation-only. A new delayed cross-family fixture exposed a real target cleanup defect. The defect was documented before repair, and `docs/WP5_9_DEFECT_001_FIX_CONTRACT.md` then allowed only the narrow correction in `firmware/src/dinghy.c`. The final +32 B image delta is within the frozen +1024 B cap; normal RAM and pool are unchanged.

`81d3464` remains the last target-validated production-code checkpoint. Documentation-only closeout/handoff commits after it are not new firmware validation checkpoints.

## Parent resource baseline

WP5.8 / D056 remains the frozen parent resource reference:

```text
image                       79804 B
normal RAM                  27792 B
pool                       113000 B
FWSC sha256                4d2805f428f5114531b4c3d379576d2b207cbc9a6c04fcbdd5263cfaa0cc84a0
all-8BIT WAV sha256        17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076
hardware_margin_claim      none
```

## Exact next action

Per `docs/WORK_PACKAGES.md`, activate **WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit** only after the WP5.9 closeout is durable and issue #52 is closed.

WP5.10 must then:

1. run a precontract audit before implementation changes;
2. freeze a reproducible realistic worst-case mixed-engine benchmark/load definition;
3. preserve all eight logical tracker tracks;
4. measure target/resource effects against the frozen D056/WP5.9 baseline;
5. distinguish measured target/resource evidence from physical-audio stability;
6. optimize production code only if the frozen benchmark demonstrates a real measured constraint;
7. record a Phase 5 exit decision.

WP5.10 does not implement future VA/subtractive or drum engines. Those remain explicit product directions and must survive the handoff.

No physical FM-1 is available. `hardware_margin_claim=none`.
