# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, `D047_MIXED_ENGINE_ARCHITECTURE.md`, `CURRENT_DECISIONS.md`, `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `CURRENT_TEST_STATE.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - Phase 4 / WP4.10 / D046 COMPLETE at host/device-build level; WP5.0 is the only active package - 2026-10-24**

Current work package: **WP5.0 - mixed-engine routing contract + migration**  
Current branch: `phase4/8bit-engine` until the WP5.0 branch/handoff is created under the execution protocol.

WP5.0 must freeze its contract before any FM4 production implementation begins.

## Phase status summary

### Phase 0 - foundation - COMPLETE

The hardware-first tracker boundary, eight logical tracks, 64-step patterns, 16 pattern slots, STEP REC and first-class p-lock direction were established. D004 originally restricted V0.1 to a project-wide 8-BIT or 16-BIT sound mode. D047 supersedes only that project-wide-mode restriction for future work.

### Phase 1 - clean Dinghy baseline - COMPLETE

Pinned Dinghy/Felucca/SLOOP provenance, fixed `FM-1_987` identity, pinned JieLi/AC79 build inputs and baseline resource/safety checks are verified.

### Phase 2 - tracker core - COMPLETE

The current core model is proven at host/device-build level:

- 8 tracks x 64 steps x 16 patterns;
- `FmtStep` exactly 8 B;
- one 65,896 B pool-backed `FmtSong`;
- exact integer D020 transport;
- deterministic NOTE/REST/TIE/OFF STEP REC behavior;
- direct page/track selection prototypes;
- bounded tracker screen and full Phase 2 integration.

Final Phase 2 checkpoint: `7063be51e53c2657178084b3bef5ace813dafcd7`.

### Phase 3 - p-locks/effects/runtime ingress - COMPLETE

D027-D036 are complete. The project has stable one-byte semantic parameter IDs, one global 2,048-record sparse lock pool, held-step edit/browse/delete, row-local playback snapshots, exact six-tick tracker effects, NOTE DELAY/RETRIG/CUT scheduling, a bounded 32-entry SPSC audition queue, bounded UI feedback and full worst-case Phase 3 closeout.

Final verified Phase 3 footprint:

- app image: 66,960 B / 581,564 B
- normal RAM: 13,296 B / 98,304 B
- pool RAM: 113,000 B / 344,064 B
- package identity: `FM-1_987`

Final Phase 3 closeout:

- production baseline `354bf7d356774ad828f1f660bd456dbab7f0490d`
- workflow checkpoint `e90d0232c179ea92e2295171403697ceaf5e3ecf`
- run `40084706845`
- job `120845144453`
- evidence `docs/WP3_10_VERIFICATION.md`

### Phase 4 - first engine family: 8-BIT/chip - COMPLETE

WP4.1 through WP4.10 are complete at host/device-build level.

The first complete engine family contains:

- PULSE - D038
- NOISE - D039
- TRIANGLE - D040
- WAVETABLE-8 - D041
- common integer ADSR - D042
- deterministic eight-slot allocator / detached-tail policy - D043
- engine parameter pages + six stable D044 parameter IDs - D044
- linked all-eight-track production integration - D045
- representative/full-pool/realtime/target Phase 4 exit verification - D046

Final production baseline remains the D045 Gate-4 production checkpoint:

`c93c39ad344760b4da0dfbc76829ca5fef886174`

D046 found no production defect and changed no production source.

Final WP4.10 / D046 closeout:

- canonical verification head `2379d3e7e52d1cefe9f845f8287ad0ccda8c019a`
- run `40124536375`
- job `120963022289`
- artifact `wp4-10-d046-phase4-exit`, ID `12571958995`
- artifact digest `sha256:0cefb815415f12c88e45ad3477621e0805e67ff3b928ad142e2fd3c8835c8915`
- evidence `docs/WP4_10_VERIFICATION.md`
- representative WAV SHA-256 `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9`
- 4,096-row / 64-wrap repeat-identical linked long run
- zero active cross-track steals
- inherited regressions green
- structural realtime audit green
- pinned target identity/resources/artifacts exact
- `hardware_margin_claim=none`

Final linked target footprint:

- image 72,504 B, D037 margin 59,992 B
- normal RAM 19,604 B, D037 margin 1,884 B
- pool RAM 113,000 B, D037 margin 16,384 B
- `fm1tracker.bin` SHA-256 `214fba8ec0af4be3120bc91dafbaa24e7d4d8a5a6de7395de2473f12374c4215`
- `fm1tracker-0.1.fwsc` SHA-256 `20f0bbe556edcf6a00118f9983335913dda2f29fc64d3c87997bfd6756bcf119`
- normal WP4.9 linked default WAV `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`

## Active work package - WP5.0

Normative architecture direction: `docs/D047_MIXED_ENGINE_ARCHITECTURE.md`.

WP5.0 is a routing/migration contract package. It must be completed before FM4 is linked into tracker playback.

WP5.0 must freeze:

- per-instrument family + engine as authoritative routing identity;
- the new non-gating meaning of `FmtSongMeta.mode` while preserving compatibility until an explicit persistence migration exists;
- simultaneous different engine families across logical tracks;
- deterministic cross-family instrument switching on a track;
- at most one owned primary sequenced voice per logical track across all families;
- no active cross-track primary stealing;
- bounded static family/engine dispatch, with no dynamic plugin architecture;
- measured cross-family allocator/ownership representation;
- common publication/parameter adapter boundary;
- exact 8-BIT regression equivalence under the new router;
- the migration/test strategy before any production source change.

### Exact next execution action

