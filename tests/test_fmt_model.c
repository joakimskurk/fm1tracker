/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "fmt_model.h"

static FmtSong song;

static void test_constants(void)
{
    assert(FMT_TRACKS == 8u);
    assert(FMT_STEPS_PER_PATTERN == 64u);
    assert(FMT_PATTERN_SLOTS == 16u);
    assert(FMT_ORDER_ENTRIES == 128u);
    assert(FMT_INSTRUMENTS == 16u);
    assert(FMT_LOCKS_PER_STEP == 4u);
    assert(FMT_LOCK_POOL_MAX == 2048u);

    assert(FMT_NOTE_MIN == 0);
    assert(FMT_NOTE_MAX == 127);
    assert(FMT_NOTE_TIE == 0xFD);
    assert(FMT_NOTE_OFF == 0xFE);
    assert(FMT_NOTE_EMPTY == 0xFF);
    assert(FMT_INSTRUMENT_INHERIT == 0xFF);
    assert(FMT_VOLUME_INHERIT == 0xFF);
    assert(FMT_EFFECT_NONE == 0);
    assert(FMT_LOCK_NONE == 0xFFFF);
}

static void test_sizes(void)
{
    assert(sizeof(FmtStep) == 8u);
    assert(offsetof(FmtStep, lock_head) == 6u);
    assert(sizeof(FmtPattern) == 4096u);
    assert(sizeof(FmtTrackConfig) == 4u);
    assert(sizeof(FmtInstrumentHeader) == 4u);
    assert(sizeof(FmtOrderEntry) == 2u);
    assert(sizeof(FmtSongMeta) == 8u);
    assert(sizeof(FmtSong) == 65896u);
    assert(sizeof(FmtSong) + FMT_LOCK_STORAGE_BUDGET <= FMT_EDITABLE_SONG_POOL_BUDGET);
}

static void test_step_defaults(void)
{
    FmtStep step = {0};

    fmt_step_init(&step);
    assert(step.note == FMT_NOTE_EMPTY);
    assert(step.instrument == FMT_INSTRUMENT_INHERIT);
    assert(step.volume == FMT_VOLUME_INHERIT);
    assert(step.effect == FMT_EFFECT_NONE);
    assert(step.effect_value == 0u);
    assert(step.flags == 0u);
    assert(step.lock_head == FMT_LOCK_NONE);

    /* Null initialization is intentionally a safe no-op. */
    fmt_step_init((FmtStep *)0);
}

static void test_pattern_bounds(void)
{
    FmtPattern pattern;
    const FmtPattern *cp = &pattern;

    assert(fmt_pattern_step(&pattern, 0u, 0u) == &pattern.steps[0][0]);
    assert(fmt_pattern_step(&pattern, FMT_TRACKS - 1u, FMT_STEPS_PER_PATTERN - 1u) ==
           &pattern.steps[FMT_TRACKS - 1u][FMT_STEPS_PER_PATTERN - 1u]);
    assert(fmt_pattern_step(&pattern, FMT_TRACKS, 0u) == (FmtStep *)0);
    assert(fmt_pattern_step(&pattern, 0u, FMT_STEPS_PER_PATTERN) == (FmtStep *)0);
    assert(fmt_pattern_step((FmtPattern *)0, 0u, 0u) == (FmtStep *)0);

    assert(fmt_pattern_step_const(cp, 0u, 0u) == &pattern.steps[0][0]);
    assert(fmt_pattern_step_const(cp, FMT_TRACKS, 0u) == (const FmtStep *)0);
    assert(fmt_pattern_step_const(cp, 0u, FMT_STEPS_PER_PATTERN) == (const FmtStep *)0);
    assert(fmt_pattern_step_const((const FmtPattern *)0, 0u, 0u) == (const FmtStep *)0);
}

static void test_song_bounds(void)
{
    const FmtSong *cs = &song;

    assert(fmt_song_pattern(&song, 0u) == &song.patterns[0]);
    assert(fmt_song_pattern(&song, FMT_PATTERN_SLOTS - 1u) == &song.patterns[FMT_PATTERN_SLOTS - 1u]);
    assert(fmt_song_pattern(&song, FMT_PATTERN_SLOTS) == (FmtPattern *)0);
    assert(fmt_song_pattern((FmtSong *)0, 0u) == (FmtPattern *)0);

    assert(fmt_song_pattern_const(cs, 0u) == &song.patterns[0]);
    assert(fmt_song_pattern_const(cs, FMT_PATTERN_SLOTS) == (const FmtPattern *)0);

    assert(fmt_song_step(&song, 0u, 0u, 0u) == &song.patterns[0].steps[0][0]);
    assert(fmt_song_step(&song,
                         FMT_PATTERN_SLOTS - 1u,
                         FMT_TRACKS - 1u,
                         FMT_STEPS_PER_PATTERN - 1u) ==
           &song.patterns[FMT_PATTERN_SLOTS - 1u]
                .steps[FMT_TRACKS - 1u][FMT_STEPS_PER_PATTERN - 1u]);
    assert(fmt_song_step(&song, FMT_PATTERN_SLOTS, 0u, 0u) == (FmtStep *)0);
    assert(fmt_song_step(&song, 0u, FMT_TRACKS, 0u) == (FmtStep *)0);
    assert(fmt_song_step(&song, 0u, 0u, FMT_STEPS_PER_PATTERN) == (FmtStep *)0);
    assert(fmt_song_step((FmtSong *)0, 0u, 0u, 0u) == (FmtStep *)0);

    assert(fmt_song_step_const(cs, 0u, 0u, 0u) == &song.patterns[0].steps[0][0]);
    assert(fmt_song_step_const(cs, FMT_PATTERN_SLOTS, 0u, 0u) == (const FmtStep *)0);
}

int main(void)
{
    test_constants();
    test_sizes();
    test_step_defaults();
    test_pattern_bounds();
    test_song_bounds();

    printf("FMT_MODEL_TESTS_OK=1\n");
    printf("FmtStep=%zu FmtPattern=%zu FmtSong=%zu lock_budget=%u total_budgeted=%zu\n",
           sizeof(FmtStep), sizeof(FmtPattern), sizeof(FmtSong),
           (unsigned)FMT_LOCK_STORAGE_BUDGET,
           sizeof(FmtSong) + (size_t)FMT_LOCK_STORAGE_BUDGET);
    return 0;
}
