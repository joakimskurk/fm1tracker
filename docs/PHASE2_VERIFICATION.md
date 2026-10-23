# Phase 2 Verification — tracker core closeout

Date: 2026-10-23  
Status: **COMPLETE / VERIFIED**

## Scope

Phase 2 establishes the bounded FM1Tracker tracker core on top of the verified Phase 1 Dinghy/Felucca platform baseline. It deliberately proves deterministic tracker data, editing, transport, one-lane audible playback and compact display behavior before Phase 3 p-locks or later synth engines are introduced.

Completed work packages:

- WP2.1 — compile-time V0.1 data model / D018;
- WP2.2 — one pool-backed runtime song / D019;
- WP2.3 — exact integer transport / D020;
- WP2.4 — one-track audible proof playback / D021;
- WP2.5 — four-page white-key step addressing / D022;
- WP2.6 — direct black-key track selector prototype / D023;
- WP2.7 — stopped STEP REC ordinary-note entry / D024;
- WP2.8 — REST / TIE / NOTE OFF editor and playback semantics / D025;
- WP2.9 — bounded compact tracker screen / D026;
- WP2.10 — complete integration and Phase 2 closeout.

WP2.10 adds **no new production semantics**. It exists only to prove that D018–D026 compose correctly end-to-end.

## Checkpoints

Phase 2 branch start / completed Phase 1 head:

`3ed4b65d7cda15c5831533f153f91e9014a9fde7`

WP2.10 start head:

`47e9093ae300043b804f85ce62fee1fe758fc943`

Final verified WP2.10 implementation/workflow checkpoint:

`7063be51e53c2657178084b3bef5ace813dafcd7`

GitHub Actions:

- run: `40015242947`
- job: `120625519388` (`phase2-closeout`)
- result: **SUCCESS**

The first full closeout attempt, run `40015079910`, stopped only because the new workflow initially omitted the Pillow dependency used by the already-frozen WP2.9 host screenshot helper. All tests through the WP2.9 raw framebuffer hash checks had passed. The environment omission was corrected without changing firmware, test expectations or reviewed framebuffer hashes; the final run above is authoritative.

## WP2.10 scope integrity

The final gate proves:

```text
WP2_10_PRODUCTION_SOURCE_UNCHANGED=1
```

`firmware/`, `tools/` and `build.sh` are byte-identical to the WP2.10 start head. Therefore WP2.10 closes Phase 2 entirely through tests, integration fixtures and evidence.

The final device artifacts are also byte-identical to the verified WP2.9 artifacts:

- `fm1tracker.bin` SHA-256: `34d0e8c4344e091704608e08357964e7ae0b18673dd0b93f36c4e5bcc2cb4c7c`
- `fm1tracker-0.1.fwsc` SHA-256: `3cfd0a2da3836720d2f921b04f29e93c16ca83ab18ab7242c99f2c333d890ca7`

Final closeout marker:

```text
WP2_10_PHASE2_CLOSEOUT_GATE_OK=1
```

## Direct end-to-end integration proof

`tests/test_phase2_integration.c` links the real Phase 2 modules together without test-only firmware hooks:

- store;
- transport;
- playback;
- pattern input;
- STEP REC;
- tracker-screen presentation.

Verified output:

```text
PHASE2_INTEGRATION_TESTS_OK=1
visited_steps=64 tracks=8 edits_track=7 wrap=63_to_0 page_cross=15_to_16 boundaries_120=64 loop_frames=352800 final_cursor=20 final_page=1 final_track=7
store_bytes=65896 final_events=note,empty,tie,off proof_lane=track0_unchanged screen_window=16_23
```

### Complete 64-step navigation

The test enters D022 pattern context and addresses all four pages using the exact frozen sixteen-white-key mapping. Every global step `0..63` is visited exactly once with no duplicate or missing address. The song remains byte-identical during navigation.

### Complete eight-track selector pass

All eight D023 black-key track selectors are exercised and map bijectively to logical tracks `0..7`. Track selection does not move the cursor or mutate song data.

The integration edits intentionally target logical track 7 / UI track 8 so the fixed D021 proof playback lane on logical track 0 remains untouched.

### Exact editor boundary behavior

The integration sequence proves both important automatic cursor boundaries:

- step `63 -> 0` wrap;
- step `15 -> 16` page crossing.

Final editor state is exact:

- selected logical track: 7 / UI `T8`;
- page: 1 internally / `PG2/4`;
- local step: 4;
- global cursor: 20 / UI `CUR21`.

