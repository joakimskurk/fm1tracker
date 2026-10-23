# WP2.7 Verification — STEP REC note entry

Date: 2026-10-23  
Branch: `phase2/tracker-core`  
WP2.7 start head: `6996dc8a38f41ca8bf361c5e897b3f3342f03e4c`  
Final verified implementation/workflow commit: `790fb87596150567307f88fb537cd63134a3f5fc`

GitHub issue: #15  
GitHub Actions run: `39999680852`  
Job: `120572238726` (`step-record`)  
Result: **SUCCESS**

## Scope verified

WP2.7 adds the first actual tracker write operation while keeping later editor semantics out of scope.

D024 is the frozen contract for this package:

- STEP REC is entered with REC only after the audio side has acknowledged a stopped D020 transport;
- REC while transport runs is ignored;
- successful REC entry enters/keeps pattern/SEQ context without changing selected page/step/track;
- REC exits STEP REC and remains in pattern context;
- HOME exits STEP REC and returns to PERFORMANCE;
- PLAY while STEP REC is active exits STEP REC before playback resumes;
- outside STEP REC, PLAY requests stop/resume of the existing proof transport;
- all 27 note keys are pitch input during STEP REC;
- OCT-/OCT+ control pitch octave during STEP REC, while ordinary SEQ retains D022 page navigation;
- stored pitch equals audition pitch: `53 + 12 * octave + key_index` with octave `-2..+2`;
- simultaneous new key downs choose the lowest physical key/lowest pitch for the single write;
- the write gesture remains latched until all physical note keys are released;
- only `FmtStep.note` is changed;
- successful entry advances the edit cursor exactly one global step modulo 64, including `63 -> 0`;
- pattern remains fixed to pattern 0 for this package;
- REST, TIE and NOTE OFF editing remain WP2.8.

## Architecture / concurrency

WP2.7 does not let the main/editor loop write the transport's live `running` field while the audio ISR may be advancing it.

Instead:

- main owns a bounded 32-bit `tracker_play_requested` mailbox;
- audio owns all mutations of `FmtTransport.running`;
- audio publishes `tracker_transport_stopped` only after the real transport is stopped;
- STEP REC entry requires both `tracker_play_requested == 0` and the audio-side stopped acknowledgement;
- while stopped, `fmt_playback_end_frame()` performs no row/store read because transport is not running;
- editor writes therefore occur only after the audio-side transport/store reader is quiescent.

The existing WP2.4 proof tone may sustain while transport is stopped. This does not advance rows or read new tracker steps.

## Host/unit verification

All inherited regressions passed:

```text
FMT_MODEL_TESTS_OK=1
FmtStep=8 FmtPattern=4096 FmtSong=65896 lock_budget=16384 total_budgeted=82280

FMT_STORE_TESTS_OK=1
store_bytes=65896 steps=8192 patterns=16 tracks=8 steps_per_pattern=64

FMT_TRANSPORT_TESTS_OK=1
phase_per_step=661500 default_bpm=120 state_bytes=8 long_run_hours=6 max_bpm=65535

FMT_PLAYBACK_TESTS_OK=1
pattern=0 track=0 proof_notes=16 spacing=4 loop_frames_120=352800 state_bytes=1 event_bytes=4

FMT_PATTERN_INPUT_TESTS_OK=1
state_bytes=4 pages=4 steps_per_page=16 total_steps=64 white_keys=16 tracks=8 track_keys=8 reserved_black=3
```

New STEP REC editor tests passed:

```text
FMT_STEP_RECORD_TESTS_OK=1
state_bytes=2 pattern=0 note_min=0 note_max=127 wrap=63_to_0 chord=lowest_key
```

The focused tests prove:

- running transport rejects STEP REC entry;
- active/inactive/null/invalid-state gates are deterministic;
- a valid write changes exactly the selected pattern-0 track/step note byte;
- target instrument, volume, effect, effect value, flags and lock-head are preserved;
- neighboring song bytes remain unchanged in the exact-target snapshot comparison;
- invalid notes, invalid cursor/track, running transport and latched gestures cannot mutate or advance;
- after a successful note write, the cursor advances exactly once;
- step 63 wraps to step 0 while selected track is preserved;
- every one-hot physical key can be selected deterministically;
- simultaneous key-down masks select the lowest key;
- bits outside the physical 27-key span are ignored;
- the latch prevents a second write until the note-key set is fully released.

## Full-app host gesture verification

Inherited WP2.5 and WP2.6 context fixtures remain green:

```text
WP2_5_PATTERN_CONTEXT_HOST_OK=1
WP2_6_TRACK_CONTEXT_HOST_OK=1
```

The new full-app gesture fixture passes:

```text
WP2_7_STEP_REC_CONTEXT_HOST_OK=1
```

It proves:

- REC while the default proof transport is running is ignored;
- the first key therefore remains normal PERFORMANCE F3 / MIDI 53 audition;
- PLAY stop followed by the audio acknowledgement permits REC entry;
- OCT+ in STEP REC changes pitch octave;
- simultaneous physical key 0 + key 2 downs audition MIDI 65 and 67;
- a later key 3 gesture auditions MIDI 68;
- PLAY from STEP REC exits recording before returning to ordinary SEQ/playback;
- a white key after that PLAY is consumed by ordinary SEQ rather than auditioning;
- REC exit while stopped remains in ordinary SEQ;
- HOME returns to PERFORMANCE and audition is restored;
- note releases remain paired with NOTE OFF across context changes.

## Frozen audio regression

The untouched default host run is still byte-identical to WP2.4/WP2.5/WP2.6:

```text
HOST_WAV_SHA256=453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440
WP2_4_AUDIO_REGRESSION_UNCHANGED=1
```

Therefore the new stop/REC/editor control layer does not change the default proof playback when the new controls are not used.

## Device build / safety

Pinned environment remained unchanged:

- JieLi toolchain package: `jieli-linux-toolchains-20260730.1`;
- clang: 4.0.1;
- official AC79 SDK tag: `AC79NN_SDK_V1.2.1_2023-12-13`;
- SDK commit: `d179b4484759423312073f5fbb232501aa491047`;
- frozen SDK package-input hashes passed.

Inherited device safety checks passed:

```text
loader: image 8232 B at 0x1c0a800, bss end 0x1c0f400
loader: ota.bin 6954 B
ok .ram_text: 901 insns, no calls
ok register access: hal/ only (src/, loader/ clean)
PACKAGE_IDENTITY=FM-1_987
```

No HAL, loader, linker layout, model, store, transport, playback or build-tool source was changed by WP2.7.

## Linked runtime state

```text
01c0b060 g .bss 00000002 fmt_step_record_runtime
01c0b062 g .bss 00000004 fmt_pattern_input_runtime
01c0b068 g .bss 00000008 fmt_transport_runtime
01c0b070 g .bss 00000001 fmt_playback_runtime
01c20000 g .pool 00010168 fmt_store_storage
```

The song store therefore remains exactly 65,896 B in `.pool` at `0x01C20000`.

`FmtStepRecord` is exactly 2 B. The two 32-bit transport handoff flags plus linker/alignment effects produce only the measured +4 B total normal-RAM delta versus WP2.6.

## Resource result

| Resource | WP2.6 | WP2.7 | Delta | Limit | Free after WP2.7 |
|---|---:|---:|---:|---:|---:|
| app image | 56,060 B | 56,548 B | **+488 B** | 581,564 B | 525,016 B |
| normal RAM (`.data + .bss`) | 12,400 B | 12,404 B | **+4 B** | 98,304 B | 85,900 B |
| pool RAM | 96,616 B | 96,616 B | **+0 B** | 344,064 B | 247,448 B |

The edit operation consumes no additional song/pool storage.

## Final artifacts

- `fm1tracker.bin` SHA-256: `9d0c198b7428996c8147eb6bab085ad3580dc432291b2e9e56114a65d1dfaeb4`
- `fm1tracker-0.1.fwsc` SHA-256: `9eb9c278175ac2aac693bc70cea4b7af1a5cef7b87fc9c845c68e4d6f1b73130`
- host WAV SHA-256: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`
- package identity: `FM-1_987`

Evidence artifact:

- name: `wp2-7-step-record-evidence`
- artifact ID: `12521283164`
- ZIP SHA-256: `d74cd8b38687ef8802b840beaf626565a3f552900deccafe605e3069df587224`
- uploaded size: 1,110,401 bytes

## Explicitly not verified / not implemented

- physical FM-1 STEP REC ergonomics are not tested because hardware is unavailable;
- final chord-entry ergonomics remain provisional until Phase 7;
- REST is not implemented;
- TIE is not implemented;
- explicit NOTE OFF editing is not implemented;
- no tracker screen is added;
- no multitrack playback expansion is added;
- no persistence, p-lock or synth-engine work is included.

WP2.8 owns REST / TIE / NOTE-OFF semantics and must be a separate work package.
