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

## D033 — WP3.7 uses fixed-state six-tick retrig / note-delay / note-cut scheduling without the WP3.8 queue

Date: 2026-10-24
Status: accepted for the WP3.7 one-lane proof

**Decision:** WP3.7 extends the existing `FmtStep.effect/effect_value` command domain with `0x06 RETRIG`, `0x07 NOTE_DELAY` and `0x08 NOTE_CUT`. The detailed normative semantics are frozen in `docs/WP3_7_EVENT_CONTRACT.md`.

The three commands reuse D032's exact six positions per row: tick 0 at row dispatch and ticks 1..5 at transport phases `110250`, `220500`, `330750`, `441000`, `551250`. The next row at `661500` wins over any sub-row action; no tick 6 exists.

`RETRIG` values `1..5` are tick intervals and fire at internal ticks divisible by the interval; value `0` performs no repeats and values above `5` are safe no-ops. The proof adapter resets oscillator phase only, preserving persistent pitch, row level target and amplitude state; future engines may map the same semantic action to their envelope retrigger primitive.

`NOTE_DELAY` values `1..5` withhold only an ordinary NOTE until that tick; value `0` is immediate and values above `5` do not suppress the NOTE. TIE, EMPTY/NONE and OFF are never delayed. While a NOTE waits, a previous active note may continue and receives the current row's D031 row-local parameter snapshot. The delayed NOTE then uses that same row snapshot and follows ordinary non-portamento persistent-pitch semantics.

`NOTE_CUT` values `0..5` synthesize full D025 OFF semantics at that tick without rewriting the stored row; values above `5` are safe no-ops. CUT can stop a new same-row note or an earlier sustained note and cannot be resurrected by later TIE/EMPTY/retrig ticks.

Because one row still owns only one `effect/effect_value` command, D032 pitch effects and D033 event commands cannot be simultaneous on the same row. D031 p-locks remain orthogonal and may coexist. RETRIG does not mutate persistent pitch/porta state; delayed NOTE sets persistent pitch when it lands and clears a pending porta target; CUT clears active pitch/porta/transient state through OFF semantics.

WP3.7 explicitly does **not** introduce the general WP3.8 queue. One command/value, one pending note byte and one pending-valid byte are sufficient fixed state, with a target of at most 4 bytes. RETRIG repetitions are derived mathematically from the current D032 tick and never enqueued. The existing D032 64-row effect cache is reused; no third row cache is required.

Reserved command bytes `0x09..0xFF` continue to resolve to NONE. Valid D033 commands with value above `5` never wrap modulo six and never schedule work into another row. Playback never mutates `FmtStep`, p-lock records or stored effect bytes.

`FmtStep` remains exactly 8 B, `FmtPlaybackEvent` remains exactly 4 B, D020/D021 ordering is unchanged, and all D031/D032 audio regressions remain mandatory.

**Reason:** the existing one-command-per-row model plus D032's exact six-tick clock gives a complete bounded implementation for retrig, delay and cut. A queue would add overflow and ordering machinery without solving a WP3.7 requirement and is therefore correctly deferred to WP3.8.

**Consequence:** WP3.7 source may extend D032 command validation/cache and add one tiny fixed event scheduler plus the minimal proof-voice adapter hooks. Tests must cover values `0..5` and `>5`, exact frame/tick timing, NOTE/TIE/EMPTY/OFF interaction, delayed-note D031 row values, CUT non-resurrection, RETRIG after OFF no-op, D032 persistent pitch/porta continuation, invalid-command NONE fallback, stored-data immutability and all inherited regressions. No WP3.8 queue/overflow implementation may be added under WP3.7.


## D034 — WP3.8 uses a 32-entry SPSC main-to-audio runtime event queue with reject-newest overflow

Date: 2026-10-24
Status: accepted before WP3.8 production source

**Decision:** WP3.8 freezes the general runtime-event ingress anticipated by the Phase-0 architecture/data model as a single-producer/single-consumer main-loop -> audio-ISR queue. The normative detailed contract is `docs/WP3_8_QUEUE_CONTRACT.md`.

D034 does **not** move D033 RETRIG / NOTE DELAY / NOTE CUT into a queue. Those remain the verified four-byte fixed-state scheduler on D032's exact six-tick clock. D024's PLAY desired-state mailbox likewise remains a mailbox because desired run state is idempotent state rather than an ordered discrete event.

`FmtRtEvent` is frozen at exactly 8 bytes: one-byte `type`, one-byte logical `track`, 16-bit `value_a`, 32-bit `value_b`. WP3.8 assigns only NOTE_ON (`0x01`) and NOTE_OFF (`0x02`); `value_b` remains zero/reserved and is not silently defined as a timestamp. Queue slots publish the event as two aligned 32-bit words.

Capacity is exactly 32 events. The 256-byte slot array plus producer-owned `write_seq`, consumer-owned `read_seq` and producer-owned `overflow_seq` gives a target `FmtRtQueue` size of exactly 268 bytes ordinary RAM. Capacity 32 covers all 27 physical FM-1 note-key edges from one main-loop poll while retaining deterministic behavior for larger USB-MIDI bursts.

Publication is SPSC: main writes both slot words, executes a compiler memory barrier, then advances `write_seq`; audio observes `write_seq`, executes the matching barrier, reads both words, then advances only `read_seq`. No interrupt masking, lock, allocator, spin or retry is allowed. Unsigned monotonic sequence counters and a power-of-two mask define wrap-safe bounded occupancy.

Overflow is **reject-newest**. When the queue already contains 32 unread events, the producer leaves every unread slot and `write_seq` untouched, increments `overflow_seq` once and returns FULL. Drop-oldest is explicitly rejected because it would require producer mutation of consumer ownership or overwrite unread state.

