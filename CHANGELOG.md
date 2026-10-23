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

- Pinned official JieLi AC79 SDK tag `AC79NN_SDK_V1.2.1_2023-12-13` to commit `d179b4484759423312073f5fbb232501aa491047` as an external build dependency.
- Verified consumed SDK package inputs against frozen SHA-256 hashes.
- Completed the full renamed FM1Tracker device build and inherited safety checks.
- Captured clean baseline: 55,088 B app image, 12,368 B normal RAM and 30,720 B pool RAM.
- Final verified artifacts: `fm1tracker.bin` `7b313502f2ab222e6ecf19f970772a36f314bce7c880b505157d4d5a2e4dfbaf`; `fm1tracker-0.1.fwsc` `a3afcc069e4a9fe161f8470f477a76720b411fc6f302ebcf862c272a9a683bc2`.
- Detailed evidence: `docs/WP1_5_VERIFICATION.md`.

### Phase 1 complete

The clean platform/identity/build baseline is reproducible and budgeted. Physical FM-1 install/audio/render-time verification remains a later hardware gate.

## 2026-10-23 - Phase 2 tracker core

### WP2.1 - Compile-time tracker data model

- Added the V0.1 model and froze 8 tracks, 64 steps/pattern, 16 patterns and an 8-byte `FmtStep`.
- Added deterministic defaults/sentinels, bounds-safe accessors and compile-time budget checks.
- Detailed evidence: `docs/WP2_1_VERIFICATION.md`.

### WP2.2 - Bounds-safe pattern store

- Added one 65,896-byte runtime `FmtSong` in `.pool` at `0x01C20000`.
- Explicitly initialized all 8,192 steps and verified bounds/reset/mutation isolation.
- Final resources: 55,188 B image, 12,368 B normal RAM, 96,616 B pool.
- Detailed evidence: `docs/WP2_2_VERIFICATION.md`.

### WP2.3 - Integer transport clock

- Added D020 exact rational integer 1/16-note timing with threshold `661500`.
- Added deterministic reset/start/stop/resume, exact 63 -> 0 wrap and long-run no-drift verification.
- Kept firmware transport free of float, runtime division and 64-bit arithmetic.
- Final resources: 55,328 B image, 12,376 B normal RAM, 96,616 B pool.
- Final verification: run `39988630290`.
- Detailed evidence: `docs/WP2_3_VERIFICATION.md`.

### WP2.4 - One-track audible playback

- Added D021 frame-0 dispatch and deterministic pattern-0 / track-0 proof playback.
- At 120 BPM, step 1 begins frame 5513 and the complete loop returns frame 352800.
- Frozen host WAV SHA-256 `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`.
- Final resources: 55,692 B image, 12,396 B normal RAM, 96,616 B pool.
- Final verification: run `39991490860`.
- Detailed evidence: `docs/WP2_4_VERIFICATION.md`.

### WP2.5 - 16-step page input

- Frozen D022 PERFORMANCE/SEQ context and four clamped 16-step pages.
- Mapped all 16 physical white keys bijectively across 64 global steps.
- Preserved performance audition and proved navigation cannot mutate song data.
- Final resources: 55,980 B image, 12,400 B normal RAM, 96,616 B pool.
- Final verification: run `39994230797`.
- Detailed evidence: `docs/WP2_5_VERIFICATION.md`.

### WP2.6 - Black-key track selection prototype

- Frozen D023: black-key indexes `1,3,5,8,10,13,15,17` select logical tracks 0..7; `20,22,25` reserved.
- Extended `FmtPatternInput` to 4 B.
- Track selection is SEQ-only, preserves page/step and cannot mutate the song.
- Final resources: 56,060 B image, 12,400 B normal RAM, 96,616 B pool.
- Final verification: run `39995856373`.
- Detailed evidence: `docs/WP2_6_VERIFICATION.md`.

### WP2.7 - STEP REC note entry

