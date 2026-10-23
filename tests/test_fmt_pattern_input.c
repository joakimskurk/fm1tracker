/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmt_pattern_input.h"
#include "fmt_store.h"

static const uint8_t WHITE_KEYS[FMT_PATTERN_INPUT_STEPS_PER_PAGE] = {
    0u, 2u, 4u, 6u, 7u, 9u, 11u, 12u,
    14u, 16u, 18u, 19u, 21u, 23u, 24u, 26u
};

static const uint8_t TRACK_KEYS[FMT_PATTERN_INPUT_TRACK_COUNT] = {
    1u, 3u, 5u, 8u, 10u, 13u, 15u, 17u
};

static const uint8_t RESERVED_BLACK_KEYS[3] = {20u, 22u, 25u};

static int expected_local(uint32_t key)
{
    uint32_t i;
    for (i = 0u; i < FMT_PATTERN_INPUT_STEPS_PER_PAGE; i++)
        if (WHITE_KEYS[i] == key)
            return (int)i;
    return -1;
}

static int expected_track(uint32_t key)
{
    uint32_t i;
    for (i = 0u; i < FMT_PATTERN_INPUT_TRACK_COUNT; i++)
        if (TRACK_KEYS[i] == key)
            return (int)i;
    return -1;
}

static void test_reset_and_context(void)
{
    FmtPatternInput input = {99u, 99u, 99u, 99u};

    fmt_pattern_input_reset(&input);
    assert(sizeof input == 4u);
    assert(input.context == FMT_PATTERN_CONTEXT_PERFORMANCE);
    assert(input.page == 0u);
    assert(input.local_step == 0u);
    assert(input.selected_track == 0u);
    assert(fmt_pattern_input_global_step(&input) == 0u);
    assert(!fmt_pattern_input_is_pattern(&input));

    assert(!fmt_pattern_input_select_key(&input, WHITE_KEYS[5]));
    assert(!fmt_pattern_input_select_track_key(&input, TRACK_KEYS[5]));
    assert(!fmt_pattern_input_advance_step(&input));
    assert(input.local_step == 0u);
    assert(input.selected_track == 0u);
    assert(!fmt_pattern_input_page_next(&input));

    fmt_pattern_input_enter_pattern(&input);
    assert(fmt_pattern_input_is_pattern(&input));
    assert(fmt_pattern_input_select_key(&input, WHITE_KEYS[5]));
    assert(fmt_pattern_input_select_track_key(&input, TRACK_KEYS[5]));
    assert(input.local_step == 5u);
    assert(input.selected_track == 5u);
    assert(fmt_pattern_input_global_step(&input) == 5u);

    fmt_pattern_input_enter_performance(&input);
    assert(!fmt_pattern_input_is_pattern(&input));
    assert(input.selected_track == 5u);
    assert(!fmt_pattern_input_select_key(&input, WHITE_KEYS[9]));
    assert(!fmt_pattern_input_select_track_key(&input, TRACK_KEYS[2]));
    assert(!fmt_pattern_input_advance_step(&input));
    assert(input.local_step == 5u);
    assert(input.selected_track == 5u);

    fmt_pattern_input_enter_pattern(&input);
    assert(input.selected_track == 5u);

    fmt_pattern_input_reset(NULL);
    fmt_pattern_input_enter_pattern(NULL);
    fmt_pattern_input_enter_performance(NULL);
    assert(!fmt_pattern_input_is_pattern(NULL));
    assert(!fmt_pattern_input_select_key(NULL, 0u));
    assert(!fmt_pattern_input_select_track_key(NULL, 1u));
    assert(!fmt_pattern_input_page_prev(NULL));
    assert(!fmt_pattern_input_page_next(NULL));
    assert(!fmt_pattern_input_advance_step(NULL));
    assert(fmt_pattern_input_global_step(NULL) == 0u);
}