The first production integration is the existing manual/audition NOTE ON/OFF path. Main no longer chooses/mutates Dinghy manual voice internals directly; it enqueues note intent, and audio consumes up to 32 events once at callback start before rendering. This is not live recording. Current hardware/USB-MIDI audition simply proves the real main->audio boundary already required by the architecture.

Because reject-newest could otherwise lose NOTE_OFF, the audition adapter observes `overflow_seq`. On any change it releases all four manual voice gates click-safely and consumer-discards the pending batch in O(1) by advancing `read_seq` to the published `write_seq`. Overflow therefore fails toward silence, never toward an indefinitely stuck note.

WP3.8 queue timing is callback-boundary immediate ingress, not a new sample-accurate scheduler. Successfully queued events are FIFO-consumed at the beginning of the next audio callback. D033 remains responsible for its already verified sample-exact sub-row timing. Future live capture/sample-offset semantics require a later explicit decision and may assign event-specific meaning to the reserved `value_b` without changing the queue envelope.

**Reason:** The current proof firmware still has one architectural shortcut where main-context `play()` / `stop()` directly mutate manual audio voice state. A small SPSC queue removes that cross-context mutation, matches the original architecture, gives future live-performance work a measured ingress primitive, and provides a real overflow problem to solve without dragging live recording or multi-track engine work into Phase 3. A 32-entry/8-byte design is large enough for one full 27-key hardware edge set but cheap enough to measure now.

**Consequence:** WP3.8 may add only the 8-byte runtime event envelope, exact 268-byte queue, NOTE ON/OFF audition adapter and the tests/diagnostics required by `WP3_8_QUEUE_CONTRACT.md`. Tests must prove exact layout, FIFO, full/reject-newest semantics, sequence wrap, O(1) discard, forced-overflow no-stuck-note recovery, real key/MIDI audition, queue-only MIDI neutrality, target alignment/resource cost and all inherited Phase 2 + WP3.1-WP3.7 deterministic audio regressions. Live REC/capture, timestamped automation, D033-through-queue, multi-track runtime dispatch, new engines and visual feedback remain out of scope.


## D035 — WP3.9 extends the existing bounded tracker surface with lock/effect feedback

Date: 2026-10-24
Status: accepted and verified

**Decision:** WP3.9 keeps the D026 240×240 tracker screen, ten redraw regions and maximum two tracker-region renders per main-loop poll. It adds no screen/page/menu architecture and no audio/ISR display coupling. The normative detailed contract is `docs/WP3_9_VISUAL_CONTRACT.md`.

Rows are `ST NOTE IN VOL LK FX`. `LK` summarizes D028 storage as `--`, `L1..L4`, or `L!`; while the D029/D030 held-step editor owns that visible row it becomes `P0..P4` or `P!`. The existing D026 `>` cursor and `*` playhead remain independent and unchanged.

`FX` is a four-character view of stored `FmtStep.effect/effect_value`: `----`, `ARhh`, `SUhh`, `SDhh`, `POhh`, `VIhh`, `RThh`, `DLhh`, `CThh`, or `????` for reserved commands. The raw stored value is shown in uppercase hexadecimal; presentation never clamps or reinterprets D032/D033 semantics.

TRACK PAN remains storage-only under D031. The row summary intentionally reports only lock storage/count. Detailed D029/D030 footer feedback appends `STORE` when the selected parameter is TRACK PAN so the storage-only status is visible without inventing a second row-level classification.

`FmtTrackerScreen` is frozen at exactly 62 bytes ordinary RAM, still below the original D026 64-byte cap. Each sync inspects only the same eight visible rows and performs at most one bounded `fmt_lock_count()` per row, therefore at most 32 lock-record inspections. No pool allocation or retained mutable lock pointer is introduced.

**Reason:** the existing D026 row already reserved an FX column but intentionally deferred stable effect names, while D028-D030 p-lock state had detailed footer feedback but no normal-row presence indication. D035 makes the now-stable Phase 3 semantics legible without increasing redraw topology or crossing the main/audio ownership boundary.

**Consequence:** WP3.9 changes presentation only. The reviewed active/released framebuffer PPM hashes are `d290415387ac8e55006863819c2188d64f0d0453e204bb194cb8d004121484f3` and `ec15c6db429fe26d453ada14cc28a1eab8c2aa93e83c6f0bbfff67112f1a86c6`. All frozen D031-D034 audio/runtime behavior remains byte-identical. Final resources are 66,960 B image / 13,296 B normal RAM / 113,000 B pool. Evidence is `docs/WP3_9_VERIFICATION.md`.

## D036 — WP3.10 is a worst-case verification and Phase 3 exit package, not a feature package

Date: 2026-10-24
Status: accepted and verified — Phase 3 exit complete

**Decision:** WP3.10 starts from the final WP3.9 production checkpoint `354bf7d356774ad828f1f660bd456dbab7f0490d` and freezes its complete stress/exit obligations in `docs/WP3_10_TEST_PLAN.md`. Production source should remain byte-identical unless a stress test exposes a real defect; any such fix must be minimal, receive a focused regression and trigger the entire exit gate again.

D036 requires worst-case D028 pool saturation/recovery, D031/D032 cache boundaries, a full D032/D033 timing matrix, D034 saturation/wrap/overflow storms, combined playback/audition/editor/UI stress, D035 maximum visible-row lock inspection and dirty-backlog convergence, repeated editor/context transitions, long deterministic clock checks, static real-time boundedness audit, all inherited regressions and the pinned JieLi/AC79 target-build/resource gate.

