# WP5.1 D049 Pre-Implementation Evidence

Date: 2026-10-24  
Package: WP5.1 / issue #44  
Branch: `phase5/fm4-operator`  
Status: **GREEN - production FM operator implementation may begin**

## Canonical gate

- D049 contract commit: `f9b97edeb66483199fdf5bbfc964839f31841dc0`
- canonical green head: `010d1dff2f74e27450edc6784ae7792eca358e59`
- workflow run: `40131135259`
- job: `120982895786`
- artifact: `12574973209`
- artifact SHA-256: `ae74fcd6272eafffdf62a27ef0f9fd415941f1c8024901c4344cb19a0212f83b`
- physical margin claim: `hardware_margin_claim=none`

## What was proven before production FM source existed

The gate ran while `firmware/src/fmt_fm_operator.c` and `.h` were absent and `dinghy.c` had no FM operator include.

It proved:

- zero `firmware/src` / `firmware/gen` delta versus WP5.0 handoff;
- the D049 257-entry quarter-wave generation rule;
- table binary SHA-256 `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68`;
- every frozen sine anchor and non-node interpolation vector;
- 65,536 deterministic half-cycle sign-symmetry probes;
- positive, negative and signed-extreme phase-modulation vectors;
- all frozen Q15 level vectors and sign mirrors;
- sample-current-before-phase-advance ordering;
- unsigned phase wrap and zero-increment modulation behavior;
- the frozen 4,096-render output SHA-256 `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66`;
- final stress-vector phase `0xf1532bdf`;
- the canonical WP5.0 normal full-app WAV remained byte-identical at `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`.

Canonical markers:

```text
WP5_1_D049_PRODUCTION_ABSENT_OK=1 firmware_delta=0 operator_source=absent dinghy_include=absent
WP5_1_D049_REFERENCE_OK=1 table_entries=257 table_sha256=6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68 long_sha256=499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66 final_phase=0xf1532bdf symmetry_probes=65536
WP5_1_D049_BASELINE_WAV_OK=1 sha256=17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076
WP5_1_D049_CONTRACT_FREEZE_OK=1 production=absent source_delta=0 reference=pass full_app_wav=wp5_0_identical hardware_margin_claim=none
```

## First run note

Run `40131001738`, job `120982495656`, failed only after the complete D049 reference model had already passed. The inherited host runner attempted to emit its PNG and Pillow was missing from the workflow environment.

The workflow was corrected to install Pillow. No contract, firmware source or reference constant changed. The canonical rerun above is green.

## Decision

The contract-first gate is satisfied. WP5.1 may now add the isolated generated sine data and `fmt_fm_operator.*` implementation. The production module must remain unlinked from `dinghy.c` and must match D049 exactly before target/full-app closeout work begins.
