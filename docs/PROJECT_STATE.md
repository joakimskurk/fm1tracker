# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, `D047_MIXED_ENGINE_ARCHITECTURE.md`, `WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md`, `CURRENT_DECISIONS.md`, `WORK_PACKAGES.md`, `EXECUTION_PROTOCOL.md`, `CURRENT_TEST_STATE.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 / D048 contract frozen; pre-migration test evidence is next - 2026-10-24**

Current work package: **WP5.0 - mixed-engine routing contract + migration**  
Parent issue: **#42**  
Work-package issue: **#43**  
Current branch: `phase5/mixed-engine-routing`  
Phase 5 branch point: `d56505ac15c233eb806c567b6a5944bccf02ec1e`  
D048 contract commit: `166dbf8272e7323aea2abe16c33544edba0b456f`

No WP5.0 production source has been changed yet. The next allowed work is pre-migration host/test harness evidence only.

## Phase status summary

### Phase 0 / 1 - foundation and clean Dinghy baseline - COMPLETE

Pinned Dinghy/Felucca/SLOOP provenance, fixed `FM-1_987` identity, pinned JieLi/AC79 build inputs and baseline resource/safety checks are verified.

### Phase 2 - tracker core - COMPLETE

The core model is proven at host/device-build level:

- 8 tracks x 64 steps x 16 patterns;
- `FmtStep` exactly 8 B;
- one 65,896 B pool-backed `FmtSong`;
- exact integer D020 transport;
- deterministic NOTE/REST/TIE/OFF STEP REC behavior;
- provisional step-page/track-selection controls;
- bounded tracker screen.

Final Phase 2 checkpoint: `7063be51e53c2657178084b3bef5ace813dafcd7`.

### Phase 3 - p-locks/effects/runtime ingress - COMPLETE

D027-D036 are complete: stable semantic parameter IDs, 2,048-record sparse p-lock pool, held-step edit/browse/delete, row-local publication snapshots, exact six-tick tracker effects, NOTE DELAY/RETRIG/CUT, bounded 32-entry SPSC audition queue, bounded UI and worst-case closeout.

Final Phase 3 target footprint:

- image 66,960 B
- normal RAM 13,296 B
- pool RAM 113,000 B

Final closeout: `e90d0232c179ea92e2295171403697ceaf5e3ecf`, run `40084706845`, job `120845144453`.

### Phase 4 - first complete 8-BIT engine family - COMPLETE

WP4.1-WP4.10 are complete at host/device-build level.

The family contains PULSE, NOISE, TRIANGLE, WAVETABLE-8, common integer ADSR, deterministic eight-slot ownership/allocation, D044 parameter pages, linked all-eight-track playback and D046 representative/full-pool/realtime/target closeout.

Frozen production baseline:

`c93c39ad344760b4da0dfbc76829ca5fef886174`

Canonical D046 closeout:

- verification head `2379d3e7e52d1cefe9f845f8287ad0ccda8c019a`
- run `40124536375`
- job `120963022289`
- evidence `docs/WP4_10_VERIFICATION.md`
- representative WAV SHA-256 `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9`
- 4,096-row / 64-wrap repeat-identical linked long run
- exact D033/D043 counts
- zero active cross-track steals
- structural realtime audit green
- inherited regressions green
- pinned target identity/resources/artifacts exact
- `hardware_margin_claim=none`

Final linked target footprint:

- image 72,504 B
- normal RAM 19,604 B
- pool RAM 113,000 B
- D037 remaining margin: image 59,992 B / RAM 1,884 B / pool 16,384 B
- BIN SHA-256 `214fba8ec0af4be3120bc91dafbaa24e7d4d8a5a6de7395de2473f12374c4215`
- FWSC SHA-256 `20f0bbe556edcf6a00118f9983335913dda2f29fc64d3c87997bfd6756bcf119`
- normal linked WAV SHA-256 `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`

## Active work package - WP5.0 / D048

Normative documents:

- `docs/D047_MIXED_ENGINE_ARCHITECTURE.md`
- `docs/WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md`

D048 is frozen before production migration.

### D048 implementation contract

- `FmtStep` remains exactly 8 B and continues to reference instruments.
- `FmtInstrumentHeader` remains exactly 4 B; its existing `mode` byte is interpreted as family identity.
- Family compatibility values stay `0 = 8-BIT/chip`, `1 = FM/digital`.
- `FmtSongMeta.mode` remains physically present but is legacy/default-family metadata only and must never gate playback/publication/edit eligibility.
- Audio gets instrument family/engine from a fixed aligned 64 B publication bank mirroring all 16 instrument headers.
- Global primary routing owns exactly one family byte per logical track, 8 B total.
- Family-local engines retain their own voice handles/allocators.
- Cross-family NOTE hard-silences/clears the old primary before the new family is attempted and commits the new route only on success.
- OFF/CUT release through the active family and clear global primary ownership in the same event.
- TIE/EMPTY/RETRIG do not allocate or change family.
- Family dispatch is static and bounded, never a runtime plugin system.
- The existing single 2,048 B row-page publication cache is generalized/reused across families; parallel 2,048 B caches are forbidden by default.
- Non-applicable family-specific p-lock IDs remain stored but are ignored, never reinterpreted.
- WP5.0 contains no FM4 production DSP; mixed-family router behavior is proven using a bounded test-only stub family.

