# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 3 - p-locks and tracker effects**

State: **IN PROGRESS - WP3.6 complete; WP3.7 ready to start - 2026-10-24**

Parent issue: #20  
Current branch: `phase3/plocks-effects`

Phase 3 branch start / completed Phase 2 handoff:

`c3693d8c7bb3d5306794aa232d6d25bef50c6c99`

## Completed Phase 3 packages

### WP3.1 - stable parameter ID registry - COMPLETE - #19

Verified checkpoint `5af8065a9c98884d22682075b600d9790cfa7940`.  
Evidence: `docs/WP3_1_VERIFICATION.md`.

D027 freezes one-byte semantic parameter identity and the four current production-lockable IDs: VOICE LEVEL, FINE PITCH, TRACK LEVEL and TRACK PAN.

### WP3.2 - sparse four-lock-per-step storage - COMPLETE - #21

Verified checkpoint `da2f5afe9b1da6defc3857b91f52b40d7525f33b`.  
Evidence: `docs/WP3_2_VERIFICATION.md`.

D028 freezes one owner-checked global sparse pool of 2,048 eight-byte lock records, max four active locks per step, sorted semantic IDs and corruption-safe bounded operations.

### WP3.3 - hold-step + encoder p-lock editing - COMPLETE - #22

Verified checkpoint `df5d7e07fc763909a141c99283cfd2be19cff847`, final run `40054322180`.  
Evidence: `docs/WP3_3_VERIFICATION.md`.

D029 freezes held-white-step target ownership, stale/begin/release encoder-delta discard and KNOB1..4 normalized `0..255` lock editing.

### WP3.4 - p-lock browse / single-lock delete UX - COMPLETE - #23

Verified checkpoint `178ba57a19adc8e25106f1c63e0df2f16478fb33`, final run `40064122343`.  
Evidence: `docs/WP3_4_VERIFICATION.md`.

D030 freezes sorted SELECT browse plus deliberate EDIT arm -> later SEL one-lock deletion, with same-poll rejection, cancellation safety and no clear-all panel gesture.

### WP3.5 - basic pitch/volume p-lock playback - COMPLETE - #24

Verified checkpoint `17316f203dba7a93c9d03a52df6f45b12a7a2580`.  
Final run `40069110111`, job `120798506694`.  
Evidence: `docs/WP3_5_VERIFICATION.md`.

D031 makes VOICE LEVEL, FINE PITCH and TRACK LEVEL audible as row-local overrides on pattern 0 / track 0; TRACK PAN remains storage-only. Audio consumes one aligned 32-bit row snapshot and never traverses D028 lists.

### WP3.6 - arpeggio / slide / portamento / vibrato tracker effects - COMPLETE - #25

Start head:

`44f4a6573c8729735a82283294fcf4837cf70b24`

Final verified source/workflow checkpoint:

`d1910cb86b84240051cedfabb7701140259d8274`

Final GitHub Actions:

- run `40073680548`
- job `120812368036` (`tracker-effects-closeout`)
- result **SUCCESS**

Evidence: `docs/WP3_6_VERIFICATION.md`  
Normative contract: `docs/WP3_6_EFFECT_CONTRACT.md`

Evidence artifact:

- ID `12552566558`
- ZIP SHA-256 `3ce38fec8f73f1f2a842cc335a45a4a95be7bd60e1edba65cc89654a9cd4a3de`

D032 freezes the tracker effect command domain:

- `0x00` NONE
- `0x01` ARPEGGIO
- `0x02` SLIDE_UP
- `0x03` SLIDE_DOWN
- `0x04` PORTAMENTO
- `0x05` VIBRATO

Effects use the existing `FmtStep.effect/effect_value` bytes and remain separate from D027 p-lock IDs. `FmtStep` remains 8 B and `FmtPlaybackEvent` remains 4 B.

D032 timing uses exactly six effect ticks per row from the D020 phase domain:

- row phase `661500`
- effect tick phase `110250`
- tick 0 at row dispatch
- internal thresholds `110250`, `220500`, `330750`, `441000`, `551250`
- next row at `661500`; row dispatch wins over any synthetic tick 6.

At 120 BPM the verified effect tick frames are `919,1838,2757,3675,4594`; the next row starts at frame `5513`.

Persistent proof pitch is Q7. SLIDE and PORTAMENTO may change persistent pitch; ARPEGGIO and VIBRATO are transient. Final pitch precedence is:

`persistent_pitch_q7 + (D031_FINE - 128) + transient_effect_mod_q7`

clamped to `0..16383`.

WP3.6 adds:

- `fmt_tracker_effect_cache[64]`: 256 B ordinary RAM
- `FmtTrackerEffectState`: 12 B ordinary RAM

Audio reads one 32-bit effect snapshot at row dispatch. No allocator, D028 traversal, variable event loop, screen/flash work, float or runtime 64-bit division enters the effect path.

Focused proof:

`FMT_TRACKER_EFFECT_TESTS_OK=1`

`WP3_6_TICK_GRID_TESTS_OK=1`

`WP3_6_EFFECT_AUDIO_BEHAVIOR_OK=1 arp_ticks=1 slide_up=1 slide_down=1 porta_glide=1 vibrato=1 fine_composition=1 invalid_none=1 midi_events=0`

Invalid command audio is byte-identical to control.

Historical default WAV remains byte-identical:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

D031 locked WAV remains byte-identical:

`8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8`

Final WP3.6 resources:

| Resource | Used | Limit | Free | Delta vs WP3.5 |
|---|---:|---:|---:|---:|
| app image | 65,528 B | 581,564 B | 516,036 B | +804 B |
| normal RAM | 13,004 B | 98,304 B | 85,300 B | +268 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B | +0 B |

The +268 B RAM delta is exactly the 256 B D032 effect cache plus 12 B effect runtime state.

Final artifacts:

- `fm1tracker.bin` SHA-256 `01f8dc1fcec3371256dca767458709dcdc48303c833f7a4f9cb214ca4d5f0860`
- `fm1tracker-0.1.fwsc` SHA-256 `ee7d14f32497fd701c3a291a4728fa1f3b56a881dab2dafdf497fb74fb3d5968`
- package identity `FM-1_987`

## Next milestone

**WP3.7 - retrig / note delay / note cut**

State: **READY TO START - no WP3.7 implementation has begun**

GitHub issue: #26

Before source, freeze D033 covering:

- exact command IDs and encoding for retrig, note delay and note cut;
- exact sub-row timing and interaction with D020/D021/D032;
- NOTE/TIE/OFF/EMPTY behavior;
- retrigger phase/gate semantics;
- delay/cut behavior on rows without a NOTE;
- deterministic ordering against D032 ARP/slide/porta/vibrato and D031 p-locks;
- bounded fixed state/work and malformed-command fallback;
- whether WP3.7 truly needs any minimal WP3.8 queue primitive.

Do not pull the general event queue/overflow design forward without an explicit superseding decision.

## Important decisions / do not regress

- D005: 8 logical tracks x 64 steps x 16 patterns.
- D018: `FmtStep` exactly 8 B.
- D020/D021: exact integer transport and next-frame row-boundary ordering.
- D025: explicit NONE/NOTE/TIE/OFF semantics.
- D027: parameter IDs remain separate from tracker effect command IDs.
- D028-D030: bounded p-lock storage/editor behavior.
- D031: three row-local p-lock playback parameters through a 256 B 32-bit cache; TRACK PAN storage-only.
- D032: six-tick tracker effects, persistent Q7 slide/porta pitch and transient ARP/vibrato through a second 256 B row cache.
- Preserve the historical default WAV unless a later deliberate decision supersedes it with explicit evidence.
- No float in firmware DSP and no runtime 64-bit division.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted future core direction. Phase 3 is the bounded tracker/p-lock/effect/event foundation for that milestone.

## Hardware limitation still open

No physical FM-1 is currently available. Host/device-build evidence does not claim physical effect feel, install/boot/audio behavior, physical sound quality or measured AC791N execution margin.

## Exact next action

Start WP3.7 / issue #26 as its own package. Verify actual branch HEAD, mark WP3.7 IN PROGRESS, freeze D033 before source, and do not begin WP3.8.

## Last relevant checkpoints

- Phase 2 verified: `7063be51e53c2657178084b3bef5ace813dafcd7`
- Phase 3 branch start: `c3693d8c7bb3d5306794aa232d6d25bef50c6c99`
- WP3.1 verified: `5af8065a9c98884d22682075b600d9790cfa7940`
- WP3.2 verified: `da2f5afe9b1da6defc3857b91f52b40d7525f33b`
- WP3.3 verified: `df5d7e07fc763909a141c99283cfd2be19cff847`
- WP3.4 verified: `178ba57a19adc8e25106f1c63e0df2f16478fb33`
- WP3.5 verified: `17316f203dba7a93c9d03a52df6f45b12a7a2580`
- WP3.6 verified: `d1910cb86b84240051cedfabb7701140259d8274`

## Last updated

2026-10-24 - WP3.6 verified complete. WP3.7 / #26 is READY TO START; no WP3.7 source exists.