static void test_physical_key_maps(void)
{
    uint32_t key;
    uint8_t seen_steps[FMT_PATTERN_INPUT_STEPS_PER_PAGE] = {0};
    uint8_t seen_tracks[FMT_PATTERN_INPUT_TRACK_COUNT] = {0};

    for (key = 0u; key < FMT_PATTERN_INPUT_KEY_COUNT; key++) {
        uint8_t local = 0xA5u, track = 0xA5u;
        int expected_l = expected_local(key);
        int expected_t = expected_track(key);
        int step_ok = fmt_pattern_input_key_to_local(key, &local);
        int track_ok = fmt_pattern_input_key_to_track(key, &track);

        assert(!(step_ok && track_ok));
        if (expected_l >= 0) {
            assert(step_ok);
            assert(local == (uint8_t)expected_l);
            seen_steps[local]++;
        } else {
            assert(!step_ok);
            assert(local == 0u);
        }

        if (expected_t >= 0) {
            assert(track_ok);
            assert(track == (uint8_t)expected_t);
            seen_tracks[track]++;
        } else {
            assert(!track_ok);
            assert(track == 0u);
        }
    }

    for (key = 0u; key < FMT_PATTERN_INPUT_STEPS_PER_PAGE; key++)
        assert(seen_steps[key] == 1u);
    for (key = 0u; key < FMT_PATTERN_INPUT_TRACK_COUNT; key++)
        assert(seen_tracks[key] == 1u);

    assert(!fmt_pattern_input_key_to_local(FMT_PATTERN_INPUT_KEY_COUNT, NULL));
    assert(!fmt_pattern_input_key_to_local(UINT32_MAX, NULL));
    assert(!fmt_pattern_input_key_to_track(FMT_PATTERN_INPUT_KEY_COUNT, NULL));
    assert(!fmt_pattern_input_key_to_track(UINT32_MAX, NULL));
}

static void test_all_64_addresses_and_store_integrity(void)
{
    FmtPatternInput input;
    uint8_t seen[FMT_STEPS_PER_PATTERN] = {0};
    FmtSong *before;
    uint32_t page, local, track;

    fmt_store_reset();
    before = malloc(sizeof *before);
    assert(before);
    memcpy(before, fmt_store_song_const(), sizeof *before);

    fmt_pattern_input_reset(&input);
    fmt_pattern_input_enter_pattern(&input);

    for (track = 0u; track < FMT_PATTERN_INPUT_TRACK_COUNT; track++) {
        assert(fmt_pattern_input_select_track_key(&input, TRACK_KEYS[track]));
        assert(input.selected_track == track);
        assert(input.page == 0u);
        assert(input.local_step == 0u);
    }

    for (page = 0u; page < FMT_PATTERN_INPUT_PAGE_COUNT; page++) {
        if (page != 0u)
            assert(fmt_pattern_input_page_next(&input));
        assert(input.page == page);

        for (local = 0u; local < FMT_PATTERN_INPUT_STEPS_PER_PAGE; local++) {
            uint8_t step;
            uint8_t selected_track = input.selected_track;
            assert(fmt_pattern_input_select_key(&input, WHITE_KEYS[local]));
            step = fmt_pattern_input_global_step(&input);
            assert(step == page * FMT_PATTERN_INPUT_STEPS_PER_PAGE + local);
            assert(step < FMT_STEPS_PER_PATTERN);
            assert(input.selected_track == selected_track);
            seen[step]++;
        }
    }

    for (local = 0u; local < FMT_STEPS_PER_PATTERN; local++)
        assert(seen[local] == 1u);

    assert(memcmp(before, fmt_store_song_const(), sizeof *before) == 0);
    free(before);
}

static void test_clamped_pages_preserve_local_step_and_track(void)
{
    FmtPatternInput input;

    fmt_pattern_input_reset(&input);
    fmt_pattern_input_enter_pattern(&input);
    assert(fmt_pattern_input_select_key(&input, WHITE_KEYS[10]));
    assert(fmt_pattern_input_select_track_key(&input, TRACK_KEYS[6]));
    assert(fmt_pattern_input_global_step(&input) == 10u);

    assert(!fmt_pattern_input_page_prev(&input));
    assert(input.page == 0u && input.local_step == 10u && input.selected_track == 6u);

    assert(fmt_pattern_input_page_next(&input));
    assert(input.page == 1u && input.local_step == 10u && input.selected_track == 6u);
    assert(fmt_pattern_input_global_step(&input) == 26u);

    assert(fmt_pattern_input_page_next(&input));
    assert(input.page == 2u && input.local_step == 10u && input.selected_track == 6u);
    assert(fmt_pattern_input_global_step(&input) == 42u);

    assert(fmt_pattern_input_page_next(&input));
    assert(input.page == 3u && input.local_step == 10u && input.selected_track == 6u);
    assert(fmt_pattern_input_global_step(&input) == 58u);

    assert(!fmt_pattern_input_page_next(&input));
    assert(input.page == 3u && input.local_step == 10u && input.selected_track == 6u);

    assert(fmt_pattern_input_page_prev(&input));
    assert(fmt_pattern_input_page_prev(&input));
    assert(fmt_pattern_input_page_prev(&input));
    assert(!fmt_pattern_input_page_prev(&input));
    assert(fmt_pattern_input_global_step(&input) == 10u);
    assert(input.selected_track == 6u);
}

