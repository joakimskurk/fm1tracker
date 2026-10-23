# FM1Tracker V0.1 Control Map

Status: **provisional hardware map**

The interaction model is intentionally built around the FM-1 rather than around a desktop tracker keyboard.

The panel labels remain useful where possible. Context-sensitive repurposing is allowed when the screen clearly states it.

## 1. Global controls

| Control | Default role |
| --- | --- |
| PLAY | play / stop |
| REC | enter/leave STEP REC while stopped; future live-record arm during playback |
| SEQ | pattern/tracker page |
| HOME | performance/overview page |
| EDIT | focused step/instrument edit |
| SAVE | explicit save flow; only writes when safe |
| GLO | project/global settings |
| ENV | envelope page for current instrument |
| LFO | modulation page |
| FX | effects page; becomes REST/modifier in STEP REC |
| ARP | arpeggio page / tracker arpeggio helper |
| SEL | select/focus; becomes TIE / OFF modifier target in STEP REC |
| OCT- / OCT+ | keyboard octave while playing/recording; page navigation where screen context explicitly takes over |

Encoders:

| Encoder | Default tracker role |
| --- | --- |
| SELECT | cursor/value navigation |
| ALGORITHM | track / engine / effect-type context selector |
| KNOB1..4 | current page parameters; become p-lock writers while a step is held/selected |
| PRESETS | instrument/pattern selection depending on page |

MASTER remains master output level through the platform.

## 2. HOME / performance mode

Keyboard: plays the current instrument normally.

Current Phase 2 HOME screen remains the inherited/simple performance surface. Future engine work may enrich it, but WP2.9 explicitly preserves HOME ownership and proves that returning from SEQ restores it without tracker pixels.

Future live-performance capture extends this mode rather than replacing it. The accepted direction is documented in `LIVE_PERFORMANCE_CAPTURE_VISION.md`.

## 3. Pattern / SEQ mode

### White keys = physical steps

The FM-1 keyboard span F3..G5 contains 16 white keys. In pattern edit mode those 16 white keys represent the current 16-step page.

Pages:

- 01-16
- 17-32
- 33-48
- 49-64

D022 remains the current prototype mapping. OCT- / OCT+ select the previous/next 16-step page while in ordinary SEQ mode.

### Black keys = track/control candidates

**Provisional:** the first eight black keys select tracks 1-8. The remaining black keys are reserved for fast page/mute/solo helpers after hardware usability testing.

This mapping is deterministic in software but must still be validated on physical hardware before it becomes permanent.

### Current WP2.9 tracker screen

D026/WP2.9 implements the first real tracker surface rather than the earlier candidate mockup.

The 240×240 display shows **one selected logical track** and exactly **eight tracker rows** at a time. The edit cursor owns the viewport: each 16-step page is split into two fixed halves.

Possible visible windows are:

- 01-08
- 09-16
- 17-24
- 25-32
- 33-40
- 41-48
- 49-56
- 57-64

Playback does not scroll the edit viewport. If the playhead is outside the visible eight rows, its exact position remains visible as `PLYnn` in the header.

The header exposes:

- ordinary `SEQ` or `STEP REC` mode;
- `CURnn` edit position;
- `PLYnn` while running or `STOP` while stopped;
- current BPM;
- `P01` for the current Phase 2 pattern-0 editor/proof path;
- selected track `T1..T8`;
- current page `PG1/4..PG4/4`;
- visible global row range.

Cursor and playhead are deliberately independent:

- `>` = selected edit row;
- `*` = running playhead row;
- `>*` = both on the same row.

Current row columns are:

```text
ST NOTE IN VOL FX
```

Current note/special-event notation:

- ordinary notes: tracker pitch such as `C-4`, `C#4`;
- EMPTY / REST: `---`;
- TIE: `===`;
- NOTE OFF: `OFF`;
- malformed/reserved note: `???`.

Current placeholder rules while later editors are not implemented:

- inherited instrument: `..`;
- explicit instrument: UI `01..16`;
- inherited volume: `...`;
- explicit volume: `000..254`;
- effect NONE: `---`;
- any non-zero effect: `???` until Phase 3 freezes stable effect IDs/meaning.

Ordinary SEQ footer:

```text
KEYS STEP/TRK  OCT PAGE
```

STEP REC footer:

```text
FX RST  SEL TIE  FX+SEL OFF
```

The tracker surface uses ten bounded dirty regions and renders at most two regions per main-loop poll. Drawing remains outside the audio ISR. Exact layout/formatting contract and verification are recorded in `WP2_9_SCREEN_CONTRACT.md` and `WP2_9_VERIFICATION.md`.

The software layout is now deterministic, but physical readability, update feel and marker ergonomics remain provisional until Phase 7 hardware testing.

## 4. STEP REC

Entry:

