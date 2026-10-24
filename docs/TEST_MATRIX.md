# FM1Tracker Test Matrix

This file is the current regression index. Exact historical vectors, hashes and package evidence remain in the corresponding `WP*_VERIFICATION.md`, contract and test-plan documents.

Legend:

- ✅ verified/frozen
- ⬜ active/future requirement
- ⛔ physical hardware unavailable

## Permanent invariants

| Check | Status |
|---|---|
| package identity `FM-1_987` | ✅ |
| pinned JieLi/AC79 build path | ✅ |
| 8 logical tracks x 64 steps x 16 patterns | ✅ |
| `FmtStep` exactly 8 B | ✅ |
| exact integer transport / row-boundary ordering | ✅ |
| 2,048 sparse p-locks, max four per step | ✅ |
| D032 exact six-tick tracker clock | ✅ |
| D034 exact 32-entry SPSC queue | ✅ |
| no active still-owned cross-track primary stealing | ✅ |
| no float in firmware DSP | ✅ through WP5.3, required onward |
| no runtime 64-bit division in firmware DSP | ✅ through WP5.3, required onward |
| physical AC791N execution margin | ⛔ no physical FM-1 |

## Completed baselines

### D046 Phase 4 all-8-BIT

- representative WAV `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9` ✅
- normal WAV `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076` ✅
- target 72,504 / 19,604 / 113,000 B ✅
- run `40124536375`, job `120963022289` ✅

### D048 WP5.0 mixed-engine routing

- per-instrument family/engine routing ✅
- song mode non-gating ✅
- 64 B identity publication / 8 B family route ✅
- deterministic cross-family fail-safe behavior ✅
- D046 audio exact ✅
- target 72,792 / 19,676 / 113,000 B ✅
- run `40129822006`, job `120978955451` ✅

### D049 WP5.1 fixed-point FM operator

- state 4 B ✅
- uint32 phase / signed int32 phase modulation ✅
- Q15 level/output ✅
- 257-entry generated quarter-wave / 514 B ✅
- table SHA `6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68` ✅
- 65,536 symmetry probes ✅
- long SHA `499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66` ✅
- PI32v2 object 692 B ✅
- full app exact WP5.0 ✅
- run `40134823600`, host `120994003059`, target `120994002842` ✅

### D050 WP5.2 curated FM4 routing

- D050 source-absent gate before production source ✅
- four stable algorithms ✅
- OP4 -> OP3 -> OP2 -> OP1 fixed evaluation ✅
- every operator advances once/sample ✅
- state 16 B, no routing history ✅
- algorithm-switch phase continuity ✅
- mod depth / modulo phase accumulation exact ✅
- carrier normalization exact ✅
- invalid IDs -> STACK4 ✅
- four first-12 vectors exact ✅
- four 4,096-render hashes exact ✅
- 65,536 renders per algorithm stress ✅
- no loops/graph traversal/heap/float/int64/divide/modulo/store/router access in render ✅
- PI32v2 router object 362 B / 2,048 B ceiling ✅
- object SHA `ed85fe6ed70402bd3b0727e59b1c8eee21f6d15f264ec8dc439603968a38f469` ✅
- full target remains 72,792 / 19,676 / 113,000 B ✅
- BIN `a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125` ✅
- FWSC `cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073` ✅
- run `40136425976`, host `120998804031`, target `120998804175` ✅
- evidence `WP5_2_VERIFICATION.md` ✅

Frozen D050 hashes:

- STACK4 `25dfde1bb29de2c70305d6abafee91fd7480e69eaa71fd1a3fbde3c1e6e855cd`
- FAN3 `8d9ae5310369e3d5a36bcf8c44d5e74049d3d6e109362a844be2796b26330653`
- DUAL2 `c4c31cabf4b8e121591c4c1e0923aee67523eaf7d5b60b60ca1a545c13fc2269`
- PARALLEL4 `d316c3e5467cbfc606c788f669a3b19dba13535969dcce3b67b2821f0977a882`

### D051 WP5.3 integer ratio/pitch mapping

- D051 frozen before production ratio source ✅
- source-absent run `40138575267`, job `121005262360` ✅
- 16 stable Q8 ratio IDs ✅
- invalid-ID -> UNITY ✅
- canonical base max `1293741834` ✅
- output ceiling `0x7fffffff` ✅
- deterministic 32-bit split-Q8 round-half-up ✅
- all high-register first-clamp anchors exact ✅
- exhaustive 16,384 pitch x 16 ratio mapping ✅
- exhaustive payload 1,048,576 B / 262,144 outputs ✅
- intentional ceiling outputs 27,615 ✅
- exhaustive SHA `5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c` ✅
- source boundary only `fmt_fm_ratio.h/.c` ✅
- no loops/runtime div/mod/int64/float/heap/external DSP deps ✅
- D038/D049/D050 unchanged ✅
- inherited D049/D050/WP5.0 host regressions exact ✅
- PI32v2 ratio object 264 B / 1,024 B ceiling ✅
- object SHA `aa5bd8b9e1afa72054af2effeefbf468cfbb8b2b8d978e028540a30dc162956d` ✅
- full target exact 72,792 / 19,676 / 113,000 B ✅
- BIN/FWSC exact WP5.0 ✅
- canonical run `40139154497`, host `121006986068`, target `121006985928` ✅
- evidence `WP5_3_VERIFICATION.md` ✅

## WP5.4 level/modulation-index mapping - NEXT CONTRACT-FIRST

Issue #47.

| Check | Status |
|---|---|
| inspect D049 Q15 level semantics | ⬜ |
| inspect D050 mod-depth/topology semantics | ⬜ |
| choose bounded carrier/modulator control model | ⬜ |
| decide global vs per-op/per-edge modulation representation | ⬜ |
| stable internal control domains | ⬜ |
| zero/nominal/max semantics | ⬜ |
| integer mapping / rounding / clamp | ⬜ |
| all four D050 algorithm fixtures | ⬜ |
| representative D051 ratio fixtures | ⬜ |
| overflow/gain-bound proof | ⬜ |
| source/realtime/PI32v2 ceilings | ⬜ |
| D052 frozen before production source | ⬜ |
| source-absent reference CI | ⬜ |
| production mapping implementation | ⬜ blocked until D052 gate |
| inherited D049/D050/D051 regressions | ⬜ |
| full app exact WP5.0 while isolated | ⬜ |
| `WP5_4_VERIFICATION.md` | ⬜ |

D052 does not exist yet.

## Remaining Phase 5

WP5.5 feedback, WP5.6 envelope, WP5.7 FM voice/router integration, WP5.8 FM pages/p-locks, WP5.9 mixed validation, WP5.10 worst-case Phase 5 exit.

No physical FM-1 is available. `hardware_margin_claim=none`.
