# WP4.9 Gate 2 Verification — D045 Pure Linked 8-BIT Engine

Date: 2026-10-24  
Package: WP4.9 / issue #40  
Decision: D045  
Branch: `phase4/8bit-engine`  
Normative contract: `docs/WP4_9_LINKED_ENGINE_CONTRACT.md`

## Result

**D045 Gate 2 is GREEN.**

The pure fixed-state linked 8-BIT engine layer is implemented and verified in isolation. It composes the already-verified D038-D044 primitives into one deterministic eight-slot engine without song/editor/cache traversal and without crossing the `dinghy.c` migration boundary.

Gate 3 D044 registry/cache/editor activation remains untouched. The current application still renders the inherited proof-sine tracker lane byte-for-byte.

## Gate 2 production boundary

Clean Gate 2 pre-source checkpoint:

`fe5816fc4c8fae23b5dd7781f7ea9ddafab2b01b`

Exactly two production files differ from that checkpoint:

- `firmware/src/fmt_8bit_engine.h`
- `firmware/src/fmt_8bit_engine.c`

The permanent runner rejects any Gate 2 production delta in:

- `firmware/src/dinghy.c`;
- D044 registry/parameter files;
- Gate 1 playback/D031/D032 publication files;
- D038-D043 pitch/oscillator/envelope/allocator primitives.

Production implementation commits:

- header: `99766436c8d9b38b58cfe9c8cea16c518c11c727`
- source: `6ea6ae1527a101b4df16928c48d75785fbbcc296`

## Frozen state sizes

The D045 contract sizes are compile-time asserted and host-tested:

```text
Fmt8BitOscState = 8 B
Fmt8BitVoice    = 44 B
Fmt8BitEngine   = 404 B
voice slots     = 8
```

`Fmt8BitEngine` owns exactly one 52 B D043 allocator and eight 44 B voices. No global engine instance is allocated in Gate 2.

## Verified lifecycle semantics

The focused suite proves:

- reset produces eight IDLE voices and no owners;
- valid NOTE uses D043 allocation/reuse only;
- same-track/same-engine/same-instrument NOTE retains oscillator phase while hard-restarting D042;
- instrument or engine identity change resets the oscillator union deterministically;
- RETRIG never allocates, resets oscillator state and hard-restarts D042;
- TIE/EMPTY row update changes only live pitch/gain/page state and does not retime the active envelope;
- PULSE/WAVETABLE ATTACK comes from instrument base while NOISE/TRIANGLE slot 0 is ATTACK;
- D042 DECAY/SUSTAIN are captured at articulation start rather than retroactively rewritten by later row updates;
- OFF/CUT-style release consumes the current row RELEASE, starts D042 release and detaches D043 ownership in the same event;
- detached release tails continue rendering and are eligible for deterministic D043 release-tail stealing;
- zero RELEASE becomes immediate IDLE and clears the voice;
- malformed/unsupported NOTE fails toward silence for the affected track and never creates a voice;
- engine rendering scans the fixed eight slots in ascending order;
- source × envelope × gain uses the frozen Q15/32-bit path;
- positive and negative eight-voice overload clamp to `+32767` / `-32767`;
- quiet state requires every envelope, including detached tails, to be IDLE;
- no dynamic allocation, float, 64-bit math, unbounded loop, store traversal or function-pointer dispatch appears in the Gate 2 module.

Focused marker:

```text
osc_state_bytes=8 voice_bytes=44 engine_bytes=404 slots=8 mix_clamp=32767 identity_reset=1 phase_reuse=1 retrig_reset=1 release_tail=1 malformed_silence=1
```

## Deterministic replay

The mixed four-engine lifecycle replay covers PULSE, NOISE, TRIANGLE and WAVETABLE-8 plus live row update, RETRIG, release-tail detach and same-identity NOTE reuse.

Frozen FNV-1a32:

```text
c2a8d4df
```

The permanent Gate 2 runner requires this exact value.

## Primitive regressions

The isolated Gate 2 adapter reruns and passes the D038-D044 focused suites:

```text
D038 PULSE       PASS
D039 NOISE       PASS
D040 TRIANGLE    PASS
D041 WAVETABLE-8 PASS
D042 envelope    PASS
D043 allocator   PASS
D044 parameters  PASS
```

Marker:

```text
WP4_9_GATE2_PRIMITIVE_REGRESSIONS_OK=1 d038=1 d039=1 d040=1 d041=1 d042=1 d043=1 d044=1
```

## Full-app isolation proof

Because Gate 2 remains unlinked from `dinghy.c`, the historical proof lane remains exact:

```text
WAV SHA-256 453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440
frames         286464
```

This is the same byte-identical full-app host baseline retained through WP4.8 and Gate 1.

## PI32v2 proof

`fmt_8bit_engine.c` compiles with the pinned JieLi PI32v2 compiler under `-Os -ffunction-sections -fno-builtin -Wall -Werror`.

Frozen object SHA-256:

```text
967fba0d5672a5d572ac71e58772ca4e2c959fb2642b95a3c9f39072431b59a9
```

The permanent target job requires this exact object hash.

## Permanent verification

Workflow:

- `.github/workflows/wp4_9_gate2_verify.yml`

Runner/test:

- `tests/run_wp4_9_gate2.sh`
- `tests/test_wp4_9_gate2.c`

Canonical final verification:

- head `382c758a64c1b2572adbe608f703a4c3d58397f6`
- run `40109064516` — **SUCCESS**
- host job `120916760172` — **SUCCESS**
- PI32v2 job `120916760440` — **SUCCESS**

Canonical artifacts:

- host artifact `12565787706`
  - ZIP SHA-256 `11c7ea28b8f91e51efb522c39240757689a7bd5d32b7f3e3e1dda5aae0104303`
- PI32v2 artifact `12565628873`
  - ZIP SHA-256 `e5d53e3da9a07c27bb89aaa023ac3355c3c143640da3e55c42a97c1a1ffca27e`

The initial Gate 2 run failed only on an unused local in the new test source. The next discovery run proved the engine behavior and yielded the replay/object identities; the canonical run above additionally locks both identities as hard requirements.

## Gate boundary still closed

Gate 2 does **not**:

- modify or link `dinghy.c`;
- activate D044 IDs in the linked D027 registry;
- allocate the 96 B linked instrument-base bank;
- allocate/publish the 2048 B linked D044 row cache;
- add page-aware D044 p-lock editing;
- replace the proof-sine tracker lane;
- claim final linked firmware/RAM/timing margins.

## Next gate

Proceed to **D045 Gate 3 only**: activate the six D044 IDs in the linked parameter registry, add the fixed 16 × 6 B instrument base bank, add the aligned 8 × 64 × 4 B D044 row cache with main-context publication, and make the hold-step encoder use the selected track's four D044 page IDs.

Gate 3 must preserve D028 max-four sparse locks, the existing track-0 compatibility layers and all Gate 1/Gate 2 source boundaries. Audio/store traversal remains forbidden and `dinghy.c` must remain unchanged until Gate 3 is green.
