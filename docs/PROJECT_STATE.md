# FM1Tracker Project State

This is the **single source of truth for current execution state**.

Update this file whenever work starts, stops, is blocked, or completes. A new chat/session should read this file first, then `WORK_PACKAGES.md`, then the active package documents/issues.

## Current phase

**Phase 1 — clean Dinghy baseline**

## Active work package

**WP1.2 — minimal complete Dinghy import**

State: **IN PROGRESS**

GitHub issue: #4

### Already complete

- Phase 0 architecture frozen and merged.
- Dinghy selected as platform baseline.
- Dinghy upstream pinned to commit `d44df7db09b98ccad2f1233b8f83f5f09c04829e`.
- WP1.1 provenance freeze completed.
- `THIRD_PARTY.md` records upstream provenance and external SDK inputs.
- `docs/UPSTREAM_DINGHY.md` records baseline policy.
- `docs/DINGHY_IMPORT_MANIFEST.md` records exact import paths, upstream blob IDs, exclusions and verification procedure.
- Dinghy logo explicitly excluded from FM1Tracker.
- Furnace remains reference/interoperability only; no Furnace code imported.
- Phase 1 is split into issues #3–#7 under parent issue #2.

### Current objective

Perform a mechanical, reviewable import of the pinned Dinghy build/platform baseline with **zero FM1Tracker tracker or synth changes**.

### Immediate next actions

1. Copy files listed as import targets in `DINGHY_IMPORT_MANIFEST.md` from the pinned commit.
2. Preserve all upstream SPDX/copyright headers.
3. Include root GPL-3.0-only license text and Terminus OFL license.
4. Do not import Dinghy logo/branding assets or unnecessary Dinghy product docs/screenshots.
5. Compare the imported tree against the manifest and pinned upstream.
6. Update `THIRD_PARTY.md` to `baseline imported, pristine verification pending`.
7. Close WP1.2 only when the import itself is audit-ready.
8. Then start WP1.3: pristine host/device build verification before any renaming.

## Current constraints that must not be forgotten

- No floating point in FM-1 firmware DSP.
- Avoid runtime 64-bit division.
- Audio callback total period is about 5.8 ms for 256 frames at 44.1 kHz; normal design target is below half that budget where practical.
- Platform plumbing should remain upstream-like until Phase 1 is verified.
- No tracker implementation before the pristine baseline has been validated or a precise external blocker is documented.
- No FM1Tracker identity rename during WP1.2; pristine verification comes first.
- No PCM/sample subsystem before tracker core is proven.
- V0.1: 8 tracks, 64 steps/pattern, 16 patterns, max four p-locks/step.
- STEP REC, REST, and parameter locks are core product behavior, not optional polish.
- Provisional control choices remain provisional until physical hardware UX validation.

## Blockers

None intrinsic to WP1.2.

Potential build-environment blockers for WP1.3/WP1.5:
- JieLi AC79 toolchain availability;
- required external AC79 SDK build inputs;
- no physical FM-1 available to the project owner yet, so hardware-only verification must be clearly separated from host verification.

## Definition of next milestone

**WP1.2 complete** means the repository contains the exact minimal pinned Dinghy baseline specified by the manifest, with licensing intact and no behavior changes.

**Phase 1 complete** means FM1Tracker builds/runs as a renamed clean Dinghy-derived firmware, provenance is complete, and baseline memory/code/timing numbers are captured. Only then does Phase 2 tracker implementation start.

## Last updated

2026-10-23 — WP1.1 completed; WP1.2 active.
