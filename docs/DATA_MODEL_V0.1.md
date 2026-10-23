# FM1Tracker V0.1 Data Model

Status: **design freeze candidate**

The data model must stay compact, deterministic and versionable. It is designed for fixed-capacity firmware storage, not for arbitrary desktop-sized songs.

All persistent structures require explicit serialization. Do not write raw compiler structs directly to flash as the file format.

## 1. Constants

V0.1 limits:

```c
#define FMT_TRACKS              8
#define FMT_STEPS_PER_PATTERN  64
#define FMT_PATTERN_SLOTS      16
#define FMT_ORDER_ENTRIES     128
#define FMT_INSTRUMENTS        16
#define FMT_LOCKS_PER_STEP      4
#define FMT_LOCK_POOL_MAX    2048
```

`FMT_LOCK_POOL_MAX` is the global sparse lock capacity for the complete editable song. It is deliberately smaller than `8192 steps * 4 locks = 32768` because the V0.1 song-memory contract does not promise that every resident step can be fully locked at once. Any individual step may own at most four locks; all steps share the fixed 2,048-record pool.

## 2. Note representation

Use one byte.

```c
enum {
    FMT_NOTE_MIN   = 0,      /* MIDI-style pitch number */
    FMT_NOTE_MAX   = 127,
    FMT_NOTE_TIE   = 0xFD,
    FMT_NOTE_OFF   = 0xFE,
    FMT_NOTE_EMPTY = 0xFF
};
```

`EMPTY` means no note event on this step.

`OFF` explicitly releases the track/voice according to the current engine policy.

`TIE` continues the prior note without retriggering its envelope where supported.

A REST action in STEP REC normally writes/keeps `EMPTY` and advances. It does not implicitly write NOTE OFF.

## 3. Step record

Target size: **8 bytes**.

Logical representation:

```c
typedef struct {
    uint8_t  note;        /* 0..127, TIE, OFF, EMPTY */
    uint8_t  instrument;  /* 0..15, or 0xFF = inherit */
    uint8_t  volume;      /* 0..127, or 0xFF = inherit */
    uint8_t  effect;      /* effect ID, 0 = none */
    uint8_t  effect_value;/* compact effect argument */
    uint8_t  flags;       /* accent/chord/reserved flags */
    uint16_t lock_head;   /* sparse lock-list index; 0xFFFF = none */
} FmtStep;
```

The implementation preserves this exact 8-byte representation. `lock_head` remains a 16-bit index into the D028 global lock pool; it is never a raw pointer.

### Why inheritance values exist

Tracker rows frequently do not need to restate instrument or volume. `0xFF` means retain the currently active track value. This avoids forcing redundant writes during keyboard entry.

## 4. Pattern

A pattern contains eight logical tracks, each with 64 steps.

Conceptually:

```c
typedef struct {
    FmtStep steps[FMT_TRACKS][FMT_STEPS_PER_PATTERN];
} FmtPattern;
```

Memory:

```text
8 bytes * 8 tracks * 64 steps = 4096 bytes
```

Sixteen resident pattern slots:

```text
4096 * 16 = 65536 bytes
```

The runtime song storage lives in the Dinghy pool section.

## 5. Parameter locks

Locks are sparse and separately allocated from the fixed D028 pool.

D028 freezes the WP3.2 in-memory lock record at exactly 8 bytes:

```c
typedef struct {
    uint16_t next;      /* active-list or free-list index; 0xFFFF = end */
    uint16_t value;     /* full 0..65535 storage domain */
    uint16_t owner;     /* linear song-step owner 0..8191; 0xFFFF = none */
    FmtParamId param_id;
    uint8_t state;      /* FREE or ACTIVE */
} FmtLock;
```

WP3.2 in-memory offsets are:

```text
next      0
value     2
owner     4
param_id  6
state     7
sizeof    8
```

This raw compiler struct is **not** the persistent project format. Phase 6 must serialize lock semantics explicitly.

### Global sparse capacity

The complete song has:

```text
16 patterns * 8 tracks * 64 steps = 8192 steps
```

A theoretical four-lock allocation on every resident step would require:

```text
8192 steps * 4 locks = 32768 records
```

That is intentionally **not** the V0.1 RAM contract.

The actual D028 pool is globally bounded at:

```text
2048 records * 8 bytes = 16384 bytes
```

This is enough for, for example, one 512-step pattern with four locks on every row, or a much larger number of sparsely locked rows distributed across the song. Pool exhaustion is explicit and deterministic; existing locks are never discarded automatically to make room.

