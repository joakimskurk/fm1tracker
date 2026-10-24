# FM1Tracker Decision Log

This file records decisions that constrain future work. It is intentionally short and append-only in spirit: if a decision changes, add a superseding entry rather than silently rewriting history.

Format:
- ID
- date
- status: accepted / provisional / superseded
- decision
- reason
- consequence

---

## D001 — Dinghy as platform baseline

Date: 2026-10-23  
Status: accepted

**Decision:** Build FM1Tracker on the open Dinghy/Felucca/SLOOP FM-1 platform baseline rather than the closed M-VAVE sound engine.

**Reason:** Gives us source-level control of audio, panel, screen, MIDI, USB audio, updater/rescue, host mode and safety checks under a clear GPL-3.0-only path.

**Consequence:** Preserve upstream GPL/provenance and keep platform modifications minimal until the baseline is proven.

## D002 — exact Dinghy pin

Date: 2026-10-23  
Status: accepted

**Decision:** Base the initial import on Dinghy commit `d44df7db09b98ccad2f1233b8f83f5f09c04829e`.

**Reason:** Reproducible provenance and stable review target.

**Consequence:** Upstream changes are not pulled implicitly. Any later rebase/update requires an explicit decision and compatibility review.

## D003 — no full Furnace port

Date: 2026-10-23  
Status: accepted

**Decision:** Furnace is a tracker/workflow/interoperability reference, not the firmware architecture to be transplanted.

**Reason:** Furnace's desktop-scale engine and third-party core matrix do not match the FM-1 resource/runtime constraints.

**Consequence:** No Furnace source/core enters the repository without a separate license/resource audit and explicit work package.

## D004 — one tracker, two sound modes

Date: 2026-10-23  
Status: accepted for V0.1

**Decision:** 8-BIT and 16-BIT share the same tracker/sequencer. V0.1 uses a project-wide sound mode rather than arbitrary mixed modes per track.

**Reason:** Keeps data model, UI and DSP scheduling bounded while both engine families are established.

**Consequence:** Mixed 8-BIT/16-BIT tracks are Phase 9 unless profiling and UX justify promotion.

## D005 — V0.1 tracker dimensions

Date: 2026-10-23  
Status: accepted

**Decision:** V0.1 targets 8 logical tracks, 64 steps per pattern, 16 patterns and max four sparse parameter locks per step.

**Reason:** Hardware-friendly scope with predictable memory bounds.

**Consequence:** Any increase requires memory-budget review before code changes.

## D006 — STEP REC is primary workflow

Date: 2026-10-23  
Status: accepted

**Decision:** Keyboard STEP REC with deterministic cursor advance is a first-class interaction, not an optional recording mode.

**Reason:** It is the main advantage of placing a tracker on the FM-1 keyboard hardware.

**Consequence:** Phase 2 is not complete without computer-free 64-step entry.

## D007 — dedicated REST action

Date: 2026-10-23  
Status: accepted concept; mapping provisional

**Decision:** STEP REC must have a one-action REST that inserts an empty step and advances exactly once.

**Current provisional mapping:** `FX` button while STEP REC is active.

**Consequence:** Gesture is tested on physical hardware in Phase 7 before V1 freeze.

## D008 — 16 white keys as step page

Date: 2026-10-23  
Status: provisional

**Decision:** In step editing, 16 white keys address 16 steps; four pages cover the 64-step pattern.

**Reason:** Direct physical correspondence is faster and clearer than cursor-only navigation.

**Consequence:** Exact page-navigation gesture remains subject to hardware UX test.

## D009 — black-key track selection

Date: 2026-10-23  
Status: provisional

**Decision:** Prototype the first eight suitable black keys as direct track selectors.

**Reason:** Makes eight-track navigation hardware-direct without adding a menu layer.

**Consequence:** Do not hard-code this as permanent visual language until Phase 7 hardware testing.

## D010 — parameter locks are first-class data

Date: 2026-10-23  
Status: accepted

**Decision:** P-locks live in the sequencer data model with bounded sparse storage, not as ad-hoc automation added later.

**Reason:** Per-step parameter control is core to the intended instrument.

**Consequence:** Stable parameter IDs and deterministic lock replacement/deletion are required before engine parameter maps expand.

## D011 — fixed-point/integer DSP from day one

Date: 2026-10-23  
Status: accepted

**Decision:** FM-1 real-time DSP uses integer/fixed-point techniques and avoids runtime 64-bit division.

**Reason:** AC791N/Dinghy toolchain and real-time constraints.

**Consequence:** Tables/constants should be generated offline where useful; each engine must be benchmarked rather than assuming desktop math is portable.

## D012 — initial engine scope

Date: 2026-10-23  
Status: accepted for V0.1

**Decision:** Initial 8-BIT family: PULSE, NOISE, TRIANGLE, WAVETABLE-8. Initial 16-BIT family: compact fixed-point 4-operator FM.

**Reason:** Broad enough to establish two distinct palettes while keeping implementation measurable.

**Consequence:** PCM/sample playback and named chip emulations remain out of the critical path.

## D013 — PCM deferred

Date: 2026-10-23  
Status: accepted

**Decision:** Do not build sample storage/playback before tracker core, p-locks and initial synth engines are stable.

**Reason:** Samples introduce storage, transfer, allocation and persistence complexity that can obscure core tracker problems.

**Consequence:** PCM lives in Phase 9 unless later promoted by explicit decision.

## D014 — baseline before features

Date: 2026-10-23  
Status: accepted

**Decision:** No tracker or synth feature work begins until the imported Dinghy baseline has been verified as far as the available build environment permits, then renamed and re-verified.

**Reason:** Prevents platform/import failures from being confused with FM1Tracker failures.

**Consequence:** Phase 1 is deliberately boring and must stay that way.

## D015 — repository carries project memory

Date: 2026-10-23  
Status: accepted

**Decision:** Current status, work-package decomposition, architectural decisions, provenance and test expectations live in repository files, not only in chat history.

**Reason:** The project must survive new chats, long pauses and handoffs without reconstructing intent from memory.

**Consequence:** `PROJECT_STATE.md` is updated as execution changes; architectural deviations are recorded here.

## D016 — fixed FM1Tracker package identity

Date: 2026-10-23  
Status: accepted

**Decision:** FM1Tracker uses the fixed package/device identity `FM-1_987` across releases. Release version remains in `APP_VER` and artifact filenames rather than changing the package identity.

**Reason:** The Firmware Hub live identity table was checked on 2026-10-23 and did not list `FM-1_987`. Keeping the identity in the `FM-1_9NN` form also preserves the existing Felucca/Dinghy interrupted-update/resume assumptions, whereas a longer project/version identity would change that behavior in the current updater tooling.

**Consequence:** `tools/build.py` must not derive the product identity from the release version. Future releases keep `FM-1_987` unless a documented collision forces a superseding decision. Before any public release, re-check the live Firmware Hub identity table.

## D017 — WP1.4 closes on identity/host evidence; full device build remains WP1.5

Date: 2026-10-23  
Status: accepted

**Decision:** WP1.4 is considered complete once the identity-only code path, visible host identity and unchanged host audio are verified. Actual renamed device firmware/package generation and resource capture remain the responsibility of WP1.5 and are not pulled into WP1.4 merely to work around the missing external AC79 SDK checkout.

**Reason:** WP1.4's purpose is to isolate identity changes from musical/platform changes. WP1.5 already exists specifically to verify the renamed baseline and capture firmware budgets. The separately required AC79 SDK checkout is still unavailable, while WP1.4 CI can prove that the protected baseline stayed unchanged, the FM1Tracker identity wrapper is configured correctly, the host output is renamed/visible, and the audio render is bit-identical to WP1.3.

**Consequence:** `docs/WP1_4_VERIFICATION.md` records exactly what was and was not verified. WP1.5 remains the only next work package, and Phase 2 tracker implementation may not begin until WP1.5 is closed or a later explicit decision changes that gate.

