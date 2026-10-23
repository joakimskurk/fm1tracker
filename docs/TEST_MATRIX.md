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
| Exact integer transport, no cumulative rounding drift | required | required | preferred | ✅ WP2.3/WP2.10 |
| Transport reset/start/stop/resume deterministic | required | required | later | ✅ WP2.3 |
| No float/runtime division/firmware 64-bit arithmetic in transport | inspect/test | required | n/a | ✅ WP2.3 |
| Step 0 dispatch before playback frame 0 | required | compile | later | ✅ WP2.4 |
| Exact sub-block row boundary timing | required | required | later | ✅ WP2.4, step 1 frame 5513 at 120 BPM |
| Complete 64-step loop exact | required | required | later | ✅ WP2.4/WP2.10, frame 352800 at 120 BPM |
| Default audible tracker output deterministic | required | required | n/a | ✅ frozen WAV `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440` |
| Continuous tracker audio blocks unsafe flash erase | inspect/test | compile | preferred | ✅ WP2.4+ |
| Four 16-step pages cover 0..63 exactly | required | compile | required | ✅ WP2.5 |
| 16 white physical keys map bijectively to local 0..15 | required | compile | required | ✅ WP2.5 |
| SEQ step/page navigation cannot mutate song | required | compile | n/a | ✅ WP2.5 |
| Performance note release safe across context changes | required | compile | required | ✅ WP2.5–WP2.10 regressions |
| Eight black keys map bijectively to tracks 0..7 | required | compile | required | ✅ WP2.6 |
| Reserved black keys cannot select unintended track | required | compile | required | ✅ WP2.6 |
| Track selection preserves page/step and cannot mutate song | required | compile | n/a | ✅ WP2.6 |
| STEP REC requires real stopped-transport acknowledgement | required | compile | required | ✅ WP2.7/WP2.10 |
| Main/editor does not race-write live transport state | inspect/test | compile | n/a | ✅ WP2.7 request/ack handoff |
| STEP REC ordinary note writes exact selected row only | required | required | required | ✅ WP2.7 |
| Ordinary note entry preserves all other row fields | required | compile | n/a | ✅ WP2.7 |
| Ordinary note entry advances once, 63 -> 0 | required | required | required | ✅ WP2.7/WP2.10 |
| Stored pitch matches physical audition pitch | required | compile | required | ✅ WP2.7 |
| Simultaneous downs have deterministic monophonic policy | required | compile | required | ✅ lowest physical key/pitch |
| Chord-like gesture cannot double-advance before release | required | compile | required | ✅ WP2.7 |
| STEP REC uses OCT for pitch; ordinary SEQ uses OCT for pages | required | compile | required | ✅ WP2.7 |
| REST writes `EMPTY` and advances exactly once | required | required | required | ✅ WP2.8 |
| TIE editor gesture deterministic | required | required | required | ✅ D025: SELECT without pending FX |
| NOTE OFF editor gesture deterministic | required | required | required | ✅ D025: hold FX then SELECT |
| FX+SELECT cannot create a second REST on FX release | required | compile | required | ✅ WP2.8 button grammar + host fixture |
| Pending REST cancelled by REC/HOME/PLAY exit | required | compile | required | ✅ WP2.8 |
| REST/TIE/OFF mutate only selected `note` byte | required | compile | n/a | ✅ WP2.8 |
| REST/TIE/OFF each advance exactly once, including 63 -> 0 | required | compile | required | ✅ WP2.8 |
| Playback event remains bounded 4 B | required | required | n/a | ✅ WP2.8 |
| EMPTY -> NONE playback semantics | required | compile | later | ✅ WP2.8 |
| TIE -> explicit sustain/no retrigger semantics | required | compile | later | ✅ WP2.8 |
| OFF -> explicit release semantics | required | compile | later | ✅ WP2.8 |
| Sentinel values can never become raw pitches | required | compile | n/a | ✅ WP2.8 |
| NOTE OFF produces audible release to exact silence in host proof | required | compile | later | ✅ `before=4410 quiet=0 after=4410` |
| Historical WP2.5–WP2.9 full-app regressions remain green | required | compile | later | ✅ WP2.10 closeout run `40015242947` |
| Tracker display uses bounded/dirty redraw | inspect/test | required | required | ✅ WP2.9; 10 regions, max 2 per poll |
| Eight cursor-owned half-page windows are exact | required | compile | required | ✅ WP2.9 |
| Selected row and playback row visually distinct | required | compile | required | ✅ D026 `>` / `*` / `>*` |
| Ordinary note + REST/TIE/OFF display deterministic | required | compile | required | ✅ reviewed/frozen framebuffers |
| Instrument/volume/effect placeholders deterministic | required | compile | n/a | ✅ WP2.9 formatting tests |
| Drawing/presentation sync cannot mutate song data | required | compile | n/a | ✅ WP2.9/WP2.10 full-song comparison |
| Tracker drawing remains outside audio ISR | inspect/test | required | n/a | ✅ WP2.9 |
| SEQ -> HOME restores clean performance screen | required | compile | required | ✅ WP2.9 and full WP2.10 integration A/B |
| All 64 editor addresses traversed exactly once in one integration scenario | required | compile | required | ✅ WP2.10 |
| All eight track selectors compose with editor state without mutation | required | compile | required | ✅ WP2.10 |
| Integrated automatic cursor boundaries 63->0 and 15->16 exact | required | compile | required | ✅ WP2.10 |
| Final integrated 65,896-byte song matches expected copy byte-for-byte | required | compile | n/a | ✅ WP2.10 |
| Track-7 integration edits leave track-0 proof lane unchanged | required | compile | n/a | ✅ WP2.10 |
| Full-app integrated panel scenario leaks no SEQ audition MIDI | required | compile | required | ✅ exactly four intended STEP REC note pairs |
| Integrated tracker framebuffer reviewed/frozen | required | compile | required | ✅ `a993551ce922d90186080bf554b82477b4336e160a6b301c446f0d275569b6d7` |
| Complete computer-free Phase 2 workflow composes end-to-end | required | required | required | ✅ host/device-build WP2.10; physical feel remains Phase 7 |
| WP2.10 production source unchanged from WP2.9 | inspect | required | n/a | ✅ byte-identical `firmware/`, `tools/`, `build.sh` |
| WP2.10 device artifacts identical to WP2.9 | n/a | required | n/a | ✅ exact BIN/FWSC hashes |
| Final Phase 2 resource delta recorded | n/a | required | n/a | ✅ +4,164 B image / +96 B RAM / +65,896 B pool vs Phase 1 |

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
- `WP2_10_TEST_PLAN.md`
- `PHASE2_VERIFICATION.md`

