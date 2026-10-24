# FM1Tracker Test Matrix

This file is the current regression index. Exact historical vectors, hashes and package evidence remain in the corresponding `WP*_VERIFICATION.md`, contract and test-plan documents.

Legend:

- ✅ verified/frozen as stated
- 🟨 active implementation evidence pending
- ⬜ future requirement
- ⛔ physical hardware unavailable

## Permanent invariants

| Check | Status |
|---|---|
| package identity `FM-1_987` | ✅ |
| pinned JieLi/AC79 build path, SDK `d179b4484759423312073f5fbb232501aa491047` | ✅ |
| 8 logical tracks x 64 steps x 16 patterns | ✅ |
| `FmtStep` exactly 8 B | ✅ |
| exact integer transport / next-frame row-boundary ordering | ✅ |
| NOTE/REST/TIE/OFF STEP REC semantics | ✅ |
| 2,048 sparse p-locks, max four per step | ✅ |
| exact D032 six-tick tracker clock | ✅ |
| exact D034 32-entry SPSC queue | ✅ |
| no still-owned active cross-track primary stealing | ✅ |
| no float in firmware DSP | ✅ through WP5.1, required onward |
| no runtime 64-bit division in firmware DSP | ✅ through WP5.1, required onward |
| physical AC791N execution margin | ⛔ no physical FM-1 |

## Completed regression baselines

### Phase 2 tracker core

Complete. Evidence: `WP2_1_VERIFICATION.md` through `WP2_9_VERIFICATION.md`, `WP2_10_TEST_PLAN.md`, `PHASE2_VERIFICATION.md`.

### Phase 3 p-locks/effects/runtime ingress

Complete, D027-D036. Final target 66,960 B image / 13,296 B RAM / 113,000 B pool.

### Phase 4 first 8-BIT family

Complete, D037-D046.

- PULSE / NOISE / TRIANGLE / WAVETABLE-8 ✅
- common integer ADSR ✅
- deterministic eight-slot allocation ✅
- D044 IDs `0x20..0x25` ✅
- all-eight-track linked playback ✅
- representative WAV `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9` ✅
- normal WAV `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076` ✅
- target 72,504 / 19,604 / 113,000 B ✅
- aggregate run `40124536375`, job `120963022289` ✅

### WP5.0 mixed-engine routing

Complete, D047/D048. Evidence: `WP5_0_VERIFICATION.md`.

- per-instrument family + engine authoritative ✅
- song mode non-gating ✅
- 64 B identity publication / 8 B family route ✅
- deterministic cross-family routing/fail-safe behavior ✅
- D046 representative/normal WAV byte-identical ✅
- target 72,792 / 19,676 / 113,000 B ✅
- run `40129822006`, job `120978955451` ✅

### WP5.1 fixed-point FM operator

Complete, D049. Evidence: `WP5_1_PREIMPLEMENTATION_EVIDENCE.md`, `WP5_1_VERIFICATION.md`.

- 4 B uint32 phase state ✅
- signed int32 phase modulation / Q15 levels ✅
- generated 257-entry / 514 B quarter-wave ✅
- table SHA `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68` ✅
- 65,536 symmetry probes ✅
- long SHA `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66` ✅
- primitive forbidden operations absent ✅
- PI32v2 object text+rodata 692 B ✅
- full target/audio identity exact WP5.0 ✅
- run `40134823600`, host `120994003059`, target `120994002842` ✅

## WP5.2 / D050 curated FM4 algorithm routing - ACTIVE

Normative contract: `WP5_2_FM_ALGORITHM_CONTRACT.md`.  
Pre-implementation evidence: `WP5_2_PREIMPLEMENTATION_EVIDENCE.md`.

### Contract-first evidence

| Check | Status |
|---|---|
| D050 committed before production routing source | ✅ |
| production routing source absent during reference run | ✅ |
| D049 source/data unchanged | ✅ |
| algorithm count exactly 4 | ✅ |
| ID 0 STACK4 | ✅ |
| ID 1 FAN3 | ✅ |
| ID 2 DUAL2 | ✅ |
| ID 3 PARALLEL4 | ✅ |
| operator numbering OP1..OP4 = index 0..3 | ✅ |
| evaluation order OP4 -> OP3 -> OP2 -> OP1 | ✅ |
| all four operators advance exactly once per sample | ✅ |
| algorithm state exactly 16 B, no extra history | ✅ frozen |
| invalid algorithm fallback STACK4 | ✅ |
| mod-depth internal domain 0..16383 | ✅ |
| exact edge mapping `sample * depth * 4` | ✅ |
| multi-modulator accumulation modulo 2^32 | ✅ |
| carrier counts restricted to 1/2/4 | ✅ |
| carrier normalization via power-of-two toward zero | ✅ |
| no runtime graph traversal | ✅ frozen |
| router object ceiling <= 2,048 B text+rodata | ✅ frozen |
| STACK4 long SHA `25dfde1bb29de2c70305d6abafee91fd7480e69eaa71fd1a3fbde3c1e6e855cd` | ✅ |
| FAN3 long SHA `8d9ae5310369e3d5a36bcf8c44d5e74049d3d6e109362a844be2796b26330653` | ✅ |
| DUAL2 long SHA `c4c31cabf4b8e121591c4c1e0923aee67523eaf7d5b60b60ca1a545c13fc2269` | ✅ |
| PARALLEL4 long SHA `d316c3e5467cbfc606c788f669a3b19dba13535969dcce3b67b2821f0977a882` | ✅ |
| 65,536-render stress per algorithm | ✅ |
| D049 production regression | ✅ |
| WP5.0 router/representative host baseline | ✅ |
| source-absent run `40135979431`, job `120997461479` | ✅ |
| evidence artifact `12577609841`, SHA `d3465c39b17416d58c56d98c196a18ae1c4dbd6e07d2ab7cc4ca2b3992cab85` | ✅ |

### Production implementation gates

| Check | Status |
|---|---|
| `fmt_fm_algorithm.*` source/header | 🟨 implementation now allowed |
| exact state-size/reset/null behavior | 🟨 |
| algorithm sanitizer | 🟨 |
| mod-depth clamp/edge anchors | 🟨 |
| modular accumulation anchors | 🟨 |
| carrier normalization anchors | 🟨 |
| exact first-12 vectors | 🟨 |
| exact four long hashes/final phases | 🟨 |
| 65,536-render-per-algorithm stress | 🟨 |
| algorithm-switch phase continuity | 🟨 |
| no heap/float/int64/divide/modulo/unbounded traversal | 🟨 |
| pinned PI32v2 router compile | ⬜ |
| router object <= 2,048 B | ⬜ |
| D049 regression exact | ⬜ |
| full target/audio identity exact WP5.0 | ⬜ |
| `WP5_2_VERIFICATION.md` | ⬜ |

No WP5.3+ ratio, final level mapping, feedback, envelope, voice integration, parameter-page or audible mixed FM feature is allowed in WP5.2.

## Remaining Phase 5

WP5.3 ratios, WP5.4 levels/mod index, WP5.5 feedback, WP5.6 envelope, WP5.7 FM voice/router integration, WP5.8 FM pages/p-locks, WP5.9 mixed validation, WP5.10 worst-case Phase 5 exit.

## Later mandatory core

Live performance capture, drums, transport-locked metronome, CHANNEL/GLOBAL performance FX/automation, song/order/persistence and physical UX remain planned core work.

No physical FM-1 is available. `hardware_margin_claim=none`.
