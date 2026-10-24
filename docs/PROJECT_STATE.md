# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 3 - p-locks and tracker effects**

State: **IN PROGRESS - WP3.1-WP3.7 complete; WP3.8 active, D034 frozen before source - 2026-10-24**

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

Final verified source/workflow checkpoint:

`d1910cb86b84240051cedfabb7701140259d8274`

Final GitHub Actions:

- run `40073680548`
- job `120812368036` (`tracker-effects-closeout`)
- result **SUCCESS**

Evidence: `docs/WP3_6_VERIFICATION.md`  
Normative contract: `docs/WP3_6_EFFECT_CONTRACT.md`

D032 freezes commands `0x00..0x05` for NONE / ARPEGGIO / SLIDE_UP / SLIDE_DOWN / PORTAMENTO / VIBRATO in the existing `FmtStep.effect/effect_value` bytes. `FmtStep` remains 8 B and `FmtPlaybackEvent` remains 4 B.

D032 timing uses exactly six effect ticks per row from the D020 phase domain: row phase `661500`, effect tick phase `110250`, tick 0 at row dispatch, internal thresholds `110250/220500/330750/441000/551250`, next row at `661500`. At 120 BPM the verified nonzero tick frames are `919,1838,2757,3675,4594`; the next row starts at `5513`.

WP3.6 adds one 256 B aligned effect-row cache and a 12 B `FmtTrackerEffectState`.

Final WP3.6 resources: 65,528 B image / 13,004 B normal RAM / 113,000 B pool.

### WP3.7 - retrig / note delay / note cut - COMPLETE - #26

Start head:

`e2f354018b4cbad636b51ac8ae98c9b226e7ef18`

Production audio-lane integration:

`9ab3646a8b7ee36027d22855bd7bade61b3e46cf`

Final clean-tree verified source/workflow checkpoint:

`d2a97798149816957745de818e4568c518a9a7e7`

Final GitHub Actions:

- run `40077809743`
- job `120824870575` (`tracker-events-closeout`)
- result **SUCCESS**

Evidence: `docs/WP3_7_VERIFICATION.md`  
Normative contract: `docs/WP3_7_EVENT_CONTRACT.md`

Evidence artifact:

- ID `12553932464`
- ZIP SHA-256 `5fca527df35446a879934b91d35d22acf0a0d7bc09ef1eb93e8e57fa25c9d332`

D033 extends the same tracker effect-command domain:

- `0x06` RETRIG
- `0x07` NOTE DELAY
- `0x08` NOTE CUT
- `0x09..0xFF` safe NONE/fallback.

D033 reuses D032's exact six-tick row grid and deliberately does **not** introduce the WP3.8 general queue. It adds only `FmtTrackerRowEventState`, exactly 4 B ordinary RAM.

Key semantics:

- RETRIG `1..5` resets proof-oscillator phase at matching nonzero ticks while preserving logical gate/level, pitch and D032 state; `0` means no retrig and `>5` is a no-op.
- NOTE DELAY `1..5` withholds only an ordinary NOTE until the exact tick. TIE/EMPTY/OFF are never delayed. The delayed NOTE consumes the exact D031 row snapshot captured at row dispatch. `0` is immediate and `>5` is a no-op.
- NOTE CUT `0..5` applies synthetic D025 OFF semantics at the exact tick; CUT 0 occurs after the ordinary row event but before the first rendered sample. `>5` is a no-op.
- at each nonzero tick there is at most one fixed-state D033 action; no variable event loop or allocator enters the audio path.

Focused proof:

`FMT_TRACKER_EVENT_TESTS_OK=1`

`WP3_7_EVENT_AUDIO_BEHAVIOR_OK=1 delay=1 cut=1 cut0=1 retrig=1 invalid_noop=1 midi_events=0`

Measured 120 BPM behavior:

- NOTE DELAY 3 starts at frame `2757`
- NOTE CUT 3 diverges from control at frame `2757`
- RETRIG 2 diverges from control at frame `1838`
- delay/cut/retrig value `6` are each byte-identical to the control WAV.

Final WP3.7 resources:

