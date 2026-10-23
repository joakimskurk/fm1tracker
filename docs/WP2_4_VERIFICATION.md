# WP2.4 Verification — One-Track Audible Playback

Date: 2026-10-23  
Status: **COMPLETE**

## Scope

WP2.4 proves the first audible tracker playback layer without pulling editor/UI/multitrack/synth-engine work forward.

Implemented scope:

- pattern 0 / logical track 0 only;
- one dedicated monophonic proof voice reusing the existing integer sine primitive;
- deterministic 16-note proof fixture, one note every four steps;
- D021 step-0/frame-0 dispatch contract;
- exact D020 frame-boundary row changes inside normal 256-frame audio blocks;
- EMPTY means no new note event and therefore sustains the current proof note;
- TIE/OFF/other special values are explicitly not interpreted as pitches in WP2.4;
- no general event queue;
- no STEP REC/editor/page/track-selection/tracker-screen/p-lock/new-engine/song/persistence work.

## Checkpoints

WP2.4 start head:

`64c4b7d260dca2471d7658a9a59bec23a24454a5`

Final verified implementation/workflow head:

`c8555ce8b68f5813e785b057386110cb177fb9d0`

Final GitHub Actions verification:

- run: `39991490860`
- job: `120544086455` (`one-track-playback`)
- result: **SUCCESS**

The preceding reproducibility run `39991152809` also passed after the audible host hash was frozen. The final run additionally contains the flash-write safety correction described below.

## Decision D021

Playback begins unambiguously:

1. store and proof pattern are initialized in `app_main()`;
2. a one-way `tracker_ready` flag prevents the already-enabled audio IRQ from reading partially initialized tracker state;
3. step 0 is decoded exactly once before playback frame 0;
4. the current tracker state is rendered for one frame;
5. the D020 transport ticks at the end of that frame;
6. a crossed row boundary is decoded immediately and becomes active on the next rendered frame.

At 120 BPM:

- step 1 becomes active on playback frame **5513**;
- step 2 lands with zero phase remainder after another 5512 frames;
- the complete 64-step loop returns `63 -> 0` on frame **352800** exactly.

This avoids quantizing tracker events to the normal 256-frame callback boundary.

## Host functional tests

`tests/test_fmt_playback.c` verifies:

- null/reset safety;
- `FmtPlayback` = 1 B;
- `FmtPlaybackEvent` = 4 B;
- proof fixture seeds exactly pattern 0 / track 0 and does not alias another lane/pattern;
- step 0 emits once before frame 0;
- duplicate start is rejected;
- fractional 120 BPM step timing is exact;
- EMPTY produces no note event;
- TIE/OFF never become accidental raw pitches in this package;
- callback grouping in 256-frame chunks does not alter event timing;
- all expected proof-note frames are exact;
- full 64-step loop is exactly 352800 frames.

Marker:

```text
FMT_PLAYBACK_TESTS_OK=1
pattern=0 track=0 proof_notes=16 spacing=4 loop_frames_120=352800 state_bytes=1 event_bytes=4
```

All inherited suites also pass:

- `FMT_MODEL_TESTS_OK=1`
- `FMT_STORE_TESTS_OK=1`
- `FMT_TRANSPORT_TESTS_OK=1`

## Deterministic audible host proof

The host harness now contains audible tracker output before any simulated manual key press. The first manual host input occurs at 200 ms, while the first 100 ms contains **8,820 non-zero stereo samples**.

Final deterministic host WAV:

- frames: 286,464
- sample rate: 44,100 Hz
- channels: 2
- SHA-256: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

The workflow freezes this exact hash. It differs intentionally from the pre-playback baseline because WP2.4 is the first tracker package allowed to change audio output.

## Flash-write safety correction

The inherited Dinghy octave-setting path erases/writes flash only when `quiet()` reports silence because an erase may stall audio.

WP2.4 introduces a continuously sounding proof lane. The original `quiet()` only inspected the four manual audition voices, which could have allowed `settings_save()` to run while tracker playback was audible.

The final verified implementation therefore counts the tracker proof voice as sounding too. While the temporary always-running WP2.4 proof is active, octave-setting persistence is deferred rather than risking a flash-induced playback glitch. A later package with an explicit stop/safe-save flow can re-enable persistence at a genuinely silent point.

This safety correction did **not** alter the frozen host WAV hash.

## Device build and safety

Final device build passes with:

- package identity: `FM-1_987`;
- loader image: 8,232 B;
- loader `ota.bin`: 6,954 B;
- `.ram_text`: 901 instructions, no calls;
- direct register access remains HAL-only;
- official AC79 SDK remains exact commit `d179b4484759423312073f5fbb232501aa491047` with frozen consumed-input hashes.

Linked runtime symbols:

```text
01c20000 g       .pool        00010168 fmt_store_storage
01c0b060 g       .bss         00000008 fmt_transport_runtime
01c0b068 g       .bss         00000001 fmt_playback_runtime
```

The song store remains exactly 65,896 B at the same `.pool` address.

## Resource delta versus WP2.3

| Resource | WP2.3 | WP2.4 | Delta |
|---|---:|---:|---:|
| app image | 55,328 B | **55,692 B** | **+364 B** |
| normal RAM (`.data + .bss`) | 12,376 B | **12,396 B** | **+20 B** |
| pool RAM | 96,616 B | **96,616 B** | **0 B** |

Remaining physical pool: **247,448 B**.

The small ordinary-RAM increase covers playback/control/proof-voice state. Editable-song pool usage does not increase.

## Final artifacts

- `fm1tracker.bin` SHA-256: `09840601fc6d09f43f27a0f241bc7df371066904e4fa61caaeb33f891e3ed0aa`
- `fm1tracker-0.1.fwsc` SHA-256: `4853dfbb4bbcd0dd08a3a600f7e79a430bd59202e0e5ad022ae73bc68b0147fd`
- host WAV SHA-256: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

Final Actions evidence artifact:

- name: `wp2-4-one-track-playback-evidence`
- artifact ID: `12517493683`
- size: 1,106,447 B
- ZIP SHA-256: `026cbe66f44a066b7f198380081b62f2c10c3aa169330a680103fafe78734e15`

## Verification level

Verified:

- automated host functional/event timing;
- deterministic audible host output;
- full JieLi device cross-build;
- linker placement/resource accounting;
- inherited build/safety gates.

Not verified because no physical FM-1 is currently available:

- actual device install/boot;
- audible playback on the physical DAC/output;
- real callback execution-time headroom;
- physical panel behavior.

WP2.4 is complete at the host/device-build evidence level. WP2.5 may begin only as its own work package.