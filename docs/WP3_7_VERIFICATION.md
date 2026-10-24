# WP3.7 Verification

Status: **COMPLETE**  
Date: 2026-10-24  
Package: WP3.7 / issue #26  
Branch: `phase3/plocks-effects`

## Checkpoints

WP3.7 start head:

`e2f354018b4cbad636b51ac8ae98c9b226e7ef18`

Production audio-lane integration commit:

`9ab3646a8b7ee36027d22855bd7bade61b3e46cf`

Final clean-tree verified source/workflow checkpoint:

`d2a97798149816957745de818e4568c518a9a7e7`

Final GitHub Actions:

- run `40077809743`
- job `120824870575` (`tracker-events-closeout`)
- result **SUCCESS**

Evidence artifact:

- name `wp3-7-tracker-events-evidence`
- ID `12553932464`
- size `2,960,902` B
- ZIP SHA-256 `5fca527df35446a879934b91d35d22acf0a0d7bc09ef1eb93e8e57fa25c9d332`

## Architecture freeze

D033 was accepted before production audio integration. The normative detailed contract is `docs/WP3_7_EVENT_CONTRACT.md`.

WP3.7 extends the existing `FmtStep.effect/effect_value` command domain:

- `0x06` RETRIG
- `0x07` NOTE DELAY
- `0x08` NOTE CUT
- values `0x09..0xFF` remain safe NONE/fallback commands.

`FmtStep` remains exactly 8 B. `FmtPlaybackEvent` remains exactly 4 B.

No general event queue was introduced. D033 uses one fixed 4-byte `FmtTrackerRowEventState` containing bounded row-local pending state only.

## Exact D020/D032 timing

WP3.7 reuses the already frozen six-tick D032 row grid exactly:

- row phase = `661500`
- ticks per row = `6`
- tick phase = `110250`
- tick 0 begins at row dispatch
- internal thresholds: `110250`, `220500`, `330750`, `441000`, `551250`
- row boundary at `661500` wins; there is no synthetic tick 6.

At 120 BPM the verified frames remain:

- tick 1: frame `919`
- tick 2: frame `1838`
- tick 3: frame `2757`
- tick 4: frame `3675`
- tick 5: frame `4594`
- next row: frame `5513`

## Event semantics

### RETRIG

- value `0`: no retrigger;
- values `1..5`: retrigger every matching nonzero six-tick interval;
- values `>5`: scheduling no-op;
- requires a logically active note;
- retrigger resets proof-oscillator phase only and preserves gate/level, persistent pitch, D031 FINE state and any D032 portamento target.

### NOTE DELAY

- value `0`: ordinary NOTE dispatch remains immediate;
- values `1..5`: an ordinary NOTE is withheld until that exact D032 tick;
- values `>5`: scheduling no-op and the ordinary NOTE remains immediate;
- TIE, EMPTY and OFF are never delayed;
- the delayed NOTE uses the exact D031 row snapshot captured at row dispatch rather than re-reading mutable main-thread state mid-row;
- pending delay state never survives the row boundary.

### NOTE CUT

- values `0..5`: synthetic D025 OFF semantics at that exact tick;
- value `0`: row event is applied first, then CUT before the first rendered sample;
- values `>5`: scheduling no-op;
- cut clears logical note-active state and D032 continuation state and cannot be resurrected by later ticks in the row.

## D031 / D032 / D033 ordering

At row dispatch audio reads exactly one D031 32-bit parameter word and one D032/D033 32-bit effect word.

D033 first resolves whether the ordinary row event is immediate or, for a valid NOTE DELAY, withheld. D032 then begins the row from that resolved immediate event. Tick-0 CUT is applied after ordinary row dispatch but before the first rendered frame.

On nonzero ticks the deterministic order is:

1. advance the existing D032 effect tick;
2. evaluate at most one fixed-state D033 row action;
3. apply delayed NOTE / NOTE CUT / RETRIG action if due;
4. apply the resulting D032 pitch state for the next rendered frame.

No allocator, D028 lock traversal, variable event loop, general queue, float or runtime 64-bit division was added to the audio path.

## Focused tests

Scheduler/unit proof:

`FMT_TRACKER_EVENT_TESTS_OK=1`

with:

`state_bytes=4 delay_ticks=0..5 cut_ticks=0..5 retrig_intervals=0..5 invalid_value_noop=1 no_tick6=1 no_queue=1`

Full-app audio proof:

`WP3_7_EVENT_AUDIO_BEHAVIOR_OK=1 delay=1 cut=1 cut0=1 retrig=1 invalid_noop=1 midi_events=0`

Measured representative timing:

- NOTE DELAY 3 first nonzero sample: frame `2757`
- NOTE CUT 3 first difference from control: frame `2757`
- RETRIG 2 first difference from control: frame `1838`

