# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and relevant source/history before changing code.

## Current phase

**Phase 2 - tracker core without synth ambition**

State: **IN PROGRESS**

Phase 2 parent issue: #8

Current branch: `phase2/tracker-core`

Phase 2 branch start / completed Phase 1 head:

`3ed4b65d7cda15c5831533f153f91e9014a9fde7`

WP2.1 through WP2.5 are complete. WP2.6 is the only active implementation package.

## Current milestone

**WP2.6 - track selection prototype**

State: **IN PROGRESS - 2026-10-23**

GitHub issue: #14

WP2.6 start head:

`0e7d1794325a25e350ffefe2783ca84738a075c5`

### Exact objective

Extend the bounded D022 pattern-edit cursor so eight designated physical black keys select the eight logical tracker tracks inside explicit SEQ/pattern-edit context. Track selection changes edit/cursor target only. WP2.6 must not write notes, mutate song data or expand the WP2.4 one-track proof into general multitrack playback.

### D023 track-selection contract

D023 is accepted before implementation:

- selected logical track is `0..7`, with reset selecting track 0 / UI track 1;
- physical black-key indexes `1,3,5,8,10,13,15,17` map in order to logical tracks `0..7`;
- these correspond to F#3, G#3, A#3, C#4, D#4, F#4, G#4 and A#4 on the FM-1 F3..G5 key span;
- remaining black-key indexes `20,22,25` are reserved and rejected in WP2.6;
- track selection is active only in D022 SEQ/pattern-edit context;
- PERFORMANCE context keeps normal audition/MIDI behavior for every note key;
- HOME/SEQ context changes preserve the currently selected track;
- selecting a track does not change page, local step or derived global step;
- selecting a track does not mutate `FmtSong`;
- WP2.4 proof playback remains fixed to pattern 0 / logical track 0 and ignores the editor-selected track;
- D022 white-key/page semantics remain unchanged.

### Expected implementation files

- `firmware/src/fmt_pattern_input.h` / `fmt_pattern_input.c` - add bounded selected-track state and black-key mapping/select API;
- `firmware/src/dinghy.c` - route black-key downs in SEQ to track selection while preserving white-key selection and performance audition/release ownership;
- `tests/test_fmt_pattern_input.c` - all eight track mappings, three reserved black keys, invalid indexes, context preservation, white-key/page regression and song-store integrity;
- `tests/run_pattern_context_host.sh` / host script as needed - prove black keys audition in PERFORMANCE but are consumed as track selectors in SEQ with safe releases;
- `.github/workflows/wp2_6_track_selection_verify.yml` - frozen prior regressions, host context/audio, device build/safety/resource evidence;
- documentation/status files only as required for closeout.

### Required verification

1. eight designated black keys map bijectively to logical tracks `0..7`;
2. reserved black keys `20,22,25` cannot select a track;
3. invalid indexes cannot alter selected track;
4. selector API is inactive outside explicit SEQ context;
5. selected track survives HOME/SEQ context changes;
6. track selection leaves page/local/global step unchanged;
7. all D022 white-key and four-page mappings remain green;
8. track selection and page/step navigation leave `FmtSong` byte-identical;
9. PERFORMANCE black keys still audition/send MIDI normally;
10. manual notes started in PERFORMANCE still receive NOTE OFF across a SEQ context change;
11. WP2.1 model, WP2.2 store, WP2.3 transport, WP2.4 playback and WP2.5 page-input regressions remain green;
12. host playback WAV remains frozen at the WP2.4 hash;
13. full JieLi device build, package identity and inherited safety checks remain green;
14. image, normal-RAM and pool deltas are recorded against WP2.5.

### Strict non-goals

- no STEP REC or note writes;
- no REST/TIE/NOTE-OFF editing;
- no tracker grid/screen;
- no general multitrack playback expansion;
- no p-lock allocation/effects;
- no new synth-engine work;
- no song/order mode or persistence;
- no HAL/loader/update redesign;
- do not begin WP2.7 in this package.

## Completed WP2.5 checkpoint

**WP2.5 - 16-step page input**

State: **COMPLETE - 2026-10-23**

GitHub issue: #13

WP2.5 start head:

`55d3ce02e96d5d9fe3090cc7b1a89fd1e067aef1`

Final verified implementation/workflow commit:

`28d8ac051eb7107e745bb007e1a39014f51c1169`

GitHub Actions:

- run: `39994230797`;
- job: `120553466206` (`page-input`);
- result: **SUCCESS**.

Durable evidence: `docs/WP2_5_VERIFICATION.md`.

D022 carried forward:

- reset starts in PERFORMANCE context, page 0, local step 0;
- `SEQ` enters pattern context and `HOME` returns to performance;
- physical white-key indexes `0,2,4,6,7,9,11,12,14,16,18,19,21,23,24,26` map to local steps `0..15`;
- global selected step is `page * 16 + local`;
- four pages cover steps `0..63` exactly;
- `OCT-` / `OCT+` page navigation clamps at 0/3 and preserves local position;
- performance audition/MIDI and release ownership remain safe across context changes;
- selection/page movement cannot mutate `FmtSong`.

