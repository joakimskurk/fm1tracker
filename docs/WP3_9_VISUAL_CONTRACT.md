# WP3.9 Visual Feedback Contract — D035

Date: 2026-10-24
Status: **ACCEPTED BEFORE WP3.9 PRODUCTION UI SOURCE**

This document freezes WP3.9 lock/effect visual feedback before implementation. It extends the already accepted D026 tracker-screen contract without changing sequencer, p-lock, effect, queue or audio semantics.

## D035 — lock/effect feedback extends the existing bounded tracker surface

### 1. Scope and ownership

WP3.9 is presentation-only.

- The existing 240×240 D026 tracker screen remains the only SEQ / STEP REC tracker surface.
- The existing ten redraw regions remain unchanged: one header, one footer and eight 18-pixel row strips.
- The D026 limit remains **at most two tracker regions rendered/blitted per main-loop poll**.
- All display synchronization and drawing remain main-loop work.
- Audio code/ISR must never call drawing code, mutate screen state or wait for screen work.
- Screen synchronization may read the current `FmtStep`, D028 lock store and D029/D030 lock-editor state, but must never mutate song, locks, editor, transport, effect runtime or audio state.
- D034 queue overflow remains diagnostic/runtime-safety state in WP3.9. It does **not** acquire a normal tracker-screen indicator here.

### 2. Row layout

The D026 fixed row becomes:

`ST NOTE IN VOL LK FX`

The existing `ST`, `NOTE`, `IN` and `VOL` meanings do not change.

`LK` is a two-character p-lock summary field.

`FX` is a four-character stored tracker-effect field. It represents the row's stored `FmtStep.effect` + `FmtStep.effect_value`; it does not display transient audio-runtime tick state.

The row remains fixed-width and must fit the existing row strip/canvas without changing the 240×18 region geometry.

### 3. P-lock summary notation

For each visible row, main-loop presentation sync calls the bounded D028 `fmt_lock_count()` API for pattern 0 / selected track / row.

Normal row notation:

- `--` = zero locks
- `L1` = one stored lock
- `L2` = two stored locks
- `L3` = three stored locks
- `L4` = four stored locks
- `L!` = lock-list/address/corruption error while reading presentation state

Held-step edit target notation:

- `P0`..`P4` replace `--` / `L1..L4` only on the row currently owned by the active D029/D030 held-step p-lock editor.
- `P!` is used if that target's lock-count read fails.

`P` means **p-lock edit target**, not playhead and not a fifth cursor marker. The D026 `>` cursor and `*` playhead remain unchanged and independent.

The row summary deliberately does not encode individual parameter identities. D028 permits at most four locks, and the existing footer remains the detailed browse/edit surface.

### 4. Audible versus storage-only lock feedback

Normal row `LK` summarizes storage only. It does not visually classify each lock as audible or storage-only.

When the active p-lock editor is showing `FMT_PARAM_TRACK_PAN`, the footer must append the literal `STORE` marker to the existing parameter/value feedback, because TRACK PAN is still storage-only under D031.

Examples:

- `P1/2 K4 PAN 128 STORE`
- `K4 PAN 140 LOCK STORE`

VOICE LEVEL, FINE PITCH and TRACK LEVEL keep the existing footer wording with no `STORE` suffix.

This distinction is presentation-only and must not change parameter IDs, storage or playback behavior.

### 5. Effect notation

The four-character `FX` field uses a two-letter command mnemonic followed by the stored value as two uppercase hexadecimal digits:

- `----` = `FMT_TRACKER_EFFECT_NONE` (`0x00`)
- `ARhh` = ARPEGGIO (`0x01`)
- `SUhh` = SLIDE UP (`0x02`)
- `SDhh` = SLIDE DOWN (`0x03`)
- `POhh` = PORTAMENTO (`0x04`)
- `VIhh` = VIBRATO (`0x05`)
- `RThh` = RETRIG (`0x06`)
- `DLhh` = NOTE DELAY (`0x07`)
- `CThh` = NOTE CUT (`0x08`)
- `????` = unsupported/reserved command `0x09..0xFF`

`hh` is always the raw stored `effect_value` byte in uppercase hexadecimal, including values that are semantic no-ops for a particular command. WP3.9 displays stored truth; it does not reinterpret or clamp D032/D033 values.

Examples: `AR37`, `SU04`, `RT02`, `DL03`, `CT00`.

### 6. Cursor / playhead / p-lock-editor priority

D026 marker ownership is unchanged:

- first marker cell: `>` selected edit cursor
- second marker cell: `*` running playhead
- same row: `>*`

WP3.9 adds no third marker cell.

If the active p-lock editor owns the selected row, `LK` uses `P0..P4/P!`. The footer then owns detailed selected-lock/delete feedback. Delete-arm state does not replace cursor/playhead markers and does not invent a row-level destructive icon.

Priority therefore remains explicit and non-destructive:

