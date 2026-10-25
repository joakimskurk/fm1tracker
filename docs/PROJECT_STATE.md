# FM1Tracker Project State

This is the **single source of truth for current execution state**.

Read `START_HERE.md` first.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.7 complete; WP5.8/D056 contract frozen, source-absent gate active - 2026-10-25**

Current active package: **WP5.8 - FM4 parameter pages and p-lock mapping**  
Active issue: **#51**  
Active branch: `phase5/fm4-parameters`

## Completed Phase 5 foundation

- WP5.0 / D048 mixed-engine routing - COMPLETE.
- WP5.1 / D049 fixed-point FM operator - COMPLETE.
- WP5.2 / D050 curated four-operator routing - COMPLETE.
- WP5.3 / D051 integer ratio/pitch mapping - COMPLETE.
- WP5.4 / D052 level/modulation-index mapping - COMPLETE.
- WP5.5 / D053 bounded OP4 feedback - COMPLETE.
- WP5.6 / D054 four independent FM4 ADSR envelopes - COMPLETE.
- WP5.7 / D055 FM voice allocation and D048 integration - COMPLETE; closeout `80587f12786122dcec819260ddad5dd5db4b1005`, run `40177151084` SUCCESS.

## WP5.8 / D056 current state

Audit: `docs/WP5_8_PRECONTRACT_AUDIT.md`.  
Normative: `docs/WP5_8_FM_PARAMETER_PAGE_CONTRACT.md`.

D056 is frozen before production code:

```text
FM semantic IDs          27
namespace                0x60..0x7B, except 0x63
logical pages            7
FmtFmParamSet            27 B target
FM base bank            432 B target
max p-locks per step      4 unchanged
generalized row cache  8704 B target
legacy 8BIT view       2048 B exact
known normal-RAM growth 7088 B
```

Exact default normalized FM controls must compose the D055 bridge patch SHA:

```text
3d2ef849d345d12f79312af92950062db0dbfd5b56966c6f7a2d557251edac52
```

Frozen absolute package ceilings:

```text
image       <= 93432 B
normal RAM  <= 28656 B
pool         = 113000 B
```

D049-D055 synthesis/voice source and generated tables remain frozen.

## Current exact action

Run the D056 source-absent contract workflow. It must prove zero firmware/gen diff from the WP5.7 closeout and green D055 focused regressions. If green, write durable preimplementation evidence and only then begin the bounded production source activation defined in D056.

Do not start WP5.9.

No physical FM-1 is available. `hardware_margin_claim=none`.
