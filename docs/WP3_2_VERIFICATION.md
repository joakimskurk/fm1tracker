# WP3.2 Verification — sparse four-lock-per-step storage

Date: 2026-10-24  
Status: **COMPLETE at host/device-build level**

WP3.2 implements the first concrete bounded p-lock storage layer on top of the verified Phase 2 song store and D027 stable parameter registry. It does not implement encoder editing, user-facing lock UX, playback application, synth-engine behavior or persistence.

## Checkpoints

Phase 3 / WP3.2 branch:

`phase3/plocks-effects`

WP3.2 start head:

`dc0f4280f27e8a1a1a4608faef8d59cfceb68a1e`

Final verified source/workflow checkpoint:

`da2f5afe9b1da6defc3857b91f52b40d7525f33b`

Focused storage run:

- run `40037916472`
- job `120701822736` (`lock-store-focused`)
- result **SUCCESS**

First complete measurement run:

- run `40038242601`
- job `120702893751`
- result **SUCCESS**

Final frozen closeout run:

- run `40038453521`
- job `120703577806` (`lock-store-closeout`)
- result **SUCCESS**

## D028 frozen storage contract

D028 was recorded before storage source implementation.

The complete song shares one global sparse pool of **2,048 records**. This does not promise four locks on every one of the 8,192 resident steps simultaneously. Any one step may own at most four active locks, while the entire song shares the fixed record capacity.

`FmtLock` is exactly 8 bytes:

```c
typedef struct {
    uint16_t next;
    uint16_t value;
    uint16_t owner;
    FmtParamId param_id;
    uint8_t state;
} FmtLock;
```

Frozen in-memory offsets:

```text
next      0
value     2
owner     4
param_id  6
state     7
sizeof    8
```

Pool cost:

```text
2048 records * 8 B = 16,384 B
```

Allocator runtime state is exactly 4 bytes:

```c
typedef struct {
    uint16_t free_head;
    uint16_t used_count;
} FmtLockStoreRuntime;
```

`FmtStep` remains exactly 8 bytes and retains its 16-bit `lock_head`; `0xFFFF` remains the no-lock/list-end sentinel.

## Owner isolation

Every active record contains a 16-bit linear step owner:

```text
pattern * (8 * 64) + track * 64 + step
```

This covers exactly `0..8191`.

All 8,192 owner addresses are exhaustively tested for uniqueness. A record reached through a valid in-range index but carrying a different owner is treated as corruption rather than another step's valid lock.

This owner check is the main reason WP3.2 uses the already-budgeted 8-byte record instead of the older illustrative 4-byte sketch.

## Deterministic list and allocator behavior

Reset produces exactly one ascending free list:

```text
0 -> 1 -> ... -> 2047 -> 0xFFFF
```

Every free record appears exactly once after reset.

Active per-step lists are strictly sorted by ascending `FmtParamId`. They therefore do not depend on edit history.

Storage semantics verified:

- only D027 IDs reported lockable by the authoritative registry may be stored;
- setting an existing parameter replaces its value in place and consumes no record;
- adding a new parameter inserts in sorted order;
- duplicate active parameter IDs are rejected as corrupt state;
- a fifth prevalidated unique lock returns `STEP_FULL` before allocation;
- global exhaustion returns `POOL_FULL` without changing existing locks;
- removing one parameter returns exactly one record to the free list;
- clearing a valid step returns its complete list exactly once;
- a just-freed record is deterministically reusable;
- malformed/out-of-range/cross-owned/unsorted/overlong lists fail with no partial mutation.

Because D027 currently exposes exactly four production-lockable IDs, the fifth-lock branch is tested through a dedicated test-only lockability stub accepting IDs 1..5. This does not add a fifth production parameter.

## Focused host evidence

The production-registry lock-store test emits:

```text
FMT_LOCK_STORE_TESTS_OK=1
record_bytes=8 capacity=2048 storage_bytes=16384 runtime_bytes=4 owners=8192 max_per_step=4
pool_exhaustion=explicit owner_check=1 sorted_by_param=1 replace_in_place=1 corruption_no_partial_mutation=1
```

The focused max-four harness emits:

```text
FMT_LOCK_LIMIT_TESTS_OK=1
max_per_step=4 fifth_prevalidated=STEP_FULL used_unchanged=1
```

Both are compiled with `-Wall -Wextra -Werror` in the normal host gate.

## Complete inherited regression result

The final closeout run preserves all inherited verification from WP3.1 and Phase 2:

- D027 parameter registry exhaustive test;
- model and store tests;
- exact integer transport tests;
- playback tests;
- pattern input tests;
- STEP REC tests;
- tracker screen tests;
- WP2.5 pattern-context host fixture;
- WP2.6 track-selection fixture;
- WP2.7 STEP REC context fixture;
- WP2.8 REST/TIE/OFF and release-audio fixture;
- WP2.9 reviewed tracker screens and HOME return;
- direct Phase 2 integration test;
- full-app Phase 2 panel/display integration fixture.

The exact frozen host audio remains:

```text
HOST_WAV_SHA256=453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440
HOST_WAV_FRAMES=286464
WP3_2_FROZEN_AUDIO_OK=1
```

WP3.2 storage is intentionally not applied to audio/playback yet.

## JieLi target and device build

The dedicated registry and lock-store translation units compile with the pinned real JieLi PI32v2 compiler before the normal app build.

Toolchain:

- package `jieli-linux-toolchains-20260730.1`
- clang 4.0.1

External AC79 SDK remains frozen at:

- tag `AC79NN_SDK_V1.2.1_2023-12-13`
- commit `d179b4484759423312073f5fbb232501aa491047`
- inherited SDK input SHA-256 checks remain green.

Device safety remains green:

```text
.ram_text: 901 insns, no calls
register access: hal/ only (src/, loader/ clean)
PACKAGE_IDENTITY=FM-1_987
```

## Linked symbols

Final linked WP3.2 symbols:

```text
01c20000 .pool 00010168 fmt_store_storage
01c30168 .pool 00004000 fmt_lock_store_storage
01c0b080 .bss  00000004 fmt_lock_store_runtime
```

Therefore:

- `fmt_store_storage` remains 65,896 B at `.pool` `0x01C20000`;
- `fmt_lock_store_storage` is exactly 16,384 B at `.pool` `0x01C30168`;
- `fmt_lock_store_runtime` is exactly 4 B in ordinary `.bss` at `0x01C0B080`.

No raw pointers are stored in `FmtStep`.

## Final resource footprint

WP3.1 baseline:

| Resource | WP3.1 |
|---|---:|
| app image | 59,252 B |
| normal RAM | 12,464 B |
| pool RAM | 96,616 B |

Final WP3.2:

| Resource | Used | Limit | Free | Delta vs WP3.1 |
|---|---:|---:|---:|---:|
| app image | 59,388 B | 581,564 B | 522,176 B | +136 B |
| normal RAM | 12,468 B | 98,304 B | 85,836 B | +4 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B | +16,384 B |

The pool and normal-RAM deltas match D028 exactly.

Core editable data remains:

```text
FmtSong core              65,896 B
lock records              16,384 B
---------------------------------
editable core + locks     82,280 B
```

This remains below the frozen 96 KiB editable-song target by 16,024 B.

## Final artifacts

`fm1tracker.bin`:

`b717a504f96601393e493f52142e3120204051c5e11b733b0113309e0219ac2b`

`fm1tracker-0.1.fwsc`:

`36c054c0f6c0cc8e86dd869fadf8d0093a560f5851a9ebd9b2405552642d5966`

Package identity:

`FM-1_987`

Evidence artifact from final run:

- name `wp3-2-lock-store-evidence`
- artifact ID `12538271532`
- size `1,115,903 B`
- ZIP SHA-256 `ecb0985b24201e3506d1512ead85e406e4226bebc7915d945cd5812e5dbd1830`

## Capacity correction recorded

WP3.2 explicitly corrects an older planning-arithmetic error. The song contains:

```text
16 patterns * 8 tracks * 64 steps = 8192 steps
```

Four locks on every resident step would therefore require:

```text
8192 * 4 = 32768 records
```

The earlier data-model prose that called 2,048 records the whole-song worst case omitted the 16 pattern slots. D028 keeps 2,048 as a deliberate **global sparse capacity**, preserving the existing RAM target rather than silently expanding the allocator by 16x.

## Explicit non-goals preserved

WP3.2 does not implement:

- hold-step + encoder editing;
- user-facing replace/delete gestures;
- p-lock screen feedback;
- applying locks to playback/audio;
- tracker effect semantics;
- 8-BIT or FM4 engines;
- live performance capture/automation;
- persistence/file format;
- hardware-control-map freeze.

These remain later packages.

## Hardware limitation

No physical FM-1 is currently available. This verification proves host behavior, target compilation, linked resource placement and firmware/package generation. It does not claim physical installation, boot/audio behavior, control ergonomics or measured AC791N execution timing.

## Result

WP3.2 exit criteria are satisfied at the host/device-build level. The project now has stable D027 parameter identity plus a bounded, deterministic, owner-checked sparse lock store suitable for the next package's editor integration.
