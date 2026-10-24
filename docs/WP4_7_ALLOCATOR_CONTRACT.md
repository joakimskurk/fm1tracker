# WP4.7 Deterministic Voice Allocator Contract

Date: 2026-10-24  
Package: WP4.7 / issue #37  
Decision: D043  
Branch: `phase4/8bit-engine`

This document is normative for WP4.7. Production allocator source must not precede it and must not silently widen its scope.

## 1. Scope

WP4.7 adds one isolated deterministic allocator for the Phase 4 maximum-eight-voice engine frozen by D037.

It does **not** integrate the oscillator/envelope primitives into `dinghy.c`, replace the Phase 3 proof sine, add parameter pages, or change D032/D033 event timing. Full engine integration remains WP4.9.

The allocator owns only voice-slot ownership/recency bookkeeping. Oscillator state, D042 envelope state, pitch and row parameters remain outside it.

## 2. Why stealing is release-tail stealing, not cross-track active stealing

D037 freezes:

- eight logical tracker tracks;
- at most eight Phase 4 voice slots;
- at most one primary sequenced Phase 4 voice owned by each logical track.

Therefore, in any valid state where a requesting track does not already own a voice, eight simultaneously **owned** voices would imply that the requesting track already owns one. An allocator does not need to cut another track's active primary voice to satisfy a valid eight-track/eight-slot request.

D043 therefore forbids active cross-track stealing.

OFF/NOTE CUT are different: D042 starts RELEASE from the current level, then D043 detaches the logical track from that slot immediately. The audio tail may continue as an **unowned RELEASE tail**. A later NOTE may steal/reclaim such an unowned tail if no IDLE slot exists.

This preserves release tails when capacity exists while ensuring that a busy engine never kills another track's still-owned primary voice merely to restart a track that already released.

## 3. Constants and identity

Exactly:

```text
FMT_VOICE_ALLOCATOR_SLOTS = 8
FMT_VOICE_ALLOCATOR_TRACKS = 8
FMT_VOICE_NONE = 0xFF
FMT_TRACK_NONE = 0xFF
```

Valid logical tracks are `0..7`. Valid voice slots are `0..7`.

`0xFF` is the only unowned/no-voice sentinel. No modulo wrapping of malformed track or voice IDs is allowed.

## 4. Persistent allocator state

Production state is exactly:

```c
typedef struct {
    uint32_t age[8];
    uint32_t clock;
    uint8_t voice_owner[8];
    uint8_t track_voice[8];
} FmtVoiceAllocator;
```

Required size:

```text
sizeof(FmtVoiceAllocator) == 52 B
```

Meaning:

- `voice_owner[v]` is logical track `0..7` or `FMT_TRACK_NONE`;
- `track_voice[t]` is slot `0..7` or `FMT_VOICE_NONE`;
- owned mappings are always bidirectional;
- `age[v]` is a deterministic recency stamp for a non-IDLE slot;
- `clock` is the next recency source.

The allocator contains no pointer into `FmtSong`, p-lock storage, tracker-screen state, or editor state.

There is one allocator for the Phase 4 eight-slot engine, not one allocator per track.

## 5. D042 stage view is caller-owned

Allocator decisions receive one fixed eight-byte stage snapshot supplied by the audio-owned engine layer:

```text
stage[0..7]
```

Each byte must be one D042 stage ID:

```text
IDLE=0, ATTACK=1, DECAY=2, SUSTAIN=3, RELEASE=4
```

The allocator does not duplicate envelope level/rate state.

A slot is **genuinely free** only when its D042 stage is IDLE after stale ownership has been synchronized away.

An unowned non-IDLE slot is valid only as a RELEASE tail. An unowned ATTACK/DECAY/SUSTAIN slot is malformed allocator/engine state and must fail safely rather than be guessed at.

## 6. Reset and mapping invariants

`fmt_voice_allocator_reset()` produces exactly:

- `clock = 0`;
- all `age[v] = 0`;
- all `voice_owner[v] = FMT_TRACK_NONE`;
- all `track_voice[t] = FMT_VOICE_NONE`.

The following invariant must hold after every successful allocator operation:

```text
voice_owner[v] == t  <=>  track_voice[t] == v
```

No two voices may own the same track and no track may map to more than one voice.

