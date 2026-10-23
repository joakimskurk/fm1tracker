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

### Next

**WP2.1 — compile-time tracker data model.** Implement only the approved V0.1 constants/structures/accessors/static size assertions and host/device compile tests before any transport, UI or audio behavior is added.