## D018 — WP2.1 freezes the core in-memory model, not storage allocation or persistence

Date: 2026-10-23  
Status: accepted

**Decision:** WP2.1 defines the compile-time V0.1 core types and dimensions only: the 8-byte `FmtStep`, `FmtPattern`, track configuration, order entry, instrument header, song metadata and a non-instantiated `FmtSong` aggregate used for compile-time sizing/bounds. It preserves a 16-bit `lock_head` with `0xFFFF` as the no-lock sentinel, but does not choose or implement the final p-lock allocator/list record. No global pattern/song backing store is allocated until WP2.2. Raw compiler structs are not a persistence format.

For budgeting only, WP2.1 reserves a conservative maximum of 8 bytes per future lock entry when checking the existing 96 KiB editable-song pool target. This is a budget reservation, not a lock-record ABI or allocator decision. Effect IDs beyond `NONE = 0` and stable parameter IDs remain unfrozen until their later dedicated work packages.

**Reason:** `DATA_MODEL_V0.1.md` explicitly requires an 8-byte step and 16-bit-capable lock indexing while also explicitly deferring the final lock container/allocator until profiling. Separating type definition from storage allocation keeps WP2.1 compile-time-only and leaves WP2.2/Phase 3 free to choose the measured implementation without rewriting the core step layout.

**Consequence:** WP2.1 tests exact core sizes, sentinels, defaults and bounds without consuming ~64 KiB of runtime pool RAM yet. WP2.2 owns actual song/pattern storage placement; Phase 3 owns lock allocation/edit semantics and stable parameter IDs; Phase 6 owns serialized project layout.

## D019 — the V0.1 runtime song store is one pool-backed `FmtSong`

Date: 2026-10-23  
Status: accepted

**Decision:** WP2.2 allocates exactly one runtime `FmtSong` object in the existing linker `.pool` section, 4-byte aligned. The store is owned by a narrow FM1Tracker store module and is initialized/reset explicitly before use. Runtime callers access it through bounds-safe song/pattern/step accessors rather than by inventing parallel storage or pointer arithmetic.

The explicit reset writes the already-frozen WP2.1 empty-step sentinels to all 8,192 steps. Zero-initialized pool memory is not treated as a valid empty tracker song because `FMT_NOTE_EMPTY`, instrument/volume inheritance and `FMT_LOCK_NONE` are nonzero sentinel values. Song metadata/track/order/instrument fields are reset deterministically; `default_pattern_length` is set to the already-approved 64-step V0.1 length. This decision does not create a serialized format.

**Reason:** `firmware/app.ld` provides a 344,064-byte `.pool` specifically for large zero-initialized buffers, while normal `.data + .bss` is limited to 96 KiB. The existing graphics canvas already demonstrates the intended section mechanism. A single 65,896-byte `FmtSong` raises measured pool use from the 30,720-byte Phase 1/WP2.1 baseline to an expected 96,616 bytes before later lock storage, while leaving normal RAM free for small runtime/control state.

**Consequence:** WP2.2 must prove by device symbol inspection that the store is physically inside `.pool`, must verify the actual pool delta, and must keep host audio behavior unchanged. The p-lock allocator remains Phase 3 work, transport remains WP2.3, and persistence remains Phase 6.

## D020 — V0.1 transport uses an exact BPM-per-frame rational phase accumulator

Date: 2026-10-23  
Status: accepted

**Decision:** WP2.3 represents V0.1 1/16-note timing with a 32-bit integer phase accumulator in the 44.1 kHz frame domain. One 1/16-note step has the exact rational duration `44100 * 15 / BPM` frames, so the implementation uses a fixed threshold of `661500 = 44100 * 15` and adds the integer BPM value once per rendered frame. When phase reaches or exceeds 661500, the threshold is subtracted and the current step advances exactly once, wrapping `63 -> 0`.

The transport runtime state is a small ordinary-RAM control object: 32-bit phase, 16-bit BPM, 8-bit current step and 8-bit running flag. Reset produces a stopped transport at step 0, phase 0 and runtime default tempo 120 BPM. Start resumes from the current step/phase; stop preserves them. A nonzero tempo change preserves phase progress. BPM 0 is rejected rather than used as a hidden stop command.

**Reason:** This formulation contains no floating point, no division in the real-time path and no runtime 64-bit arithmetic. Because the phase is always below 661500 and a 16-bit BPM is at most 65535, one frame can cross at most one step boundary and `phase + bpm` cannot overflow 32 bits. It also preserves the exact fractional remainder automatically, so tempos such as 137 BPM cannot accumulate drift from repeatedly rounded frame intervals. A per-frame tick naturally exposes the exact frame where a future WP2.4 note event belongs without adding event dispatch in WP2.3.

**Consequence:** WP2.3 host tests must prove exact accumulator/reference equivalence over long runs, deterministic start/stop/reset and exact 64-step wrap. The device integration may advance the transport from the audio callback, but it remains stopped in WP2.3 so no audible behavior changes. Audible row dispatch begins only in WP2.4. This runtime default does not yet redefine persisted `FmtSongMeta` defaults or create a file-format tempo rule.

## D021 — WP2.4 dispatches step 0 at frame 0 and applies boundaries to the next rendered frame

Date: 2026-10-23  
Status: accepted for the one-track playback proof

**Decision:** WP2.4 starts playback from a reset transport by dispatching pattern 0 / track 0 / step 0 exactly once before the first audio frame is rendered, then starts the D020 transport. During audio rendering, the current tracker voice is rendered first and `fmt_transport_tick_frame()` is applied at the end of that frame. If the tick crosses a step boundary, the newly selected row is decoded immediately and becomes active from the next rendered frame.

For the WP2.4 proof, only ordinary note values `0..127` create note events. `FMT_NOTE_EMPTY` creates no event and therefore sustains the current proof voice. `TIE`, `OFF` and malformed/special note values are treated as no event in this package; their full playback/editor semantics remain WP2.8. The proof is fixed to pattern 0, logical track 0 and one dedicated monophonic instance of the existing baseline sine tone. It does not create a general event queue, track-selection system or new synth engine.

**Reason:** Dispatching step 0 before frame 0 gives an unambiguous musical start. Ticking after each rendered frame makes the D020 accumulator represent elapsed frames directly: at 120 BPM, step 1 begins on frame 5513 and the first complete 64-step loop returns to step 0 on frame 352800. Processing the single proof lane per frame also preserves exact sub-block boundaries inside the normal 256-frame callback instead of quantizing row changes to callback edges.

**Consequence:** WP2.4 tests must prove the initial frame-0 event, exact representative boundary offsets, exact `63 -> 0` loop timing and deterministic note-event traces from the real pool-backed store. The dedicated proof voice may add only small bounded control state and reuses the existing integer sine tone. General note-off/tie semantics, editing, multitrack dispatch and any bounded sequencer-to-audio queue remain later work packages.

## D022 — WP2.5 uses clamped four-page cursor addressing in explicit SEQ context

Date: 2026-10-23  
Status: accepted for the WP2.5 input layer; hardware ergonomics remain provisional

**Decision:** WP2.5 introduces a small pattern-edit cursor state with four pages (`0..3`), sixteen local step positions (`0..15`) and a derived global step `page * 16 + local`, covering exactly `0..63`. The physical white-key indexes are fixed to `0,2,4,6,7,9,11,12,14,16,18,19,21,23,24,26`, mapping in order to local steps `0..15`. Other note-key indexes are not step selectors in WP2.5.

`SEQ` enters the pattern-edit input context and `HOME` returns to normal performance context. In performance context all 27 note keys retain the existing audition/MIDI behavior. In pattern-edit context, a white-key press updates cursor state only and is consumed as a step selection; black-key presses are ignored until WP2.6 rather than auditioning or selecting tracks. Releases of notes that were started in performance context are still honored after a context change so entering SEQ cannot create stuck manual voices.

