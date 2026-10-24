# FM1Tracker Current Test State

Date: 2026-10-24  
Purpose: **current verification overlay**

`TEST_MATRIX.md` remains the broader regression index. Detailed historical evidence remains in the package verification files.

Legend:

- ✅ verified
- 🟨 frozen/committed, CI evidence pending
- ⬜ active/future requirement
- ⛔ physical gate unavailable

## Stable completed baseline

Phase 1-3 are complete.

Phase 4 / D046 complete all-8-BIT baseline:

- representative WAV `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9` ✅
- normal WAV `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076` ✅
- target 72,504 B image / 19,604 B RAM / 113,000 B pool ✅
- canonical run `40124536375`, job `120963022289` ✅

WP5.0 / D048 mixed-engine routing is complete:

- per-instrument family + engine authoritative ✅
- `FmtSongMeta.mode` non-gating ✅
- 8 B router / 64 B publication ✅
- deterministic immediate/delayed cross-family routing ✅
- malformed/unsupported family/engine fail-safe ✅
- non-applicable p-locks ignored, never reinterpreted ✅
- zero active cross-track primary steals ✅
- D046 representative/normal WAV byte-identical ✅
- inherited/realtime gates ✅
- target 72,792 B image / 19,676 B RAM / 113,000 B pool ✅
- canonical run `40129822006`, job `120978955451` ✅
- evidence `WP5_0_VERIFICATION.md` ✅

## WP5.1 / D049 current state

Normative contract: `WP5_1_FM_OPERATOR_CONTRACT.md`.

| Check | Status | Requirement / frozen result |
|---|---|---|
| D049 contract frozen before production source | ✅ | contract commit contains no FM operator production source |
| operator state | ✅ frozen | one `uint32_t phase`, exactly 4 B |
| phase convention | ✅ frozen | one cycle = 2^32, unsigned wrap, sample-before-advance |
| phase modulation | ✅ frozen | signed int32 phase offset, modulo 2^32 |
| operator level | ✅ frozen | Q15 clamp 0..32767 |
| carrier representation | ✅ frozen | 257-entry positive quarter-wave Q15 table |
| interpolation | ✅ frozen | 22-bit fractional linear interpolation |
| output domain | ✅ frozen | -32767..32767 |
| table binary hash | ✅ frozen | `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68` |
| long reference hash | ✅ frozen | `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66` |
| isolated object ceiling | ✅ frozen | text+rodata <= 4,096 B |
| full-app identity requirement | ✅ frozen | exact WP5.0 WAV/BIN/FWSC/resources |
| contract-first reference CI | 🟨 | pending green run after D049 commit |
| production `fmt_fm_operator.*` | ⬜ | forbidden until contract-first CI is green |
| checked-in generated table + generator | ⬜ | implementation step after gate |
| focused C operator vectors | ⬜ | must match D049 exactly |
| reset/retrigger/null safety | ⬜ | production test |
| phase-wrap/zero-increment tests | ⬜ | production test |
| positive/negative/extreme modulation tests | ⬜ | production test |
| level clamp/sign symmetry tests | ⬜ | production test |
| 4,096-render production hash | ⬜ | must match D049 |
| no float/double/heap/divide/modulo/64-bit arithmetic | ⬜ | production source audit |
| pinned JieLi target compile | ⬜ | isolated module |
| isolated object section size | ⬜ | record exact values |
| full WP5.0 app exact regression | ⬜ | byte-identical because operator remains isolated |
| `WP5_1_VERIFICATION.md` | ⬜ | required before WP5.2 |

The contract-first gate must also prove 65,536 deterministic half-cycle sign-symmetry probes and reproduce all frozen sine/interpolation/modulation/level/render-order vectors before production source exists.

## Explicit WP5.1 non-goals

No algorithms, ratio policy, feedback, FM envelopes, FM voice allocation, FM parameter pages/p-lock IDs or audible mixed FM production integration belong to WP5.1.

## Physical hardware boundary

No physical FM-1 is available. Physical install/boot/audio quality/control feel/screen feel and measured AC791N callback margin remain unverified.

`hardware_margin_claim=none`
