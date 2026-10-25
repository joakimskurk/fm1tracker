# WP5.9 Closeout Evidence - Mixed 8-BIT + FM4 Validation

Status: **CLOSED**  
Date: 2026-10-25  
Branch: `phase5/mixed-validation`  
Tracking issue: `#52`  
Last target-validated production checkpoint: `81d3464059270130747e26fcc950b7a2e1bb03e1`  
Closeout workflow run: `40210173573`  
Target job: `121219510315` - SUCCESS  
Host job: `121219510342` - SUCCESS

## Frozen basis

WP5.9 was governed by:

- `docs/WP5_9_PRECONTRACT_AUDIT.md`
- `docs/WP5_9_MIXED_VALIDATION_CONTRACT.md`
- `docs/WP5_9_DEFECT_001_DELAYED_CROSS_FAMILY.md`
- `docs/WP5_9_DEFECT_001_FIX_CONTRACT.md`
- the accepted WP5.8 / D056 baseline in `docs/WP5_8_CLOSEOUT_EVIDENCE.md`

The original WP5.9 package was validation-only unless a new fixture exposed an actual production defect. The eight logical tracker tracks remained mandatory. Physical-device audio stability and physical free-RAM margin remained outside the evidence boundary because no physical FM-1 is available.

## Defect-governed production change

The new delayed cross-family validation exposed a real cleanup defect before a production fix was made. When an outgoing delayed lane from one engine family reached zero while the immediate step of the other family started, stale silent family enable/on state could remain latched.

The failure was recorded in `docs/WP5_9_DEFECT_001_DELAYED_CROSS_FAMILY.md`. Production work then stopped until `docs/WP5_9_DEFECT_001_FIX_CONTRACT.md` froze the narrow correction.

The amendment allowed only the bounded cleanup correction in `firmware/src/dinghy.c` and froze these resource limits:

- normal RAM growth: `0 B`
- `vm40_pool` growth: `0 B`
- app-image growth: at most `+1024 B`

No broader DSP rule, synthesis feature or optimization was reopened.

## Host closeout

The closeout host job `121219510342` completed successfully at production checkpoint `81d3464059270130747e26fcc950b7a2e1bb03e1`.

The host closeout retains the standard regression suite together with the WP5.9 mixed-engine fixtures and the defect regression. This keeps the existing all-8BIT, default/no-lock FM and locked-FM behavior in the verification chain while adding the real mixed-family cases required by the WP5.9 contract.

## Target closeout

Target job `121219510315` completed successfully at the same production checkpoint.

Pinned-target evidence includes:

- `WP5.9 TARGET MIXED VALIDATION: PASS`
- delayed single-family validation in both directions: PASS
  - delayed PCM -> FM: PASS
  - delayed FM -> PCM: PASS
- delayed mixed -> PCM validation: PASS
- inherited D056 target closeout regression: PASS

This closes the validation gap identified by the precontract audit and keeps the defect reproduction/fix path under target verification.

## Resource reconciliation

The accepted D056 baseline and the final WP5.9 target measurement are:

| Resource | D056 baseline | WP5.9 | Delta |
|---|---:|---:|---:|
| app image | 79,804 B | 79,836 B | +32 B |
| normal/runtime RAM | 27,792 B | 27,792 B | +0 B |
| `vm40_pool` | 113,000 B | 113,000 B | +0 B |

The defect fix therefore satisfies the frozen amendment:

- RAM growth is exactly zero;
- pool growth is exactly zero;
- image growth is +32 B, well inside the +1024 B cap.

These are target build/resource measurements, not a physical free-RAM or physical-audio stability claim.

`hardware_margin_claim=none`

## Acceptance outcome

WP5.9 acceptance is satisfied:

1. real simultaneous-family mixed validation is present and green;
2. strengthened family-switch/event cleanup coverage is present and green;
3. the discovered delayed cross-family defect was documented before production repair;
4. the repair stayed inside its separately frozen source/resource contract;
5. host and pinned-target closeout are green on one production checkpoint;
6. D056 resource growth is explicitly reconciled;
7. all eight logical tracker tracks remain intact;
8. no physical-hardware margin claim is made.

**WP5.9 is CLOSED.**

`81d3464059270130747e26fcc950b7a2e1bb03e1` remains the last target-validated WP5.9 production-code checkpoint. Any later closeout/handoff documentation commit is documentation-only and must not be represented as a new target-validated firmware checkpoint.

## Next package

Per `docs/WORK_PACKAGES.md`, the next package is **WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit**.

WP5.10 must begin with a precontract audit and freeze the benchmark/load definition before any production optimization. The eight-track logical model remains mandatory, optimization is allowed only in response to a measured constraint, and `hardware_margin_claim=none` remains explicit until real hardware evidence exists.