`OCT-` / `OCT+` navigate pattern pages only while in pattern-edit context. Page navigation is clamped at page 0 and page 3 rather than wrapping. Changing page preserves the current local step index, so the selected global step moves by exactly 16 where a page move succeeds. Reset starts in performance context with page 0, local step 0 and selected global step 0. No WP2.5 action mutates pattern data.

**Reason:** The explicit SEQ/HOME context preserves the baseline keyboard instrument outside pattern editing and makes the white-key reinterpretation intentional. Clamped page navigation prevents a repeated previous/next gesture from unexpectedly jumping between opposite ends of the 64-step pattern, while preserving the local physical key position gives a simple and learnable relation between pages. The exact hardware ergonomics can still be superseded after Phase 7 physical testing without changing pattern data or timing semantics.

**Consequence:** WP2.5 must host-test all 64 global mappings, all 27 physical note-key indexes, clamped page boundaries, local-index preservation, context transitions and proof that step selection does not mutate the song store. It may add only small bounded ordinary-RAM cursor state and main-loop input handling. Black-key track selection remains WP2.6, note writes remain WP2.7 and no tracker screen is added here.

## D023 — WP2.6 maps the first eight physical black keys to tracks 1–8 in SEQ context

Date: 2026-10-23  
Status: accepted for the WP2.6 prototype; hardware ergonomics remain provisional

**Decision:** WP2.6 extends `FmtPatternInput` with one selected-track byte `0..7`. Reset selects logical track 0 (track 1 in the UI). The first eight physical black note-key indexes in ascending keyboard order are `1,3,5,8,10,13,15,17`; in the FM-1 F3..G5 span these are F#3, G#3, A#3, C#4, D#4, F#4, G#4 and A#4. They map bijectively to logical tracks `0..7`. The remaining black-key indexes `20,22,25` are reserved and rejected in WP2.6.

Track selection is active only in the explicit D022 SEQ/pattern-edit context. In PERFORMANCE context all note keys, including all black keys, retain normal audition/MIDI behavior. Entering HOME or SEQ does not reset the selected track; the current track survives context changes. Selecting a track does not alter page or local/global step state, does not mutate `FmtSong`, and does not alter the WP2.4 proof playback lane, which remains fixed to pattern 0 / logical track 0.

**Reason:** Using the first eight black keys preserves the direct left-to-right hardware relationship already proposed by D009 while leaving the final three black keys available for later fast helpers. Keeping track selection purely in the editor cursor state prevents this prototype from accidentally becoming multitrack audio or STEP REC work. Preserving the track across HOME/SEQ avoids unnecessary navigation loss while still giving reset a deterministic track-1 default.

**Consequence:** WP2.6 must test all eight mappings, all three reserved black keys, invalid indexes, PERFORMANCE-context rejection by the selector API, D022 white-key/page regression, context preservation, no page/step movement on track changes, byte-identical song-store state and unchanged frozen WP2.4 audio. Physical ergonomics remain unverified until Phase 7 and may supersede this provisional hardware mapping without changing tracker data.

## D024 — WP2.7 STEP REC is a stopped-transport, one-note-per-gesture editor mode

Date: 2026-10-23  
Status: accepted for the WP2.7 note-entry layer; chord ergonomics remain provisional

**Decision:** STEP REC is a submode of the D022 pattern/SEQ context. A REC press enters STEP REC only while the D020 transport is stopped; a REC press while transport is running is ignored. Successful entry forces pattern/SEQ context but preserves the current D022 page/local step and D023 selected track. Pressing REC again exits STEP REC and remains in pattern context. HOME exits STEP REC and returns to PERFORMANCE. PLAY while STEP REC is active exits STEP REC before resuming playback, so WP2.7 cannot become implicit live recording. Outside STEP REC, PLAY toggles the existing proof transport stop/resume state without changing D020 timing semantics.

While STEP REC is active, all 27 physical note keys are interpreted as pitch input rather than D022 white-key step selectors or D023 black-key track selectors. `OCT-` / `OCT+` control the keyboard octave in STEP REC rather than changing the 16-step page. The stored MIDI note is exactly the same note used for audition: physical key 0 is F3 / MIDI 53 and the value is `53 + 12 * octave + key_index`, with the inherited octave range `-2..+2`, so the WP2.7 physical-note range is 29..103 and always fits the ordinary `0..127` note domain.

WP2.7 is monophonic for writing. At the beginning of an unlatched note gesture, if one or more physical note-key downs are observed in the same poll, the lowest physical key / lowest pitch is the single note written. The write gesture then latches until all physical note keys have been released. Additional key presses while latched may audition normally but cannot write or advance another tracker step. This prevents scan/order-dependent chord behavior without freezing final chord UX before Phase 7.

A successful write targets pattern 0, the current D023 selected track and the current D022 global step. It changes only `FmtStep.note`; instrument, volume, effect, effect value, flags and lock-head remain byte-for-byte unchanged. The cursor then advances exactly one global step modulo 64 while preserving selected track: step 63 wraps to step 0 / page 0 / local step 0. Invalid notes, running transport, inactive STEP REC, invalid cursor/track state or a latched gesture produce no write and no cursor movement. REST, TIE and NOTE OFF editing remain WP2.8.

**Reason:** The existing WP2.4 proof lane auto-starts the D020 transport, so WP2.7 needs an explicit stop/resume gesture before the documented stopped-transport STEP REC workflow can be real. Reusing PLAY and the existing D020 stop/resume semantics keeps that addition minimal. A one-note latch gives deterministic behavior for both simultaneous and staggered chord-like presses, while auditioning remains musical. Writing only the note byte preserves the frozen V0.1 step defaults and avoids inventing instrument/volume semantics before their dedicated editor work.

**Consequence:** WP2.7 must add a small bounded editor state, exact cursor-advance support, host tests for REC/PLAY/context gating, lowest-note chord selection, gesture latching, exact target isolation, field preservation and `63 -> 0` wrap, plus full prior regression/device/resource verification. The default WP2.4 proof playback and host WAV must remain unchanged when no WP2.7 controls are used. Physical chord-entry feel remains provisional until Phase 7.

## D025 — WP2.8 gives REST, TIE and NOTE OFF distinct editor and playback semantics

Date: 2026-10-23  
Status: accepted for the WP2.8 prototype; physical gesture ergonomics remain provisional

**Decision:** WP2.8 keeps the frozen special note bytes from D018 and assigns them explicit STEP REC and one-lane playback meaning without changing the 8-byte `FmtStep` layout.

Editor gestures while D024 STEP REC is active and transport is stopped are:

- **REST / EMPTY:** press and release `FX` without using its modifier action. The implementation defers the REST write until `FX` release so a NOTE OFF combination cannot accidentally write REST first.
- **TIE:** press `SELECT` while `FX` is not pending/held.
- **NOTE OFF:** hold `FX`, then press `SELECT`. This confirms the existing provisional `FX + SELECT` direction with an explicit modifier-first order. The NOTE OFF write happens on the `SELECT` press and cancels the pending REST; releasing `FX` afterward must not write another event.

All three successful special-event edits target pattern 0, the currently selected logical track and current global step. They change **only** `FmtStep.note`, respectively to `FMT_NOTE_EMPTY`, `FMT_NOTE_TIE` or `FMT_NOTE_OFF`; instrument, volume, effect, effect value, flags and lock-head are preserved. Every successful special-event write advances exactly one global step modulo 64 with the same `63 -> 0` rule as D024. They require active STEP REC, a stopped transport, valid cursor/track state and an unlatched D024 write gesture. A successful special event participates in the D024 gesture latch so a simultaneous note-key down cannot also write a second row; the existing all-note-keys-released rule clears the latch. Leaving STEP REC through REC, HOME or PLAY cancels any pending `FX` REST without writing it.

Playback keeps the existing 4-byte `FmtPlaybackEvent` size but replaces the WP2.4 boolean note flag with an explicit bounded event kind: `NONE`, `NOTE`, `TIE` or `OFF`. At each D021 row boundary:

