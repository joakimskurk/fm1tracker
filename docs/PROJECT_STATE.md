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

WP2.1 through WP2.7 are complete. No WP2.8 implementation has started.

## Current milestone

**WP2.8 - REST / TIE / NOTE-OFF semantics**

State: **READY TO START**

GitHub issue: #16

### Exact objective

Define and implement explicit editor + playback semantics for the frozen special note values `EMPTY`, `TIE` and `NOTE OFF` on top of the verified WP2.7 stopped-transport STEP REC path.

### Before WP2.8 source code

Follow `EXECUTION_PROTOCOL.md` and record a new decision before implementation that freezes:

1. exact REST write semantics, including which target-step fields are preserved or cleared;
2. exact NOTE OFF editing gesture and whether provisional `FX + SELECT` remains the mapping;
3. exact TIE editing gesture — this is **not yet frozen and must not be guessed**;
4. playback meaning of EMPTY, TIE and NOTE OFF for the existing monophonic proof lane;
5. exact cursor advance/wrap behavior for each special event;
6. interaction with D024 gesture latching, REC exit and stopped-transport safety.

### Strict WP2.8 non-goals

- no tracker grid/screen — WP2.9;
- no general multitrack playback expansion;
- no p-lock allocation/effects;
- no new synth engine;
- no song/order mode or persistence;
- no HAL/loader/update redesign;
- do not begin WP2.9 in the same package.

## Completed WP2.7 checkpoint

**WP2.7 - STEP REC note entry**

State: **COMPLETE - 2026-10-23**

GitHub issue: #15

WP2.7 start head:

`6996dc8a38f41ca8bf361c5e897b3f3342f03e4c`

Final verified implementation/workflow commit:

`790fb87596150567307f88fb537cd63134a3f5fc`

GitHub Actions:

- run: `39999680852`;
- job: `120572238726` (`step-record`);
- result: **SUCCESS**.

Durable evidence:

`docs/WP2_7_VERIFICATION.md`

### D024 verified STEP REC behavior

- REC enters STEP REC only after audio has acknowledged a stopped D020 transport;
- REC while transport is running is ignored;
- successful REC entry forces/keeps pattern context while preserving current page/local step and selected track;
- REC exits STEP REC and remains in ordinary SEQ/pattern context;
- HOME exits STEP REC and returns to PERFORMANCE;
- PLAY while STEP REC is active exits STEP REC before playback resumes;
- outside STEP REC, PLAY requests stop/resume without changing D020 timing semantics;
- main owns only a bounded play-request mailbox; audio remains the sole writer of the live transport running state and publishes the stopped acknowledgement;
- all 27 physical note keys are pitch input in STEP REC rather than step/track selectors;
- OCT-/OCT+ control pitch octave in STEP REC, while ordinary SEQ keeps D022 page navigation;
- stored pitch is identical to audition pitch: `53 + 12 * octave + key_index`, with octave `-2..+2` and physical note range 29..103;
- simultaneous key downs deterministically choose the lowest physical key/lowest pitch for the single write;
- the write gesture latches until all physical note keys are released; extra held/staggered presses can audition but cannot write or advance again;
- successful writes target pattern 0, current selected track and current global step;
- only `FmtStep.note` changes; instrument, volume, effect, effect value, flags and lock-head remain unchanged;
- successful writes advance exactly one global step modulo 64;
- step 63 wraps to step 0 / page 0 / local step 0 while selected track is preserved;
- REST/TIE/NOTE-OFF were deliberately not implemented in WP2.7.

### Frozen regressions

All WP2.1-WP2.6 host suites remained green. The focused full-app host fixtures also pass:

```text
WP2_5_PATTERN_CONTEXT_HOST_OK=1
WP2_6_TRACK_CONTEXT_HOST_OK=1
WP2_7_STEP_REC_CONTEXT_HOST_OK=1
```

Frozen default host WAV remains byte-identical:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

### WP2.7 runtime baseline carried into WP2.8

| Resource | Used | Limit | Free |
|---|---:|---:|---:|
| app image | 56,548 B | 581,564 B | 525,016 B |
| normal RAM (`.data + .bss`) | 12,404 B | 98,304 B | 85,900 B |
| pool RAM | 96,616 B | 344,064 B | 247,448 B |

Linked runtime symbols:

```text
01c0b060 g       .bss         00000002 fmt_step_record_runtime
01c0b062 g       .bss         00000004 fmt_pattern_input_runtime
01c0b068 g       .bss         00000008 fmt_transport_runtime
01c0b070 g       .bss         00000001 fmt_playback_runtime
01c20000 g       .pool        00010168 fmt_store_storage
```

WP2.7 delta vs WP2.6:

- image: **+488 B**;
- normal RAM: **+4 B**;
- pool RAM: **+0 B**.

Final WP2.7 artifacts:

- `fm1tracker.bin`: SHA-256 `9d0c198b7428996c8147eb6bab085ad3580dc432291b2e9e56114a65d1dfaeb4`;
- `fm1tracker-0.1.fwsc`: SHA-256 `9eb9c278175ac2aac693bc70cea4b7af1a5cef7b87fc9c845c68e4d6f1b73130`;
- host WAV: SHA-256 `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`;
- package identity: `FM-1_987`.

Evidence artifact:

- artifact ID `12521283164`;
- ZIP SHA-256 `d74cd8b38687ef8802b840beaf626565a3f552900deccafe605e3069df587224`;
- size 1,110,401 B.

Physical FM-1 STEP REC/chord ergonomics, install/boot/audio behavior and actual callback execution time remain **NOT TESTED** because hardware is not currently available.

## Completed earlier checkpoints

- WP2.6 / issue #14: D023 eight-black-key track-selection prototype. Evidence: `docs/WP2_6_VERIFICATION.md`; run `39995856373`.
- WP2.5 / issue #13: D022 four-page white-key step addressing. Evidence: `docs/WP2_5_VERIFICATION.md`; run `39994230797`.
- WP2.4 / issue #12: deterministic audible one-track playback, D021 exact frame timing. Evidence: `docs/WP2_4_VERIFICATION.md`; run `39991490860`.
- WP2.3 / issue #11: exact D020 integer transport. Evidence: `docs/WP2_3_VERIFICATION.md`; run `39988630290`.
- WP2.2 / issue #10: one 65,896-byte `FmtSong` at `.pool` `0x01C20000`. Evidence: `docs/WP2_2_VERIFICATION.md`.
- WP2.1 / issue #9: frozen V0.1 core model. Evidence: `docs/WP2_1_VERIFICATION.md`.

## Important decisions / do not regress

- D005: V0.1 dimensions remain 8 tracks × 64 steps × 16 patterns.
- D006: STEP REC is the primary recording workflow and advances deterministically.
- D007: one-action REST exists conceptually; implementation begins only in WP2.8.
- D009/D023: exact black-key track mapping remains the current provisional prototype.
- D016: fixed package identity remains `FM-1_987`.
- D018: core in-memory model is frozen; persistence and p-lock allocator are separate.
- D019: exactly one runtime `FmtSong` lives in `.pool` with deterministic explicit reset.
- D020: exact rational integer transport remains frozen.
- D021: frame-0 one-lane proof playback timing remains frozen.
- D022: explicit PERFORMANCE/SEQ context and white-key four-page addressing remain frozen for the current prototype layer.
- D023: selected track is bounded `0..7`, SEQ-only and editor-only.
- D024: STEP REC is stopped-transport, one-note-per-fully-released-gesture note entry with exact modulo-64 cursor advance.
- Preserve WP2.1-WP2.7 regression coverage as WP2.8 adds special events.
- No floating point in firmware DSP and no runtime 64-bit division.

## Next action

Start issue #16 as its own package. Before code, mark WP2.8 IN PROGRESS and record exact REST/TIE/NOTE-OFF editor and playback semantics, especially the currently-unfrozen TIE gesture. Verify special-event write isolation, exact one-step cursor advance/wrap, playback event semantics at D020/D021 boundaries, old STEP REC behavior, frozen regressions, device safety and measured resources. Do not begin WP2.9.

## Last relevant checkpoints

- Phase 1 closeout / Phase 2 branch start: `3ed4b65d7cda15c5831533f153f91e9014a9fde7`
- WP2.1 verified implementation/workflow: `3e922da4a33894bc938e3252e575a2908355f3a3`
- WP2.2 verified implementation/workflow: `e180ac8cb5ba7a68fd3932883c988ad9ed67003b`
- WP2.3 verified implementation/workflow: `c83ee9cd835503eb410dd8110c2049351a645578`
- WP2.4 verified implementation/workflow: `c8555ce8b68f5813e785b057386110cb177fb9d0`
- WP2.5 verified implementation/workflow: `28d8ac051eb7107e745bb007e1a39014f51c1169`
- WP2.6 verified implementation/workflow: `707d0b24eb15c19193136f68e7c9f4e1d0a7dc7d`
- WP2.7 start: `6996dc8a38f41ca8bf361c5e897b3f3342f03e4c`
- WP2.7 verified implementation/workflow: `790fb87596150567307f88fb537cd63134a3f5fc`
- WP2.7 final Actions run: `39999680852`

## Last updated

2026-10-23 - WP2.7 closed and durably verified. WP2.8 / issue #16 is READY TO START; no WP2.8 implementation has started.
