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
| Registry translation unit compiles on JieLi PI32v2 | host n/a | required | n/a | ✅ WP3.1+ |
| Concrete lock record/capacity/pool budget frozen | required | required | n/a | ✅ D028 / WP3.2 |
| `FmtLock` exact 8 B layout and offsets | required | target compile | n/a | ✅ WP3.2 |
| Global lock storage exactly 2,048 records / 16,384 B | required | required | n/a | ✅ `.pool` `0x01C30168` |
| All 8,192 step-owner mappings unique | required | compile | n/a | ✅ exhaustive WP3.2 test |
| Maximum four locks per individual step enforced | required | required | optional | ✅ fifth prevalidated ID -> STEP_FULL |
| Lock pool reset/free-list contains every record exactly once | required | compile | n/a | ✅ WP3.2 |
| Active lists strictly sorted by parameter ID | required | compile | n/a | ✅ WP3.2 |
| Duplicate parameter set replaces in place without allocation | required | compile | later | ✅ WP3.2/WP3.3 |
| Pool exhaustion fails safely and one free record recovers exactly | required | compile | n/a | ✅ WP3.2 |
| Invalid/unassigned parameter IDs rejected by lock storage | required | compile | n/a | ✅ WP3.2 |
| Cross-owner/in-range corruption rejected without neighbor mutation | required | compile | n/a | ✅ WP3.2 owner check |
| Malformed/out-of-range/unsorted/overlong chains fail without partial mutation | required | compile | n/a | ✅ WP3.2 |
| Hold-white-step target snapshots exact track/global step | required | required | required | ✅ D029/WP3.3 all 64×8 targets |
| Active p-lock hold suppresses track/page/step retargeting | required | required | required | ✅ WP3.3 full-app fixture |
| STEP REC/PERFORMANCE cannot start p-lock hold | required | compile | required | ✅ WP3.3 |
| PLAY may toggle without losing held p-lock target | required | required | required | ✅ WP3.3 reviewed full-app fixture |
| KNOB1..4 map only to four current D027 IDs | required | compile | required | ✅ D029/WP3.3 |
| Editor value domain/reference/clamp deterministic | required | compile | required | ✅ 0..255; refs 255/128/255/128 |
| Inactive/begin/release encoder deltas cannot leak into lock write | required | required | required | ✅ WP3.3 fixture and frozen screens |
| Zero delta creates no lock | required | compile | n/a | ✅ WP3.3 |
| Missing lock first edit deterministic | required | compile | required | ✅ reference + signed delta |
| Existing lock replacement is in-place and capacity-neutral | required | compile | required | ✅ WP3.3 |
| Existing current-ID value >255 rejected without reinterpretation | required | compile | n/a | ✅ editor RANGE |
| WP3.3 full-app SEQ p-lock gesture emits no audition MIDI | required | compile | required | ✅ `midi_events=0` |
| WP3.3 p-lock footer framebuffers visually reviewed/frozen | required | compile | required | ✅ `224c0c05...` / `95bbee21...` |
| P-lock editor writes storage only; playback/base params remain untouched | required | required | observable | ✅ WP3.3/WP3.4 frozen WAV unchanged |
| WP3.3 `FmtLockEditor` exact 8 B and adds no pool allocation | required | required | n/a | ✅ WP3.3 +8 B RAM / +0 B pool |
| WP3.3 final resource delta measured/frozen | n/a | required | n/a | ✅ +2,328 B image / +8 B RAM / +0 B pool |
| Existing-lock browse follows D028 sorted semantic order | required | compile | required | ✅ D030/WP3.4 |
| SELECT browse clamps first/last and never wraps | required | compile | required | ✅ WP3.4 including extreme deltas |
| Zero-lock browse is explicit safe no-op | required | compile | n/a | ✅ `PLOCK NO LOCKS` |
| Successful KNOB edit selects the semantic lock for browse/delete | required | compile | required | ✅ WP3.4 |
| EDIT arm is non-destructive | required | required | required | ✅ WP3.4 |
| Same-poll EDIT+SEL cannot delete | required | required | required | ✅ reviewed armed 1000-ms fixture |
| EDIT release cancels pending delete | required | required | required | ✅ WP3.4 full-app fixture |
| PLAY cancels delete arm before transport action | required | required | required | ✅ WP3.4 full-app fixture |
| Unrelated note/OCT intent cannot leave latent destructive arm | required | required | required | ✅ note fixture + D030 ordering |
| Encoder motion while delete is armed cannot move/edit target | required | required | required | ✅ drained/discarded D030 input path |
| First/middle/last/only selected lock deletion deterministic | required | compile | required | ✅ WP3.4 unit coverage |
| Successful delete restores exactly one free record | required | compile | n/a | ✅ WP3.4 free-list checks |
| User-facing one-lock delete cannot corrupt neighboring step/locks | required | required | required | ✅ D030/WP3.4 |
| RANGE current-ID lock can be identified and removed without reinterpretation | required | compile | n/a | ✅ WP3.4 |
| NOT_FOUND/BAD_ADDRESS/CORRUPT delete paths have no partial mutation | required | compile | n/a | ✅ WP3.4 |
| No clear-all panel gesture exists in WP3.4 | inspect/test | compile | required later | ✅ deliberate D030 boundary |
| WP3.4 full-app browse/delete emits no audition MIDI | required | compile | required | ✅ `midi_events=0` |
| WP3.4 armed/deleted framebuffers visually reviewed/frozen | required | compile | required | ✅ `6ce56a09...` / `f0185637...` |
| `FmtLockEditor` exact 10 B after D030 and adds no pool allocation | required | required | n/a | ✅ WP3.4, pool +0 B |
| WP3.4 final resource delta measured/frozen | n/a | required | n/a | ✅ +2,512 B image / +4 B RAM / +0 B pool |
| WP3.4 preserves all Phase 2 + WP3.1 + WP3.2 + WP3.3 regressions | required | required | later | ✅ run `40064122343` |
| D031 audible subset is exactly VOICE LEVEL / FINE PITCH / TRACK LEVEL | required | compile | observable | ✅ WP3.5; TRACK PAN storage-only |
| D031 row snapshot cache is exactly 64×32-bit / 256 B and aligned | required | required | n/a | ✅ `.bss` `0x01C0B074` |
| Audio ISR never traverses mutable D028 lock lists | inspect/test | required | n/a | ✅ one 32-bit row snapshot load at dispatch |
| Lock application to playback is deterministic and does not mutate stored base data | required | required | observable | ✅ D031/WP3.5 |
| Unlocked rows restore D031 defaults rather than leaking prior lock state | required | compile | observable | ✅ locked-audio restore fixture |
| NOTE/TIE/EMPTY/OFF compose deterministically with row-local locks | required | compile | observable | ✅ WP3.5 unit/audio proof |
| D031 structural/domain-invalid lock data falls back safely | required | compile | n/a | ✅ separate fallback flags |
| Default no-lock WAV remains byte-identical after p-lock playback integration | required | required | n/a | ✅ `453dc6b7...` |
| Locked audio proof deterministic/frozen | required | compile | observable | ✅ `8d2e478e...`, 52,736 frames |
| WP3.5 final resource delta measured/frozen | n/a | required | n/a | ✅ +496 B image / +256 B RAM / +0 B pool |
| WP3.5 preserves all Phase 2 + WP3.1–WP3.4 regressions | required | required | later | ✅ run `40069110111` |
| D032 effect command domain exactly NONE/ARP/SLIDE_UP/SLIDE_DOWN/PORTA/VIBRATO | required | compile | observable | ✅ WP3.6 |
| D032 exact six-tick D020 grid and row-boundary precedence | required | compile | required | ✅ 919/1838/2757/3675/4594 -> row 5513 at 120 BPM |
| D032 effect cache exactly 64×32-bit / 256 B aligned | required | required | n/a | ✅ `.bss` `0x01C0B074` |
| `FmtTrackerEffectState` exact 12 B at verified checkpoint | required | required | n/a | ✅ WP3.6 |
| ARP 0,X,Y,0,X,Y deterministic | required | compile | observable | ✅ frozen audio proof |
| Slide up/down persistent Q7 motion and saturation deterministic | required | compile | observable | ✅ frozen audio proof |
| Portamento target/resume/reach semantics deterministic | required | compile | observable | ✅ WP3.6 unit/audio proof |
| Vibrato speed/depth/phase continuation deterministic | required | compile | observable | ✅ frozen audio proof |
| D031 FINE composes additively without mutating D032 persistent pitch | required | compile | observable | ✅ ARP+FINE fixture |
| Invalid tracker effect command falls back to NONE byte-identically | required | compile | observable | ✅ control == invalid WAV |
| D032 playback does not mutate stored song/effect data | required | required | n/a | ✅ WP3.6 |
| Default and D031 locked WAVs remain byte-identical through WP3.6 | required | required | n/a | ✅ `453dc6b7...` / `8d2e478e...` |
| WP3.6 final resource delta measured/frozen | n/a | required | n/a | ✅ +804 B image / +268 B RAM / +0 B pool |
| WP3.6 preserves all Phase 2 + WP3.1–WP3.5 regressions | required | required | later | ✅ run `40073680548` |
| D033 command domain exactly RETRIG `0x06` / NOTE DELAY `0x07` / NOTE CUT `0x08` | required | compile | observable | ✅ WP3.7 |
| D033 row-event state exactly 4 B and no general queue | required | required | n/a | ✅ WP3.7 |
| NOTE DELAY 0..5 timing bounded on exact D032 six-tick grid | required | required | required | ✅ frame 2757 for delay 3 at 120 BPM |
| NOTE CUT 0..5 timing bounded including CUT0-before-first-sample | required | required | required | ✅ WP3.7 full-app audio proof |
| RETRIG interval/count bounded to nonzero ticks 1..5 | required | required | required | ✅ frame 1838 first divergence for retrig 2 |
| D033 values >5 are scheduling no-ops without modulo wrap | required | compile | observable | ✅ delay6/cut6/retrig6 byte-identical to control |
| Delayed NOTE consumes exact D031 row snapshot captured at dispatch | required | compile | observable | ✅ WP3.7 |
| D033 preserves frozen D032 audio hashes and historical default/D031 WAVs | required | required | n/a | ✅ run `40077809743` |
| WP3.7 final resource delta measured/frozen | n/a | required | n/a | ✅ +412 B image / +4 B RAM / +0 B pool |
| WP3.7 preserves all Phase 2 + WP3.1-WP3.6 regressions | required | required | later | ✅ run `40077809743` |
| `FmtRtEvent` exact 8 B layout / no pointers | required | target compile | n/a | ✅ WP3.8 / D034 |
| `FmtRtQueue` exact 32-entry / 268 B ordinary-RAM layout | required | required | n/a | ✅ WP3.8 |
| SPSC ownership / aligned two-word publication bounded | required | required | n/a | ✅ main producer / audio consumer |
| Sequence counters remain FIFO/depth-correct through `UINT32_MAX` wrap | required | compile | n/a | ✅ WP3.8 wrap test |
| Queue overflow policy deterministic | required | required | stress test | ✅ reject-newest; 33rd rejected without unread mutation |
| Audition overflow cannot leave a stuck manual gate | required | required | later | ✅ forced >32 NOTE ON fixture fails to silence |
| Queue recovery returns to usable NOTE ON/OFF operation | required | required | later | ✅ post-overflow recovery fixture |
| Physical-key + USB-MIDI audition enter audio only through D034 queue | required | compile | required later | ✅ WP3.8 full-app proof |
| Queue consumption itself emits no MIDI output | required | compile | later | ✅ queue_midi_neutral=1 |
| Compiler publication barrier stays behind HAL boundary | inspect/test | required | n/a | ✅ only new HAL file `fm1_barrier.h` |
| D034 preserves D024 mailbox and D033 fixed-state scheduler boundaries | inspect/test | required | n/a | ✅ WP3.8 |
| WP3.8 final resource delta measured/frozen | n/a | required | n/a | ✅ +368 B image / +272 B RAM / +0 B pool |
| WP3.8 preserves all Phase 2 + WP3.1-WP3.7 regressions | required | required | later | ✅ run `40079824265` |
| D035 tracker presentation state exactly 62 B and <= D026 64 B cap | required | required | n/a | ✅ WP3.9 |
| D035 redraw topology remains exactly 10 regions / max 2 per poll | required | compile | required later | ✅ WP3.9 |
| D035 lock summaries `--/L1..L4/P0..P4/L!/P!` deterministic | required | compile | required later | ✅ WP3.9 |
| D035 stored effect command/value mnemonics deterministic | required | compile | required later | ✅ `----/AR/SU/SD/PO/VI/RT/DL/CT/????` |
| TRACK PAN detailed feedback explicitly marks storage-only state | required | compile | required later | ✅ `STORE` suffix |
| D035 visible-row sync bounded to <=32 lock-record inspections | inspect/test | compile | n/a | ✅ 8 rows × max 4 locks |
| D035 held-target begin/end and row/effect changes dirty only bounded regions | required | compile | required later | ✅ focused D035 tests |
| D035 HOME return clears stale feedback | required | compile | required later | ✅ pixel-identical control |
| D035 reviewed active/released framebuffers frozen | required | compile | required later | ✅ `d2904153...` / `ec15c6db...` |
| WP3.9 preserves historical default and all D031-D034 audio fixtures | required | required | n/a | ✅ run `40083510351` |
| WP3.9 final resource delta measured/frozen | n/a | required | n/a | ✅ +652 B image / +16 B RAM / +0 B pool |
| D036 full 2,048-record lock-pool saturation/recovery stress | required | required | n/a | ✅ 2,048 records / 512 owners / hash `4f426945` |
| D036 D032/D033 complete timing/boundary stress | required | required | n/a | ✅ all 256 values / command; 4,096 rows / 64 wraps |
| D036 D034 repeated full/wrap/overflow-storm/recovery stress | required | required | later | ✅ 2,048 full cycles / 6,144 rejects + recovery |
| D036 combined playback + queue + editor + D035 backlog stress | required | required | later | ✅ deterministic; two overflow epochs; recovery; screen-neutral queue |
| D036 static real-time boundedness audit | inspect/test | required | n/a | ✅ fail-fast audit; no allocator/list/screen in audio; queue cap 32 |
| D036 final Phase 3 inherited regression/device-build exit gate | required | required | hardware caveat | ✅ run `40084706845`; zero production delta; physical hardware pending |