- ordinary note `0..127` emits `NOTE` with that pitch;
- `FMT_NOTE_EMPTY` emits `NONE`: no pitch or gate command is sent, so the current engine/voice state continues naturally;
- `FMT_NOTE_TIE` emits `TIE`: do not retrigger or change pitch; if a note is currently gated, keep that gate active through the row. A TIE with no active note does not resurrect a stale pitch;
- `FMT_NOTE_OFF` emits `OFF`: release the currently gated proof voice without interpreting `0xFE` as pitch;
- malformed/reserved non-note values emit `NONE` safely.

For the current proof sine, `NOTE` sets pitch and the existing target level, `TIE` preserves the active gate/pitch without retrigger, `OFF` moves the gate target to zero and uses a bounded click-safe release, and `EMPTY` leaves the voice untouched. The default WP2.4 proof pattern contains no TIE/OFF rows, so its frozen WAV hash must remain byte-identical. D021 frame ordering is unchanged: a special event decoded at a crossed boundary applies beginning with the next rendered frame.

**Reason:** REST, TIE and NOTE OFF need different musical intent even when a minimal sustained proof oscillator can make EMPTY and TIE sound similar in some cases. EMPTY means no row command, TIE explicitly means continue an already gated note without retrigger, and OFF explicitly releases it. Keeping edits note-byte-only preserves future effects/p-locks on otherwise note-empty rows. Delaying REST until `FX` release makes the provisional `FX + SELECT` NOTE OFF gesture deterministic without timing races, while direct `SELECT` gives TIE a fast one-button action in STEP REC.

**Consequence:** WP2.8 must host-test all three edit gestures, field/neighbor preservation, modifier ordering, cancellation on mode exit, latch interaction, exact one-step advance including `63 -> 0`, and rejection outside stopped STEP REC. Playback tests must prove exact `NONE/NOTE/TIE/OFF` decoding at D020/D021 boundaries, safe malformed values and no raw sentinel pitch. The full-app proof voice must audibly release on OFF while the default frozen WP2.4 WAV remains unchanged. No tracker screen, general multitrack playback, p-lock implementation or engine work enters WP2.8.

## D026 — WP2.9 uses a cursor-owned eight-row one-track tracker screen with bounded dirty redraw

Date: 2026-10-23  
Status: accepted for the WP2.9 prototype; physical readability remains provisional

**Decision:** WP2.9 adds a presentation-only tracker screen for D022 pattern context. It shows one selected logical track and exactly eight rows at a time. The edit cursor owns the viewport: each 16-step page is split into fixed first/second eight-row halves, while playback never scrolls the edit surface. The header explicitly exposes mode, `CURnn`, `PLYnn` or `STOP`, BPM, pattern `P01`, selected track, page and visible range. Row markers remain independent: `>` is the selected edit row and `*` is the running playhead; same-row state shows both.

Row notation is frozen for this package: ordinary notes use tracker pitch text, EMPTY=`---`, TIE=`===`, OFF=`OFF`, malformed=`???`; instrument inherit=`..`, explicit instruments are UI `01..16`; volume inherit=`...`, explicit volume is decimal `000..254`; effect NONE=`---` and any non-zero effect remains `???` until Phase 3 assigns stable effect semantics. HOME/PERFORMANCE keeps the existing performance screen.

The 240×240 tracker UI is tiled into ten bounded regions: one `240×64` header, eight `240×18` row strips and one `240×32` footer. A small ordinary-RAM presentation cache owns a persistent 10-bit dirty mask. At most **two tracker regions may be rendered/blitted per main-loop iteration**. Entering/changing view may dirty all regions, but pending bits drain under that fixed limit. State inspection touches only the eight visible rows. No drawing occurs in the audio ISR and display code may not mutate song/editor/transport state. Detailed formatting/layout/verification rules live in `docs/WP2_9_SCREEN_CONTRACT.md`.

**Reason:** A stable cursor-owned viewport preserves the direct physical relationship between the 16 step keys and the edit cursor while playback runs; following the playhead would make the editing surface move underneath the user. Eight rows leave enough 240×240 space for explicit context and readable tracker fields. Independent cursor/playhead markers avoid conflating editing with transport. The two-region limit turns redraw cost into a measurable bounded main-loop operation rather than a hidden full-screen refresh.

**Consequence:** WP2.9 must unit-test all eight possible row windows, note/placeholder formatting, marker independence, read-only song access and dirty-mask drain behavior. Representative host screenshots must be deterministic and visually inspected. HOME behavior and all WP2.1–WP2.8 regressions, including the frozen default WAV, must remain unchanged. Device build/resource evidence must prove the presentation state stays <=64 B ordinary RAM, adds no pool allocation and does not alter HAL/loader/audio architecture. WP2.10 and Phase 3 remain out of scope.

## D027 — WP3.1 freezes an 8-bit stable parameter-ID namespace before p-lock storage

Date: 2026-10-23  
Status: accepted

**Decision:** `FmtParamId` is exactly one unsigned byte. Parameter identity is semantic and stable: it is never derived from an encoder number, UI page position, table index or compiler enum ordinal. `0x00` is `FMT_PARAM_NONE`, meaning no parameter and never a valid active p-lock target. `0xFF` is `FMT_PARAM_INVALID`, used for invalid/corrupt lookup results and never assignable.

The V0.1 identity space is partitioned as follows:

- `0x01..0x1F` — common voice parameters shared across compatible engines;
- `0x20..0x5F` — 8-BIT engine-specific parameters;
- `0x60..0x9F` — 16-BIT engine-specific parameters;
- `0xA0..0xBF` — track/mixer parameters;
- `0xC0..0xDF` — channel/send/effect parameters;
- `0xE0..0xEF` — future GLOBAL/master parameter namespace, reserved and unassigned in WP3.1;
- `0xF0..0xFE` — future/reserved and unassigned;
- `0xFF` — INVALID.

A namespace range does **not** make every byte in that range a valid parameter. Only IDs present in the authoritative registry are assigned/lockable. WP3.1 assigns only the minimum useful cross-phase set:

- `0x01` — `FMT_PARAM_VOICE_LEVEL`;
- `0x02` — `FMT_PARAM_PITCH_FINE`;
- `0xA0` — `FMT_PARAM_TRACK_LEVEL`;
- `0xA1` — `FMT_PARAM_TRACK_PAN`.

Envelope parameters, oscillator/wavetable/noise controls, FM operator controls, send/effect controls and all GLOBAL/master parameters remain unassigned until their owning work packages establish real semantics. The `FmtStep.effect` tracker-command byte remains a separate identity domain; tracker effect command IDs are not allocated from `FmtParamId` merely because the effect may later expose lockable parameters.

Registry labels/metadata are presentation/debug aids only. They are not persistence keys and may be refined without renumbering a semantic ID. Once an ID is used by a released project format or public interchange contract, it must never be recycled for unrelated meaning; any future incompatible change requires a new ID and migration rule. Reserving `0xE0..0xEF` for GLOBAL/master identity does not imply that per-step track p-lock storage may own global automation; global storage/ownership remains a separate future decision.

**Reason:** One byte preserves the compact lock direction already anticipated by `DATA_MODEL_V0.1.md`, leaves large bounded namespaces for both engine families, and keeps lookup/event payloads cheap on the AC791N. Explicit NONE/INVALID values make zeroed/untrusted data safer. A separate GLOBAL namespace preserves the accepted live-performance vision without prematurely forcing master automation into track-owned locks.

**Consequence:** WP3.1 implements one authoritative registry/API and exhaustively tests all 256 possible byte values for classification, assignment, uniqueness and safe lookup. WP3.2 may use `FmtParamId` when choosing the sparse lock record/allocator, but WP3.1 allocates no lock pool and changes no song layout. Later engine/effect packages may add IDs only inside their documented ranges and must update the registry/tests/decision history rather than scattering numeric literals.

## D028 — WP3.2 uses one 2,048-record global sparse lock pool with owner-checked 8-byte records