The earlier design example that described 2,048 records as the worst case for every song step omitted the sixteen pattern slots. D028 corrects that arithmetic while preserving the already-budgeted 2,048-record sparse capacity.

### Ownership and lists

`FmtStep.lock_head` is the first record index; `0xFFFF` means no list. Active lists are strictly sorted by ascending `FmtParamId` and contain at most four records.

Every active record carries the exact linear owner:

```text
pattern * (8 * 64) + track * 64 + step
```

which covers `0..8191`. This owner tag makes an in-range cross-linked/corrupt index detectable before another step's records can be read, replaced or freed.

Free records reuse `next` for a single free list. Reset chains record `0 -> 1 -> ... -> 2047 -> 0xFFFF`. Runtime allocator state is only a 16-bit free-list head plus a 16-bit used count.

Storage rules:

- only D027 registry IDs marked lockable may be active;
- duplicate parameter IDs are never stored on one step;
- setting an existing parameter replaces its value in place;
- a new parameter is inserted in sorted ID order;
- adding a fifth unique lock returns STEP_FULL without allocation;
- global exhaustion returns POOL_FULL without modifying existing song data;
- remove returns exactly one record to the free list;
- clear returns the complete validated step list exactly once;
- all mutations validate the existing bounded list first;
- malformed/out-of-range/cross-owned/unsorted/overlong lists return CORRUPT with no partial mutation.

The 16-bit value is opaque storage at this layer. Each parameter's owning engine/effect package defines scaling and valid semantic range later.

## 6. Parameter IDs

D027 freezes parameter identity as an explicit one-byte semantic domain independent of UI layout.

```c
typedef uint8_t FmtParamId;
```

Special values:

```text
0x00  NONE     no parameter; never an active p-lock target
0xFF  INVALID  invalid/corrupt; never assignable
```

V0.1 namespace ranges:

```text
0x01-0x1F common voice parameters
0x20-0x5F 8-BIT engine-specific parameters
0x60-0x9F 16-BIT engine-specific parameters
0xA0-0xBF track/mixer parameters
0xC0-0xDF channel/send/effect parameters
0xE0-0xEF future GLOBAL/master parameters; reserved in WP3.1
0xF0-0xFE future/reserved
0xFF      INVALID
```

A byte belonging to a namespace is **not automatically a valid parameter**. Only IDs present in the authoritative registry are assigned/lockable.

WP3.1 assigns only:

```text
0x01 FMT_PARAM_VOICE_LEVEL
0x02 FMT_PARAM_PITCH_FINE
0xA0 FMT_PARAM_TRACK_LEVEL
0xA1 FMT_PARAM_TRACK_PAN
```

Envelope, oscillator, wavetable/noise, FM operator, send/effect and GLOBAL/master parameter IDs remain deliberately unassigned until their owning work packages establish actual semantics.

`FmtStep.effect` is a separate tracker-command identity domain. Effect command IDs are not parameter IDs merely because an effect may later expose lockable controls.

Rules:

- numeric IDs are stable semantic identity, never encoder/page positions;
- registry labels are presentation/debug metadata, not persistence keys;
- unassigned/reserved IDs fail lookup safely and are not lockable;
- once an ID is used by a released persistent/interchange format, never recycle it for unrelated meaning;
- global/master identity does not imply track-owned p-lock storage; global automation ownership remains a future explicit decision.

## 7. Tracker effects

The compact effect slot uses an 8-bit effect ID + 8-bit argument.

Proposed initial IDs:

```c
typedef enum {
    FMT_FX_NONE = 0,
    FMT_FX_ARPEGGIO,
    FMT_FX_SLIDE_UP,
    FMT_FX_SLIDE_DOWN,
    FMT_FX_PORTAMENTO,
    FMT_FX_VIBRATO,
    FMT_FX_RETRIG,
    FMT_FX_NOTE_DELAY,
    FMT_FX_NOTE_CUT,
    FMT_FX_VOLUME_SLIDE
} FmtEffect;
```

Exact argument encoding is documented per effect. Do not inherit Furnace/ProTracker hexadecimal semantics accidentally; compatibility should be deliberate rather than assumed.

## 8. Track state

Track configuration is separate from pattern events.

Logical fields:

```c
typedef struct {
    uint8_t instrument;
    uint8_t mute;
    uint8_t solo;
    uint8_t reserved;
} FmtTrackConfig;
```