Phase 3 evidence so far:

- `WP3_1_VERIFICATION.md`
- `WP3_2_VERIFICATION.md`
- `WP3_3_VERIFICATION.md`
- `WP3_4_VERIFICATION.md`
- `WP3_5_VERIFICATION.md`
- `WP3_6_VERIFICATION.md`
- `WP3_7_VERIFICATION.md`
- `WP3_8_VERIFICATION.md`
- `WP3_9_VERIFICATION.md`
- `WP3_10_TEST_PLAN.md`
- `WP3_10_VERIFICATION.md` — final Phase 3 exit evidence

## Phase 4 8-BIT engine

| Check | Automated host | Firmware/device build | Hardware | Status |
|---|---:|---:|---:|---|
| D037 engine/voice/realtime contract frozen before oscillator source | inspect/gate | n/a | n/a | ✅ WP4.1 / #31 |
| WP4.1 production tree byte-identical to clean Phase 4 branch start | git compare | required | n/a | ✅ `d14955f2...`, delta 0 |
| No PULSE/NOISE/TRIANGLE/WAVETABLE-8 production source in WP4.1 | inspect/gate | required | n/a | ✅ WP4.1 |
| Complete inherited Phase 3 exit/stress/realtime gate reruns green | required | required | later | ✅ run `40087703654` |
| Pinned JieLi/AC79 build remains exact Phase 3 artifact identity | n/a | required | n/a | ✅ 66,960 B image / 13,296 B RAM / 113,000 B pool |
| WP4.1 firmware BIN/FWSC hashes unchanged | n/a | required | n/a | ✅ `b2fc3af1...` / `7870d11b...` |
| D037 Phase 4 resource ceilings frozen | inspect/gate | n/a | n/a | ✅ image 132,496 / RAM 21,488 / pool 129,384 maximum |
| No float/double symbols in Phase 4 DSP | inspect/build | required | n/a | ⬜ retained gate |
| No runtime 64-bit divide in Phase 4 DSP | inspect/build | required | n/a | ⬜ retained gate |
| D038 PULSE primitive/test contract frozen | required | compile | n/a | ✅ `WP4_2_PULSE_CONTRACT.md` / `247ff3c0...` |
| PULSE deterministic vectors + JieLi target compile | required | required | optional later | ✅ run `40088983995` / closeout `40089122159` |
| WP4.2 full app remains exact WP4.1 baseline while PULSE is isolated | required | required | n/a | ✅ 0 B linked delta / WAV+BIN+FWSC identical |
| D039 NOISE primitive/test contract frozen before source | required | compile | n/a | ✅ `WP4_3_NOISE_CONTRACT.md` / `9cf3fd7d...` |
| NOISE deterministic vectors + JieLi target compile | required | required | optional later | ✅ run `40093607376` / closeout `40093740514` |
| WP4.3 full app remains exact WP4.2 baseline while NOISE is isolated | required | required | n/a | ✅ 0 B linked delta / WAV+BIN+FWSC identical |
| D040 TRIANGLE primitive/test contract frozen before source | required | compile | n/a | ✅ `WP4_4_TRIANGLE_CONTRACT.md` / `79167dd9...` |
| TRIANGLE deterministic vectors + JieLi target compile | required | required | optional later | ✅ run `40094383287` / closeout `40094496353` |
| WP4.4 full app remains exact WP4.3 baseline while TRIANGLE is isolated | required | required | n/a | ✅ 0 B linked delta / WAV+BIN+FWSC identical |
| D041 WAVETABLE-8 primitive/test contract frozen before source | required | compile | n/a | ✅ `WP4_5_WAVETABLE8_CONTRACT.md` / `c543c99d...` |
| WAVETABLE-8 deterministic tables/vectors + JieLi target compile | required | required | optional later | ✅ run `40095129004` / closeout `40095198147` |
| WP4.5 full app remains exact WP4.4 baseline while WAVETABLE-8 is isolated | required | required | n/a | ✅ 0 B linked delta / WAV+BIN+FWSC identical |
| D042 common envelope primitive/test contract frozen before source | required | compile | n/a | ✅ `WP4_6_ENVELOPE_CONTRACT.md` / `983757fc...` |
| Envelope stage/time/gate/retrigger/release vectors + JieLi target compile | required | required | optional later | ✅ run `40102268841` / closeout `40102339789` |
| WP4.6 full app remains exact WP4.5 baseline while envelope is isolated | required | required | n/a | ✅ 0 B linked delta / WAV+BIN+FWSC identical |
| D043 deterministic allocator/steal contract frozen before source | required | compile | n/a | ✅ `WP4_7_ALLOCATOR_CONTRACT.md` / `1215c89a...` |
| Voice allocation/stealing deterministic | required | required | required later | ✅ run `40103284377` / closeout `40103427588`; audit `40103577801` |
| WP4.7 full app remains exact WP4.6 baseline while allocator is isolated | required | required | n/a | ✅ 0 B linked delta / WAV+BIN+FWSC identical |
| D044 engine parameter-page/p-lock publication contract frozen before source | required | compile | n/a | ✅ `WP4_8_PARAMETER_PAGE_CONTRACT.md` / D044 |
| D044 six IDs + four-engine pages + all-256 WIDTH/TABLE mappings + bounded p-lock fallback | required | required | optional later | ✅ run `40104638904` / closeout `40104723586` |
| WP4.8 full app remains exact WP4.7 baseline while parameter layer is isolated | required | required | n/a | ✅ 0 B linked delta / WAV+BIN+FWSC identical |
| D045 linked 8-BIT engine integration contract frozen before source | required | required | n/a | ✅ `WP4_9_LINKED_ENGINE_CONTRACT.md` / `fd31801b...` |
| D045 Gate 1 eight-track row events + track-aware D031/D032 publication | required | required | n/a | ✅ run `40108097887`; host `120913890803`; PI32v2 `120913891037` |
| D045 Gate 2 fixed 404 B engine state + deterministic oscillator/envelope/mix | required | required | optional later | ✅ run `40109064516`; replay `c2a8d4df`; PI32v2 object `967fba0d...` |
| D045 Gate 3 D044 linked registry/cache/page-aware p-lock editor | required | required | optional later | ✅ run `40111447360`; 96 B base bank / 2048 B cache; frozen PI32v2 objects |
| Integrated PULSE/NOISE/TRIANGLE/WAVETABLE host fixtures through real adapter | required | required | optional later | ⬜ WP4.9 |
| Integrated NOTE/TIE/OFF/DELAY/RETRIG/CUT + D031/D032/D044 precedence | required | required | optional later | ⬜ WP4.9 |
| Linked engine resources remain inside D037 ceilings | inspect/build | required | n/a | ⬜ WP4.9 |
| P-lock modulation stable | required | required | required later | ⬜ WP4.9 |
| Representative eight-track timing | benchmark | required | required later | ⬜ WP4.10 |
| Physical FM-1 install/audio/execution margin | n/a | n/a | required later | ⬜ no physical FM-1 available |