Date: 2026-10-24  
Status: accepted

**Decision:** WP3.2 freezes the first concrete V0.1 p-lock storage representation. The existing `FMT_LOCK_POOL_MAX = 2048` is a **global sparse capacity shared by the complete song**, not a promise that every one of the 8,192 resident steps can simultaneously carry four locks. D005 still guarantees that any individual step may own at most four active locks. If the global 2,048-record pool is exhausted, adding a new lock fails explicitly and leaves all existing data unchanged.

The in-memory lock record is exactly eight bytes and is not a persistence/file-format ABI:

```c
typedef struct {
    uint16_t next;      /* active-list or free-list index; 0xFFFF = end */
    uint16_t value;     /* full 0..65535 storage domain; semantics owned by parameter */
    uint16_t owner;     /* linear song-step owner 0..8191; 0xFFFF = none */
    FmtParamId param_id;
    uint8_t state;      /* FREE or ACTIVE */
} FmtLock;
```

Offsets are fixed for the WP3.2 in-memory contract: `next=0`, `value=2`, `owner=4`, `param_id=6`, `state=7`. Two thousand forty-eight records therefore consume exactly 16,384 bytes of `.pool` RAM, matching the conservative eight-bytes-per-entry reservation made in D018. The allocator owns only a 16-bit free-list head and 16-bit used-count in ordinary RAM. `FMT_LOCK_NONE = 0xFFFF` remains the empty `FmtStep.lock_head` and list terminator. `FMT_LOCK_OWNER_NONE = 0xFFFF` means a record has no step owner.

A step owner is the deterministic linear index:

`pattern * (FMT_TRACKS * FMT_STEPS_PER_PATTERN) + track * FMT_STEPS_PER_PATTERN + step`

covering exactly `0..8191`. Active records must carry that exact owner. This lets the storage layer reject an in-range but cross-owned `lock_head`/`next` instead of accidentally reading, replacing or freeing another step's records.

Reset is deterministic startup/editor work, never audio-ISR work. It initializes record indexes `0..2047` as one ascending free list, ending in `FMT_LOCK_NONE`, clears every record's value/owner/parameter/state to the documented free form, sets free-head to record 0 and used-count to zero. The normal complete-song reset must reset both `FmtSong` step heads and this lock allocator so stale records cannot survive a new-song/reset operation.

Each active step list is a singly linked list sorted in **strict ascending `FmtParamId` order**. Storage order therefore does not depend on the user's edit history. Duplicate parameter IDs are never stored on one step. Setting an already-present parameter replaces only its 16-bit value in place and consumes no new record. Adding a new parameter allocates one free record and inserts it in sorted position. A fifth unique lock is rejected as STEP_FULL before allocation. Removing one parameter unlinks exactly one record and returns it to the free list. Clearing a step returns every record in the validated list exactly once and resets that step's `lock_head` to `FMT_LOCK_NONE`.

Only D027 IDs for which the authoritative registry reports `lockable` may be stored. NONE, INVALID, reserved and currently unassigned namespace members are rejected without allocation. The 16-bit value is deliberately opaque to WP3.2: engine/parameter-specific ranges, scaling and audio application remain later packages.

All public lock operations address a song step through bounded pattern/track/step coordinates rather than accepting arbitrary raw step pointers. Before any mutation, the existing list is fully validated with a hard traversal bound of `FMT_LOCKS_PER_STEP`. Validation rejects and reports corruption for any out-of-range record index, non-ACTIVE active-list record, wrong owner, non-lockable stored parameter, duplicate/non-ascending parameter ID, self/cyclic/overlong chain or inconsistent allocator head/count state encountered by the operation. A corruption failure performs **no partial mutation**. Free-list allocation likewise validates the record being popped before changing allocator state. POOL_FULL, STEP_FULL, INVALID_PARAM, BAD_ADDRESS, NOT_FOUND and CORRUPT remain distinct storage outcomes; user-facing wording/gestures belong to WP3.3/WP3.4.

**Reason:** A fixed global sparse pool preserves the V0.1 96 KiB editable-song target while making per-step p-locks first-class. Four locks on every resident step across all 16 patterns would require 32,768 records and cannot fit that target; the earlier data-model example that described 2,048 records as the whole-song worst case omitted the 16 pattern slots and is corrected by this decision. Eight-byte records use the budget already reserved by D018 and buy a 16-bit owner tag that materially improves corruption/isolation safety. Sorted active lists give deterministic lookup/iteration independent of edit order, while a shared `next` field keeps both active lists and the free list compact.

**Consequence:** WP3.2 allocates exactly 16,384 bytes of new pool storage plus four bytes of allocator runtime state before measured code/linker overhead. With the WP3.1 baseline this predicts 113,000 bytes total pool use and 12,468 bytes normal RAM before linker alignment effects, leaving 231,064 bytes of pool. `FmtSong` remains 65,896 bytes and `FmtStep` remains exactly eight bytes. WP3.2 must test all 8,192 owner mappings, exhaustive reset/free-list uniqueness, deterministic set/get/replace/remove/clear, global exhaustion/recovery, cross-owner and malformed-index rejection, no-neighbor mutation and the max-four invariant. Because D027 currently exposes exactly four production-lockable IDs, the fifth-lock rejection branch must additionally be exercised by a focused storage test using a test-only lockability stub; this does not assign a fifth production parameter ID. WP3.3 owns encoder/hold-step editing, WP3.4 owns user-facing replace/delete UX, and WP3.5+ owns application of locks to playback/audio.

## D029 — WP3.3 uses a white-step hold latch and relative KNOB1..4 deltas for storage-only p-lock editing

Date: 2026-10-24  
Status: accepted for the WP3.3 prototype; physical ergonomics remain provisional

**Decision:** WP3.3 adds the first hardware/editor path that writes D028 locks. P-lock editing exists only in ordinary D022 SEQ/pattern context. It is unavailable in PERFORMANCE and D024 STEP REC. Transport running/stopped state does not gate WP3.3 editing because this package writes only the lock store and the current audio/playback path does not read or apply locks.

A lock gesture begins on a new physical **white step-key** down while ordinary SEQ is active and no lock gesture is already active. If multiple white step-key downs arrive in the same main-loop poll, the lowest physical white key is the single gesture owner. The key is first selected through the existing D022 mapping, then the editor snapshots **pattern 0, the selected logical track and the resulting global step**. If a black track key and white step key arrive in the same poll, the white-key gesture uses the track selection that existed at the beginning of that poll; the simultaneous black-key selection is suppressed. This prevents scan/order-dependent targeting.

The gesture remains active only while its owning physical white key remains held. During that hold, ordinary step selection, black-key track selection and OCT page navigation are suppressed, so the cursor/header and the snapshotted target cannot diverge. Pressing other note keys cannot retarget the gesture. Releasing the owning white key ends the gesture immediately. HOME/performance exit or entering STEP REC cancels it immediately without writing an extra value. PLAY does not cancel it; playback may start/stop while the held storage target remains fixed. A new gesture always requires a fresh white-key down after the previous gesture has ended; holding a second key across the first key's release never transfers ownership automatically.

The FM-1 HAL's `fm1_enc_take()` relative detent stream is the intent signal. WP3.3 consumes/drains KNOB1..4 on every main-loop poll, even when no lock gesture is active. Deltas observed while inactive are discarded. On the poll that begins, releases or cancels a gesture, all KNOB1..4 deltas are also discarded. Therefore encoder motion that occurred before the hold can never leak into the new target, and a release occurring in the same poll as encoder movement wins over the movement. Only deltas from later polls while the gesture remains stably active can write locks. Zero delta never creates or replaces a lock.

KNOB1..4 are mapped directly to the four D027 production-lockable IDs for this prototype:

- `KNOB1` -> `FMT_PARAM_VOICE_LEVEL` (`0x01`)
- `KNOB2` -> `FMT_PARAM_PITCH_FINE` (`0x02`)
- `KNOB3` -> `FMT_PARAM_TRACK_LEVEL` (`0xA0`)
- `KNOB4` -> `FMT_PARAM_TRACK_PAN` (`0xA1`)