**Reason:** Phase 3 has accumulated several individually bounded systems whose final risk is interaction under simultaneous worst-case load rather than another missing feature. Closing the phase requires proving those declared bounds together before engine work begins.

**Consequence:** D036 passed without finding a production defect. WP3.10 therefore leaves the final WP3.9 production checkpoint `354bf7d356774ad828f1f660bd456dbab7f0490d` byte-identical and adds zero production resource cost. Final run `40084706845`, job `120845144453`, artifact `12556590742` (`c4f494716ec98507ca9af6fe59313c01116136deececa7ba84e24598e78b82ee`) satisfies the Phase 3 host/device-build exit gate. Final resources remain 66,960 B image / 13,296 B normal RAM / 113,000 B pool and firmware hashes remain `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27` / `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`. Evidence is `docs/WP3_10_VERIFICATION.md`.

Phase 3 is now formally complete at host/device-build level. Phase 4 may begin only with WP4.1 / D037 engine-interface architecture; physical FM-1 install/audio/ergonomics/LCD/execution-margin gates remain explicitly open.

## D037 — Phase 4 uses one bounded eight-voice 8-BIT engine interface

Date: 2026-10-24
Status: accepted — architecture frozen before production engine DSP

**Decision:** WP4.1 freezes the complete Phase 4 engine/voice/realtime boundary in `docs/WP4_1_ENGINE_CONTRACT.md` before PULSE, NOISE, TRIANGLE or WAVETABLE-8 production DSP is written.

The Phase 4 `8-BIT` family is `FmtInstrumentHeader.mode == 1`. Within that family engine IDs are `0 SAFE/NONE`, `1 PULSE`, `2 NOISE`, `3 TRIANGLE`, `4 WAVETABLE-8`; unsupported values fail to silence rather than aliasing another engine. The existing mode-zero proof firmware remains untouched until a later explicit migration package.

Audio owns all mutable engine runtime state. Phase 4 has exactly eight logical track adapters and a fixed maximum of eight engine voice slots, with one primary sequenced voice handle per track. Main/editor owns song/lock/UI mutation and may reach audio only through bounded aligned publication snapshots plus the already-frozen D034 audition queue. D021/D032/D033 tracker timing does not move into that queue.

The engine consumes the existing D031/D032 unsigned Q7 semitone pitch domain `0..16383`, with one semitone equal to 128 units. D032 remains the owner of persistent/transient pitch effects; D033 remains the owner of RETRIG / NOTE DELAY / NOTE CUT scheduling. Normal NOTE does not imply oscillator-phase restart, while RETRIG explicitly restarts deterministic oscillator phase/state without allocation or pitch change. TIE/EMPTY never allocate or resurrect a voice; OFF/CUT release through the common gate/envelope boundary.

Oscillator selection uses explicit bounded integer `switch` dispatch, never a vtable or runtime registration. Pitched engines use a 32-bit wrapping phase accumulator. Each oscillator produces one signed nominal-Q15 mono sample, common envelope/level gain is unsigned Q15, and at most eight scaled voices are summed in signed 32-bit accumulators with explicit saturation and no integer wrap. TRACK PAN remains storage-only until a later explicit package makes it audible.

D027 remains authoritative for parameters. Phase 4 engine-specific IDs may only come from `0x20..0x5F`; concrete IDs/page mappings remain WP4.8. Audio never traverses D028. The reserved maximum resolved engine-parameter publication shape is one aligned 32-bit word per `(track,row)`, at most `8 * 64 * 4 = 2048` bytes ordinary RAM.

The hard realtime bound is fixed: at callback ingress at most 32 D034 events; at a row boundary at most eight track dispatches; at an effect tick at most eight track updates; per rendered frame at most eight voice renders plus fixed gain/mix work; allocation may inspect at most eight slots. Heap/pool allocation, D028 traversal, screen work, persistence, blocking waits/retries, float, runtime 64-bit division, recursion and unbounded search are forbidden in audio.

The complete Phase 4 incremental ceilings versus the frozen Phase 3 baseline are `+65,536 B` app image, `+8,192 B` normal RAM and `+16,384 B` pool RAM, yielding total ceilings of `132,496 B`, `21,488 B` and `129,384 B`. Within normal RAM, the optional full engine-row cache may consume at most 2,048 B and all engine realtime voice/track/allocator/envelope state together should remain at or below 2,048 B. Crossing a ceiling requires a new recorded decision before acceptance.

**Reason:** Phase 3 deliberately proved tracker timing, p-locks, effect scheduling, runtime ingress and UI before replacing the sine proof lane. Phase 4 now needs one measured engine contract so oscillator packages cannot invent incompatible pitch domains, ownership, queues, allocation rules, parameter paths or mix formats independently. Eight fixed voices cover the eight logical sequencer tracks while keeping worst-case audio work trivial to bound and leaving substantial device headroom for Phase 5 and later core work.

**Consequence:** WP4.1 is documentation/architecture only and must close with zero production firmware/DSP delta from the clean Phase 4 branch start. WP4.2-WP4.8 may build isolated deterministic primitives behind this interface. The first production migration away from the Phase 3 proof sine must be explicit, preserve D020-D034 semantics through the adapter, rerun inherited regressions and freeze new intentional audio evidence instead of silently rewriting old hashes. WP4.10 must prove the final eight-track Phase 4 configuration remains inside the D037 ceilings or record a superseding decision.

## D038 — WP4.2 PULSE is a 4-byte hard-edge oscillator using the existing Q7 pitch math

Date: 2026-10-24
Status: accepted — frozen before PULSE production source

