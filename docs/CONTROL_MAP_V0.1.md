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
| EDIT | focused step/instrument edit; delete-arm modifier during a held p-lock target |
| SAVE | explicit save flow; only writes when safe |
| GLO | project/global settings |
| ENV | envelope page for current instrument |
| LFO | modulation page |
| FX | effects page; becomes REST/modifier in STEP REC |
| ARP | arpeggio page / tracker arpeggio helper |
| SEL | select/focus; becomes TIE / OFF modifier target in STEP REC and delete confirm during an armed p-lock hold |
| OCT- / OCT+ | keyboard octave while playing/recording; page navigation where screen context explicitly takes over |

Encoders:

| Encoder | Default tracker role |
| --- | --- |
| SELECT | cursor/value navigation; browses existing locks while a step is held |
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
- `P01` for the current Phase 2/3 pattern-0 editor/proof path;
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

D029/WP3.3 freezes the first real p-lock editor prototype. D030/WP3.4 adds deliberate browsing and one-lock deletion. D031/WP3.5 now applies VOICE LEVEL, FINE PITCH and TRACK LEVEL to the current pattern-0/track-0 proof playback lane; TRACK PAN remains storage-only.

### Hold-step gesture

P-lock editing is available only in ordinary SEQ/pattern mode. It is deliberately disabled in PERFORMANCE and STEP REC because STEP REC owns all 27 note keys as pitches.

1. press and **hold** one white step key;
2. that key is selected through the existing D022 mapping;
3. pattern 0, the currently selected track and the resulting global step are snapshotted as the lock target;
4. while the key remains held, turn KNOB1..4 to create/replace locks on that exact target;
5. release the owning white key to leave p-lock hold mode.

A quick white-key tap with no later encoder movement therefore behaves like ordinary step selection and creates no lock.

While the p-lock hold is active, other step keys, black track selectors and OCT page navigation cannot retarget the editor. The cursor/header remain on the snapshotted target. HOME or entering STEP REC cancels the hold without an extra write. PLAY is allowed to start/stop while the hold remains active. D031 publishes relevant pattern-0/track-0 edits through the bounded row snapshot cache while the held D029 target itself remains fixed.

If several white step keys go down in one input poll, the lowest physical white key owns the gesture. If a black track key and white step key arrive together, the hold uses the track that was selected before that poll and the simultaneous black-key change is suppressed.

### Encoder intent and stale-delta protection

The FM-1 HAL already reports signed relative detent steps. One physical encoder click is one relative step.

SELECT and KNOB1..4 are drained every main-loop poll. Encoder deltas observed while inactive are discarded. Deltas from the same poll that starts, releases or cancels a hold are also discarded.

This means encoder movement made before the step is held can never leak into the new lock target. Only movement observed on later polls while the same white key remains held can browse or write.

### Current four-parameter mapping

| Encoder | D027 parameter | Editor reference |
| --- | --- | ---: |
| KNOB1 | VOICE LEVEL (`0x01`) | 255 |
| KNOB2 | FINE PITCH (`0x02`) | 128 |
| KNOB3 | TRACK LEVEL (`0xA0`) | 255 |
| KNOB4 | TRACK PAN (`0xA1`) | 128 |

This is UI mapping only. The parameter IDs remain semantic and independent of encoder position.

### WP3.3 value domain

For the four current parameters, WP3.3 uses an editor-normalized `0..255` range stored in D028's 16-bit value field.

- one detent = `+1` / `-1`;
- multi-detent deltas are applied in one bounded operation;
- values clamp at 0 and 255;
- an existing current-ID lock above 255 is rejected as an editor-domain error rather than silently reinterpreted;
- a missing lock starts from the parameter's reference value above, then applies the first nonzero delta;
- a nonzero encoder movement creates the lock even if clamping leaves the result at the reference boundary.

These values do **not** yet define dB, cents, a pan law or engine coefficient scaling. Those semantics belong to later playback/engine packages.

If several KNOB1..4 deltas are available in one poll, they are handled in fixed KNOB1 -> KNOB4 order as separate bounded storage edits.

### Existing-lock browse

D030 uses the SELECT encoder only while a white-step p-lock hold is active.

- the existing locks on the snapshotted step are visited in D028's strict ascending parameter-ID order;
- a hold begins with the first existing lock selected when one exists;
- SELECT movement is clamped at the first/last lock rather than wrapping;
- a successful KNOB1..4 set/replace makes that parameter the selected lock;
- if no locks exist, browse is a safe no-op and the footer says so;
- browse never changes the held step, selected track or lock values.

Because a step can hold at most four locks, browse work is always bounded.

### Deliberate one-lock deletion

Deletion requires a stable held-step target and an existing selected lock.

