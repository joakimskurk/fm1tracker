# FM1Tracker Test Matrix

This file is the current regression index. Exact historical vectors, hashes and package evidence remain in the corresponding `WP*_VERIFICATION.md`, contract and test-plan documents.

Legend:

- ✅ verified/frozen as stated
- ⬜ active/future requirement
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
- full 2,048-lock representative workload ✅
- 4,096-row / 64-wrap repeat identity ✅
- zero active cross-track primary steals ✅
- representative WAV `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9` ✅
- normal WAV `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076` ✅
- target 72,504 / 19,604 / 113,000 B ✅
- aggregate run `40124536375`, job `120963022289` ✅

Evidence: `WP4_1_VERIFICATION.md` through `WP4_10_VERIFICATION.md`.

### WP5.0 mixed-engine routing

Complete, D047/D048.

| Check | Status |
|---|---|
| per-instrument family + engine authoritative | ✅ |
| `FmtSongMeta.mode` non-gating | ✅ |
| 64 B identity publication | ✅ |
| 8 B global family route | ✅ |
| immediate/delayed same-track cross-family routing deterministic | ✅ |
| malformed/unsupported family/engine fail-safe | ✅ |
| non-applicable p-lock ignored, never reinterpreted | ✅ |
| one shared 2,048 B row cache | ✅ |
| static bounded dispatch, no dynamic plugin/heap | ✅ |
| D046 representative/normal WAV byte-identical | ✅ |
| inherited Phase 1-4 regressions | ✅ |
| structural realtime audit | ✅ |
| target 72,792 / 19,676 / 113,000 B | ✅ |
| aggregate run `40129822006`, job `120978955451` | ✅ |

Evidence: `WP5_0_VERIFICATION.md`.

## WP5.1 / D049 fixed-point FM operator - COMPLETE

Normative contract: `WP5_1_FM_OPERATOR_CONTRACT.md`.  
Evidence: `WP5_1_PREIMPLEMENTATION_EVIDENCE.md`, `WP5_1_VERIFICATION.md`.

| Check | Status |
|---|---|
| D049 committed before production FM operator source | ✅ |
| source-absent contract/reference CI | ✅ run `40131135259`, job `120982895786` |
| operator state exactly 4 B | ✅ |
| uint32 phase, 2^32 cycle, natural wrap | ✅ |
| sample current phase before advance | ✅ |
| signed int32 phase modulation | ✅ |
| Q15 level clamps 0..32767 | ✅ |
| output domain -32767..32767 | ✅ |
| generated 257-entry positive quarter-wave table | ✅ |
| table payload 514 B | ✅ |
| 22-bit fractional linear interpolation | ✅ |
| table SHA `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68` | ✅ |
| generator reproduces checked-in table exactly | ✅ |
| exact sine/interpolation vectors | ✅ |
| 65,536 half-cycle sign-symmetry probes | ✅ |
| positive/negative/INT32 modulation edges | ✅ |
| level clamp/sign mirror | ✅ |
| reset/retrigger/null safety | ✅ |
| phase wrap / zero-increment hold | ✅ |
| 4,096-render SHA `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66` | ✅ |
| final phase `f1532bdf` | ✅ |
| no loop/heap/float/int64/divide/modulo/store/router access in primitive | ✅ |
| pinned PI32v2 compile | ✅ |
| object text+rodata 692 B / 4,096 B ceiling | ✅ |
| object SHA `46868461ac6910d499975dbd93d06ee0af9db905d01c32583321eeaea0e33fad` | ✅ |
| normal WP5.0 WAV unchanged | ✅ |
| D046 representative WAV unchanged | ✅ |
| WP5.0 router regression | ✅ |
| full target remains 72,792 / 19,676 / 113,000 B | ✅ |
| BIN `a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125` | ✅ |
| FWSC `cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073` | ✅ |
| canonical run `40134823600`, host `120994003059`, target `120994002842` | ✅ |

The D049 operator remains isolated from `dinghy.c`; no audible FM production path exists yet.

## WP5.2 curated FM4 algorithm routing - ACTIVE CONTRACT-FIRST

Issue #45.

| Check | Status |
|---|---|
| inspect D049 and current FM4 constraints | ⬜ |
| curated algorithm count | ⬜ |
| stable internal algorithm IDs | ⬜ |
| exact graph topology | ⬜ |
| exact per-sample operator evaluation order | ⬜ |
| signed modulation accumulation/scaling | ⬜ |
| overflow/saturation semantics | ⬜ |
| carrier sum/output normalization | ⬜ |
| pre-feedback mutable-state requirement | ⬜ |
| deterministic graph fixtures | ⬜ |
| long-run graph hashes | ⬜ |
| realtime/source ceiling | ⬜ |
| pinned target object ceiling | ⬜ |
| D050 contract frozen before production source | ⬜ |
| source-absent pre-implementation CI | ⬜ |
| production routing implementation | ⬜ blocked until D050 gate |
| D049 operator regression remains exact | ⬜ |
| full app remains exact WP5.0 while routing is isolated | ⬜ |
| `WP5_2_VERIFICATION.md` | ⬜ |

WP5.2 excludes ratios, final level/mod-index UX, feedback, envelopes, voice integration, FM p-lock pages and audible mixed FM integration.

## Remaining Phase 5

WP5.3 ratios, WP5.4 levels/mod index, WP5.5 feedback, WP5.6 envelope, WP5.7 FM voice/router integration, WP5.8 FM pages/p-locks, WP5.9 mixed validation, WP5.10 worst-case Phase 5 exit.

## Later mandatory core

Live performance capture, drums, transport-locked metronome, CHANNEL/GLOBAL performance FX/automation, song/order/persistence and physical UX remain planned core work. See `LIVE_PERFORMANCE_CAPTURE_VISION.md` and `ROADMAP.md`.

No physical FM-1 is available. `hardware_margin_claim=none`.
