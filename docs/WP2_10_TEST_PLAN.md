# WP2.10 Test Plan — Phase 2 integration and closeout

Date: 2026-10-23  
Status: **FROZEN BEFORE INTEGRATION TEST IMPLEMENTATION**

WP2.10 start head:

`47e9093ae300043b804f85ce62fee1fe758fc943`

Issue: #18

## Purpose

WP2.10 adds no product semantics. It proves that the already accepted D018–D026 implementation composes correctly as one Phase 2 workflow and produces durable Phase 2 closeout evidence.

No D027 is required unless this integration work discovers a real architectural conflict or changes user-visible/runtime semantics. A defect fix, if required, must be the smallest demonstrated fix and must be documented before it spreads.

## Two-layer integration fixture

The integration proof deliberately uses two complementary host layers so production firmware does not need test-only state-dump hooks.

### Layer A — direct Phase 2 C integration test

A single host C test links the real Phase 2 modules together:

- `fmt_store`;
- `fmt_transport`;
- `fmt_playback`;
- `fmt_pattern_input`;
- `fmt_step_record`;
- `fmt_tracker_screen`.

It uses the same public/module APIs as the firmware and inspects the real pool-style `FmtSong` store directly. A complete post-seed baseline song is copied before editing; an expected copy is then modified only at the deliberately edited rows. The final real song must compare byte-for-byte equal to that expected song.

This layer proves state composition, complete-store isolation and exact final editor/display/transport state without changing `dinghy.c` or adding a host-only firmware backdoor.

### Layer B — unchanged full-app `dinghy.c` host scenario

The normal `DINGHY_HOST` application is driven through the existing panel script interface. This verifies that real main-loop button/note handling reaches the same Phase 2 workflow, that the D026 screen presents the integration state, that PLAY resumes the proof lane, and that HOME reclaims the screen cleanly.

No production source change is expected for this layer.

## Layer A deterministic scenario

### Initial state

1. `fmt_store_reset()`.
2. Seed the existing WP2.4 proof pattern on pattern 0 / track 0.
3. Snapshot the entire 65,896-byte `FmtSong` as the immutable integration baseline.
4. Reset transport, playback, pattern input, STEP REC and tracker-screen presentation state.

### Complete 64-step address traversal

Enter pattern context and use the frozen D022 physical white-key mapping.

For each page 0..3:

- select all sixteen physical white keys in order;
- assert exact global steps `page * 16 + local`;
- mark each global address in a 64-entry seen table;
- assert every address 0..63 is visited exactly once;
- navigate to the next page only with the existing clamped page API.

After traversal:

- cursor is exactly step 63 / page 3 / local 15;
- the entire `FmtSong` is still byte-identical to the seeded baseline.

### Complete track-selector pass

Use the eight frozen D023 black-key selectors in order and assert they select logical tracks 0..7 bijectively without changing the selected step or song bytes.

Leave logical track 7 selected for all integration edits. This intentionally keeps the WP2.4 audible proof lane on track 0 untouched.

### STEP REC stop gate and 63 -> 0 wrap

1. Start the D020 transport and prove STEP REC entry is rejected while running.
2. Stop the transport and enter D024 STEP REC successfully.
3. Write MIDI note 53 at track 7 / step 63.
4. Release the write gesture.
5. Assert exact cursor wrap to page 0 / local 0 / global step 0.
6. Write MIDI note 55 at track 7 / step 0 and release the gesture.

### Non-wrapping page-boundary advance and special events

Exit STEP REC but remain in pattern context.

1. Select page 0 / local 15 / global step 15 using the D022 white-key mapping.
2. Re-enter STEP REC while transport remains stopped.
3. Write MIDI note 60 at step 15 and assert automatic advance to step 16 / page 1 / local 0.
4. Write REST / EMPTY at step 16 through the D025 FX press/release grammar.
5. Write TIE at step 17 through SELECT.
6. Write NOTE OFF at step 18 through FX-held then SELECT; later FX release must not create another write.
7. Write MIDI note 64 at step 19.
8. Final edit cursor must be exactly global step 20 / page 1 / local 4, selected logical track 7, STEP REC active.

The final visible D026 window is therefore steps 16..23 and must contain, in order:

- step 16: `---` REST/EMPTY;
- step 17: `===` TIE;
- step 18: `OFF`;
- step 19: ordinary MIDI 64 / `E-4`;
- step 20: edit cursor, still empty.