**Decision:** WP4.2 implements one isolated hard-edge PULSE primitive plus a canonical shared Q7-to-phase-increment helper, exactly as frozen in `docs/WP4_2_PULSE_CONTRACT.md`. It is not connected to the full-app proof lane in WP4.2.

`FmtPulseOsc` contains exactly one `uint32_t phase` and is therefore exactly 4 B. Reset and retrigger set phase to zero. Rendering samples the current phase before advancing it, returns exactly `+32767` or `-32767`, then advances phase by unsigned 32-bit addition/wrap. A zero increment holds phase and output. Null render fails to silence with return value zero.

Pulse width is one unsigned byte with a legal oscillator-domain range `32..224`, default `128`, interpreted as `width / 256` of one cycle high. The render threshold is `width << 24`; values below/above the legal range clamp to 32/224. This yields a continuous 12.5–87.5% range containing the classic 12.5/25/50/75% chip widths without permitting a deliberate all-high/all-low DC mode. WP4.8, not WP4.2, owns the eventual stable parameter ID/page mapping.

D038 explicitly keeps PULSE band-unlimited/hard-edge. WP4.2 adds no BLEP/polyBLEP/minBLEP, oversampling or hidden smoothing/filter. Upper-register aliasing is accepted as part of the Phase 4 chip-style identity. Any later anti-aliasing strategy must supersede D038 because it changes state, CPU cost and deterministic sample vectors.

The common pitch helper reuses the exact existing Phase 3 proof-lane behavior: unsigned Q7 semitone pitch `0..16383`, the existing 12-entry C9 phase-increment table, octave shifts and `fmt_playback_interp_inc()` rounding. Frozen compatibility vectors include C4 `25480118`, A4 `42852281`, A4+0.5 semitone `44126346`, MIDI127 `1221665363` and maximum Q7 `1293741834`. The helper runs only when pitch changes, not per rendered sample. No float or runtime 64-bit division is permitted.

WP4.2 remains isolated: `fmt_engine_pitch.*` and `fmt_osc_pulse.*` must compile independently with the pinned JieLi compiler but are not included by `dinghy.c`. Therefore the required full-app image/RAM/pool delta, default WAV delta and BIN/FWSC delta for WP4.2 are all exactly zero. The isolated state cost is 4 B per PULSE oscillator (32 B for eight future instances), the immutable pitch table is 48 B, and there is no static mutable or pool storage.

**Reason:** D037 requires one common pitch domain, fixed state and bounded per-sample work before engine integration. Reusing the already-proven pitch integers avoids a second tuning model; phase-only state makes PULSE trivial to bound; a 12.5–87.5% continuous duty range preserves classic chip voices without pathological DC endpoints; and deliberately postponing full-app integration keeps the Phase 3/WP4.1 audio/device baseline intact while the first oscillator primitive is tested in isolation.

**Consequence:** Focused tests must freeze exact pitch vectors, 25/50/75% sample vectors, clamp behavior, phase wrap, zero increment, deterministic replay and a long-render hash before WP4.2 closes. Both new translation units require real PI32v2 target compilation and static realtime audit. No NOISE/TRIANGLE/WAVETABLE-8, envelope, allocator, engine parameter page/cache or proof-sine migration may be pulled into WP4.2. Any future anti-aliasing or change to pitch integers/pulse edge ordering requires a superseding decision and new deterministic evidence.

## D039 — WP4.3 NOISE is an 8-byte clocked 15-bit LFSR with a saturated x16 pitch clock

Date: 2026-10-24
Status: accepted — frozen before NOISE production source

**Decision:** WP4.3 implements one isolated deterministic NOISE primitive exactly as frozen in `docs/WP4_3_NOISE_CONTRACT.md`. It remains disconnected from `dinghy.c` and the running full app.

`FmtNoiseOsc` is exactly 8 bytes: one 32-bit wrapping clock phase plus one 32-bit field whose low 15 bits hold the LFSR. Reset and D033 RETRIG both restore phase zero and seed `0x5A3D`. The LFSR recurrence is fixed to a right shift with feedback `bit0 XOR bit1` inserted at bit14: `(state >> 1) | (((state ^ (state >> 1)) & 1) << 14)`. Every valid nonzero state lies on the same maximal 32,767-step cycle; WP4.3 defines no alternate short/metallic tap mode.

NOISE samples current LFSR bit0 before any state advance and maps it exactly to `+32767` for one and `-32767` for zero. It then adds one unsigned 32-bit noise-clock increment to phase. Only an unsigned phase wrap may advance the LFSR, and one render can therefore perform zero or one LFSR step — never a catch-up loop.

D039 reuses D038 pitch rather than inventing another tracker pitch domain. The NOISE clock is `fmt_engine_pitch_inc_q7(pitch_q7) * 16`, implemented as a saturating left shift: inputs above `UINT32_MAX >> 4` return `UINT32_MAX` instead of wrapping. This yields approximately 4.186 kHz LFSR clock at C4 and 7.040 kHz at A4, while low notes remain deliberately coarse and high notes saturate toward one state update per audio frame.

The primitive is intentionally clocked/periodic chip noise, not mathematically white full-band noise. There is no interpolation, whitening, BLEP, oversampling, filter or hidden random source. The 32,767-step period, sample-and-hold behavior and clock/sample-rate aliasing are accepted parts of the Phase 4 8-BIT identity.

Frozen deterministic evidence includes the reset seed, first LFSR states, the `0x80000000` sixteen-sample sign vector `++--++++++++----`, exact x16/saturation clock vectors, full-cycle period proof and a 65,536-sample A4 signature: final phase `0xFB900000`, final LFSR `0x5DC1`, FNV-1a32 `0xF50385B6`.

