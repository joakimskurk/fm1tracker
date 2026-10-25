# FM1Tracker Project State

This is the **single source of truth for current execution state**.

Read `START_HERE.md` first.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.4 complete; WP5.5 bounded feedback ACTIVE contract-first - 2026-10-25**

Current active package: **WP5.5 - bounded deterministic FM4 feedback**  
Active issue: **#48**  
Active branch: `phase5/fm4-feedback`  
Parent closeout: **WP5.4 / D052** at `d3501eca952f3e903d4dcc4faab2851d439f7ad0`

D053 does **not** exist yet. No production feedback source is permitted until D053 and its source-absent gate are frozen and green.

Do not begin WP5.6 envelopes, WP5.7 voice integration, WP5.8 tracker parameter pages, WP5.9 audible mixed integration or WP5.10 Phase 5 exit while WP5.5 is active.

## Completed Phase 5 baseline

- WP5.0 / D048 mixed-engine routing - COMPLETE.
- WP5.1 / D049 fixed-point FM operator - COMPLETE.
- WP5.2 / D050 curated four-operator routing - COMPLETE.
- WP5.3 / D051 integer ratio/pitch mapping - COMPLETE.
- WP5.4 / D052 operator level/modulation-index mapping - COMPLETE.

WP5.4 final closeout evidence:

- closeout head `d3501eca952f3e903d4dcc4faab2851d439f7ad0`;
- D052 source-absent run `40140119411`, job `121009903030`;
- implementation run `40143482844`, host `121019875458`, target `121019875309`;
- final closeout-head run `40163616219`, host `121080077905`, target `121080077699`, both SUCCESS;
- PI32v2 `fmt_fm_controls.o` text+rodata **108 B**, SHA `05867ebe120829d1e3b8bf6f0504f29d63e9643e4fa5eede4c6d858d31d25059`;
- full target remains exact WP5.0.

Durable evidence: `docs/WP5_4_VERIFICATION.md`.

## D052 frozen compatibility

D052 owns exactly five internal byte controls: OP1_LEVEL, OP2_LEVEL, OP3_LEVEL, OP4_LEVEL and INDEX, all `0..255`.

```text
sanitize(x)  = min(x,255)
LEVEL -> Q15 = (v << 7) | (v >> 1)
INDEX -> Q14 = (v << 6) | (v >> 2)
```

Frozen behavior:

- `FmtFmMappedControls` exactly 10 B and transient;
- four LEVEL values independent from one global INDEX;
- no persistent D052 state;
- all 65,536 legal LEVEL x INDEX single-edge combinations signed-32 safe;
- worst legal edge magnitude `2147287044`, signed-32 margin `196603`;
- production source only `firmware/src/fmt_fm_controls.h/.c`;
- `dinghy.c`, D049, D050 and D051 unchanged.

## Full target identity at WP5.5 entry

FM primitives through D052 remain isolated from the running application:

```text
image       72792 B
normal RAM  19676 B
pool RAM   113000 B
identity   FM-1_987
BIN SHA    a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125
FWSC SHA   cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073
```

## Active package - WP5.5 / issue #48

Required order:

1. audit D049 phase-modulation arithmetic and D050 fixed evaluation order for the smallest deterministic feedback insertion point;
2. choose one bounded V1 feedback topology rather than arbitrary graph feedback;
3. freeze stable feedback control domain, zero/nominal/max semantics, clamp/rounding and malformed-input behavior;
4. freeze exact one-sample history ownership, reset semantics and algorithm-switch semantics;
5. prove arithmetic safety against D051 ratio extremes and D052 maximum LEVEL/INDEX;
6. define deterministic vectors, reset/switch fixtures and long-run stress before implementation;
7. freeze persistent-state, realtime/source and PI32v2 object ceilings;
8. freeze D053 and pass a source-absent reference gate;
9. only then add production feedback source.

WP5.5 excludes envelopes, voice/router integration, tracker-facing p-lock IDs/pages and audible mixed-engine production.

## Hardware boundary

No physical FM-1 is available. `hardware_margin_claim=none`.

## Last updated

2026-10-25 - WP5.4 / D052 formally closed. WP5.5 / #48 is ACTIVE on `phase5/fm4-feedback`; D053 contract audit is the exact next work.