### Exact final store expectation

Relative to the seeded baseline, only these note bytes may differ:

- pattern 0 / track 7 / step 63 = 53;
- pattern 0 / track 7 / step 0 = 55;
- pattern 0 / track 7 / step 15 = 60;
- pattern 0 / track 7 / step 16 = `FMT_NOTE_EMPTY`;
- pattern 0 / track 7 / step 17 = `FMT_NOTE_TIE`;
- pattern 0 / track 7 / step 18 = `FMT_NOTE_OFF`;
- pattern 0 / track 7 / step 19 = 64.

All non-note fields and all other song bytes must equal the seeded baseline exactly. Track 0 proof rows must therefore remain unchanged.

### Screen and playback composition

Synchronize D026 presentation state at the final stopped STEP REC state and assert:

- active tracker screen;
- STEP REC indicator active;
- track 7 internally / `T8` in UI;
- page 1 internally / `PG2/4` in UI;
- visible window starts at global step 16;
- selected step is 20;
- REST/TIE/OFF/note row text matches the frozen D026 notation;
- no song byte changes during screen synchronization.

Then exit STEP REC and start the existing D021 playback object from step 0:

- initial event must still be the seeded proof-lane MIDI note 60 from track 0;
- D020/D021 frame ticking is exercised through a complete 352,800-frame 120 BPM loop;
- exactly 64 row boundaries occur and transport returns to step 0 without cumulative drift;
- the track-7 integration edits do not alter proof-lane event selection.

Finally stop playback, return pattern input to PERFORMANCE and prove tracker presentation deactivates without mutating state.

## Layer B full-app panel timeline

The full-app script mirrors the important user-facing path using the real `dinghy.c` main loop:

1. enter SEQ;
2. visit every physical white-key step on pages 1..4 with OCT+ page changes;
3. finish on step 64;
4. select track 8 through black-key index 17;
5. press PLAY to stop the auto-started proof transport;
6. wait long enough for the audio-side stopped acknowledgement;
7. enter STEP REC;
8. enter a note at step 64 and therefore wrap to step 1;
9. enter another note at step 1;
10. exit STEP REC;
11. select step 16;
12. re-enter STEP REC;
13. enter ordinary note at step 16 -> automatic page-boundary advance to step 17;
14. enter REST, TIE, OFF and an ordinary note on steps 17..20;
15. press PLAY while STEP REC is active, proving exit-and-resume behavior;
16. retain cursor/track state while playback continues;
17. capture the tracker framebuffer at 1000 ms;
18. press HOME after that capture;
19. capture HOME at 3000 ms.

A separate HOME-equivalent control run auditions the same final manual note state but never enters tracker context. The integration run's 3000 ms HOME framebuffer must be byte-identical to that control framebuffer. This proves no tracker pixels leak into HOME while preserving legitimate HOME state such as `last_note`.

## Frozen verification gates

WP2.10 may close only after all of the following pass:

- new direct Phase 2 integration test;
- new full-app integration host scenario and deterministic framebuffer hashes;
- HOME-equivalent pixel comparison;
- every WP2.1–WP2.9 unit/focused host suite;
- frozen default WAV SHA-256 `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`;
- exact D020/D021 timing regression, including frame 352800 loop;
- pinned JieLi toolchain and AC79 SDK input checks;
- full device build;
- `FM-1_987` package identity;
- HAL-only direct register access;
- resource measurement and total Phase 2 delta versus the Phase 1 baseline;
- durable `docs/PHASE2_VERIFICATION.md`;
- issue #18 close;
- parent issue #8 close only after the complete integration gate is green.

## Resource expectation

Because WP2.10 is expected to add tests/evidence only, the preferred result is **zero production resource delta versus WP2.9**:

- image: 59,252 B;
- normal RAM: 12,464 B;
- pool RAM: 96,616 B.

Any non-zero production delta is treated as evidence of a source change and must be explained by a demonstrated defect fix before Phase 2 can close.

## Strict non-goals

- no p-lock allocator or editor;
- no stable Phase 3 parameter/effect registry;
- no new audio engine;
- no multitrack playback expansion;
- no live-performance recording;
- no song/order/persistence;
- no final physical control-map freeze;
- no HAL/loader/update redesign;
- no Phase 3 implementation.