# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 2 — tracker core without synth ambition**

State: **IN PROGRESS — final integration/closeout package active**

Phase 2 parent issue: #8

Current branch: `phase2/tracker-core`

Phase 2 branch start / completed Phase 1 head:

`3ed4b65d7cda15c5831533f153f91e9014a9fde7`

WP2.1 through WP2.9 are complete.

## Current milestone

**WP2.10 — Phase 2 integration test and closeout**

State: **IN PROGRESS — 2026-10-23**

GitHub issue: #18

WP2.10 start head:

`47e9093ae300043b804f85ce62fee1fe758fc943`

WP2.10 is an integration/verification package, not a feature package. No new tracker, playback, synth, p-lock, persistence or live-capture semantics are permitted here.

### Exact objective

Build one deterministic end-to-end Phase 2 host fixture against the already verified D018–D026 implementation and prove that the complete editor/display/playback workflow composes correctly:

- PERFORMANCE -> SEQ context;
- all four 16-step pages / complete 0..63 address space;
- direct track selection and write isolation;
- real stopped-transport acknowledgement before STEP REC;
- ordinary note entry plus REST / TIE / NOTE OFF;
- automatic cursor advance across half-page and page boundaries;
- exact 63 -> 0 editor wrap;
- tracker display synchronized to cursor, track, page and special-event notation;
- resume the existing D020/D021 pattern-0 / track-0 proof playback;
- clean HOME return;
- direct inspection of final song/store state against the expected integration pattern.

Prefer tests/evidence over firmware changes. If the integration fixture demonstrates a defect, fix only the smallest proven defect and rerun every inherited gate.

### Expected WP2.10 changes

Primary expected changes are test/evidence files only:

- `docs/WP2_10_TEST_PLAN.md` — deterministic integration fixture contract;
- one focused Phase 2 integration test/host scenario and runner;
- `.github/workflows/wp2_10_phase2_integration_verify.yml` — complete Phase 2 regression/device gate;
- `docs/PHASE2_VERIFICATION.md` and closeout documentation only after a real green checkpoint.

Existing firmware/source modules are expected to remain unchanged unless the integration test proves a concrete defect.

### Required verification

1. all WP2.1–WP2.9 unit/focused host suites remain green;
2. deterministic 64-step traversal has no gaps, duplicates or out-of-bounds cursor states;
3. writes to selected tracks are isolated from every other row/track;
4. ordinary note / EMPTY / TIE / OFF final contents match the exact expected pattern;
5. final cursor/page/track and STEP REC state are exact;
6. the D026 framebuffer matches the intended integration end state and HOME return remains clean;
7. D020/D021 timing remains exact, including the 120 BPM 64-step loop at frame 352800;
8. frozen default host WAV remains `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`;
9. full JieLi build, `FM-1_987` identity, HAL-only register access and resource checks pass;
10. final Phase 2 footprint is compared against the Phase 1 baseline;
11. durable `docs/PHASE2_VERIFICATION.md` records the complete closeout evidence;
12. parent #8 closes only after this complete gate is green.

### Strict WP2.10 non-goals

- no p-lock allocation/editing or stable parameter IDs;
- no new effect semantics;
- no new synth/audio engine;
- no general multitrack playback expansion;
- no live-performance capture;
- no song/order/persistence;
- no hardware control-map freeze;
- no HAL/loader/update redesign;
- **do not begin Phase 3 in this package**.

## Completed WP2.9 checkpoint

**WP2.9 — compact tracker screen**

State: **COMPLETE — 2026-10-23**

GitHub issue: #17

WP2.9 start head:

`5c2830cf2d37c8466f8564aab9a1bd89feaad38a`

Final verified implementation/workflow commit:

`e53d785b811371cf817d22be1fc8631b1e12cd12`

GitHub Actions:

- run: `40009689758`
- job: `120606559644` (`tracker-screen`)
- result: **SUCCESS**

Durable evidence:

- `docs/WP2_9_SCREEN_CONTRACT.md`
- `docs/WP2_9_VERIFICATION.md`

### Current verified runtime baseline after WP2.9

