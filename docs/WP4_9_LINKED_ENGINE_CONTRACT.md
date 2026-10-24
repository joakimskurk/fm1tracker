# WP4.9 Linked 8-BIT Engine Integration Contract

Date: 2026-10-24  
Package: WP4.9 / issue #40  
Decision: D045  
Branch: `phase4/8bit-engine`

This document is normative for WP4.9. It is the deliberate Phase 4 migration contract that permits the previously isolated D038-D044 engine stack to enter the linked FM1Tracker audio path.

No linked engine migration source may precede this contract.

## 1. Scope and migration statement

WP4.9 deliberately replaces the Phase 3 **tracker proof-sine lane** with the bounded Phase 4 8-BIT engine stack.

WP4.9 does all of the following:

- makes pattern 0 tracks `0..7` audible through one shared eight-slot Phase 4 engine;
- preserves the existing D020/D021 exact transport/frame ordering;
- preserves the existing four-byte Phase 3 `FmtPlaybackEvent` API for legacy tests and adds a separate four-byte multi-track row event instead of enlarging it;
- activates the six D044 parameter IDs in the linked D027 registry;
- publishes D031 common-parameter, D032 effect and D044 engine-page snapshots for all eight logical tracks;
- links D038 PULSE, D039 NOISE, D040 TRIANGLE, D041 WAVETABLE-8, D042 envelope and D043 allocation into the tracker lane;
- gives every logical track one primary sequenced voice at most;
- preserves unowned D042 RELEASE tails exactly as D043 requires;
- intentionally supersedes the historical proof-sine tracker WAV/hash with new deterministic 8-BIT evidence.

WP4.9 does **not** replace the separate D034 manual/audition lane. The existing four sine audition voices used by HOME/keyboard/MIDI remain isolated and behaviorally unchanged in this package. Live-performance/polyphonic capture is a later milestone and must not be conflated with sequenced Phase 4 ownership.

WP4.10 remains the representative eight-track timing/benchmark and Phase 4 closeout package. WP4.9 proves correctness and linked resource bounds first.

## 2. Project mode and engine identities

D045 freezes the first numeric project-mode identities:

```text
FMT_MODE_8BIT  = 0
FMT_MODE_16BIT = 1
```

The zero value is intentionally 8-BIT so the existing zero-reset `FmtSongMeta.mode` and `FmtInstrumentHeader.mode` remain a safe 8-BIT default rather than becoming an accidental foreign mode.

D044 engine identities remain:

```text
0 SAFE/NONE
1 PULSE
2 NOISE
3 TRIANGLE
4 WAVETABLE-8
```

For WP4.9 tracker audio:

- `song.meta.mode` must equal `FMT_MODE_8BIT`;
- a selected instrument header must have `mode == FMT_MODE_8BIT`;
- engine must be `1..4`;
- unsupported mode/engine fails toward silence for that NOTE and never aliases another engine;
- instrument header `version` and `flags` are carried but receive no new runtime meaning in WP4.9.

## 3. Deterministic integrated demo seed

There is still no final instrument browser/persistence UI. To make all four engines directly testable in the emulator while preserving deterministic startup, `app_main()` seeds only the current runtime song after `fmt_store_reset()`:

```text
song mode = 8-BIT
instrument 0 = PULSE
instrument 1 = NOISE
instrument 2 = TRIANGLE
instrument 3 = WAVETABLE-8
instrument 4..15 = SAFE/NONE
track default instrument = track_index & 3
```

All sixteen D044 `Fmt8BitParamSet` runtime bases initialize to the frozen D044 defaults:

```text
A24 / D0 / S255 / R72 / WIDTH128 / TABLE0
```

The historical proof melody still seeds notes only on pattern 0 / track 0, so automatic startup playback becomes a deterministic PULSE fixture. Tracks 1..7 stay empty until edited/recorded, but their default instruments make NOISE/TRIANGLE/WAVETABLE-8 immediately available through ordinary track selection + STEP REC.

This seed is a runtime prototype default, not Phase 6 persistence.

## 4. Multi-track playback event without breaking the Phase 3 ABI

The existing D021/D025 event remains exactly four bytes and unchanged:

```c
FmtPlaybackEvent
```

D045 adds one separate multi-track row event, also exactly four bytes:

```c
typedef struct {
    uint8_t step;
    uint8_t kind;
    uint8_t note;
    uint8_t instrument;
} FmtTrackPlaybackEvent;
```

