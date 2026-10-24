# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, `WP5_0_VERIFICATION.md`, `D047_MIXED_ENGINE_ARCHITECTURE.md`, `WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md`, `CURRENT_DECISIONS.md`, `WORK_PACKAGES.md`, `EXECUTION_PROTOCOL.md`, `CURRENT_TEST_STATE.md`, `TEST_MATRIX.md`, and `ROADMAP.md` before changing code.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 complete, WP5.1 contract/test freeze is the only active package - 2026-10-24**

Current work package: **WP5.1 - fixed-point FM operator primitive**  
Parent issue: **#42**  
Work-package issue: **#44**  
Current branch: `phase5/fm4-operator`  
Phase 5 branch point: `d56505ac15c233eb806c567b6a5944bccf02ec1e`  
WP5.0 production verification head: `813792fb25632b4d7acda787422e51f713fad21e`

Do not begin WP5.2 algorithm routing, feedback, envelopes, voice allocation, parameter pages or mixed FM playback while WP5.1 is active.

## Phase status summary

### Phase 0 / 1 - foundation and clean Dinghy baseline - COMPLETE

Pinned Dinghy/Felucca/SLOOP provenance, fixed `FM-1_987` identity, pinned JieLi/AC79 build inputs and baseline resource/safety checks are verified.

### Phase 2 - tracker core - COMPLETE

- 8 tracks x 64 steps x 16 patterns
- `FmtStep` exactly 8 B
- one 65,896 B pool-backed `FmtSong`
- exact integer transport
- deterministic NOTE/REST/TIE/OFF STEP REC
- provisional step-page/track-selection controls
- bounded tracker screen

Final Phase 2 checkpoint: `7063be51e53c2657178084b3bef5ace813dafcd7`.

### Phase 3 - p-locks/effects/runtime ingress - COMPLETE

D027-D036 are complete: stable semantic parameter IDs, 2,048-record sparse p-lock pool, held-step edit/browse/delete, row-local publication snapshots, exact six-tick tracker effects, NOTE DELAY/RETRIG/CUT, bounded 32-entry SPSC audition queue, bounded UI and worst-case closeout.

Final Phase 3 target footprint: 66,960 B image / 13,296 B normal RAM / 113,000 B pool.

Final closeout: `e90d0232c179ea92e2295171403697ceaf5e3ecf`, run `40084706845`, job `120845144453`.

### Phase 4 - first complete 8-BIT engine family - COMPLETE

WP4.1-WP4.10 are complete at host/device-build level.

PULSE, NOISE, TRIANGLE, WAVETABLE-8, common integer ADSR, deterministic eight-slot allocation, D044 parameter pages, linked all-eight-track playback and D046 representative/full-pool/realtime/target closeout are frozen.

Canonical D046 closeout:

- production baseline `c93c39ad344760b4da0dfbc76829ca5fef886174`
- verification head `2379d3e7e52d1cefe9f845f8287ad0ccda8c019a`
- run `40124536375`, job `120963022289`
- representative WAV `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9`
- normal WAV `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`
- target 72,504 B image / 19,604 B RAM / 113,000 B pool
- `hardware_margin_claim=none`

Evidence: `docs/WP4_10_VERIFICATION.md`.

### WP5.0 / D048 - mixed-engine routing migration - COMPLETE

D047 supersedes the old future project-wide 8-BIT/16-BIT restriction. D048 implements the smallest bounded production migration.

Accepted production model:

```text
tracker row/track
    -> instrument reference
    -> published instrument family + engine identity
    -> fixed per-track family router
    -> family-local engine/allocator
```

Frozen facts:

- `FmtStep` remains 8 B.
- `FmtInstrumentHeader` remains 4 B.
- family compatibility values remain `0 = 8-BIT/chip`, `1 = FM/digital`.
- `FmtSongMeta.mode` remains physically present but no longer gates playback/publication.
- instrument identity publication is fixed 64 B.
- global route ownership is exactly 8 B, one family byte per logical track.
- static bounded dispatch only, no runtime plugin registry or heap.
- one existing 2,048 B row-page cache remains shared.
- unsupported/malformed family or engine fails safely toward silence.
- non-applicable family-specific p-lock IDs are ignored, never reinterpreted.
- no FM4 production DSP was introduced by WP5.0.

Canonical WP5.0 closeout:

- production verification head `813792fb25632b4d7acda787422e51f713fad21e`
- aggregate run `40129822006`
- aggregate job `120978955451`
- artifact `12574707919`
- artifact SHA-256 `7a43d638fbf8e9ad8be9124b88645237459577c6cbf5fab8ea2661f100b898fa`
- representative D046 WAV remains byte-identical `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9`
- normal D046 WAV remains byte-identical `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`
- target 72,792 B image / 19,676 B RAM / 113,000 B pool
- delta versus D046 +288 B image / +72 B RAM / +0 B pool
- BIN SHA-256 `a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125`
- FWSC SHA-256 `cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073`
- inherited regressions green
- realtime audit green
- family-switch/malformed/delayed/non-applicable-p-lock tests green
- `hardware_margin_claim=none`

Evidence: `docs/WP5_0_VERIFICATION.md`.

## Active work package - WP5.1 / issue #44

Goal: implement and verify the smallest deterministic fixed-point FM operator primitive required by later FM4 work.

### Required execution order

1. Inspect the accepted integer pitch/phase/oscillator conventions from the 8-BIT family.
2. Write and freeze the normative WP5.1 / D049 contract before production FM DSP source changes.
3. Freeze exact operator state, phase format, signed modulation input, level/output range, reset semantics and overflow/saturation behavior.
4. Freeze deterministic host vectors before implementation.
5. Define target compile, realtime and resource guards.
6. Only then implement the operator primitive.
7. Verify repeat identity, edge cases, target build and resource delta.
8. Write `WP5_1_VERIFICATION.md` before WP5.2 begins.

### WP5.1 allowed scope

- fixed-point FM operator state and phase accumulator
- deterministic carrier lookup primitive chosen by the contract
- signed phase-modulation input
- bounded operator-level scaling
- reset/init primitive
- host vectors and edge tests
- target compile/resource measurement
- no-float/no-heap/no-runtime-64-bit-division guards

### WP5.1 explicitly out of scope

- algorithm routing, WP5.2
- ratio table/pitch policy, WP5.3
- final level/modulation-index mappings, WP5.4
- feedback, WP5.5
- FM envelope strategy, WP5.6
- FM voice allocation/router integration, WP5.7
- FM parameter pages/p-lock mapping, WP5.8
- simultaneous production 8-BIT + FM4 playback, WP5.9

## Phase 5 sequence after WP5.1

- WP5.2 curated FM algorithm routing
- WP5.3 ratio table
- WP5.4 levels/modulation index
- WP5.5 bounded feedback
- WP5.6 FM envelope strategy
- WP5.7 FM voice allocation/integration
- WP5.8 FM parameter pages + p-lock mapping
- WP5.9 mixed 8-BIT + FM4 validation
- WP5.10 worst-case mixed-engine benchmark and Phase 5 exit

## Important invariants - do not regress

- 8 logical tracks x 64 steps x 16 patterns.
- `FmtStep` exactly 8 B.
- exact integer transport and next-frame row-boundary ordering.
- explicit NONE/NOTE/TIE/OFF semantics.
- semantic parameter IDs remain separate from tracker-effect IDs.
- bounded corruption-safe p-lock storage/editor behavior.
- D031 row snapshots and D032 exact six-tick effect clock.
- D033 bounded RETRIG/NOTE DELAY/NOTE CUT scheduling.
- D034 exact 32-entry SPSC queue and fail-to-silence recovery.
- D043 still-owned active cross-track primary voices may not be stolen.
- D044 8-BIT IDs `0x20..0x25` remain stable.
- D048 per-instrument family/engine routing remains authoritative.
- no float in firmware DSP and no runtime 64-bit division.
- eight logical tracks are never silently reduced for engine cost.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted core direction: tracker depth underneath, immediate performance/capture on top.

Mixed-engine routing and FM4 are engine foundations for that product direction, not permission to pull live-capture or optional synth-family work into WP5.1.

## Hardware limitation still open

No physical FM-1 is currently available. Host/device-build evidence does not prove physical install/boot behavior, physical sound quality, control feel, screen feel or measured AC791N callback margin.

`hardware_margin_claim=none`

## Last updated

2026-10-24 - WP5.0 / D048 is complete and canonically green. WP5.1 / issue #44 is the only active package and must begin contract-first before FM operator production code.
