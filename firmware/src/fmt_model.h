/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_MODEL_H
#define FM1TRACKER_FMT_MODEL_H

#include <stddef.h>
#include <stdint.h>

/* V0.1 dimensions frozen by D005 / DATA_MODEL_V0.1.md. */
#define FMT_TRACKS               8u
#define FMT_STEPS_PER_PATTERN   64u
#define FMT_PATTERN_SLOTS       16u
#define FMT_ORDER_ENTRIES      128u
#define FMT_INSTRUMENTS         16u
#define FMT_LOCKS_PER_STEP       4u
#define FMT_LOCK_POOL_MAX     2048u

/* Editable-song pool target. The lock bytes are a conservative budget only;
 * WP2.1 deliberately does not freeze a lock-record ABI or allocator. */
#define FMT_EDITABLE_SONG_POOL_BUDGET       (96u * 1024u)
#define FMT_LOCK_BUDGET_BYTES_PER_ENTRY      8u
#define FMT_LOCK_STORAGE_BUDGET             (FMT_LOCK_POOL_MAX * FMT_LOCK_BUDGET_BYTES_PER_ENTRY)

/* Special values are part of the V0.1 semantic model. */
enum {
    FMT_NOTE_MIN = 0,
    FMT_NOTE_MAX = 127,
    FMT_NOTE_TIE = 0xFD,
    FMT_NOTE_OFF = 0xFE,
    FMT_NOTE_EMPTY = 0xFF,
    FMT_INSTRUMENT_INHERIT = 0xFF,
    FMT_VOLUME_INHERIT = 0xFF,
    FMT_EFFECT_NONE = 0,
    FMT_LOCK_NONE = 0xFFFF
};

/* Exact V0.1 target: 8 bytes. This is an in-memory representation, not the
 * persistent project byte format. */
typedef struct {
    uint8_t note;
    uint8_t instrument;
    uint8_t volume;
    uint8_t effect;
    uint8_t effect_value;
    uint8_t flags;
    uint16_t lock_head;
} FmtStep;

typedef struct {
    FmtStep steps[FMT_TRACKS][FMT_STEPS_PER_PATTERN];
} FmtPattern;

typedef struct {
    uint8_t instrument;
    uint8_t mute;
    uint8_t solo;
    uint8_t reserved;
} FmtTrackConfig;

typedef struct {
    uint8_t mode;
    uint8_t engine;
    uint8_t version;
    uint8_t flags;
} FmtInstrumentHeader;

typedef struct {
    uint8_t pattern;
    uint8_t repeats;
} FmtOrderEntry;

/* Runtime/editable metadata only. Persistence gets an explicitly serialized
 * layout later in Phase 6; raw compiler structs must never be written as the
 * file format. BPM is integer in V0.1. */
typedef struct {
    uint16_t bpm;
    uint8_t mode;
    uint8_t order_length;
    uint8_t loop_start;
    uint8_t loop_end;
    uint8_t default_pattern_length;
    uint8_t flags;
} FmtSongMeta;

/* Compile-time core song shape. WP2.1 defines this type for deterministic
 * sizing/access only; WP2.2 owns actual backing-store allocation/placement. */
typedef struct {
    FmtSongMeta meta;
    FmtTrackConfig tracks[FMT_TRACKS];
    FmtOrderEntry order[FMT_ORDER_ENTRIES];
    FmtInstrumentHeader instruments[FMT_INSTRUMENTS];
    FmtPattern patterns[FMT_PATTERN_SLOTS];
} FmtSong;

/* Old JieLi clang compatibility: use typedef assertions rather than depending
 * on a particular C11 _Static_assert mode. */
#define FMT_STATIC_ASSERT(name, expr) typedef char fmt_static_assert_##name[(expr) ? 1 : -1]

FMT_STATIC_ASSERT(step_size_is_8, sizeof(FmtStep) == 8u);
FMT_STATIC_ASSERT(step_lock_head_offset_is_6, offsetof(FmtStep, lock_head) == 6u);
FMT_STATIC_ASSERT(pattern_size_is_4096, sizeof(FmtPattern) == 4096u);
FMT_STATIC_ASSERT(track_config_size_is_4, sizeof(FmtTrackConfig) == 4u);
FMT_STATIC_ASSERT(instrument_header_size_is_4, sizeof(FmtInstrumentHeader) == 4u);
FMT_STATIC_ASSERT(order_entry_size_is_2, sizeof(FmtOrderEntry) == 2u);
FMT_STATIC_ASSERT(song_meta_size_is_8, sizeof(FmtSongMeta) == 8u);
FMT_STATIC_ASSERT(song_core_size_is_65896, sizeof(FmtSong) == 65896u);
FMT_STATIC_ASSERT(song_core_plus_lock_budget_fits_96k,
                  sizeof(FmtSong) + FMT_LOCK_STORAGE_BUDGET <= FMT_EDITABLE_SONG_POOL_BUDGET);
FMT_STATIC_ASSERT(lock_head_is_16_bit, sizeof(((FmtStep *)0)->lock_head) == 2u);

static inline void fmt_step_init(FmtStep *step)
{
    if (!step)
        return;
    step->note = FMT_NOTE_EMPTY;
    step->instrument = FMT_INSTRUMENT_INHERIT;
    step->volume = FMT_VOLUME_INHERIT;
    step->effect = FMT_EFFECT_NONE;
    step->effect_value = 0u;
    step->flags = 0u;
    step->lock_head = FMT_LOCK_NONE;
}

static inline FmtPattern *fmt_song_pattern(FmtSong *song, uint32_t pattern)
{
    if (!song || pattern >= FMT_PATTERN_SLOTS)
        return (FmtPattern *)0;
    return &song->patterns[pattern];
}

static inline const FmtPattern *fmt_song_pattern_const(const FmtSong *song, uint32_t pattern)
{
    if (!song || pattern >= FMT_PATTERN_SLOTS)
        return (const FmtPattern *)0;
    return &song->patterns[pattern];
}

static inline FmtStep *fmt_pattern_step(FmtPattern *pattern, uint32_t track, uint32_t step)
{
    if (!pattern || track >= FMT_TRACKS || step >= FMT_STEPS_PER_PATTERN)
        return (FmtStep *)0;
    return &pattern->steps[track][step];
}

static inline const FmtStep *fmt_pattern_step_const(const FmtPattern *pattern, uint32_t track, uint32_t step)
{
    if (!pattern || track >= FMT_TRACKS || step >= FMT_STEPS_PER_PATTERN)
        return (const FmtStep *)0;
    return &pattern->steps[track][step];
}

static inline FmtStep *fmt_song_step(FmtSong *song, uint32_t pattern, uint32_t track, uint32_t step)
{
    FmtPattern *p = fmt_song_pattern(song, pattern);
    return fmt_pattern_step(p, track, step);
}

static inline const FmtStep *fmt_song_step_const(const FmtSong *song, uint32_t pattern, uint32_t track, uint32_t step)
{
    const FmtPattern *p = fmt_song_pattern_const(song, pattern);
    return fmt_pattern_step_const(p, track, step);
}

#endif /* FM1TRACKER_FMT_MODEL_H */