Track identity is the fixed array index `0..7`; it is not duplicated in the event.

New bounded playback helpers emit exactly eight events in ascending track order at a row boundary:

```text
fmt_playback_start_tracks(... events[8])
fmt_playback_end_frame_tracks(... events[8])
```

The existing one-track start/end helpers remain intact for inherited regression tests.

The new helpers read only the current row's one-byte `note` and `instrument` fields from the fixed song store. STEP REC mutates note data only while transport is stopped, so WP4.9 does not introduce concurrent main/audio writes to those fields.

No event object grows beyond four bytes and no queue is inserted into D020-D033 tracker timing.

## 5. Instrument inheritance semantics

Each track owns one audio-runtime `current_instrument` byte.

Reset source:

```text
current_instrument = song.tracks[track].instrument
```

A NOTE row resolves its instrument as:

1. explicit `0..15` -> use it and make it the track's new `current_instrument`;
2. `FMT_INSTRUMENT_INHERIT (0xFF)` -> retain `current_instrument`;
3. any other byte -> malformed NOTE, fail toward silence and do not mutate `current_instrument`.

D045 intentionally applies step-level instrument selection on **NOTE rows only**. TIE, EMPTY/REST and OFF do not switch the current instrument in WP4.9. This keeps D037's TIE/no-restart rule coherent and keeps the four-byte row-parameter publication deterministic. A later package may supersede this only with explicit tracker semantics and new evidence.

NOTE DELAY captures the NOTE row's resolved instrument at row dispatch and uses that exact instrument when the delayed NOTE fires. A main-thread change cannot alter the pending delayed NOTE meaning.

A valid instrument index whose header mode/engine is unsupported becomes a safe-silent NOTE. It may remain the track's `current_instrument`; subsequent inherited NOTEs remain silent until a later valid explicit instrument is selected.

## 6. Track-aware D031 and D032 publication

The original track-0 cache symbols remain intact for Phase 3 regression compatibility.

D045 adds exactly seven extra track rows behind new track-aware APIs for each cache family:

### D031 common row words

Existing track 0:

```text
64 * 4 = 256 B
```

Extra tracks 1..7:

```text
7 * 64 * 4 = 1792 B
```

New APIs provide bounded `(track,row)` refresh/read while old APIs remain track-0 wrappers.

### D032 effect row words

Existing track 0:

```text
64 * 4 = 256 B
```

Extra tracks 1..7:

```text
7 * 64 * 4 = 1792 B
```

The same compatibility rule applies: old APIs remain track-0 wrappers; new APIs address tracks `0..7`.

All rebuild/refresh traversal is main/editor work. Audio reads naturally aligned 32-bit snapshots only.

## 7. D044 linked parameter state and cache

WP4.9 activates the six D044 IDs in `FMT_PARAM_REGISTRY` with their frozen meanings and lockable status:

```text
0x20 ATTACK
0x21 DECAY
0x22 SUSTAIN
0x23 RELEASE
0x24 WIDTH
0x25 TABLE
```

No other `0x26..0x5F` ID becomes assigned.

WP4.9 links exactly:

```text
16 * Fmt8BitParamSet = 96 B runtime instrument base bank
Fmt8BitRowParamCache = 2048 B aligned row publication cache
```

There is no pool allocation.

The base bank has no live editing UI in WP4.9. It is initialized before `tracker_ready` and may be mutated only by deterministic tests/startup or while transport is stopped. A later live base editor requires its own publication/ownership decision.

### Row-cache rebuild

Main/editor resolves effective instrument state sequentially from row 0 for each track using the NOTE-only inheritance rule in Section 5.

For each `(track,row)`:

- determine the engine that an active/new voice on that row belongs to;
- collect at most the already-bounded four D028 lock snapshots;
- resolve the D044 four-slot page from that instrument's 6 B base set;
- publish one aligned 32-bit word;
- publish zero for unsupported/malformed engine state.

A single-row refresh may bounded-scan at most 64 rows on that track to reconstruct instrument inheritance. This is main-thread work only.

Audio never traverses D028.

## 8. D044 primary p-lock editing becomes active

The held-step D029 editor keeps its existing gesture/delete/browse grammar.

The legacy `fmt_lock_editor_apply_delta()` common-parameter helper remains available for Phase 3 tests, but the linked WP4.9 8-BIT app maps held-step KNOB1..4 writes through D044's active engine page:

