# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 2 — tracker core without synth ambition**

State: **COMPLETE — 2026-10-23**

Phase 2 parent issue: #8

Current branch: `phase2/tracker-core`

Phase 2 branch start / completed Phase 1 head:

`3ed4b65d7cda15c5831533f153f91e9014a9fde7`

All WP2.1 through WP2.10 exit criteria are verified at the host/device-build level.

Durable Phase 2 closeout evidence:

`docs/PHASE2_VERIFICATION.md`

## Final WP2.10 checkpoint

**WP2.10 — Phase 2 integration test and closeout**

State: **COMPLETE — 2026-10-23**

GitHub issue: #18

WP2.10 start head:

`47e9093ae300043b804f85ce62fee1fe758fc943`

Final verified implementation/workflow checkpoint:

`7063be51e53c2657178084b3bef5ace813dafcd7`

GitHub Actions:

- run: `40015242947`
- job: `120625519388` (`phase2-closeout`)
- result: **SUCCESS**

WP2.10 is integration/evidence only. The final gate proves `firmware/`, `tools/` and `build.sh` are byte-identical to the WP2.10 start head. No production defect fix was required.

### Integrated workflow proven

- all 64 step addresses are visited exactly once through D022 page/key addressing;
- all eight D023 track selectors map bijectively and do not mutate song data;
- STEP REC rejects running transport and enters only after a real stopped state;
- ordinary note entry plus REST/TIE/OFF compose correctly;
- automatic cursor boundaries `63 -> 0` and `15 -> 16` are exact;
- the full 65,896-byte final song store matches the expected integration copy byte-for-byte;
- edits on logical track 7 leave the fixed track-0 proof lane unchanged;
- D026 display shows exact track/page/cursor and `---` / `===` / `OFF` / `E-4` notation;
- full-app `dinghy.c` scenario traverses the same workflow through real panel input handling;
- ordinary SEQ traversal leaks no audition MIDI;
- HOME reclaim after the integrated tracker workflow is pixel-identical to an equivalent HOME-only control;
- playback performs exactly 64 row boundaries over the 352,800-frame 120 BPM loop with no cumulative timing drift;
- every WP2.1–WP2.9 regression remains green.

Direct integration marker:

```text
PHASE2_INTEGRATION_TESTS_OK=1
visited_steps=64 tracks=8 edits_track=7 wrap=63_to_0 page_cross=15_to_16 boundaries_120=64 loop_frames=352800 final_cursor=20 final_page=1 final_track=7
store_bytes=65896 final_events=note,empty,tie,off proof_lane=track0_unchanged screen_window=16_23
```

Full-app marker:

```text
WP2_10_FULL_APP_HOST_OK=1 midi_note_pairs=4 traversed_steps=64 selected_track=8
```

Reviewed integrated tracker framebuffer:

`a993551ce922d90186080bf554b82477b4336e160a6b301c446f0d275569b6d7`

HOME-equivalent framebuffer:

`ea33024427340416c17a73ecfb2303e37262b8c61e2f2d30ca06265ba6e0b899`

Frozen default host WAV remains:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

Host frames remain `286464`.

## Final Phase 2 runtime/resources

| Resource | Phase 1 baseline | Phase 2 closeout | Phase 2 delta | Limit | Free |
|---|---:|---:|---:|---:|---:|
| app image | 55,088 B | 59,252 B | +4,164 B | 581,564 B | 522,312 B |
| normal RAM | 12,368 B | 12,464 B | +96 B | 98,304 B | 85,840 B |
| pool RAM | 30,720 B | 96,616 B | +65,896 B | 344,064 B | 247,448 B |

WP2.10 production delta vs WP2.9 is exactly zero.

Linked runtime layout:

```text
01c0b070 g .bss  00000003 fmt_step_record_runtime
01c0b073 g .bss  00000004 fmt_pattern_input_runtime
01c0b078 g .bss  00000008 fmt_transport_runtime
01c0b080 g .bss  0000002c fmt_tracker_screen_runtime
01c0b0ac g .bss  00000001 fmt_playback_runtime
01c20000 g .pool 00010168 fmt_store_storage
```

Final artifacts remain byte-identical to WP2.9:

- `fm1tracker.bin`: `34d0e8c4344e091704608e08357964e7ae0b18673dd0b93f36c4e5bcc2cb4c7c`
- `fm1tracker-0.1.fwsc`: `3cfd0a2da3836720d2f921b04f29e93c16ca83ab18ab7242c99f2c333d890ca7`
- package identity: `FM-1_987`
- closeout evidence artifact ID: `12527623388`
- evidence ZIP SHA-256: `2a4cbcda3e7f1f26fe2f117062fdd72c9fb274ae0ce8aa4ccfa1a7763829bd9c`

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
- WP2.10 / #18 — `docs/PHASE2_VERIFICATION.md`

## Important decisions / do not regress

- D005: 8 logical tracks × 64 steps × 16 patterns.
- D006: STEP REC is first-class.
- D016: fixed package identity `FM-1_987`.
- D018/D019: frozen 8-byte step and one pool-backed runtime `FmtSong`.
- D020: exact integer transport.
- D021: exact frame-boundary dispatch ordering and fixed proof lane.
- D022: PERFORMANCE/SEQ context and four white-key step pages.
- D023: provisional direct black-key track selection.
- D024: stopped STEP REC, one deterministic write gesture, modulo-64 advance.
- D025: explicit REST/TIE/OFF editor and playback semantics.
- D026: cursor-owned eight-row one-track tracker display and bounded redraw.
- Preserve the frozen host WAV unless a later intentional audio change records a superseding decision/test.
- No floating point in firmware DSP and no runtime 64-bit division.

## Hardware limitation still open

No physical FM-1 is currently available. Phase 2 does not claim real-hardware validation of install/boot, panel ergonomics, display readability/smoothness, output audio or measured AC791N callback/main-loop timing. Those remain explicit later hardware gates, especially Phase 7.

## Next phase

**Phase 3 — p-locks and tracker effects**

State: **READY TO START — no Phase 3 implementation has begun**

The first bounded package is WP3.1 — stable parameter ID registry. Follow the normal execution protocol: create/confirm the package issue, mark it IN PROGRESS, freeze any architectural contract needed before source code, and do not pull later Phase 3 packages forward.

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted future core direction after the p-lock and engine foundations. It must not be folded into WP3.1.

## Last relevant checkpoints

- Phase 1 closeout / Phase 2 branch start: `3ed4b65d7cda15c5831533f153f91e9014a9fde7`
- WP2.4 verified playback: `c8555ce8b68f5813e785b057386110cb177fb9d0`
- WP2.5 verified page input: `28d8ac051eb7107e745bb007e1a39014f51c1169`
- WP2.6 verified track selection: `707d0b24eb15c19193136f68e7c9f4e1d0a7dc7d`
- WP2.7 verified STEP REC: `790fb87596150567307f88fb537cd63134a3f5fc`
- WP2.8 verified special events: `c66405bead1c79fd891cf0d12c8bd241019a7aaf`
- WP2.9 verified screen: `e53d785b811371cf817d22be1fc8631b1e12cd12`
- WP2.10 / Phase 2 verified closeout: `7063be51e53c2657178084b3bef5ace813dafcd7`
- Phase 2 final Actions run: `40015242947`

## Last updated

2026-10-23 — Phase 2 verified complete. Phase 3 / WP3.1 is ready to start, but no Phase 3 implementation has begun.