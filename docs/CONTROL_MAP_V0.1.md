# FM1Tracker V0.1 Control Map

Status: **provisional hardware map**

The interaction model is intentionally built around the FM-1 rather than around a desktop tracker keyboard.

The panel labels remain useful where possible. Context-sensitive repurposing is allowed when the screen clearly states it.

## 1. Global controls

| Control | Default role |
| --- | --- |
| PLAY | play / stop |
| REC | enter/leave STEP REC while stopped; later live-record modifier |
| SEQ | pattern/tracker page |
| HOME | performance/overview page |
| EDIT | focused step/instrument edit |
| SAVE | explicit save flow; only writes when safe |
| GLO | project/global settings |
| ENV | envelope page for current instrument |
| LFO | modulation page |
| FX | effects page; becomes REST in STEP REC |
| ARP | arpeggio page / tracker arpeggio helper |
| SEL | select/focus modifier |
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

This mapping must be validated on the host panel emulator and then on physical hardware before it becomes permanent.

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

The screen must always show that keyboard presses will be written, not merely auditioned.

Example:

```text
STEP REC     TRK 2
PAT 03       17/64

PLAY NOTE OR CHORD
FX = REST

INST 05      OCT 4
```

### Note entry

1. press one or more note keys;
2. current step receives the note/chord representation supported by the current engine;
3. held keys may be auditioned normally;
4. when the entry gesture completes, cursor advances by one step.

For monophonic engines, chord behavior must be deterministic. V0.1 may use the last/lowest note according to an explicit project setting; it must not silently produce random voice choices.

### REST

`FX` is the dedicated REST button while STEP REC is active.

- tap FX: clear/leave the current step empty and advance one step
- hold FX + SELECT: write explicit NOTE OFF and advance

This mapping is provisional but the existence of a one-button REST action is not provisional.

### Audition vs write

REC off: keyboard auditions only.

REC on: keyboard writes to the current step and auditions.

This distinction must remain visually obvious.

### End-of-pattern behavior

Default V0.1: wrap to step 1 of the same pattern after step 64.

A project setting may later choose STOP AT END.

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

The 240 x 240 screen should show one track in detail rather than attempting a desktop multi-channel spreadsheet.

Candidate layout:

```text
P03 T2  124     17-24

16  B-3 05 78  ---
>17 C#4 05 92  V37
18  --- .. ..  ---
19  G-4 05 84  R03
20  --- .. ..  ---
21  A-4 05 90  ---
22  --- .. ..  ---
23  E-4 06 A0  ---

LOCK: DUTY 42  ENV 81
```

The exact number of visible rows depends on font tests. Readability wins over showing more rows.

## 8. Playback editing

During playback:

- white step LEDs follow playhead without destroying edit selection;
- selecting steps and changing p-locks is allowed if the shared-state handoff is atomic;
- pattern/song structure edits that could invalidate currently executing data may be deferred until a safe boundary in early versions;
- flash save remains unavailable while audio is active if the platform write path causes dropouts.

## 9. Principles to test on hardware

The following must be explicitly evaluated before V0.1 control freeze:

- whether black-key track selection feels natural;
- whether FX-as-REST is discoverable enough;
- whether holding a white key while turning encoders is physically comfortable;
- whether page switching should use OCT buttons or SELECT/PRESETS;
- whether live parameter locking needs latch mode so the user does not need three hands;
- whether chord step entry should advance on first key release or after all held notes are released.
