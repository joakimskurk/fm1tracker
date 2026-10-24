# FM1Tracker Current Test State

Date: 2026-10-24  
Purpose: **current verification overlay**

Legend: ✅ verified/frozen, 🟨 implementation in progress, ⬜ future, ⛔ physical unavailable.

## Completed baselines

- Phase 1-3: COMPLETE ✅
- D046 Phase 4 all-8-BIT baseline: COMPLETE ✅ run `40124536375`, job `120963022289`
- D048 WP5.0 mixed-engine routing: COMPLETE ✅ run `40129822006`, job `120978955451`
- D049 WP5.1 fixed-point FM operator: COMPLETE ✅ run `40134823600`, host `120994003059`, target `120994002842`

D049 compatibility identifiers remain:

- 4 B operator state ✅
- table SHA `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68` ✅
- long SHA `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66` ✅
- PI32v2 object text+rodata 692 B ✅
- full app exact WP5.0 ✅

## WP5.2 / D050 current state

Normative contract: `WP5_2_FM_ALGORITHM_CONTRACT.md`.  
Pre-implementation evidence: `WP5_2_PREIMPLEMENTATION_EVIDENCE.md`.

### Contract-first gate

| Check | Status |
|---|---|
| D050 frozen before production algorithm source | ✅ |
| production `fmt_fm_algorithm.*` absent during reference run | ✅ |
| D049 source/data unchanged | ✅ |
| four stable algorithm IDs/topologies | ✅ |
| fixed OP4 -> OP3 -> OP2 -> OP1 evaluation | ✅ |
| all four operators advance once per sample | ✅ |
| mod depth 0..16383 / edge mapping exact | ✅ |
| modulo 2^32 phase-offset accumulation | ✅ |
| carrier normalization 1/2/4 exact | ✅ |
| invalid IDs exact STACK4 fallback | ✅ |
| first-12 vectors all algorithms | ✅ |
| four frozen 4,096-render hashes | ✅ |
| common final phases exact | ✅ |
| 65,536 renders per algorithm stress | ✅ |
| D049 production regression | ✅ |
| WP5.0 router + representative host baseline | ✅ |
| normal WP5.0 WAV unchanged | ✅ |
| source-absent run `40135979431`, job `120997461479` | ✅ |
| artifact ID `12577609841`, SHA `d3465c39b17416d58c56d98c196a18ae1c4dbd6e07d2ab7cc4ca2b3992cab85` | ✅ |

### Production routing implementation

| Check | Status |
|---|---|
| `fmt_fm_algorithm.h/.c` | 🟨 next action |
| state exactly four D049 operators / 16 B | 🟨 |
| reset/retrigger/null safety | 🟨 |
| algorithm/depth sanitizers | 🟨 |
| safe modulo phase-offset helper | 🟨 |
| sign-safe carrier normalization | 🟨 |
| exact four unrolled topologies | 🟨 |
| algorithm-switch phase continuity | 🟨 |
| focused first-12 vectors/hashes/stress | 🟨 |
| no loop/heap/float/int64/divide/modulo/store/router access in render | 🟨 |
| pinned PI32v2 router compile | ⬜ after source commit |
| router object <= 2,048 B text+rodata | ⬜ |
| full WP5.0 target identity exact | ⬜ |
| `WP5_2_VERIFICATION.md` | ⬜ |

Frozen D050 long hashes:

- STACK4 `25dfde1bb29de2c70305d6abafee91fd7480e69eaa71fd1a3fbde3c1e6e855cd`
- FAN3 `8d9ae5310369e3d5a36bcf8c44d5e74049d3d6e109362a844be2796b26330653`
- DUAL2 `c4c31cabf4b8e121591c4c1e0923aee67523eaf7d5b60b60ca1a545c13fc2269`
- PARALLEL4 `d316c3e5467cbfc606c788f669a3b19dba13535969dcce3b67b2821f0977a882`

WP5.2 excludes ratios, final level/mod-index UX, feedback, envelopes, voice integration, p-lock pages and audible mixed FM integration.

No physical FM-1 is available. `hardware_margin_claim=none`.