This encoder mapping is UI/page policy, not parameter identity; D027 numeric IDs remain semantic and independent of encoder positions.

WP3.3 freezes a small **editor-normalized** value domain for only these four current IDs. Values are stored in D028's 16-bit field but valid WP3.3 editor values are `0..255`. One encoder detent changes the editor value by exactly one, with signed multi-detent deltas applied in one bounded operation and clamped to `0..255`. These numbers do not yet mean dB, cents, a pan law or engine coefficients; audio/DSP interpretation remains WP3.5+/engine work.

When a parameter has no lock on the target step, the first nonzero eligible delta starts from a fixed editor reference and then applies that delta:

- VOICE LEVEL reference = `255`
- FINE PITCH reference = `128`
- TRACK LEVEL reference = `255`
- TRACK PAN reference = `128`

The level references represent the editor's unattenuated end of its normalized range; FINE and PAN use the normalized center. They are not a claim about a future engine's internal default. If an existing lock for one of these IDs contains a value above `255`, WP3.3 treats that value as an editor-domain error and performs no mutation rather than silently clamping/reinterpreting it. A later decision may supersede the editor reference/scaling when real engine base parameters exist, but it may not silently reinterpret released persisted data.

For an existing valid lock, each eligible encoder delta computes from the stored value and calls D028 `fmt_lock_set()`, which replaces in place without allocation. For a missing lock, the reference-plus-delta result is written as the first lock. A nonzero delta still creates the missing lock when the result saturates at the reference boundary, so e.g. clockwise movement from a level reference of 255 records an explicit 255 rather than disappearing as no intent.

If multiple KNOB1..4 deltas are available in one poll, they are processed in fixed order KNOB1, KNOB2, KNOB3, KNOB4 as separate bounded atomic storage edits. An earlier successful edit remains valid if a later encoder in the same poll encounters POOL_FULL or another error; the ordering is deliberate and testable. D028 still guarantees that each individual `fmt_lock_set()` is corruption-safe and has no partial mutation.

WP3.3 owns no delete/clear gesture. D028 statuses are surfaced only as bounded editor feedback. The editor keeps the last attempted parameter, value and status for the active hold. `OK` shows the successfully stored normalized value. STEP_FULL, POOL_FULL, BAD_ADDRESS, CORRUPT and the WP3.3 editor-domain range error show explicit error feedback without retry loops or automatic deletion. INVALID_PARAM should be unreachable through the frozen KNOB mapping but is handled safely if encountered.

Minimal display feedback lives only in the existing D026 tracker footer. While a white step is held, the footer identifies p-lock hold context and the target step/track. After an encoder attempt during that hold, the footer shows the knob/parameter short name plus stored value or a concise error. Releasing/cancelling the hold restores the ordinary SEQ footer. Full per-row lock markers, browsing and detailed lock visualization remain WP3.9. Footer dirtiness remains bounded under the existing D026 max-two-regions-per-poll renderer; no drawing enters the audio ISR.

WP3.3 changes storage only. Encoder p-lock writes must not alter `FmtStep.note`, instrument, volume, tracker effect fields, playback voice state, audition state or base instrument/track parameters. No lock is read by or applied to audio in this package.

**Reason:** A held physical step plus relative detent movement is the direct hardware gesture already anticipated by the control map, while the existing HAL gives a reliable one-click relative intent signal so no guessed analog pickup threshold is required. Snapshotting and then freezing the target prevents track/page/cursor changes from redirecting a write. Continuously draining inactive encoders prevents stale motion from becoming an accidental lock. A normalized byte-scale editor domain is compact, readable and sufficient to prove the edit architecture without prematurely freezing DSP units.

**Consequence:** WP3.3 must introduce a small bounded editor state/module, extend the host harness with deterministic encoder-delta events, and test context gating, all sixteen white-key targets across pages, all eight tracks, target snapshot/focus locking, stale-delta discard, release-before-delta precedence, fixed four-encoder ordering, first-lock references, clamping, replacement-in-place, editor-range rejection and D028 storage-error propagation. The full app must demonstrate hold-step + encoder editing without MIDI audition or unintended cursor/track movement. All Phase 2, WP3.1 and WP3.2 regressions plus the frozen default WAV remain required. Physical comfort of holding a white key while turning encoders is still a Phase 7 hardware-UX question. WP3.4 owns delete/clear UX; WP3.5+ owns p-lock application to playback/audio.

## D030 — WP3.4 browses existing locks with SELECT and requires an armed EDIT -> SEL confirmation to delete one lock

Date: 2026-10-24  
Status: accepted for the WP3.4 prototype; physical ergonomics remain provisional

**Decision:** WP3.4 extends the existing D029 held-white-step editor with bounded browse/select/delete behavior only. It does not change D028 storage layout/capacity, D027 parameter identity, D029 target ownership or playback/audio behavior.

The WP3.4 destructive editor exists only while a valid D029 lock hold is already active in ordinary SEQ. The target remains D029's snapshotted **pattern 0 + logical track + global step** for the entire hold. PERFORMANCE, STEP REC, HOME/context exit or owner-key release cannot browse or delete and cancel all WP3.4 destructive state immediately.

### Existing-lock selection and browse

At the beginning of a held-step gesture, WP3.4 queries D028's fully validated count/ordinal APIs. If the target has one or more locks, the initial selected lock is ordinal 0 in D028's strict ascending `FmtParamId` order. If the target has zero locks, selection is NONE and browse/delete are safe no-ops.

The **SELECT encoder** (`ENC_SELECT`) is the browse control while the D029 hold is active. Its relative delta moves through the currently existing lock list in D028 ordinal order. Browse is **clamped**, not wrapped: movement below the first lock stays on the first lock and movement above the last stays on the last. Multi-detent deltas are applied in one bounded operation. At most four records can be visited, so refresh/search work is hard-bounded by `FMT_LOCKS_PER_STEP`.

`ENC_SELECT` is drained on every main-loop poll, including when no lock hold exists. Its delta is discarded while inactive and on the poll that begins, releases or cancels a D029 hold, exactly like D029's KNOB1..4 stale-delta rule. Therefore pre-hold SELECT movement cannot silently select a different lock on a new target.

A successful D029 KNOB1..4 set/replace makes that parameter the currently selected lock after the write. This includes a newly created lock. An unsuccessful D029 write does not invent a selection. Selection is always refreshed through validated D028 data before destructive mutation.

### One-lock delete gesture

WP3.4 exposes **one-lock deletion only**. There is deliberately no user-facing clear-all gesture in this package.

Deletion is a two-stage destructive action:

1. while a held-step target has a valid selected lock, press `EDIT` to **arm** deletion of that selected semantic parameter;
2. on a **later main-loop poll**, press `SEL` to confirm while that arm is still valid.

`EDIT` alone never removes anything. `EDIT` + `SEL` arriving together in the same poll also never removes anything: the confirm is accepted only if the delete arm already existed before that poll. After one successful delete, the arm is consumed, so another lock cannot be deleted until EDIT is released/pressed and a new arm is created.

Releasing `EDIT` before confirmation cancels the arm. Owner-key release, HOME/context exit, successful STEP REC entry, invalid/corrupt target data, or any storage failure also cancels it. PLAY remains allowed to toggle transport without changing the D029 target, but a PLAY press cancels a pending delete arm before transport handling so a latent destructive confirmation cannot survive an unrelated transport action. OCT/page intent and any additional note-key down remain suppressed by D029 while the hold is active; if a delete arm is pending, such unrelated focus/navigation intent cancels the arm even though it still cannot retarget the held step.

While a delete arm is pending, all SELECT and KNOB1..4 encoder deltas are still drained but **discarded**. They may not change selection, write a lock or alter the armed semantic parameter. This makes the confirmation target stable and prevents a same-hand encoder adjustment from changing what SEL would delete. BTN_SEL is otherwise unused in ordinary SEQ, so the confirmation does not conflict with D025's TIE/OFF behavior, which only exists in STEP REC.

