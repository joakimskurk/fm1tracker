/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "fmt_store.h"

static void assert_default_step(const FmtStep *s)
{
    assert(s != (const FmtStep *)0);
    assert(s->note == FMT_NOTE_EMPTY);
    assert(s->instrument == FMT_INSTRUMENT_INHERIT);
    assert(s->volume == FMT_VOLUME_INHERIT);
    assert(s->effect == FMT_EFFECT_NONE);
    assert(s->effect_value == 0u);
    assert(s->flags == 0u);
    assert(s->lock_head == FMT_LOCK_NONE);
}

static void test_reset_defaults_all_steps(void)
{
    uint32_t pattern, track, step;
    FmtSong *song;

    fmt_store_reset();
    song = fmt_store_song();
    assert(song != (FmtSong *)0);
    assert(fmt_store_song_const() == song);

    assert(song->meta.bpm == 0u);
    assert(song->meta.mode == 0u);
    assert(song->meta.order_length == 0u);
    assert(song->meta.loop_start == 0u);
    assert(song->meta.loop_end == 0u);
    assert(song->meta.default_pattern_length == FMT_STEPS_PER_PATTERN);
    assert(song->meta.flags == 0u);

    for (track = 0; track < FMT_TRACKS; track++) {
        assert(song->tracks[track].instrument == 0u);
        assert(song->tracks[track].mute == 0u);
        assert(song->tracks[track].solo == 0u);
        assert(song->tracks[track].reserved == 0u);
    }

    for (pattern = 0; pattern < FMT_PATTERN_SLOTS; pattern++)
        for (track = 0; track < FMT_TRACKS; track++)
            for (step = 0; step < FMT_STEPS_PER_PATTERN; step++)
                assert_default_step(fmt_store_step_const(pattern, track, step));
}

static void test_bounds(void)
{
    assert(fmt_store_pattern(0u) != (FmtPattern *)0);
    assert(fmt_store_pattern(FMT_PATTERN_SLOTS - 1u) != (FmtPattern *)0);
    assert(fmt_store_pattern(FMT_PATTERN_SLOTS) == (FmtPattern *)0);
    assert(fmt_store_pattern(UINT32_MAX) == (FmtPattern *)0);

    assert(fmt_store_pattern_const(0u) != (const FmtPattern *)0);
    assert(fmt_store_pattern_const(FMT_PATTERN_SLOTS) == (const FmtPattern *)0);

    assert(fmt_store_step(0u, 0u, 0u) != (FmtStep *)0);
    assert(fmt_store_step(FMT_PATTERN_SLOTS - 1u,
                          FMT_TRACKS - 1u,
                          FMT_STEPS_PER_PATTERN - 1u) != (FmtStep *)0);

    assert(fmt_store_step(FMT_PATTERN_SLOTS, 0u, 0u) == (FmtStep *)0);
    assert(fmt_store_step(0u, FMT_TRACKS, 0u) == (FmtStep *)0);
    assert(fmt_store_step(0u, 0u, FMT_STEPS_PER_PATTERN) == (FmtStep *)0);
    assert(fmt_store_step(UINT32_MAX, 0u, 0u) == (FmtStep *)0);
    assert(fmt_store_step(0u, UINT32_MAX, 0u) == (FmtStep *)0);
    assert(fmt_store_step(0u, 0u, UINT32_MAX) == (FmtStep *)0);

    assert(fmt_store_step_const(FMT_PATTERN_SLOTS, 0u, 0u) == (const FmtStep *)0);
    assert(fmt_store_step_const(0u, FMT_TRACKS, 0u) == (const FmtStep *)0);
    assert(fmt_store_step_const(0u, 0u, FMT_STEPS_PER_PATTERN) == (const FmtStep *)0);
}

static void test_mapping_and_isolation(void)
{
    FmtSong *song = fmt_store_song();
    FmtStep *first = fmt_store_step(0u, 0u, 0u);
    FmtStep *next = fmt_store_step(0u, 0u, 1u);
    FmtStep *next_track = fmt_store_step(0u, 1u, 0u);
    FmtStep *last = fmt_store_step(FMT_PATTERN_SLOTS - 1u,
                                   FMT_TRACKS - 1u,
                                   FMT_STEPS_PER_PATTERN - 1u);

    assert(first == &song->patterns[0].steps[0][0]);
    assert(next == &song->patterns[0].steps[0][1]);
    assert(next_track == &song->patterns[0].steps[1][0]);
    assert(last == &song->patterns[FMT_PATTERN_SLOTS - 1u]
                         .steps[FMT_TRACKS - 1u][FMT_STEPS_PER_PATTERN - 1u]);

    assert(first != next);
    assert(first != next_track);
    assert(first != last);

    first->note = 60u;
    first->instrument = 3u;
    first->volume = 99u;
    first->effect = 7u;
    first->effect_value = 42u;
    first->flags = 0xA5u;
    first->lock_head = 123u;

    assert_default_step(next);
    assert_default_step(next_track);
    assert_default_step(last);

    assert(first->note == 60u);
    assert(first->instrument == 3u);
    assert(first->volume == 99u);
    assert(first->effect == 7u);
    assert(first->effect_value == 42u);
    assert(first->flags == 0xA5u);
    assert(first->lock_head == 123u);
}

static void test_reset_after_mutation(void)
{
    FmtSong *song = fmt_store_song();
    FmtStep *first = fmt_store_step(0u, 0u, 0u);
    FmtStep *last = fmt_store_step(FMT_PATTERN_SLOTS - 1u,
                                   FMT_TRACKS - 1u,
                                   FMT_STEPS_PER_PATTERN - 1u);

    first->note = 48u;
    last->note = 72u;
    song->meta.bpm = 200u;
    song->tracks[3].mute = 1u;
    song->order[7].pattern = 12u;
    song->order[7].repeats = 9u;
    song->instruments[4].engine = 5u;

    fmt_store_reset();

    assert_default_step(fmt_store_step_const(0u, 0u, 0u));
    assert_default_step(fmt_store_step_const(FMT_PATTERN_SLOTS - 1u,
                                             FMT_TRACKS - 1u,
                                             FMT_STEPS_PER_PATTERN - 1u));
    assert(song->meta.bpm == 0u);
    assert(song->meta.default_pattern_length == FMT_STEPS_PER_PATTERN);
    assert(song->tracks[3].mute == 0u);
    assert(song->order[7].pattern == 0u);
    assert(song->order[7].repeats == 0u);
    assert(song->instruments[4].engine == 0u);
}

int main(void)
{
    test_reset_defaults_all_steps();
    test_bounds();
    test_mapping_and_isolation();
    test_reset_after_mutation();

    printf("FMT_STORE_TESTS_OK=1\n");
    printf("store_bytes=%zu steps=%u patterns=%u tracks=%u steps_per_pattern=%u\n",
           sizeof(FmtSong),
           (unsigned)(FMT_PATTERN_SLOTS * FMT_TRACKS * FMT_STEPS_PER_PATTERN),
           (unsigned)FMT_PATTERN_SLOTS,
           (unsigned)FMT_TRACKS,
           (unsigned)FMT_STEPS_PER_PATTERN);
    return 0;
}
