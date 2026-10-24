# WP4.9 / D045 Gate 3 Verification

Date: 2026-10-24  
Branch: `phase4/8bit-engine`  
Package: WP4.9 / issue #40  
Gate: 3 — D044 linked registry/cache/base-bank + page-aware p-lock editor

## Canonical verification

Verified head:

`6db9e87d03edd959421ebbb6ae751886583c6ddb`

Workflow:

- run `40111447360` — **SUCCESS**
- host job `120923852664` — **SUCCESS**
- PI32v2 job `120923852448` — **SUCCESS**

Artifacts:

- host artifact `12566329170`
  - ZIP SHA-256 `a50f5a26bb23057d08b2e71aa04471335a76f839da2340f3e01e07dca1f8652d`
- PI32v2 artifact `12566277842`
  - ZIP SHA-256 `dc732fe84d46ca6effd8ce74e5d4da96df45dcf7614ff5f66ec13388d621030e`

Frozen PI32v2 object identities:

- `fmt_8bit_linked_params.o`
  - SHA-256 `3188576bf205d4ea013d9b8bccf76471ab9c2732c9f09e0c7295c2be52eaa45c`
- `fmt_lock_editor.o`
  - SHA-256 `69adabd71f4bf0ec972bcda4c8edd926741c59e2b2bbbedace7a9e83c92858bd`
- `fmt_param_registry.o`
  - SHA-256 `dc84feac8e0ad745d8e28514ed99c7256caf95655ddd3375580bfda122c86d48`

## What Gate 3 proves

Gate 3 deliberately activates the D044 parameter domain without yet migrating `dinghy.c` to the linked 8-BIT tracker engine.

The verified production delta from the clean Gate 3 pre-source checkpoint `c990305379332e1d12ed3d1abdde0a923fe03bb1` is exactly:

- `firmware/src/fmt_8bit_linked_params.c`
- `firmware/src/fmt_8bit_linked_params.h`
- `firmware/src/fmt_8bit_params.h`
- `firmware/src/fmt_lock_editor.c`
- `firmware/src/fmt_lock_editor.h`
- `firmware/src/fmt_model.h`
- `firmware/src/fmt_param_registry.h`

The following remain unchanged across Gate 3:

- `firmware/src/dinghy.c`
- pure D045 Gate 2 engine state/render implementation
- Gate 1 playback/D031/D032 publication modules
- D038-D043 oscillator/envelope/allocator primitives
- D032 tracker-effect ownership

### D027 registry activation

The linked registry now contains exactly ten assigned IDs:

- four inherited common/track IDs;
- six D044 8-BIT IDs `0x20..0x25`:
  - ATTACK
  - DECAY
  - SUSTAIN
  - RELEASE
  - WIDTH
  - TABLE

All six are lockable and remain in the 8-BIT namespace. `0x26..0x5F` remain unassigned.

### Exact Gate 3 state

- runtime instrument base bank: `16 × 6 B = 96 B`
- aligned D044 row publication cache: `8 × 64 × 4 B = 2048 B`
- no pool allocation
- all sixteen base sets reset to the frozen D044 defaults
- publication cache resets to zero

### Deterministic publication

Focused host tests prove:

- NOTE-only instrument inheritance;
- TIE/EMPTY/OFF do not switch current instrument;
- malformed explicit NOTE instrument publishes zero for that row without corrupting later inherited context;
- unsupported instrument mode/engine publishes zero and remains safely silent until a later valid explicit instrument;
- all four engine pages resolve correctly;
- unrelated valid locks are ignored by a page rather than misinterpreted;
- one row refresh reconstructs inheritance with a bounded maximum 64-row scan;
- base-bank mutation rebuilds the fixed publication cache deterministically;
- invalid track/row/base addresses fail safely.

### Page-aware p-lock editing

Gate 3 adds one semantic editor mutation path and keeps the inherited D029/D030 storage grammar.

Verified KNOB1..4 pages are:

- PULSE: WIDTH / DECAY / SUSTAIN / RELEASE
- NOISE: ATTACK / DECAY / SUSTAIN / RELEASE
- TRIANGLE: ATTACK / DECAY / SUSTAIN / RELEASE
- WAVETABLE-8: TABLE / DECAY / SUSTAIN / RELEASE

The selected instrument base value is used as the reference when creating a new lock.

Successful writes and deletes refresh exactly:

- the affected D031 track-row snapshot;
- the affected D044 track-row snapshot.

They do **not** refresh D032 because effect bytes are not p-lock storage.

Footer/debug labels now recognize the six 8-BIT semantic IDs with correct K1..K4 slot identity.

### Historical D044 regression handling

The archived WP4.8 D044 test remains unchanged. Its one historical assertion that `0x20..0x25` were intentionally unassigned is superseded by Gate 3, so Gate 3 runs a temporary build-directory copy with only that registry-isolation assertion adapted to the now-active D045 registry.

All other D044 resolver checks, including replay signature `635311c5`, remain unchanged and green.

The pure Gate 2 engine replay remains frozen at:

`c2a8d4df`

## Realtime/static audit

The Gate 3 publication/editor layer contains:

- no heap allocation;
- no float/double;
- no runtime 64-bit math;
- no `while`, `goto`, `qsort` or `bsearch` in the linked publication module;
- only fixed-row/fixed-lock traversal in main/editor context;
- no D032 effect refresh from p-lock mutation.

The three changed Gate 3 modules compile with the pinned JieLi PI32v2 toolchain and their object identities are frozen above.

## Proof-lane preservation

Because Gate 4 has not yet migrated `dinghy.c`, the inherited tracker proof-sine host WAV remains byte-identical:

- SHA-256 `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`
- frames `286464`

This is intentional. Gate 4 is where the proof-sine tracker lane is deliberately replaced by the linked 8-BIT engine and new audio identities become normative.

## Gate result

**D045 Gate 3 is GREEN.**

The only next WP4.9 integration step is Gate 4:

- migrate `dinghy.c` from proof-sine tracker audio to the bounded D037-D045 8-BIT engine;
- seed deterministic 8-BIT instruments/base sets;
- link the Gate 3 base bank/cache;
- connect all eight tracker tracks;
- preserve the separate D034 audition-sine lane;
- add deterministic PULSE/NOISE/TRIANGLE/WAVETABLE/mixed/eight-track/D033/precedence fixtures;
- build with the pinned target toolchain and prove linked resources remain inside D037 ceilings.

WP4.10 representative timing/Phase 4 closeout remains blocked until Gate 4 is green.

Physical FM-1 install/audio/execution-margin verification remains pending because no physical hardware is currently available.