1. press `EDIT` to arm deletion of the currently selected semantic parameter;
2. the footer names the armed target;
3. on a **later input poll**, press `SEL` to confirm;
4. the selected lock is removed once and the arm is consumed.

Important safeguards:

- `EDIT` alone never deletes;
- `EDIT+SEL` arriving together in one poll never deletes;
- releasing EDIT before SEL cancels;
- owner-key release, HOME, successful STEP REC entry or invalid/corrupt target data cancels;
- PLAY may still toggle transport, but cancels a pending delete arm first;
- unrelated note/OCT focus intent cancels a pending delete arm even though D029 continues to suppress retargeting;
- while delete is armed, SELECT and KNOB1..4 deltas are drained but discarded, so the confirmation target cannot move;
- one successful delete cannot cascade into another without a fresh EDIT arm.

After deletion, the white-step hold remains active. Selection moves to the lock now occupying the deleted ordinal, otherwise clamps to the new last lock, or becomes NONE when the step has no locks left.

D028 storage is revalidated before deletion. NOT FOUND, BAD ADDRESS or CORRUPT produce feedback and no partial mutation.

### No clear-all shortcut in WP3.4

WP3.4 intentionally exposes **no clear-all panel gesture**. D028 has an internal verified `fmt_lock_clear()` primitive, but a step can hold at most four locks, so deliberate one-by-one deletion is fast and considerably safer while physical ergonomics are still unknown. A later focused editor or Phase 7 decision may add a protected clear-all action.

### Feedback

Current p-lock feedback stays in the existing tracker footer only:

- hold context identifies target step/track;
- an existing selected lock can show ordinal/count, parameter and value;
- a successful encoder action shows the parameter and stored normalized value;
- an armed delete names the semantic target and SEL confirmation;
- zero-lock, successful delete and storage failures are explicit;
- stored values above the current D029 `0..255` editor domain may be browsed as RANGE and are still removable because delete does not reinterpret the value;
- releasing/cancelling the key restores the ordinary SEQ footer.

Full row lock markers and detailed lock visualization remain WP3.9.

### Playback rule after WP3.5

P-lock editing still mutates only D028 lock storage. It does not rewrite note, instrument, row volume, tracker-effect fields or persistent base instrument/track values.

D031 resolves the current pattern-0/track-0 proof row outside the audio ISR into one fixed 32-bit snapshot. The current audible parameters and row defaults are:

- VOICE LEVEL = 255;
- FINE PITCH = 128;
- TRACK LEVEL = 255.

TRACK PAN remains storage-only. These playback values are row-local overrides: an unlocked next row restores the explicit defaults. Audio never traverses the mutable D028 linked list directly, and `FmtPlaybackEvent` remains 4 B.

The historical no-lock proof remains byte-identical. The current level law and Q7 fine-pitch mapping are bounded proof semantics; full engine/mixer parameter laws remain later engine work.

Physical comfort of holding a white key while browsing/arming/confirming deletion remains provisional until Phase 7 hardware testing.

## 6. Focused step edit

A readable focused editor remains planned as an accessibility layer for users who do not want to interpret compact tracker notation. It is not part of WP2.9 and its exact control behavior is not yet frozen.

## 7. Playback editing

During playback:

- D026 keeps playhead and selected edit row visually separate;
- the playhead does not steal/scroll the cursor-owned edit viewport;
- D029/D030 allow p-lock edit/browse/delete while playback continues;
- D031 publishes relevant pattern-0/track-0 edits as complete aligned 32-bit row snapshots; audio consumes them only at row dispatch and never traverses D028 lock lists;
- pattern/song structure edits that could invalidate currently executing data may be deferred until a safe boundary in early versions;
- flash save remains unavailable while audio is active if the platform write path causes dropouts.

## 8. Principles to test on hardware

The following must be explicitly evaluated before V1 control freeze:

- whether black-key track selection feels natural;
- whether FX-as-REST is discoverable enough;
- whether SELECT-as-TIE and FX+SELECT-as-OFF are comfortable and understandable;
- whether holding a white key while turning encoders is physically comfortable;
- whether SELECT-browse plus EDIT -> SEL delete confirmation is comfortable and discoverable;
- whether the D029 focus latch feels natural or a later latch/toggle mode is needed;
- whether page switching should remain on OCT buttons;
- whether live parameter locking needs latch mode so the user does not need three hands;
- whether the current lowest-note/full-release STEP REC chord behavior matches user expectation;
- whether D026 tracker rows, cursor/playhead markers and special-event notation are readable on the physical 240×240 display;
- whether the max-two-region update policy feels visually smooth on hardware;
- later, whether seamless live PLAY/REC and CHANNEL/GLOBAL performance FX feel immediate enough for the accepted live-performance direction.
