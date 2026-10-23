# WP2.9 Screen Contract — D026

Date: 2026-10-23  
Status: **ACCEPTED FOR WP2.9 PROTOTYPE — physical readability remains provisional**

This document freezes the display/readability contract for WP2.9 before source implementation. It is the detailed companion to D026 and must not be used to pull later editor, p-lock, engine or live-capture work into Phase 2.

## D026 — compact tracker screen is cursor-owned, one-track and bounded

### Context ownership

- The tracker screen is active only in the existing D022 pattern context: ordinary `SEQ` and D024 `STEP REC`.
- `HOME` / PERFORMANCE keeps the existing performance screen. Returning from tracker context must force a clean HOME redraw so no tracker pixels remain.
- The screen is presentation-only. It may read the existing pattern/input/transport/editor state but must never mutate `FmtSong`, transport or editor state.
- Pattern selection is not a WP2.9 feature. The displayed pattern is therefore explicitly `P01`, matching the current Phase 2 pattern-0 editor/playback proof.

### Header contract

The tracker header occupies one bounded region and has three small-text lines:

1. mode + cursor + transport + tempo, for example `SEQ CUR05 PLY02 120` or `STEP REC CUR05 STOP 120`;
2. pattern + selected track + 16-step page + visible global row range, for example `P01 T1 PG1/4 01-08`;
3. fixed column labels: `ST NOTE IN VOL FX`.

Rules:

- `CURnn` is always the selected edit step, shown 1-based `01..64`.
- while transport is running, `PLYnn` is the actual D020 transport step, shown 1-based `01..64`;
- while stopped, `STOP` replaces `PLYnn`;
- tempo is the current integer transport BPM;
- track is shown 1-based `T1..T8`;
- page is shown `PG1/4..PG4/4`.

### Visible rows and viewport

Exactly **8 tracker rows** are visible.

The viewport is owned by the edit cursor and is aligned to one half of the current 16-step D022 page:

- local cursor `0..7` shows the first eight rows of that page;
- local cursor `8..15` shows the second eight rows of that page.

Therefore the only possible global windows are:

- `01-08`, `09-16`;
- `17-24`, `25-32`;
- `33-40`, `41-48`;
- `49-56`, `57-64`.

The playhead never scrolls the viewport. This prevents playback from moving the edit surface under the user's physical step selection. If the playhead is outside the visible eight rows, its exact step remains visible as `PLYnn` in the header.

### Cursor versus playhead

Each row has two independent marker cells:

- `>` = selected edit row / cursor, drawn white;
- `*` = current running playhead row, drawn in the tracker accent colour.

If both refer to the same row, both markers are shown as `>*`; they are never collapsed into one ambiguous symbol. When transport is stopped no `*` marker is drawn. The header also uses explicit `CUR` and `PLY`/`STOP` labels, so selection and playback position are distinguishable even when the playhead is off-screen.

### Row text contract

Each visible row displays:

`ST NOTE IN VOL FX`

where:

- `ST` is the 1-based global step `01..64`;
- `NOTE` is a four-character fixed field;
- `IN` is the instrument field;
- `VOL` is the volume field;
- `FX` is the effect field.

Note representation:

- ordinary note: tracker-style pitch using the same MIDI octave convention as the existing performance display, e.g. `C-4 ` or `C#4 `; octave `-1` is represented explicitly as `C--1` / `C#-1` so the full model range remains unambiguous;
- `FMT_NOTE_EMPTY`: `--- `;
- `FMT_NOTE_TIE`: `=== `;
- `FMT_NOTE_OFF`: `OFF `;
- malformed/reserved note value: `??? `.

Instrument representation:

- `FMT_INSTRUMENT_INHERIT`: `..`;
- explicit valid raw instrument index `0..15`: UI value `01..16`;
- any other value: `??`.

Volume representation:

- `FMT_VOLUME_INHERIT`: `...`;
- explicit value `0..254`: zero-padded decimal `000..254`.

Effect representation:

- `FMT_EFFECT_NONE`: `---`;
- any non-zero effect: `???` in WP2.9 because stable effect IDs/names are deliberately deferred to Phase 3.

WP2.9 does not invent effect names, lock notation or editing semantics that do not exist yet.

### Footer contract

One bounded footer region provides context help only:

- ordinary SEQ: `KEYS STEP/TRK  OCT PAGE`;
- STEP REC: `FX RST  SEL TIE  FX+SEL OFF`.

The footer does not add new controls; it documents the already-frozen D022–D025 behavior.

### Pixel-region layout

The 240×240 tracker screen is tiled by exactly ten non-overlapping redraw regions:

- header: `240×64` at `y=0`;
- eight row strips: each `240×18`, covering `y=64..207`;
- footer: `240×32` at `y=208`.

These regions cover the full screen exactly, so entering tracker context requires no unbounded full-framebuffer renderer. The existing small canvas remains sufficient because the largest tracker region is `240×64`, below the existing `CV_MAX` strip budget.

### Dirty/redraw contract

WP2.9 introduces a small bounded presentation cache/dirty mask in normal RAM.

Hard rules:

- there are exactly 10 dirty regions: header, footer and eight rows;
- at most **2 tracker regions may be rendered/blitted per main-loop iteration**;
- entering tracker context or changing selected track/page/half-page/mode marks all 10 regions dirty, then drains them at the two-region limit;
- cursor movement within the same eight-row window dirties only the old/new cursor rows plus the header;
- a running playhead move dirties only the old/new visible playhead rows plus the header;
- changes to display-relevant row data dirty only the affected visible row;
- state inspection is bounded to the eight visible rows per update;
- all drawing happens in the main loop; the audio ISR never calls drawing code;
- pending dirty bits persist until rendered, so the two-region limit never drops updates.

The presentation state target is **<= 64 bytes ordinary RAM** and must allocate no pool memory.

### Required verification before closeout

Automated tests must prove:

1. all eight possible half-page windows and their exact global ranges;
2. selected cursor is always inside its viewport;
3. `>` and `*` are independent, including same-row `>*` state and off-screen playhead behavior;
4. exact ordinary/EMPTY/TIE/OFF/malformed note strings;
5. instrument/volume/effect placeholder rules;
6. dirty-region transitions and the two-region-per-poll drain limit;
7. screen-state reads cannot mutate the full song store;
8. HOME↔SEQ/STEP REC transitions force the correct screen ownership;
9. representative host screenshots are deterministic and visually inspected;
10. all WP2.1–WP2.8 regressions and the frozen default WAV remain unchanged;
11. full JieLi device build, package identity and inherited safety checks remain green;
12. image/RAM/pool delta is recorded against WP2.8.

## Non-goals

D026 does not add:

- p-lock allocation/editing;
- effect IDs or effect editing;
- general multitrack playback;
- a new synth engine;
- live-performance capture;
- song/order/pattern selection;
- persistence;
- HAL/loader/update changes;
- WP2.10 integration work.

Physical 240×240 readability and marker ergonomics remain provisional until the Phase 7 hardware UX freeze.