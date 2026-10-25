# FM1Tracker Project State

This is the **single source of truth for current execution state**.

Read `START_HERE.md` first.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.6 complete; WP5.7 D055 contract freeze active - 2026-10-25**

Current active package: **WP5.7 - FM4 voice allocation and D048 router integration**  
Active issue: **#50**  
Active branch: `phase5/fm4-voice-integration`

Do not begin production FM voice/router integration until the D055 source-absent gate is green and durable.

## Completed Phase 5 baseline

- WP5.0 / D048 mixed-engine routing - COMPLETE.
- WP5.1 / D049 fixed-point FM operator - COMPLETE.
- WP5.2 / D050 curated four-operator routing - COMPLETE.
- WP5.3 / D051 integer ratio/pitch mapping - COMPLETE.
- WP5.4 / D052 operator level/modulation-index mapping - COMPLETE.
- WP5.5 / D053 bounded OP4 feedback - COMPLETE.
- WP5.6 / D054 four independent FM4 ADSR envelopes - COMPLETE.

WP5.6 final closeout:

- head `886cb496f4e73a91c4ded16bf9e722b770a040f6`;
- run `40168286355`;
- host `121093715194` SUCCESS;
- target `121093715000` SUCCESS;
- full running application remains exact WP5.0.

## WP5.7 D055 current contract candidate

Normative contract: `docs/WP5_7_FM_VOICE_INTEGRATION_CONTRACT.md`.  
Audit: `docs/WP5_7_PRECONTRACT_AUDIT.md`.

Frozen candidate architecture:

```text
FM slots                     8
FmtFmVoice                  92 B
FmtVoiceAllocator           52 B
FmtFmEngine                788 B
immutable bridge patch      28 B ROM
mutable FM instrument bank   0 B in WP5.7
```

Allocator policy is the existing D043 policy:

- reuse own primary;
- otherwise lowest idle;
- otherwise oldest detached release tail;
- never steal another track's active primary.

The bridge patch is only for WP5.7 integration. WP5.8 owns mutable FM parameter pages/p-lock storage.

D055 permits one narrow backward-compatible D050 composition seam so D053 can render OP4 before D050 renders the remaining topology. Existing D050 output must remain exact.

## D055 resource ceilings

```text
parent image          72792 B
parent normal RAM     19676 B
parent pool          113000 B

FM engine target        788 B
RAM increment max      1024 B
total RAM max         20700 B
image increment max    8192 B
total image max       80984 B
pool increment            0 B
pool total           113000 B
```

## Current exact action

1. freeze D055 plus source-absent reference tests;
2. prove production source is still exactly the WP5.6 closeout;
3. run inherited D054/D053/.../WP5.0 host regressions;
4. record durable preimplementation evidence;
5. only then implement the FM engine, D050 composition seam and D048 FM dispatch.

No WP5.8 parameter-page/p-lock work is allowed in WP5.7.

No physical FM-1 is available. `hardware_margin_claim=none`.