Runtime track state additionally contains current note, active instrument, effect state and engine voice handles. Runtime state is not automatically persistent song data.

## 9. Instruments

V0.1 provides 16 instrument slots.

Each instrument begins with a small common header:

```c
typedef struct {
    uint8_t mode;        /* 8-BIT / 16-BIT */
    uint8_t engine;      /* pulse/noise/FM/etc. */
    uint8_t version;
    uint8_t flags;
} FmtInstrumentHeader;
```

Engine-specific payload follows in a tagged/versioned representation.

The on-flash format must include payload length so newer firmware can skip an unknown instrument payload safely instead of reading past it.

V0.1 project-wide mode means instruments from the other mode are rejected or converted explicitly; they are never silently reinterpreted.

## 10. Song/order model

```c
typedef struct {
    uint8_t pattern;       /* 0..15 */
    uint8_t repeats;       /* 1..255; 0 treated as 1 or invalid by serializer */
} FmtOrderEntry;
```

128 entries consume only 256 bytes.

Song metadata contains at least:

```text
mode
bpm
order_length
loop_start
loop_end
default_pattern_length (64 in V0.1)
```

Pattern length may be stored even if V0.1 initially fixes it at 64, so future shorter patterns do not require a format rewrite.

## 11. Project header

Persistent project data begins with a serialized header conceptually containing:

```text
magic:        "FMT1"
format_major
format_minor
payload_bytes
crc32
mode
flags
reserved
```

The exact byte layout will be frozen only when persistence is implemented.

Rules:

- all multi-byte values use an explicitly chosen byte order;
- never depend on compiler padding;
- CRC covers a documented region;
- unsupported major versions fail safely;
- minor-version additions must be skippable or defaultable.

## 12. Chords

Chord recording is desired, but the storage model should not inflate every step before the engine behavior is proven.

V0.1 options to prototype:

1. distribute a chord over multiple logical tracks;
2. use a small chord/event pool referenced by `flags`/an auxiliary index;
3. constrain STEP REC to monophonic note entry initially while still auditioning polyphonically.

Do not silently discard chord notes. Until the chord representation is frozen, the UI must state the behavior for the active engine.

## 13. Event handoff to audio

Pattern storage is not the real-time event format.

The sequencer should decode rows into compact runtime events such as:

```c
typedef struct {
    uint8_t type;
    uint8_t track;
    uint16_t value_a;
    uint32_t value_b;
} FmtRtEvent;
```

The exact structure is provisional. Required properties:

- fixed size;
- bounded queue;
- no pointers into editable UI memory where a concurrent edit can invalidate meaning;
- timestamp/offset support if NOTE DELAY or sample-accurate dispatch requires it;
- deterministic queue-overflow policy visible in debug builds.

## 14. Memory summary target

Core editable song plus the frozen D028 lock records:

```text
FmtSong core              65,896 B
lock records              16,384 B
---------------------------------
editable core + locks     82,280 B
```

The lock allocator's four-byte runtime head/count lives in ordinary RAM rather than the editable-song pool.

The 82,280-byte core+lock total remains below the **96 KiB editable-song target**, leaving 16,024 bytes inside that target for later instrument/song-associated data before that policy would need revisiting.

Measured platform pool use is a separate whole-firmware number because Dinghy already consumes pool RAM for its own buffers. From the verified WP3.1 baseline of 96,616 B total pool use, adding the D028 16,384-byte record array predicts 113,000 B total pool use before linker measurement, leaving 231,064 B of the 344,064-byte pool.

## 15. Invariants for tests

Host tests must verify at minimum:

- every pattern index stays in range;
- every step index stays in range;
- REST advances exactly once;
- pattern 64 -> pattern step 1 wrap is deterministic;
- all 8,192 step-owner IDs map uniquely;
- no valid step list exceeds four locks;
- adding a fifth prevalidated unique p-lock is rejected as STEP_FULL;
- duplicate parameter set replaces in place and consumes no record;
- global 2,048-record exhaustion returns POOL_FULL without data loss;
- freeing one record after exhaustion permits exactly one new allocation;
- clearing a step releases all lock references owned by that step exactly once;
- wrong-owner or malformed indexes never become raw pointer access and cause no partial mutation;
- unassigned/invalid D027 parameter IDs are never stored;
- malformed effect IDs are treated as NONE/safe error;
- serializers round-trip every special note value and inheritance value once persistence exists.
