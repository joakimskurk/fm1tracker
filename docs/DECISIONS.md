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