On confirmation, the editor revalidates that the armed semantic parameter still exists on the snapshotted D029 target, then calls D028 `fmt_lock_remove()` once. `NOT_FOUND`, `BAD_ADDRESS` or `CORRUPT` produce no mutation and explicit feedback. The implementation never retries or falls back to a different lock automatically.

After a successful delete, the held-step gesture remains active. If locks remain, selection moves deterministically to the lock now occupying the deleted lock's old ordinal; if the deleted item was the last ordinal, selection clamps to the new last lock. If no locks remain, selection becomes NONE. Exactly one D028 record is returned to the free list and the surrounding step/list state remains validated.

### No clear-all gesture in WP3.4

D028's internal `fmt_lock_clear()` primitive remains verified and available, but WP3.4 intentionally does **not** expose it on the panel. A step can hold at most four locks, so deliberate one-by-one deletion is bounded and fast enough for this prototype. A mass-destructive shortcut would add disproportionate accidental-loss risk before physical hardware ergonomics are known. A later focused editor or Phase 7 decision may add clear-all with its own explicit safeguard without changing D028 storage.

### Feedback and value handling

WP3.4 continues to use only the existing D026 footer region. Full row lock markers remain WP3.9.

While a valid lock is selected, the footer may show ordinal/count, semantic short name and value. A stored current-ID value above D029's `0..255` editor domain is still a structurally valid D028 lock: browse may identify it as RANGE and deletion remains allowed because removal does not reinterpret its value. Zero locks show explicit no-lock feedback. An armed delete shows the semantic target and confirmation hint. Successful delete and D028 NOT_FOUND/BAD_ADDRESS/CORRUPT failures have explicit bounded feedback. Footer updates remain main-loop-only and under the existing D026 two-regions-per-poll redraw limit.

### Input order

For a stable D029 hold, WP3.4 input precedence is deterministic:

1. HOME/context exit, STEP REC transition and owner-key release cancel the hold/arm first;
2. all SELECT + KNOB1..4 encoder streams are drained;
3. unrelated focus/navigation/PLAY intent cancels a pending delete arm before any confirmation;
4. an existing delete arm may accept a later SEL confirm;
5. a new EDIT press may arm deletion only if no confirm occurred and a valid selected lock exists;
6. if no delete arm is active, SELECT browse is applied before KNOB1..4 edits;
7. KNOB1..4 retain D029's fixed K1 -> K4 order, and a successful write selects that parameter.

A poll that begins or ends a held-step gesture consumes no browse/edit deltas. A poll that creates a delete arm cannot also confirm it.

WP3.4 remains storage-only. Delete/browse operations must not alter `FmtStep.note`, instrument, volume, effect, effect value or flags, proof/manual voice state, base instrument/track parameters or transport timing. No p-lock is read or applied by audio yet.

**Reason:** D028 already supplies sorted bounded iteration and corruption-safe one-lock removal, so WP3.4 should expose those primitives without inventing a second storage model. SELECT is the panel's natural navigation encoder. Requiring an EDIT arm followed by a later SEL confirm makes deletion intentional, gives the display time to name the destructive target, and prevents same-poll chords or stray knob motion from deleting data. Deferring clear-all keeps the first destructive UX small and recoverable while physical ergonomics remain unknown.

**Consequence:** WP3.4 may extend `FmtLockEditor` with only the small bounded selection/arm state required by this contract. Tests must cover zero/one/four-lock browse, clamping, stale SELECT discard, KNOB-write selection, same-poll EDIT+SEL rejection, EDIT-release cancellation, PLAY/unrelated-input cancellation, first/middle/last/only deletion, exactly-one-record free-list recovery, neighbor/row isolation, RANGE-value deletion, and D028 NOT_FOUND/BAD_ADDRESS/CORRUPT failure paths. The full app must demonstrate the real panel sequence with zero unintended MIDI. All Phase 2, WP3.1, WP3.2 and WP3.3 regressions plus the frozen WAV remain required. WP3.5+ still owns application of locks to playback/audio, and Phase 7 still owns final physical gesture approval.

## D031 — WP3.5 applies three row-local p-lock parameters through a 32-bit main-to-audio row snapshot

Date: 2026-10-24  
Status: accepted for the WP3.5 one-lane proof

**Decision:** WP3.5 keeps the existing D021 proof lane fixed to pattern 0 / logical track 0 and makes exactly three current D027 parameters audible:

- `FMT_PARAM_VOICE_LEVEL` (`0x01`);
- `FMT_PARAM_PITCH_FINE` (`0x02`);
- `FMT_PARAM_TRACK_LEVEL` (`0xA0`).

`FMT_PARAM_TRACK_PAN` (`0xA1`) remains fully editable/storable/browsable/deletable but **storage-only** in WP3.5. No stereo pan law is frozen before a real engine/mixer package needs it.

### Row-local base/default model

Playback values are recomputed independently for every 1/16-note row. Missing accepted locks use explicit playback defaults:

- VOICE LEVEL = 255;
- FINE PITCH = 128;
- TRACK LEVEL = 255.

A lock affects only the row that owns it. The next row with no corresponding lock restores that parameter to its default. No p-lock value persists implicitly through later rows. This is intentionally different from a stateful automation lane and does not mutate base instrument/track parameters.

### Level mapping

VOICE LEVEL and TRACK LEVEL both use the full normalized `0..255` domain already exposed by D029. `255` means unity relative to the current proof voice target, `0` means zero target amplitude.

The current proof base remains `TRACKER_TARGET_AMP = 6000`. Composition order is frozen as:

`gate = scale_u8(scale_u8(6000, voice_level), track_level)`

where:

`scale_u8(x, v) = (x * v + 127) / 255`

using bounded 32-bit integer arithmetic. This gives exact unity at 255 and exact zero at 0. No dB law is implied; later engine/mixer work may supersede the perceptual curve without changing stored normalized values.

### Fine-pitch mapping

FINE PITCH uses a signed Q7 semitone offset centered at 128:

`offset_q7 = fine_value - 128`

so:

- value 0 = -128/128 = -1.000 semitone;
- value 128 = 0 semitones;
- value 255 = +127/128 = +0.9921875 semitone.

For a stored base MIDI note `n`, effective pitch is:

`pitch_q7 = clamp(n * 128 + offset_q7, 0, 16383)`

covering MIDI 0 through 127 + 127/128. Integer note increments continue to come from the existing `note_inc()` semitone table. Fractional Q7 pitch is implemented by deterministic **linear interpolation of phase increment** between adjacent semitone increments, not by floating point and not by claiming an exact exponential cent law between table points. The interpolation uses only 32-bit multiply/add/shift decomposition; no runtime 64-bit division is introduced. Fine value 128 must produce exactly the historical `note_inc(n)` result.

### NOTE / TIE / OFF / EMPTY interaction

The row parameter snapshot and note event are consumed together at the existing D021 dispatch boundary.

- **NOTE:** stores the new base note, marks the proof voice logically active, computes pitch from that note plus the row FINE value, and sets the row-composed level target.
- **TIE:** does not replace the stored base note and does not retrigger phase. If a proof note is logically active, the row's FINE and level snapshot may change its pitch/target for that row. A TIE with no logically active note remains silent and cannot resurrect an OFF'd voice.
- **EMPTY / NONE:** sends no note event. If a proof note is logically active, the row's FINE and level snapshot still applies, allowing explicit p-lock modulation on note-empty rows and restoring defaults on an unlocked row. If no note is logically active, it does nothing audible.
- **OFF:** clears logical note-active state and sets gate target to zero. The release tail keeps its already active oscillator pitch; the OFF row does not retune or re-open the released voice. Later TIE/EMPTY rows cannot resurrect it. A later NOTE creates a new active note with that row's snapshot.

