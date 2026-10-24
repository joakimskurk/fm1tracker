# FM1Tracker Test Matrix

This file is the **current regression index**. Exact historical vectors, hashes, counts and package-specific evidence remain in the corresponding `WP*_VERIFICATION.md`, contract and test-plan documents. Historical evidence is not deleted or rewritten by this compact current matrix.

Legend:

- ✅ verified
- ⬜ active/future requirement
- ⛔ blocked by unavailable physical hardware

## Permanent platform / safety invariants

| Check | Status | Evidence / rule |
|---|---|---|
| Package identity `FM-1_987` | ✅ | Phase 1 onward |
| Pinned JieLi/AC79 build path | ✅ | SDK `d179b4484759423312073f5fbb232501aa491047` |
| HAL/build safety baseline | ✅ | Phase 1 verification |
| No float in firmware DSP | ✅ through WP5.0 | structural audits |
| No runtime 64-bit division in firmware DSP | ✅ through WP5.0 | structural audits |
| No unbounded heap/plugin mechanism in realtime engine path | ✅ through WP5.0 | D048 audit |
| Actual physical AC791N callback margin | ⛔ | no physical FM-1 |
| Physical install/boot/audio/panel/screen behavior | ⛔ | no physical FM-1 |

## Phase 2 - tracker core - COMPLETE

| Invariant | Status |
|---|---|
| 8 tracks x 64 steps x 16 patterns | ✅ |
| `FmtStep` exactly 8 B | ✅ |
| one 65,896 B pool-backed `FmtSong` | ✅ |
| exact integer transport / no cumulative drift | ✅ |
| next-frame row-boundary ordering | ✅ |
| NOTE / REST / TIE / OFF semantics | ✅ |
| stopped STEP REC | ✅ |
| four 16-step pages cover all 64 rows | ✅ |
| eight track selectors | ✅ provisional hardware mapping |
| bounded tracker screen / main-loop-only drawing | ✅ |

Evidence: `WP2_1_VERIFICATION.md` through `WP2_9_VERIFICATION.md`, `WP2_10_TEST_PLAN.md`, `PHASE2_VERIFICATION.md`.

## Phase 3 - p-locks / effects / realtime ingress - COMPLETE

| Invariant | Status |
|---|---|
| stable one-byte semantic parameter IDs | ✅ |
| tracker effect IDs separate from parameter IDs | ✅ |
| 2,048-record sparse global lock pool | ✅ |
| max four locks per step | ✅ |
| corruption-safe lock traversal/editor behavior | ✅ |
| row-local D031 snapshots | ✅ |
| exact six-tick D032 clock | ✅ |
| ARP/SLIDE/PORTA/VIBRATO deterministic | ✅ |
| D033 RETRIG / NOTE DELAY / NOTE CUT bounded | ✅ |
| exact 32-entry SPSC queue | ✅ |
| reject-newest overflow and O(1) fail-to-silence recovery | ✅ |
| bounded p-lock/effect visual feedback | ✅ |
| full saturation, timing and long-run stress | ✅ |

Final Phase 3 target: 66,960 B image / 13,296 B RAM / 113,000 B pool.

Evidence: `WP3_1_VERIFICATION.md` through `WP3_10_VERIFICATION.md` and D027-D036 contracts/test plans.

## Phase 4 - first engine family: 8-BIT/chip - COMPLETE

| Invariant | Status |
|---|---|
| PULSE deterministic primitive | ✅ |
| NOISE deterministic primitive | ✅ |
| TRIANGLE deterministic primitive | ✅ |
| WAVETABLE-8 deterministic primitive | ✅ |
| common integer ADSR | ✅ |
| deterministic eight-slot allocator | ✅ |
| no stealing of still-owned active cross-track primary | ✅ |
| D044 engine pages and IDs `0x20..0x25` | ✅ |
| linked all-eight-track playback | ✅ |
| NOTE/TIE/OFF/DELAY/RETRIG/CUT integration | ✅ |
| complete 2,048-lock / 512-owner representative workload | ✅ |
| 4,096-row / 64-wrap repeat-identical long run | ✅ |
| D034 concurrent pressure/recovery while tracker runs | ✅ |
| structural realtime audit | ✅ |
| inherited regressions | ✅ |
| representative WAV `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9` | ✅ |
| normal linked WAV `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076` | ✅ |
| target 72,504 B image / 19,604 B RAM / 113,000 B pool | ✅ |
| Phase 4 aggregate exit | ✅ run `40124536375`, job `120963022289` |

Evidence: `WP4_1_VERIFICATION.md` through `WP4_10_VERIFICATION.md`.

## Phase 5 / WP5.0 - mixed-engine routing - COMPLETE

