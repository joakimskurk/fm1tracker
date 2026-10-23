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