WP4.3 remains isolated, so full-app image/RAM/pool/WAV/BIN/FWSC delta must remain exactly zero versus WP4.2. The isolated mutable state cost is 8 B per NOISE oscillator, 64 B for eight future instances, with no static mutable or pool storage.

**Reason:** D037 requires fixed-size deterministic engine state and bounded per-sample work, while NOISE must still share the tracker Q7 pitch domain. A 15-bit maximal LFSR is recognizably chip-like and cheap; the saturated x16 clock makes ordinary musical pitches useful as noise rates without overflow wrap; and the one-wrap/one-step rule makes worst-case audio work independent of pitch.

**Consequence:** Focused tests must prove the exact recurrence, 32,767-step period, reset/retrigger, clock vectors/saturation, zero/one-step render bound, short vectors, repeatability and long-run signature. `fmt_osc_noise.c` must compile with the real pinned PI32v2 compiler and pass the same no-float/no-int64/no-allocation/no-unbounded-loop audit. TRIANGLE, WAVETABLE-8, envelope, allocator, parameter pages and proof-sine migration remain later packages. Any alternate tap mode, whitening, seed, clock multiplier or multi-step catch-up rule requires a superseding decision and new deterministic evidence.

## D040 — WP4.4 TRIANGLE is a 4-byte phase-only integer triangle with exact zero crossings and Q15 peaks

Date: 2026-10-24
Status: accepted — frozen before TRIANGLE production source

**Decision:** WP4.4 implements one isolated deterministic TRIANGLE primitive exactly as frozen in `docs/WP4_4_TRIANGLE_CONTRACT.md`. It remains disconnected from `dinghy.c` and the running full app.

`FmtTriangleOsc` contains exactly one `uint32_t phase` and is therefore exactly 4 B. Reset and D033 RETRIG both set phase to zero. Rendering samples current phase before advancing it by the already-frozen D038 phase increment.

The exact phase-to-sample mapping derives `quadrant = phase >> 30` and `offset = (phase >> 15) & 0x7FFF`. Quadrants map to `offset`, `32767-offset`, `-offset`, and `-32767+offset`. Canonical quarter-cycle samples are therefore exactly `0, +32767, 0, -32767`, output is always inside D037 `-32767..32767`, and half-cycle antisymmetry is exact.

The 15-bit quarter-slope quantization is intentional. It supplies every required integer Q15 amplitude without multiplication, division or `-32768`; phase cells around quadrant boundaries may hold an extremum/zero for adjacent phase values. WP4.4 adds no interpolation, smoothing, BLEP, oversampling or hidden filter.

TRIANGLE reuses `fmt_engine_pitch_inc_q7()` directly and creates no engine-specific pitch math. The primitive has no parameter beyond phase increment, no waveform table and no static mutable state. One render is fixed O(1) integer work with no loop, allocation, tracker-store traversal, float or 64-bit arithmetic.

Frozen deterministic vectors include quarter-cycle `0,+32767,0,-32767` and eighth-cycle `0,+16384,+32767,+16383,0,-16384,-32767,-16383`. The 65,536-sample A4 signature is final phase `0xDFB90000`, FNV-1a32 `0xBC0F75ED`.

WP4.4 remains isolated, so full-app image/RAM/pool/WAV/BIN/FWSC delta must remain exactly zero versus WP4.3. The isolated mutable state cost is 4 B per TRIANGLE oscillator, 32 B for eight future instances.

**Reason:** D037 already freezes a common 32-bit phase/Q7 pitch domain and nominal-Q15 range. The quadrant/15-bit-offset construction gives natural zero-crossing reset semantics, exact positive/negative full-scale peaks, exact half-cycle symmetry and trivially bounded target arithmetic without introducing a table, multiply/divide path or parallel tuning model.

**Consequence:** Focused tests must prove exact canonical/boundary vectors, exhaustive high-16 range bounds and half-cycle antisymmetry, wrap/zero-increment/replay/guards, unchanged D038 pitch vectors and the frozen long-run signature. `fmt_osc_triangle.c` must compile with the real pinned PI32v2 compiler while the D038 pitch object stays byte-identical. WAVETABLE-8, envelope, allocator, parameter pages and proof-sine migration remain later packages. Any alternate phase orientation, amplitude rounding, interpolation or anti-aliasing requires a superseding decision and new deterministic evidence.

## D041 — WP4.5 WAVETABLE-8 is eight immutable 32-sample signed-8-bit tables with zero-order-hold lookup

Date: 2026-10-24
Status: accepted — frozen before WAVETABLE-8 production source

**Decision:** WP4.5 implements one isolated deterministic WAVETABLE-8 primitive exactly as frozen in `docs/WP4_5_WAVETABLE8_CONTRACT.md`. It remains disconnected from `dinghy.c` and the running full app.

WAVETABLE-8 means exactly eight curated immutable tables of 32 signed 8-bit samples each: SINE, SAW8, SQUARE8, ORGAN, HOLLOW, NASAL, METAL and DIGI. The 256 table bytes are frozen in the contract with row-major SHA-256 `f95b092a5106a8b05acb57d2960b0162b48ea42e2f8217b8923b0b7346046aee`. No runtime table generation or editable table memory belongs to WP4.5.

`FmtWavetable8Osc` contains exactly one `uint32_t phase` and is therefore exactly 4 B. Reset and D033 RETRIG set phase to zero. The top five phase bits select table index `phase >> 27`; there is no interpolation. Current phase/sample is read before the common D038 phase increment advances the oscillator.

