# WP2.9 Verification — compact tracker screen

Date: 2026-10-23  
Status: **COMPLETE / VERIFIED**

## Scope

WP2.9 adds the first compact tracker display to the FM-1 240×240 screen. It is deliberately presentation-only: no tracker model, store, transport, playback, STEP REC, synth, p-lock, persistence, HAL or loader semantics are expanded here.

Detailed screen contract: `docs/WP2_9_SCREEN_CONTRACT.md`  
Decision: D026 in `docs/DECISIONS.md`

## Checkpoints

WP2.9 start head:

`5c2830cf2d37c8466f8564aab9a1bd89feaad38a`

Final verified implementation/workflow commit:

`e53d785b811371cf817d22be1fc8631b1e12cd12`

GitHub Actions:

- run: `40009689758`
- job: `120606559644` (`tracker-screen`)
- result: **SUCCESS**

An earlier model-only gate also passed before renderer integration:

- run: `40008827313`
- job: `120603588405`
- result: **SUCCESS**

This established that viewport/formatting/dirty-state logic was green independently before LCD integration.

## D026 implementation

### Presentation model

New files:

- `firmware/src/fmt_tracker_screen.h`
- `firmware/src/fmt_tracker_screen.c`

`FmtTrackerScreen` is exactly **44 bytes** and lives in ordinary `.bss`. It contains only bounded presentation/cache state:

- 10-bit persistent dirty mask;
- current BPM/context/track/page/window/cursor/playhead state;
- cached note/instrument/volume/effect bytes for exactly eight visible rows.

Compile-time contract:

- 8 visible tracker rows;
- 10 redraw regions total;
- maximum 2 tracker regions rendered per main-loop poll;
- presentation state <=64 bytes.

The presentation module reads pattern 0 through the existing bounds-safe const store accessor. Its tests snapshot the complete `FmtSong` and prove synchronization does not mutate song data.

### Cursor-owned viewport

The current 16-step D022 page is split into two fixed eight-row halves. The edit cursor chooses the half; playback does not scroll it.

Possible windows are exactly:

```text
01-08
09-16
17-24
25-32
33-40
41-48
49-56
57-64
```

All page/local-step combinations were tested and the selected step is always inside the derived window.

### Explicit cursor/playhead semantics

Rows use independent marker cells:

- `>` = edit cursor;
- `*` = running playhead;
- `>*` = both on the same row.

If the playhead is outside the current eight-row window it is still shown exactly as `PLYnn` in the header. A stopped transport displays `STOP` and no playhead row marker.

### Tracker text semantics

D026 formatting is deterministic:

- ordinary notes: tracker pitch notation such as `C-4` / `C#4`;
- MIDI octave -1 remains explicit, e.g. `C--1`;
- EMPTY / REST: `---`;
- TIE: `===`;
- NOTE OFF: `OFF`;
- malformed/reserved note: `???`;
- inherited instrument: `..`;
- explicit instrument 0..15 -> UI `01..16`;
- inherited volume: `...`;
- explicit volume -> decimal `000..254`;
- effect NONE: `---`;
- nonzero effect: `???` until Phase 3 freezes stable effect semantics.

### Screen ownership and redraw bound

`firmware/src/dinghy.c` now includes the tracker presentation module and owns the renderer in the main loop only.

The 240×240 tracker surface is partitioned into exactly ten non-overlapping regions:

- one 240×64 header;
- eight 240×18 row strips;
- one 240×32 footer.

No tracker drawing function exists in the audio callback. CI compares the entire `/* ---- the sound */` section of `dinghy.c` against the WP2.8 start head and proves it is byte-identical:

```text
WP2_9_AUDIO_SECTION_UNCHANGED=1
```

The main loop drains at most two pending tracker regions per poll. Full view changes may mark all ten dirty, but the dirty bits persist until consumed.

Returning from tracker context to HOME forces a clean HOME redraw. A dedicated host A/B fixture proves that `SEQ -> HOME` at 3000 ms is pixel-identical to a control run that never left HOME:

```text
WP2_9_HOME_RETURN_PIXEL_IDENTICAL=1
```

## Host screen evidence

Representative tracker screenshots were generated from the real unchanged host application path and visually inspected before their raw framebuffer hashes were frozen.

Reviewed SEQ framebuffer:

`e67d0cf9aa2fa391464237a40ed795bb6ba881e15aff40340e1b92a4a8eccb7a`

Reviewed STEP REC framebuffer:

`4b0192b084e6ee483d7a8f98add2ae95229dcc914583d3cbe957944824cb6424`

The STEP REC screen visibly distinguishes:

- `STEP REC` mode;
- selected cursor row;
- REST `---`;
- TIE `===`;
- NOTE OFF `OFF`;
- D025 footer help `FX RST  SEL TIE  FX+SEL OFF`.

PNG render hashes from the verified run, retained as human-viewable evidence:

- SEQ: `6ff7eee04f9e7ee911e6645b08fd1448ed88d956bde84099c61f0646089f3d10`
- STEP REC: `9233d19650a0c35956f4454c877e3889fcea4f628038a92a23f4390eb897d6cc`
- HOME return: `2e76ff62c81e9acfbcdc8263eaf77240ba88815d0d1edbbcf1c2c455a2b7de24`

