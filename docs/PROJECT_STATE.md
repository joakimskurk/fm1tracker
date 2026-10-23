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

WP2.1 through WP2.4 are complete. WP2.5 is the only active implementation package.

## Current milestone

**WP2.5 - 16-step page input**

State: **IN PROGRESS - 2026-10-23**

GitHub issue: #13

WP2.5 start head:

`55d3ce02e96d5d9fe3090cc7b1a89fd1e067aef1`

### Exact objective

Add a bounded pattern-edit cursor/page input layer so the FM-1's 16 physical white keys address all 64 pattern steps through four 16-step pages without writing any pattern data. The package must preserve normal keyboard audition outside explicit SEQ/pattern-edit context and preserve the verified WP2.4 audible playback path.

### D022 page/input contract

D022 is accepted before implementation:

- cursor pages are `0..3` and local step positions are `0..15`;
- global selected step is always `page * 16 + local`, covering `0..63` exactly;
- physical white-key indexes are `0,2,4,6,7,9,11,12,14,16,18,19,21,23,24,26`, mapping in order to local steps `0..15`;
- `SEQ` enters pattern-edit input context;
- `HOME` returns to performance context;
- performance context keeps the existing 27-key audition/MIDI behavior;
- in pattern-edit context, white-key presses select cursor steps only and black-key presses are ignored until WP2.6;
- releases of manual notes started in performance context are still honored after a context change, preventing stuck notes;
- `OCT-` / `OCT+` navigate pages only in pattern-edit context;
- page navigation clamps at page 0 and page 3 rather than wrapping;
- changing page preserves the current local step index;
- reset begins in performance context at page 0 / local 0 / global step 0;
- no WP2.5 action mutates pattern data.

### Expected implementation files

- `firmware/src/fmt_pattern_input.h` / `fmt_pattern_input.c` - small deterministic cursor/context/key-map helper;
- `firmware/src/dinghy.c` - minimal SEQ/HOME context routing, pattern-mode white-key selection and context-sensitive OCT behavior while preserving performance releases;
- `tests/test_fmt_pattern_input.c` plus runner - all 64 mappings, all 27 physical key indexes, boundary/context/cursor/store-mutation tests;
- `.github/workflows/wp2_5_page_input_verify.yml` - scope, regressions, deterministic host audio, device build/safety/resource evidence;
- documentation/status files only as required for package closeout.

### Required verification

1. the 16 documented white physical keys map bijectively to local steps `0..15`;
2. four pages cover global steps `0..63` exactly once with no gaps or overlap;
3. all black/non-white note-key indexes are rejected as step selectors;
4. reset/context transitions are deterministic;
5. page navigation clamps at both boundaries and preserves local step position;
6. performance context does not lose the existing manual audition path;
7. entering SEQ while a performance note is held cannot create a stuck note when that key is released;
8. step selection and page navigation do not mutate `FmtSong` data;
9. WP2.1 model, WP2.2 store, WP2.3 transport and WP2.4 playback regressions remain green;
10. host playback WAV remains the frozen WP2.4 hash unless a separately documented non-audio reason proves a deliberate change;
11. full JieLi device build, package identity and inherited safety checks remain green;
12. image, normal-RAM and pool deltas are recorded against WP2.4.

### Strict non-goals

- no black-key track selection;
- no STEP REC or note writes;
- no REST/TIE/NOTE-OFF editing;
- no tracker grid/screen;
- no p-lock allocation/effects;
- no multitrack playback expansion;
- no new synth-engine work;
- no song/order mode or persistence;
- no HAL/loader/update redesign;
- do not begin WP2.6 in this package.

## Completed WP2.4 checkpoint

**WP2.4 - one-track audible playback**

State: **COMPLETE - 2026-10-23**

GitHub issue: #12

WP2.4 start head:

`64c4b7d260dca2471d7658a9a59bec23a24454a5`

Final verified implementation/workflow commit:

`c8555ce8b68f5813e785b057386110cb177fb9d0`

GitHub Actions:

- run: `39991490860`;
- job: `120544086455` (`one-track-playback`);
- result: **SUCCESS**.

Durable evidence: `docs/WP2_4_VERIFICATION.md`.

WP2.4 carried into WP2.5:

- D021 step-0/frame-0 playback contract;
- pattern 0 / track 0 proof playback only;
- one dedicated monophonic voice reusing the baseline integer sine primitive;
- deterministic 16-note proof fixture, one note every four steps;
- exact row timing inside normal 256-frame callbacks;
- continuous tracker playback prevents inherited flash settings writes from running while audio is active.

At 120 BPM, step 1 begins on frame 5,513 and the complete `63 -> 0` loop returns on frame 352,800 exactly.

Frozen deterministic host WAV:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

WP2.4 runtime baseline:

| Resource | Used | Limit | Free |
|---|---:|---:|---:|
| app image | 55,692 B | 581,564 B | 525,872 B |
| normal RAM (`.data + .bss`) | 12,396 B | 98,304 B | 85,908 B |
| pool RAM | 96,616 B | 344,064 B | 247,448 B |

Linked runtime symbols:

```text
01c20000 g       .pool        00010168 fmt_store_storage
01c0b060 g       .bss         00000008 fmt_transport_runtime
01c0b068 g       .bss         00000001 fmt_playback_runtime
```

Final WP2.4 artifacts:

- `fm1tracker.bin`: SHA-256 `09840601fc6d09f43f27a0f241bc7df371066904e4fa61caaeb33f891e3ed0aa`;
- `fm1tracker-0.1.fwsc`: SHA-256 `4853dfbb4bbcd0dd08a3a600f7e79a430bd59202e0e5ad022ae73bc68b0147fd`;
- package identity: `FM-1_987`.

Physical FM-1 install/boot/audio/panel behavior and actual callback execution time remain **NOT TESTED** because hardware is not currently available.

## Completed earlier checkpoints

- WP2.3 / issue #11: exact D020 integer transport. Evidence: `docs/WP2_3_VERIFICATION.md`; verified run `39988630290`.
- WP2.2 / issue #10: one 65,896-byte `FmtSong` at `.pool` `0x01C20000`, all 8,192 steps explicitly reset, bounds-safe runtime store. Evidence: `docs/WP2_2_VERIFICATION.md`.
- WP2.1 / issue #9: frozen 8-track × 64-step × 16-pattern compile-time core, 8-byte `FmtStep`, 4,096-byte `FmtPattern`, 65,896-byte `FmtSong`. Evidence: `docs/WP2_1_VERIFICATION.md`.

## Important decisions / do not regress

- D005: V0.1 dimensions remain 8 tracks × 64 steps × 16 patterns.
- D016: fixed package identity remains `FM-1_987`.
- D018: core in-memory model is frozen; persistence and p-lock allocator are separate.
- D019: exactly one runtime `FmtSong` lives in `.pool` with deterministic explicit reset.
- D020: transport uses the exact BPM-per-frame rational phase accumulator.
- D021: step 0 dispatch/frame timing and WP2.4 one-lane proof semantics are frozen.
- D022: WP2.5 page/context/key-addressing semantics are frozen as described above.
- Preserve WP2.1-WP2.4 regression coverage as later packages expand interaction.
- No floating point in firmware DSP and no runtime 64-bit division.
- No black-key track selection before WP2.6 and no note writing before WP2.7.

## Next action

Implement only WP2.5 according to issue #13 and D022. Close it only after all mapping/context/store-integrity tests, frozen playback regression, device safety and measured resource deltas pass. Then make WP2.6 the only READY package without implementing track selection.

## Last relevant checkpoints

- Phase 1 closeout / Phase 2 branch start: `3ed4b65d7cda15c5831533f153f91e9014a9fde7`
- WP2.1 verified implementation/workflow: `3e922da4a33894bc938e3252e575a2908355f3a3`
- WP2.2 verified implementation/workflow: `e180ac8cb5ba7a68fd3932883c988ad9ed67003b`
- WP2.3 verified implementation/workflow: `c83ee9cd835503eb410dd8110c2049351a645578`
- WP2.4 final verified implementation/workflow: `c8555ce8b68f5813e785b057386110cb177fb9d0`
- WP2.4 final Actions run: `39991490860`
- WP2.5 start: `55d3ce02e96d5d9fe3090cc7b1a89fd1e067aef1`

## Last updated

2026-10-23 - WP2.5 started. D022 is recorded before code. 16-step page input is the only active implementation package; WP2.6 has not started.