Table selectors `0..7` select the eight exact identities. Every malformed selector `>=8` falls back to table 0 / SINE rather than wrapping modulo 8. The table bytes are restricted to `-127..127`; `-128` is deliberately unused. Each raw byte expands to nominal Q15 by exact integer multiplication `raw * 258`, yielding full table range `-32766..32766` without a clamp/divide/asymmetric endpoint case.

The primitive has fixed O(1) render work: one selector comparison, one phase shift, one immutable lookup, one constant signed multiply and one phase add/store. It has no loop, allocation, float, 64-bit arithmetic, tracker-store traversal or hidden smoothing. The 32-sample zero-order-hold steps, aliasing and imaging are intentional Phase 4 8-BIT character; later interpolation/anti-aliasing requires an explicit superseding decision.

Frozen 65,536-sample A4 FNV-1a32 signatures are: SINE `40a7ddab`, SAW8 `7c509a67`, SQUARE8 `a573d449`, ORGAN `1d6f9fcb`, HOLLOW `b8048a69`, NASAL `a3661307`, METAL `27204689`, DIGI `48f2989f`, each ending at phase `0xDFB90000`.

The isolated immutable image-data cost is exactly 256 B and mutable oscillator-state cost is 4 B per instance / 32 B for eight future instances. Because WAVETABLE-8 remains unlinked in WP4.5, full-app image/RAM/pool/WAV/BIN/FWSC delta must remain exactly zero versus WP4.4.

**Reason:** the fourth 8-BIT engine needs a broader tonal palette without introducing unbounded DSP, extra tuning math or large RAM. Eight small 32-step signed-byte waves give a deliberately lo-fi tracker palette for only 256 immutable bytes, while direct D038 phase ownership and no interpolation keep render cost trivial to bound.

**Consequence:** focused tests must prove all 256 frozen samples, selector fallback, `raw*258` expansion, zero/wrap/replay/guards, unchanged D038 pitch vectors, table-data SHA and all eight long-run signatures. `fmt_osc_wavetable8.c` must compile with the real pinned PI32v2 compiler while the D038 pitch object stays byte-identical. Envelope, allocator, parameter pages and proof-sine migration remain later packages. Any table-byte, interpolation, selector, expansion or table-count change requires a superseding decision and new deterministic evidence.

## D042 — WP4.6 common envelope is a 20-byte linear ADSR with hard NOTE/RETRIG articulation and division-free render rates

Date: 2026-10-24
Status: accepted — frozen before envelope production source

**Decision:** WP4.6 implements one isolated common linear ADSR amplitude envelope exactly as frozen in `docs/WP4_6_ENVELOPE_CONTRACT.md`. It remains disconnected from `dinghy.c` and all four Phase 4 oscillator primitives during WP4.6.

`FmtEnvelopeParams` is exactly 4 B (attack/decay/sustain/release bytes). `FmtEnvelope` is exactly 20 B: Q15.16 level, current rate, pre-resolved decay rate, sustain target and one explicit stage byte plus reserved alignment. Stage IDs are IDLE=0, ATTACK=1, DECAY=2, SUSTAIN=3 and RELEASE=4. Eight future envelope states cost exactly 160 B mutable RAM.

Level is `0..0x7FFF0000`; render returns unsigned Q15 `0..32767`. Sustain maps exactly as `(control * 257) >> 1`. A/D/R control 0 is instantaneous. Nonzero A/D/R use a frozen 16-entry `uint32_t` base-rate table indexed by the low nibble and shifted by the high nibble, so time resolution requires no runtime division and the render loop contains only fixed add/subtract/compare/clamp work. The 64 B rate table spans roughly 1.07 ms to 63.24 s full-scale slopes at 44.1 kHz.

D042 resolves event articulation explicitly. Ordinary NOTE hard-restarts the envelope from level zero but does not restart oscillator phase; TIE and EMPTY make no envelope call; D033 RETRIG performs the same hard envelope restart while D037 separately restarts oscillator phase/state; OFF and NOTE CUT start RELEASE from the exact current level. Zero-time stages collapse immediately without dummy rendered frames. `fmt_envelope_render()` advances the current linear stage before returning its Q15 level, matching the existing proof-lane ramp direction.

Frozen neutral/default params are A24/D0/S255/R72, approximately 3.02 ms attack and 23.97 ms full-scale release. The frozen long sequence A64/D96/S160, 4096 renders, R112 gate-off, 4096 renders transitions ATTACK->DECAY on render 705, DECAY->SUSTAIN on total render 1755, RELEASE->IDLE on release render 3534 and yields FNV-1a32 `0x40A44A2E` with final level zero/IDLE.

The primitive is intentionally isolated in WP4.6. Full-app image/RAM/pool/WAV/BIN/FWSC delta must remain exactly zero versus WP4.5. The only isolated immutable data is the 64 B rate table; there is no pool storage or allocation.

**Reason:** Phase 4 now has four deterministic oscillator primitives but still needs one common articulation boundary before allocator and engine integration. Linear ADSR gives familiar tracker articulation and explicit TIE legato while keeping state small. A compact rate table plus nibble shifting removes division from the audio render path, makes worst-case work trivial to bound and spans useful millisecond-to-minute slopes without floating point.

**Consequence:** Focused tests must prove exact state/layout, all 256 rate controls, sustain mapping, NOTE/TIE/RETRIG/OFF semantics, zero-time edges, monotonic ramps, exact terminal levels, default timing and the frozen long-run hash. `fmt_envelope.c` must compile with the real pinned PI32v2 compiler and contain no render loop/division/float/int64/allocation/store traversal. Final allocator, parameter mapping and full engine integration remain WP4.7-WP4.9. Any change to hard articulation, stage topology, rate table, update ordering or live retiming requires a superseding decision and new deterministic evidence.