WP4.1 detailed evidence: `WP4_1_ENGINE_CONTRACT.md`, `WP4_1_VERIFICATION.md`.
WP4.2 detailed evidence: `WP4_2_PULSE_CONTRACT.md`, `WP4_2_VERIFICATION.md`.
WP4.3 detailed evidence: `WP4_3_NOISE_CONTRACT.md`, `WP4_3_VERIFICATION.md`.
WP4.4 detailed evidence: `WP4_4_TRIANGLE_CONTRACT.md`, `WP4_4_VERIFICATION.md`.
WP4.5 detailed evidence: `WP4_5_WAVETABLE8_CONTRACT.md`, `WP4_5_VERIFICATION.md`.
WP4.6 detailed evidence: `WP4_6_ENVELOPE_CONTRACT.md`, `WP4_6_VERIFICATION.md`.
WP4.7 detailed evidence: `WP4_7_ALLOCATOR_CONTRACT.md`, `WP4_7_VERIFICATION.md`.
WP4.8 detailed evidence: `WP4_8_PARAMETER_PAGE_CONTRACT.md`, `WP4_8_VERIFICATION.md`.
WP4.9 Gate 1 detailed evidence: `WP4_9_LINKED_ENGINE_CONTRACT.md`, `WP4_9_GATE1_VERIFICATION.md`.
WP4.9 Gate 2 detailed evidence: `WP4_9_LINKED_ENGINE_CONTRACT.md`, `WP4_9_GATE2_VERIFICATION.md`.
WP4.9 Gate 3 detailed evidence: `WP4_9_LINKED_ENGINE_CONTRACT.md`, `WP4_9_GATE3_VERIFICATION.md`.
WP4.8 normative contract: `WP4_8_PARAMETER_PAGE_CONTRACT.md`.

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
- p-lock hold/delete ergonomics comfortable and deliberate;
- later live PLAY/REC, metronome and CHANNEL/GLOBAL performance FX feel immediate.

## Regression rule

Once a check becomes ✅, later phases keep it passing. If a deliberate design change invalidates a previous check, record a superseding decision and replace the test rather than silently dropping coverage.