| Check | Status | Evidence / result |
|---|---|---|
| D047 architecture frozen before production migration | ✅ | per-instrument family + engine |
| D048 routing contract frozen before production migration | ✅ | `WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md` |
| pre-migration project-wide gate reproduced | ✅ | run `40126003645` |
| global router state exactly 8 B | ✅ | one family byte x 8 tracks |
| instrument publication exactly 64 B | ✅ | 16 x 4 B headers |
| `FmtSongMeta.mode` non-gating | ✅ | actual-app mode8/mode16 WAV byte-identical |
| family identity coexistence across tracks | ✅ | router fixture |
| same-track cross-family NOTE ordering | ✅ | hard-silence/clear/attempt/commit |
| delayed NOTE uses same router | ✅ | focused fixture |
| unknown family fail-safe | ✅ | silence |
| invalid engine fail-safe | ✅ | silence |
| non-applicable p-lock behavior | ✅ | ignored, never reinterpreted |
| one owned primary family route per logical track | ✅ | fixed router semantics |
| active cross-track primary steals | ✅ | exactly 0 |
| static/bounded dispatch, no dynamic registry | ✅ | realtime audit |
| one shared 2,048 B row-page cache | ✅ | no duplicate family cache |
| D046 representative WAV byte-identical | ✅ | `33e819237fb0...` |
| D046 normal WAV byte-identical | ✅ | `17fa5ce1759c...` |
| D046 representative/full-pool/long-run workload | ✅ | migrated production source |
| applicable Phase 1-4 regressions | ✅ | inherited suite |
| structural D048 realtime audit | ✅ | bounded O(1) family access |
| no FM4 production DSP in WP5.0 | ✅ | source guards |
| target resource budget | ✅ | 72,792 B image / 19,676 B RAM / 113,000 B pool |
| incremental D048 cost | ✅ | +288 / +72 / +0 B |
| final BIN | ✅ | `a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125` |
| final FWSC | ✅ | `cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073` |
| WP5.0 aggregate exit | ✅ | run `40129822006`, job `120978955451` |
| physical callback margin | ⛔ | `hardware_margin_claim=none` |

Evidence: `WP5_0_PREMIGRATION_EVIDENCE.md`, `WP5_0_VERIFICATION.md`.

## Phase 5 / WP5.1 - fixed-point FM operator primitive - ACTIVE

| Check | Status |
|---|---|
| D049/WP5.1 normative contract frozen before production source | ⬜ |
| existing accepted phase/pitch conventions inspected | ⬜ |
| operator state size/layout frozen | ⬜ |
| phase accumulator format and exact wrap frozen | ⬜ |
| deterministic carrier lookup representation frozen | ⬜ |
| signed modulation input format/range frozen | ⬜ |
| level/output format/range frozen | ⬜ |
| overflow/saturation behavior frozen | ⬜ |
| reset/init semantics frozen | ⬜ |
| exact deterministic host vectors | ⬜ |
| positive/negative/extreme modulation vectors | ⬜ |
| zero/max level vectors | ⬜ |
| repeated-run identity | ⬜ |
| no float/double | ⬜ |
| no heap | ⬜ |
| no runtime 64-bit division | ⬜ |
| pinned JieLi/AC79 target compile | ⬜ |
| exact image/RAM/pool delta | ⬜ |
| semantically applicable WP5.0 router regressions | ⬜ |
| D046 all-8-BIT regression remains exact | ⬜ |
| durable `WP5_1_VERIFICATION.md` | ⬜ |

WP5.1 must not implement algorithm routing, ratio policy, feedback, FM envelopes, FM voice allocation, FM parameter pages or mixed production FM playback.

## Remaining Phase 5 gates

| Package | Core verification |
|---|---|
| WP5.2 curated algorithms | exact static graph/routing fixtures |
| WP5.3 ratio table | deterministic tuning vectors |
| WP5.4 levels/mod index | bounded integer mappings |
| WP5.5 feedback | bounded deterministic stress |
| WP5.6 envelope strategy | deterministic NOTE/TIE/RETRIG/OFF |
| WP5.7 FM voice integration | bounded ownership, no cross-track primary steal |
| WP5.8 FM parameter pages | stable semantic IDs and p-lock tests |
| WP5.9 mixed 8-BIT + FM4 | simultaneous and same-track family switching |
| WP5.10 Phase 5 exit | mixed worst-case workload, realtime audit, target resources/artifacts |

## Mandatory live performance capture milestone

Future required gates include REC arm/disarm while PLAY continues, deterministic live-note mapping, safe track switching, drum capture, transport-locked metronome, explicit CHANNEL/GLOBAL automation ownership, bounded parameter/FX capture and measured polyphony without reducing eight tracker tracks.

Reference: `LIVE_PERFORMANCE_CAPTURE_VISION.md`.

## Physical hardware boundary

No physical FM-1 is available. Host/device-build evidence does not prove install/boot, physical audio quality, control feel, screen feel or measured AC791N callback execution margin.

`hardware_margin_claim=none`
