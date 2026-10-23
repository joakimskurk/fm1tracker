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

WP2.1 through WP2.6 are complete. WP2.7 is the only active implementation package.

## Current milestone

**WP2.7 - STEP REC note entry**

State: **IN PROGRESS - 2026-10-23**

GitHub issue: #15

WP2.7 start head:

`6996dc8a38f41ca8bf361c5e897b3f3342f03e4c`

### Exact objective

Add the first actual tracker edit operation: while explicit STEP REC is active and transport is stopped, write exactly one deterministic ordinary note to pattern 0 at the currently selected D022 global step and D023 logical track, then advance the cursor exactly one logical step.

### D024 STEP REC contract

D024 is accepted before implementation:

- REC enters STEP REC only when transport is stopped; REC while transport runs is ignored;
- successful REC entry forces D022 pattern/SEQ context while preserving selected page/local step and D023 track;
- REC exits STEP REC but remains in pattern context;
- HOME exits STEP REC and returns to PERFORMANCE;
- PLAY while STEP REC is active exits STEP REC before playback resumes; outside STEP REC PLAY toggles D020 stop/resume;
- all 27 note keys are pitch keys in STEP REC, not D022/D023 selectors;
- `OCT-` / `OCT+` control keyboard octave in STEP REC rather than page navigation;
- stored note equals audition note: `53 + 12 * octave + key_index`, with octave `-2..+2`, producing physical range 29..103;
- at the beginning of one unlatched gesture, the lowest newly pressed physical key is the single written note;
- the write gesture remains latched until all physical note keys are released, so extra presses can audition but cannot write or advance again;
- successful writes target pattern 0, current selected track and current global step;
- only `FmtStep.note` is modified; instrument, volume, effect, effect value, flags and lock-head are preserved;
- cursor advances exactly one global step modulo 64; step 63 wraps to step 0 / page 0 / local 0;
- inactive STEP REC, running transport, invalid note/cursor/track state or latched gesture produce no mutation and no cursor movement;
- REST/TIE/NOTE-OFF remain WP2.8.

### Expected implementation files

- `firmware/src/fmt_pattern_input.h` / `fmt_pattern_input.c` - add deterministic one-step cursor advance with `63 -> 0` wrap;
- `firmware/src/fmt_step_record.h` / `fmt_step_record.c` - small bounded STEP REC state and exact note-write gate;
- `firmware/src/dinghy.c` - REC/PLAY/HOME handling, STEP REC octave behavior, pitch audition and deterministic one-write gesture routing;
- `tests/test_fmt_step_record.c` / runner - exact write target, neighbor/field preservation, gating, latch and wrap tests;
- focused host gesture script/runner - prove stopped-transport REC entry, note audition, OCT behavior, REC exit and PLAY safety;
- `.github/workflows/wp2_7_step_record_verify.yml` - prior regressions, focused STEP REC tests, frozen host audio, device safety and measured resources;
- documentation/status files only as required at closeout.

### Required verification

1. no write while STEP REC is inactive;
2. no write while transport is running;
3. REC cannot enter while transport runs;
4. valid write changes exactly one selected pattern-0 track/step note byte;
5. all other fields in the target step are preserved;
6. neighboring tracks/steps remain unchanged;
7. note value equals deterministic physical audition pitch and stays `0..127`;
8. simultaneous key downs choose the lowest pitch for the single write;
9. additional presses before all keys release cannot write/advance again;
10. successful write advances exactly one global step;
11. step 63 wraps exactly to step 0 while preserving selected track;
12. STEP REC uses OCT buttons for pitch octave, while ordinary SEQ keeps D022 page navigation;
13. PERFORMANCE and ordinary SEQ behavior remain intact outside STEP REC;
14. held audition notes still receive NOTE OFF across mode changes;
15. WP2.1-WP2.6 regressions remain green;
16. the untouched default host run preserves the frozen WP2.4 WAV hash;
17. full JieLi build, identity and inherited safety gates remain green;
18. image/normal-RAM/pool delta is recorded against WP2.6.

### Strict WP2.7 non-goals

- no REST action;
- no TIE semantics;
- no explicit NOTE OFF editing;
- no tracker grid/screen;
- no general multitrack playback expansion;
- no p-lock allocation/effects;
- no new synth engine;
- no song/order mode or persistence;
- no HAL/loader/update redesign;
- do not begin WP2.8 in this package.

## Completed WP2.6 checkpoint

**WP2.6 - track selection prototype**

State: **COMPLETE - 2026-10-23**

GitHub issue: #14

WP2.6 start head:

`0e7d1794325a25e350ffefe2783ca84738a075c5`

Final verified implementation/workflow commit:

`707d0b24eb15c19193136f68e7c9f4e1d0a7dc7d`

GitHub Actions:

- run: `39995856373`;
- job: `120559070230` (`track-selection`);
- result: **SUCCESS**.

Durable evidence:

`docs/WP2_6_VERIFICATION.md`

### D023 verified behavior

- black-key indexes `1,3,5,8,10,13,15,17` map bijectively to logical tracks `0..7`;
- reserved black-key indexes `20,22,25` cannot select tracks;
- selector is active only in SEQ context;
- black keys still audition/send MIDI in PERFORMANCE;
- selected track persists across HOME/SEQ;
- track selection leaves page/local/global step unchanged;
- white-key/page navigation leaves selected track unchanged;
- full `FmtSong` remains byte-identical during navigation/selection;
- a PERFORMANCE note released after entering SEQ still receives its NOTE OFF;
- WP2.4 proof playback remains fixed to pattern 0 / track 0 and is unaffected by editor track selection.

