# FM1Tracker Changelog

Git remains the detailed history. This file records milestone-level changes only.

## 2026-10-23 - Phase 0 and Phase 1 baseline work

### Foundation complete

- Frozen V0.1 architecture, control map, data model and engine scope.
- Defined 8 tracks, 64 steps/pattern, 16 patterns and max four parameter locks per step.
- Defined STEP REC, REST and parameter-lock behavior as core future product requirements.
- Full Furnace port explicitly kept out of scope; Furnace is currently reference/interoperability only.

### WP1.1 - Dinghy provenance freeze

- Pinned Dinghy upstream to `d44df7db09b98ccad2f1233b8f83f5f09c04829e`.
- Recorded GPL/OFL provenance and external JieLi SDK requirements.
- Explicitly excluded Dinghy branding assets.

### WP1.2 - Minimal pristine Dinghy import

- Imported the manifest-selected platform/HAL/loader/app/test/build baseline.
- Verified every imported path against pinned upstream Git blob SHA and Git mode.
- Preserved script executable bits and source licensing headers.
- Corrected the import-manifest typo for `firmware/hal/fm1_uart.h`.
- No FM1Tracker tracker/synth behavior added.

### WP1.3 - Pristine baseline verification

- Verified the pristine host harness in GitHub Actions.
- Produced and hashed host WAV and two screen PNG artifacts.
- Verified Dinghy's JieLi toolchain installer and clang 4.0.1 execution.
- Isolated the remaining pristine-device-build dependency to the external AC79 SDK checkout.
- No physical FM-1 verification performed.
- Detailed evidence: `docs/WP1_3_VERIFICATION.md`.

### WP1.4 - FM1Tracker identity rename

- Checked the current Firmware Hub identity list and selected fixed package identity `FM-1_987`.
- Added `tools/build_fm1tracker.py` as an identity wrapper around the unchanged verified Dinghy build driver.
- Routed `build.sh` through the FM1Tracker wrapper while preserving the existing toolchain/SDK gates.
- Renamed host output artifacts to `fm1tracker*` and compiled visible app name `FM1TRACKER`.
- Added a CI regression gate proving the protected Dinghy app/platform/HAL/loader/build-driver paths stayed unchanged.
- Verified the FM1Tracker host screen identity and produced renamed host artifacts.
- Verified the FM1Tracker WAV is bit-identical to the pristine WP1.3 WAV (`dac1695b8e69f26334d548005e6cae505cf39ace297dcf6a57338e70fc386b9b`).
- Detailed evidence: `docs/WP1_4_VERIFICATION.md`.

### WP1.5 - Renamed device baseline and resource budget

- Pinned the official JieLi AC79 SDK tag `AC79NN_SDK_V1.2.1_2023-12-13` to commit `d179b4484759423312073f5fbb232501aa491047` as an external build dependency.
- Verified the three consumed SDK package inputs against their frozen SHA-256 hashes.
- Completed the full renamed FM1Tracker device build in GitHub Actions.
- Verified final package identity `FM-1_987` and inherited Dinghy safety checks.
- Captured the clean reference budget: 55,088 B app image, 12,368 B normal RAM and 30,720 B pool RAM.
- Recorded the nominal 256-frame / 44.1 kHz callback period as 5.804988662 ms without claiming unmeasured hardware render time.
- Final verified artifacts: `fm1tracker.bin` SHA-256 `7b313502f2ab222e6ecf19f970772a36f314bce7c880b505157d4d5a2e4dfbaf`; `fm1tracker-0.1.fwsc` SHA-256 `a3afcc069e4a9fe161f8470f477a76720b411fc6f302ebcf862c272a9a683bc2`.
- No physical FM-1 verification performed.
- Detailed evidence: `docs/WP1_5_VERIFICATION.md`.

### Phase 1 complete

The clean platform/identity/build baseline is now reproducible and budgeted. No tracker implementation has been mixed into Phase 1.

## 2026-10-23 - Phase 2 tracker core

### WP2.1 - Compile-time tracker data model

- Added the FM1Tracker-owned V0.1 compile-time model in `firmware/src/fmt_model.h`.
- Frozen 8 tracks, 64 steps/pattern, 16 pattern slots, 128 order entries, 16 instruments and the existing four-lock-per-step semantic limit.
- Enforced the 8-byte `FmtStep` representation and 4,096-byte `FmtPattern` at compile time.
- Added deterministic empty-step defaults plus null-safe/bounds-safe pattern and step accessors.
- Defined a non-instantiated 65,896-byte `FmtSong` core shape and kept the p-lock allocator unfrozen.
- Proved WP2.1 adds no runtime footprint and preserved Phase 1 artifacts byte-identically.
- Detailed evidence: `docs/WP2_1_VERIFICATION.md`.

### WP2.2 - Bounds-safe pattern store

- Added a narrow runtime store API and exactly one 65,896-byte `FmtSong` in `.pool` at `0x01C20000`.
- Explicitly initialized all 8,192 steps to the frozen tracker sentinels and verified bounds/reset/mutation isolation.
- Final resources: 55,188 B image, 12,368 B normal RAM and 96,616 B pool RAM.
- Detailed evidence: `docs/WP2_2_VERIFICATION.md`.

### WP2.3 - Integer transport clock

