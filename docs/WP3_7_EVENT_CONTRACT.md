# WP3.7 Event Contract - D033

Status: accepted design target before WP3.7 production source  
Date: 2026-10-24  
Package: WP3.7 / issue #26

## Purpose

WP3.7 adds retrigger, note-delay and note-cut semantics on top of D031 p-lock playback and the D032 six-tick tracker-effect clock without introducing the general event queue reserved for WP3.8.

The contract is deliberately one-lane and bounded like the existing Phase 3 proof path. It keeps `FmtStep` at 8 B, `FmtPlaybackEvent` at 4 B and uses the already-frozen D020/D021/D032 timing domain.

## Command domain

WP3.7 extends the existing `FmtStep.effect/effect_value` command namespace. It does not create new D027 p-lock parameter IDs.

The exact command assignments become:

- `0x00` NONE
- `0x01` ARPEGGIO
- `0x02` SLIDE_UP
- `0x03` SLIDE_DOWN
- `0x04` PORTAMENTO
- `0x05` VIBRATO
- `0x06` RETRIG
- `0x07` NOTE_DELAY
- `0x08` NOTE_CUT
- `0x09..0xFF` reserved and resolved safely to NONE.

There is still exactly one tracker-effect/event command and one argument byte per row. WP3.7 therefore cannot combine ARPEGGIO and RETRIG, PORTAMENTO and NOTE_DELAY, or any other two tracker commands on the same row. Parameter locks remain a separate orthogonal data plane and may coexist with any row command.

## Timing domain

WP3.7 reuses D032 exactly. There is no second sub-row clock.

Each 1/16-note row contains six logical effect/event positions:

- tick 0 at row dispatch
- tick 1 at transport phase `110250`
- tick 2 at `220500`
- tick 3 at `330750`
- tick 4 at `441000`
- tick 5 at `551250`
- next row at phase `661500`.

At 120 BPM the internal tick boundaries remain frames `919, 1838, 2757, 3675, 4594`; the next row takes effect beginning at frame `5513` under D021.

The row-boundary branch keeps precedence over any sub-row action. No tick 6 exists. Values must never schedule work outside the current row.

At each nonzero D032 tick, fixed ordering is:

1. advance the D032 tracker-effect tick counter;
2. evaluate at most one D033 fixed-state row-event action;
3. apply a delayed NOTE, synthetic CUT/OFF or RETRIG action if due;
4. publish/apply resulting fixed pitch/gate state for the next rendered frame.

No queue, loop over pending events or dynamic allocation is permitted.

## Value domain

D033 uses the raw argument byte as a tick selector/interval, not as a normalized automation value.

### RETRIG `0x06`

- value `0`: no retriggers; the ordinary row event still behaves normally;
- value `1..5`: retrigger interval in D032 ticks;
- value `>5`: safe no-op for retrigger scheduling.

For interval `r`, retrigger actions occur at each internal tick `t` in `1..5` where `t % r == 0`.

Examples:

- `r=1`: ticks `1,2,3,4,5`
- `r=2`: ticks `2,4`
- `r=3`: tick `3`
- `r=4`: tick `4`
- `r=5`: tick `5`.

The row's tick-0 NOTE, if present, is the initial trigger and is not counted as an extra retrigger action.

### NOTE_DELAY `0x07`

- value `0`: ordinary immediate row-event behavior;
- value `1..5`: an ordinary NOTE on this row is withheld until that tick;
- value `>5`: safe no-op for delay scheduling; the row event is not suppressed.

Only an ordinary NOTE can be delayed. TIE, EMPTY/NONE and OFF are never held back by NOTE_DELAY and retain their normal immediate D025 semantics.

### NOTE_CUT `0x08`

- value `0..5`: synthesize OFF/cut at that tick;
- value `>5`: safe no-op for cut scheduling.

CUT can act on a note started on the same row or on an already-active sustained note from an earlier row. If no logical note is active when the cut becomes due, the cut is a no-op.

## Row-dispatch ordering

D033 row start is deterministic.

1. Read the D031 parameter snapshot and D032/D033 effect word for the row.
2. Decode D033 fixed-state scheduling.
3. If NOTE_DELAY value `1..5` owns an ordinary NOTE, store that note in fixed pending state and replace only the immediate note event with NONE for row-start application.
4. Apply the immediate D025 event to D032 persistent pitch state and D031 proof-voice state.
5. If NOTE_CUT value `0` is valid, apply a synthetic OFF after the row event and before the first rendered frame.
6. Compute row pitch/gate state.

Therefore NOTE+CUT0 may become logically active and then immediately OFF before the first frame of the row. From silence this produces no attack frame. On an existing release/sustain it uses the normal click-safe OFF tail.

## NOTE_DELAY semantics

While an ordinary NOTE is pending:

- the previously active logical note, if any, remains active until the selected tick;
- the new row's D031 VOICE LEVEL, FINE PITCH and TRACK LEVEL are row-local and may therefore modulate that previous sustain during the waiting portion of the row;
- the D032 row command is NOTE_DELAY, so no ARP/slide/porta/vibrato modulation runs on the same row;
- a remembered D032 portamento target may remain stored but does not move while the row command is NOTE_DELAY.

At the selected tick, the pending NOTE is applied using the same row's already-published D031 snapshot. Because NOTE_DELAY is not PORTAMENTO, the delayed NOTE follows the same persistent-pitch rule as an ordinary non-portamento NOTE: persistent pitch jumps to `note * 128`, pending portamento target is cleared, logical note-active becomes true and the row level target is applied.