### Frozen audio regression

Host WAV remains:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

### WP2.6 runtime baseline carried into WP2.7

| Resource | Used | Limit | Free |
|---|---:|---:|---:|
| app image | 56,060 B | 581,564 B | 525,504 B |
| normal RAM (`.data + .bss`) | 12,400 B | 98,304 B | 85,904 B |
| pool RAM | 96,616 B | 344,064 B | 247,448 B |

Linked runtime symbols:

```text
01c0b060 g       .bss         00000004 fmt_pattern_input_runtime
01c0b064 g       .bss         00000008 fmt_transport_runtime
01c0b06c g       .bss         00000001 fmt_playback_runtime
01c20000 g       .pool        00010168 fmt_store_storage
```

WP2.6 delta vs WP2.5:

- image: **+80 B**;
- normal RAM: **+0 B**;
- pool RAM: **+0 B**.

Final WP2.6 artifacts:

- `fm1tracker.bin`: SHA-256 `ed87a905eeba8d976d0ac965b8df2b53b6a9bcc6d21754f0c89276d820f307fa`;
- `fm1tracker-0.1.fwsc`: SHA-256 `af64a8b4a8c4716dcde0ddaeebc4cad8c80ffcdf573d8ef0ff60d5d0c55be879`;
- package identity: `FM-1_987`.

Evidence artifact:

- artifact ID `12519971447`;
- ZIP SHA-256 `5d87727a17446a455462fee9708c3c79d3f032ffb73562f7c7e0f9b9acbc66cb`.

Physical FM-1 black-key ergonomics, page/track discoverability, install/boot/audio behavior and actual callback execution time remain **NOT TESTED** because hardware is not currently available.

## Completed earlier checkpoints

- WP2.5 / issue #13: D022 four-page white-key step addressing. Evidence: `docs/WP2_5_VERIFICATION.md`; run `39994230797`.
- WP2.4 / issue #12: deterministic audible one-track playback, D021 exact frame timing. Evidence: `docs/WP2_4_VERIFICATION.md`; run `39991490860`.
- WP2.3 / issue #11: exact D020 integer transport. Evidence: `docs/WP2_3_VERIFICATION.md`; run `39988630290`.
- WP2.2 / issue #10: one 65,896-byte `FmtSong` at `.pool` `0x01C20000`. Evidence: `docs/WP2_2_VERIFICATION.md`.
- WP2.1 / issue #9: frozen V0.1 core model. Evidence: `docs/WP2_1_VERIFICATION.md`.

## Important decisions / do not regress

- D005: V0.1 dimensions remain 8 tracks × 64 steps × 16 patterns.
- D006: STEP REC is the primary recording workflow and must advance deterministically.
- D009/D023: exact WP2.6 black-key track mapping remains the current provisional hardware prototype.
- D016: fixed package identity remains `FM-1_987`.
- D018: core in-memory model is frozen; persistence and p-lock allocator are separate.
- D019: exactly one runtime `FmtSong` lives in `.pool` with deterministic explicit reset.
- D020: exact rational integer transport remains frozen.
- D021: frame-0 one-lane proof playback timing remains frozen.
- D022: explicit PERFORMANCE/SEQ context and white-key four-page addressing remain frozen for the current prototype layer.
- D023: selected track is bounded `0..7`, SEQ-only and editor-only.
- D024: STEP REC is stopped-transport, one-note-per-gesture note entry with exact modulo-64 cursor advance.
- Preserve WP2.1-WP2.6 regression coverage.
- No floating point in firmware DSP and no runtime 64-bit division.

## Next action

Implement only WP2.7 according to issue #15 and D024. Add bounded STEP REC editor state, deterministic note-only writes and modulo-64 cursor advance, then verify focused host behavior plus all inherited regressions/device budgets. Do **not** begin WP2.8 REST/TIE/NOTE-OFF semantics.

## Last relevant checkpoints

- Phase 1 closeout / Phase 2 branch start: `3ed4b65d7cda15c5831533f153f91e9014a9fde7`
- WP2.1 verified implementation/workflow: `3e922da4a33894bc938e3252e575a2908355f3a3`
- WP2.2 verified implementation/workflow: `e180ac8cb5ba7a68fd3932883c988ad9ed67003b`
- WP2.3 verified implementation/workflow: `c83ee9cd835503eb410dd8110c2049351a645578`
- WP2.4 verified implementation/workflow: `c8555ce8b68f5813e785b057386110cb177fb9d0`
- WP2.5 verified implementation/workflow: `28d8ac051eb7107e745bb007e1a39014f51c1169`
- WP2.6 start: `0e7d1794325a25e350ffefe2783ca84738a075c5`
- WP2.6 verified implementation/workflow: `707d0b24eb15c19193136f68e7c9f4e1d0a7dc7d`
- WP2.6 final Actions run: `39995856373`
- WP2.7 start: `6996dc8a38f41ca8bf361c5e897b3f3342f03e4c`

## Last updated

2026-10-23 - WP2.7 started. D024 is recorded before source code. STEP REC note entry is the only active implementation package; WP2.8 has not started.