```text
PULSE      WIDTH   DECAY SUSTAIN RELEASE
NOISE      ATTACK  DECAY SUSTAIN RELEASE
TRIANGLE   ATTACK  DECAY SUSTAIN RELEASE
WAVETABLE  TABLE   DECAY SUSTAIN RELEASE
```

The reference value for a newly created lock is the selected instrument base value for that semantic ID.

D045 may add a semantic `apply_param_delta()` editor helper so app code supplies explicit ID/reference without duplicating storage mutation logic.

Browse/delete continues to operate on every valid lock, including legacy D031 common locks. New footer/debug labels must recognize ATTACK/DECAY/SUSTAIN/RELEASE/WIDTH/TABLE.

After a successful lock write/delete, main refreshes the affected D031 track-row snapshot and D044 track-row snapshot. D032 is row-effect data, not p-lock storage, and is refreshed only when effect bytes change.

WP4.9 does not add a full instrument/base editor or secondary ENV page UI.

## 9. Exact linked engine state

D045 freezes one common oscillator-state union:

```c
typedef union {
    FmtPulseOsc      pulse;
    FmtNoiseOsc      noise;
    FmtTriangleOsc   triangle;
    FmtWavetable8Osc wavetable8;
} Fmt8BitOscState;
```

Required size:

```text
8 B
```

One linked voice is exactly:

```c
typedef struct {
    FmtEnvelope envelope;      /* 20 B */
    Fmt8BitOscState osc;       /*  8 B */
    uint32_t phase_inc;        /*  4 B */
    uint32_t page_word;        /*  4 B */
    uint16_t pitch_q7;         /*  2 B */
    uint16_t gain_q15;         /*  2 B */
    uint8_t engine;            /*  1 B */
    uint8_t instrument;        /*  1 B */
    uint8_t reserved[2];       /*  2 B */
} Fmt8BitVoice;
```

Required size:

```text
44 B
```

The shared linked engine is exactly:

```c
typedef struct {
    FmtVoiceAllocator allocator; /* 52 B */
    Fmt8BitVoice voice[8];       /* 352 B */
} Fmt8BitEngine;
```

Required size:

```text
404 B
```

No voice duplicates allocator ownership fields and no voice stores pointers into song/lock/editor state.

## 10. Voice lifecycle and oscillator reset rules

D043 acquisition remains authoritative.

### Fresh IDLE / stolen RELEASE tail

`reset_required = 1`:

- reset the selected oscillator/engine state to deterministic defaults for the new engine;
- reset its envelope state;
- replace engine/instrument identity;
- then perform D042 NOTE articulation.

### Same-track REUSE with same engine/instrument

Ordinary NOTE:

- retains oscillator phase/state;
- updates pitch/page/gain;
- hard-restarts D042 envelope from zero.

This is D037's NOTE-vs-RETRIG distinction.

### Same-track REUSE with a different valid engine/instrument

An engine/instrument identity change cannot reinterpret stale union state. Therefore:

- hard-reset oscillator/engine state to the new engine's deterministic initial state;
- replace engine/instrument identity;
- hard-restart envelope.

This reset is required even though D043 reports same-track REUSE.

### Unsupported/malformed NOTE

If instrument/mode/engine validation fails:

- any currently owned primary voice for that track is forced to zero/IDLE and detached safely from allocator ownership;
- no new voice is allocated;
- no stale prior engine continues under the malformed NOTE.

Malformed data therefore fails toward silence for the affected track only.

## 11. Envelope parameter consumption

D042 forbids silent live retiming of an in-progress envelope. D045 resolves how D044 row pages interact with this.

### NOTE / delayed NOTE / RETRIG

ATTACK, DECAY and SUSTAIN are **articulation snapshots** consumed when NOTE/RETRIG actually fires.

- NOISE/TRIANGLE ATTACK comes from page slot 0;
- PULSE/WAVETABLE-8 ATTACK comes from the selected instrument's 6 B base set because ATTACK is not on those primary pages;
- DECAY comes from page slot 1;
- SUSTAIN comes from page slot 2.

D033 RETRIG uses the current row page and the voice's selected instrument base, resets oscillator state, and calls the same D042 envelope restart.

### OFF / NOTE CUT

RELEASE is consumed from the **current row's slot 3** at the exact OFF/CUT timing. It releases from the actual current envelope level, then D043 detaches the track immediately.

### TIE / EMPTY and live timbre

