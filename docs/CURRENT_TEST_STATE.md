# FM1Tracker Current Test State

Date: 2026-10-24  
Purpose: **current verification overlay**

Legend: ✅ verified/frozen, ⬜ active/future, ⛔ physical gate unavailable.

## Stable completed baseline

Phase 1-3 are complete.

Phase 4 / D046 all-8-BIT baseline:

- representative WAV `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9` ✅
- normal WAV `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076` ✅
- target 72,504 B image / 19,604 B RAM / 113,000 B pool ✅
- run `40124536375`, job `120963022289` ✅

WP5.0 / D048 mixed-engine routing:

- per-instrument family + engine authoritative ✅
- song mode non-gating ✅
- 8 B router / 64 B publication ✅
- deterministic immediate/delayed cross-family routing ✅
- malformed/unsupported family/engine fail-safe ✅
- non-applicable family p-locks ignored, never reinterpreted ✅
- zero active cross-track primary steals ✅
- D046 representative/normal WAV byte-identical ✅
- inherited/realtime gates ✅
- target 72,792 B image / 19,676 B RAM / 113,000 B pool ✅
- run `40129822006`, job `120978955451` ✅

## WP5.1 / D049 fixed-point FM operator - COMPLETE

Normative contract: `WP5_1_FM_OPERATOR_CONTRACT.md`.  
Pre-implementation evidence: `WP5_1_PREIMPLEMENTATION_EVIDENCE.md`.  
Final evidence: `WP5_1_VERIFICATION.md`.

Canonical implementation verification:

- run `40134823600` ✅
- host job `120994003059` ✅
- target job `120994002842` ✅
- verification head `52df09bf363ad5a9ad2d722a38734eaa580ecb90` ✅

| Check | Status |
|---|---|
| D049 frozen before production source | ✅ |
| source-absent contract/reference gate | ✅ run `40131135259` |
| operator state one uint32 phase, 4 B | ✅ |
| uint32 phase / natural 2^32 wrap | ✅ |
| sample-before-advance | ✅ |
| signed int32 phase modulation | ✅ |
| Q15 level 0..32767 | ✅ |
| output -32767..32767 | ✅ |
| generated 257-entry quarter-wave | ✅ |
| table payload 514 B | ✅ |
| table SHA `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68` | ✅ |
| generator output byte-identical to checked-in table | ✅ |
| exact sine/interpolation vectors | ✅ |
| 65,536 sign-symmetry probes | ✅ |
| positive/negative/INT32 modulation edges | ✅ |
| reset/retrigger/null safety | ✅ |
| phase wrap / zero-increment hold | ✅ |
| 4,096-render SHA `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66` | ✅ |
| final phase `f1532bdf` | ✅ |
| no loop/heap/float/int64/divide/modulo/store/router access in primitive | ✅ |
| pinned PI32v2 module compile | ✅ |
| object text+rodata | ✅ 692 B / 4,096 B ceiling |
| object SHA `46868461ac6910d499975dbd93d06ee0af9db905d01c32583321eeaea0e33fad` | ✅ |
| normal WP5.0 WAV unchanged | ✅ |
| D046 representative WAV unchanged | ✅ |
| WP5.0 router regression | ✅ |
| full target image/RAM/pool unchanged | ✅ 72,792 / 19,676 / 113,000 B |
| full BIN/FWSC unchanged | ✅ |
| `WP5_1_VERIFICATION.md` | ✅ |

WP5.1 remains isolated from `dinghy.c`; FM is not yet audible in the production app.

## WP5.2 current state - ACTIVE CONTRACT-FIRST

Issue #45. Planned branch `phase5/fm4-algorithms`.

Required before production algorithm code:

| Check | Status |
|---|---|
| inspect D049 and FM4 routing constraints | ⬜ |
| curated algorithm count and stable internal IDs | ⬜ |
| exact graph topology per algorithm | ⬜ |
| exact per-sample evaluation order | ⬜ |
| signed modulation accumulation/scaling boundary | ⬜ |
| overflow/saturation policy | ⬜ |
| carrier sum/output normalization | ⬜ |
| routing-layer mutable-state requirement | ⬜ |
| deterministic graph fixtures | ⬜ |
| long-run graph hashes | ⬜ |
| target object/realtime ceiling | ⬜ |
| D050 contract committed before production routing source | ⬜ |
| source-absent pre-implementation CI | ⬜ |

WP5.2 explicitly excludes ratios, final level/mod-index UX, feedback, envelopes, voice/router integration, p-lock pages and audible mixed FM integration.

## Physical hardware boundary

No physical FM-1 is available. Physical install/boot/audio quality/control feel/screen feel and measured AC791N callback margin remain unverified.

`hardware_margin_claim=none`
