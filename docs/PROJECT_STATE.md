# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 3 - p-locks and tracker effects**

State: **IN PROGRESS - WP3.6 active - 2026-10-24**

Parent issue: #20
Current package issue: #25
Current branch: `phase3/plocks-effects`

Phase 3 branch start / completed Phase 2 handoff:

`c3693d8c7bb3d5306794aa232d6d25bef50c6c99`

## Completed Phase 3 packages

### WP3.1 - stable parameter ID registry - COMPLETE - #19
Verified checkpoint `5af8065a9c98884d22682075b600d9790cfa7940`. Evidence: `docs/WP3_1_VERIFICATION.md`.

D027 freezes one-byte semantic parameter identity and the four current production-lockable IDs: VOICE LEVEL, FINE PITCH, TRACK LEVEL and TRACK PAN.

### WP3.2 - sparse four-lock-per-step storage - COMPLETE - #21
Verified checkpoint `da2f5afe9b1da6defc3857b91f52b40d7525f33b`. Evidence: `docs/WP3_2_VERIFICATION.md`.

D028 freezes one owner-checked global sparse pool of 2,048 eight-byte lock records, max four active locks per step, sorted semantic IDs and corruption-safe bounded operations.

### WP3.3 - hold-step + encoder p-lock editing - COMPLETE - #22
Verified checkpoint `df5d7e07fc763909a141c99283cfd2be19cff847`, final run `40054322180`. Evidence: `docs/WP3_3_VERIFICATION.md`.

D029 freezes held-white-step target ownership, stale/begin/release encoder-delta discard and KNOB1..4 normalized `0..255` lock editing.

### WP3.4 - p-lock browse / single-lock delete UX - COMPLETE - #23
Verified checkpoint `178ba57a19adc8e25106f1c63e0df2f16478fb33`, final run `40064122343`. Evidence: `docs/WP3_4_VERIFICATION.md`.

D030 freezes sorted SELECT browse plus deliberate EDIT arm -> later SEL one-lock deletion, with same-poll rejection, cancellation safety and no clear-all panel gesture.

### WP3.5 - basic pitch/volume p-lock playback - COMPLETE - #24

Start head `01b35603ec387e079e4af512f8a8109e413d2714`.

Verified source/workflow checkpoint:

`17316f203dba7a93c9d03a52df6f45b12a7a2580`

Final verification:

- run `40069110111`
- job `120798506694`
- result SUCCESS
- evidence `docs/WP3_5_VERIFICATION.md`
- artifact ID `12550920281`
- artifact ZIP SHA-256 `a6e92feef5639f69619afa458e18b626bd717e9c8d20b483fc414921a7f5cbcd`

D031 makes VOICE LEVEL, FINE PITCH and TRACK LEVEL audible as row-local overrides on the pattern-0 / track-0 proof lane; TRACK PAN remains storage-only. Main/editor resolves at most four D028 records into one aligned 32-bit row snapshot. Audio never traverses mutable D028 lists. `FmtPlaybackEvent` remains 4 B and D020/D021 timing is unchanged.

Frozen no-lock WAV remains byte-identical:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

Frozen D031 locked-audio WAV:

`8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8`

Final WP3.5 resources:

| Resource | Used | Limit | Free | Delta vs WP3.4 |
|---|---:|---:|---:|---:|
| app image | 64,724 B | 581,564 B | 516,840 B | +496 B |
| normal RAM | 12,736 B | 98,304 B | 85,568 B | +256 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B | +0 B |

The +256 B RAM delta is exactly the D031 row snapshot cache.

Final WP3.5 artifacts:

- `fm1tracker.bin` SHA-256 `d0380ac6cd2ee0e5edb1eab3e4d3937bb62eb074d27219927b3e8b7e52a244ae`
- `fm1tracker-0.1.fwsc` SHA-256 `5e14f28985fcda4c3f54dbe8a45ff020aba65a565696972fde30d4fa366d3372`
- identity `FM-1_987`

## Current milestone

**WP3.6 - arpeggio / slide / portamento / vibrato tracker effects**

State: **IN PROGRESS - architecture freeze only - 2026-10-24**

GitHub issue: #25

WP3.6 start head:

`44f4a6573c8729735a82283294fcf4837cf70b24`

No WP3.6 production implementation existed at this checkpoint.

### Exact objective

