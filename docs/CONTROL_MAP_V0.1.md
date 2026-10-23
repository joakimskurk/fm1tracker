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

Suggested screen content:

```text
FM1TRACKER        8-BIT
PAT 03  TRK 2     124 BPM
PULSE  INST 05

[compact transport / pattern status]

SEQ=PATTERN   REC=STEP REC
```

Encoders:

- SELECT: track
- ALGORITHM: engine/voice type within current sound mode
- KNOB1..4: four primary performance parameters of the current instrument
- PRESETS: instrument slot

The four primary parameters are engine-defined but stable enough to learn. Example for 8-BIT pulse: DUTY, ENV, PITCH/FINE, LEVEL.

Future live-performance capture extends this mode rather than replacing it. The accepted direction is documented in `LIVE_PERFORMANCE_CAPTURE_VISION.md`.

## 3. Pattern / SEQ mode

### White keys = physical steps

The FM-1 keyboard span F3..G5 contains 16 white keys. In pattern edit mode those 16 white keys represent the current 16-step page.

Pages:

- 01-16
- 17-32
- 33-48
- 49-64

The corresponding key LED indicates whether the step contains a note/event and a brighter/blinking state may indicate the playhead/current edit step.

### Black keys = track/control candidates

**Provisional:** the first eight black keys select tracks 1-8. The remaining black keys are reserved for fast page/mute/solo helpers after hardware usability testing.

This mapping must be validated on physical hardware before it becomes permanent.

### Pattern encoders

- SELECT: move tracker cursor / row selection
- ALGORITHM: select editable lane/column (NOTE, INST, VOL, FX, LOCK view)
- KNOB1..4: context values / p-lock parameters
- PRESETS: pattern slot

OCT- / OCT+: previous/next 16-step page while in SEQ mode and not in STEP REC.

## 4. STEP REC

Entry:

- transport stopped
- press REC

The D024/WP2.7 implementation requires the audio side to acknowledge a real stopped transport before REC can enter. REC while transport is running is ignored. PLAY while STEP REC is active exits STEP REC before playback resumes.

The screen must always show that keyboard presses will be written, not merely auditioned once the tracker screen exists.

Example direction:

```text
STEP REC     TRK 2
PAT 03       17/64

PLAY NOTE
FX=REST SEL=TIE
FX+SEL=OFF   OCT 4
```

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

Playback semantics are also explicit:

- EMPTY / REST = no new pitch/gate command;
- TIE = continue an already active note without retrigger;
- NOTE OFF = release the active note;
- special sentinel values are never interpreted as raw pitches.

The control gestures remain subject to physical-hardware ergonomics testing in Phase 7 even though their current software semantics are now deterministic.

### Audition vs write

REC off: keyboard auditions only in PERFORMANCE; ordinary SEQ uses the keys for step/track selection.

REC on: all physical note keys audition, and exactly one deterministic note is written for each fully released STEP REC note gesture. REST/TIE/OFF use the button gestures above.

This distinction must remain visually obvious.

### End-of-pattern behavior

Current V0.1 behavior: wrap to step 1 of the same pattern after step 64.

A project setting may later choose STOP AT END, but no such setting exists yet.

## 5. Parameter locks

Primary gesture:

1. select or hold a step via its white key;
2. turn KNOB1..4;
3. the changed parameter is stored as a lock on that step.

Rules:

- maximum four locks per step in V0.1;
- turning a parameter that already has a lock replaces its value;
- encoder movement must not create a lock until it crosses a small pickup/intent threshold or an explicit relative delta is observed;
- the screen immediately shows the locked parameter name/value;
- a locked step receives a distinct LED/UI marker.

Deletion gesture candidate:

- hold step + press EDIT, then select `CLEAR LOCKS`, or
- hold step + hold SEL while turning the parameter back to `--`.

Do not freeze the deletion gesture until the first host prototype.

## 6. Focused step edit

Press EDIT on a selected step to open a readable editor instead of forcing all data into a dense tracker grid.

Suggested fields:

```text
STEP 17 / TRK 2
NOTE   C#4
INST   05
VOL    92
FX     VIBRATO
VALUE  37
LOCKS  3
```

SELECT navigates fields; encoder turns change values.

This is the accessibility layer for users who do not want to interpret compact tracker notation.

## 7. Tracker grid screen

WP2.9 owns the first implementation. The 240×240 screen should show one track in detail rather than attempting a desktop multi-channel spreadsheet.

Candidate direction before the WP2.9 design freeze:

```text
P03 T2  124     17-24

16  B-3 05 78  ---
>17 C#4 05 92  V37
18  --- .. ..  ---
19  G-4 05 84  R03
20  === .. ..  ---
21  OFF .. ..  ---
22  A-4 05 90  ---
23  E-4 06 A0  ---
```

The exact note/special-event notation, visible-row count and selected-row/playhead markers must be frozen before WP2.9 source code. Readability wins over showing more rows.

## 8. Playback editing

During playback:

- white step LEDs follow playhead without destroying edit selection;
- selected edit row and playhead must remain distinct;
- selecting steps and changing future p-locks is allowed only when shared-state handoff is atomic;
- pattern/song structure edits that could invalidate currently executing data may be deferred until a safe boundary in early versions;
- flash save remains unavailable while audio is active if the platform write path causes dropouts.

## 9. Principles to test on hardware

The following must be explicitly evaluated before V1 control freeze:

- whether black-key track selection feels natural;
- whether FX-as-REST is discoverable enough;
- whether SELECT-as-TIE and FX+SELECT-as-OFF are comfortable and understandable;
- whether holding a white key while turning encoders is physically comfortable;
- whether page switching should remain on OCT buttons;
- whether live parameter locking needs latch mode so the user does not need three hands;
- whether the current lowest-note/full-release STEP REC chord behavior matches user expectation;
- whether tracker rows/special-event notation are readable on 240×240;
- later, whether seamless live PLAY/REC and CHANNEL/GLOBAL performance FX feel immediate enough for the accepted live-performance direction.