- Frozen D024 stopped-transport STEP REC semantics before source code.
- Added main-to-audio play-request/stopped-ack handoff so audio remains sole writer of live transport state.
- Added deterministic ordinary-note writes at selected track/step and exact modulo-64 cursor advance.
- All 27 note keys become pitch keys in STEP REC; stored pitch matches audition pitch.
- Simultaneous downs store the lowest pitch once; gesture latches until all note keys release.
- Successful writes change only `FmtStep.note` and preserve all other row fields.
- REC exit remains SEQ, HOME returns PERFORMANCE and PLAY exits STEP REC before resuming.
- Preserved every earlier regression and the exact frozen WP2.4 WAV.
- Final resources: 56,548 B image, 12,404 B normal RAM, 96,616 B pool.
- Final artifacts: `fm1tracker.bin` `9d0c198b7428996c8147eb6bab085ad3580dc432291b2e9e56114a65d1dfaeb4`; `fm1tracker-0.1.fwsc` `9eb9c278175ac2aac693bc70cea4b7af1a5cef7b87fc9c845c68e4d6f1b73130`.
- Final verification: run `39999680852`.
- Detailed evidence: `docs/WP2_7_VERIFICATION.md`.

### Live performance capture product direction recorded

- Added `docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` as accepted future core direction.
- Defined the intended complement to precise stopped STEP REC: seamless PLAY -> REC capture while transport continues.
- Preserved all eight logical tracks and set a reference performance configuration covering drums, bass, poly synth and lead plus four flexible tracks.
- Added a measured 3–6 voice target for at least one polyphonic track if hardware budget permits.
- Defined future metronome, CHANNEL/GLOBAL performance FX and bounded parameter/FX capture as tracker-editable data.
- Promoted live capture from optional Phase 9 to a planned core milestone after p-lock/event and engine foundations exist.
- No firmware/test behavior changed in these planning commits.

### WP2.8 - REST / TIE / NOTE-OFF semantics

- Frozen D025 before source implementation.
- REST is an FX tap committed on FX release; `SELECT` alone writes TIE; hold FX then press SELECT writes NOTE OFF.
- Delaying REST until FX release makes the modifier deterministic: OFF consumes pending REST and later FX release cannot double-write.
- REC/HOME/PLAY exit cancels pending REST safely.
- Every special edit changes only the selected row's `note` byte, preserves all other row fields, advances exactly once and wraps 63 -> 0.
- Extended `FmtStepRecord` from 2 B to 3 B so pending FX modifier state is owned/tested by the editor layer.
- Kept `FmtPlaybackEvent` exactly 4 B and replaced the old boolean note flag with explicit `NONE`, `NOTE`, `TIE`, `OFF` kinds.
- EMPTY emits no voice command; TIE explicitly preserves an already active gate without retrigger; OFF releases the proof voice; malformed/reserved values remain safe NONE.
- Extended the host panel harness with backward-compatible `btndown` / `btnup` support so held modifier/release behavior is testable like the real HAL.
- Added focused full-app audio proof: `before=4410 quiet=0 after=4410`, demonstrating exact silence after NOTE OFF and sound returning on the following note.
- Preserved all WP2.1–WP2.7 regressions and the exact frozen default WAV SHA-256 `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`.
- Full JieLi device build and safety gates pass with identity `FM-1_987`.
- Final resources: 56,748 B image (**+200 B** vs WP2.7), 12,420 B normal RAM (**+16 B**) and 96,616 B pool (**+0 B**).
- Final artifacts: `fm1tracker.bin` `a4dd8d8326f9f28852d243c2a07e88e0ac3af5c5ce302ff1ac8b45883b72eecf`; `fm1tracker-0.1.fwsc` `fd569d068c272e49b00cf0505bb613b774be08ae0aa757932594005b1c36ef69`.
- Evidence artifact: ID `12523074553`, ZIP SHA-256 `263d20fbf9162f6cc5e199980f2c76dd9267a2ec92c84baf5bc4408f476b041b`.
- Final verification: Actions run `40004436981`, job `120588627172`.
- Detailed evidence: `docs/WP2_8_VERIFICATION.md`.