The CI freeze uses raw PPM hashes rather than PNG hashes so image-encoder changes cannot create false visual regressions.

## Regression evidence

All inherited Phase 2 host suites are green at the verified source checkpoint:

```text
FMT_MODEL_TESTS_OK=1
FMT_STORE_TESTS_OK=1
FMT_TRANSPORT_TESTS_OK=1
FMT_PLAYBACK_TESTS_OK=1
FMT_PATTERN_INPUT_TESTS_OK=1
FMT_STEP_RECORD_TESTS_OK=1
FMT_TRACKER_SCREEN_TESTS_OK=1
WP2_5_PATTERN_CONTEXT_HOST_OK=1
WP2_6_TRACK_CONTEXT_HOST_OK=1
WP2_7_STEP_REC_CONTEXT_HOST_OK=1
WP2_8_OFF_AUDIO_BEHAVIOR_OK=1 before=4410 quiet=0 after=4410
WP2_8_SPECIAL_EVENTS_CONTEXT_HOST_OK=1
WP2_9_HOME_RETURN_PIXEL_IDENTICAL=1
WP2_9_REVIEWED_SCREEN_HASHES_OK=1
WP2_9_TRACKER_SCREEN_HOST_OK=1
```

The frozen default WP2.4 host WAV remains byte-identical:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

Host frames remain 286,464 for the default fixture.

## Device build evidence

Pinned JieLi environment remains unchanged:

- toolchain: `jieli-linux-toolchains-20260730.1`
- clang: `4.0.1`
- AC79 SDK tag: `AC79NN_SDK_V1.2.1_2023-12-13`
- AC79 SDK commit: `d179b4484759423312073f5fbb232501aa491047`

Inherited safety checks remain green:

- loader image: 8,232 B at `0x01C0A800`;
- OTA image: 6,954 B;
- `.ram_text`: 901 instructions, no calls;
- direct register access remains HAL-only;
- package identity remains `FM-1_987`.

## Linked runtime symbols

```text
01c0b070 g .bss  00000003 fmt_step_record_runtime
01c0b073 g .bss  00000004 fmt_pattern_input_runtime
01c0b078 g .bss  00000008 fmt_transport_runtime
01c0b080 g .bss  0000002c fmt_tracker_screen_runtime
01c0b0ac g .bss  00000001 fmt_playback_runtime
01c20000 g .pool 00010168 fmt_store_storage
```

The tracker screen cache is therefore exactly 44 B in ordinary RAM and the 65,896 B song store remains unchanged at `.pool` `0x01C20000`.

## Resource delta vs WP2.8

| Resource | WP2.8 | WP2.9 | Delta | Limit | Remaining |
|---|---:|---:|---:|---:|---:|
| app image | 56,748 B | 59,252 B | **+2,504 B** | 581,564 B | 522,312 B |
| normal RAM | 12,420 B | 12,464 B | **+44 B** | 98,304 B | 85,840 B |
| pool RAM | 96,616 B | 96,616 B | **+0 B** | 344,064 B | 247,448 B |

The normal-RAM delta is exactly the 44-byte `FmtTrackerScreen` object. WP2.9 adds no pool allocation.

## Final verified artifacts

`fm1tracker.bin` SHA-256:

`34d0e8c4344e091704608e08357964e7ae0b18673dd0b93f36c4e5bcc2cb4c7c`

`fm1tracker-0.1.fwsc` SHA-256:

`3cfd0a2da3836720d2f921b04f29e93c16ca83ab18ab7242c99f2c333d890ca7`

Host WAV SHA-256:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

Evidence artifact:

- name: `wp2-9-tracker-screen-evidence`
- artifact ID: `12525582378`
- ZIP size: 1,123,329 B
- ZIP SHA-256: `6c23c962c96de458d5d328e60a3b5cbd224e2011933b2d812ee70d8fe6c8daa8`

## Scope integrity

Diff from WP2.9 start head to the verified checkpoint contains only:

- the D026 decision/status documentation;
- new tracker-screen presentation module;
- bounded renderer integration in `dinghy.c`;
- focused tracker-screen unit/host fixtures;
- the WP2.9 verification workflow.

The existing model/store/transport/playback/pattern-input/STEP-REC modules, platform, HAL, loader and build tools are protected by CI and remained unchanged.

## Not verified on physical hardware

No physical FM-1 is currently available. Therefore WP2.9 does **not** claim:

- physical LCD readability under real viewing conditions;
- physical screen update latency/subjective smoothness;
- actual main-loop render execution time on AC791N hardware;
- physical install/boot/audio/panel behavior.

These remain Phase 7/hardware verification items. Host framebuffer output, full firmware linking, memory placement and inherited safety are verified.

## Exit result

WP2.9 exit criteria are satisfied at the host/firmware-build level. The first compact tracker screen is implemented, deterministic, bounded, read-only with respect to song data, isolated from the audio ISR, pixel-regression protected and within the frozen resource budgets.

Do not extend this checkpoint into WP2.10 or Phase 3. The next package is the dedicated Phase 2 integration test/closeout.