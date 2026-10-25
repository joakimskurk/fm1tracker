# FM1Tracker Project State

This is the **single source of truth for current execution state**.

Read `START_HERE.md` first.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.4 complete; WP5.5 bounded feedback is next and must begin contract-first - 2026-10-25**

Current completed package: **WP5.4 - FM4 operator level and modulation-index mapping**  
Completed issue: **#47**  
Closeout branch: `phase5/fm4-levels`  
Next package: **WP5.5 - bounded deterministic FM4 feedback**  
Next issue: **#48**  
Expected next branch: `phase5/fm4-feedback`

Do not begin WP5.6 envelopes, WP5.7 voice integration, WP5.8 tracker parameter pages, WP5.9 audible mixed integration or WP5.10 Phase 5 exit while WP5.5 is active.

## Completed Phase 5 baseline

- WP5.0 / D048 mixed-engine routing - COMPLETE.
- WP5.1 / D049 fixed-point FM operator - COMPLETE.
- WP5.2 / D050 curated four-operator routing - COMPLETE.
- WP5.3 / D051 integer ratio/pitch mapping - COMPLETE.
- WP5.4 / D052 operator level/modulation-index mapping - COMPLETE.

Canonical WP5.4 evidence:

- D052 source-absent run `40140119411`, job `121009903030`.
- D052 implementation verification run `40143482844`.
- host job `121019875458`.
- target job `121019875309`.
- host artifact ID `12579074081`, SHA `8f98edfc38e09a1972f5ca168ad3b8690508782ca14b31179496ec4edb27de2c`.
- target artifact ID `12579672157`, SHA `7b4d5e9d76555998ba3d099090f1e5ef592f775e25338f19b4c09de30b6bc01f`.
- PI32v2 `fmt_fm_controls.o` text+rodata **108 B**, SHA `05867ebe120829d1e3b8bf6f0504f29d63e9643e4fa5eede4c6d858d31d25059`.
- full target remains exact WP5.0.

WP5.4 verification: `docs/WP5_4_VERIFICATION.md`.

## D052 frozen compatibility

D052 owns exactly five internal byte controls: OP1_LEVEL, OP2_LEVEL, OP3_LEVEL, OP4_LEVEL and INDEX, all `0..255`.

Frozen transforms:

```text
sanitize(x)  = min(x,255)
LEVEL -> Q15 = (v << 7) | (v >> 1)
INDEX -> Q14 = (v << 6) | (v >> 2)
```

Frozen behavior:

- `FmtFmMappedControls` is exactly 10 B and transient;
- four LEVEL values remain independent from one global INDEX;
- null level input maps to four zero levels;
- no persistent state, heap, float, int64, runtime division/modulo or table search;
- all 65,536 legal LEVEL x INDEX edge combinations are signed-32 safe under the frozen D049/D050 single-edge arithmetic;
- worst legal edge magnitude `2147287044`, signed-32 margin `196603`;
- production source is only `firmware/src/fmt_fm_controls.h/.c`;
- `dinghy.c`, D049, D050 and D051 remain unchanged.

## Full target identity after WP5.4

FM primitives through D052 remain isolated from the running application, so target identity remains exact WP5.0:

```text
image       72792 B
normal RAM  19676 B
pool RAM   113000 B
identity   FM-1_987
BIN SHA    a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125
FWSC SHA   cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073
```

## Next package - WP5.5 / issue #48

WP5.5 must start contract-first. D053 does **not** exist yet.

Required first work:

1. audit D049 phase-modulation arithmetic and D050 evaluation order for the smallest deterministic feedback insertion point;
2. choose one bounded V1 feedback topology instead of expanding to arbitrary graph feedback;
3. freeze the internal feedback control domain, zero/nominal/max semantics, clamp/rounding and malformed-input behavior;
4. freeze exact one-sample history/reset/algorithm-switch semantics and state ownership;
5. prove arithmetic safety using D051 ratios and D052 maximum LEVEL/INDEX combinations;
6. define deterministic vectors, reset/switch tests and long stress before implementation;
7. freeze persistent-state and PI32v2 object ceilings;
8. freeze D053 plus a source-absent gate before any production feedback source.

WP5.5 excludes envelopes, voice/router integration, tracker-facing p-lock IDs/pages and audible mixed-engine production.

## Hardware boundary

No physical FM-1 is available. `hardware_margin_claim=none`.

## Last updated

2026-10-25 - WP5.4 / D052 complete and fully verified. WP5.5 / #48 is next and remains blocked from implementation until D053 is frozen source-absent.