TIE/EMPTY never restart or retime ATTACK/DECAY/SUSTAIN.

However row-local oscillator timbre is live where a frozen engine actually has such a parameter:

- PULSE WIDTH uses current row slot 0 on every active row;
- WAVETABLE TABLE uses current row slot 0 on every active row;
- NOISE/TRIANGLE slot 0 is ATTACK and therefore has no live effect without NOTE/RETRIG.

A DECAY/SUSTAIN p-lock on a TIE/EMPTY row is stored/resolved but does not retroactively reshape an already-running envelope. It does affect a D033 RETRIG that fires on that same row.

## 12. D031 level and D032 pitch precedence

D031 common parameter identities remain unchanged.

For every active owned voice, the row's VOICE LEVEL and TRACK LEVEL bytes become one Q15 common gain:

```text
gain_q15 = fmt_playback_level_target(32767,
                                     voice_level,
                                     track_level)
```

Default `255/255` therefore yields `32767`.

This preserves D031's semantic multiplicative precedence while moving from the proof lane's `TRACKER_TARGET_AMP=6000` to the D037 nominal-Q15 engine domain.

The gain is updated at every row dispatch for an owned voice, including TIE/EMPTY. OFF/CUT snapshots the current row gain before the release tail is detached so the unowned tail keeps a deterministic fixed gain while releasing.

`FmtStep.volume` remains outside WP4.9 and is not silently interpreted as a second gain law.

Pitch precedence remains exactly D037:

```text
final_q7 = D032 persistent pitch
         + (D031 FINE - 128)
         + D032 transient pitch
         clamped 0..16383
```

The existing `fmt_tracker_effect_final_pitch_q7()` remains authoritative. `fmt_engine_pitch_inc_q7()` converts the already-resolved Q7 pitch only when pitch changes at row/tick boundaries, never per sample.

## 13. D032/D033 per-track runtime

Each logical track owns one D032 `FmtTrackerEffectState` and one D033 `FmtTrackerRowEventState`.

The existing track-0 externally verified runtime symbols remain present. Tracks 1..7 use fixed extra state arrays so inherited symbol/regression checks are not broken merely by multi-track integration.

Each track additionally holds only fixed row snapshots such as:

```text
current D031 word
current D044 page word
current instrument
captured NOTE-row instrument for delayed NOTE
```

No pointer to editable row/lock storage is retained.

At row dispatch, track order is strictly ascending `0 -> 7`.

At D032/D033 tick actions, track order is strictly ascending `0 -> 7`.

This makes simultaneous allocation outcomes deterministic.

## 14. Exact row/event ordering

The D020/D021 frame contract remains unchanged.

For each rendered frame:

1. render the current eight-voice engine state for the current frame;
2. call transport/playback end-frame logic;
3. if a row boundary crossed, publish/apply the new row for the **next** frame, tracks 0..7 ascending;
4. otherwise, if the D032 tick boundary is due, advance/apply track effects/actions for tracks 0..7 ascending.

Row dispatch per track:

1. read one D031 common word;
2. read one D032 effect word;
3. read one D044 page word;
4. capture/resolve NOTE instrument state;
5. call D033 row-event begin;
6. call D032 begin-row using D033's immediate kind/note;
7. apply current row gain/live timbre to an already owned voice when applicable;
8. execute immediate NOTE/TIE/OFF/NONE semantics;
9. apply tick-0 NOTE CUT action if D033 requests it;
10. apply resolved D032/D031 pitch to any still-owned voice.

NOTE DELAY does not allocate at row dispatch. Its captured instrument, common word and D044 page word remain fixed until the exact D033 delayed tick fires.

RETRIG never allocates. NOTE CUT follows OFF ordering.

## 15. Engine render and mix formula

Each rendered frame scans exactly eight fixed voice slots in ascending order.

A non-IDLE voice performs exactly one engine-specific render:

- PULSE: `fmt_pulse_render()` using current mapped WIDTH;
- NOISE: `fmt_noise_render()` using `fmt_noise_clock_inc(phase_inc)`;
- TRIANGLE: `fmt_triangle_render()`;
- WAVETABLE-8: `fmt_wavetable8_render()` using current mapped TABLE.

Then:

```text
source_q15 = signed oscillator sample (-32767..32767)
env_q15    = fmt_envelope_render() (0..32767)
gain_q15   = voice common D031 gain (0..32767)

x = (source_q15 * env_q15) >> 15
x = (x * gain_q15) >> 15
mix += x
```