Add the first bounded tracker-effect command layer on the existing pattern-0 / track-0 proof lane after D031, without pulling WP3.7 retrig/note-delay/note-cut or WP3.8 general event-queue architecture forward.

Before source, the next decision must freeze:

- exact effect command IDs and `FmtStep.effect/effect_value` encoding;
- separation from D027 parameter-lock IDs;
- row-local command versus persistent pitch/effect state semantics;
- exact sub-row timing derived from D020/D021;
- integer arpeggio/slide/portamento/vibrato pitch rules;
- NOTE/TIE/OFF/EMPTY interaction;
- precedence/composition with D031 FINE PITCH;
- invalid-command fallback;
- bounded main/audio ownership and any fixed cache;
- hard per-frame/per-row work bounds;
- deterministic host/event/audio evidence and the frozen no-effect regression rule.

### Reviewed implementation direction before the decision

The existing `FmtStep` already carries one-byte `effect` and one-byte `effect_value` fields. `FMT_EFFECT_NONE = 0` is frozen by the core model, while no nonzero tracker effect IDs have yet been assigned.

A narrow direction to freeze is:

- use `FmtStep.effect/effect_value`, not D027 p-lock IDs, for tracker effects;
- keep the 4-byte D021 `FmtPlaybackEvent` unchanged;
- publish effect command/value outside the ISR through a fixed row cache rather than expanding the general event object;
- derive six exact tracker sub-ticks per row from the same D020 integer phase domain, with constant per-frame add/compare work;
- keep effect values explicit per row rather than adding hidden zero-value parameter memory;
- let stateful pitch motion from slide/portamento remain deterministic pitch state while command/value themselves are row-local;
- apply D031 FINE PITCH as a row-local additive offset to the stateful coarse/effect pitch domain;
- keep arpeggio/vibrato transient and non-destructive to the persistent center pitch;
- use safe NONE behavior for unassigned effect IDs.

This is not yet the accepted decision and must not be coded until D032 is appended to `DECISIONS.md`.

### Strict non-goals

- no retrig / note delay / note cut - WP3.7;
- no general event queue or overflow policy - WP3.8;
- no full row lock/effect visualization - WP3.9;
- no synth engine implementation;
- no persistence or live capture;
- no final hardware control-map freeze.

## Important decisions / do not regress

- D005: 8 logical tracks x 64 steps x 16 patterns.
- D018: `FmtStep` exactly 8 B.
- D020/D021: exact integer transport and next-frame row-boundary ordering.
- D025: explicit NONE/NOTE/TIE/OFF semantics.
- D027: parameter IDs remain separate from tracker effect command IDs.
- D028-D030: bounded p-lock storage/editor behavior.
- D031: three row-local p-lock playback parameters through a 256-byte 32-bit cache; TRACK PAN storage-only.
- Preserve the historical default WAV unless a deliberate effect fixture explicitly adds separate evidence.
- No float in firmware DSP and no runtime 64-bit division.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted future core direction. Phase 3 is still only the bounded tracker/p-lock/effect foundation for that milestone.

## Hardware limitation still open

No physical FM-1 is currently available. Host/device-build evidence cannot claim physical effect feel, sound quality, install/boot behavior or measured AC791N execution margin.

## Exact next action

Append and accept D032 before changing firmware source. Then implement the smallest pure tracker-effect state/cache module and focused host tests before integrating it into `dinghy.c`. Do not begin WP3.7.

## Last relevant checkpoints

- Phase 2 verified: `7063be51e53c2657178084b3bef5ace813dafcd7`
- Phase 3 branch start: `c3693d8c7bb3d5306794aa232d6d25bef50c6c99`
- WP3.1 verified: `5af8065a9c98884d22682075b600d9790cfa7940`
- WP3.2 verified: `da2f5afe9b1da6defc3857b91f52b40d7525f33b`
- WP3.3 verified: `df5d7e07fc763909a141c99283cfd2be19cff847`
- WP3.4 verified: `178ba57a19adc8e25106f1c63e0df2f16478fb33`
- WP3.5 verified: `17316f203dba7a93c9d03a52df6f45b12a7a2580`
- WP3.6 start head: `44f4a6573c8729735a82283294fcf4837cf70b24`

## Last updated

2026-10-24 - WP3.6 / issue #25 marked IN PROGRESS at `44f4a657...`; D032 must be frozen before source.