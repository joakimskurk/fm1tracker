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

static int expected_local(uint32_t key)
{
    uint32_t i;
    for (i = 0u; i < FMT_PATTERN_INPUT_STEPS_PER_PAGE; i++)
        if (WHITE_KEYS[i] == key)
            return (int)i;
    return -1;
}

static void test_reset_and_context(void)
{
    FmtPatternInput input = {99u, 99u, 99u};

    fmt_pattern_input_reset(&input);
    assert(sizeof input == 3u);
    assert(input.context == FMT_PATTERN_CONTEXT_PERFORMANCE);
    assert(input.page == 0u);
    assert(input.local_step == 0u);
    assert(fmt_pattern_input_global_step(&input) == 0u);
    assert(!fmt_pattern_input_is_pattern(&input));

    assert(!fmt_pattern_input_select_key(&input, WHITE_KEYS[5]));
    assert(input.local_step == 0u);
    assert(!fmt_pattern_input_page_next(&input));
    assert(input.page == 0u);

    fmt_pattern_input_enter_pattern(&input);
    assert(fmt_pattern_input_is_pattern(&input));
    assert(fmt_pattern_input_select_key(&input, WHITE_KEYS[5]));
    assert(input.local_step == 5u);
    assert(fmt_pattern_input_global_step(&input) == 5u);

    fmt_pattern_input_enter_performance(&input);
    assert(!fmt_pattern_input_is_pattern(&input));
    assert(!fmt_pattern_input_select_key(&input, WHITE_KEYS[9]));
    assert(input.local_step == 5u);

    fmt_pattern_input_reset(NULL);
    fmt_pattern_input_enter_pattern(NULL);
    fmt_pattern_input_enter_performance(NULL);
    assert(!fmt_pattern_input_is_pattern(NULL));
    assert(!fmt_pattern_input_select_key(NULL, 0u));
    assert(!fmt_pattern_input_page_prev(NULL));
    assert(!fmt_pattern_input_page_next(NULL));
    assert(fmt_pattern_input_global_step(NULL) == 0u);
}

static void test_physical_key_map(void)
{
    uint32_t key;
    uint8_t seen[FMT_PATTERN_INPUT_STEPS_PER_PAGE] = {0};

    for (key = 0u; key < FMT_PATTERN_INPUT_KEY_COUNT; key++) {
        uint8_t local = 0xA5u;
        int expected = expected_local(key);
        int ok = fmt_pattern_input_key_to_local(key, &local);
        if (expected >= 0) {
            assert(ok);
            assert(local == (uint8_t)expected);
            seen[local]++;
        } else {
            assert(!ok);
            assert(local == 0u);
        }
    }

    for (key = 0u; key < FMT_PATTERN_INPUT_STEPS_PER_PAGE; key++)
        assert(seen[key] == 1u);

    assert(!fmt_pattern_input_key_to_local(FMT_PATTERN_INPUT_KEY_COUNT, NULL));
    assert(!fmt_pattern_input_key_to_local(UINT32_MAX, NULL));
}

static void test_all_64_addresses_and_store_integrity(void)
{
    FmtPatternInput input;
    uint8_t seen[FMT_STEPS_PER_PATTERN] = {0};
    FmtSong *before;
    uint32_t page, local;

    fmt_store_reset();
    before = malloc(sizeof *before);
    assert(before);
    memcpy(before, fmt_store_song_const(), sizeof *before);

    fmt_pattern_input_reset(&input);
    fmt_pattern_input_enter_pattern(&input);

    for (page = 0u; page < FMT_PATTERN_INPUT_PAGE_COUNT; page++) {
        if (page != 0u)
            assert(fmt_pattern_input_page_next(&input));
        assert(input.page == page);

        for (local = 0u; local < FMT_PATTERN_INPUT_STEPS_PER_PAGE; local++) {
            uint8_t step;
            assert(fmt_pattern_input_select_key(&input, WHITE_KEYS[local]));
            step = fmt_pattern_input_global_step(&input);
            assert(step == page * FMT_PATTERN_INPUT_STEPS_PER_PAGE + local);
            assert(step < FMT_STEPS_PER_PATTERN);
            seen[step]++;
        }
    }

    for (local = 0u; local < FMT_STEPS_PER_PATTERN; local++)
        assert(seen[local] == 1u);

    assert(memcmp(before, fmt_store_song_const(), sizeof *before) == 0);
    free(before);
}

static void test_clamped_pages_preserve_local_step(void)
{
    FmtPatternInput input;

    fmt_pattern_input_reset(&input);
    fmt_pattern_input_enter_pattern(&input);
    assert(fmt_pattern_input_select_key(&input, WHITE_KEYS[10]));
    assert(fmt_pattern_input_global_step(&input) == 10u);

    assert(!fmt_pattern_input_page_prev(&input));
    assert(input.page == 0u && input.local_step == 10u);

    assert(fmt_pattern_input_page_next(&input));
    assert(input.page == 1u && input.local_step == 10u);
    assert(fmt_pattern_input_global_step(&input) == 26u);

    assert(fmt_pattern_input_page_next(&input));
    assert(input.page == 2u && input.local_step == 10u);
    assert(fmt_pattern_input_global_step(&input) == 42u);

    assert(fmt_pattern_input_page_next(&input));
    assert(input.page == 3u && input.local_step == 10u);
    assert(fmt_pattern_input_global_step(&input) == 58u);

    assert(!fmt_pattern_input_page_next(&input));
    assert(input.page == 3u && input.local_step == 10u);
    assert(fmt_pattern_input_global_step(&input) == 58u);

    assert(fmt_pattern_input_page_prev(&input));
    assert(fmt_pattern_input_global_step(&input) == 42u);
    assert(fmt_pattern_input_page_prev(&input));
    assert(fmt_pattern_input_global_step(&input) == 26u);
    assert(fmt_pattern_input_page_prev(&input));
    assert(fmt_pattern_input_global_step(&input) == 10u);
    assert(!fmt_pattern_input_page_prev(&input));
    assert(fmt_pattern_input_global_step(&input) == 10u);
}

static void test_black_keys_do_not_move_cursor(void)
{
    FmtPatternInput input;
    uint32_t key;

    fmt_pattern_input_reset(&input);
    fmt_pattern_input_enter_pattern(&input);
    assert(fmt_pattern_input_select_key(&input, WHITE_KEYS[8]));

    for (key = 0u; key < FMT_PATTERN_INPUT_KEY_COUNT; key++) {
        if (expected_local(key) < 0) {
            assert(!fmt_pattern_input_select_key(&input, key));
            assert(input.page == 0u);
            assert(input.local_step == 8u);
            assert(fmt_pattern_input_global_step(&input) == 8u);
        }
    }
}

int main(void)
{
    test_reset_and_context();
    test_physical_key_map();
    test_all_64_addresses_and_store_integrity();
    test_clamped_pages_preserve_local_step();
    test_black_keys_do_not_move_cursor();

    puts("FMT_PATTERN_INPUT_TESTS_OK=1");
    printf("state_bytes=%u pages=%u steps_per_page=%u total_steps=%u white_keys=%u\n",
           (unsigned)sizeof(FmtPatternInput),
           (unsigned)FMT_PATTERN_INPUT_PAGE_COUNT,
           (unsigned)FMT_PATTERN_INPUT_STEPS_PER_PAGE,
           (unsigned)FMT_STEPS_PER_PATTERN,
           (unsigned)FMT_PATTERN_INPUT_STEPS_PER_PAGE);
    return 0;
}