### Exact integrated edit pattern

Relative to the seeded proof-song baseline, only the expected track-7 note fields are allowed to differ:

- step 63 = MIDI 53;
- step 0 = MIDI 55;
- step 15 = MIDI 60;
- step 16 = `FMT_NOTE_EMPTY` / REST;
- step 17 = `FMT_NOTE_TIE`;
- step 18 = `FMT_NOTE_OFF`;
- step 19 = MIDI 64.

The test constructs an expected copy of the complete `FmtSong` and compares all **65,896 bytes** against the real store. This proves track/row isolation and preservation of every non-note field and every unrelated song byte.

The fixed track-0 proof lane remains unchanged.

## Tracker-screen integration proof

The final stopped STEP REC presentation resolves to the cursor-owned D026 window steps 16..23 and verifies:

- track 8 / `T8`;
- `PG2/4`;
- selected row 21;
- REST `---`;
- TIE `===`;
- NOTE OFF `OFF`;
- MIDI 64 as `E-4`;
- STEP REC footer `FX RST  SEL TIE  FX+SEL OFF`.

Screen synchronization is proven read-only by full-song comparison before and after presentation sync.

## Full-app panel integration proof

A separate host scenario runs the unchanged real `firmware/src/dinghy.c` main loop and uses only the existing FM-1 panel script surface.

It:

- enters SEQ from PERFORMANCE;
- traverses all four pages and all 64 step selectors;
- selects UI track 8 through the D023 black key;
- stops the auto-started proof transport;
- enters STEP REC after the real audio-side stopped acknowledgement;
- performs ordinary note writes, REST, TIE and OFF;
- exercises `63 -> 0` and `15 -> 16` through actual main-loop input handling;
- exits STEP REC through PLAY and resumes playback;
- captures the integrated tracker screen;
- returns HOME and proves clean screen ownership.

Ordinary SEQ traversal emits no audition MIDI. Exactly four intended STEP REC note gestures produce four note-on/note-off pairs:

```text
53, 55, 60, 64
```

Verified markers:

```text
WP2_10_HOME_RETURN_PIXEL_IDENTICAL=1
WP2_10_REVIEWED_INTEGRATION_SCREEN_OK=1
WP2_10_FULL_APP_HOST_OK=1 midi_note_pairs=4 traversed_steps=64 selected_track=8
```

The actual 240×240 integration framebuffer was manually inspected before its raw PPM hash was frozen.

Reviewed integration tracker framebuffer SHA-256:

`a993551ce922d90186080bf554b82477b4336e160a6b301c446f0d275569b6d7`

HOME-equivalent framebuffer SHA-256:

`ea33024427340416c17a73ecfb2303e37262b8c61e2f2d30ca06265ba6e0b899`

The HOME result is byte-identical to a control run that never enters tracker context but has the same legitimate final manual-note state.

## Transport and playback proof

The integrated proof starts the unchanged D021 pattern-0 / logical-track-0 playback from step 0 and processes a complete D020 loop at 120 BPM.

Verified:

- phase threshold: `661500`;
- complete loop: exactly `352800` frames;
- row boundaries: exactly 64;
- ordinary proof-note boundaries: 16;
- transport returns to step 0, phase 0;
- no cumulative timing drift;
- track-7 edits do not alter track-0 playback event selection.

The frozen default host audio remains exactly:

- frames: `286464`;
- SHA-256: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`.

## Complete inherited regression evidence

The final closeout run keeps every prior Phase 2 gate green:

```text
FMT_MODEL_TESTS_OK=1
FMT_STORE_TESTS_OK=1
FMT_TRANSPORT_TESTS_OK=1
FMT_PLAYBACK_TESTS_OK=1
FMT_PATTERN_INPUT_TESTS_OK=1
FMT_STEP_RECORD_TESTS_OK=1
FMT_TRACKER_SCREEN_TESTS_OK=1
WP2_5_PATTERN_CONTEXT_HOST_OK=1
WP2_6_TRACK_CONTEXT_HOST_OK=1
WP2_7_STEP_REC_CONTEXT_HOST_OK=1
WP2_8_OFF_AUDIO_BEHAVIOR_OK=1 before=4410 quiet=0 after=4410
WP2_8_SPECIAL_EVENTS_CONTEXT_HOST_OK=1
WP2_9_HOME_RETURN_PIXEL_IDENTICAL=1
WP2_9_REVIEWED_SCREEN_HASHES_OK=1
WP2_9_TRACKER_SCREEN_HOST_OK=1
WP2_10_HOME_RETURN_PIXEL_IDENTICAL=1
WP2_10_REVIEWED_INTEGRATION_SCREEN_OK=1
WP2_10_FULL_APP_HOST_OK=1 midi_note_pairs=4 traversed_steps=64 selected_track=8
WP2_10_FROZEN_AUDIO_OK=1
WP2_10_PHASE2_CLOSEOUT_BUDGET_OK=1
WP2_10_PHASE2_CLOSEOUT_GATE_OK=1
```

## Device build and safety evidence

Pinned external environment remains unchanged:

- JieLi toolchain: `jieli-linux-toolchains-20260730.1`;
- clang: `4.0.1`;
- AC79 SDK tag: `AC79NN_SDK_V1.2.1_2023-12-13`;
- AC79 SDK commit: `d179b4484759423312073f5fbb232501aa491047`;
- all consumed SDK input hashes remain frozen and verified.

Device safety checks:

- loader image: 8,232 B at `0x01C0A800`;
- OTA image: 6,954 B;
- `.ram_text`: 901 instructions, no calls;
- direct register access remains HAL-only;
- package identity: `FM-1_987`.

## Final linked runtime layout

```text
01c0b070 g .bss  00000003 fmt_step_record_runtime
01c0b073 g .bss  00000004 fmt_pattern_input_runtime
01c0b078 g .bss  00000008 fmt_transport_runtime
01c0b080 g .bss  0000002c fmt_tracker_screen_runtime
01c0b0ac g .bss  00000001 fmt_playback_runtime
01c20000 g .pool 00010168 fmt_store_storage
```

## Final Phase 2 resource footprint

| Resource | Phase 1 baseline | Phase 2 closeout | Total Phase 2 delta | Limit | Remaining |
|---|---:|---:|---:|---:|---:|
| app image | 55,088 B | 59,252 B | **+4,164 B** | 581,564 B | 522,312 B |
| normal RAM | 12,368 B | 12,464 B | **+96 B** | 98,304 B | 85,840 B |
| pool RAM | 30,720 B | 96,616 B | **+65,896 B** | 344,064 B | 247,448 B |

The pool delta is exactly the one runtime `FmtSong` introduced by D019. WP2.10 itself adds zero production image, RAM or pool cost relative to WP2.9.

## Final artifacts

`fm1tracker.bin` SHA-256:

`34d0e8c4344e091704608e08357964e7ae0b18673dd0b93f36c4e5bcc2cb4c7c`

`fm1tracker-0.1.fwsc` SHA-256:

`3cfd0a2da3836720d2f921b04f29e93c16ca83ab18ab7242c99f2c333d890ca7`

Host WAV SHA-256:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

Evidence artifact:

- name: `wp2-10-phase2-closeout-evidence`;
- artifact ID: `12527623388`;
- ZIP size: 1,118,421 B;
- ZIP SHA-256: `2a4cbcda3e7f1f26fe2f117062fdd72c9fb274ae0ce8aa4ccfa1a7763829bd9c`.

## Physical hardware status

No physical FM-1 is currently available. Phase 2 therefore does **not** claim physical validation of:

- install/boot behavior on a real unit;
- panel feel or key-combination ergonomics;
- actual LCD readability/smoothness;
- actual AC791N callback/main-loop execution time;
- physical audio-output behavior.

These remain explicit later hardware gates, especially Phase 7. Host behavior, deterministic framebuffer output, device linking, memory layout, update package identity and inherited safety are verified.

## Phase 2 exit result

The Phase 2 exit criteria are satisfied at the host/device-build level:

- the full 64-step address space is deterministic;
- eight-track selection is deterministic and isolated;
- stopped STEP REC writes ordinary notes plus REST/TIE/OFF exactly once;
- cursor wrap/page crossing are deterministic;
- special playback semantics are explicit and safe;
- the compact tracker view clearly exposes cursor/playhead/editor state;
- playback timing remains exact with no cumulative drift;
- complete-song bounds/isolation verification passes;
- all prior regressions remain green;
- the firmware remains within the measured resource budgets;
- WP2.10 required no production changes.

**Phase 2 is complete.**

Phase 3 may now begin with its first bounded package, WP3.1 stable parameter ID registry. No Phase 3 implementation is part of this checkpoint.