1. cursor/playhead marker cells keep D026 meaning;
2. row text reports stored note/instrument/volume/lock/effect state;
3. active lock-editor detail supersedes ordinary context help in the footer.

### 7. Footer composition and stale-state cancellation

The existing D029/D030 lock-editor footer continues to supersede the ordinary D026 footer while a lock editor is active.

WP3.9 only adds the `STORE` suffix for TRACK PAN as described above.

On editor release/cancel, HOME transition, STEP REC transition or any other state that ends the D029/D030 gesture:

- the old target row must be dirtied so `P*` returns to its normal `L*`/`--` representation;
- the footer must be dirtied so stale browse/delete/value text cannot remain;
- HOME still performs the existing clean HOME redraw;
- returning to tracker context still forces the normal D026 full ten-region refresh.

### 8. Presentation cache and RAM budget

`FmtTrackerScreen` remains the sole bounded tracker presentation cache.

WP3.9 adds exactly:

- `row_effect_value[8]` — 8 bytes;
- `row_lock_info[8]` — 8 bytes, encoding count `0..4` or one presentation-error sentinel;
- `lock_target_active` — 1 byte;
- `lock_target_step` — 1 byte.

With the verified D026 44-byte baseline, the target `FmtTrackerScreen` size is exactly **62 bytes** ordinary RAM and must remain below the existing 64-byte D026 cap. WP3.9 allocates no pool memory for presentation state.

The screen sync API may accept a read-only `FmtLockEditor` pointer so target state can be snapshotted. It must not retain pointers to mutable lock records.

### 9. Bounded inspection and dirty rules

Per tracker-screen sync:

- exactly the same eight visible `FmtStep` rows are inspected as D026;
- each visible row may perform one `fmt_lock_count()` call;
- each count traverses at most the D028 maximum of four lock records;
- therefore lock presentation work is bounded to at most 8 count calls / 32 lock-record inspections per sync;
- no allocator call and no mutation is permitted.

Dirty behavior:

- entering tracker context, track/page/half-page/mode changes: all ten regions dirty, unchanged from D026;
- note/instrument/volume/effect-command/effect-value change: only that visible row becomes dirty;
- lock count/error presentation change: only that visible row becomes dirty;
- lock-editor target begins/ends or changes visible target: old/new target row(s) become dirty;
- lock browse/value/delete feedback that changes only footer detail dirties footer only;
- cursor/playhead/header rules remain D026;
- pending dirty bits persist until consumed;
- renderer still consumes no more than two regions per main-loop iteration.

### 10. Color and readability

WP3.9 does not depend on new colors for semantic correctness. The fixed textual mnemonics must carry the information even in monochrome screenshots.

The existing row color policy remains valid:

- selected cursor row text may remain white;
- ordinary row text may remain the existing key/text color;
- playhead `*` remains accent-colored.

Any additional field-specific coloring is optional polish only if it does not alter region count, redraw bounds or deterministic screenshot semantics.

### 11. Required host/device verification

Before WP3.9 can close, tests must prove at minimum:

1. `FmtTrackerScreen` is exactly 62 bytes and still within the D026 64-byte cap;
2. exact `LK` strings for 0..4 locks, active-target `P0..P4`, and error state;
3. all D032/D033 effect mnemonics and uppercase hexadecimal values;
4. unsupported effect commands render `????` without mutating stored data;
5. effect-value-only changes dirty only the affected visible row;
6. lock-count changes dirty only the affected visible row;
7. editor begin/end dirties target row plus existing footer feedback as required;
8. TRACK PAN footer feedback is visibly marked `STORE` while the three D031 audible parameters are not;
9. D026 cursor/playhead marker independence and all eight viewport windows remain unchanged;
10. the ten-region / two-regions-per-poll contract remains exact;
11. screen sync cannot mutate song or lock-store state;
12. HOME↔tracker transitions clear stale p-lock/effect feedback;
13. deterministic host framebuffer fixtures cover ordinary rows, effects, p-lock counts and active browse/delete feedback;
14. reviewed screenshots fit the existing row/header/footer geometry without clipping;
15. historical default WAV, D031 locked WAV, D032/D033 fixtures and WP3.8 queue-audio fixtures remain byte-identical;
16. full JieLi/PI32v2 device build, package identity, safety checks and resource accounting remain green.

### 12. Non-goals

D035/WP3.9 does not add:

- effect editing controls;
- new tracker effects or effect values;
- new p-lock parameters;
- D034 queue behavior or overflow UI;
- live capture/REC semantics;
- multitrack playback migration;
- synth-engine work;
- persistence;
- a new screen/page/menu system;
- more than ten tracker redraw regions;
- more than two tracker redraws per poll;
- Phase 3 worst-case closeout (WP3.10).

Physical 240×240 readability remains provisional until hardware is available and Phase 7 performs the final control/display UX freeze.