### WP2.9 - Compact tracker screen

- Frozen D026 before source implementation and documented the exact 240×240 screen contract in `docs/WP2_9_SCREEN_CONTRACT.md`.
- Added a 44-byte `FmtTrackerScreen` presentation cache with exactly eight visible rows, ten persistent dirty regions and a hard maximum of two rendered regions per main-loop poll.
- Implemented cursor-owned half-page windows so playback never scrolls the edit surface under the user's selection.
- Added independent `>` edit-cursor and `*` playhead markers, including explicit `>*` same-row state and header `PLYnn` when the playhead is off-screen.
- Added deterministic row notation for ordinary notes, REST `---`, TIE `===`, NOTE OFF `OFF`, malformed values and current instrument/volume/effect placeholders.
- Integrated tracker drawing only in the main loop; CI proves the complete audio section is byte-identical to the WP2.9 start baseline.
- Added deterministic SEQ and STEP REC framebuffer fixtures plus a pixel-identical `SEQ -> HOME` A/B proof.
- Visually reviewed and froze raw framebuffer hashes: SEQ `e67d0cf9aa2fa391464237a40ed795bb6ba881e15aff40340e1b92a4a8eccb7a`; STEP REC `4b0192b084e6ee483d7a8f98add2ae95229dcc914583d3cbe957944824cb6424`.
- Preserved all WP2.1–WP2.8 unit/full-app regressions and the exact frozen default WAV.
- Full JieLi device build and safety gates pass with identity `FM-1_987`.
- Final resources: 59,252 B image (**+2,504 B** vs WP2.8), 12,464 B normal RAM (**+44 B**) and 96,616 B pool (**+0 B**). The RAM delta is exactly the presentation state.
- Final artifacts: `fm1tracker.bin` `34d0e8c4344e091704608e08357964e7ae0b18673dd0b93f36c4e5bcc2cb4c7c`; `fm1tracker-0.1.fwsc` `3cfd0a2da3836720d2f921b04f29e93c16ca83ab18ab7242c99f2c333d890ca7`.
- Evidence artifact: ID `12525582378`, ZIP SHA-256 `6c23c962c96de458d5d328e60a3b5cbd224e2011933b2d812ee70d8fe6c8daa8`.
- Final verification: Actions run `40009689758`, job `120606559644`.
- Detailed evidence: `docs/WP2_9_VERIFICATION.md`.

### WP2.10 - Phase 2 integration and closeout

- Added a direct module-composition integration test covering all 64 editor addresses, all eight track selectors, exact `63 -> 0` and `15 -> 16` cursor transitions, ordinary note / REST / TIE / OFF entry, D026 screen synchronization and exact 352,800-frame transport-loop timing.
- Compared the complete 65,896-byte final `FmtSong` against an expected copy byte-for-byte and proved track-7 integration edits leave the fixed track-0 proof lane unchanged.
- Added a full-app host scenario through the real unchanged `dinghy.c` panel path. Ordinary SEQ traversal leaks no audition MIDI; only four deliberate STEP REC note gestures produce MIDI pairs.
- Visually reviewed and froze the integrated tracker framebuffer SHA-256 `a993551ce922d90186080bf554b82477b4336e160a6b301c446f0d275569b6d7`.
- Proved HOME return after the full workflow is pixel-identical to an equivalent HOME-only control (`ea33024427340416c17a73ecfb2303e37262b8c61e2f2d30ca06265ba6e0b899`).
- Ran every WP2.1–WP2.9 unit/full-app regression in one closeout gate and preserved the frozen default WAV SHA-256 `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`.
- Full JieLi device build, identity `FM-1_987`, HAL-only register access and resource checks remain green.
- WP2.10 required **zero production-source changes** relative to the WP2.9 closeout baseline. Final BIN/FWSC artifacts are byte-identical to WP2.9.
- Final Phase 2 resources: 59,252 B image, 12,464 B normal RAM, 96,616 B pool.
- Total Phase 2 cost versus Phase 1: **+4,164 B image, +96 B normal RAM, +65,896 B pool**.
- Final artifacts: `fm1tracker.bin` `34d0e8c4344e091704608e08357964e7ae0b18673dd0b93f36c4e5bcc2cb4c7c`; `fm1tracker-0.1.fwsc` `3cfd0a2da3836720d2f921b04f29e93c16ca83ab18ab7242c99f2c333d890ca7`.
- Final verification: Actions run `40015242947`, job `120625519388`; evidence artifact ID `12527623388`, ZIP SHA-256 `2a4cbcda3e7f1f26fe2f117062fdd72c9fb274ae0ce8aa4ccfa1a7763829bd9c`.
- Durable evidence: `docs/PHASE2_VERIFICATION.md`.