Phase 2 is complete at the host/device-build level. Physical key feel, REST/TIE/OFF ergonomics, tracker readability/smoothness, install/audio behavior and measured AC791N render/main-loop timing remain separate hardware gates.

## Phase 3 p-locks/effects

| Check | Automated host | Firmware/device build | Hardware | Status |
|---|---:|---:|---:|---|
| `FmtParamId` exactly one byte | required | target compile | n/a | ✅ WP3.1 / D027 |
| NONE `0x00` and INVALID `0xFF` never assigned/lockable | required | target compile | n/a | ✅ WP3.1 |
| Parameter namespaces exact/non-overlapping | required | target compile | n/a | ✅ all 256 byte values classified |
| Namespace membership does not imply assignment | required | target compile | n/a | ✅ WP3.1 |
| Assigned parameter IDs unique and deterministic | required | target compile | n/a | ✅ exactly 4 initial IDs |
| GLOBAL/master namespace remains reserved/unassigned | required | target compile | n/a | ✅ `0xE0..0xEF` |
| Tracker effect-command IDs remain separate from parameter IDs | inspect/test | compile | n/a | ✅ D027 |
| Registry translation unit compiles on JieLi PI32v2 | host n/a | required | n/a | ✅ WP3.1/WP3.2 |
| WP3.1 preserves all Phase 2 unit/full-app regressions | required | required | later | ✅ run `40025790828` |
| WP3.1 default WAV remains byte-identical | required | required | n/a | ✅ frozen Phase 2 SHA-256 |
| WP3.1 runtime cost is zero | n/a | required | n/a | ✅ +0 B image / +0 B RAM / +0 B pool |
| Concrete lock record/capacity/pool budget frozen | required | required | n/a | ✅ D028 / WP3.2 |
| `FmtLock` exact 8 B layout and offsets | required | target compile | n/a | ✅ WP3.2 |
| Global lock storage exactly 2,048 records / 16,384 B | required | required | n/a | ✅ `.pool` `0x01C30168` |
| Allocator runtime exactly 4 B | required | required | n/a | ✅ `.bss` `0x01C0B080` |
| All 8,192 step-owner mappings unique | required | compile | n/a | ✅ exhaustive WP3.2 test |
| Maximum four locks per individual step enforced | required | required | optional | ✅ fifth prevalidated ID -> STEP_FULL |
| Lock pool reset/free-list contains every record exactly once | required | compile | n/a | ✅ WP3.2 |
| Active lists strictly sorted by parameter ID | required | compile | n/a | ✅ WP3.2 |
| Duplicate parameter set replaces in place without allocation | required | compile | later | ✅ WP3.2 storage API |
| Pool exhaustion fails safely and one free record recovers exactly | required | compile | n/a | ✅ WP3.2 |
| Invalid/unassigned parameter IDs rejected by lock storage | required | compile | n/a | ✅ WP3.2 |
| Cross-owner/in-range corruption rejected without neighbor mutation | required | compile | n/a | ✅ WP3.2 owner check |
| Malformed/out-of-range/unsorted/overlong chains fail without partial mutation | required | compile | n/a | ✅ WP3.2 |
| WP3.2 preserves all Phase 2 + WP3.1 regressions | required | required | later | ✅ run `40038453521` |
| WP3.2 default WAV remains byte-identical | required | required | n/a | ✅ `453dc6...3440` |
| WP3.2 final resource delta measured/frozen | n/a | required | n/a | ✅ +136 B image / +4 B RAM / +16,384 B pool |
| Hold-step + encoder target isolation deterministic | required | required | required | ⬜ WP3.3 |
| Encoder-to-parameter/value mapping documented and deterministic | required | required | required | ⬜ WP3.3 |
| User-facing delete/clear cannot corrupt neighbors | required | required | required | ⬜ WP3.4 |
| Lock application does not mutate stored base instrument | required | required | observable | ⬜ WP3.5+ |
| Queue overflow policy deterministic | required | required | stress test | ⬜ WP3.8 |
| Note delay/cut timing bounded | required | required | required | ⬜ WP3.7 |
| Retrig timing/count bounded | required | required | required | ⬜ WP3.7 |

Phase 3 evidence so far:

- `WP3_1_VERIFICATION.md`
- `WP3_2_VERIFICATION.md`

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