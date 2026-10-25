# FM1Tracker Project State

This is the **single source of truth for current execution state**.

Read `START_HERE.md` first.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.5 complete; WP5.6 ACTIVE contract-first - 2026-10-25**

Current active package: **WP5.6 - bounded FM4 envelope strategy**  
Active issue: **#49**  
Active branch: `phase5/fm4-envelope`  
Parent closeout: **WP5.5 / D053** at `216c7e34b6d761cadb683bd1fc356c216b98cc27`

Do not begin WP5.7 voice/router integration, WP5.8 tracker parameter pages, WP5.9 audible mixed integration or WP5.10 Phase 5 exit while WP5.6 is active.

## Completed Phase 5 baseline

- WP5.0 / D048 mixed-engine routing - COMPLETE.
- WP5.1 / D049 fixed-point FM operator - COMPLETE.
- WP5.2 / D050 curated four-operator routing - COMPLETE.
- WP5.3 / D051 integer ratio/pitch mapping - COMPLETE.
- WP5.4 / D052 operator level/modulation-index mapping - COMPLETE.
- WP5.5 / D053 bounded OP4 one-sample feedback - COMPLETE.

WP5.5 final closeout:

- head `216c7e34b6d761cadb683bd1fc356c216b98cc27`;
- run `40165352623`;
- host `121085156557` SUCCESS;
- target `121085156767` SUCCESS;
- host artifact ID `12588226173`, SHA `e22ee8417d0c1521f57bf56ef5b89e159edf898f768afda61eef2ec22aae5798`;
- target artifact ID `12588765241`, SHA `bfbfdf8d49e275edcc3b0a540b40c6e85c8c97ad2515092b02e2b988f091593b`.

## WP5.6 precontract audit

Audit: `docs/WP5_6_PRECONTRACT_AUDIT.md`.

Current findings:

- a post-D050 common amplitude envelope is rejected for V1 because it only shapes final loudness and cannot independently evolve operator/modulator contribution;
- D052 already exposes four independent operator LEVEL values before D049/D050 rendering, which is the clean insertion surface for envelopes;
- D049 accepts a Q15 level per operator and therefore needs no contract change;
- D050 evaluates each operator exactly once per sample, so four fixed envelope level scalars remain bounded O(4);
- D053 feedback consumes OP4 after level scaling, so an OP4 envelope naturally changes both OP4 audible/modulation contribution and feedback history without changing D053 semantics;
- current working candidate is **four independent per-operator envelopes**, applied by scaling D052 Q15 LEVEL before D049 render;
- D054 is **not frozen yet**. Time mapping, stage model, update cadence, state packing, zero-time semantics and exact ceilings must be proven first.

## Full target identity at WP5.6 entry

The isolated FM primitives remain outside the running application:

```text
image       72792 B
normal RAM  19676 B
pool RAM   113000 B
identity   FM-1_987
BIN SHA    a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125
FWSC SHA   cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073
```

No physical FM-1 is available. `hardware_margin_claim=none`.

## Current exact action

1. finish the common-vs-per-operator envelope architecture audit;
2. choose the smallest musically useful V1 stage/control model;
3. freeze exact integer time/rate mapping, level domain, retrigger/reset/release and zero-time semantics;
4. freeze sample/control update cadence and persistent-state/object ceilings;
5. write D054 plus source-absent reference tests;
6. pass the D054 source-absent CI gate before any `fmt_fm_envelope.*` production source exists.