static void test_track_selection_preserves_step_and_reserved_keys(void)
{
    FmtPatternInput input;
    uint32_t i;

    fmt_pattern_input_reset(&input);
    fmt_pattern_input_enter_pattern(&input);
    assert(fmt_pattern_input_select_key(&input, WHITE_KEYS[8]));
    assert(fmt_pattern_input_page_next(&input));
    assert(fmt_pattern_input_global_step(&input) == 24u);

    for (i = 0u; i < FMT_PATTERN_INPUT_TRACK_COUNT; i++) {
        assert(fmt_pattern_input_select_track_key(&input, TRACK_KEYS[i]));
        assert(input.selected_track == i);
        assert(input.page == 1u);
        assert(input.local_step == 8u);
        assert(fmt_pattern_input_global_step(&input) == 24u);
    }

    for (i = 0u; i < 3u; i++) {
        uint8_t before = input.selected_track;
        assert(!fmt_pattern_input_select_track_key(&input, RESERVED_BLACK_KEYS[i]));
        assert(input.selected_track == before);
        assert(fmt_pattern_input_global_step(&input) == 24u);
    }

    assert(!fmt_pattern_input_select_track_key(&input, FMT_PATTERN_INPUT_KEY_COUNT));
    assert(!fmt_pattern_input_select_track_key(&input, UINT32_MAX));
    assert(input.selected_track == 7u);
}

static void test_cursor_advance_exactly_once_and_wraps(void)
{
    FmtPatternInput input;
    uint32_t step;

    fmt_pattern_input_reset(&input);
    fmt_pattern_input_enter_pattern(&input);
    assert(fmt_pattern_input_select_track_key(&input, TRACK_KEYS[4]));

    for (step = 0u; step < FMT_STEPS_PER_PATTERN; step++) {
        assert(fmt_pattern_input_global_step(&input) == step);
        assert(input.selected_track == 4u);
        assert(fmt_pattern_input_advance_step(&input));
        assert(input.selected_track == 4u);
        assert(fmt_pattern_input_global_step(&input) == (uint8_t)((step + 1u) % FMT_STEPS_PER_PATTERN));
    }

    assert(input.page == 0u);
    assert(input.local_step == 0u);
    assert(input.selected_track == 4u);

    input.page = FMT_PATTERN_INPUT_PAGE_COUNT;
    assert(!fmt_pattern_input_advance_step(&input));
    input.page = 0u;
    input.local_step = FMT_PATTERN_INPUT_STEPS_PER_PAGE;
    assert(!fmt_pattern_input_advance_step(&input));
    input.local_step = 0u;
    input.selected_track = FMT_PATTERN_INPUT_TRACK_COUNT;
    assert(!fmt_pattern_input_advance_step(&input));
}

int main(void)
{
    test_reset_and_context();
    test_physical_key_maps();
    test_all_64_addresses_and_store_integrity();
    test_clamped_pages_preserve_local_step_and_track();
    test_track_selection_preserves_step_and_reserved_keys();
    test_cursor_advance_exactly_once_and_wraps();

    puts("FMT_PATTERN_INPUT_TESTS_OK=1");
    printf("state_bytes=%u pages=%u steps_per_page=%u total_steps=%u white_keys=%u tracks=%u track_keys=%u reserved_black=%u\n",
           (unsigned)sizeof(FmtPatternInput),
           (unsigned)FMT_PATTERN_INPUT_PAGE_COUNT,
           (unsigned)FMT_PATTERN_INPUT_STEPS_PER_PAGE,
           (unsigned)FMT_STEPS_PER_PATTERN,
           (unsigned)FMT_PATTERN_INPUT_STEPS_PER_PAGE,
           (unsigned)FMT_PATTERN_INPUT_TRACK_COUNT,
           (unsigned)FMT_PATTERN_INPUT_TRACK_COUNT,
           3u);
    return 0;
}