| Resource | Used | Limit | Free |
|---|---:|---:|---:|
| app image | 59,252 B | 581,564 B | 522,312 B |
| normal RAM (`.data + .bss`) | 12,464 B | 98,304 B | 85,840 B |
| pool RAM | 96,616 B | 344,064 B | 247,448 B |

Linked runtime objects remain:

- `FmtStepRecord`: 3 B;
- `FmtPatternInput`: 4 B;
- `FmtTransport`: 8 B;
- `FmtTrackerScreen`: 44 B;
- `FmtPlayback`: 1 B;
- one 65,896 B `FmtSong` at `.pool` `0x01C20000`.

Reviewed deterministic framebuffer references:

- SEQ: `e67d0cf9aa2fa391464237a40ed795bb6ba881e15aff40340e1b92a4a8eccb7a`;
- STEP REC: `4b0192b084e6ee483d7a8f98add2ae95229dcc914583d3cbe957944824cb6424`.

Frozen default host WAV:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

Package identity remains `FM-1_987`.

Physical FM-1 screen readability, physical update latency, install/boot/audio behavior and actual AC791N execution time remain **NOT TESTED** because hardware is not currently available.

## Completed Phase 2 package evidence

- WP2.1 / #9 — `docs/WP2_1_VERIFICATION.md`
- WP2.2 / #10 — `docs/WP2_2_VERIFICATION.md`
- WP2.3 / #11 — `docs/WP2_3_VERIFICATION.md`
- WP2.4 / #12 — `docs/WP2_4_VERIFICATION.md`
- WP2.5 / #13 — `docs/WP2_5_VERIFICATION.md`
- WP2.6 / #14 — `docs/WP2_6_VERIFICATION.md`
- WP2.7 / #15 — `docs/WP2_7_VERIFICATION.md`
- WP2.8 / #16 — `docs/WP2_8_VERIFICATION.md`
- WP2.9 / #17 — `docs/WP2_9_VERIFICATION.md`

## Important decisions / do not regress

- D005: 8 logical tracks × 64 steps × 16 patterns.
- D006: STEP REC is first-class.
- D016: fixed package identity `FM-1_987`.
- D018/D019: frozen 8-byte step and exactly one pool-backed runtime `FmtSong`.
- D020: exact integer transport.
- D021: exact frame-boundary dispatch ordering.
- D022: explicit PERFORMANCE/SEQ context and four white-key step pages.
- D023: provisional direct black-key track selection.
- D024: stopped STEP REC, deterministic lowest-note/full-release gesture, modulo-64 advance.
- D025: explicit REST/TIE/OFF editor and playback semantics.
- D026: cursor-owned eight-row one-track tracker display with independent cursor/playhead and bounded two-region redraw.
- Preserve the frozen default WAV unless a later package explicitly changes audio semantics.
- No floating point in firmware DSP and no runtime 64-bit division.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted future core direction. It is not optional Phase 9 work, but it must not be pulled into WP2.10.

## Exact next action

Freeze `docs/WP2_10_TEST_PLAN.md` against this start head, then implement the deterministic Phase 2 integration fixture. The fixture must inspect final store state directly and should require no production-firmware change. Run the focused integration gate before expanding to the full inherited/device gate. Do not begin Phase 3.

## Last relevant checkpoints

- Phase 1 closeout / Phase 2 branch start: `3ed4b65d7cda15c5831533f153f91e9014a9fde7`
- WP2.4 verified playback: `c8555ce8b68f5813e785b057386110cb177fb9d0`
- WP2.5 verified page input: `28d8ac051eb7107e745bb007e1a39014f51c1169`
- WP2.6 verified track selection: `707d0b24eb15c19193136f68e7c9f4e1d0a7dc7d`
- WP2.7 verified STEP REC: `790fb87596150567307f88fb537cd63134a3f5fc`
- WP2.8 verified special events: `c66405bead1c79fd891cf0d12c8bd241019a7aaf`
- WP2.9 verified implementation/workflow: `e53d785b811371cf817d22be1fc8631b1e12cd12`
- WP2.9 final Actions run: `40009689758`
- WP2.10 start: `47e9093ae300043b804f85ce62fee1fe758fc943`

## Last updated

2026-10-23 — WP2.10 / issue #18 started as the final Phase 2 integration/closeout package. No WP2.10 integration fixture has been implemented yet.