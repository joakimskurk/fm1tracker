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

WP2.1 through WP2.5 are complete. No WP2.6 implementation has started.

## Current milestone

**WP2.6 - track selection prototype**

State: **READY TO START**

GitHub issue: #14

### Exact objective

Prototype direct selection of the eight logical tracker tracks using eight suitable physical black keys inside the explicit D022 SEQ/pattern-edit context. The package must preserve the verified white-key/page addressing, normal performance audition and frozen WP2.4 audible proof. Track selection changes only edit/cursor target in WP2.6; it must not silently expand the one-track proof into general multitrack playback.

### Before code

1. mark WP2.6 IN PROGRESS and record its start head;
2. record the exact eight physical black-key indexes and track mapping as a new decision before implementation;
3. define selected-track reset/context behavior and reserved black-key handling;
4. keep D022 white-key/page semantics unchanged;
5. preserve performance-note release safety across context switches;
6. define host/store-integrity/device/resource tests before implementation spreads.

### Strict non-goals

- no STEP REC or note writes — WP2.7;
- no REST/TIE/NOTE-OFF editing — WP2.8;
- no tracker grid/screen — WP2.9;
- no general multitrack playback expansion;
- no p-lock allocation/effects;
- no new 8-BIT/FM4 engine work;
- no song/order mode or persistence;
- no HAL/loader/update redesign;
- do not begin WP2.7 in the same package.

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
- white-key presses in SEQ select cursor only;
- black-key presses are rejected until WP2.6;
- performance audition/MIDI remains unchanged outside SEQ;
- manual notes started in performance still receive NOTE OFF if released after entering SEQ;
- selection/page movement cannot mutate `FmtSong`.

The frozen WP2.4 audible host WAV remains byte-identical:

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
- D009: black-key track selection remains provisional and must be prototyped before hardware UX freeze.
- D016: fixed package identity remains `FM-1_987`.
- D018: core in-memory model is frozen; persistence and p-lock allocator are separate.
- D019: exactly one runtime `FmtSong` lives in `.pool` with deterministic explicit reset.
- D020: transport uses the exact BPM-per-frame rational phase accumulator.
- D021: step 0 dispatch/frame timing and WP2.4 one-lane proof semantics remain frozen.
- D022: explicit PERFORMANCE/SEQ context and white-key four-page addressing remain frozen for the current prototype layer.
- Preserve WP2.1-WP2.5 regression coverage as later packages expand interaction.
- No floating point in firmware DSP and no runtime 64-bit division.
- No note writing before WP2.7.
- Physical FM-1 install/boot/audio/panel behavior, page ergonomics and actual callback execution time remain **NOT TESTED** because hardware is not currently available.

## Next action

Start issue #14 as its own package. Before code, mark WP2.6 IN PROGRESS and record the exact eight-black-key mapping plus selected-track semantics. Verify all eight track mappings, reserved black keys, D022 white-key/page regression, no song mutation, performance-context safety, frozen audible host output, device safety and measured resource delta. Do not begin WP2.7.

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

## Last updated

2026-10-23 - WP2.5 closed and durably verified. WP2.6 / issue #14 is READY TO START; no WP2.6 implementation has started.