1. Create/freeze the WP5.0 normative contract under D047 before changing production source.
2. Audit the existing D045/D046 routing assumptions against the contract, especially `FmtSongMeta.mode`, `FmtInstrumentHeader`, instrument inheritance and allocator ownership.
3. Define focused host tests that fail under the current project-wide family gate and prove the intended mixed-family router semantics.
4. Only after the contract and tests are frozen, begin the smallest bounded production migration.
5. Keep the complete all-8-BIT D046 path as an explicit regression baseline throughout WP5.0.

Do not jump directly to FM4 oscillator/operator implementation.

## D047 - accepted architecture direction

FM1Tracker is **one engine-agnostic tracker/sequencer**.

The project itself is not globally 8-BIT or 16-BIT. Each instrument owns an engine family + engine identity. Tracks/rows reference instruments, allowing future mixed configurations subject to measured CPU/RAM/voice budgets and explicit engine work packages.

The existing model already provides the key indirection:

- `FmtStep.instrument` remains one byte;
- `FmtTrackConfig.instrument` supplies a track default;
- `FmtInstrumentHeader` remains exactly 4 B with `mode`, `engine`, `version`, `flags`;
- D027 parameter IDs remain semantic and engine-independent at the tracker-storage layer;
- D020/D031/D032/D033/D034 timing is independent of oscillator family;
- D045 already defines deterministic instrument/engine identity-change reset behavior.

D047 does not rewrite historical Phase 1-4 evidence. The complete 8-BIT path remains the first proven engine family and the regression baseline for mixed-engine routing.

## Phase 5 sequence after WP5.0

- WP5.1 FM operator primitive
- WP5.2 curated FM algorithm routing
- WP5.3 ratio table
- WP5.4 levels/modulation index
- WP5.5 bounded feedback
- WP5.6 FM envelope strategy
- WP5.7 FM voice allocation/integration
- WP5.8 FM parameter pages + p-lock mapping
- WP5.9 mixed 8-BIT + FM4 pitch/modulation/instrument-switch validation
- WP5.10 worst-case mixed-engine benchmark and Phase 5 exit

Modern VA/supersaw/wavetable/drum engines are architecturally allowed later, but are not silently promoted into the current Phase 5 scope.

## Important invariants - do not regress

- D005: 8 logical tracks x 64 steps x 16 patterns.
- D018: `FmtStep` exactly 8 B.
- D020/D021: exact integer transport and next-frame row-boundary ordering.
- D025: explicit NONE/NOTE/TIE/OFF semantics.
- D027: parameter IDs remain semantic and separate from tracker-effect command IDs.
- D028-D030: bounded, corruption-safe p-lock storage/editor behavior.
- D031: common VOICE LEVEL/FINE/TRACK LEVEL row snapshots; TRACK PAN currently storage-only.
- D032: one transport-derived exact six-tick effect clock; no second tracker clock.
- D033: bounded RETRIG/NOTE DELAY/NOTE CUT scheduling; no general queue dependency.
- D034: exact 32-entry SPSC main->audio audition queue, reject-newest overflow and fail-to-silence recovery.
- D035: bounded main-loop-only tracker presentation.
- D036: Phase 3 worst-case exit remains the inherited tracker/runtime baseline.
- D037-D046: complete 8-BIT family implementation and Phase 4 exit evidence remain authoritative for that family.
- D043: still-owned active cross-track primary voices may not be stolen.
- D044: assigned 8-BIT parameter IDs `0x20..0x25` remain stable.
- D047: no project-wide engine-family restriction in future architecture; instrument is the routing authority.
- No float in firmware DSP and no runtime 64-bit division.
- Eight logical tracks are not silently reduced to obtain polyphony or expensive engines.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted core direction. The target experience is tracker depth underneath and immediate performance/capture on top.

The mixed-engine architecture strengthens that direction: a live performance setup may use different engine families for drums, bass, poly, lead and flexible tracks without changing the tracker model.

## Hardware limitation still open

No physical FM-1 is currently available. Phase 4 host/device-build evidence does not claim physical install/boot behavior, physical sound quality, control feel, screen feel or measured AC791N callback execution margin.

Physical execution-margin and UX validation remain explicit later gates.

## Last relevant checkpoints

- Phase 2 verified: `7063be51e53c2657178084b3bef5ace813dafcd7`
- Phase 3 verified production: `354bf7d356774ad828f1f660bd456dbab7f0490d`
- Phase 3 closeout: `e90d0232c179ea92e2295171403697ceaf5e3ecf`, run `40084706845`
- WP4.9 D045 contract: `fd31801b8a73e27345d4575026b593fb45e10c20`
- WP4.9 final closeout: `89fabc51348078101c8e376bba7814af9d42ae80`, run `40116897104`
- WP4.10 D046 contract: `aaceafc24322611bf00255bf028466d7d88643de`
- WP4.10 focused realtime audit: `c8acea33dfbb942dc26527ea2fb9517cb2a6c1fe`, run `40120436446`
- WP4.10 final contract-complete verification: `2379d3e7e52d1cefe9f845f8287ad0ccda8c019a`, run `40124536375`, job `120963022289`
- WP4.10 evidence: `docs/WP4_10_VERIFICATION.md`
- D047 decision contract: `docs/D047_MIXED_ENGINE_ARCHITECTURE.md`

## Last updated

2026-10-24 - WP4.10 / D046 and Phase 4 closed at host/device-build level. Production remains frozen to the D045 baseline. WP5.0 mixed-engine routing contract + migration is now the only active package. Physical FM-1 execution-margin and UX validation remain open future gates.
