# WP5.9 Preimplementation Evidence

Date: 2026-10-25

WP5.9 mixed 8-BIT + FM4 validation contract was frozen before validation-harness implementation.

```text
head                     8ab25e9d7c0f31dfd2933b6dc6074b7f1236da0d
run                      40204778462 SUCCESS
job                      121203297228 SUCCESS
artifact                 wp5-9-contract-evidence / 12603476355
artifact SHA-256         78420553026b320d72988defd492a8d7e67fdab55cc3236fa5e35222cd2e21c9
```

Accepted freeze marker:

```text
WP5_9_CONTRACT_FREEZE_OK=1 production_delta=0 real_simultaneous=required same_track_switch=required event_semantics=required plock_family_safe=required target_identity=d056_exact hardware_margin_claim=none
```

The gate also re-proved the full D056 host closeout on an unchanged production tree.

At this checkpoint:

- `firmware/src/**`, `firmware/gen/**` and `tools/**` are byte-for-byte the accepted D056 target source/tool state;
- the existing real-engine mixed fixture is confirmed to be single-track/sequential rather than proof of simultaneous real 8-BIT + FM4 ownership;
- the production mixer already renders both real families once per frame and saturating-sums them;
- no production defect has been identified;
- WP5.9 validation-harness work is now authorized under `WP5_9_MIXED_VALIDATION_CONTRACT.md`.

If a new fixture exposes a production defect, validation work must stop and the contract must be amended/frozen before firmware is changed.

`hardware_margin_claim=none`.
