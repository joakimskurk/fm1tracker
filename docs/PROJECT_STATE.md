# FM1Tracker Project State

This is the **single source of truth for current execution state**.

Read `START_HERE.md` first.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.6 complete; WP5.7/D055 technically complete, documentation closeout active - 2026-10-25**

Current active package: **WP5.7 - FM4 voice allocation and D048 router integration closeout**  
Active issue: **#50**  
Active branch: `phase5/fm4-voice-integration`

## Completed Phase 5 foundation

- WP5.0 / D048 mixed-engine routing - COMPLETE.
- WP5.1 / D049 fixed-point FM operator - COMPLETE.
- WP5.2 / D050 curated four-operator routing - COMPLETE.
- WP5.3 / D051 integer ratio/pitch mapping - COMPLETE.
- WP5.4 / D052 operator level/modulation-index mapping - COMPLETE.
- WP5.5 / D053 bounded OP4 feedback - COMPLETE.
- WP5.6 / D054 four independent FM4 ADSR envelopes - COMPLETE.

## WP5.7 / D055 technical checkpoint

Normative: `docs/WP5_7_FM_VOICE_INTEGRATION_CONTRACT.md`.  
Evidence: `docs/WP5_7_VERIFICATION.md`.

Canonical implementation checkpoint:

```text
head    75ff8136881799ad652df380a53002c5efa70516
run     40173076046 SUCCESS
host    121107852886 SUCCESS
target  121107852943 SUCCESS
```

Verified architecture:

```text
FM slots                    8
FmtFmVoice                 92 B
FmtFmEngine               788 B
active cross-track steals    0
allocator                   exact D043
mutable FM parameter bank     0 B
```

Verified full target:

```text
image       77048 B  (+4256 vs WP5.0)
normal RAM  20464 B  (+788 vs WP5.0)
pool       113000 B  (unchanged)
```

Default all-8BIT production output remains exact WP5.0:

```text
frames  286464
sha256  17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076
```

A production `FM4 -> 8-BIT -> FM4` same-track fixture renders deterministically with byte-identical repeated WAV output.

## Current exact action

This documentation closeout head must pass the normal WP5.7 host+target workflow. When green, close #50 and branch WP5.8 from this exact head.

WP5.8 is **FM parameter pages + p-lock mapping**. It must begin contract-first by auditing existing page semantics, p-lock semantic IDs/storage, family-specific dispatch and the temporary immutable 28 B WP5.7 bridge patch before any mutable FM parameter source is introduced.

No physical FM-1 is available. `hardware_margin_claim=none`.