If no note was active before the delayed NOTE, the proof lane remains silent until the delay tick.

A pending delayed NOTE cannot survive the row boundary. The fixed state is replaced by the next row before any hypothetical tick 6.

## NOTE_CUT semantics

A valid NOTE_CUT action uses full D025 OFF semantics without mutating the stored row:

- logical note-active becomes false;
- proof gate target becomes zero;
- D032 persistent pitch validity for the active proof note is cleared;
- pending portamento target is cleared;
- transient ARP/vibrato modulation is cleared;
- later TIE/EMPTY or event ticks cannot resurrect the cut note;
- a later real NOTE on a subsequent row may start normally.

The click-safe release tail is unchanged. CUT does not rewrite `FmtStep.note` to OFF.

## RETRIG semantics

RETRIG is a semantic retrigger action on the currently logical-active note.

For the present sine proof adapter, each due RETRIG:

- resets the proof oscillator phase to zero;
- preserves persistent Q7 pitch and the current D031 FINE offset;
- preserves any remembered portamento target;
- preserves current row VOICE/TRACK level target;
- does not create a new stored NOTE;
- does not reset the proof amplitude accumulator.

This proof behavior avoids freezing a synthetic envelope law before Phase 4/5 engines exist. Future engine adapters may map the same RETRIG action to envelope/voice retrigger semantics, but D033's timing and event identity remain stable.

RETRIG on TIE or EMPTY may retrigger an already-active sustained note. RETRIG after OFF or CUT is a no-op because no logical note is active.

## D032 interaction

Because `FmtStep` has one effect command byte, a D033 event command and a D032 pitch-effect command cannot both be active on the same row.

Entering RETRIG, NOTE_DELAY or NOTE_CUT therefore stops row-local ARPEGGIO/VIBRATO transient modulation for that row and performs no slide/porta movement on that row.

Persistent D032 pitch and remembered portamento target follow existing D032 continuation rules until a D033 semantic action changes them:

- RETRIG never changes persistent pitch or porta target;
- NOTE_DELAY preserves prior persistent state while waiting, then its delayed non-porta NOTE sets new persistent pitch and clears porta target;
- NOTE_CUT clears active pitch/porta state when the cut fires.

A prior portamento target may therefore survive a RETRIG/TIE or RETRIG/EMPTY row and resume on a later PORTAMENTO row, consistent with D032's existing non-porta TIE/EMPTY continuation rule.

## D031 interaction

D031 p-lock data remains independent and may coexist with all D033 commands.

VOICE LEVEL and TRACK LEVEL continue to determine the row gate target. FINE remains row-local and additive to D032 persistent pitch. RETRIG preserves those current row values. A delayed NOTE starts with the current row's D031 values. NOTE_CUT overrides the audible gate by applying OFF, even if the row level locks are nonzero.

A level-zero p-lock is still not semantic OFF. It may be retriggered while logically active, and a later unlocked row may restore it unless a real OFF or NOTE_CUT has occurred.

## Fixed state instead of queue

WP3.7 explicitly does **not** introduce the WP3.8 general event queue.

The entire row scheduler can be represented by fixed state no larger than 4 bytes:

- current D033 command;
- current argument value;
- one pending delayed-note byte;
- one pending-valid byte.

There is at most one D033 command per row and at most one logical action at each six-tick position. RETRIG derives repeated actions mathematically from the current tick and interval; it does not enqueue five records.

The D032 effect cache is reused. No third 64-row cache is required for WP3.7.

Audio work stays constant and bounded:

- no allocator;
- no list traversal;
- no event search loop;
- no overflow path;
- no screen or flash work;
- no float;
- no runtime 64-bit division.

WP3.8 remains responsible for the general bounded queue and overflow policy needed by later multi-event/live-capture architecture.

## Invalid/fallback behavior

Reserved command bytes `0x09..0xFF` continue to resolve to NONE through the tracker-effect row cache and must remain byte-identical to control audio.

For valid D033 commands with value `>5`, the row's ordinary D025 note event still executes normally and the D033 scheduler performs no delayed/retrig/cut action. No invalid value may wrap modulo six or schedule an action in another row.

Playback never mutates `FmtStep`, p-lock records or stored effect bytes.

## Regression obligations

WP3.7 must preserve:

- `FmtStep == 8 B`;
- `FmtPlaybackEvent == 4 B`;
- D020/D021 row-boundary ordering;
- D031 default and locked audio behavior;
- all D032 ARP/slide/porta/vibrato/tick-grid behavior;
- historical default WAV SHA-256 `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`;
- D031 locked WAV SHA-256 `8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8`;
- all prior Phase 2 and Phase 3 editor/storage tests.

New deterministic tests must cover RETRIG intervals 0..5 and >5 fallback, NOTE_DELAY 0..5 and >5 fallback, NOTE_CUT 0..5 and >5 fallback, NOTE/TIE/EMPTY/OFF interaction, delayed-note use of current D031 row values, cut non-resurrection, retrig-after-OFF no-op, D032 persistent-pitch/porta continuation rules, exact frame/tick application and stored-data immutability.

## Non-goals

WP3.7 does not implement:

- the general WP3.8 event queue or overflow policy;
- simultaneous multiple tracker commands on one row;
- full effect-entry UI or row visualization;
- engine-specific envelope design;
- persistence;
- live performance capture;
- final physical control-map approval.