Each multiply remains within signed 32-bit range.

After all eight slots:

```text
mix = clamp(mix, -32767, +32767)
```

The same mono-centre sample is added to both platform output channels. TRACK PAN remains storage-only.

The platform's existing final MASTER scaling/clamp remains unchanged and still protects the combined tracker + separate audition output.

## 16. Release tails and quiet semantics

OFF/CUT ordering is exactly D043:

1. update voice row gain/page snapshot;
2. call D042 gate-off with current row RELEASE;
3. snapshot all eight envelope stages;
4. detach the track from its primary voice in the same event.

A nonzero release remains an unowned RELEASE tail and continues rendering. The tail keeps its engine/instrument, phase state, page/timbre state and common gain as frozen at detach.

Allocator acquisition already reaps IDLE states before selecting a slot. No per-sample allocator scan is added.

`quiet()` treats the linked tracker engine as quiet only when all eight envelope stages are IDLE. Unowned release tails therefore still block flash settings writes until silent.

The separate D034 audition queue/voices retain their existing quiet checks.

## 17. Main-thread cache refresh ownership

Startup order before `tracker_ready = 1` is frozen as:

1. reset song/lock stores;
2. seed deterministic Phase 4 mode/instrument/track defaults;
3. seed proof pattern;
4. initialize 16 D044 parameter bases;
5. reset/rebuild track-aware D031 cache;
6. reset/rebuild track-aware D032 cache;
7. reset/rebuild D044 8x64 cache;
8. reset engine/allocator and per-track effect/event/instrument runtime;
9. reset transport/playback/editor/screen state;
10. publish `tracker_ready`.

After a successful p-lock write/delete in main:

- refresh D031 `(track,row)`;
- refresh D044 `(track,row)`;
- leave D032 unchanged because its source is the row's effect bytes.

Audio is the only writer of engine/allocator/envelope/effect runtime state. Main is the only writer of publication caches and D044 base-bank state.

## 18. Linked resource budget

WP4.8 baseline before linked integration:

```text
image       66,960 B
normal RAM  13,296 B
pool RAM    113,000 B
```

D037 absolute Phase 4 ceilings remain:

```text
image       132,496 B
normal RAM   21,488 B
pool RAM    129,384 B
```

D045 freezes the major linked normal-RAM additions before linker measurement:

```text
D031 extra tracks 1..7       1,792 B
D032 extra tracks 1..7       1,792 B
D044 row cache               2,048 B
D044 16-instrument base bank    96 B
Fmt8BitEngine                   404 B
extra D032 track states          <= 112 B
extra D033 track states           28 B
row/instrument snapshot state     <= 96 B
-----------------------------------------
planned major delta            <= 6,368 B
```

The exact linker result is authoritative. Normal RAM must remain `<= 21,488 B` and image/pool must remain inside D037 ceilings.

WP4.9 is not allowed to consume pool RAM for engine state.

## 19. Hard realtime bounds

Per callback:

- existing D034 queue consume remains max 32 events;
- each rendered tracker frame scans exactly eight engine slots;
- row boundary handles exactly eight tracks;
- effect tick handles exactly eight tracks;
- NOTE allocation performs D043's fixed O(8) scans only when needed;
- no audio path scans 64 rows, patterns or D028 lists.

Forbidden in linked audio remains:

- dynamic allocation/free;
- D028 traversal;
- screen/UI/persistence work;
- blocking/waits/retries;
- float/double;
- runtime 64-bit division;
- recursion;
- unbounded search;
- function-pointer/vtable oscillator dispatch.

The existing D031 32-bit `/255` level mapping occurs only at row/event boundaries and is not a per-sample operation. Runtime 64-bit division remains forbidden everywhere in Phase 4 DSP.

## 20. Legacy behavior that must not regress

The audio hash is intentionally superseded, but these boundaries remain inherited gates:

- D020/D021 exact transport/frame ordering;
- D025 NOTE/TIE/OFF/EMPTY semantics;
- STEP REC cursor/write behavior;
- D028 storage invariants;
- D029/D030 hold/browse/delete safety;
- D031 common p-lock precedence;
- D032 six-tick timing and pitch effects;
- D033 exact RETRIG/DELAY/CUT tick timing;
- D034 queue/overflow fail-to-silence behavior for the separate audition lane;
- D035 tracker screen redraw/layout contract;
- D036 combined corruption/overflow recovery rules.

