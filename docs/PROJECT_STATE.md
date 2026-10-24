# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 4 - 8-BIT engine**

State: **IN PROGRESS - WP4.1 active; D037 engine-interface architecture freeze next - 2026-10-24**

Parent issue: #30
Current work package: WP4.1 / #31
Current branch: `phase4/8bit-engine`

Phase 4 branch start / final Phase 3 handoff:

`d14955f23a11044e8a348884feca24f8978fd700`

Final Phase 3 production firmware baseline inherited by Phase 4:

`354bf7d356774ad828f1f660bd456dbab7f0490d`

Final Phase 3 closeout run/job:

`40084706845` / `120845144453`

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

### WP3.8 - bounded runtime event queue and overflow policy - COMPLETE - #27

Production audition-queue integration:

`223fec91746a95cc003eabe7cd1540af75e5ed26`

Final clean-tree verified source/workflow checkpoint:

`5c2828921f30c7d299cbc0e7aa6b7cfa9f4b3018`

Final GitHub Actions:

- run `40079824265`
- job `120830854974` (`runtime-queue-closeout`)
- result **SUCCESS**

Evidence: `docs/WP3_8_VERIFICATION.md`
Normative contract: `docs/WP3_8_QUEUE_CONTRACT.md`

Evidence artifact:

- ID `12554234479`
- ZIP SHA-256 `2bd3b62cbfe5eec8867066acbdcecc5221dc515afa525f3914dab04b3f8b219f`

D034 freezes an exact 8-byte `FmtRtEvent` and a 32-entry / 268-byte SPSC main-loop -> audio-ISR queue. Physical-key and USB-MIDI audition NOTE ON/OFF now enter audio through the queue; main no longer selects or mutates manual audio voices directly.

Overflow is reject-newest. Audio detects `overflow_seq`, releases all manual gates click-safely and consumer-discards the compromised batch in O(1), so overflow fails toward silence rather than a stuck note. Sequence wrap is verified across `UINT32_MAX`.

The compiler publication barrier is isolated behind `firmware/hal/fm1_barrier.h`, which is the only HAL-file difference from the WP3.7 verified checkpoint. D024 transport state and D033 sub-row scheduling remain outside the queue.

Focused proof:

`FMT_RT_QUEUE_TESTS_OK=1`

`WP3_8_QUEUE_AUDIO_BEHAVIOR_OK=1 fifo=1 hardware=1 midi=1 overflow_silence=1 recovery=1 queue_midi_neutral=1`

Final WP3.8 resources:

| Resource | Used | Limit | Free | Delta vs WP3.7 |
|---|---:|---:|---:|---:|
| app image | 66,308 B | 581,564 B | 515,256 B | +368 B |
| normal RAM | 13,280 B | 98,304 B | 85,024 B | +272 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B | +0 B |

Final firmware artifacts:

- `fm1tracker.bin` SHA-256 `a1d422b0c533af7f713deca37191a45e438640e2fc1be7c2734c1e4539822251`
- `fm1tracker-0.1.fwsc` SHA-256 `b93d5dd57d4e23214ac4ef500e47d91b3e4528b620b646f8804312654df98b8a`
- package identity `FM-1_987`.

Historical default WAV, D031 locked WAV and all frozen WP3.6/WP3.7 audio fixtures remain byte-identical through WP3.8.

### WP3.9 - lock/effect visual feedback - COMPLETE - #28

Final verified production checkpoint:

`354bf7d356774ad828f1f660bd456dbab7f0490d`

Final GitHub Actions:

- run `40083510351`
- job `120841687772` (`visual-feedback-closeout`)
- result **SUCCESS**

Evidence: `docs/WP3_9_VERIFICATION.md`
Normative contract: `docs/WP3_9_VISUAL_CONTRACT.md`

Evidence artifact:

- ID `12555799262`
- ZIP SHA-256 `62bdffec5faa027484ccc8bf99b2babbb978874054182de97970b8d52f333bca`

D035 keeps the D026 ten-region / max-two-redraw tracker surface and adds `LK` p-lock summaries plus four-character stored effect mnemonics. `FmtTrackerScreen` is exactly 62 B. TRACK PAN detailed feedback is marked `STORE`; D031-D034 runtime/audio semantics are unchanged.

Reviewed framebuffers:

- active target `d290415387ac8e55006863819c2188d64f0d0453e204bb194cb8d004121484f3`
- released rows `ec15c6db429fe26d453ada14cc28a1eab8c2aa93e83c6f0bbfff67112f1a86c6`

Final WP3.9 resources:

| Resource | Used | Limit | Free | Delta vs WP3.8 |
|---|---:|---:|---:|---:|
| app image | 66,960 B | 581,564 B | 514,604 B | +652 B |
| normal RAM | 13,296 B | 98,304 B | 85,008 B | +16 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B | +0 B |

Final firmware artifacts:

- `fm1tracker.bin` SHA-256 `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27`
- `fm1tracker-0.1.fwsc` SHA-256 `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`
- package identity `FM-1_987`.

Historical default WAV, D031 locked WAV, all frozen D032/D033 fixtures and all D034 queue fixtures remain byte-identical through WP3.9.

### WP3.10 - Phase 3 worst-case tests and exit gate - COMPLETE - #29

Final closeout workflow checkpoint:

`e90d0232c179ea92e2295171403697ceaf5e3ecf`

Final GitHub Actions:

- run `40084706845`
- job `120845144453` (`phase3-closeout`)
- result **SUCCESS**

Evidence: `docs/WP3_10_VERIFICATION.md`
Normative test plan: `docs/WP3_10_TEST_PLAN.md`

Evidence artifact:

- ID `12556590742`
- ZIP SHA-256 `c4f494716ec98507ca9af6fe59313c01116136deececa7ba84e24598e78b82ee`

D036 found no production defect. It proved the complete D027-D035 stack under full lock-pool pressure, all-row cache pressure, repeated queue full/wrap/overflow recovery, combined playback/audition/editor/UI stress, all D032/D033 byte values, 4,096 rows / 64 wraps, the retained six-hour transport proof and a fail-fast static realtime audit.

Key exit markers:

- `WP3_10_LOCK_POOL_STRESS_OK=1 records=2048 owners=512 hash=4f426945`
- `WP3_10_QUEUE_STRESS_OK=1 cycles=2048 rejected=6144 final_write=65519 final_read=65519 overflow=5143`
- `WP3_10_COMBINED_STRESS_OK=1 deterministic=1 two_overflow_epochs=1 recovery=1 midi_neutral=1 screen_queue_neutral=1`
- `WP3_10_TIMING_MATRIX_OK=1 d032_commands=5 values_each=256 d033_commands=3 values_each=256 no_tick6=1 hash=364d0c2c`
- `WP3_10_REALTIME_AUDIT_OK=1`
- `WP3_10_PHASE3_EXIT_OK=1 wp3_1_through_wp3_10=complete host_device_build=1 physical_hardware_pending=1`

WP3.10 changes no production firmware source. Final Phase 3 resources therefore remain:

| Resource | Used | Limit | Free | Delta vs WP3.9 |
|---|---:|---:|---:|---:|
| app image | 66,960 B | 581,564 B | 514,604 B | +0 B |
| normal RAM | 13,296 B | 98,304 B | 85,008 B | +0 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B | +0 B |

Final firmware artifacts:

- `fm1tracker.bin` SHA-256 `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27`
- `fm1tracker-0.1.fwsc` SHA-256 `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`
- package identity `FM-1_987`.

## Next milestone

**WP4.2 - PULSE oscillator**

State: **IN PROGRESS - WP4.1/D037 COMPLETE; D038 FROZEN; focused PULSE implementation/tests next; no full-app engine integration**

Phase 4 parent issue: #30
WP4.1 issue: #31 — COMPLETE
WP4.2 issue: #32 — IN PROGRESS
Branch: `phase4/8bit-engine`

WP4.1 closed on run `40087703654`, job `120853898352`, with zero production delta from the Phase 4 branch start and the full inherited Phase 3 target gate green. Evidence: `docs/WP4_1_VERIFICATION.md`.

D038 is frozen in `docs/WP4_2_PULSE_CONTRACT.md`: 4-byte phase-only state, exact existing Q7 pitch integers, sample-before-advance ordering, pulse width 32..224, hard-edge/band-unlimited output, focused deterministic vectors and zero full-app delta while isolated.

The Phase 3 proof sine remains the full-app production audio implementation during isolated WP4.2 primitive work. Do not pull NOISE/TRIANGLE/WAVETABLE-8, envelope, final allocator, parameter pages or full engine integration forward.

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
- D034: runtime ingress is an exact 32-entry / 268-byte SPSC main->audio queue with 8-byte events, reject-newest overflow and fail-to-silence audition recovery; D024/D033 remain outside it.
- D035: tracker presentation remains ten regions / max two redraws, `FmtTrackerScreen` 62 B, with bounded lock-count and stored-effect feedback only.
- D036: the complete D027-D035 stack passed worst-case combined verification with zero production delta; Phase 3 is complete at host/device-build level.
- D037: Phase 4 engine ownership, eight-voice cap, Q7 pitch domain, event/effect adapter boundary, Q15/32-bit mix rules, realtime bounds and complete Phase 4 resource ceilings are frozen before oscillator source.
- Preserve the historical default WAV unless a later deliberate decision supersedes it with explicit evidence.
- No float in firmware DSP and no runtime 64-bit division.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted future core direction. Phase 3 is the bounded tracker/p-lock/effect/event foundation for that milestone.

## Hardware limitation still open

No physical FM-1 is currently available. Host/device-build evidence does not claim physical effect/event feel, install/boot/audio behavior, physical sound quality or measured AC791N execution margin.

## Exact next action

Freeze D038 and the focused WP4.2 PULSE test contract in the repository before creating PULSE production source. Keep the Phase 3 proof sine as full-app production audio while the isolated primitive is implemented and verified.

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
- WP3.8 verified: `5c2828921f30c7d299cbc0e7aa6b7cfa9f4b3018`
- WP3.9 verified production: `354bf7d356774ad828f1f660bd456dbab7f0490d`
- WP3.10 / Phase 3 closeout workflow: `e90d0232c179ea92e2295171403697ceaf5e3ecf`
- WP4.1 D037 contract: `a740da66a0587b4e257629d7aba12781fad73f73`
- WP4.1 closeout workflow: `01138c5f8a420efcc8e438ff3c2eb4090c086586`

## Last updated

2026-10-24 - WP4.2 / #32 active. D038 is now frozen in `WP4_2_PULSE_CONTRACT.md` before PULSE source. Exact next action is isolated PULSE/pitch implementation plus deterministic host/PI32v2 verification with the full-app proof sine unchanged.
