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

WP2.1 through WP2.4 are complete. No WP2.5 implementation has started yet.

## Current milestone

**WP2.5 - 16-step page input**

State: **READY TO START**

GitHub issue: #13

Goal from the approved Phase 2 sequence:

Add bounded edit-cursor/page addressing so the FM-1's 16 white keys can select all 64 pattern steps through four 16-step pages. Step selection is address/navigation only: no note writes, STEP REC, black-key track selection or tracker grid yet.

Before WP2.5 code changes, follow `EXECUTION_PROTOCOL.md`: mark WP2.5 `IN PROGRESS`, record exact page-boundary behavior and state/API/tests, then implement only the page-input package.

Control-map constraints already documented:

- white keys in SEQ/pattern-edit context represent the current 16-step page;
- pages are 01-16, 17-32, 33-48 and 49-64;
- `OCT-` / `OCT+` are provisional previous/next page controls in SEQ mode and not STEP REC;
- normal performance audition outside the explicit pattern-edit context must not be silently replaced;
- WP2.5 selects steps only and must not modify pattern note data.

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

Durable evidence:

`docs/WP2_4_VERIFICATION.md`

### What WP2.4 implemented

- D021 step-0/frame-0 playback contract;
- pattern 0 / track 0 proof playback only;
- one dedicated monophonic voice reusing the baseline integer sine primitive;
- deterministic 16-note proof fixture, one note every four steps;
- exact row timing inside normal 256-frame callbacks;
- safe EMPTY/TIE/OFF handling appropriate to the WP2.4 proof;
- startup handoff preventing the audio IRQ from reading tracker store before initialization;
- continuous tracker playback is counted as non-quiet so the inherited flash erase/save path cannot stall audible tracker output.

### Exact timing evidence

At 120 BPM:

- step 0 is active before playback frame 0;
- step 1 begins on frame 5,513;
- full `63 -> 0` loop returns on frame 352,800 exactly;
- callback grouping does not alter the event trace.

### Deterministic audible host evidence

Final host WAV:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

The first 100 ms contains 8,820 non-zero stereo samples before any simulated manual key input, proving that the tracker lane itself is audible.

### WP2.4 measured resources

| Resource | WP2.3 baseline | WP2.4 | Delta |
|---|---:|---:|---:|
| app image | 55,328 B | **55,692 B** | **+364 B** |
| normal RAM (`.data + .bss`) | 12,376 B | **12,396 B** | **+20 B** |
| pool RAM | 96,616 B | **96,616 B** | **0 B** |

Remaining physical pool: **247,448 B**.

Linked runtime symbols:

```text
01c20000 g       .pool        00010168 fmt_store_storage
01c0b060 g       .bss         00000008 fmt_transport_runtime
01c0b068 g       .bss         00000001 fmt_playback_runtime
```

Final artifacts:

- `fm1tracker.bin`: SHA-256 `09840601fc6d09f43f27a0f241bc7df371066904e4fa61caaeb33f891e3ed0aa`;
- `fm1tracker-0.1.fwsc`: SHA-256 `4853dfbb4bbcd0dd08a3a600f7e79a430bd59202e0e5ad022ae73bc68b0147fd`;
- package identity: `FM-1_987`.

Actions evidence artifact:

- artifact ID `12517493683`;
- size 1,106,447 B;
- ZIP SHA-256 `026cbe66f44a066b7f198380081b62f2c10c3aa169330a680103fafe78734e15`.

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
- Preserve WP2.1-WP2.4 regression coverage as later packages expand interaction.
- No floating point in firmware DSP and no runtime 64-bit division.
- No black-key track selection before WP2.6 and no note writing before WP2.7.

## WP2.5 strict non-goals

- no black-key track selection;
- no STEP REC or note writes;
- no REST/TIE/NOTE-OFF editing;
- no tracker grid/screen;
- no p-lock allocation/effects;
- no multitrack playback expansion;
- no new synth-engine work;
- no song/order mode or persistence;
- no HAL/loader/update redesign.

## Next action

Start WP2.5 / issue #13 as its own package. Decide and document page-boundary behavior before code, then implement only deterministic SEQ-context white-key step addressing across four pages. Do **not** begin WP2.6 in the same package.

## Last relevant checkpoints

- Phase 1 closeout / Phase 2 branch start: `3ed4b65d7cda15c5831533f153f91e9014a9fde7`
- WP2.1 verified implementation/workflow: `3e922da4a33894bc938e3252e575a2908355f3a3`
- WP2.2 verified implementation/workflow: `e180ac8cb5ba7a68fd3932883c988ad9ed67003b`
- WP2.3 verified implementation/workflow: `c83ee9cd835503eb410dd8110c2049351a645578`
- WP2.4 start: `64c4b7d260dca2471d7658a9a59bec23a24454a5`
- WP2.4 final verified implementation/workflow: `c8555ce8b68f5813e785b057386110cb177fb9d0`
- WP2.4 final Actions run: `39991490860`

## Last updated

2026-10-23 - WP2.4 completed and verified. WP2.5 / issue #13 is READY TO START; no WP2.5 implementation has been written yet.