# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and relevant source/history before changing code.

## Current phase

**Phase 1 - clean Dinghy baseline**

## Current milestone

**WP1.5 - renamed baseline verification and budget capture**

State: **IN PROGRESS**

GitHub issue: #7

Working branch: `phase1/fm1tracker-identity`

WP1.5 start commit:

`57617e288972a476fd221ae379e7bb75853b7451`

## Current implementation state

WP1.1 through WP1.4 are complete. The repository contains the pinned Dinghy-derived FM-1 baseline from upstream commit `d44df7db09b98ccad2f1233b8f83f5f09c04829e` plus an identity-only FM1Tracker layer.

Current identity state:

- visible app name: `FM1TRACKER`;
- fixed package identity: `FM-1_987`;
- device build entry point: `build.sh` -> `tools/build_fm1tracker.py` -> unchanged Dinghy build driver;
- host artifacts: `build/host/fm1tracker.wav` and `fm1tracker-*.png`;
- WP1.4 host WAV is bit-identical to the pristine WP1.3 WAV;
- protected app/platform/HAL/loader/build-driver implementation remained unchanged during WP1.4.

No tracker engine, STEP REC implementation, p-lock implementation, song mode, 8-BIT engine, 16-BIT FM4 engine or Furnace source has been added.

## WP1.5 objective

Establish the clean renamed FM1Tracker baseline as the measurable reference that Phase 2 will build on.

Required outputs:

1. confirm/reuse the renamed host verification;
2. run the renamed firmware/package build and inherited safety checks if a legitimate external AC79 SDK checkout can be supplied;
3. capture code size, normal RAM usage and pool RAM usage from the renamed device build when possible;
4. capture baseline audio callback timing at the highest honest verification level available;
5. update reproducible setup/build instructions;
6. record unavailable measurements explicitly rather than substituting invented estimates.

## WP1.5 non-goals

- no tracker/sequencer data structures;
- no transport clock;
- no STEP REC/REST implementation;
- no p-lock implementation;
- no 8-BIT or 16-BIT engine work;
- no HAL/loader/rescue/USB redesign;
- no vendoring or fabricating third-party SDK files merely to make CI green.

## Files expected to change

Only verification/setup/project-memory material unless a build-only fix is genuinely required:

- `README.md` / setup documentation;
- `docs/PROJECT_STATE.md`;
- `docs/TEST_MATRIX.md`;
- `docs/ROADMAP.md` / `docs/WORK_PACKAGES.md` at closeout;
- a dedicated WP1.5 verification/budget record;
- CI verification workflow if needed;
- `THIRD_PARTY.md` only if external SDK provenance is clarified.

The protected Dinghy app/platform/HAL/loader behavior should not change in WP1.5.

## Required verification

WP1.5 should capture, separately and honestly:

- renamed host build/run status;
- renamed device build status;
- build safety-check status;
- app image/code size;
- normal `.data + .bss` RAM usage;
- pool RAM usage;
- baseline audio timing measurement or a precise statement of why hardware timing is unavailable;
- physical FM-1 status;
- exact external dependency/provenance for any SDK checkout used.

## Existing verified evidence

### WP1.3 pristine baseline

Actions run `39958478536`:

- pristine host build/run: PASS;
- host audio/screen artifacts: PASS;
- JieLi toolchain acquisition/execution: PASS;
- full device build: blocked by missing external AC79 SDK checkout;
- physical FM-1: not tested.

### WP1.4 FM1Tracker identity

Actions run `39969032366`:

- protected baseline unchanged: PASS;
- identity wrapper configuration: PASS;
- FM1Tracker host build/run: PASS;
- visible `FM1TRACKER` screen identity: PASS;
- host audio bit-identical to WP1.3: PASS;
- package identity `FM-1_987`: selected after live Firmware Hub collision check;
- full renamed device build: not completed because the external AC79 SDK checkout remains unavailable.

Exact evidence: `docs/WP1_3_VERIFICATION.md` and `docs/WP1_4_VERIFICATION.md`.

## Active blocker to investigate first

The pinned build expects a separately obtained JieLi SDK checkout:

`AC79NN_SDK_V1.2.1_2023-12-13`

Expected files/hashes are frozen in `docs/DINGHY_IMPORT_MANIFEST.md` and `THIRD_PARTY.md`.

WP1.5 must first determine whether an exact, legitimate external source for that SDK is available in the current environment. Do not copy it into the FM1Tracker repository. If no trustworthy source is available, keep the blocker explicit and complete all remaining host/static measurements that are still meaningful.

## Important decisions / do not regress

- D016: fixed package identity is `FM-1_987`; release version lives in `APP_VER` and artifact filenames.
- D017: full renamed device build/resource capture belongs to WP1.5.
- Do not modify proven HAL, loader, rescue/update or USB behavior without a separately justified need.
- Keep the baseline sine/test-tone behavior unchanged through Phase 1.
- Do not start tracker implementation before WP1.5 is complete or explicitly superseded by a recorded decision.
- No Furnace source import without a fresh exact-file/license audit.
- No floating point in FM-1 firmware DSP; avoid runtime 64-bit division.

## Next recommended step

Inspect the pinned Dinghy upstream documentation/history and current public sources for the exact AC79 SDK acquisition/provenance path. If an exact legitimate checkout can be used externally, wire it into a one-off/CI build without committing SDK material. If not, document the blocker and continue with host/static baseline measurements.

## Last relevant commits

- pristine code verification target: `21cb59ee455205a8243fee381e24eda517da2c23`
- WP1.4 branch start: `ff0a095b2384532f19724204b9059e88aac5a52c`
- WP1.4 verified implementation: `78a8f70879a2daec1ad5f99f95fbe5cfbdd5bba3`
- WP1.5 start: `57617e288972a476fd221ae379e7bb75853b7451`

## Last updated

2026-10-23 - WP1.5 started. First task is external SDK provenance/build-environment audit; no tracker implementation is permitted.
