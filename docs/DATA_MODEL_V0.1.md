# FM1Tracker V0.1 Data Model

Status: **design freeze candidate**

The data model must stay compact, deterministic and versionable. It is designed for fixed-capacity firmware storage, not for arbitrary desktop-sized songs.

All persistent structures require explicit serialization. Do not write raw compiler structs directly to flash as the file format.

## 1. Constants

Proposed V0.1 limits:

```c
#define FMT_TRACKS              8
#define FMT_STEPS_PER_PATTERN  64
#define FMT_PATTERN_SLOTS      16
#define FMT_ORDER_ENTRIES     128
#define FMT_INSTRUMENTS        16
#define FMT_LOCKS_PER_STEP      4
#define FMT_LOCK_POOL_MAX    2048
```

These values are chosen to keep the complete editable song comfortably inside pool RAM while leaving substantial room for DSP buffers.

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
    uint16_t lock_head;   /* sparse lock list index; 0xFFFF = none */
} FmtStep;
```

The final implementation may reorder fields for alignment but must preserve the 8-byte target unless profiling proves that a larger representation is worth the cost.

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

The first implementation should place this storage in the Dinghy pool section.

## 5. Parameter locks

Locks are sparse and separately allocated.

Logical record:

```c
typedef struct {
    uint8_t  param_id;
    uint8_t  next;       /* implementation may use wider index */
    uint16_t value;
} FmtLock;
```

The exact list representation is implementation-specific. The persistent model must preserve:

- stable parameter ID;
- integer value;
- association with a particular step;
- at most four active locks on one V0.1 step.

Because `FMT_LOCK_POOL_MAX` exceeds 255, the actual in-memory linked/free-list index will normally need 16 bits. A practical implementation can therefore use a 6-byte packed record or an 8-byte naturally aligned record, or use per-step fixed four-slot lock storage if measurement proves simpler/faster.

**Do not choose the final lock allocator until host profiling compares the options.** The semantic contract matters more than the container.

Worst-case bounded storage if each lock is 4 bytes and every step uses all four locks:

```text
8 tracks * 64 steps * 4 locks = 2048 locks
2048 * 4 bytes                = 8192 bytes
```

If the implementation requires wider indexes and grows each record, update the pool budget explicitly.

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

Approximate editable song footprint before instruments/runtime data:

```text
patterns                 65,536 B
max compact lock pool     8,192 B
order list                  256 B
track/song metadata        <1 KiB
---------------------------------
base target              ~74 KiB
```

Allowing for allocator metadata, larger lock indexes and instruments, Phase 1 should enforce a **96 KiB pool budget for the full editable song model**.

That leaves roughly 240 KiB of the Dinghy pool for DSP/sample/delay/UI buffers, subject to the actual imported platform build report.

## 15. Invariants for tests

Host tests must verify at minimum:

- every pattern index stays in range;
- every step index stays in range;
- REST advances exactly once;
- pattern 64 -> pattern step 1 wrap is deterministic;
- adding a fifth p-lock is rejected/replaced according to a documented rule;
- clearing a step releases all lock references owned by that step;
- malformed effect IDs are treated as NONE/safe error;
- malformed persistent indexes never become raw pointer access;
- serializers round-trip every special note value and inheritance value.
