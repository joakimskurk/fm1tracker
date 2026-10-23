# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and relevant source/history before changing code.

## Current phase

**Phase 1 - clean Dinghy baseline**

## Current milestone

**WP1.4 - FM1Tracker identity rename**

State: **IN PROGRESS**

GitHub issue: #6

Working branch: `phase1/fm1tracker-identity`

WP1.4 start commit:

`ff0a095b2384532f19724204b9059e88aac5a52c`

## Current implementation state

The repository contains a minimal, audited Dinghy-derived FM-1 platform baseline pinned to upstream commit:

`d44df7db09b98ccad2f1233b8f83f5f09c04829e`

WP1.1-WP1.3 are complete under their documented verification rules. The branch was verified at the WP1.4 start commit before identity work began.

No tracker engine, 8-BIT engine, 16-BIT FM4 engine, STEP REC implementation, p-lock implementation, song mode or Furnace code has been added.

At WP1.4 start, the imported application/build/host artifacts still use Dinghy names. WP1.4 is changing identity only while preserving the pristine test-tone behavior and platform plumbing.

## Active WP1.4 decision

Current Firmware Hub identity evidence was checked on 2026-10-23. `FM-1_987` was not present in the live identity table and has been selected as FM1Tracker's package identity.

Rationale:

- it is unused on the current Firmware Hub list to the best available evidence;
- it remains in the `FM-1_9NN` family used by the Felucca/Dinghy updater path, preserving the existing interrupted-update/resume assumptions;
- the identity will remain stable across FM1Tracker releases; release version stays in `APP_VER` and artifact filenames instead of consuming the package identity.

This decision must also be recorded in `DECISIONS.md` before implementation spreads.

## WP1.4 objective

Make the verified baseline identify as FM1Tracker without changing musical behavior.

Expected identity-only changes:

- rename the app source path from `firmware/src/dinghy.c` to `firmware/src/fm1tracker.c` without changing the test-tone implementation;
- compile the device and host builds with visible app name `FM1TRACKER`;
- rename app/build/host artifacts from `dinghy*` to `fm1tracker*`;
- use fixed package identity `FM-1_987`;
- update affected setup/test documentation and add WP1.4 host verification.

## WP1.4 non-goals

- no tracker data model implementation;
- no STEP REC, REST, p-lock or song implementation;
- no 8-BIT or 16-BIT engine work;
- no HAL, rescue, OTA, USB or loader redesign;
- no test-tone synthesis changes.

## Required WP1.4 verification

Before closeout:

1. host harness builds and runs from `firmware/src/fm1tracker.c`;
2. host WAV and screen PNG artifacts use the `fm1tracker` name;
3. screen identity visibly says `FM1TRACKER`;
4. host WAV remains behaviorally identical to the pristine baseline test tone where identity-only rendering differences cannot affect audio;
5. build tooling emits `fm1tracker.bin` / `fm1tracker*.fwsc` and fixed identity `FM-1_987` by inspection/build where available;
6. no tracker/synth feature code is introduced;
7. device build blocker remains honestly recorded if the external AC79 SDK is still unavailable.

## What works / has been verified

### WP1.1 - provenance

Complete.

- Upstream revision pinned.
- GPL-3.0-only provenance recorded.
- Terminus OFL material recorded.
- Dinghy logo/branding excluded.
- Furnace remains reference/interoperability only; no Furnace source imported.

### WP1.2 - pristine import

Complete.

- Every manifest-selected import path matched pinned upstream Git blob SHA and Git mode.
- Upstream executable bits are preserved.
- Source SPDX/copyright headers are preserved.
- The original manifest typo for `firmware/hal/fm1_uart.h` was corrected to blob `5d5ef83dad665f239df83bb8fbfd4d405e28fbfb`.

### WP1.3 - pristine verification

Complete under its stated exit rule.

Verified code commit:

`21cb59ee455205a8243fee381e24eda517da2c23`

GitHub Actions run:

`39958478536`

Results:

- pristine host compile/run: **PASS**;
- host WAV produced and hashed: **PASS**;
- two host screen PNGs produced and hashed: **PASS**;
- JieLi toolchain download/execution: **PASS** (`jieli-linux-toolchains-20260730.1`, clang 4.0.1);
- device firmware build: **NOT COMPLETED** because the separately required AC79 SDK checkout is absent;
- exact blocker: `build.sh: no JieLi AC79 SDK in ...`;
- physical FM-1: **NOT TESTED** because hardware is not currently available.

Exact hashes and evidence are in `WP1_3_VERIFICATION.md` and the real verification level is mirrored in `TEST_MATRIX.md`.

## Known incomplete work

WP1.4 identity changes and host verification are now active.

WP1.5 will then perform renamed-baseline verification and capture code/RAM/pool/timing budgets. The full device firmware build remains dependent on an external AC79 SDK checkout.

## Known bugs

No FM1Tracker product bugs yet because tracker functionality has not been implemented.

No failure was found in the pristine host baseline during WP1.3.

## Blockers

### External build dependency

Full device firmware build requires the separately obtained JieLi AC79 SDK checkout expected by Dinghy (`AC79NN_SDK_V1.2.1_2023-12-13`). The repository must not vendor those SDK files merely to bypass setup.

The JieLi compiler/toolchain itself has already been verified to install and execute.

### Hardware verification

No physical FM-1 is currently available. Hardware-only verification must remain clearly distinct from host/emulator/build verification.

## Important constraints / do not regress

- No tracker implementation during WP1.4. Identity only.
- Do not alter proven HAL, loader, rescue/update or USB behavior during the rename unless strictly necessary and separately justified.
- Keep the pristine sine/test-tone behavior unchanged.
- No Furnace source import without a fresh exact-file/license audit.
- No floating point in FM-1 firmware DSP.
- Avoid runtime 64-bit division.
- Audio callback period is about 5.8 ms for 256 frames at 44.1 kHz; target below half-budget where practical once DSP work begins.
- V0.1 remains 8 tracks, 64 steps/pattern, 16 patterns, max four p-locks/step.
- STEP REC, REST and p-lock editing remain core future product behavior.
- No PCM/sample subsystem before the tracker core is proven.
- Provisional hardware mappings remain provisional until physical FM-1 UX validation.

## Next recommended step

Record the `FM-1_987` identity decision in `DECISIONS.md`, then perform the smallest identity-only rename in app/build/host paths and rerun the host harness. Do not start tracker code in the same change.

## Verification references

- `WP1_3_VERIFICATION.md`
- `TEST_MATRIX.md`
- `DINGHY_IMPORT_MANIFEST.md`
- `THIRD_PARTY.md`
- issue #5 - WP1.3 (complete)
- issue #6 - WP1.4 (active)

## Last relevant commits

- pristine code verification target: `21cb59ee455205a8243fee381e24eda517da2c23`
- WP1.4 branch start / handoff: `ff0a095b2384532f19724204b9059e88aac5a52c`

## Last updated

2026-10-23 - WP1.4 started after branch/repository verification; package identity `FM-1_987` selected from current Firmware Hub evidence; identity-only implementation is next.