A bounded validator may reject malformed state. Rejection returns `NO_VOICE`/failure and must not partially mutate allocator state.

## 7. IDLE synchronization

Before acquisition, D043 synchronizes any IDLE slot to free state:

- if an IDLE slot still has an owner, that track mapping is cleared;
- `voice_owner[v] = FMT_TRACK_NONE`;
- `age[v] = 0`.

This is a bounded eight-slot pass.

A public bounded reap/sync helper may expose the same operation for engine housekeeping, but allocator correctness must not depend on main-thread cleanup.

## 8. NOTE acquisition order

A NOTE or delayed NOTE calls the allocator exactly once when it actually fires.

For valid input, acquisition order is frozen as:

### 8.1 Existing owned voice — REUSE

If the requesting track already owns a non-IDLE voice:

- return the same slot;
- refresh its recency stamp;
- keep ownership unchanged;
- report `reset_required = 0`.

D042 NOTE then hard-restarts the envelope from zero, but D037 ordinary NOTE does **not** restart oscillator phase/state.

### 8.2 Genuinely free IDLE slot — FRESH

Otherwise choose the lowest-index IDLE slot after IDLE synchronization.

- assign it to the requesting track;
- refresh recency;
- report `reset_required = 1`.

The engine caller must reset oscillator/engine-specific state to its deterministic initial state before starting the new note, then perform D042 NOTE articulation.

### 8.3 Unowned RELEASE tail — STEAL_TAIL

If no IDLE slot exists, choose an unowned RELEASE slot.

Winner is:

1. smallest `age[v]` (oldest release-tail recency);
2. lower voice index if ages tie.

The selected tail is hard-repurposed for the new track:

- previous owner is necessarily `FMT_TRACK_NONE`;
- assign new owner mapping;
- refresh recency;
- report `reset_required = 1` and `kind = STEAL_TAIL`.

The engine caller resets oscillator/engine state and performs D042 NOTE articulation. The old release tail ends at the steal boundary.

### 8.4 No legal candidate — NO_VOICE

D043 never steals a slot still owned by another logical track.

If the requester owns no voice, no IDLE slot exists, and no valid unowned RELEASE tail exists, acquisition returns `FMT_VOICE_NONE` with no mutation.

For a valid D037 eight-track/eight-slot state this case should be unreachable except malformed/corrupt input, because a requester with no owned slot implies at least one slot is not owned by another track. It nevertheless remains an explicit fail-safe interface outcome as required by D037.

## 9. OFF / NOTE CUT detaches ownership after starting release

For OFF or NOTE CUT on an active track, caller ordering is frozen:

1. call D042 `gate_off` for the owned voice;
2. obtain the resulting D042 stage snapshot;
3. call allocator detach for that track in the same audio event.

If release is nonzero/current level is nonzero, the slot becomes:

```text
owner = NONE
stage = RELEASE
```

and its recency stamp is refreshed at release start.

If D042 collapses immediately to IDLE, detach leaves the slot genuinely free with age zero.

The track's `track_voice` entry becomes `FMT_VOICE_NONE` immediately. This is what permits a later NOTE on that track to start a new primary voice while an older release tail continues if capacity exists.

Detach never zeroes oscillator or envelope state itself.

## 10. TIE, EMPTY and RETRIG

D037/D042 ownership remains unchanged:

- TIE: no allocator call;
- EMPTY/REST: no allocator call;
- RETRIG: no allocator call and no allocation; it operates only on the track's currently owned voice;
- RETRIG with `track_voice == NONE` is a no-op;
- NOTE DELAY calls normal NOTE acquisition only when the delayed NOTE fires;
- NOTE CUT uses the OFF detach sequence above.

## 11. Recency clock and wrap behavior

D043 uses unsigned 32-bit recency only.

On every successful reuse, fresh allocation or non-IDLE detach:

```text
if clock < UINT32_MAX:
    clock += 1
else:
    clock remains UINT32_MAX
age[selected_voice] = clock
```

The counter **saturates; it never wraps**.

This removes wrap arithmetic from the audio path. Once saturation is reached, newly touched voices may share age `UINT32_MAX`; the frozen lower-voice-index tie-break remains deterministic.

IDLE slots always have age zero after synchronization and are selected by the free-slot rule before release-tail age comparison.

No runtime 64-bit arithmetic is required.

## 12. Allocation result

