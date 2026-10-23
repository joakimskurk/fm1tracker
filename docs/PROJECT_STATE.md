# FM1Tracker Project State

This is the **single source of truth for current execution state**.

Update this file whenever work starts, stops, is blocked, or completes. A new chat/session should read this file first, then `WORK_PACKAGES.md`, then the active package documents/issues.

## Current phase

**Phase 1 — clean Dinghy baseline**

## Active work package

**WP1.1 — upstream provenance freeze**

State: **IN PROGRESS**

### Already complete

- Phase 0 architecture frozen and merged.
- Dinghy selected as platform baseline.
- Dinghy upstream pinned to commit `d44df7db09b98ccad2f1233b8f83f5f09c04829e`.
- `THIRD_PARTY.md` created.
- `docs/UPSTREAM_DINGHY.md` created.
- Dinghy logo explicitly excluded from FM1Tracker.
- Furnace remains reference/interoperability only; no Furnace code imported.

### Current objective

Finish the provenance/import manifest needed for a reviewable minimal Dinghy import.

### Immediate next actions

1. Inventory the exact upstream files required for host + firmware build.
2. Record which files are copied verbatim and which will later be modified.
3. Include required third-party license files for any generated/font material retained.
4. Mark WP1.1 complete only when the import can be audited without guessing.
5. Begin WP1.2: minimal complete Dinghy import, still with zero tracker changes.

## Current constraints that must not be forgotten

- No floating point in FM-1 firmware DSP.
- Avoid runtime 64-bit division.
- Audio callback total period is about 5.8 ms for 256 frames at 44.1 kHz; normal design target is below half that budget where practical.
- Platform plumbing should remain upstream-like until Phase 1 is verified.
- No tracker implementation before the pristine baseline has been validated or a precise external blocker is documented.
- No PCM/sample subsystem before tracker core is proven.
- V0.1: 8 tracks, 64 steps/pattern, 16 patterns, max four p-locks/step.
- STEP REC, REST, and parameter locks are core product behavior, not optional polish.
- Provisional control choices remain provisional until physical hardware UX validation.

## Blockers

None intrinsic to project direction.

Potential build-environment blockers for WP1.3/WP1.5:
- JieLi AC79 toolchain availability;
- required external AC79 SDK build inputs;
- no physical FM-1 available to the project owner yet, so hardware-only verification must be clearly separated from host verification.

## Definition of next milestone

**Phase 1 complete** means FM1Tracker builds/runs as a renamed clean Dinghy-derived firmware, provenance is complete, and baseline memory/code/timing numbers are captured. Only then does Phase 2 tracker implementation start.

## Last updated

2026-10-23 — execution framework created; WP1.1 active.
