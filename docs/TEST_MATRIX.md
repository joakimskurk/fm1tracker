# FM1Tracker Test Matrix

This file tracks what is verified and what remains required. Detailed package evidence lives in the corresponding `WP*_VERIFICATION.md` documents.

Legend:
- ✅ verified
- 🟨 partial / host-only / environment-limited
- ⬜ not yet verified
- ⛔ blocked with reason in `PROJECT_STATE.md`

## Platform / baseline

| Check | Host | Firmware build | Physical FM-1 | Status |
|---|---:|---:|---:|---|
| Pinned Dinghy provenance complete | n/a | n/a | n/a | ✅ |
| Pristine/renamed host behavior verified | required | n/a | n/a | ✅ WP1.3–WP1.5 |
| FM1Tracker package identity fixed | n/a | required | n/a | ✅ `FM-1_987` |
| Exact JieLi SDK/tag/input hashes pinned | n/a | required | n/a | ✅ SDK commit `d179b4484759423312073f5fbb232501aa491047` |
| Build safety checks | n/a | required | n/a | ✅ `.ram_text`, RAM/image limits, HAL-only register access |
| Baseline resources captured | n/a | required | n/a | ✅ 55,088 B image / 12,368 B normal RAM / 30,720 B pool |
| USB rescue/update path source preserved | inspect | inspect/build | required later | 🟨 physical path not tested |
| Actual audio callback execution time | optional | useful | required before hardware signoff | ⬜ no physical FM-1 available |
| Install/boot/audio/panel behavior | n/a | n/a | required | ⬜ no physical FM-1 available |

Evidence: `WP1_3_VERIFICATION.md`, `WP1_4_VERIFICATION.md`, `WP1_5_VERIFICATION.md`.

## Phase 2 tracker core

| Check | Host | Firmware build | Hardware | Status |
|---|---:|---:|---:|---|
| 8 tracks × 64 steps × 16 patterns frozen | required | required | n/a | ✅ WP2.1 |
| `FmtStep` = 8 B, `FmtPattern` = 4096 B | required | required | n/a | ✅ WP2.1 |
| Deterministic sentinels/defaults/bounds | required | compile | n/a | ✅ WP2.1/WP2.2 |
| One 65,896 B `FmtSong` physically linked in `.pool` | inspect/test | required | n/a | ✅ WP2.2, `0x01C20000` |
| Store reset and mutation isolation | required | compile | n/a | ✅ WP2.2 |
| Exact integer transport, no cumulative rounding drift | required | required | preferred | ✅ WP2.3 |
| Transport reset/start/stop/resume deterministic | required | required | later | ✅ WP2.3 |
| No float/runtime division/firmware 64-bit arithmetic in transport | inspect/test | required | n/a | ✅ WP2.3 |
| Step 0 dispatch before playback frame 0 | required | compile | later | ✅ WP2.4 |
| Exact sub-block row boundary timing | required | required | later | ✅ WP2.4, step 1 frame 5513 at 120 BPM |
| Complete 64-step loop exact | required | required | later | ✅ WP2.4, frame 352800 at 120 BPM |
| Default audible tracker output deterministic | required | required | n/a | ✅ frozen WAV `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440` |
| Continuous tracker audio blocks unsafe flash erase | inspect/test | compile | preferred | ✅ WP2.4+ |
| Four 16-step pages cover 0..63 exactly | required | compile | required | ✅ WP2.5 |
| 16 white physical keys map bijectively to local 0..15 | required | compile | required | ✅ WP2.5 |
| SEQ step/page navigation cannot mutate song | required | compile | n/a | ✅ WP2.5 |
| Performance note release safe across context changes | required | compile | required | ✅ WP2.5–WP2.9 regressions |
| Eight black keys map bijectively to tracks 0..7 | required | compile | required | ✅ WP2.6 |
| Reserved black keys cannot select unintended track | required | compile | required | ✅ WP2.6 |
| Track selection preserves page/step and cannot mutate song | required | compile | n/a | ✅ WP2.6 |
| STEP REC requires real stopped-transport acknowledgement | required | compile | required | ✅ WP2.7 |
| Main/editor does not race-write live transport state | inspect/test | compile | n/a | ✅ WP2.7 request/ack handoff |
| STEP REC ordinary note writes exact selected row only | required | required | required | ✅ WP2.7 |
| Ordinary note entry preserves all other row fields | required | compile | n/a | ✅ WP2.7 |
| Ordinary note entry advances once, 63 -> 0 | required | required | required | ✅ WP2.7 |
| Stored pitch matches physical audition pitch | required | compile | required | ✅ WP2.7 |
| Simultaneous downs have deterministic monophonic policy | required | compile | required | ✅ lowest physical key/pitch |
| Chord-like gesture cannot double-advance before release | required | compile | required | ✅ WP2.7 |
| STEP REC uses OCT for pitch; ordinary SEQ uses OCT for pages | required | compile | required | ✅ WP2.7 |
| REST writes `EMPTY` and advances exactly once | required | required | required | ✅ WP2.8 run `40004436981` |
| TIE editor gesture deterministic | required | required | required | ✅ D025: SELECT without pending FX |
| NOTE OFF editor gesture deterministic | required | required | required | ✅ D025: hold FX then SELECT |
| FX+SELECT cannot create a second REST on FX release | required | compile | required | ✅ WP2.8 button grammar + host fixture |
| Pending REST cancelled by REC/HOME/PLAY exit | required | compile | required | ✅ WP2.8 |
| REST/TIE/OFF mutate only selected `note` byte | required | compile | n/a | ✅ WP2.8 full-song/field isolation tests |
| REST/TIE/OFF each advance exactly once, including 63 -> 0 | required | compile | required | ✅ WP2.8 |
| Playback event remains bounded 4 B | required | required | n/a | ✅ WP2.8 |
| EMPTY -> NONE playback semantics | required | compile | later | ✅ WP2.8 |
| TIE -> explicit sustain/no retrigger semantics | required | compile | later | ✅ WP2.8 |
| OFF -> explicit release semantics | required | compile | later | ✅ WP2.8 |
| Sentinel values can never become raw pitches | required | compile | n/a | ✅ WP2.8 |
| NOTE OFF produces audible release to exact silence in host proof | required | compile | later | ✅ `before=4410 quiet=0 after=4410` |
| Historical WP2.5/WP2.6/WP2.7/WP2.8 full-app gestures still pass | required | compile | later | ✅ WP2.9 regression gate |
| Tracker display uses bounded/dirty redraw | inspect/test | required | required | ✅ WP2.9 run `40009689758`; 10 regions, max 2 per poll |
| Eight cursor-owned half-page windows are exact | required | compile | required | ✅ WP2.9 all `01-08` through `57-64` windows tested |
| Selected row and playback row visually distinct | required | compile | required | ✅ D026 `>` / `*` / `>*`, off-screen playhead remains `PLYnn` |
| Ordinary note + REST/TIE/OFF display deterministic | required | compile | required | ✅ reviewed/frozen SEQ and STEP REC framebuffers |
| Instrument/volume/effect placeholders deterministic | required | compile | n/a | ✅ WP2.9 formatting tests |
| Drawing/presentation sync cannot mutate song data | required | compile | n/a | ✅ WP2.9 full-song snapshot comparison |
| Tracker drawing remains outside audio ISR | inspect/test | required | n/a | ✅ WP2.9 sound section byte-identical to start baseline |
| SEQ -> HOME restores clean performance screen | required | compile | required | ✅ pixel-identical A/B fixture |
| WP2.9 default WAV remains byte-identical | required | required | n/a | ✅ exact frozen SHA-256 unchanged |
| WP2.9 resource delta recorded | n/a | required | n/a | ✅ +2,504 B image / +44 B RAM / +0 B pool |
| Complete computer-free 64-step entry/playback workflow | required | required | required | ⬜ WP2.10 / issue #18 |