Frozen deterministic host WAV carried forward:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

WP2.5 runtime baseline carried into WP2.6:

| Resource | Used | Limit | Free |
|---|---:|---:|---:|
| app image | 55,980 B | 581,564 B | 525,584 B |
| normal RAM (`.data + .bss`) | 12,400 B | 98,304 B | 85,904 B |
| pool RAM | 96,616 B | 344,064 B | 247,448 B |

Linked runtime symbols:

```text
01c0b060 g       .bss         00000003 fmt_pattern_input_runtime
01c0b064 g       .bss         00000008 fmt_transport_runtime
01c0b06c g       .bss         00000001 fmt_playback_runtime
01c20000 g       .pool        00010168 fmt_store_storage
```

Final WP2.5 artifacts:

- `fm1tracker.bin`: SHA-256 `ed16268fb4c2599342986d8e2588e6f90bf7229ec9d0d9e2cc92ff6378aaed30`;
- `fm1tracker-0.1.fwsc`: SHA-256 `7d215b4b78e3232f670a6d146b27c4ae5064d069dfdeac40df8468c8016697a2`;
- package identity: `FM-1_987`.

WP2.5 resource delta vs WP2.4:

- image: **+288 B**;
- normal RAM: **+4 B**;
- pool RAM: **+0 B**.

## Completed earlier checkpoints

- WP2.4 / issue #12: deterministic audible one-track playback, D021 exact frame timing. Evidence: `docs/WP2_4_VERIFICATION.md`; verified run `39991490860`.
- WP2.3 / issue #11: exact D020 integer transport. Evidence: `docs/WP2_3_VERIFICATION.md`; verified run `39988630290`.
- WP2.2 / issue #10: one 65,896-byte `FmtSong` at `.pool` `0x01C20000`, all 8,192 steps explicitly reset, bounds-safe runtime store. Evidence: `docs/WP2_2_VERIFICATION.md`.
- WP2.1 / issue #9: frozen 8-track × 64-step × 16-pattern compile-time core, 8-byte `FmtStep`, 4,096-byte `FmtPattern`, 65,896-byte `FmtSong`. Evidence: `docs/WP2_1_VERIFICATION.md`.

## Important decisions / do not regress

- D005: V0.1 dimensions remain 8 tracks × 64 steps × 16 patterns.
- D009/D023: black-key track selection is a provisional hardware prototype with exact WP2.6 mapping; Phase 7 may supersede ergonomics.
- D016: fixed package identity remains `FM-1_987`.
- D018: core in-memory model is frozen; persistence and p-lock allocator are separate.
- D019: exactly one runtime `FmtSong` lives in `.pool` with deterministic explicit reset.
- D020: transport uses the exact BPM-per-frame rational phase accumulator.
- D021: step 0 dispatch/frame timing and WP2.4 one-lane proof semantics remain frozen.
- D022: explicit PERFORMANCE/SEQ context and white-key four-page addressing remain frozen for the current prototype layer.
- Preserve WP2.1-WP2.5 regression coverage as WP2.6 expands editor addressing.
- No floating point in firmware DSP and no runtime 64-bit division.
- No note writing before WP2.7.
- Physical FM-1 install/boot/audio/panel behavior, page/track ergonomics and actual callback execution time remain **NOT TESTED** because hardware is not currently available.

## Next action

Implement only WP2.6 according to issue #14 and D023. Close it only after all track/white-key/context/store-integrity regressions, frozen playback audio, device safety and measured resource deltas pass. Then make WP2.7 the only READY package without implementing STEP REC in the same change.

## Last relevant checkpoints

- Phase 1 closeout / Phase 2 branch start: `3ed4b65d7cda15c5831533f153f91e9014a9fde7`
- WP2.1 verified implementation/workflow: `3e922da4a33894bc938e3252e575a2908355f3a3`
- WP2.2 verified implementation/workflow: `e180ac8cb5ba7a68fd3932883c988ad9ed67003b`
- WP2.3 verified implementation/workflow: `c83ee9cd835503eb410dd8110c2049351a645578`
- WP2.4 verified implementation/workflow: `c8555ce8b68f5813e785b057386110cb177fb9d0`
- WP2.4 final Actions run: `39991490860`
- WP2.5 start: `55d3ce02e96d5d9fe3090cc7b1a89fd1e067aef1`
- WP2.5 verified implementation/workflow: `28d8ac051eb7107e745bb007e1a39014f51c1169`
- WP2.5 final Actions run: `39994230797`
- WP2.6 start: `0e7d1794325a25e350ffefe2783ca84738a075c5`

## Last updated

2026-10-23 - WP2.6 started. D023 is recorded before code. Black-key track selection is the only active implementation package; WP2.7 has not started.
