# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, `D047_MIXED_ENGINE_ARCHITECTURE.md`, `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 4 - first engine family: 8-BIT/chip**

State: **IN PROGRESS - WP4.1/D037 through WP4.9/D045 COMPLETE; WP4.10 / #41 active; D046 FROZEN; representative + realtime gates implemented; aggregate exit gate still RED - 2026-10-24**

Parent issue: #30  
Current work package: WP4.10 / #41  
Current branch: `phase4/8bit-engine`

Phase 4 branch start / final Phase 3 handoff:

`d14955f23a11044e8a348884feca24f8978fd700`

Final Phase 3 production firmware baseline inherited by Phase 4:

`354bf7d356774ad828f1f660bd456dbab7f0490d`

Final Phase 3 closeout:

- workflow checkpoint `e90d0232c179ea92e2295171403697ceaf5e3ecf`
- run `40084706845`
- job `120845144453`
- evidence `docs/WP3_10_VERIFICATION.md`

## Phase status summary

### Phase 0 - foundation - COMPLETE

The hardware-first tracker boundary, eight logical tracks, 64-step patterns, 16 pattern slots, STEP REC and first-class p-lock direction were established. D004 originally restricted V0.1 to a project-wide 8-BIT or 16-BIT sound mode. **D047 now supersedes only that project-wide-mode restriction for future work.**

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

### Phase 4 - first engine family: 8-BIT/chip - IN PROGRESS

WP4.1 through WP4.9 are complete.

The first complete engine family now contains:

- PULSE - D038
- NOISE - D039
- TRIANGLE - D040
- WAVETABLE-8 - D041
- common integer ADSR - D042
- deterministic eight-slot allocator / detached-tail policy - D043
- engine parameter pages + six stable D044 parameter IDs - D044
- linked all-eight-track production integration - D045

WP4.9 final closeout:

- closeout head `89fabc51348078101c8e376bba7814af9d42ae80`
- run `40116897104`
- job `120939922897`
- evidence `docs/WP4_9_VERIFICATION.md`
- image 72,504 B
- normal RAM 19,604 B
- pool 113,000 B
- `fm1tracker.bin` SHA-256 `214fba8ec0af4be3120bc91dafbaa24e7d4d8a5a6de7395de2473f12374c4215`
- `fm1tracker-0.1.fwsc` SHA-256 `20f0bbe556edcf6a00118f9983335913dda2f29fc64d3c87997bfd6756bcf119`
- linked default host WAV `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`

## Active work package - WP4.10 / D046

Normative contract: `docs/WP4_10_PHASE4_EXIT_CONTRACT.md`.

D046 is a **verification/benchmark/closeout package only**. The linked production baseline is frozen at:

`c93c39ad344760b4da0dfbc76829ca5fef886174`

Unless D046 proves a real defect, production under `firmware/src`, `firmware/hal`, `firmware/loader`, `firmware/app.ld`, `build.sh` and `tools` must remain byte-identical to that checkpoint.

### What has already been implemented in WP4.10

The older handoff wording that said “benchmark implementation next” is now stale. Verification-only implementation is already present.

Implemented D046 coverage includes:

- representative all-eight-track linked workload;
- all four 8-BIT engines simultaneously;
- complete D028 pool pressure: 512 owners / 2,048 locks;
- D031/D032/D033/D044 combined representative matrix;
- D043 detached-release-tail contention checks;
- 4,096-row / 64-wrap deterministic long run;
- concurrent D034 queue full/overflow/recovery pressure;
- structural realtime ownership/work-bound audit;
- production-freeze checks;
- aggregate reuse of the authoritative WP4.9 inherited-regression + pinned-target closeout.

Focused representative gate is green. Known run:

- `40118888617` - representative linked benchmark - SUCCESS

Focused realtime structural audit is green:

- commit checkpoint `c8acea33dfbb942dc26527ea2fb9517cb2a6c1fe`
- run `40120436446` - SUCCESS
- job `120950599001`

