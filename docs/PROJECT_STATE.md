# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 2 — tracker core without synth ambition**

State: **IN PROGRESS**

Phase 2 parent issue: #8

Current branch: `phase2/tracker-core`

Phase 2 branch start / completed Phase 1 head:

`3ed4b65d7cda15c5831533f153f91e9014a9fde7`

WP2.1 through WP2.8 are complete.

## Current milestone

**WP2.9 — compact tracker screen**

State: **READY TO START**

No WP2.9 firmware implementation has started yet.

Before WP2.9 source code, freeze the exact bounded screen contract: visible rows, selected-row/playhead distinction, note/special-event text, track/page/header fields, dirty-region ownership and maximum redraw work per main-loop iteration. Keep this as a display/readability package; do not pull p-lock editing, multitrack playback or live-performance capture forward.

## Completed WP2.8 checkpoint

**WP2.8 — REST / TIE / NOTE-OFF semantics**

State: **COMPLETE — 2026-10-23**

GitHub issue: #16

WP2.8 start head:

`13c0748dd696e4f3a47279d00641068b5e915e46`

Final verified implementation/workflow commit:

`c66405bead1c79fd891cf0d12c8bd241019a7aaf`

GitHub Actions:

- run: `40004436981`
- job: `120588627172` (`special-events`)
- result: **SUCCESS**

Durable evidence:

`docs/WP2_8_VERIFICATION.md`

### D025 verified behavior

Editor behavior while stopped STEP REC is active:

- `FX` press/release -> REST / `FMT_NOTE_EMPTY`;
- `SELECT` without pending FX -> `FMT_NOTE_TIE`;
- hold `FX`, then press `SELECT` -> `FMT_NOTE_OFF`;
- REST is delayed until FX release so the OFF modifier cannot accidentally insert REST first;
- OFF consumes pending FX, so later FX release cannot double-write;
- HOME, REC exit and PLAY exit cancel pending FX safely;
- every special write changes only `FmtStep.note`, preserves the rest of the row, advances exactly once and wraps 63 -> 0;
- special writes use the existing D024 latch so simultaneous note entry cannot create a second row mutation.

Playback behavior:

- `FmtPlaybackEvent` remains 4 B and now has explicit `NONE`, `NOTE`, `TIE`, `OFF` kinds;
- EMPTY -> NONE / no pitch-gate command;
- ordinary note -> NOTE;
- TIE -> preserve an already active gate/pitch without retrigger;
- OFF -> release the proof voice;
- malformed/reserved non-note values -> safe NONE;
- special sentinels can never become raw pitches;
- D021 frame ordering is unchanged.

Focused audible OFF proof:

```text
WP2_8_OFF_AUDIO_BEHAVIOR_OK=1 before=4410 quiet=0 after=4410
```

The checked post-OFF window is exactly silent, then the following ordinary note reopens the voice.

### Frozen default audio regression

Default host WAV remains exactly:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

### Current verified runtime baseline

| Resource | Used | Limit | Free |
|---|---:|---:|---:|
| app image | 56,748 B | 581,564 B | 524,816 B |
| normal RAM (`.data + .bss`) | 12,420 B | 98,304 B | 85,884 B |
| pool RAM | 96,616 B | 344,064 B | 247,448 B |

WP2.8 delta vs WP2.7:

- image: **+200 B**
- normal RAM: **+16 B**
- pool RAM: **+0 B**

Linked runtime symbols:

```text
01c0b070 g       .bss         00000003 fmt_step_record_runtime
01c0b073 g       .bss         00000004 fmt_pattern_input_runtime
01c0b078 g       .bss         00000008 fmt_transport_runtime
01c0b080 g       .bss         00000001 fmt_playback_runtime
01c20000 g       .pool        00010168 fmt_store_storage
```

Final WP2.8 artifacts:

- `fm1tracker.bin`: SHA-256 `a4dd8d8326f9f28852d243c2a07e88e0ac3af5c5ce302ff1ac8b45883b72eecf`
- `fm1tracker-0.1.fwsc`: SHA-256 `fd569d068c272e49b00cf0505bb613b774be08ae0aa757932594005b1c36ef69`
- package identity: `FM-1_987`
- evidence artifact ID: `12523074553`
- evidence ZIP SHA-256: `263d20fbf9162f6cc5e199980f2c76dd9267a2ec92c84baf5bc4408f476b041b`

Physical FM-1 gesture ergonomics, install/boot/audio behavior and actual callback execution time remain **NOT TESTED** because hardware is not currently available.

## Completed Phase 2 package evidence

- WP2.1 / issue #9 — frozen V0.1 model: `docs/WP2_1_VERIFICATION.md`
- WP2.2 / issue #10 — pool-backed song store: `docs/WP2_2_VERIFICATION.md`
- WP2.3 / issue #11 — exact integer transport: `docs/WP2_3_VERIFICATION.md`
- WP2.4 / issue #12 — one-track deterministic playback: `docs/WP2_4_VERIFICATION.md`
- WP2.5 / issue #13 — four-page white-key step addressing: `docs/WP2_5_VERIFICATION.md`
- WP2.6 / issue #14 — eight-black-key track selection prototype: `docs/WP2_6_VERIFICATION.md`
- WP2.7 / issue #15 — stopped STEP REC note entry: `docs/WP2_7_VERIFICATION.md`
- WP2.8 / issue #16 — REST/TIE/OFF editor + playback semantics: `docs/WP2_8_VERIFICATION.md`

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
- Preserve the default WAV hash through display-only WP2.9 unless a documented intentional audio change is made.
- No floating point in firmware DSP and no runtime 64-bit division.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` is accepted future core direction: seamless play -> REC capture while transport continues, fast track switching, drums, metronome, poly target, CHANNEL/GLOBAL performance FX and bounded parameter/FX capture into tracker-editable data. It is **not** optional Phase 9 work, but it is also **not** to be pulled into WP2.9.

## Next action

Start WP2.9 as its own package. Mark it IN PROGRESS and record the exact tracker-screen/readability/dirty-redraw contract before source code. Implement only the compact tracker display layer on top of the already verified model/editor/playback state. Do not begin WP2.10 or Phase 3 in the same package.

## Last relevant checkpoints

- Phase 1 closeout / Phase 2 branch start: `3ed4b65d7cda15c5831533f153f91e9014a9fde7`
- WP2.4 verified playback: `c8555ce8b68f5813e785b057386110cb177fb9d0`
- WP2.5 verified page input: `28d8ac051eb7107e745bb007e1a39014f51c1169`
- WP2.6 verified track selection: `707d0b24eb15c19193136f68e7c9f4e1d0a7dc7d`
- WP2.7 verified STEP REC: `790fb87596150567307f88fb537cd63134a3f5fc`
- WP2.8 start: `13c0748dd696e4f3a47279d00641068b5e915e46`
- WP2.8 verified implementation/workflow: `c66405bead1c79fd891cf0d12c8bd241019a7aaf`
- WP2.8 final Actions run: `40004436981`

## Last updated

2026-10-23 — WP2.8 complete. WP2.9 compact tracker screen is READY TO START; no WP2.9 implementation has begun.