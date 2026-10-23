# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and relevant source/history before changing code.

## Current phase

**Phase 1 - clean Dinghy baseline**

## Current milestone

**WP1.5 - renamed baseline verification and budget capture**

State: **READY TO START**

WP1.4 issue: #6 - identity work complete; closeout/issue closure should reference `docs/WP1_4_VERIFICATION.md`.

Current working branch: `phase1/fm1tracker-identity`

## Current implementation state

The repository contains the pinned Dinghy-derived FM-1 platform baseline from upstream commit:

`d44df7db09b98ccad2f1233b8f83f5f09c04829e`

WP1.1 through WP1.4 are complete.

FM1Tracker identity is now applied without modifying the verified DSP/platform implementation:

- visible app name: `FM1TRACKER`;
- fixed package identity: `FM-1_987`;
- device build entry point: `build.sh` -> `tools/build_fm1tracker.py` -> unchanged Dinghy build driver;
- host artifacts: `build/host/fm1tracker.wav` and `fm1tracker-*.png`;
- final device artifacts are intended to be `fm1tracker.bin` and `fm1tracker*.fwsc` once the external SDK allows a full build.

No tracker engine, STEP REC implementation, p-lock implementation, song mode, 8-BIT engine, 16-BIT FM4 engine or Furnace source has been added.

## WP1.4 verification result

GitHub Actions run:

`39969032366`

Verified implementation commit:

`78a8f70879a2daec1ad5f99f95fbe5cfbdd5bba3`

Results:

- protected baseline app/platform/HAL/loader/build code unchanged from WP1.4 start: **PASS**;
- FM1Tracker identity wrapper syntax/configuration: **PASS**;
- host compile/run: **PASS**;
- visible screen identity `FM1TRACKER`: **PASS**;
- host WAV produced: **PASS**;
- host WAV is bit-identical to WP1.3: **PASS**;
- fixed identity `FM-1_987`: documented and checked against the live Firmware Hub identity list on 2026-10-23;
- complete renamed device firmware/package build: **NOT COMPLETED** because the separately required AC79 SDK checkout is still unavailable;
- physical FM-1: **NOT TESTED** because hardware is not currently available.

Exact hashes and evidence: `docs/WP1_4_VERIFICATION.md`.

## Active blocker

Full device firmware/package verification requires a separately obtained JieLi AC79 SDK checkout expected by the pinned Dinghy build:

`AC79NN_SDK_V1.2.1_2023-12-13`

The repository must not vendor or fabricate those SDK files simply to bypass setup. WP1.3 already verified Dinghy's JieLi toolchain installer and clang execution; the unresolved external dependency is the SDK checkout.

## Important decisions / do not regress

- D016: fixed package identity is `FM-1_987`; release version lives in `APP_VER` and artifact filenames.
- D017: WP1.4 closes on verified identity/host evidence; full renamed device build and resource capture belong to WP1.5.
- Do not modify proven HAL, loader, rescue/update or USB behavior without a separately justified need.
- Keep the baseline sine/test-tone behavior unchanged through Phase 1.
- Do not start tracker implementation before WP1.5 is complete or explicitly superseded by a recorded decision.
- No Furnace source import without a fresh exact-file/license audit.
- No floating point in FM-1 firmware DSP; avoid runtime 64-bit division.
- V0.1 remains 8 tracks, 64 steps/pattern, 16 patterns, max four p-locks/step.

## Current work item

WP1.5 must establish the renamed baseline as the measurable reference for all later work.

Required outputs:

1. reuse/confirm the renamed host run;
2. attempt the full renamed firmware/package build with the verified JieLi toolchain and a legitimate external AC79 SDK checkout if available;
3. capture code size, normal RAM and pool RAM from the device build when possible;
4. capture baseline audio callback timing where measurable without claiming hardware measurements that were not made;
5. update reproducible setup/build instructions;
6. record every unavailable measurement explicitly rather than inventing estimates.

## Next recommended step

Start WP1.5 by auditing the available build environment and locating a legitimate, separately usable source for the required `AC79NN_SDK_V1.2.1_2023-12-13` checkout. Do **not** import SDK material into the repository. If the SDK cannot be obtained in the current environment, keep that blocker explicit and complete every host/static baseline measurement that is still meaningful.

## Verification references

- `docs/WP1_3_VERIFICATION.md`
- `docs/WP1_4_VERIFICATION.md`
- `docs/TEST_MATRIX.md`
- `docs/DINGHY_IMPORT_MANIFEST.md`
- `THIRD_PARTY.md`
- issue #6 - WP1.4

## Last relevant commits

- pristine code verification target: `21cb59ee455205a8243fee381e24eda517da2c23`
- WP1.4 branch start: `ff0a095b2384532f19724204b9059e88aac5a52c`
- WP1.4 verified implementation: `78a8f70879a2daec1ad5f99f95fbe5cfbdd5bba3`

## Last updated

2026-10-23 - WP1.4 completed and verified; WP1.5 is the exact continuation point. No tracker implementation is permitted yet.