Historical Phase 3 tests whose only expected value is the proof-sine WAV/pixel-independent audio hash must be superseded by explicit WP4.9 reference evidence rather than weakened silently.

## 21. Frozen integration fixtures

Before WP4.9 closes, tests must include at least the following exact scenario classes.

### A. Per-engine direct adapter fixtures

For each engine 1..4:

- reset;
- NOTE C4 at default page/base/full common gain;
- render deterministic fixed frame span;
- RETRIG;
- render;
- OFF with default release;
- render through IDLE;
- freeze exact sample/state hash.

### B. Mixed four-engine fixture

At the same row boundary:

```text
track 0 PULSE      C4
track 1 NOISE      E4
track 2 TRIANGLE   G4
track 3 WAVETABLE  C5
```

Then exercise TIE/EMPTY and staggered OFF rows. Allocation/voice ownership and mixed WAV/state hashes are frozen in verification.

### C. Eight-track ownership fixture

All tracks `0..7` NOTE simultaneously in ascending dispatch order, using default `track & 3` engine mapping. Required first allocation is voice `0..7` respectively. Subsequent release-tail/re-note sequence must prove D043 tail behavior without active cross-track steal.

### D. Exact D033 timing fixture

Across real integrated audio:

- NOTE DELAY 3 fires at the already-frozen tick/frame;
- RETRIG 2 resets oscillator + envelope exactly once at the frozen tick;
- NOTE CUT 3 begins release/detach at the frozen tick;
- no action is shifted by callback block size.

### E. D031/D032/D044 precedence fixture

Must prove together:

- FINE + slide/porta/vibrato produce the D037 final Q7 pitch;
- VOICE LEVEL * TRACK LEVEL produces the Q15 common gain;
- WIDTH and TABLE p-locks alter only their correct engine row slot;
- envelope DECAY/SUSTAIN p-locks affect NOTE/RETRIG snapshots but do not live-retime TIE/EMPTY;
- RELEASE p-lock on OFF/CUT row controls the tail;
- unrelated locks do not alter page bytes.

### F. Full-app migration fixture

The default host app gets a new intentional deterministic WAV hash after proof-sine migration. The verification document must record the new WAV/BIN/FWSC hashes and exact resource deltas. Old non-audio UI/transport evidence remains compared independently.

## 22. Verification sequence

WP4.9 implementation must be gated in this order:

1. track-aware D031/D032 + eight-event playback/cache support, with inherited track-0 regression green before engine migration;
2. pure linked-engine state/voice/mix unit tests against D038-D044 primitives;
3. D044 registry/cache + page-aware p-lock editor integration tests;
4. `dinghy.c` tracker-lane migration;
5. deterministic per-engine/mixed/eight-track/D033/precedence host fixtures;
6. inherited Phase 2/3 non-obsolete regression gates;
7. pinned JieLi PI32v2/full AC79 firmware build and exact image/RAM/pool measurement;
8. durable `docs/WP4_9_VERIFICATION.md` with new intentional artifact hashes.

Do not skip directly to `dinghy.c` before gates 1-3 are green.

## 23. Non-goals

WP4.9 does not add:

- 16-BIT/FM4 synthesis;
- live D044 instrument-base editing while transport runs;
- full ADSR secondary-page UI;
- audible TRACK PAN;
- `FmtStep.volume` semantics;
- mute/solo semantics;
- new NOISE modes/filter or TRIANGLE shape variants;
- new effects or sends;
- persistent instrument/project serialization;
- polyphonic-per-track sequenced ownership;
- replacement of the separate four-voice sine audition lane;
- physical FM-1 timing claims without hardware;
- the final WP4.10 representative benchmark/Phase 4 closeout.

## 24. Exit rule

WP4.9 may close only when:

- D045 is recorded in `DECISIONS.md` and project state before linked source;
- all staged gates in Section 22 are green;
- all four engines are audible through the real tracker adapter;
- all eight logical tracks can own/render one primary Phase 4 voice simultaneously in deterministic tests;
- D031/D032/D033/D044 precedence/timing is proven through the linked path;
- the six D044 IDs are assigned/lockable in the linked registry;
- the new full-app audio change is explicitly frozen with new hashes;
- linked image/RAM/pool remain inside D037 ceilings;
- no physical-hardware execution-margin claim is made without an FM-1;
- WP4.10 remains the only active Phase 4 package after handoff.