WP5.0 hard incremental ceilings versus D046:

- image +4,096 B maximum, total <= 76,600 B
- normal RAM +256 B maximum, total <= 19,860 B
- pool +0 B, total remains 113,000 B

### Exact next execution action

1. Add a pre-migration host probe that reproduces the current obsolete global `FmtSongMeta.mode` gate without changing production source.
2. Add pure/test-only router semantic tests for the D048 8-byte family-route state and bounded mixed-family stub behavior.
3. Freeze those tests and CI evidence.
4. Only then begin the smallest production migration: generic identity publication, shared page-publication ownership and bounded router integration.
5. Keep the complete D046 all-8-BIT path as an explicit regression baseline throughout WP5.0.
6. Do not begin WP5.1 FM operator code until `docs/WP5_0_VERIFICATION.md` closes WP5.0.

## D047 / D048 precedence

D047 supersedes D004's project-wide 8-BIT/16-BIT restriction for future architecture. D048 is the normative implementation contract for WP5.0.

Historical D037-D046 evidence remains authoritative for the complete 8-BIT family. D047/D048 do not rewrite those checkpoints.

## Phase 5 sequence after WP5.0

- WP5.1 FM operator primitive
- WP5.2 curated FM algorithm routing
- WP5.3 ratio table
- WP5.4 levels/modulation index
- WP5.5 bounded feedback
- WP5.6 FM envelope strategy
- WP5.7 FM voice allocation/integration
- WP5.8 FM parameter pages + p-lock mapping
- WP5.9 mixed 8-BIT + FM4 validation
- WP5.10 worst-case mixed-engine benchmark and Phase 5 exit

Modern VA/supersaw/wavetable/drum engines are architecturally possible later but are not part of the current WP5.0/FM4 scope.

## Important invariants - do not regress

- 8 logical tracks x 64 steps x 16 patterns.
- `FmtStep` exactly 8 B.
- exact integer transport and next-frame row-boundary ordering.
- explicit NONE/NOTE/TIE/OFF semantics.
- semantic parameter IDs remain separate from tracker-effect command IDs.
- bounded corruption-safe p-lock storage/editor behavior.
- D031 common row snapshots and D032 exact six-tick effect clock.
- D033 bounded RETRIG/NOTE DELAY/NOTE CUT scheduling.
- D034 exact 32-entry SPSC audition queue and fail-to-silence recovery.
- D043 still-owned active cross-track primary voices may not be stolen.
- D044 8-BIT IDs `0x20..0x25` remain stable.
- no float in firmware DSP and no runtime 64-bit division.
- eight logical tracks are never silently reduced for polyphony or engine cost.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted core direction: tracker depth underneath, immediate performance/capture on top.

Mixed-engine routing is infrastructure for that product direction, not an excuse to expand WP5.0 into optional synth families or live-capture implementation.

## Hardware limitation still open

No physical FM-1 is currently available. Host/device-build evidence does not prove physical install/boot behavior, physical sound quality, control feel, screen feel or measured AC791N callback margin.

Physical execution-margin and UX validation remain explicit later gates.

## Last relevant checkpoints

- Phase 2 verified: `7063be51e53c2657178084b3bef5ace813dafcd7`
- Phase 3 production: `354bf7d356774ad828f1f660bd456dbab7f0490d`
- Phase 3 closeout: `e90d0232c179ea92e2295171403697ceaf5e3ecf`, run `40084706845`
- WP4.9 final closeout: `89fabc51348078101c8e376bba7814af9d42ae80`, run `40116897104`
- WP4.10 final verification: `2379d3e7e52d1cefe9f845f8287ad0ccda8c019a`, run `40124536375`, job `120963022289`
- Phase 5 branch point: `d56505ac15c233eb806c567b6a5944bccf02ec1e`
- D047: `docs/D047_MIXED_ENGINE_ARCHITECTURE.md`
- D048 contract commit: `166dbf8272e7323aea2abe16c33544edba0b456f`
- D048: `docs/WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md`

## Last updated

2026-10-24 - Phase 5 branch and issues #42/#43 created. D048 WP5.0 mixed-engine routing contract frozen before production migration. Next step is pre-migration test evidence only; production remains at the D045/D046 8-BIT baseline until those tests are frozen.
