# WP4.7 Verification — deterministic voice allocation and stealing

Date: 2026-10-24  
Package: WP4.7 / issue #37  
Decision: D043  
Branch: `phase4/8bit-engine`

## Result

**COMPLETE at host/device-build level.**

WP4.7 adds the isolated deterministic eight-slot allocator frozen by D043. It is not connected to `dinghy.c`, so the running full application remains byte-identical to the WP4.6 / Phase 3 production baseline.

Physical FM-1 install/audio/execution-margin verification remains pending because no physical device is available.

## Contract and checkpoints

- D043 contract: `docs/WP4_7_ALLOCATOR_CONTRACT.md`
- contract commit: `1215c89aa48cd2727c10c6af327fdc0052d72562`
- D043 decision/status freeze commit: `f6e89267cf23c280283888ead9b63422027722d0`
- documentation-freeze run/job: `40103015219` / `120898850822` — SUCCESS
- clean pre-source checkpoint: `d9f6bf350bcb15a8f3dc62c4fc1112c9f0f1ffa2`
- allocator header commit: `897dd0175679f9d8fd3395f49144fcae28ad6af9`
- allocator implementation commit: `c998725431e04f6655fb884f372b18a7f8c472c7`
- permanent focused workflow commit: `d7ce8273eba0a6c2fda540e34cdc6f31308963a1`
- closeout workflow commit: `850cff1beba891da68b4222db82df1f83b4eeb08`

## Frozen allocator semantics proved

D043 verification proves:

- `FmtVoiceAllocator` is exactly 52 B;
- `FmtVoiceAllocation` is exactly 4 B;
- exactly eight logical tracks and eight voice slots;
- reset produces all-unowned mappings, zero ages and zero clock;
- first allocations choose slots `0..7` deterministically;
- an already-owned non-IDLE voice is REUSEd for the same track and does not request oscillator reset;
- stale IDLE ownership is synchronized away before allocation;
- OFF / NOTE CUT may detach a track after D042 enters RELEASE, preserving an unowned release tail;
- genuinely free IDLE slots always win before release-tail stealing;
- only unowned RELEASE tails are legal steal candidates;
- release-tail winner is oldest age, then lower slot index;
- a voice still owned by another track is never stolen;
- the recency clock saturates at `UINT32_MAX` and never wraps;
- malformed track IDs, stage IDs and broken bidirectional mappings fail without partial mutation;
- all 256 unowned IDLE/RELEASE masks are deterministic;
- guard words remain intact;
- identical state/input sequences replay byte-identically;
- the frozen 4096-event sequence yields FNV-1a32 `c6baff38` and final clock `2658`.

This confirms D043's central policy: Phase 4 uses release-tail stealing only, not active cross-track stealing.

## Focused host verification

Workflow run: `40103284377`

Host job: `120899638680` — **SUCCESS**

Focused output:

```text
WP4_7_ALLOCATOR_TESTS_OK=1
allocator_bytes=52 result_bytes=4 slots=8 masks=256 active_cross_track_steal=0 release_tail_steal=1
selection=reuse,lowest_idle,oldest_release_tail tie=lower_slot clock=saturating_u32
long_hash_fnv1a32=c6baff38 events=4096 final_clock=2658
WP4_7_ALLOCATOR_FOCUSED_GATE_OK=1
```

Static audit also passed:

```text
WP4_7_STATIC_AUDIT_OK=1 bounded_slots=8 alloc=0 float=0 int64=0 divide_modulo=0 store_traversal=0 active_cross_track_steal=0
```

The production delta from the clean pre-source checkpoint is exactly:

```text
firmware/src/fmt_voice_allocator.h
firmware/src/fmt_voice_allocator.c
```

`dinghy.c`, D038-D042 engine primitives and previous production DSP remain unchanged.

## Real JieLi PI32v2 compilation

Target job: `120899638431` — **SUCCESS**

Pinned real PI32v2 object:

```text
fmt_voice_allocator.o
SHA-256 5a2733984653568b3e04637d097c44a267f546a74bc1b7b55e4e18c269e965bd
```

Focused target artifact:

- artifact ID `12562853858`
- artifact ZIP SHA-256 `3f8b9d2996ac1b5adfa06fde4b753c7f5b01e8afeb1718dfcef5bfe5d65ea4ae`

## Final full-app closeout

Closeout run: `40103427588`  
Closeout job: `120900076833` — **SUCCESS**

The closeout reran:

- exact isolated source-boundary proof;
- D043 deterministic focused tests;
- static realtime audit;
- full inherited host render;
- real PI32v2 allocator object compilation;
- pinned AC79 SDK/input validation;
- complete target firmware build;
- exact resource/hash/package-identity comparisons.

Closeout markers:

```text
WP4_7_ISOLATED_PRODUCTION_DELTA_OK=1 files=2 linked_delta=0 prior_engine_delta=0
WP4_7_TARGET_OBJECT_OK=1 allocator_sha=5a2733984653568b3e04637d097c44a267f546a74bc1b7b55e4e18c269e965bd
WP4_7_D043_EXIT_OK=1 allocator=isolated deterministic=1 masks=256 release_tail_steal=1 active_cross_track_steal=0 pi32v2=1 full_app_identical=1 physical_hardware_pending=1
WP4_7_CLOSEOUT_OK=1
```

Closeout evidence artifact:

- artifact ID `12564135499`
- size `1,120,058` B
- artifact ZIP SHA-256 `69cc5c847abfb36495517bb62a827ac7d5b6a34ce07e9313c5ffdb8995c6398b`

## Full application remains exactly unchanged

Because the allocator remains isolated/unlinked, the complete running app is exactly the WP4.6 baseline:

```text
image             66,960 B
normal RAM         13,296 B
pool RAM          113,000 B
image delta             0 B
normal RAM delta        0 B
pool RAM delta          0 B
```

Artifacts:

```text
host WAV SHA-256
453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440

fm1tracker.bin SHA-256
b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27

fm1tracker-0.1.fwsc SHA-256
7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c

package identity
FM-1_987
```

## Resource interpretation

The isolated allocator adds a future persistent engine-state requirement of 52 B normal RAM when integrated, but WP4.7 intentionally does not link it into the full application. Therefore measured full-app RAM/image deltas are still exactly zero.

The 52 B allocator remains comfortably inside D037's Phase 4 runtime-state and total RAM ceilings. Integration cost will be measured only when the engine is deliberately connected in WP4.9.

## Exit status

WP4.7 exit criteria are satisfied at host/device-build level:

- D043 frozen before source — PASS;
- exact fixed state/result layouts — PASS;
- deterministic owner/mapping invariants — PASS;
- all-eight allocation order — PASS;
- same-track reuse — PASS;
- IDLE synchronization — PASS;
- release-tail detach/steal semantics — PASS;
- no active cross-track steal — PASS;
- 256 IDLE/RELEASE masks — PASS;
- saturating recency/tie-break — PASS;
- malformed-state no-mutation — PASS;
- replay/guard/long-hash proof — PASS;
- static realtime audit — PASS;
- pinned PI32v2 compile — PASS;
- complete pinned target build — PASS;
- full-app byte identity — PASS;
- physical FM-1 verification — PENDING LATER HARDWARE GATE.

Next package: **WP4.8 — four-parameter engine pages + p-lock mapping**. The next decision must freeze the stable parameter IDs, per-engine page layout/defaults/domains, D027/D031 publication mapping and malformed-value behavior before production parameter-page source.
