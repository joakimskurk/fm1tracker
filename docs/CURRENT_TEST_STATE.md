# FM1Tracker Current Test State

Date: 2026-10-24  
Purpose: **current verification overlay**

`TEST_MATRIX.md` is the current regression index. Detailed historical evidence remains in the corresponding `WP*_VERIFICATION.md`, contract and test-plan documents.

Legend:

- ✅ verified
- ⬜ active/future requirement
- ⛔ physical gate unavailable

## Stable completed baseline

### Phase 1-3

Foundation, tracker core, p-lock/effect/runtime ingress and their pinned closeouts are complete. See `TEST_MATRIX.md` and the package verification files.

### Phase 4 / D046

The complete all-8-BIT family is frozen as the mixed-engine regression baseline.

- PULSE / NOISE / TRIANGLE / WAVETABLE-8 ✅
- common integer ADSR ✅
- deterministic eight-slot allocation ✅
- D044 parameter pages/p-lock mappings ✅
- all-eight-track linked playback ✅
- complete 2,048-lock representative workload ✅
- 4,096-row / 64-wrap repeat identity ✅
- zero active cross-track primary steals ✅
- structural realtime audit ✅
- pinned target 72,504 B image / 19,604 B RAM / 113,000 B pool ✅
- representative WAV `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9` ✅
- normal WAV `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076` ✅
- physical FM-1 timing/UX ⛔ unavailable

Canonical evidence: `docs/WP4_10_VERIFICATION.md`, run `40124536375`, job `120963022289`.

## WP5.0 / D048 final state - COMPLETE

| Check | Status | Evidence / note |
|---|---|---|
| D047/D048 contract frozen before migration | ✅ | `D047_MIXED_ENGINE_ARCHITECTURE.md`, `WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md` |
| Historical project-wide family gate reproduced before removal | ✅ | pre-migration run `40126003645` |
| Fixed router state exactly 8 B | ✅ | one family byte per logical track |
| Instrument identity publication exactly 64 B | ✅ | 16 x 4 B headers |
| Per-instrument family + engine authoritative | ✅ | production D048 adapter |
| `FmtSongMeta.mode` does not gate valid playback | ✅ | actual-app mode8/mode16 WAV byte-identical |
| Different family identities coexist in router model | ✅ | router host fixture |
| Same-track cross-family NOTE ordering deterministic | ✅ | hard silence -> clear -> attempt -> commit on success |
| Delayed NOTE uses the same router | ✅ | focused delayed fixture |
| Unknown family fails toward silence | ✅ | focused malformed fixture |
| Invalid 8-BIT engine fails toward silence | ✅ | focused malformed fixture |
| Non-applicable 8-BIT p-lock on FM-family row | ✅ | stored, ignored, never reinterpreted |
| Active cross-track primary steals | ✅ | exactly 0 |
| Dispatch static/bounded | ✅ | no dynamic plugin registry/heap |
| Shared row-page publication cache | ✅ | exactly one 2,048 B cache retained |
| D046 representative workload regression | ✅ | exact representative WAV retained |
| D046 normal full-app WAV regression | ✅ | exact normal WAV retained |
| 4,096-row long run and queue pressure | ✅ | inherited representative gate |
| Structural realtime audit | ✅ | `WP5_0_REALTIME_AUDIT_OK=1` |
| Applicable Phase 1-4 regressions | ✅ | inherited suite green |
| No FM4 production DSP in WP5.0 | ✅ | source guards |
| Pinned JieLi/AC79 target build | ✅ | identity `FM-1_987` |
| WP5.0 target resources | ✅ | 72,792 B image / 19,676 B RAM / 113,000 B pool |
| Incremental delta versus D046 | ✅ | +288 B image / +72 B RAM / +0 B pool |
| Final BIN | ✅ | `a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125` |
| Final FWSC | ✅ | `cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073` |
| Aggregate exit | ✅ | run `40129822006`, job `120978955451` |
| Physical FM-1 callback margin / UX | ⛔ | `hardware_margin_claim=none` |

Canonical marker:

```text
WP5_0_EXIT_OK=1 representative_wav_sha256=33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9 default_wav_sha256=17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076 image=72792 ram=19676 pool=113000 bin_sha256=a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125 fwsc_sha256=cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073 inherited=pass realtime=pass family_switch=pass song_meta=nongating fm_dsp=absent hardware_margin_claim=none
```

Final evidence: `docs/WP5_0_VERIFICATION.md`.

## Active package test obligations - WP5.1 / issue #44

WP5.1 must remain primitive-only and contract-first.

| Check | Status | Requirement |
|---|---|---|
| D049/WP5.1 normative contract | ⬜ | freeze before production FM operator source |
| Existing integer phase/pitch conventions inspected | ⬜ | reuse proven patterns where appropriate |
| Operator state size frozen | ⬜ | compile-time asserted |
| Phase accumulator format frozen | ⬜ | exact wrap behavior |
| Signed modulation input format frozen | ⬜ | positive/negative/extreme vectors |
| Carrier lookup policy frozen | ⬜ | deterministic integer-only primitive |
| Operator level/output range frozen | ⬜ | zero/max/edge vectors |
| Overflow/saturation behavior frozen | ⬜ | no undefined arithmetic |
| Reset/init semantics frozen | ⬜ | repeat-identical vectors |
| Deterministic host vectors | ⬜ | exact expected outputs |
| No float/double | ⬜ | source/target guard |
| No heap | ⬜ | source guard |
| No runtime 64-bit division | ⬜ | source/target guard |
| Pinned target compile | ⬜ | JieLi/AC79 |
| Incremental image/RAM/pool delta | ⬜ | measured and accepted |
| WP5.0 router regression remains green | ⬜ | semantically applicable inherited gate |
| D046 all-8-BIT regression remains green | ⬜ | no accidental audio change |
| `WP5_1_VERIFICATION.md` | ⬜ | required before WP5.2 |

Explicitly not tested/implemented in WP5.1: algorithm graphs, ratio table, feedback, FM envelopes, FM voice allocation, FM parameter pages/p-lock IDs and mixed production FM playback.

## Physical hardware boundary

No physical FM-1 is available. Host/device-build verification cannot prove physical install/boot, sound quality, UI feel or actual AC791N callback margin.

`hardware_margin_claim=none`
