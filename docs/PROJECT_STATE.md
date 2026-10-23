# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 2 — tracker core without synth ambition**

State: **IN PROGRESS — final integration package remains**

Phase 2 parent issue: #8

Current branch: `phase2/tracker-core`

Phase 2 branch start / completed Phase 1 head:

`3ed4b65d7cda15c5831533f153f91e9014a9fde7`

WP2.1 through WP2.9 are complete.

## Current milestone

**WP2.10 — Phase 2 integration test and closeout**

State: **READY TO START**

GitHub issue: #18

No WP2.10 implementation has started.

This is an integration/closeout package, not a feature package. It must exercise D018–D026 together, verify the complete 64-step computer-free editor/display/playback workflow, record total Phase 2 resource cost and close parent #8 only after a green end-to-end gate.

Strict rule: **do not begin Phase 3 before WP2.10 closes Phase 2.**

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

Earlier isolated presentation-model gate:

- run: `40008827313`
- job: `120603588405`
- result: **SUCCESS**

Durable evidence:

- `docs/WP2_9_SCREEN_CONTRACT.md`
- `docs/WP2_9_VERIFICATION.md`

### D026 verified screen behavior

- tracker screen owns ordinary SEQ and STEP REC only; HOME/PERFORMANCE retains the original performance screen;
- exactly one selected logical track is displayed;
- exactly eight tracker rows are visible;
- each 16-step page is split into a fixed first/second eight-row half selected by the edit cursor;
- playback never scrolls the edit viewport;
- header exposes mode, `CURnn`, `PLYnn` or `STOP`, BPM, `P01`, selected track, page and visible range;
- `>` is edit cursor, `*` is running playhead, `>*` means both;
- ordinary note text is deterministic;
- EMPTY/REST=`---`, TIE=`===`, NOTE OFF=`OFF`, malformed note=`???`;
- instrument inherit=`..`, explicit instruments=`01..16`;
- volume inherit=`...`, explicit volume=`000..254`;
- effect NONE=`---`, nonzero effect=`???` until Phase 3 defines stable effect semantics;
- screen uses one header, eight row strips and one footer = exactly 10 bounded regions;
- at most **2 tracker regions are rendered per main-loop poll**;
- presentation synchronization inspects only eight visible rows and cannot mutate the song store;
- all tracker drawing remains outside the audio ISR;
- `SEQ -> HOME` is pixel-identical to a control run that never left HOME.

### Reviewed deterministic framebuffer references

SEQ PPM SHA-256:

`e67d0cf9aa2fa391464237a40ed795bb6ba881e15aff40340e1b92a4a8eccb7a`

STEP REC PPM SHA-256:

`4b0192b084e6ee483d7a8f98add2ae95229dcc914583d3cbe957944824cb6424`

These raw framebuffers were visually inspected before being frozen in CI.

### Audio regression

The complete `/* ---- the sound */` section of `dinghy.c` is byte-identical to the WP2.9 start baseline.

Frozen default host WAV remains:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

### Current verified runtime baseline

| Resource | Used | Limit | Free |
|---|---:|---:|---:|
| app image | 59,252 B | 581,564 B | 522,312 B |
| normal RAM (`.data + .bss`) | 12,464 B | 98,304 B | 85,840 B |
| pool RAM | 96,616 B | 344,064 B | 247,448 B |

WP2.9 delta vs WP2.8:

- image: **+2,504 B**
- normal RAM: **+44 B**
- pool RAM: **+0 B**

The normal-RAM delta is exactly the 44-byte tracker-screen presentation state.

Linked runtime symbols:

```text
01c0b070 g .bss  00000003 fmt_step_record_runtime
01c0b073 g .bss  00000004 fmt_pattern_input_runtime
01c0b078 g .bss  00000008 fmt_transport_runtime
01c0b080 g .bss  0000002c fmt_tracker_screen_runtime
01c0b0ac g .bss  00000001 fmt_playback_runtime
01c20000 g .pool 00010168 fmt_store_storage
```

Final WP2.9 artifacts:

- `fm1tracker.bin`: SHA-256 `34d0e8c4344e091704608e08357964e7ae0b18673dd0b93f36c4e5bcc2cb4c7c`
- `fm1tracker-0.1.fwsc`: SHA-256 `3cfd0a2da3836720d2f921b04f29e93c16ca83ab18ab7242c99f2c333d890ca7`
- package identity: `FM-1_987`
- evidence artifact ID: `12525582378`
- evidence ZIP SHA-256: `6c23c962c96de458d5d328e60a3b5cbd224e2011933b2d812ee70d8fe6c8daa8`
- evidence ZIP size: 1,123,329 B

Physical FM-1 screen readability, physical update latency, install/boot/audio behavior and actual AC791N render execution time remain **NOT TESTED** because hardware is not currently available.

## Completed Phase 2 package evidence

- WP2.1 / #9 — V0.1 core model: `docs/WP2_1_VERIFICATION.md`
- WP2.2 / #10 — pool-backed song store: `docs/WP2_2_VERIFICATION.md`
- WP2.3 / #11 — exact integer transport: `docs/WP2_3_VERIFICATION.md`
- WP2.4 / #12 — one-track deterministic playback: `docs/WP2_4_VERIFICATION.md`
- WP2.5 / #13 — four-page white-key step addressing: `docs/WP2_5_VERIFICATION.md`
- WP2.6 / #14 — black-key track-selection prototype: `docs/WP2_6_VERIFICATION.md`
- WP2.7 / #15 — stopped STEP REC note entry: `docs/WP2_7_VERIFICATION.md`
- WP2.8 / #16 — REST/TIE/OFF editor + playback semantics: `docs/WP2_8_VERIFICATION.md`
- WP2.9 / #17 — bounded compact tracker screen: `docs/WP2_9_VERIFICATION.md`

## Important decisions / do not regress

- D005: 8 logical tracks × 64 steps × 16 patterns.
- D006: STEP REC is first-class.
- D016: fixed package identity `FM-1_987`.
- D018/D019: frozen 8-byte step and exactly one pool-backed runtime `FmtSong`.
- D020: exact integer transport.
- D021: exact frame-boundary dispatch ordering.
- D022: explicit PERFORMANCE/SEQ context and four white-key step pages.
- D023: provisional direct black-key track selection.
- D024: stopped STEP REC note entry, deterministic lowest-note/full-release gesture, modulo-64 advance.
- D025: explicit REST/TIE/OFF editor and playback semantics.
- D026: cursor-owned eight-row one-track tracker display with independent cursor/playhead and bounded two-region redraw.
- Preserve the frozen default WAV unless a later package explicitly changes audio semantics.
- No floating point in firmware DSP and no runtime 64-bit division.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted future core direction: seamless play -> REC capture while transport continues, fast track switching, drums, metronome, poly target, CHANNEL/GLOBAL performance FX and bounded parameter/FX capture into tracker-editable data. It is not optional Phase 9 work, but it must not be pulled into WP2.10.

## Exact next action

Start issue #18 as its own package. First construct the deterministic Phase 2 end-to-end integration fixture against the verified WP2.9 baseline. Prefer tests and evidence over new firmware. If integration reveals a defect, fix only the smallest demonstrated defect and rerun all inherited gates. Do not add new feature semantics and do not begin Phase 3.

## Last relevant checkpoints

- Phase 1 closeout / Phase 2 branch start: `3ed4b65d7cda15c5831533f153f91e9014a9fde7`
- WP2.4 verified playback: `c8555ce8b68f5813e785b057386110cb177fb9d0`
- WP2.5 verified page input: `28d8ac051eb7107e745bb007e1a39014f51c1169`
- WP2.6 verified track selection: `707d0b24eb15c19193136f68e7c9f4e1d0a7dc7d`
- WP2.7 verified STEP REC: `790fb87596150567307f88fb537cd63134a3f5fc`
- WP2.8 verified special events: `c66405bead1c79fd891cf0d12c8bd241019a7aaf`
- WP2.9 start: `5c2830cf2d37c8466f8564aab9a1bd89feaad38a`
- WP2.9 verified implementation/workflow: `e53d785b811371cf817d22be1fc8631b1e12cd12`
- WP2.9 final Actions run: `40009689758`

## Last updated

2026-10-23 — WP2.9 closed and durably verified. WP2.10 / issue #18 is READY TO START; no WP2.10 implementation has started.