| Resource | Used | Limit | Free | Delta vs WP3.6 |
|---|---:|---:|---:|---:|
| app image | 65,940 B | 581,564 B | 515,624 B | +412 B |
| normal RAM | 13,008 B | 98,304 B | 85,296 B | +4 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B | +0 B |

Final firmware artifacts:

- `fm1tracker.bin` SHA-256 `f71bc5770ce2b56bcbc3fb4034b882f1a80f0b1997cdd4534cc7b8dd45100e2b`
- `fm1tracker-0.1.fwsc` SHA-256 `cd4b3138ecb6e4e853b67cbe4b8a049e43290798dad26accf606279dc69e383d`
- package identity `FM-1_987`.

Historical default WAV remains byte-identical:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

D031 locked WAV remains byte-identical:

`8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8`

All frozen WP3.6 D032 audio fixtures also remain byte-identical through WP3.7.

## Next milestone

**WP3.8 - bounded event queue and overflow policy**

State: **IN PROGRESS - D034 frozen; production queue implementation next**

GitHub issue: #27

D034 is frozen in `docs/WP3_8_QUEUE_CONTRACT.md`: exactly 8-byte `FmtRtEvent`, 32-entry / 268-byte SPSC main->audio queue, producer-owned `write_seq`/`overflow_seq`, consumer-owned `read_seq`, aligned two-word publication, reject-newest overflow, callback-boundary immediate timing, and fail-to-silence audition recovery. D033 and the D024 transport mailbox remain outside the queue.

Production source may now implement **only** this contract, beginning with the queue module/unit tests and then routing existing manual key/USB-MIDI NOTE ON/OFF through it. Live capture/timestamp semantics and multi-track sequencer migration remain later work.

## Important decisions / do not regress

- D005: 8 logical tracks x 64 steps x 16 patterns.
- D018: `FmtStep` exactly 8 B.
- D020/D021: exact integer transport and next-frame row-boundary ordering.
- D025: explicit NONE/NOTE/TIE/OFF semantics.
- D027: parameter IDs remain separate from tracker effect command IDs.
- D028-D030: bounded p-lock storage/editor behavior.
- D031: three row-local p-lock playback parameters through a 256 B 32-bit cache; TRACK PAN storage-only.
- D032: six-tick tracker effects, persistent Q7 slide/porta pitch and transient ARP/vibrato through a second 256 B row cache.
- D033: retrig/delay/cut remain fixed-state, exact-six-tick and queue-free; event runtime exactly 4 B at the verified checkpoint.
- Preserve the historical default WAV unless a later deliberate decision supersedes it with explicit evidence.
- No float in firmware DSP and no runtime 64-bit division.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted future core direction. Phase 3 is the bounded tracker/p-lock/effect/event foundation for that milestone.

## Hardware limitation still open

No physical FM-1 is currently available. Host/device-build evidence does not claim physical effect/event feel, install/boot/audio behavior, physical sound quality or measured AC791N execution margin.

## Exact next action

Implement the frozen D034 queue module and focused unit tests first, then integrate existing manual key/USB-MIDI audition NOTE ON/OFF through the queue. Do not expand into live capture or D033 queueing.

## Last relevant checkpoints

- Phase 2 verified: `7063be51e53c2657178084b3bef5ace813dafcd7`
- Phase 3 branch start: `c3693d8c7bb3d5306794aa232d6d25bef50c6c99`
- WP3.1 verified: `5af8065a9c98884d22682075b600d9790cfa7940`
- WP3.2 verified: `da2f5afe9b1da6defc3857b91f52b40d7525f33b`
- WP3.3 verified: `df5d7e07fc763909a141c99283cfd2be19cff847`
- WP3.4 verified: `178ba57a19adc8e25106f1c63e0df2f16478fb33`
- WP3.5 verified: `17316f203dba7a93c9d03a52df6f45b12a7a2580`
- WP3.6 verified: `d1910cb86b84240051cedfabb7701140259d8274`
- WP3.7 verified: `d2a97798149816957745de818e4568c518a9a7e7`

## Last updated

2026-10-24 - WP3.7 complete and #26 closed. WP3.8 / #27 active; D034 frozen in `WP3_8_QUEUE_CONTRACT.md` before source. No WP3.8 production source has begun yet.
