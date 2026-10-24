# FM1Tracker Current Test State

Date: 2026-10-24  
Purpose: **current verification overlay**

Legend: ✅ verified/frozen, ⬜ active/future, ⛔ physical gate unavailable.

## Stable completed baseline

Phase 1-3 are complete.

Phase 4 / D046 complete all-8-BIT baseline:

- representative WAV `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9` ✅
- normal WAV `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076` ✅
- target 72,504 B image / 19,604 B RAM / 113,000 B pool ✅
- run `40124536375`, job `120963022289` ✅

WP5.0 / D048 mixed-engine routing:

- per-instrument family + engine authoritative ✅
- song mode non-gating ✅
- 8 B router / 64 B publication ✅
- immediate/delayed family routing and fail-safe malformed behavior ✅
- non-applicable family p-locks ignored, never reinterpreted ✅
- zero active cross-track primary steals ✅
- D046 representative/normal WAV byte-identical ✅
- inherited/realtime gates ✅
- target 72,792 B image / 19,676 B RAM / 113,000 B pool ✅
- run `40129822006`, job `120978955451` ✅

## WP5.1 / D049 current state

Normative contract: `WP5_1_FM_OPERATOR_CONTRACT.md`.  
Pre-implementation evidence: `WP5_1_PREIMPLEMENTATION_EVIDENCE.md`.

| Check | Status |
|---|---|
| D049 contract frozen before production source | ✅ |
| operator state 4 B / uint32 phase | ✅ |
| signed int32 phase modulation | ✅ |
| Q15 level 0..32767 | ✅ |
| 257-entry quarter-wave / 514 B | ✅ |
| table SHA `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68` | ✅ |
| long reference SHA `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66` | ✅ |
| isolated object ceiling <= 4,096 B text+rodata | ✅ |
| contract-first source-absent gate | ✅ run `40131135259`, job `120982895786` |
| 65,536 reference sign-symmetry probes | ✅ |
| frozen sine/interpolation/modulation/level vectors | ✅ |
| reference 4,096-render vector/final phase | ✅ |
| WP5.0 normal WAV still byte-identical before implementation | ✅ |
| generated checked-in table + generator | ⬜ next |
| production `fmt_fm_operator.*` | ⬜ next |
| focused C production vectors | ⬜ |
| reset/retrigger/null safety | ⬜ |
| phase wrap / zero-increment behavior | ⬜ |
| production long-vector hash | ⬜ |
| no float/double/heap/divide/modulo/64-bit in primitive | ⬜ |
| pinned JieLi target module compile | ⬜ |
| exact isolated object sections | ⬜ |
| exact WP5.0 full-app target/WAV/BIN/FWSC regression | ⬜ |
| `WP5_1_VERIFICATION.md` | ⬜ |

## Explicit WP5.1 non-goals

No algorithms, ratio policy, feedback, FM envelopes, FM voice allocation, FM parameter pages/p-lock IDs or audible mixed FM production integration belong to WP5.1.

## Physical hardware boundary

No physical FM-1 is available. Physical install/boot/audio quality/control feel/screen feel and measured AC791N callback margin remain unverified.

`hardware_margin_claim=none`