## D043 — WP4.7 uses an eight-slot deterministic allocator that steals only detached RELEASE tails

Date: 2026-10-24
Status: accepted — frozen before allocator production source

**Decision:** WP4.7 implements one isolated fixed eight-slot allocator exactly as frozen in `docs/WP4_7_ALLOCATOR_CONTRACT.md`. `FmtVoiceAllocator` is exactly 52 B: eight 32-bit recency ages, one saturating 32-bit clock, eight voice->track owner bytes and eight track->voice bytes. `FmtVoiceAllocation` is exactly 4 B and reports NONE/REUSE/FRESH_IDLE/STEAL_TAIL plus whether engine state must be reset.

D043 preserves D037's one-primary-voice-per-track invariant and explicitly forbids stealing a voice still owned by another logical track. OFF/NOTE CUT first start D042 RELEASE and then detach the track from that slot; the release may continue as an unowned tail. NOTE acquisition reuses an already-owned non-IDLE slot, otherwise takes the lowest-index IDLE slot, otherwise steals the oldest unowned RELEASE tail with lower slot index as the age tie-break. If none of those candidates exists, allocation fails to `NO_VOICE` without mutation.

This release-tail rule is sufficient for the eight-track/eight-slot Phase 4 architecture: a valid requesting track with no owned slot cannot require another track's still-owned primary voice to be cut. It also allows a track to start a new primary voice while its older detached release tail continues when spare capacity exists.

The allocator receives only the caller-owned eight-byte D042 stage snapshot. IDLE synchronization clears stale owner mappings. Unowned ATTACK/DECAY/SUSTAIN is malformed and fails safely. TIE/EMPTY/RETRIG never allocate; delayed NOTE allocates only when it fires. Fresh/steal results require deterministic oscillator/engine reset by the later adapter; REUSE does not reset oscillator state, preserving D037 ordinary-NOTE phase semantics.

Recency uses only 32-bit integer state. The clock saturates at `UINT32_MAX` and never wraps; once saturated, the lower voice index remains the deterministic tie-break. Allocator work is event-boundary fixed eight-slot scanning only, with no heap, blocking, tracker-store traversal, recursion, float, runtime 64-bit arithmetic or unbounded search. Persistent isolated state cost is 52 B normal RAM and no pool RAM.

WP4.7 remains disconnected from `dinghy.c`, so full-app image/RAM/pool/WAV/BIN/FWSC delta must remain exactly zero versus WP4.6.

**Reason:** D037/D042 already provide eight track identities, eight voice slots and click-safe release behavior. Detaching release tails gives the allocator a musically useful bounded steal target without ever cutting a still-owned track. Saturating recency avoids wrap arithmetic while preserving deterministic oldest-tail selection for practical lifetime and a frozen tie-break after saturation.

**Consequence:** focused tests must prove exact 52 B/4 B layouts, reset/mapping invariants, all-eight deterministic first allocation, same-track reuse, IDLE synchronization, OFF/CUT detach, free-before-tail priority, oldest-tail/tie selection, no active cross-track steal, saturation without wrap, malformed-state no-mutation, guards/replay and a frozen long sequence hash. `fmt_voice_allocator.c` must compile on pinned PI32v2 and the complete full app must remain byte-identical while isolated. Parameter mapping and full engine integration remain WP4.8/WP4.9.


## D044 — WP4.8 freezes six 8-BIT semantic IDs and one four-control page per engine

Date: 2026-10-24
Status: accepted — frozen before WP4.8 production parameter-page source

**Decision:** WP4.8 implements the isolated 8-BIT parameter/page resolver exactly as frozen in `docs/WP4_8_PARAMETER_PAGE_CONTRACT.md`. D044 assigns six stable semantic IDs in D027's reserved `0x20..0x5F` namespace: ATTACK `0x20`, DECAY `0x21`, SUSTAIN `0x22`, RELEASE `0x23`, PULSE WIDTH `0x24`, WAVETABLE `0x25`. IDs `0x26..0x5F` remain unassigned. D031 VOICE LEVEL/FINE/TRACK LEVEL and TRACK PAN keep their existing IDs and separate ownership.

The normalized semantic base set is exactly 6 B with defaults A24/D0/S255/R72/W128/T0. Each current engine exposes exactly four ordered page controls: PULSE = WIDTH/DECAY/SUSTAIN/RELEASE; NOISE = ATTACK/DECAY/SUSTAIN/RELEASE; TRIANGLE = ATTACK/DECAY/SUSTAIN/RELEASE; WAVETABLE-8 = TABLE/DECAY/SUSTAIN/RELEASE. PULSE and WAVETABLE-8 retain fixed D042 ATTACK 24 while using slot 0 for their defining oscillator control; NOISE/TRIANGLE expose full ADSR because D039/D040 freeze no extra mutable oscillator parameter.

D044 page values are normalized bytes. Envelope bytes map directly to D042. PULSE WIDTH maps by `32 + ((v * 193) >> 8)`, giving exact `0->32`, `128->128`, `255->224`. WAVETABLE maps by `v >> 5`, dividing all 256 values into eight exact 32-value D041 table bands. Default packed page words are PULSE `0x48FF0080`, NOISE `0x48FF0018`, TRIANGLE `0x48FF0018`, WAVETABLE-8 `0x48FF0000`.

Row precedence is base semantic value overridden by an applicable D028 p-lock. The pure resolver sees at most four `(FmtParamId,uint16_t)` pairs, performs at most four inspections and ignores common/non-page/other-engine IDs. Applicable values `>255` retain the base slot and report domain fallback; duplicate applicable IDs or count `>4` structurally fall back to the untouched base page; unsupported engines fail to no-page rather than aliasing another engine.

