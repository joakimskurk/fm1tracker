# FM1Tracker Project State

This is the **single source of truth for current execution state**.

Update this file whenever work starts, stops, is blocked, or completes. A new chat/session should read this file first, then `WORK_PACKAGES.md`, then the active package documents/issues.

## Current phase

**Phase 1 — clean Dinghy baseline**

## Active work package

**WP1.3 — pristine baseline verification**

State: **IN PROGRESS**

GitHub issue: #5

### Already complete

- Phase 0 architecture frozen and merged.
- Dinghy selected as platform baseline.
- Dinghy upstream pinned to commit `d44df7db09b98ccad2f1233b8f83f5f09c04829e`.
- WP1.1 provenance freeze completed.
- WP1.2 minimal complete Dinghy import completed on 2026-10-23.
- Every imported manifest file was compared against the pinned upstream Git blob SHA and Git mode in GitHub Actions; all matched.
- Root GPL-3.0-only license and Terminus OFL material are present and byte-identical to upstream.
- Executable bits for upstream scripts are preserved.
- Dinghy logo/branding assets are absent.
- Furnace remains reference/interoperability only; no Furnace code imported.
- The import-manifest typo for `firmware/hal/fm1_uart.h` was corrected to the actual pinned blob SHA `5d5ef83dad665f239df83bb8fbfd4d405e28fbfb`.

### Current objective

Prove the pristine imported baseline before **any** FM1Tracker identity rename or tracker/synth behavior change.

### Immediate next actions

1. Run the pristine host test harness and preserve the exact result.
2. Verify expected host WAV/screen artifacts where the harness produces them.
3. Attempt the pristine firmware build without vendoring external SDK files.
4. If the full firmware build is blocked by the external JieLi SDK/toolchain, record the exact missing dependency and keep that separate from source correctness.
5. Do not rename `dinghy.c`, package identity, product IDs or artifacts until WP1.3 is closed.
6. Move to WP1.4 only after the pristine baseline is verified or the external build blocker is precisely documented.

## Current constraints that must not be forgotten

- No floating point in FM-1 firmware DSP.
- Avoid runtime 64-bit division.
- Audio callback total period is about 5.8 ms for 256 frames at 44.1 kHz; normal design target is below half that budget where practical.
- Platform plumbing should remain upstream-like until Phase 1 is verified.
- No tracker implementation before the pristine baseline has been validated or a precise external blocker is documented.
- No FM1Tracker identity rename during WP1.3; pristine verification comes first.
- No PCM/sample subsystem before tracker core is proven.
- V0.1: 8 tracks, 64 steps/pattern, 16 patterns, max four p-locks/step.
- STEP REC, REST, and parameter locks are core product behavior, not optional polish.
- Provisional control choices remain provisional until physical hardware UX validation.

## Blockers

None for host verification.

Potential device-build/environment blockers:
- JieLi `pi32v2` toolchain availability;
- required external AC79 SDK build inputs;
- no physical FM-1 available to the project owner yet, so hardware-only verification must be clearly separated from host verification.

## Definition of next milestone

**WP1.3 complete** means the pristine host baseline is verified and the device build is either verified or has a precisely documented external dependency blocker.

**Phase 1 complete** means FM1Tracker builds/runs as a renamed clean Dinghy-derived firmware, provenance is complete, and baseline memory/code/timing numbers are captured. Only then does Phase 2 tracker implementation start.

## Last updated

2026-10-23 — WP1.2 completed with exact blob/mode verification; WP1.3 active.
