# FM1Tracker Project State

This is the **single source of truth for current execution state**.

Read `START_HERE.md` first.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.6 complete; WP5.7 D055 source-absent gate ACCEPTED, implementation authorized - 2026-10-25**

Current active package: **WP5.7 - FM4 voice allocation and D048 router integration**  
Active issue: **#50**  
Active branch: `phase5/fm4-voice-integration`

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

## D055 accepted contract

Normative: `docs/WP5_7_FM_VOICE_INTEGRATION_CONTRACT.md`.  
Audit: `docs/WP5_7_PRECONTRACT_AUDIT.md`.  
Preimplementation evidence: `docs/WP5_7_PREIMPLEMENTATION_EVIDENCE.md`.

Accepted source-absent gate:

```text
head      01f8f6c8f81f9057909d99223372b0901296e3ec
run       40171940961
job       121104425642
status    SUCCESS
artifact  12590656915
SHA-256   ab44e0a8553edaf02eefbf8c175b717a7d5ee32ae4c9ad44de77d94228f0d9c5
```

Frozen D055 architecture:

```text
FM slots                     8
FmtFmVoice                  92 B
FmtVoiceAllocator           52 B
FmtFmEngine                788 B
immutable bridge patch      28 B ROM
mutable FM instrument bank   0 B in WP5.7
```

Allocator remains exact D043 policy: reuse own primary, otherwise lowest idle, otherwise oldest detached RELEASE tail, never another track's active primary.

The 28 B bridge patch is temporary WP5.7 integration input. WP5.8 owns mutable FM parameter pages/p-lock storage.

D055 permits exactly one backward-compatible D050 composition seam for externally rendered OP4 so D053 feedback can compose without duplicating topology logic.

## D055 resource ceilings

```text
parent image          72792 B
parent normal RAM     19676 B
parent pool          113000 B

FmtFmEngine target      788 B
engine hard ceiling     800 B
RAM increment max      1024 B
total RAM max         20700 B
image increment max    8192 B
total image max       80984 B
pool increment            0 B
pool total           113000 B
```

## Production boundary now authorized

Only these production files may change during WP5.7 implementation:

```text
firmware/src/fmt_fm_engine.h
firmware/src/fmt_fm_engine.c
firmware/src/fmt_fm_algorithm.h
firmware/src/fmt_fm_algorithm.c
firmware/src/dinghy.c
```

D049, D051, D052, D053, D054, D043 allocator source and generated tables remain frozen. D050 may change only for the D055 composition seam.

## Current exact action

1. add the D050 externally-rendered-OP4 composition seam while keeping the old D050 renderer bit-exact;
2. implement the exact 8-slot / 788 B FM engine;
3. connect FM family cases to the existing D048 router in `dinghy.c`;
4. verify event/release/retrigger/cross-family semantics and zero active cross-track steals;
5. require the normal all-8BIT host WAV to remain exact WP5.0;
6. run pinned PI32v2/full-target build inside the D055 image/RAM/pool ceilings;
7. write `WP5_7_VERIFICATION.md` and close #50 before WP5.8 begins.

No WP5.8 parameter-page/p-lock work is allowed in WP5.7.

No physical FM-1 is available. `hardware_margin_claim=none`.