Phase 2 detailed evidence:

- `WP2_1_VERIFICATION.md`
- `WP2_2_VERIFICATION.md`
- `WP2_3_VERIFICATION.md`
- `WP2_4_VERIFICATION.md`
- `WP2_5_VERIFICATION.md`
- `WP2_6_VERIFICATION.md`
- `WP2_7_VERIFICATION.md`
- `WP2_8_VERIFICATION.md`
- `WP2_9_SCREEN_CONTRACT.md`
- `WP2_9_VERIFICATION.md`

Physical key feel, REST/TIE/OFF ergonomics, screen readability, install/audio behavior and actual render timing remain separate hardware gates.

## Phase 3 p-locks/effects

| Check | Automated host | Hardware |
|---|---:|---:|
| Maximum four locks per step enforced | required | optional |
| Replace existing parameter lock deterministic | required | required |
| Delete lock cannot corrupt neighbors | required | required |
| Stable parameter IDs versioned/documented | required | n/a |
| Lock application does not mutate stored base instrument | required | observable |
| Queue overflow policy deterministic | required | stress test |
| Note delay/cut timing bounded | required | required |
| Retrig timing/count bounded | required | required |

## Phase 4 8-BIT engine

| Check | Host | Firmware/hardware |
|---|---:|---:|
| Pitch sanity across intended range | required | required |
| No float/double symbols | inspect/build | required |
| No runtime 64-bit divide | inspect/build | required |
| PULSE/NOISE/TRIANGLE/WAVETABLE bounds deterministic | required | required |
| Voice stealing deterministic | required | required |
| Representative eight-track timing | benchmark | required |
| P-lock modulation stable | required | required |

## Phase 5 FM4

| Check | Host | Firmware/hardware |
|---|---:|---:|
| Operator pitch/ratio correctness | required | required |
| Algorithm routing matches spec | required | required |
| Feedback/modulation bounded | stress | required |
| Voice allocation deterministic | required | required |
| Worst-case render timing inside budget | benchmark | required |

## Live performance capture core milestone

| Check | Host | Hardware |
|---|---:|---:|
| REC can arm/disarm while playback continues | required | required |
| Live notes map deterministically to tracker time | required | required |
| Track switching during playback/capture cannot misroute writes | required | required |
| Drum capture deterministic | required | required |
| Metronome locked to transport | required | required |
| CHANNEL/GLOBAL automation ownership explicit | required | required |
| Parameter/FX capture bounded, not unbounded ADC streams | required | stress |
| Reference drums+bass+poly+lead configuration meets measured budget | benchmark | required |
| Poly target measured without silently reducing 8 tracker tracks | benchmark | required |

## Phase 6 persistence

| Check | Automated | Hardware |
|---|---:|---:|
| Serializer round trip | required | optional |
| Magic/version/length/CRC validation | required | optional |
| Truncated/corrupt/future data fails safely | required | required |
| Save/load preserves reference project | required | required |
| Save path does not unexpectedly block audio | inspect | required |
| Migration fixture retained per version | required | n/a |

## Phase 7 hardware UX freeze

Physical FM-1 required:

- STEP REC understandable without repeated manual lookup;
- REST/TIE/OFF gestures fast and comfortable;
- 16-step key pages unambiguous;
- track selection avoids accidental notes/edits;
- chord behavior matches expectation;
- tracker rows readable on 240×240;
- later live PLAY/REC, metronome and CHANNEL/GLOBAL performance FX feel immediate.

## Regression rule

Once a check becomes ✅, later phases keep it passing. If a deliberate design change invalidates a previous check, record a superseding decision and replace the test rather than silently dropping coverage.