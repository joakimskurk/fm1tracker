# FM1Tracker Project State

This is the **single source of truth for current execution state**.

Read `START_HERE.md` first.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.3 complete; WP5.4 is next and must begin contract-first - 2026-10-25**

Current completed package: **WP5.3 - FM4 integer ratio table and pitch mapping**  
Completed issue: **#46**  
Closeout branch: `phase5/fm4-ratios`  
Next package: **WP5.4 - FM4 operator level and modulation-index mapping**  
Next issue: **#47**  
Next branch: `phase5/fm4-levels`

Do not begin WP5.5 feedback, WP5.6 envelopes, WP5.7 voice integration, WP5.8 tracker parameter pages or WP5.9 audible mixed integration while WP5.4 is active.

## Completed Phase 5 baseline

- WP5.0 / D048 mixed-engine routing - COMPLETE.
- WP5.1 / D049 fixed-point FM operator - COMPLETE.
- WP5.2 / D050 curated four-operator routing - COMPLETE.
- WP5.3 / D051 integer ratio/pitch mapping - COMPLETE.

Canonical runs:

- WP5.0 `40129822006`, job `120978955451`.
- WP5.1 `40134823600`, host `120994003059`, target `120994002842`.
- WP5.2 `40136425976`, host `120998804031`, target `120998804175`.
- WP5.3 source-absent D051 gate `40138575267`, job `121005262360`.
- WP5.3 canonical implementation verification `40139154497`, host `121006986068`, target `121006985928`.

WP5.3 verification: `docs/WP5_3_VERIFICATION.md`.

## D051 frozen compatibility

Stable ratio IDs 0..15 represent:

`0.5, 1.0, 1.25, 1.4140625, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 5.0, 6.0, 7.0, 8.0, 12.0, 16.0`

Frozen behavior:

- unsigned Q8 ratio representation;
- UNITY ID 1 is invalid-ID fallback;
- canonical base maximum `1293741834`;
- output ceiling `0x7fffffff`;
- precomputed per-ratio clamp boundaries;
- 32-bit split-Q8 round-half-up scaling;
- no runtime division/modulo, int64 or float;
- stateless four-output mapping;
- exhaustive 262,144-output SHA `5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c`;
- exactly 27,615 legal pitch/ratio pairs intentionally hit the ceiling;
- D051 ceiling prevents accidental oscillator-increment fold/wrap but is not an anti-aliasing claim.

Pinned PI32v2 ratio object:

- text+rodata **264 B**;
- ceiling 1,024 B;
- SHA `aa5bd8b9e1afa72054af2effeefbf468cfbb8b2b8d978e028540a30dc162956d`.

The only WP5.3 production additions are `firmware/src/fmt_fm_ratio.h/.c`. `dinghy.c`, D038, D049 and D050 remain unchanged.

## Full target identity after WP5.3

FM primitives through D051 remain isolated from the running application, so target identity remains exact WP5.0:

```text
image       72792 B
normal RAM  19676 B
pool RAM   113000 B
identity   FM-1_987
BIN SHA    a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125
FWSC SHA   cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073
```

## Next package - WP5.4 / issue #47

WP5.4 must start contract-first. D052 does **not** exist yet.

Required first work:

1. inspect D049 `level_q15[4]` semantics and D050 `mod_depth_q14` semantics across all four algorithms;
2. decide the smallest musically useful control model for carrier level and modulation strength without blindly copying a classic synth UI;
3. freeze stable internal control domains, exact integer mappings, clamp/rounding behavior and zero/nominal/max semantics;
4. decide whether V1 uses the current global modulation depth or requires a bounded per-operator/per-edge representation;
5. prove carrier/modulator gain behavior and overflow safety across all four D050 algorithms and representative D051 ratios;
6. freeze D052 and a source-absent reference gate before production mapping source.

WP5.4 excludes feedback, envelopes, voice/router integration, tracker-facing p-lock IDs/pages and audible mixed-engine production.

## Hardware boundary

No physical FM-1 is available. `hardware_margin_claim=none`.

## Last updated

2026-10-25 - WP5.3 / D051 complete and fully verified. WP5.4 / #47 is the next package and must begin contract-first.