Key realtime evidence includes:

```text
WP4_10_PRODUCTION_FREEZE_OK=1
WP4_10_AUDIO_OWNERSHIP_OK=1 d034_consumers=1 engine_render_calls=1 editor_store_traversal=0 screen_calls=0
WP4_10_TRACKER_FRAME_BOUND_OK=1 row_tracks=8 tick_due_checks_max=8 row_boundary_wins=1 second_tracker_clock=0
WP4_10_QUEUE_BOUND_OK=1 capacity=32 callback_cap=32 overflow_discard_o1=1 spin_retry=0 irq_masking=0
WP4_10_ENGINE_BOUND_OK=1 render_slots=8 mutable_store_reads=0 lock_list_reads=0
WP4_10_INTEGER_REALTIME_OK=1 callback_64bit_types=0 leaf_64bit_types=0 float_double=0
WP4_10_SINGLE_CLOCK_OK=1 row_phase=661500 tick_phase=110250 ticks_per_row=6
WP4_10_REALTIME_AUDIT_OK=1
```

### Current blocker

The first aggregate Phase 4 exit workflow is red:

- aggregate wiring head `6d3ac6debbeb7f084d92398fcaac8782db64e848`
- run `40120541899`
- job `120950915360`
- failed step: `Run aggregate D046 Phase 4 exit gate`

The focused D046 representative and realtime gates are already green, so Phase 4 must **not** be marked complete until the aggregate failure is isolated. It may be an aggregate harness/path/grep issue or a real inherited target/regression failure; this must be proven rather than assumed.

### Exact next execution action

1. Inspect the failing aggregate job/log and identify the exact command that failed.
2. If it is a verification-harness bug, make the smallest test/workflow-only correction. Do not touch production source.
3. If a real production defect is exposed, stop closeout and follow D046's focused-regression/minimal-fix/full-rerun rule.
4. Rerun the complete WP4.10 aggregate exit gate until green.
5. Freeze final benchmark digests/counts/WAV hash, inherited regression result, target resources and target artifact hashes in `docs/WP4_10_VERIFICATION.md`.
6. Update `PROJECT_STATE.md`, `TEST_MATRIX.md`, `ROADMAP.md`, `WORK_PACKAGES.md`, `START_HERE.md` and issue state to mark Phase 4 complete.
7. Only then begin Phase 5 / WP5.0.

## D047 - accepted future architecture direction

Normative decision: `docs/D047_MIXED_ENGINE_ARCHITECTURE.md`.

D047 is accepted now because the original project-wide 8-BIT/16-BIT restriction would unnecessarily limit the instrument and cause another architecture migration later.

### New product rule

FM1Tracker is **one engine-agnostic tracker/sequencer**.

The project itself is not globally 8-BIT or 16-BIT. Each instrument owns an engine family + engine identity. Tracks/rows reference instruments, so one pattern/project may eventually combine, for example:

```text
PULSE + NOISE + FM4 + WAVETABLE-8 + modern VA/supersaw + drum engines
```

subject to measured CPU/RAM/voice budgets and explicit engine work packages.

### Why this is non-destructive

The existing model already contains the right indirection:

- `FmtStep.instrument` is already one byte and remains unchanged;
- `FmtTrackConfig.instrument` already supplies a track default;
- `FmtInstrumentHeader` is already exactly 4 B with `mode`, `engine`, `version`, `flags`;
- D027 parameter IDs are semantic and engine-independent at the tracker-storage layer;
- D020/D031/D032/D033/D034 tracker/event timing is not tied to an oscillator family;
- D045 already has deterministic instrument/engine identity-change reset behavior.

The current `mode` byte in `FmtInstrumentHeader` becomes conceptually an **engine-family discriminator** for future mixed routing. It is not renamed during WP4.10 because Phase 4 production is frozen.

`FmtSongMeta.mode` remains physically present for compatibility during Phase 4, but D047 supersedes its role as a future project-wide playback eligibility gate. WP5.0 will freeze its new non-gating meaning before persistence is designed.