- transport stopped
- press REC

The D024/WP2.7 implementation requires the audio side to acknowledge a real stopped transport before REC can enter. REC while transport is running is ignored. PLAY while STEP REC is active exits STEP REC before playback resumes.

The D026 tracker screen makes STEP REC explicit in the header and shows the current edit row plus D025 special-event notation directly.

### Note entry

The current D024 monophonic prototype is explicit:

1. all 27 physical note keys become pitch keys while STEP REC is active;
2. the first unlatched note gesture writes one note to the selected track/step and auditions the pressed notes;
3. if multiple new keys arrive in the same scan, the lowest physical key / lowest pitch is the single note stored;
4. the write gesture remains latched until **all** physical note keys are released, so chord-like or staggered presses cannot advance twice;
5. after a successful write the cursor advances exactly one step, with step 64 wrapping to step 1 of the same pattern;
6. only the step's note field is changed; instrument/volume/effect/locks are preserved.

OCT- / OCT+ change keyboard pitch octave during STEP REC rather than changing the 16-step page. Stored pitch is the same pitch used for audition.

This lowest-note/full-release behavior is deterministic but the physical chord feel remains provisional until hardware UX testing.

### REST / TIE / NOTE OFF

D025/WP2.8 freezes the current prototype behavior:

- **REST:** press and release `FX` without using the modifier action. REST writes `EMPTY` and advances exactly once.
- **TIE:** press `SELECT` while `FX` is not pending/held. TIE advances exactly once.
- **NOTE OFF:** hold `FX`, then press `SELECT`. OFF is written on the SELECT press and advances exactly once.

Important modifier rule: REST is deliberately committed on **FX release**, not FX press. This allows `FX` to become a modifier. When FX+SELECT writes NOTE OFF, the pending REST is consumed, so releasing FX afterward cannot create a second edit.

REC exit, HOME and PLAY exit cancel any pending FX REST without writing it.

All three special edits change only the note field and preserve the remaining step data.

Playback semantics are explicit:

- EMPTY / REST = no new pitch/gate command;
- TIE = continue an already active note without retrigger;
- NOTE OFF = release the active note;
- special sentinel values are never interpreted as raw pitches.

The control gestures remain subject to physical-hardware ergonomics testing in Phase 7 even though their current software semantics are deterministic.

### Audition vs write

REC off: keyboard auditions only in PERFORMANCE; ordinary SEQ uses the keys for step/track selection.

REC on: all physical note keys audition, and exactly one deterministic note is written for each fully released STEP REC note gesture. REST/TIE/OFF use the button gestures above.

This distinction is now also explicit on the WP2.9 tracker display.

### End-of-pattern behavior

Current V0.1 behavior: wrap to step 1 of the same pattern after step 64.

A project setting may later choose STOP AT END, but no such setting exists yet.

## 5. Parameter locks

Primary future gesture:

1. select or hold a step via its white key;
2. turn KNOB1..4;
3. the changed parameter is stored as a lock on that step.

Rules planned for Phase 3:

- maximum four locks per step in V0.1;
- turning a parameter that already has a lock replaces its value;
- encoder movement must not create a lock until it crosses a small pickup/intent threshold or an explicit relative delta is observed;
- the screen immediately shows the locked parameter name/value;
- a locked step receives a distinct LED/UI marker.

Deletion gesture remains unfrozen until the first Phase 3 prototype.

## 6. Focused step edit

A readable focused editor remains planned as an accessibility layer for users who do not want to interpret compact tracker notation. It is not part of WP2.9 and its exact control behavior is not yet frozen.

## 7. Playback editing

During playback:

- D026 keeps playhead and selected edit row visually separate;
- the playhead does not steal/scroll the cursor-owned edit viewport;
- selecting steps and changing future p-locks is allowed only when shared-state handoff is atomic;
- pattern/song structure edits that could invalidate currently executing data may be deferred until a safe boundary in early versions;
- flash save remains unavailable while audio is active if the platform write path causes dropouts.

## 8. Principles to test on hardware

The following must be explicitly evaluated before V1 control freeze:

- whether black-key track selection feels natural;
- whether FX-as-REST is discoverable enough;
- whether SELECT-as-TIE and FX+SELECT-as-OFF are comfortable and understandable;
- whether holding a white key while turning encoders is physically comfortable;
- whether page switching should remain on OCT buttons;
- whether live parameter locking needs latch mode so the user does not need three hands;
- whether the current lowest-note/full-release STEP REC chord behavior matches user expectation;
- whether D026 tracker rows, cursor/playhead markers and special-event notation are readable on the physical 240×240 display;
- whether the max-two-region update policy feels visually smooth on hardware;
- later, whether seamless live PLAY/REC and CHANNEL/GLOBAL performance FX feel immediate enough for the accepted live-performance direction.