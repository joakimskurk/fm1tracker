# FM1Tracker Test Matrix

This file is the current regression index. Exact historical vectors, hashes and package evidence remain in the corresponding `WP*_VERIFICATION.md`, contract and test-plan documents.

Legend:

- ✅ verified/frozen as stated
- 🟨 committed/frozen, CI evidence pending
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
| no float in firmware DSP | ✅ through WP5.0, required for WP5.1 |
| no runtime 64-bit division in firmware DSP | ✅ through WP5.0, required for WP5.1 |
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

## WP5.1 / D049 fixed-point FM operator - ACTIVE

Normative contract: `WP5_1_FM_OPERATOR_CONTRACT.md`.

### Contract freeze

| Check | Status |
|---|---|
| D049 committed before FM operator production source | ✅ frozen |
| operator state exactly 4 B | ✅ frozen |
| uint32 phase, one cycle = 2^32, natural wrap | ✅ frozen |
| sample current phase before advance | ✅ frozen |
| signed int32 phase modulation in same phase-unit scale | ✅ frozen |
| Q15 operator level clamps 0..32767 | ✅ frozen |
| 257-entry positive quarter-wave table | ✅ frozen |
| table payload exactly 514 B | ✅ frozen |
| 22-bit fractional linear interpolation | ✅ frozen |
| output domain -32767..32767 | ✅ frozen |
| table binary SHA `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68` | ✅ frozen |
| 4,096-render SHA `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66` | ✅ frozen |
| isolated target object ceiling <= 4,096 B text+rodata | ✅ frozen |
| full app must remain exact WP5.0 identity | ✅ frozen |
| contract-first reference CI | 🟨 pending run |

### Production implementation gates

| Check | Status |
|---|---|
| generated table tool/data matches frozen hash | ⬜ |
| `FmtFmOperator` compile-time size assert | ⬜ |
| reset/retrigger/null safety | ⬜ |
| sine anchor vectors | ⬜ |
| non-node interpolation vectors | ⬜ |
| broad exact half-cycle sign symmetry | ⬜ |
| positive/negative/INT32 edge modulation vectors | ⬜ |
| level clamp and sign-mirror vectors | ⬜ |
| sample-before-advance vector | ⬜ |
| phase wrap / zero-increment behavior | ⬜ |
| frozen 4,096-render output hash/final phase | ⬜ |
| no float/double | ⬜ |
| no heap | ⬜ |
| no runtime divide/modulo in primitive render/sine | ⬜ |
| no 64-bit arithmetic in primitive | ⬜ |
| pinned JieLi target module compile | ⬜ |
| exact isolated object sections recorded | ⬜ |
| full normal WAV remains exact WP5.0 | ⬜ |
| full target resource/BIN/FWSC remains exact WP5.0 | ⬜ |
| semantically applicable WP5.0/D046 regressions remain green | ⬜ |
| `WP5_1_VERIFICATION.md` | ⬜ |

No WP5.2+ feature is allowed in this package.

## Remaining Phase 5

WP5.2 algorithms, WP5.3 ratios, WP5.4 levels/mod index, WP5.5 feedback, WP5.6 envelope, WP5.7 FM voice/router integration, WP5.8 FM pages/p-locks, WP5.9 mixed validation, WP5.10 worst-case Phase 5 exit.

## Later mandatory core

Live performance capture, drums, transport-locked metronome, CHANNEL/GLOBAL performance FX/automation, song/order/persistence and physical UX remain planned core work. See `LIVE_PERFORMANCE_CAPTURE_VISION.md` and `ROADMAP.md`.

No physical FM-1 is available. `hardware_margin_claim=none`.
