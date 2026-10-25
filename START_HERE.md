# FM1Tracker - Start Here

Repository state is authoritative, but roadmap order is not the whole project memory. Read persistent product context before reducing the project to the active work package.

Read in this order:

1. `docs/PROJECT_CONTEXT.md`
2. `docs/PROJECT_STATE.md`
3. `docs/WP5_9_PRECONTRACT_AUDIT.md`
4. `docs/WP5_8_CLOSEOUT_EVIDENCE.md`
5. `docs/CURRENT_DECISIONS.md`
6. `docs/WORK_PACKAGES.md`
7. `docs/EXECUTION_PROTOCOL.md`
8. `docs/CURRENT_TEST_STATE.md`
9. `docs/TEST_MATRIX.md`
10. `docs/ROADMAP.md`
11. active WP5.9 contract/evidence as it is created
12. `THIRD_PARTY.md`

## Product context first

FM1Tracker is one engine-agnostic hardware tracker/sequencer, not an FM-only project. The intended product includes 8-BIT, FM, future virtual-analog/subtractive synthesis and drums/percussion in the same tracker environment. Exact DSP/resource contracts for not-yet-implemented engine families are future work and must be audited before implementation.

`docs/PROJECT_CONTEXT.md` is the persistent product-memory layer. Substantive user decisions and product clarifications must be synchronized back into Git according to `docs/EXECUTION_PROTOCOL.md`.

## Current execution

**WP5.9 - mixed 8-BIT + FM4 validation is ACTIVE.**

```text
issue       #52
branch      phase5/mixed-validation
audit       docs/WP5_9_PRECONTRACT_AUDIT.md
mode        contract-first, validation-only unless a real defect is exposed
```

The precontract audit is complete. Its key finding is that the repository has strong routing, FM-engine and p-lock evidence, but no direct proof of **real simultaneous FM4 + 8-BIT audio on different tracks**.

Why:

- D048 cross-track coexistence uses a stub second family;
- WP5.7 uses the real FM engine but its proof pattern contains notes only on track 0, so FM -> 8-BIT -> FM happens sequentially on one track;
- D056's real FM p-lock fixture uses the same one-track shape;
- production already renders the real 8-BIT and FM engines once each per sample and saturating-sums them, but that path has not yet been isolated and proved under concurrent real-family ownership.

No production defect has been identified by the audit.

## Parent checkpoint

WP5.8 / D056 is COMPLETE.

```text
accepted run               40203483720 SUCCESS
host                       121199398344 SUCCESS
target                     121199398538 SUCCESS
image                       79804 B
normal RAM                  27792 B
pool                       113000 B
FWSC sha256                4d2805f428f5114531b4c3d379576d2b207cbc9a6c04fcbdd5263cfaa0cc84a0
all-8BIT WAV sha256        17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076
hardware_margin_claim      none
```

Full evidence: `docs/WP5_8_CLOSEOUT_EVIDENCE.md`.

The 27,792 B normal-RAM result is 864 B below the deliberately frozen D056 package ceiling. It is not a physical-free-RAM claim, but it remains mandatory planning context.

## Exact next action

1. freeze the WP5.9 mixed-validation contract from the completed audit;
2. prove that contract with a fail-closed preimplementation CI gate;
3. keep `firmware/src/**` and `firmware/gen/**` frozen while implementing the validation harness;
4. add mixed-engine-aware host observability only where required;
5. implement real simultaneous-family, same-track-switch and event-semantics fixtures;
6. preserve exact all-8BIT, D055 default/no-lock FM and D056 locked-FM behavior;
7. measure the pinned target and record exact resource deltas before closeout.

If a validation fixture exposes a target-firmware defect, stop, document the defect and amend/freeze the contract before changing production code.

WP5.9 does not implement future VA/subtractive or drum engines. Those remain explicit product directions and must survive the handoff.

No physical FM-1 is available. `hardware_margin_claim=none`.
