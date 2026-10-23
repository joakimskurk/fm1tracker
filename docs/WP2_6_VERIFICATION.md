# WP2.6 Verification — black-key track selection prototype

Date: 2026-10-23  
Branch: `phase2/tracker-core`  
WP2.6 start head: `0e7d1794325a25e350ffefe2783ca84738a075c5`  
Final verified implementation/workflow commit: `707d0b24eb15c19193136f68e7c9f4e1d0a7dc7d`

GitHub issue: #14  
GitHub Actions run: `39995856373`  
Job: `120559070230` (`track-selection`)  
Result: **SUCCESS**

## Scope verified

WP2.6 adds only the provisional direct track-selection layer on top of the already verified D022 pattern-input cursor. It does not add note writing, STEP REC, general multitrack playback, tracker-screen behavior, p-locks, synth-engine work or persistence.

D023 was recorded before source implementation.

## D023 mapping

The FM-1 note-key indexes span F3..G5 as `0..26`.

The first eight physical black keys are:

| Physical key index | Pitch | Logical track | UI track |
|---:|---|---:|---:|
| 1 | F#3 | 0 | 1 |
| 3 | G#3 | 1 | 2 |
| 5 | A#3 | 2 | 3 |
| 8 | C#4 | 3 | 4 |
| 10 | D#4 | 4 | 5 |
| 13 | F#4 | 5 | 6 |
| 15 | G#4 | 6 | 7 |
| 17 | A#4 | 7 | 8 |

Reserved black keys in this package:

- index 20 — C#5;
- index 22 — D#5;
- index 25 — F#5.

Those reserved keys are consumed/rejected in SEQ context and cannot select an unintended track.

## Runtime-state contract

`FmtPatternInput` is now exactly 4 bytes:

- `context`;
- `page`;
- `local_step`;
- `selected_track`.

Reset produces:

- PERFORMANCE context;
- page 0;
- local step 0;
- logical track 0 / UI track 1.

HOME/SEQ context changes preserve selected track. Track selection preserves page and local/global step. White-key step selection preserves selected track.

The input module contains no song-store access and the host suite verifies the complete `FmtSong` remains byte-identical across step/page/track navigation.

## Automated input tests

`tests/run_pattern_input_tests.sh` passes with:

```text
FMT_PATTERN_INPUT_TESTS_OK=1
state_bytes=4 pages=4 steps_per_page=16 total_steps=64 white_keys=16 tracks=8 track_keys=8 reserved_black=3
```

Verified:

- all 16 D022 white keys remain bijective local-step selectors;
- all four pages still cover global steps 0..63 exactly once;
- all eight D023 black keys map bijectively to tracks 0..7;
- the three reserved black keys cannot change track;
- out-of-range indexes including `UINT32_MAX` are rejected;
- track selector API is inactive in PERFORMANCE context;
- selected track survives HOME/SEQ transitions;
- track changes preserve page/local/global step;
- page changes preserve selected track;
- white-key step changes preserve selected track;
- navigation and track selection leave the entire `FmtSong` byte-identical.

## Host context gesture verification

WP2.5 context regression remains green:

```text
WP2_5_PATTERN_CONTEXT_HOST_OK=1
```

The new WP2.6 host gesture also passes:

```text
WP2_6_TRACK_CONTEXT_HOST_OK=1
```

The host trace proves a designated black key remains a normal performance key outside SEQ:

```text
100 ms  MIDI out 90 54 100
150 ms  MIDI out 80 54 0
```

The same physical black key is consumed as a track selector in SEQ and emits no MIDI note. A white step key and a reserved black key are likewise consumed in SEQ.

The release-safety case also passes: F#3 is started in PERFORMANCE at 600 ms, SEQ is entered while the key remains held, and release at 700 ms still emits the matching NOTE OFF:

```text
600 ms  MIDI out 90 54 100
700 ms  MIDI out 80 54 0
```

Thus D022 manual-note ownership remains safe across the new track-selection context behavior.

## Frozen prior regressions

The same run keeps all prior Phase 2 suites green:

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

WP2.4 proof playback intentionally remains fixed to pattern 0 / logical track 0. Editor-selected track does not affect audio in WP2.6.

## Deterministic host audio

The host WAV remains byte-identical to the frozen WP2.4/WP2.5 reference:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

CI reports:

```text
WP2_4_AUDIO_REGRESSION_UNCHANGED=1
```

Therefore the editor-only track selector does not alter tracker timing or rendered audio.

## Device build and inherited safety

Pinned build environment remains:

- JieLi toolchain package `jieli-linux-toolchains-20260730.1`;
- clang `4.0.1`;
- official AC79 SDK tag `AC79NN_SDK_V1.2.1_2023-12-13`;
- SDK commit `d179b4484759423312073f5fbb232501aa491047`;
- previously frozen SDK package-input hashes verified.

Device build passes:

```text
loader: image 8232 B at 0x1c0a800, bss end 0x1c0f400
loader: ota.bin 6954 B
ok    .ram_text: 901 insns, no calls
ok    image 56060 B of 581564; RAM .data+.bss 12400 B of 98304; pool 96616 B of 344064
ok    register access: hal/ only (src/, loader/ clean)
```

Package identity remains:

`FM-1_987`

## Linked runtime symbols

```text
01c0b060 g       .bss         00000004 fmt_pattern_input_runtime
01c0b064 g       .bss         00000008 fmt_transport_runtime
01c0b06c g       .bss         00000001 fmt_playback_runtime
01c20000 g       .pool        00010168 fmt_store_storage
```

The pattern-input object grows from 3 bytes to 4 bytes, but measured normal RAM remains unchanged because the WP2.5 layout already occupied the aligned 4-byte slot before the following 8-byte transport object.

## Resource delta against WP2.5

| Resource | WP2.5 | WP2.6 | Delta |
|---|---:|---:|---:|
| app image | 55,980 B | **56,060 B** | **+80 B** |
| normal RAM (`.data + .bss`) | 12,400 B | **12,400 B** | **0 B** |
| pool RAM | 96,616 B | **96,616 B** | **0 B** |

Remaining capacity:

- app image: 525,504 B;
- normal RAM: 85,904 B;
- pool RAM: 247,448 B.

No editable-song pool memory is consumed by WP2.6.

## Final artifacts

`fm1tracker.bin` SHA-256:

`ed87a905eeba8d976d0ac965b8df2b53b6a9bcc6d21754f0c89276d820f307fa`

`fm1tracker-0.1.fwsc` SHA-256:

`af64a8b4a8c4716dcde0ddaeebc4cad8c80ffcdf573d8ef0ff60d5d0c55be879`

Evidence artifact:

- name: `wp2-6-track-selection-evidence`;
- artifact ID: `12519971447`;
- size: 1,108,424 B;
- ZIP SHA-256: `5d87727a17446a455462fee9708c3c79d3f032ffb73562f7c7e0f9b9acbc66cb`.

## Not verified on physical hardware

No physical FM-1 is currently available. Therefore WP2.6 does **not** claim physical verification of:

- actual black-key ergonomics for track selection;
- LED/screen discoverability of the selected track;
- install/boot/update behavior on a device;
- physical panel feel or accidental-selection rate;
- actual audio callback execution time.

D009/D023 remain provisional hardware UX decisions until the Phase 7 physical tests.

## Exit conclusion

WP2.6 satisfies its host/device-build exit criteria. Eight black keys now select eight bounded editor tracks in SEQ context while D022 page/step addressing, performance audition, song integrity, exact playback timing, deterministic audio, package identity and pool usage remain unchanged.

WP2.7 STEP REC is not implemented by this checkpoint.
