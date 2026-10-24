# FM1Tracker Current Test State

Date: 2026-10-24  
Purpose: **current verification overlay**

Legend: ✅ verified/frozen, 🟨 frozen/committed with CI evidence pending, ⬜ active/future, ⛔ physical gate unavailable.

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
- deterministic family switching/fail-safe behavior ✅
- D046 representative/normal WAV byte-identical ✅
- target 72,792 B image / 19,676 B RAM / 113,000 B pool ✅
- run `40129822006`, job `120978955451` ✅

WP5.1 / D049 fixed-point FM operator:

- D049 source-absent contract gate ✅
- 4 B operator state / uint32 phase ✅
- signed int32 phase modulation ✅
- Q15 level/output convention ✅
- generated 257-entry / 514 B quarter-wave ✅
- table SHA `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68` ✅
- 65,536 symmetry probes ✅
- long SHA `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66` ✅
- no loop/heap/float/int64/divide/modulo/store/router access in primitive ✅
- pinned PI32v2 object 692 B text+rodata ✅
- full target/audio identity exact WP5.0 ✅
- run `40134823600`, host `120994003059`, target `120994002842` ✅
- evidence `WP5_1_VERIFICATION.md` ✅

## WP5.2 / D050 current state

Normative contract: `WP5_2_FM_ALGORITHM_CONTRACT.md`.

| Check | Status |
|---|---|
| D050 frozen before production algorithm source | ✅ frozen |
| production `fmt_fm_algorithm.*` absent | 🟨 source-absent CI pending |
| algorithm count exactly 4 | ✅ frozen |
| ID 0 STACK4 | ✅ frozen |
| ID 1 FAN3 | ✅ frozen |
| ID 2 DUAL2 | ✅ frozen |
| ID 3 PARALLEL4 | ✅ frozen |
| evaluation order OP4 -> OP3 -> OP2 -> OP1 | ✅ frozen |
| every operator renders exactly once per sample | ✅ frozen |
| routing state exactly 4 D049 operators / 16 B | ✅ frozen |
| invalid algorithm fallback STACK4 | ✅ frozen |
| mod depth clamp 0..16383 | ✅ frozen |
| edge mapping `sample * depth * 4` | ✅ frozen |
| phase-offset accumulation modulo 2^32 | ✅ frozen |
| carrier counts 1/2/4 only | ✅ frozen |
| carrier normalization power-of-two toward zero | ✅ frozen |
| router object ceiling <= 2,048 B text+rodata | ✅ frozen |
| STACK4 4,096 hash `25dfde1bb29de2c70305d6abafee91fd7480e69eaa71fd1a3fbde3c1e6e855cd` | ✅ frozen |
| FAN3 4,096 hash `8d9ae5310369e3d5a36bcf8c44d5e74049d3d6e109362a844be2796b26330653` | ✅ frozen |
| DUAL2 4,096 hash `c4c31cabf4b8e121591c4c1e0923aee67523eaf7d5b60b60ca1a545c13fc2269` | ✅ frozen |
| PARALLEL4 4,096 hash `d316c3e5467cbfc606c788f669a3b19dba13535969dcce3b67b2821f0977a882` | ✅ frozen |
| source-absent topology/reference CI | 🟨 pending |
| 65,536-render-per-algorithm reference stress | 🟨 pending |
| D049 regression through gate | 🟨 pending |
| full normal WP5.0 WAV through gate | 🟨 pending |
| production routing implementation | ⬜ blocked until source-absent gate green |
| pinned PI32v2 routing object compile | ⬜ after implementation |
| `WP5_2_VERIFICATION.md` | ⬜ |

## Explicit WP5.2 non-goals

No ratio/tuning mapping, final level/mod-index UX, feedback, envelopes, FM voice allocation, FM parameter pages/p-lock IDs or audible mixed FM production integration belong to WP5.2.

## Physical hardware boundary

No physical FM-1 is available. Physical install/boot/audio quality/control feel/screen feel and measured AC791N callback margin remain unverified.

`hardware_margin_claim=none`
