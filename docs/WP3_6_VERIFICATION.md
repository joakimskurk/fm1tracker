# WP3.6 Verification

Status: **COMPLETE**  
Date: 2026-10-24  
Package: WP3.6 / issue #25  
Branch: `phase3/plocks-effects`

## Checkpoints

WP3.6 start head:

`44f4a6573c8729735a82283294fcf4837cf70b24`

Final verified source/workflow checkpoint:

`d1910cb86b84240051cedfabb7701140259d8274`

Final GitHub Actions:

- run `40073680548`
- job `120812368036` (`tracker-effects-closeout`)
- result **SUCCESS**

Evidence artifact:

- name `wp3-6-tracker-effects-evidence`
- ID `12552566558`
- size `2,150,850` B
- ZIP SHA-256 `3ce38fec8f73f1f2a842cc335a45a4a95be7bd60e1edba65cc89654a9cd4a3de`

## Architecture freeze

D032 was accepted before WP3.6 production source. The normative detailed contract is `docs/WP3_6_EFFECT_CONTRACT.md`.

Tracker effects remain separate from D027 parameter-lock IDs and use the existing `FmtStep.effect/effect_value` bytes:

- `0x00` NONE
- `0x01` ARPEGGIO
- `0x02` SLIDE_UP
- `0x03` SLIDE_DOWN
- `0x04` PORTAMENTO
- `0x05` VIBRATO
- all other command bytes resolve safely to NONE.

`FmtStep` remains exactly 8 B. `FmtPlaybackEvent` remains exactly 4 B.

## Exact D020/D021 timing

WP3.6 derives six effect ticks exactly from the existing transport phase:

- row phase = `661500`
- ticks per row = `6`
- effect tick phase = `110250`
- tick 0 begins at row dispatch
- internal tick thresholds: `110250`, `220500`, `330750`, `441000`, `551250`
- row boundary at `661500` wins over any synthetic extra effect tick.

The 120 BPM frame proof is frozen as:

- tick 1 frame 919
- tick 2 frame 1838
- tick 3 frame 2757
- tick 4 frame 3675
- tick 5 frame 4594
- next-row frame 5513

At the maximum supported BPM, one rendered frame can cross at most one effect-tick threshold.

## Effect semantics

### ARPEGGIO

`XY` uses the high/low nibbles as unsigned whole-semitone offsets and produces the six-tick sequence:

`0, X, Y, 0, X, Y`

Arpeggio is transient and never mutates persistent pitch.

### SLIDE_UP / SLIDE_DOWN

The full argument byte is an unsigned Q7 semitone movement per nonzero effect tick. Movement occurs on ticks 1..5 and saturates to pitch Q7 `0..16383`.

Slide mutates persistent pitch and clears any pending portamento target.

### PORTAMENTO

The full argument byte is maximum Q7 movement per nonzero effect tick toward a remembered target. NOTE+PORTA with an already-active source voice changes the target without jumping current pitch. TIE/EMPTY+PORTA may continue toward an existing target. Reaching the target clears it. NOTE without PORTA, slide commands and OFF clear the target.

### VIBRATO

`XY` uses high nibble X as speed on a 64-phase triangle and low nibble Y as depth. Peak depth is `Y/32` semitone. Vibrato is transient and does not change persistent center pitch.

### D031 precedence

Final Q7 pitch is:

`persistent_pitch_q7 + (D031_FINE - 128) + transient_effect_mod_q7`

clamped to `0..16383`.

D031 VOICE LEVEL / TRACK LEVEL behavior is unchanged. FINE remains row-local and never mutates slide/portamento state.

## NOTE / TIE / EMPTY / OFF

- NOTE without PORTA resets persistent pitch to the new note and clears pending PORTA target.
- NOTE with PORTA follows the source/target rules above.
- TIE and EMPTY preserve persistent pitch; active effects may modulate or move it only while a logical note is active.
- OFF clears logical note-active state, pending portamento target, transient modulation and vibrato continuation.
- Effects cannot resurrect an OFF'd voice.

## Main-to-audio ownership

WP3.6 adds a second fixed proof-lane row cache:

`volatile uint32_t fmt_tracker_effect_cache[64]`

It is exactly 256 B ordinary RAM and naturally 4-byte aligned. Main/startup resolves `FmtStep.effect/effect_value` into the cache. Audio reads exactly one complete 32-bit effect word at row dispatch and never parses song rows dynamically for effect identity.

Effect runtime state is `FmtTrackerEffectState`, exactly 12 B at the verified checkpoint.

Per-frame effect work is bounded to at most one threshold comparison plus fixed integer work when a tick actually occurs. No allocator, D028 traversal, variable event loop, screen/flash work, float or runtime 64-bit division was added to the audio path.

## Focused tests

The final closeout gate passed:

`FMT_TRACKER_EFFECT_TESTS_OK=1`

with:

- state bytes = 12
- Q7 pitch state verified
- D031 FINE precedence verified
- portamento target memory verified
- invalid-command fallback verified
- no stored-data mutation verified.

Tick-grid proof:

`WP3_6_TICK_GRID_TESTS_OK=1`

with exact 120 BPM frames `919,1838,2757,3675,4594`, next row frame `5513`, row-boundary precedence and max-one-tick-per-frame verified.

Full-app audio proof:

`WP3_6_EFFECT_AUDIO_BEHAVIOR_OK=1 arp_ticks=1 slide_up=1 slide_down=1 porta_glide=1 vibrato=1 fine_composition=1 invalid_none=1 midi_events=0`

Invalid/reserved effect command audio is byte-identical to control:

`WP3_6_INVALID_COMMAND_AUDIO_IDENTICAL=1`

## Frozen audio evidence

Historical default WAV remains byte-identical:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

D031 locked-audio WAV remains byte-identical:

`8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8`

WP3.6 deterministic fixture WAVs, 39,680 frames each:

- control: `8933b4ae192e314dbaa729b929ebfc0195178842ccd9e89306deffd71a9db1b6`
- arpeggio: `5e95abbdc6a0a9a65af402b7393b9b4088e968297d239679cd78b3872595a743`
- slide up: `2e205d5d46a6c604fbe6c5976dbf57f252dc4288de70ee69a66b7a6769d4e50e`
- slide down: `9bc32956995fe8ada92c3bf2fde80d6b9f6e3a1cd32e06343a170ed70e1e3edc`
- portamento: `2d84235e03ca33d055baf2b2d0d3a430a1b28d3ff4f0faa913d5556f52f1f7e7`
- vibrato: `e14673fca74449f31783a44168edbf7146b0a1aa73f492dd9460febb7542c4a4`
- arpeggio + D031 fine: `5ab516bc70ca571e4dfa43330075e08c389d1fb5ae1a668bc14231f1a50fd950`
- invalid command: `8933b4ae192e314dbaa729b929ebfc0195178842ccd9e89306deffd71a9db1b6`

Representative measured behavior:

- ARP zero-crossing sequence `8,11,13,8,11,13`
- slide-up frequencies `261.625,269.404,277.182,285.423,293.665,302.396`
- slide-down frequencies `261.625,254.284,246.943,240.010,233.082,226.541`
- portamento frequencies `261.624,277.182,293.666,311.127,329.628,329.628`
- D031-fine + ARP ratios `1.058994,1.334251,1.586702`
- vibrato differs deterministically from control.

## Inherited regressions

The final gate re-ran the complete Phase 2 and WP3.1-WP3.5 inheritance, including:

- model/store/transport/playback
- pattern/track/STEP REC/special events
- compact tracker screen and Phase 2 integration
- D027 registry
- D028 sparse lock store
- D029/D030 editor/delete UX
- D031 playback-parameter resolver and audio proof
- historical default WAV
- D031 locked WAV
- package identity and HAL/register safety.

All passed.

## JieLi / device evidence

Toolchain:

- `jieli-linux-toolchains-20260730.1`
- JieLi clang target `pi32v2`

AC79 SDK:

- tag basis `AC79NN_SDK_V1.2.1_2023-12-13`
- commit `d179b4484759423312073f5fbb232501aa491047`

`fmt_tracker_effect.c` compiled separately with the real PI32v2 compiler:

`WP3_6_JIELI_EFFECT_COMPILE_OK=1`

Final linked symbols:

```text
01c0b074 .bss 00000100 fmt_tracker_effect_cache
01c0b174 .bss 00000100 fmt_playback_param_cache
01c0b278 .bss 0000000a fmt_lock_editor_runtime
01c0b290 .bss 0000000c fmt_tracker_effect_runtime
01c0b28c .bss 00000004 fmt_lock_store_runtime
01c20000 .pool 00010168 fmt_store_storage
01c30168 .pool 00004000 fmt_lock_store_storage
```

Loader / safety remained green:

- loader image 8,232 B at `0x01c0a800`
- loader `ota.bin` 6,954 B
- `.ram_text`: 901 instructions, no calls
- register access: `hal/` only
- package identity `FM-1_987`.

## Final resources

| Resource | WP3.5 | WP3.6 | Delta | Limit | Free |
|---|---:|---:|---:|---:|---:|
| app image | 64,724 B | 65,528 B | +804 B | 581,564 B | 516,036 B |
| normal RAM | 12,736 B | 13,004 B | +268 B | 98,304 B | 85,300 B |
| pool RAM | 113,000 B | 113,000 B | +0 B | 344,064 B | 231,064 B |

The +268 B normal-RAM delta is exactly:

- 256 B `fmt_tracker_effect_cache`
- 12 B `fmt_tracker_effect_runtime`.

No new pool allocation was introduced.

## Final firmware artifacts

- `fm1tracker.bin` SHA-256 `01f8dc1fcec3371256dca767458709dcdc48303c833f7a4f9cb214ca4d5f0860`
- `fm1tracker-0.1.fwsc` SHA-256 `ee7d14f32497fd701c3a291a4728fa1f3b56a881dab2dafdf497fb74fb3d5968`
- package identity `FM-1_987`.

## Scope preserved

WP3.6 did **not** implement:

- retrig
- note delay
- note cut
- a general sequencer/audio event queue
- effect-entry UI / full visualization
- a synth engine
- persistence
- live performance capture
- final hardware control-map freeze.

Those boundaries remain intact for later work packages.

## Handoff

WP3.7 is the next package. Before source, freeze its retrig / note-delay / note-cut semantics, exact sub-row encoding/timing, interaction with D032 effects and D025 note events, and whether the existing fixed proof-lane handoff is sufficient or WP3.8 queue work is genuinely required. Do not silently pull the general queue forward.