The proof voice therefore tracks logical note-active state separately from numeric gate target. A level lock of zero mutes an active note for that row but does **not** semantically turn it into NOTE OFF; a later unlocked row may restore the default level while the note remains logically active. Oscillator phase continues while logically active even if the current level target is zero.

This explicitly extends D025: the NOTE/TIE/OFF/EMPTY event itself still never changes pitch except NOTE, but an independent D031 row p-lock snapshot may modulate an already-active voice on TIE/EMPTY rows.

### Main/editor to audio ownership

The audio ISR must **never traverse D028's mutable linked list**.

WP3.5 introduces a fixed row-parameter cache for the 64 rows of the existing pattern-0/track-0 proof lane. Each row is published as one naturally aligned 32-bit word:

- bits `0..7`: VOICE LEVEL;
- bits `8..15`: FINE PITCH;
- bits `16..23`: TRACK LEVEL;
- bits `24..31`: resolver flags/status for tests/diagnostics; audio ignores these flags.

The default payload is therefore `voice=255, fine=128, track=255` with zero flags.

Main/editor-side refresh starts from defaults, validates the D028 row with its bounded public read APIs, inspects at most `FMT_LOCKS_PER_STEP = 4` records, applies only the three accepted IDs and ignores TRACK PAN for audio. A structurally corrupt valid proof row publishes the complete safe default payload with a structural-fallback flag. A structurally valid accepted lock whose value is above 255 falls back to that parameter's default only and sets a domain-fallback flag; other valid accepted locks on the row remain usable. Invalid caller step coordinates are rejected without out-of-bounds publication.

The cache is reset/rebuilt outside the ISR at startup. Every user lock set/replace/delete attempt on pattern 0 / track 0 is followed in main context by a refresh of that one target row. Edits to tracks 1..7 cannot alter the proof-lane cache.

Publication is a single aligned 32-bit main-context store. Audio reads exactly one 32-bit word by `event.step` at a row dispatch. If an editor publication races the exact row boundary, audio may observe either the previously published complete word or the newly published complete word, but never a torn/partially resolved lock list. An edit becomes audible only on a row dispatch that occurs after its snapshot has been published; there are no mid-row parameter changes.

This is a narrow row cache, **not** the general event queue planned for WP3.8.

### Timing and hard bounds

`FmtPlaybackEvent` remains exactly 4 bytes and keeps `boundary, step, kind, note`. WP3.5 does not enlarge it.

D021 timing remains unchanged:

- step 0 event + row snapshot are consumed before rendered frame 0;
- each audio frame renders first;
- D020 transport ticks at the end of the frame;
- a crossed boundary emits the next row;
- the next row's 32-bit snapshot and note event become active beginning with the next rendered frame.

Per-sample DSP work is constant and independent of lock count. D028 list validation/resolution happens only in main/startup refresh work. Audio row-boundary overhead is one bounded 32-bit snapshot load, fixed unpacking, fixed integer level/pitch mapping and the existing one event application. No allocator, list traversal, retry loop, screen work or flash work enters the ISR.

### Default/fallback and regression rule

The existing proof pattern contains no locks. A default row snapshot (`255/128/255`) must therefore produce the historical note increment and target amplitude exactly. The complete historical default host WAV remains frozen and must stay byte-identical:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

WP3.5 adds a **separate deterministic locked-audio fixture** proving VOICE LEVEL, TRACK LEVEL, FINE PITCH, combined level composition, row-local restore, TIE/EMPTY modulation, OFF non-resurrection and safe fallback. The new locked fixture gets its own frozen evidence/hash; it does not replace the no-lock regression.

Playback never writes row fields or lock records. Stored `FmtStep` and D028 data must remain byte-for-byte unchanged by playback.

**Reason:** Three parameters are enough to prove the p-lock-to-audio architecture against the current sine proof without inventing pan law or future engine semantics. A 64 × 32-bit cache costs only 256 bytes of ordinary RAM and removes mutable linked-list traversal from the real-time path. Row-local restore is simple, tracker-like and testable. Keeping the 4-byte playback event and D021 timing limits architectural churn before the real engines and WP3.8 queue work.

**Consequence:** WP3.5 must add a bounded playback-parameter resolver/cache, exact integer pitch/level helpers, proof-voice logical-active state and one-row cache refresh after relevant editor mutations. Unit tests must cover all 64 rows, accepted/ignored IDs, defaults, domain fallback, structural corruption fallback, snapshot publication, level composition, fine-pitch endpoints/center, NOTE/TIE/OFF/EMPTY semantics and no stored-data mutation. Full-app/default audio must keep the frozen WAV byte-identical, while a separate locked fixture proves audible deterministic changes and row-local restoration. All Phase 2 and WP3.1-WP3.4 regressions remain required. WP3.6+ modulation/effect commands, WP3.8 general event queue and TRACK PAN playback remain out of scope.
## D032 — WP3.6 tracker effects use exact six-tick D020 timing and bounded persistent pitch state

Date: 2026-10-24
Status: accepted before WP3.6 production source

**Decision:** WP3.6 uses the existing `FmtStep.effect/effect_value` bytes as a tracker-effect command domain that remains separate from D027 p-lock parameter IDs. Assigned commands are `NONE=0x00`, `ARPEGGIO=0x01`, `SLIDE_UP=0x02`, `SLIDE_DOWN=0x03`, `PORTAMENTO=0x04`, and `VIBRATO=0x05`; all other command bytes resolve safely to NONE. `FmtStep` remains exactly 8 bytes.

Effect timing is derived directly from D020: `661500 / 6 = 110250` phase units gives exactly six effect ticks per row. Tick 0 starts at D021 row dispatch; internal tick boundaries occur at transport phases `110250`, `220500`, `330750`, `441000`, and `551250`. The real D020 phase/residual remains authoritative, so WP3.6 introduces no second free-running clock or cumulative rounding.

Persistent proof pitch is Q7. SLIDE_UP/DOWN and PORTAMENTO may move persistent pitch; D031 FINE remains row-local and never mutates that state. ARPEGGIO and VIBRATO are transient only. Final pitch precedence is `persistent_pitch_q7 + (D031_FINE - 128) + transient_effect_mod_q7`, clamped to `0..16383`. NOTE/TIE/EMPTY/OFF, portamento-target memory, vibrato phase continuity, exact command arguments, and saturation/reset rules are frozen in the detailed contract below.

Main/startup publishes one aligned 32-bit effect snapshot for each of the 64 proof-lane rows, exactly 256 bytes ordinary RAM. Audio reads one effect word at row dispatch; `FmtPlaybackEvent` remains exactly 4 bytes. Effect runtime state is fixed at no more than 16 bytes. Per-frame work is bounded to at most one effect-threshold comparison plus fixed integer work at an actual effect tick. No allocator, D028 traversal, variable event loop, screen/flash work, float, or runtime 64-bit division enters the audio path.

The normative detailed WP3.6 contract is `docs/WP3_6_EFFECT_CONTRACT.md`. It freezes ARPEGGIO `XY` sequence `0,X,Y,0,X,Y`; Q7 SLIDE_UP/DOWN; Q7 PORTAMENTO with remembered target; 64-phase triangle VIBRATO; NOTE/TIE/EMPTY/OFF behavior; D031 FINE precedence; invalid-command fallback; cache ownership; real-time bounds; and required deterministic regressions.

**Reason:** six ticks divide the existing D020 integer row phase exactly, preserving tracker-style timing without pulling WP3.8 queue architecture forward. Keeping commands in the existing two step bytes preserves the data model and D027 separation between tracker effects and p-lock parameters. A fixed proof-lane cache provides bounded main-to-audio publication analogous to D031.

**Consequence:** WP3.6 may add only the small effect registry/cache/state and proof-lane audio integration required by `WP3_6_EFFECT_CONTRACT.md`. Retrig/note-delay/note-cut, general queue work, effect-entry UI, new synth engines, persistence, and live capture remain out of scope.
