# WP2.8 Verification — REST / TIE / NOTE-OFF semantics

Date: 2026-10-23  
Status: **COMPLETE / VERIFIED**

## Scope

WP2.8 adds explicit stopped-STEP-REC editor semantics and one-lane playback semantics for the already-frozen special note sentinels:

- `FMT_NOTE_EMPTY = 0xFF`
- `FMT_NOTE_TIE = 0xFD`
- `FMT_NOTE_OFF = 0xFE`

It does not add the tracker screen, multitrack playback, p-locks, engines, song mode, persistence or live-performance capture.

## Baseline and verified checkpoint

WP2.8 start head:

`13c0748dd696e4f3a47279d00641068b5e915e46`

This start head differs from the WP2.7 closeout only by the previously documented live-performance vision/roadmap planning commits; those commits changed no firmware or tests.

Final verified implementation/workflow commit:

`c66405bead1c79fd891cf0d12c8bd241019a7aaf`

GitHub Actions:

- run: `40004436981`
- job: `120588627172` (`special-events`)
- result: **SUCCESS**

## Decision frozen before code

D025 was recorded before source implementation.

### Editor semantics

While D024 STEP REC is active and transport is stopped:

- **REST / EMPTY:** press and release `FX` without using the modifier action. REST is written on `FX` release, not press.
- **TIE:** press `SELECT` while `FX` is not pending.
- **NOTE OFF:** hold `FX`, then press `SELECT`.

The modifier-first NOTE OFF gesture writes OFF on the `SELECT` press, consumes the pending FX tap and guarantees that the later `FX` release cannot insert a second REST.

Every successful special edit:

- targets pattern 0, current selected track and current global step;
- changes only `FmtStep.note`;
- preserves instrument, volume, effect, effect value, flags and lock-head;
- advances exactly one global step modulo 64;
- preserves selected track;
- wraps step 63 to step 0 exactly;
- participates in the existing D024 latch so a simultaneous note-key down cannot create a second row write.

Pending FX state is cancelled when STEP REC exits through REC, HOME or PLAY.

### Playback semantics

`FmtPlaybackEvent` remains exactly 4 bytes and now carries an explicit event kind:

- `NONE = 0`
- `NOTE = 1`
- `TIE = 2`
- `OFF = 3`

Row decoding is deterministic:

- ordinary note `0..127` -> `NOTE` + pitch;
- EMPTY -> `NONE`, no pitch/gate command;
- TIE -> `TIE`, preserve an already active gate/pitch without retrigger;
- OFF -> `OFF`, release the current proof voice;
- malformed/reserved non-note values -> safe `NONE`.

No sentinel can become a raw pitch.

D021 frame ordering is unchanged. A row decoded at a crossed boundary becomes active beginning with the next rendered frame.

## Host verification

All inherited core regressions passed:

```text
FMT_MODEL_TESTS_OK=1
FMT_STORE_TESTS_OK=1
FMT_TRANSPORT_TESTS_OK=1
FMT_PATTERN_INPUT_TESTS_OK=1
FMT_STEP_RECORD_TESTS_OK=1
FMT_PLAYBACK_TESTS_OK=1
WP2_5_PATTERN_CONTEXT_HOST_OK=1
WP2_6_TRACK_CONTEXT_HOST_OK=1
WP2_7_STEP_REC_CONTEXT_HOST_OK=1
```

WP2.8 focused editor marker:

```text
state_bytes=3 pattern=0 note_min=0 note_max=127 wrap=63_to_0 chord=lowest_key specials=empty,tie,off fx_modifier=tested
```

WP2.8 focused playback marker:

```text
pattern=0 track=0 proof_notes=16 spacing=4 loop_frames_120=352800 state_bytes=1 event_bytes=4 kinds=none,note,tie,off
```

The full-app special-event fixture passed:

```text
WP2_8_SPECIAL_EVENTS_CONTEXT_HOST_OK=1
```

It exercises a real held/released FX modifier sequence through the host panel layer, including:

- FX tap -> REST;
- SELECT -> TIE;
- FX held + SELECT -> OFF;
- FX release after OFF cannot create a second REST;
- HOME cancels pending REST;
- REC exit cancels pending REST;
- PLAY exit cancels pending REST before playback resumes;
- ordinary note entry still works after the special events.

The host harness was extended with `btndown` / `btnup` events so release/modifier behavior can be tested like the real HAL while preserving the historical one-shot `btn` behavior used by older fixtures.

## Audible NOTE OFF proof

The focused host fixture edited the beginning of the proof lane to:

`REST, TIE, OFF, F3, TIE`

After playback resumed, the WAV analysis measured:

```text
WP2_8_OFF_AUDIO_BEHAVIOR_OK=1 before=4410 quiet=0 after=4410
```

Meaning:

- 4,410 non-zero stereo samples in the checked window before OFF;
- exactly 0 non-zero samples in the checked post-release silence window;
- 4,410 non-zero samples after the next ordinary note reopened the proof gate.

This also guards the FX+SELECT ordering: an accidental extra REST on FX release would shift the following note and fail the expected post-OFF audio window.

## Frozen default audio regression

The untouched default host run remains byte-identical to WP2.4 through WP2.8:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

This proves that adding the proof-voice gate and special-event event kinds did not alter the default note-only render.

## Device build and safety

Pinned build inputs:

- JieLi toolchain: `jieli-linux-toolchains-20260730.1`
- clang: 4.0.1
- official AC79 SDK commit: `d179b4484759423312073f5fbb232501aa491047`

Device build passed:

```text
loader: image 8232 B at 0x1c0a800, bss end 0x1c0f400
loader: ota.bin 6954 B
ok .ram_text: 901 insns, no calls
ok register access: hal/ only (src/, loader/ clean)
PACKAGE_IDENTITY=FM-1_987
```

## Linked runtime state

```text
01c0b070 g       .bss         00000003 fmt_step_record_runtime
01c0b073 g       .bss         00000004 fmt_pattern_input_runtime
01c0b078 g       .bss         00000008 fmt_transport_runtime
01c0b080 g       .bss         00000001 fmt_playback_runtime
01c20000 g       .pool        00010168 fmt_store_storage
```

The STEP REC editor object grows from 2 B to 3 B to own/test the pending FX modifier state. The playback state and 4-byte playback-event budget remain unchanged.

## Resource delta vs WP2.7

| Resource | WP2.7 | WP2.8 | Delta | Limit | Free after WP2.8 |
|---|---:|---:|---:|---:|---:|
| app image | 56,548 B | 56,748 B | **+200 B** | 581,564 B | 524,816 B |
| normal RAM (`.data + .bss`) | 12,404 B | 12,420 B | **+16 B** | 98,304 B | 85,884 B |
| pool RAM | 96,616 B | 96,616 B | **+0 B** | 344,064 B | 247,448 B |

The pool-backed editable song remains exactly 65,896 B at `0x01C20000`.

## Final artifacts

- `fm1tracker.bin` SHA-256: `a4dd8d8326f9f28852d243c2a07e88e0ac3af5c5ce302ff1ac8b45883b72eecf`
- `fm1tracker-0.1.fwsc` SHA-256: `fd569d068c272e49b00cf0505bb613b774be08ae0aa757932594005b1c36ef69`
- default host WAV SHA-256: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`
- package identity: `FM-1_987`

Evidence artifact:

- name: `wp2-8-special-events-evidence`
- artifact ID: `12523074553`
- size: 1,311,404 B
- ZIP SHA-256: `263d20fbf9162f6cc5e199980f2c76dd9267a2ec92c84baf5bc4408f476b041b`

## Verification limits

Still not physically verified because no FM-1 hardware is currently available:

- FX/SELECT gesture ergonomics;
- tactile discoverability of REST/TIE/OFF;
- install/boot/audio behavior on physical FM-1;
- actual hardware callback execution time;
- audible release feel on the physical output path.

Those remain explicit hardware validation items rather than implied by host/device-build success.

## Closeout

WP2.8 exit criteria are satisfied at the host + firmware-build level. The next package is WP2.9, the compact tracker screen. WP2.9 implementation must not be mixed into this verified checkpoint.