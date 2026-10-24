# FM1Tracker V0.1 Data Model

Status: **implemented core model, amended by D027-D045 and D047**

The data model stays compact, deterministic and versionable. It is designed for fixed-capacity firmware storage, not arbitrary desktop-sized songs.

All persistent structures require explicit serialization. Raw compiler structs are **not** the project file format.

D047 changes future sound-family semantics without changing the current core layouts: the project is not globally 8-BIT or 16-BIT; each instrument carries its own engine-family + engine identity. See `D047_MIXED_ENGINE_ARCHITECTURE.md`.

## 1. Constants

Current V0.1 limits:

```c
#define FMT_TRACKS              8
#define FMT_STEPS_PER_PATTERN  64
#define FMT_PATTERN_SLOTS      16
#define FMT_ORDER_ENTRIES     128
#define FMT_INSTRUMENTS        16
#define FMT_LOCKS_PER_STEP      4
#define FMT_LOCK_POOL_MAX    2048
```

`FMT_LOCK_POOL_MAX` is the global sparse lock capacity for the complete editable song. Any individual step may own at most four locks; all resident song steps share the fixed 2,048-record pool.

## 2. Note representation

One byte:

```c
enum {
    FMT_NOTE_MIN   = 0,
    FMT_NOTE_MAX   = 127,
    FMT_NOTE_TIE   = 0xFD,
    FMT_NOTE_OFF   = 0xFE,
    FMT_NOTE_EMPTY = 0xFF
};
```

- `EMPTY` means no note command on the row.
- `OFF` explicitly releases the currently owned track voice according to the active engine adapter.
- `TIE` continues an already active note without ordinary NOTE retrigger semantics.
- STEP REC REST writes/keeps `EMPTY` and advances; REST is not NOTE OFF.

These tracker semantics are engine-family independent.

## 3. Step record

The implemented in-memory representation remains exactly **8 bytes**:

```c
typedef struct {
    uint8_t  note;        /* 0..127, TIE, OFF, EMPTY */
    uint8_t  instrument;  /* 0..15, or 0xFF = inherit */
    uint8_t  volume;      /* current inherited/explicit row byte */
    uint8_t  effect;      /* tracker effect command */
    uint8_t  effect_value;
    uint8_t  flags;
    uint16_t lock_head;   /* D028 sparse lock-list index */
} FmtStep;
```

`lock_head` is a 16-bit D028 record index, never a pointer.

### Instrument indirection is the key D047 compatibility point

D047 requires **no step-layout expansion**. `FmtStep.instrument` already gives the sequencer the correct indirection:

```text
row -> instrument slot -> engine family + engine
```

An inherited instrument keeps the current track instrument. An explicit valid instrument index may later switch engine family as part of normal NOTE semantics after WP5.0 establishes the mixed-family router.

## 4. Pattern

A pattern contains eight logical tracks, each with 64 steps:

```c
typedef struct {
    FmtStep steps[FMT_TRACKS][FMT_STEPS_PER_PATTERN];
} FmtPattern;
```

Memory:

```text
8 bytes * 8 tracks * 64 steps = 4096 bytes
4096 * 16 pattern slots       = 65536 bytes
```

The runtime song store remains pool-backed.

## 5. Parameter locks

Locks are sparse and separately allocated from the D028 global pool.

The implemented lock record is exactly 8 bytes:

```c
typedef struct {
    uint16_t next;
    uint16_t value;
    uint16_t owner;
    FmtParamId param_id;
    uint8_t state;
} FmtLock;
```

Offsets:

```text
next      0
value     2
owner     4
param_id  6
state     7
sizeof    8
```

This raw compiler struct is not a persistence ABI.

### Global sparse capacity

The complete resident song has:

```text
16 patterns * 8 tracks * 64 steps = 8192 steps
```

A theoretical four-lock allocation on every step would require 32,768 records and is intentionally not the RAM contract.

Actual D028 capacity:

```text
2048 records * 8 bytes = 16384 bytes
```

Pool exhaustion is explicit and deterministic. Existing locks are never silently discarded.

### Ownership and list rules

Each active record carries exact linear owner:

```text
pattern * (8 * 64) + track * 64 + step
```

covering `0..8191`.

Active step lists:

- are strictly sorted by ascending `FmtParamId`;
- contain at most four records;
- never contain duplicate parameter IDs;
- replace duplicate sets in place;
- reject a fifth unique lock as STEP_FULL;
- reject global exhaustion as POOL_FULL;
- validate owner/index/order/state before mutation;
- fail CORRUPT without partial mutation on malformed lists.

The 16-bit `value` remains semantic data owned by the parameter/engine contract.

## 6. Parameter IDs

D027 freezes parameter identity as one unsigned byte:

```c
typedef uint8_t FmtParamId;
```

Special values:

```text
0x00 NONE
0xFF INVALID
```

Historical namespace ranges remain:

```text
0x01-0x1F common voice parameters
0x20-0x5F first 8-BIT/chip-family engine parameters
0x60-0x9F FM/digital and future explicitly assigned engine parameters
0xA0-0xBF track/mixer parameters
0xC0-0xDF channel/send/effect parameters
0xE0-0xEF GLOBAL/master namespace
0xF0-0xFE future/reserved
0xFF      INVALID
```

The range label does not automatically assign a byte. Only registry entries explicitly marked assigned/lockable are valid.

Current assigned semantic IDs include:

```text
0x01 FMT_PARAM_VOICE_LEVEL
0x02 FMT_PARAM_PITCH_FINE
0x20 FMT_PARAM_8BIT_ATTACK
0x21 FMT_PARAM_8BIT_DECAY
0x22 FMT_PARAM_8BIT_SUSTAIN
0x23 FMT_PARAM_8BIT_RELEASE
0x24 FMT_PARAM_8BIT_WIDTH
0x25 FMT_PARAM_8BIT_TABLE
0xA0 FMT_PARAM_TRACK_LEVEL
0xA1 FMT_PARAM_TRACK_PAN
```

Exact C symbol spelling remains the production registry's authority; the important contract here is the stable numeric semantic identity frozen by D027/D044.

D047 does **not** renumber existing IDs. New FM4, modern, drum or other engine parameters require explicit collision-safe assignments in their owning packages. If the current namespace partition becomes too restrictive, a future superseding decision must preserve every already-assigned released semantic ID.

Rules:

- numeric ID is semantic identity, never encoder position;
- unassigned/reserved IDs are not lockable;
- a lock irrelevant to the resolved instrument/engine is ignored safely, never reinterpreted as another parameter;
- tracker effect command bytes remain a separate identity domain;
- GLOBAL identity does not by itself authorize track-owned p-lock storage.

## 7. Tracker effects

`FmtStep.effect` + `effect_value` are separate from p-lock IDs.

Current assigned command domain after D032/D033:

```text
0x00 NONE
0x01 ARPEGGIO
0x02 SLIDE_UP
0x03 SLIDE_DOWN
0x04 PORTAMENTO
0x05 VIBRATO
0x06 RETRIG
0x07 NOTE_DELAY
0x08 NOTE_CUT
0x09-0xFF reserved/safe NONE fallback
```

These are tracker/sequencer semantics, not private 8-BIT engine commands. New engine families must adapt the semantic actions where applicable while retaining D020/D032 timing.

## 8. Track state

Track configuration remains separate from pattern events:

```c
typedef struct {
    uint8_t instrument;
    uint8_t mute;
    uint8_t solo;
    uint8_t reserved;
} FmtTrackConfig;
```

D047 interpretation:

- a logical track has no permanent sound family;
- `instrument` selects its default/current instrument slot;
- the instrument slot determines engine family + engine;
- runtime state may hold current note, resolved instrument, effect state and a bounded engine-family voice handle/route;
- runtime voice handles are not persistent song data.

The eight logical tracks remain fixed even when engine families or polyphony differ.

## 9. Instruments

There are 16 instrument slots.

The current in-memory common header remains exactly 4 B:

```c
typedef struct {
    uint8_t mode;
    uint8_t engine;
    uint8_t version;
    uint8_t flags;
} FmtInstrumentHeader;
```

### D047 semantic amendment

The existing `mode` byte becomes conceptually the **engine-family discriminator** for mixed-engine architecture.

Current compatibility values remain:

```text
0 = current 8-BIT/chip family
1 = current 16-BIT/FM-digital family
```

The source field is deliberately not renamed during WP4.10 because D046 freezes production code. WP5.0 decides whether to keep `mode`, add a semantic alias, or perform a measured source-level rename while preserving the exact 4-byte header and existing values unless an explicit migration decision says otherwise.