- Added an 8-byte `FmtTransport` using D020 exact rational 1/16-note timing with threshold `661500` and integer BPM accumulated per audio frame.
- Added deterministic reset/start/stop/resume, exact `63 -> 0` wrap and long-run no-drift host verification.
- Kept firmware transport free of float, runtime division and 64-bit arithmetic.
- Final resources: 55,328 B image, 12,376 B normal RAM and 96,616 B pool RAM.
- Final verification: Actions run `39988630290`, job `120534295734`.
- Detailed evidence: `docs/WP2_3_VERIFICATION.md`.

### WP2.4 - One-track audible playback

- Added the D021 frame-0 dispatch contract and deterministic pattern-0 / track-0 proof playback using the existing integer sine primitive.
- At 120 BPM, step 1 starts on frame 5513 and the full 64-step loop returns on frame 352800 without callback quantization.
- Frozen host WAV SHA-256 `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`.
- Corrected startup/store handoff and deferred inherited flash settings writes while the proof lane sounds.
- Final resources: 55,692 B image, 12,396 B normal RAM and 96,616 B pool RAM.
- Final verification: Actions run `39991490860`, job `120544086455`.
- Detailed evidence: `docs/WP2_4_VERIFICATION.md`.

### WP2.5 - 16-step page input

- Frozen D022 explicit PERFORMANCE/SEQ context and four clamped pages of sixteen white-key step positions.
- Mapped the 16 physical white keys bijectively to local steps 0..15 and proved all 64 global addresses exactly once.
- Preserved performance audition and safe note release across context changes; navigation cannot mutate the song store.
- Added a 3-byte `FmtPatternInput` state and preserved the frozen WP2.4 WAV.
- Final resources: 55,980 B image, 12,400 B normal RAM and 96,616 B pool RAM.
- Final verification: Actions run `39994230797`, job `120553466206`.
- Detailed evidence: `docs/WP2_5_VERIFICATION.md`.

### WP2.6 - Black-key track selection prototype

- Frozen D023: black-key indexes `1,3,5,8,10,13,15,17` select logical tracks 0..7; indexes `20,22,25` remain reserved.
- Extended `FmtPatternInput` to 4 B with one bounded selected-track byte.
- Selection is SEQ-only, survives HOME/SEQ, preserves page/step, and cannot mutate `FmtSong`.
- Black keys continue normal audition/MIDI behavior in PERFORMANCE and held notes release safely across context changes.
- Preserved the frozen WP2.4 WAV and kept proof playback fixed to track 0.
- Final resources: 56,060 B image, 12,400 B normal RAM and 96,616 B pool RAM.
- Final verification: Actions run `39995856373`, job `120559070230`.
- Detailed evidence: `docs/WP2_6_VERIFICATION.md`.

### WP2.7 - STEP REC note entry

- Frozen D024 before source implementation: STEP REC is a stopped-transport editor mode entered with REC only after audio has acknowledged a real stop.
- Added an explicit main-to-audio play-request/stopped-ack handoff so the audio callback remains the sole writer of live D020 transport state while editor writes occur only after the playback/store reader is quiescent.
- Added a 2-byte `FmtStepRecord` state with active/gesture-latched flags.
- Added deterministic modulo-64 cursor advance to `FmtPatternInput`, including exact step `63 -> 0` while preserving selected track.
- All 27 note keys become pitch input in STEP REC; OCT-/OCT+ change pitch octave while ordinary SEQ keeps page navigation.
- Stored pitch equals audition pitch (`53 + 12 * octave + key_index`, octave -2..+2).
- Simultaneous key downs deterministically store the lowest physical key/lowest pitch once. The write gesture remains latched until all note keys are released, preventing duplicate advances from chord-like/staggered presses.
- A successful write changes only the selected row's `note` byte; instrument, volume, effect, effect value, flags and lock-head are preserved.
- REC exit remains in SEQ, HOME returns to PERFORMANCE, and PLAY exits STEP REC before resuming transport.
- Added unit tests for exact write isolation, field preservation, invalid/running-state rejection, lowest-key policy, latch behavior and `63 -> 0` wrap.
- Added a full-app host gesture fixture proving running REC rejection, stopped REC entry, STEP REC octave/audition, PLAY/REC/HOME context behavior and safe note releases.
- Preserved every WP2.1–WP2.6 regression and the exact frozen WP2.4 WAV SHA-256 `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`.
- Full JieLi device build and inherited safety checks pass with identity `FM-1_987`.
- Final resources: 56,548 B image (**+488 B** vs WP2.6), 12,404 B normal RAM (**+4 B**) and 96,616 B pool RAM (**+0 B**).
- Final artifacts: `fm1tracker.bin` `9d0c198b7428996c8147eb6bab085ad3580dc432291b2e9e56114a65d1dfaeb4`; `fm1tracker-0.1.fwsc` `9eb9c278175ac2aac693bc70cea4b7af1a5cef7b87fc9c845c68e4d6f1b73130`.
- Final verification: Actions run `39999680852`, job `120572238726`.
- Detailed evidence: `docs/WP2_7_VERIFICATION.md`.

### Next

**WP2.8 — REST / TIE / NOTE-OFF semantics.** Issue #16 is READY TO START and no implementation has begun. Before source code, freeze exact REST field semantics, NOTE OFF gesture, the currently-unfrozen TIE gesture, and EMPTY/TIE/OFF playback meaning. WP2.9 tracker-screen work must not start in the same package.