All `>5` scheduling values tested for NOTE DELAY, NOTE CUT and RETRIG are byte-identical to the control render:

`WP3_7_INVALID_VALUE_AUDIO_IDENTICAL=1`

## Frozen audio evidence

Historical default WAV remains byte-identical:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

D031 locked WAV remains byte-identical:

`8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8`

All WP3.6 deterministic D032 fixture hashes remain byte-identical at the final WP3.7 gate.

WP3.7 deterministic fixture WAVs, 39,680 frames each:

- control: `8933b4ae192e314dbaa729b929ebfc0195178842ccd9e89306deffd71a9db1b6`
- NOTE DELAY 3: `17ed9ad4ba6fa29ce756fe5e6fff28dde5af93f95a00f8438b8b4b334cf4bafb`
- NOTE CUT 3: `89242c5a668ef6deda748799e1ceb571f9fcd702060331bb9bf5b295426c14ba`
- NOTE CUT 0: `705ff805fe19884d3bb2b40480a77d0c39a4e7f30b090921d9fd44a92a733473`
- RETRIG 2: `98e223b6c120480a9c761099c4de8c987c9db6b90565b04b5faf12f859c2d938`
- NOTE DELAY 6: `8933b4ae192e314dbaa729b929ebfc0195178842ccd9e89306deffd71a9db1b6`
- NOTE CUT 6: `8933b4ae192e314dbaa729b929ebfc0195178842ccd9e89306deffd71a9db1b6`
- RETRIG 6: `8933b4ae192e314dbaa729b929ebfc0195178842ccd9e89306deffd71a9db1b6`

## Inherited regressions

The final clean-tree gate re-ran:

- D033 scheduler/unit tests;
- D033 full-app audio timing tests;
- D032 unit tests and exact six-tick grid;
- every frozen D032 full-app audio hash;
- complete Phase 2 and WP3.1-WP3.6 inherited closeout suite;
- historical default WAV;
- D031 locked WAV;
- package identity, HAL/register safety and device-build checks.

All passed.

## JieLi / device evidence

Toolchain:

- `jieli-linux-toolchains-20260730.1`
- JieLi clang target `pi32v2`

AC79 SDK basis remains:

- tag `AC79NN_SDK_V1.2.1_2023-12-13`
- commit `d179b4484759423312073f5fbb232501aa491047`

`fmt_tracker_event.c` compiled separately with the real PI32v2 compiler:

`WP3_7_JIELI_EVENT_COMPILE_OK=1`

Final linked symbols include:

```text
01c0b074 .bss 00000100 fmt_tracker_effect_cache
01c0b174 .bss 00000004 fmt_tracker_row_event_runtime
01c0b178 .bss 00000100 fmt_playback_param_cache
01c0b294 .bss 0000000c fmt_tracker_effect_runtime
```

Loader / safety remained green:

- loader image 8,232 B at `0x01c0a800`
- loader `ota.bin` 6,954 B
- `.ram_text`: 901 instructions, no calls
- register access: `hal/` only
- package identity `FM-1_987`.

## Final resources

| Resource | WP3.6 | WP3.7 | Delta | Limit | Free |
|---|---:|---:|---:|---:|---:|
| app image | 65,528 B | 65,940 B | +412 B | 581,564 B | 515,624 B |
| normal RAM | 13,004 B | 13,008 B | +4 B | 98,304 B | 85,296 B |
| pool RAM | 113,000 B | 113,000 B | +0 B | 344,064 B | 231,064 B |

The +4 B normal-RAM delta is exactly `FmtTrackerRowEventState`. No new pool allocation was introduced.

## Final firmware artifacts

- `fm1tracker.bin` SHA-256 `f71bc5770ce2b56bcbc3fb4034b882f1a80f0b1997cdd4534cc7b8dd45100e2b`
- `fm1tracker-0.1.fwsc` SHA-256 `cd4b3138ecb6e4e853b67cbe4b8a049e43290798dad26accf606279dc69e383d`
- package identity `FM-1_987`.

## Scope preserved

WP3.7 did **not** implement:

- a general bounded event queue or overflow policy;
- full lock/effect visualization;
- a synth engine;
- persistence;
- live performance capture;
- final hardware control-map freeze.

Those boundaries remain intact for later work packages.

## Hardware limitation

No physical FM-1 is currently available. Host/device-build evidence does not claim physical retrig/delay/cut feel, install/boot/audio behavior, physical sound quality or measured AC791N execution margin.

## Handoff

WP3.8 is next: bounded event queue and overflow policy. Its architecture must be frozen before production source. D033 proved that WP3.7 itself did not require the general queue, so WP3.8 begins from a clean fixed-state baseline rather than retrofitting queue assumptions into D033.
