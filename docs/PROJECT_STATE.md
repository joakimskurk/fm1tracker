# FM1Tracker Project State

This is the **single source of truth for current execution state**.

Read `START_HERE.md` first.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.7 complete; WP5.8/D056 frozen and preimplementation evidence accepted - 2026-10-25**

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
Accepted preimplementation evidence: `docs/WP5_8_PREIMPLEMENTATION_EVIDENCE.md`.

D056 source-absent freeze is accepted:

```text
contract head            9436e385a96fa89b74c4c016c03f7857c2373552
run                      40178938471 SUCCESS
job                      121125607384 SUCCESS
artifact                 wp5-8-contract-evidence / 12592494488
artifact SHA-256         9c8ca18fe027b7286c3fe4e0b2086e133780b89df8721a01f4a91eafc672d51c
production authorization yes - bounded by D056
```

Frozen implementation contract:

```text
FM semantic IDs          27
namespace                0x60..0x7B, except 0x63
logical pages             7
FmtFmParamSet             27 B target
FM base bank             432 B target
max p-locks per step       4 unchanged
generalized row cache   8704 B target
legacy 8BIT view        2048 B exact
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

D049-D055 synthesis/voice behavior remains frozen except for the bounded D056 adapter/integration seam. The legacy `fmt_8bit_row_param_cache` ABI/layout and exact 2048-byte footprint remain frozen.

## Current exact action

Begin the bounded D056 production activation. First extend the existing semantic parameter architecture with the frozen FM IDs, domains and seven-page metadata, then add the compact FM row override publication path while preserving the legacy 8BIT cache view. Stored 16-bit p-lock values must be domain-validated before publication; the audio inner loop must never traverse the lock store.

After each coherent slice, run the focused D056 tests plus inherited parameter-registry and D055 regressions. Do not start WP5.9.

No physical FM-1 is available. `hardware_margin_claim=none`.
