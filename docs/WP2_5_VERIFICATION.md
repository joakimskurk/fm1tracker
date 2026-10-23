# WP2.5 Verification — 16-step page input

Date: 2026-10-23  
Status: **COMPLETE / VERIFIED**

Issue: #13  
Branch: `phase2/tracker-core`

WP2.5 start head:

`55d3ce02e96d5d9fe3090cc7b1a89fd1e067aef1`

Final verified implementation/workflow commit:

`28d8ac051eb7107e745bb007e1a39014f51c1169`

GitHub Actions:

- run: `39994230797`;
- job: `120553466206` (`page-input`);
- conclusion: **SUCCESS**.

## What was verified

WP2.5 adds only bounded pattern-edit cursor/page addressing. It does not add note writing, track selection, tracker-screen rendering, p-locks, synth-engine work or persistence.

D022 was recorded before implementation and defines:

- explicit PERFORMANCE versus PATTERN/SEQ context;
- `SEQ` enters pattern context;
- `HOME` returns to performance context;
- four pages `0..3`;
- sixteen local positions `0..15`;
- global selected step `page * 16 + local`;
- clamped `OCT-` / `OCT+` page navigation;
- local position preserved during page changes;
- no pattern-data mutation from selection/navigation.

The sixteen physical white-key indexes are:

```text
0, 2, 4, 6, 7, 9, 11, 12,
14, 16, 18, 19, 21, 23, 24, 26
```

They map in physical order to local step indexes `0..15`. The remaining eleven note-key indexes are rejected as step selectors in WP2.5 and remain available for the later WP2.6 black-key track-selection prototype.

## Host functional/unit evidence

The dedicated pattern-input test reports:

```text
FMT_PATTERN_INPUT_TESTS_OK=1
state_bytes=3 pages=4 steps_per_page=16 total_steps=64 white_keys=16
```

It verifies:

- deterministic reset to PERFORMANCE, page 0, local step 0;
- selection rejected outside PATTERN context;
- all 27 physical note-key indexes classified deterministically;
- exactly sixteen white keys accepted and each local index `0..15` reached exactly once;
- all four pages × sixteen local positions cover global steps `0..63` exactly once;
- no gaps or overlaps in the 64-step address space;
- previous-page clamps at page 0;
- next-page clamps at page 3;
- page changes preserve local position;
- black/non-white keys cannot move the cursor;
- invalid indexes including `UINT32_MAX` fail safely;
- byte-for-byte `FmtSong` contents remain unchanged across the full mapping/navigation test.

## Host interaction evidence

A separate full-app host gesture fixture verifies the context split rather than testing only the helper module.

Expected/observed MIDI output:

```text
100 ms  MIDI out 90 53 100
150 ms  MIDI out 80 53 0
600 ms  MIDI out 90 53 100
700 ms  MIDI out 80 53 0
WP2_5_PATTERN_CONTEXT_HOST_OK=1
```

The white key pressed/released at 250/300 ms in SEQ context and the black key pressed/released at 350/400 ms produce no MIDI audition output. The second performance note begins before SEQ is entered and is released after the context switch; the NOTE OFF still occurs at 700 ms, proving the mode change does not strand a manual voice.

## Frozen regression evidence

The following earlier suites remain green:

```text
FMT_MODEL_TESTS_OK=1
FmtStep=8 FmtPattern=4096 FmtSong=65896 lock_budget=16384 total_budgeted=82280

FMT_STORE_TESTS_OK=1
store_bytes=65896 steps=8192 patterns=16 tracks=8 steps_per_pattern=64

FMT_TRANSPORT_TESTS_OK=1
phase_per_step=661500 default_bpm=120 state_bytes=8 long_run_hours=6 max_bpm=65535

FMT_PLAYBACK_TESTS_OK=1
pattern=0 track=0 proof_notes=16 spacing=4 loop_frames_120=352800 state_bytes=1 event_bytes=4
```

The default host WAV remains exactly the frozen WP2.4 audible output:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

Therefore the new input/context layer did not move one sample in the verified one-track playback path.

## Device build and safety

Pinned JieLi toolchain:

`jieli-linux-toolchains-20260730.1`

Compiler:

`clang 4.0.1`

External official AC79 SDK:

- tag: `AC79NN_SDK_V1.2.1_2023-12-13`;
- commit: `d179b4484759423312073f5fbb232501aa491047`;
- previously frozen consumed package-input SHA-256 values all verified.

Inherited device safety checks remain green:

- loader image: 8,232 B at `0x01C0A800`;
- loader `ota.bin`: 6,954 B;
- `.ram_text`: 901 instructions, no calls;
- direct register access remains HAL-only;
- package identity remains `FM-1_987`.

## Linked runtime state

```text
01c0b060 g       .bss         00000003 fmt_pattern_input_runtime
01c0b064 g       .bss         00000008 fmt_transport_runtime
01c0b06c g       .bss         00000001 fmt_playback_runtime
01c20000 g       .pool        00010168 fmt_store_storage
```

The new pattern-input state itself is exactly 3 B. Normal RAM grows by 4 B because of alignment around the existing runtime states.

## Resource delta versus WP2.4

| Resource | WP2.4 | WP2.5 | Delta |
|---|---:|---:|---:|
| app image | 55,692 B | **55,980 B** | **+288 B** |
| normal RAM (`.data + .bss`) | 12,396 B | **12,400 B** | **+4 B** |
| pool RAM | 96,616 B | **96,616 B** | **0 B** |

Limits / remaining:

- app image limit: 581,564 B; remaining **525,584 B**;
- normal RAM limit: 98,304 B; remaining **85,904 B**;
- pool limit: 344,064 B; remaining **247,448 B**.

## Final artifact hashes

- `fm1tracker.bin` SHA-256: `ed16268fb4c2599342986d8e2588e6f90bf7229ec9d0d9e2cc92ff6378aaed30`;
- `fm1tracker-0.1.fwsc` SHA-256: `7d215b4b78e3232f670a6d146b27c4ae5064d069dfdeac40df8468c8016697a2`;
- host WAV SHA-256: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`.

Actions evidence artifact:

- name: `wp2-5-page-input-evidence`;
- artifact ID: `12518902971`;
- size: 1,107,783 B;
- artifact ZIP SHA-256: `9c670e7932893cc5f702f752b11bcad7e136ab76782a19e5d3797a886ac34d2d`.

## Scope confirmation

WP2.5 did **not** implement:

- black-key track selection;
- STEP REC or note writes;
- REST/TIE/NOTE-OFF editing;
- tracker grid/screen;
- p-locks/effects;
- multitrack playback expansion;
- a new synth engine;
- song/order mode or persistence;
- HAL/loader/update redesign.

## Hardware limitation

No physical FM-1 is currently available. Physical panel feel, LED ergonomics, actual install/boot behavior and real callback execution time therefore remain unverified. The provisional D022 page-navigation ergonomics must still be evaluated in the later hardware UX phase.

WP2.5 is complete at the host/unit/device-build/safety/resource-evidence level. WP2.6 may be prepared next, but was not implemented as part of this package.
