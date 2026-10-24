# WP3.5 Verification — basic pitch/volume p-lock playback

Date: 2026-10-24  
Status: **COMPLETE**

GitHub issue: #24  
Phase 3 parent: #20  
Branch: `phase3/plocks-effects`

## Checkpoints

WP3.5 start head:

`01b35603ec387e079e4af512f8a8109e413d2714`

Final verified source/workflow checkpoint:

`17316f203dba7a93c9d03a52df6f45b12a7a2580`

Final GitHub Actions verification:

- run `40069110111`
- job `120798506694` (`playback-closeout`)
- result **SUCCESS**

The preceding measurement run was `40068853937`; the final run above repeats the complete gate with exact measured resource, symbol and artifact assertions frozen.

Evidence artifact:

- name `wp3-5-playback-evidence`
- ID `12550920281`
- size 1,413,299 B
- ZIP SHA-256 `a6e92feef5639f69619afa458e18b626bd717e9c8d20b483fc414921a7f5cbcd`
- retention expires 2027-01-22 according to GitHub Actions metadata.

## Frozen decision D031

D031 was recorded before playback/audio source implementation. WP3.5 keeps the existing D021 proof lane fixed to pattern 0 / logical track 0 and makes exactly three D027 parameters audible:

- `FMT_PARAM_VOICE_LEVEL` (`0x01`)
- `FMT_PARAM_PITCH_FINE` (`0x02`)
- `FMT_PARAM_TRACK_LEVEL` (`0xA0`)

`FMT_PARAM_TRACK_PAN` (`0xA1`) remains editable/storable/browsable/deletable but storage-only in WP3.5.

The row-local playback defaults are:

- VOICE LEVEL = 255
- FINE PITCH = 128
- TRACK LEVEL = 255

A lock affects only its owning row. The next row without that lock restores the explicit playback default; lock values never silently persist as base instrument/track state.

## Level mapping

VOICE LEVEL and TRACK LEVEL use the normalized `0..255` editor domain.

The proof base target remains `6000`, with deterministic composition:

```text
gate = scale_u8(scale_u8(6000, voice_level), track_level)
scale_u8(x, v) = (x * v + 127) / 255
```

This uses bounded 32-bit integer arithmetic. `255` is exact unity relative to the proof target and `0` is exact zero. WP3.5 does not claim a dB law.

## Fine pitch mapping

FINE PITCH is a signed Q7 semitone offset centered at 128:

```text
offset_q7 = fine_value - 128
pitch_q7 = clamp(note * 128 + offset_q7, 0, 16383)
```

Therefore:

- 0 = -1.000 semitone
- 128 = 0 semitones
- 255 = +127/128 semitone

Integer semitone increments still come from the historical `note_inc()` table. Fractional Q7 pitch uses deterministic linear interpolation between adjacent semitone phase increments with integer arithmetic. Fine=128 produces the historical increment exactly. No floating point or runtime 64-bit division was added.

## NOTE / TIE / EMPTY / OFF behavior

D031 composes row parameters with D025 note-event semantics:

- NOTE stores the new base note, marks the proof voice logically active and applies that row's pitch/level snapshot.
- TIE preserves the base note and phase; if logically active, that row may change pitch/level without retrigger.
- EMPTY/NONE emits no note event, but a logically active note still receives that row's p-lock snapshot. This permits note-empty modulation rows and deterministic restore on an unlocked row.
- OFF clears logical active state and targets gate zero. It does not retune/reopen the release tail, and later TIE/EMPTY rows cannot resurrect it.
- A level lock of zero is a row-local mute, not NOTE OFF; logical note state remains active and a later unlocked row can restore the level.

## Main-to-audio ownership

The audio ISR never traverses D028's mutable linked list.

WP3.5 adds an aligned 64-row cache:

```text
volatile uint32_t fmt_playback_param_cache[64]
```

Each 32-bit row word contains:

- bits 0..7: VOICE LEVEL
- bits 8..15: FINE PITCH
- bits 16..23: TRACK LEVEL
- bits 24..31: resolver diagnostic/fallback flags

The cache is exactly **256 B** ordinary RAM and is 4-byte aligned. Main/editor-side refresh validates at most the four D028 records allowed on one step and publishes one complete 32-bit word. The audio side performs one bounded row-word read at D021 dispatch and never receives a lock-list pointer.

A structurally corrupt proof row publishes safe complete defaults with a structural-fallback flag. A structurally valid accepted lock above 255 falls back only that parameter to its default and sets a domain-fallback flag; other valid accepted locks remain usable. TRACK PAN is ignored for audio.

Edits to pattern 0 / track 0 refresh only the affected row snapshot in main context. Edits to tracks 1..7 cannot alter the proof-lane cache.

`FmtPlaybackEvent` remains exactly **4 B** and D021 timing is unchanged: step 0 before rendered frame 0; current frame renders before transport tick; a crossed row becomes active on the next rendered frame.

## Focused unit verification

Frozen marker:

```text
FMT_PLAYBACK_PARAMS_TESTS_OK=1
cache_bytes=256 rows=64 word_bytes=4 defaults=255,128,255 active_params=voice,fine,track pan=storage_only
row_local=1 domain_fallback=1 structural_fallback=1 no_store_mutation=1 voice_state_bytes=6
```

Coverage includes:

- exact aligned 256-byte cache;
- all 64 proof rows;
- accepted three parameters and ignored TRACK PAN;
- default packing/readback;
- domain fallback;
- structural corruption fallback;
- no mutation of `FmtSong` or lock pool during resolution/playback-state work;
- integer level composition;
- FINE PITCH endpoints/center/interpolation;
- NOTE/TIE/EMPTY/OFF logical-active semantics;
- row-local zero-level mute and later restore.

## Default audio regression

The no-lock proof pattern remains byte-identical to the historical WP2.4+ reference:

- frames: 286,464
- WAV SHA-256: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

This proves the D031 default snapshot `255/128/255` preserves the old pitch and target level exactly.

## Locked audio proof

The real full-app D029 panel path creates locks while the proof transport runs:

- row 5: VOICE 128, FINE 192, TRACK 128
- row 6: VOICE 0 on an EMPTY row
- row 7: unlocked EMPTY row

A timing-matched no-lock control is rendered in parallel.

Frozen artifacts:

- control WAV SHA-256 `5545ea2322ff2f01b77bce0c94ed8ae379386eec1baea0f712b6daf000baf3fd`
- locked WAV SHA-256 `8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8`
- frames: 52,736 each

Frozen behavior marker:

```text
WP3_5_LOCKED_AUDIO_BEHAVIOR_OK=1 pre_equal=1 combined_level=1 fine_pitch=1 empty_mute=1 unlocked_restore=1 midi_events=0
```

Measured proof details:

- control and locked renders are sample-identical before row-5 boundary frame 22,050;
- row-5 measurement window absolute-sample sum: control 9,593,862 vs locked 2,417,778;
- zero crossings in the same window: control 37 vs locked 38, proving the +0.5-semitone FINE shift changes frequency;
- the row-6 EMPTY + VOICE=0 region reaches exact silence after the bounded release ramp;
- row 7 is unlocked and becomes audible again without a new NOTE, proving row-local default restoration;
- ordinary SEQ editing in this fixture emits zero MIDI audition events.

## Full regression gate

The final run preserves:

- D027 parameter-registry tests;
- D028 lock storage, owner/corruption, exhaustion and four-lock limits;
- D029/D030 lock editor and browse/delete tests;
- all Phase 2 model/store/transport/playback/input/STEP REC/screen tests;
- all historical WP2.5-WP2.10 full-app fixtures;
- frozen WP3.3 lock-editor framebuffers and panel behavior;
- frozen WP3.4 armed/deleted framebuffers and destructive-action safety;
- the historical default WAV.

The new D031 module also compiles independently with the real JieLi PI32v2 compiler.

## Device build / toolchain evidence

- JieLi package: `jieli-linux-toolchains-20260730.1`
- clang: 4.0.1
- AC79 SDK tag: `AC79NN_SDK_V1.2.1_2023-12-13`
- AC79 SDK commit: `d179b4484759423312073f5fbb232501aa491047`
- loader image: 8,232 B at `0x01C0A800`
- loader BSS end: `0x01C0F400`
- OTA loader: 6,954 B
- `.ram_text`: 901 instructions, no calls
- register access: HAL only; `src/` and `loader/` clean
- package identity: `FM-1_987`

Final relevant linked symbols:

```text
01c0b074 .bss 00000100 fmt_playback_param_cache
01c0b178 .bss 0000000a fmt_lock_editor_runtime
01c0b18c .bss 00000004 fmt_lock_store_runtime
01c20000 .pool 00010168 fmt_store_storage
01c30168 .pool 00004000 fmt_lock_store_storage
```

## Final resources

| Resource | Used | Limit | Free | Delta vs WP3.4 |
|---|---:|---:|---:|---:|
| app image | 64,724 B | 581,564 B | 516,840 B | +496 B |
| normal RAM | 12,736 B | 98,304 B | 85,568 B | +256 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B | +0 B |

The normal-RAM delta is exactly the 256-byte row snapshot cache. D031 adds no pool allocation.

## Final firmware artifacts

- `fm1tracker.bin` SHA-256 `d0380ac6cd2ee0e5edb1eab3e4d3937bb62eb074d27219927b3e8b7e52a244ae`
- `fm1tracker-0.1.fwsc` SHA-256 `5e14f28985fcda4c3f54dbe8a45ff020aba65a565696972fde30d4fa366d3372`

## Scope result

WP3.5 did **not** add:

- arpeggio, slide, portamento or vibrato;
- retrigger, note delay or note cut;
- a general event queue;
- TRACK PAN playback law;
- new parameter IDs;
- synth engine implementation;
- persistence or live-capture semantics.

Those remain owned by later work packages.

## Remaining limitation

No physical FM-1 is available. This verification proves host behavior, real target compilation/linking, resource placement and deterministic artifacts, but does not claim physical audio quality, panel feel, install/boot behavior or measured AC791N callback execution margin.
