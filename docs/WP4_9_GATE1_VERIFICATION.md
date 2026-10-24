# WP4.9 Gate 1 Verification — D045 Track-Aware Publication

Date: 2026-10-24  
Package: WP4.9 / issue #40  
Decision: D045  
Branch: `phase4/8bit-engine`  
Normative contract: `docs/WP4_9_LINKED_ENGINE_CONTRACT.md`

## Result

**D045 Gate 1 is GREEN.**

The bounded eight-track playback row event plus track-aware D031 and D032 publication layer is implemented and verified without crossing the linked-engine boundary.

`dinghy.c`, the linked D027 registry, D044 linked cache/base state and D038-D043 engine primitives remain unchanged relative to the D045 freeze checkpoint. The historical track-0 APIs and proof-sine audio remain intact.

## Gate 1 production boundary

D045 decision/status freeze checkpoint:

`53317d36bd6527f7177d6849db606d23ef49c4f0`

Exactly six production files differ from that checkpoint:

- `firmware/src/fmt_playback.h`
- `firmware/src/fmt_playback.c`
- `firmware/src/fmt_playback_params.h`
- `firmware/src/fmt_playback_params.c`
- `firmware/src/fmt_tracker_effect.h`
- `firmware/src/fmt_tracker_effect.c`

No Gate 1 production delta is permitted in:

- `firmware/src/dinghy.c`;
- `fmt_param_registry.*`;
- `fmt_8bit_params.*`;
- D038-D043 oscillator/envelope/allocator primitives.

The permanent runner enforces this exact source boundary on every relevant push.

## Verified behavior

The focused Gate 1 suite proves:

- `FMT_TRACKS == 8`;
- legacy `FmtPlaybackEvent` remains exactly 4 B;
- new `FmtTrackPlaybackEvent` is exactly 4 B;
- row events are published as one fixed `events[8]` array in ascending track order;
- NOTE/TIE/OFF/EMPTY/malformed note classification remains safe and deterministic;
- row 0 dispatch still occurs before frame 0;
- the 120 BPM step-1 boundary still occurs on rendered frame 5513;
- the event captures the row instrument byte without enlarging the event ABI;
- D031 track-0 cache remains exactly 256 B;
- D031 tracks 1..7 add exactly 1792 B and use aligned 32-bit publication words;
- D032 track-0 cache remains exactly 256 B;
- D032 tracks 1..7 add exactly 1792 B and use aligned 32-bit publication words;
- D031 and D032 track-aware reads/refreshes are isolated by `(track,row)`;
- invalid track/row reads fall back safely and invalid refreshes reject;
- historical D031/D032 track-0 helpers remain exact wrappers;
- historical playback, D031 and D032 focused regression suites remain green;
- the proof-sine host WAV remains byte-identical because `dinghy.c` is still untouched.

Focused output:

```text
WP4_9_GATE1_TESTS_OK=1
tracks=8 playback_event_bytes=4 track_event_bytes=4 d031_legacy=256 d031_extra=1792 d032_legacy=256 d032_extra=1792 boundary_step1_frame=5513
legacy_track0_wrappers=1 track_isolation=1 invalid_address_fallback=1 dinghy_migration=0
WP4_9_GATE1_INHERITED_COMPONENTS_OK=1 playback=1 d031=1 d032=1
WP4_9_GATE1_OK=1 tracks=8 d031_track_aware=1 d032_track_aware=1 legacy_track0=1 proof_audio_identical=1 dinghy_migration=0 registry_activation=0 engine_link=0
```

## Permanent verification

Workflow:

- `.github/workflows/wp4_9_gate1_verify.yml`

Runner/test:

- `tests/run_wp4_9_gate1.sh`
- `tests/test_wp4_9_gate1.c`

Canonical final run:

- run `40108097887` — **SUCCESS**
- host job `120913890803` — **SUCCESS**
- PI32v2 job `120913891037` — **SUCCESS**
- verified head `864885df9aa3e45681f116edf088f1e4a2af8bfc`

The initial workflow run exposed only CI harness portability issues (`tee` directory creation and executable mode assumptions). The focused source test and PI32v2 compilation were already green; the final run above is the canonical Gate 1 reference.

## Host evidence

Inherited component gates:

```text
FMT_PLAYBACK_TESTS_OK=1
FMT_PLAYBACK_PARAMS_TESTS_OK=1
FMT_TRACKER_EFFECT_TESTS_OK=1
```

Proof audio remains exactly:

```text
WAV SHA-256 453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440
frames         286464
```

Host evidence artifact:

- artifact ID `12565721379`
- artifact ZIP SHA-256 `03506a101f49e687fdf8632561752b74734366e38df4d4255853235ba6c2deae`

## PI32v2 evidence

The three Gate 1 production modules compile with the pinned JieLi PI32v2 compiler under `-Wall -Werror`.

Object SHA-256:

```text
fmt_playback.o         579bca719c23fa269ff0ddd22fc0c9f3f34480ed94baba9164b76cefd57ac88a
fmt_playback_params.o  ef4a2e7a88c227d39b2c250243bf97a9e967d530cdce0a134a0c80275ec614f1
fmt_tracker_effect.o   29c13d0f65ba1c97bc952a43ea6373de91c411109a03c7bca00eab780bf5c50e
```

Target evidence artifact:

- artifact ID `12565442655`
- artifact ZIP SHA-256 `7a2beac001ad5accc16a73469cc2e3f1cbc34a6ebda51bb3bab2565d39e17991`

## Gate boundary still closed

Gate 1 does **not**:

- modify `dinghy.c`;
- activate D044 IDs in the linked D027 registry;
- allocate/publish the D044 2048 B linked row cache;
- allocate the D044 16-instrument base bank;
- instantiate or render `Fmt8BitEngine`;
- replace the proof-sine tracker lane;
- claim final WP4.9 resource or timing margins.

Those changes remain staged behind later D045 gates.

## Next gate

Proceed to **D045 Gate 2 only**: implement and verify the pure linked-engine state/voice/render/mix layer defined by Sections 9, 10, 11, 15 and 16 of `WP4_9_LINKED_ENGINE_CONTRACT.md` against the already-verified D038-D044 primitives.

Gate 2 must freeze/prove the exact `Fmt8BitOscState` 8 B, `Fmt8BitVoice` 44 B and `Fmt8BitEngine` 404 B layouts, deterministic reset/NOTE/RETRIG/OFF/release-tail behavior, engine-specific render dispatch and bounded eight-slot 32-bit mix/clamp behavior.

Do not activate D044 registry/cache/editor integration (Gate 3) and do not modify `dinghy.c` (Gate 4) until Gate 2 is green.
