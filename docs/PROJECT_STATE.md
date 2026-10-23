# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and relevant source/history before changing code.

## Current phase

**Phase 1 - clean Dinghy baseline**

## Current milestone

**WP1.4 - FM1Tracker identity rename**

State: **READY TO START**

GitHub issue: #6

Intended working branch: `phase1/fm1tracker-identity`

## Current implementation state

The repository now contains a minimal, audited Dinghy-derived FM-1 platform baseline pinned to upstream commit:

`d44df7db09b98ccad2f1233b8f83f5f09c04829e`

No tracker engine, 8-BIT engine, 16-BIT FM4 engine, STEP REC implementation, p-lock implementation, song mode or Furnace code has been added yet.

The imported application is still the pristine Dinghy app and still uses Dinghy names/artifact identity. That is intentional until WP1.4.

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

Phase 1 is not finished.

WP1.4 must:

1. choose a currently unused FM-1 package identity after checking the current firmware ecosystem/Firmware Hub;
2. rename app/product/build artifact identity from Dinghy to FM1Tracker;
3. replace inherited visible Dinghy identity with project-owned minimal identity;
4. keep rescue/update/HAL/platform behavior unchanged;
5. keep the pristine test-tone behavior unchanged;
6. run the host harness again after the rename.

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
- No Furnace source import without a fresh exact-file/license audit.
- No floating point in FM-1 firmware DSP.
- Avoid runtime 64-bit division.
- Audio callback period is about 5.8 ms for 256 frames at 44.1 kHz; target below half-budget where practical once DSP work begins.
- V0.1 remains 8 tracks, 64 steps/pattern, 16 patterns, max four p-locks/step.
- STEP REC, REST and p-lock editing remain core future product behavior.
- No PCM/sample subsystem before the tracker core is proven.
- Provisional hardware mappings remain provisional until physical FM-1 UX validation.

## Next recommended step

Start WP1.4 on `phase1/fm1tracker-identity`.

First inspect the **current** FM-1 firmware ecosystem/Firmware Hub to choose a non-colliding package identity. Then perform the smallest identity-only rename needed for FM1Tracker and rerun the host harness. Do not start tracker code in the same change.

## Verification references

- `WP1_3_VERIFICATION.md`
- `TEST_MATRIX.md`
- `DINGHY_IMPORT_MANIFEST.md`
- `THIRD_PARTY.md`
- issue #5 - WP1.3 (complete)
- issue #6 - WP1.4 (next)

## Last relevant commits

- pristine code verification target: `21cb59ee455205a8243fee381e24eda517da2c23`
- later commits after that point are project-state/documentation handoff updates only, unless Git history shows otherwise.

## Last updated

2026-10-23 - WP1.3 closed from verified CI evidence; WP1.4 is the exact continuation point for the next chat.