### What D047 does NOT change

D047 does not invalidate or rewrite:

- any Phase 1-3 contract or verification;
- D037-D046 historical Phase 4 contracts;
- the four existing 8-BIT engines;
- D043 allocator evidence;
- D044 p-lock IDs/page mappings;
- D045 target hashes/resources/WAV evidence;
- D046 production freeze and exit obligations.

The complete 8-BIT path is the first proven engine family and becomes the regression baseline for mixed-engine routing.

## Next phase after a green Phase 4

**Phase 5 - mixed-engine routing + FM4**

Phase 5 begins with:

### WP5.0 - mixed-engine routing contract + migration

This package must be completed before FM4 is linked into tracker playback.

It will freeze:

- per-instrument family + engine as authoritative routing identity;
- removal of the song-level global engine-family gate;
- simultaneous different engine families across logical tracks;
- deterministic cross-family instrument switching on a track;
- at most one owned primary sequenced voice per logical track across all families;
- no active cross-track stealing;
- bounded static family/engine dispatch, no dynamic plugin architecture;
- measured cross-family allocator/ownership representation;
- common publication/parameter adapter boundary;
- exact 8-BIT regression equivalence under the new router.

Then Phase 5 proceeds with:

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

Modern VA/supersaw/wavetable/drum engines are **architecturally allowed later**, but are not silently promoted into the current Phase 5 scope. Each remains a future bounded engine package with CPU/RAM/parameter/licensing gates.

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
- D037-D045: current 8-BIT family implementation and realtime/resource contracts remain valid for that family.
- D043: still-owned active cross-track primary voices may not be stolen.
- D044: assigned 8-BIT parameter IDs `0x20..0x25` remain stable.
- D046: Phase 4 closes only on green representative/full-pool/long-run/queue/realtime/inherited/target evidence.
- D047: no project-wide engine-family restriction in future architecture; instrument is the routing authority.
- No float in firmware DSP and no runtime 64-bit division.
- Eight logical tracks are not silently reduced to obtain polyphony or expensive engines.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted core direction. The target experience is tracker depth underneath and immediate performance/capture on top.

The mixed-engine architecture strengthens that direction: a live performance setup may use different engine families for drums, bass, poly, lead and flexible tracks without changing the tracker model.

## Hardware limitation still open

No physical FM-1 is currently available. Host/device-build evidence does not claim physical install/boot behavior, physical sound quality, control feel, screen feel or measured AC791N callback execution margin.

Physical execution-margin and UX validation remain explicit later gates.

## Last relevant checkpoints

- Phase 2 verified: `7063be51e53c2657178084b3bef5ace813dafcd7`
- Phase 3 verified production: `354bf7d356774ad828f1f660bd456dbab7f0490d`
- Phase 3 closeout: `e90d0232c179ea92e2295171403697ceaf5e3ecf` - run `40084706845`
- WP4.9 D045 contract: `fd31801b8a73e27345d4575026b593fb45e10c20`
- WP4.9 final closeout: `89fabc51348078101c8e376bba7814af9d42ae80` - run `40116897104`
- WP4.10 D046 contract: `aaceafc24322611bf00255bf028466d7d88643de`
- WP4.10 realtime-audit checkpoint: `c8acea33dfbb942dc26527ea2fb9517cb2a6c1fe` - run `40120436446`
- WP4.10 aggregate wiring checkpoint: `6d3ac6debbeb7f084d92398fcaac8782db64e848` - run `40120541899` RED
- D047 decision contract: `docs/D047_MIXED_ENGINE_ARCHITECTURE.md`

## Last updated

2026-10-24 - D047 accepted as the future engine architecture while WP4.10 remains the only active package. Current execution priority is unchanged: fix/rerun the red D046 aggregate exit gate, produce final WP4.10 evidence and close Phase 4. Phase 5 then begins with WP5.0 mixed-engine routing before FM4 production integration.