The focused API must expose enough information for the later engine adapter to distinguish reuse from fresh/reset-required acquisition without inspecting internal allocator arrays.

Frozen result representation:

```c
typedef struct {
    uint8_t voice;
    uint8_t previous_owner;
    uint8_t kind;
    uint8_t reset_required;
} FmtVoiceAllocation;
```

Required size:

```text
sizeof(FmtVoiceAllocation) == 4 B
```

Frozen kinds:

```text
NONE       = 0
REUSE      = 1
FRESH_IDLE = 2
STEAL_TAIL = 3
```

For failure:

```text
voice = FMT_VOICE_NONE
previous_owner = FMT_TRACK_NONE
kind = NONE
reset_required = 0
```

For REUSE, `previous_owner` equals requesting track and `reset_required = 0`.

For FRESH_IDLE and STEAL_TAIL, `reset_required = 1`.

## 13. Bounded realtime behavior

Allocator work occurs only on note/event boundaries or explicit bounded housekeeping, never in the per-sample oscillator/envelope render loop.

Hard requirements:

- fixed eight-slot scans only;
- no heap allocation/free;
- no blocking/lock/wait;
- no tracker/editor/store traversal;
- no recursion;
- no floating point;
- no runtime 64-bit arithmetic/division;
- no unbounded queue/search/sort;
- no work proportional to patterns/steps/locks.

Normal acquire/detach/reap/validate work is O(8) with fixed upper bounds.

## 14. Isolated resource budget

Persistent allocator state:

```text
52 B normal RAM
```

No pool RAM and no mutable table are added.

`FmtVoiceAllocation` is a 4-byte transient return value, not persistent global state.

WP4.7 remains unlinked from the running application. Therefore full-app resource and artifact delta versus WP4.6 must remain exactly:

```text
image delta      0 B
normal RAM delta 0 B
pool RAM delta   0 B
WAV/BIN/FWSC     byte-identical
```

Any later integration cost belongs to WP4.9 and must be measured there.

## 15. Focused deterministic verification required before WP4.7 exit

Host tests must prove at minimum:

1. exact 52 B allocator and 4 B result layout;
2. reset sentinels/ages/clock;
3. all eight first allocations choose slots 0..7 deterministically;
4. same-track NOTE always REUSEs its owned non-IDLE slot and never requests oscillator reset;
5. IDLE synchronization clears stale owner mappings safely;
6. OFF/CUT detach preserves RELEASE tail state externally while removing track ownership;
7. free IDLE slots are always preferred over release-tail stealing;
8. unowned RELEASE tail stealing chooses oldest age then lower slot index;
9. another track's owned ATTACK/DECAY/SUSTAIN/RELEASE voice is never stolen;
10. counter saturation at `UINT32_MAX` never wraps and lower-index tie-break remains exact;
11. malformed track IDs, stage IDs and bidirectional mappings fail without mutation;
12. TIE/EMPTY/RETRIG require no allocation path;
13. guard words around allocator state remain intact;
14. identical state + input sequence yields byte-identical result/state replay;
15. a long deterministic allocate/detach/reap sequence has a frozen state/result hash;
16. real pinned JieLi PI32v2 compilation succeeds;
17. static audit proves bounded fixed-slot behavior and forbidden-operation absence;
18. complete host/device full app remains the exact WP4.6 baseline while allocator source is isolated.

Testing should include all 256 IDLE/RELEASE masks for eight unowned slots where practical, plus owned-track invariants and saturation/tie vectors.

## 16. Non-goals

WP4.7 does not add:

- polyphonic-per-track ownership;
- active cross-track voice stealing;
- user-selectable steal modes;
- velocity/level-based steal scoring;
- envelope-level inspection for quietest-voice selection;
- parameter pages or p-lock engine mapping;
- full tracker adapter integration;
- proof-sine replacement;
- hardware benchmark/Phase 4 closeout.

These require later explicit packages/decisions.

## 17. D043 exit rule

WP4.7 may close only when:

- this contract is reflected in `DECISIONS.md` and project state before production allocator source;
- isolated allocator source exactly implements the frozen semantics;
- focused host verification is green;
- pinned PI32v2 compile is green;
- full app remains byte-identical to WP4.6;
- permanent verification evidence is recorded in `docs/WP4_7_VERIFICATION.md`.