### Phase 2 complete

The tracker-core phase is verified complete at the host/device-build level. The system now has deterministic 8-track/64-step editor addressing, stopped STEP REC with ordinary notes and REST/TIE/OFF, exact integer playback timing, a bounded compact tracker display and a complete integrated closeout proof. Physical FM-1 install/audio/timing/readability/ergonomics remain later hardware gates.

## 2026-10-23 - Phase 3 p-locks and tracker effects

### WP3.1 - Stable parameter ID registry

- Created `phase3/plocks-effects` from the complete Phase 2 handoff and opened Phase 3 parent issue #20.
- Frozen D027 before source implementation: `FmtParamId` is exactly one byte with explicit NONE `0x00`, INVALID `0xFF` and non-overlapping namespaces for common voice, 8-BIT, 16-BIT, track/mixer, channel/effect, GLOBAL/master and future IDs.
- Assigned only four initial semantic IDs: VOICE LEVEL `0x01`, FINE PITCH `0x02`, TRACK LEVEL `0xA0`, TRACK PAN `0xA1`.
- Kept tracker effect-command IDs as a separate domain and reserved `0xE0..0xEF` for future GLOBAL/master identity without creating global automation storage.
- Added one authoritative stateless registry/API plus exhaustive host tests over all 256 possible ID bytes.
- Unassigned IDs inside valid namespaces remain safely unassigned/ non-lockable; namespace membership never creates semantics.
- Compiled the new registry translation unit with the real pinned JieLi `pi32v2` target compiler while deliberately leaving it unlinked from `dinghy.c` until WP3.2 consumes IDs in actual lock storage.
- Preserved every Phase 2 unit/full-app/display/audio regression and the frozen WAV SHA-256 `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`.
- Device runtime remains byte-identical to Phase 2: 59,252 B image, 12,464 B normal RAM, 96,616 B pool, delta **0 / 0 / 0 B**.
- Final artifacts remain `fm1tracker.bin` `34d0e8c4344e091704608e08357964e7ae0b18673dd0b93f36c4e5bcc2cb4c7c` and `fm1tracker-0.1.fwsc` `3cfd0a2da3836720d2f921b04f29e93c16ca83ab18ab7242c99f2c333d890ca7`.
- Final verification: Actions run `40025790828`, job `120661526848`; evidence artifact ID `12533180473`, ZIP SHA-256 `fa28342bf1fcdf06fb168c8c19b5459afdc6e515bdced044c1f9761efecaee4b`.
- Detailed evidence: `docs/WP3_1_VERIFICATION.md`.

### Next

**WP3.2 - sparse four-lock-per-step storage**, issue #21, is READY TO START. D028 must freeze the concrete record/allocator/pool design before source. No WP3.2 implementation has begun.