D037's publication shape is frozen as one naturally aligned 32-bit four-byte page word per `(track,row)`, maximum `8*64*4 = 2048 B` ordinary RAM when integrated. A complete 16-instrument semantic base bank is exactly `16*6 = 96 B`. WP4.8 defines these types/shapes but does not allocate them globally.

WP4.8 does **not** activate the six D044 IDs in the already-linked global `FMT_PARAM_REGISTRY`, does not change the current D029/D031 common four-knob step-hold UI and does not connect the engine to `dinghy.c`. Registry/UI/cache activation is deferred to WP4.9's explicit integration/migration checkpoint, where the numeric IDs/page meanings may not change. This preserves the required WP4.8 zero-linked-delta full-app identity.

**Reason:** the frozen oscillators expose only two oscillator-specific editable controls (PULSE width and WAVETABLE table), while D042 supplies common articulation. A single four-control page per engine matches the hardware/page budget without inventing fake NOISE/TRIANGLE DSP or a second page. Keeping D044 IDs normative but unlinked lets the complete page/p-lock translation be proven before deliberate production migration.

**Consequence:** focused tests must prove exact IDs/layouts/default words, all-256 WIDTH/TABLE mappings, engine-page applicability, p-lock precedence/fallback, 6 B base set, 2048 B cache type, bounded <=4-lock work, replay/guards/hash and real PI32v2 compile. Production delta from the D044 pre-source checkpoint must be only the isolated WP4.8 module, and the complete full app must remain byte-identical to WP4.7. Any ID renumbering, page-order change, WIDTH/TABLE mapping change or second-page addition requires a superseding decision.

## D045 — WP4.9 deliberately migrates the tracker proof lane to one bounded eight-track/eight-voice 8-BIT engine

Date: 2026-10-24
Status: accepted — frozen before linked engine source

**Decision:** `docs/WP4_9_LINKED_ENGINE_CONTRACT.md` is normative. WP4.9 is the first Phase 4 package allowed to change linked tracker audio. It makes pattern 0 tracks `0..7` audible through D038-D044 while preserving D020-D035 timing/editor/UI ownership and leaving the separate D034 four-voice sine audition lane unchanged.

D045 freezes project modes 8-BIT=0 and 16-BIT=1, engine identities SAFE=0/PULSE=1/NOISE=2/TRIANGLE=3/WAVETABLE-8=4, a deterministic runtime demo seed (instrument 0..3 = the four engines; track default instrument = track & 3), NOTE-only step-instrument inheritance, and safe silence for malformed/unsupported mode/engine data.

The existing four-byte `FmtPlaybackEvent` remains unchanged. A separate four-byte multi-track row event captures step/kind/note/instrument for each fixed track index. Row and D032/D033 action dispatch order is always track 0 -> 7. D031 and D032 retain their verified track-0 cache symbols and add exactly seven extra 64x32-bit track rows each. D044 activates IDs 0x20..0x25, links a 96 B sixteen-instrument base bank and the frozen 2048 B aligned row cache. Audio consumes aligned publication words only and never traverses D028.

One `Fmt8BitOscState` is 8 B, one `Fmt8BitVoice` is exactly 44 B, and one `Fmt8BitEngine` is exactly 404 B including the 52 B D043 allocator plus eight voices. Ordinary NOTE reuses oscillator phase only when the same track retains the same engine/instrument; fresh/tail-steal or engine/instrument change resets oscillator state. NOTE/RETRIG consume D042 A/D/S snapshots, OFF/CUT consume current-row RELEASE, PULSE WIDTH and WAVETABLE TABLE are live row timbre, and TIE/EMPTY never live-retime D042 A/D/S.

D031 VOICE LEVEL x TRACK LEVEL becomes one Q15 gain via `fmt_playback_level_target(32767, ...)`. D032 remains sole owner of persistent/transient pitch and D033 remains sole owner of NOTE DELAY/RETRIG/CUT timing. Per-frame render scans exactly eight slots, applies oscillator * envelope * D031 gain with bounded signed 32-bit arithmetic, sums to a 32-bit accumulator, clamps to +/-32767 and feeds mono-centre to both channels. TRACK PAN remains storage-only.

OFF/CUT starts D042 release, then D043 detaches ownership in the same event; unowned RELEASE tails continue rendering and may later be reclaimed only under D043. Allocator scans remain event-boundary O(8), never per-sample. Planned major normal-RAM delta is <=6368 B and the linked result must remain inside D037's 21,488 B normal-RAM, 132,496 B image and 129,384 B pool ceilings.

WP4.9 uses staged gates: (1) track-aware playback/D031/D032 publication with track-0 regressions green, (2) pure linked-engine state/mix tests, (3) D044 registry/cache/page-aware p-lock editing, then (4) `dinghy.c` migration and deterministic per-engine/mixed/eight-track/D033/precedence fixtures, followed by inherited regressions and pinned full firmware measurement. The historical proof-sine WAV hash is intentionally superseded only at this explicit migration checkpoint.

**Reason:** Phase 4 primitives are individually verified but not yet a product engine. D045 crosses that boundary without mixing in live-performance polyphony or the final timing benchmark, while keeping event/cache ownership and RAM cost statically bounded.

**Consequence:** No linked engine source may precede D045. WP4.9 closes only with all four engines audible through the real tracker adapter, deterministic eight-track ownership/timing/p-lock evidence, new intentional full-app hashes and resource measurements inside D037. WP4.10 remains the representative timing benchmark/Phase 4 exit gate.
