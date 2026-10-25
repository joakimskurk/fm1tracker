# FM1Tracker - Start Here

Repository state is authoritative. Do not reconstruct the plan from old chat context.

Read in this order:

1. `docs/PROJECT_STATE.md`
2. `docs/WP5_8_FM_PARAMETER_PAGE_CONTRACT.md`
3. `docs/WP5_8_PRECONTRACT_AUDIT.md`
4. `docs/WP5_7_VERIFICATION.md`
5. `docs/WP5_7_FM_VOICE_INTEGRATION_CONTRACT.md`
6. `docs/CURRENT_DECISIONS.md`
7. `docs/WORK_PACKAGES.md`
8. `docs/EXECUTION_PROTOCOL.md`
9. `docs/CURRENT_TEST_STATE.md`
10. `docs/TEST_MATRIX.md`
11. `docs/ROADMAP.md`
12. `THIRD_PARTY.md`

## Current handoff

WP5.0/D048 through WP5.7/D055 are complete.

WP5.8 / D056 is the only active package.

Active issue: #51.  
Active branch: `phase5/fm4-parameters`.

Parent closeout checkpoint:

- head `80587f12786122dcec819260ddad5dd5db4b1005`
- run `40177151084` SUCCESS
- host `121120200847` SUCCESS
- target `121120200983` SUCCESS
- target 77,048 B image / 20,464 B normal RAM / 113,000 B pool
- exact all-8BIT SHA `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`

## D056 freeze state

The WP5.8 audit is complete and D056 is frozen in `docs/WP5_8_FM_PARAMETER_PAGE_CONTRACT.md` before production implementation.

Key frozen decisions:

- 27 stable FM semantic IDs in `0x60..0x7B`, with `0x63` intentionally unassigned;
- seven logical pages: CORE/RATIO/LEVEL/ATTACK/DECAY/SUSTAIN/RELEASE;
- normalized 27 B FM base per instrument, 432 B total;
- exact D055 default-patch reproduction;
- existing max four sparse p-locks per step unchanged;
- one generalized 8,704 B physical engine-row cache preserving the old 2,048 B 8BIT view;
- D049-D055 DSP/voice modules frozen;
- +8,192 B normal-RAM and +16,384 B image package ceilings.

## Exact next action

1. require `.github/workflows/wp5_8_contract_freeze.yml` to pass on the D056 documentation/source-absent checkpoint;
2. record the run/job/artifact as WP5.8 preimplementation evidence;
3. only then activate the 27 IDs and implement `fmt_fm_params` / linked publication within the frozen source boundary;
4. preserve exact all-8BIT behavior and do not start WP5.9.

No physical FM-1 is available. `hardware_margin_claim=none`.