`engine` selects an engine within that family.

The target meaning is therefore:

```text
FmtInstrumentHeader.mode   -> family
FmtInstrumentHeader.engine -> engine within family
```

Different instrument slots may use different families in the same project.

Future persistent engine-specific payloads must be tagged/versioned and length-delimited so unknown engines can be rejected/skipped safely.

## 10. Song/order model

Order entry remains:

```c
typedef struct {
    uint8_t pattern;
    uint8_t repeats;
} FmtOrderEntry;
```

The current runtime `FmtSongMeta` is exactly 8 B and includes a `mode` byte for historical D045 compatibility.

### D047 rule for song-level mode

`FmtSongMeta.mode` remains physically present through frozen Phase 4, but it is **no longer the target project-wide playback eligibility gate**.

WP5.0 must redefine its runtime meaning as a non-gating value such as legacy/default-family metadata, import hint or reserved field. The exact choice is frozen there.

Phase 6 persistence must not serialize a rule that forbids valid instruments merely because they belong to different engine families.

## 11. Persistent project header

The exact byte layout is still Phase 6 work.

Conceptually it requires:

```text
magic
format_major
format_minor
payload_bytes
crc32
flags
reserved/versioned metadata
```

A mandatory project-wide sound-family field is **not** required by D047.

Engine family + engine identity belongs to serialized instrument records. If a future project header carries a preferred/default family hint, it must not invalidate other valid instrument families.

Persistence rules:

- explicit byte order;
- no compiler-padding dependency;
- documented CRC region;
- unsupported major versions fail safely;
- minor additions are skippable/defaultable where possible;
- unknown instrument family/engine fails safely without reinterpretation.

## 12. Chords and polyphony

Chord recording remains desired without inflating every step record.

The tracker track count and engine voice count are separate concepts.

D047 reinforces:

- do not reduce eight logical tracks silently to obtain polyphony;
- a polyphonic instrument may consume more engine voices than a monophonic one;
- exact chord storage/capture policy is frozen in a later package;
- engine/resource limits must produce explicit deterministic behavior, not random note loss or engine substitution.

## 13. Runtime event/publication handoff

Pattern storage is not the real-time audio event representation.

Current architecture includes:

- D034 exact 8-byte SPSC audition event envelope with 32-entry capacity;
- D045 separate fixed four-byte multi-track row events for tracker row dispatch;
- D031/D032/D044 aligned row snapshots/caches for audio-safe publication.

D047 does not require putting tracker timing into D034 and does not create a generic dynamic event bus.

WP5.0 must define the narrow bounded common instrument-router handoff needed for mixed families while preserving:

- fixed-size/bounded work;
- no pointers into concurrently mutable editor data;
- no D028 traversal in audio;
- deterministic row/tick ordering;
- no second tracker clock.

## 14. Memory summary

Core editable song plus D028 lock records:

```text
FmtSong core              65,896 B
lock records              16,384 B
---------------------------------
editable core + locks     82,280 B
```

The lock allocator head/count is ordinary RAM.

Measured Phase 4 linked total pool remains 113,000 B. Engine-family runtime state is budgeted separately from editable-song storage.

D047 deliberately reuses existing instrument indirection instead of expanding every `FmtStep` or duplicating song data per engine family.

## 15. Invariants for tests

Current/future tests must preserve at minimum:

- every pattern/track/step index stays in range;
- `FmtStep` remains exactly 8 B unless an explicit migration decision supersedes it;
- all 8,192 step-owner IDs are unique;
- no valid step list exceeds four locks;
- global 2,048-record exhaustion/recovery is deterministic;
- wrong-owner/malformed lock indexes cause no partial mutation;
- invalid/unassigned parameter IDs are never stored;
- tracker effect invalid bytes fail safely;
- instrument inheritance is deterministic;
- unsupported family/engine fails toward silence rather than aliasing another engine;
- once WP5.0 exists, two or more valid engine families can coexist across tracks in one project;
- cross-family instrument change cannot leave two owned primary voices for one track;
- one track may not steal another track's still-owned active primary voice;
- 8-BIT-only behavior remains a regression baseline under the mixed router;
- serializers round-trip special notes, inheritance and per-instrument family/engine identity once persistence exists.
