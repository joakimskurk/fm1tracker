/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmt_pattern_input.h"
#include "fmt_step_record.h"
#include "fmt_store.h"
#include "fmt_transport.h"

static void test_reset_entry_exit_and_transport_gate(void)
{
    FmtStepRecord record = {99u, 99u};
    FmtPatternInput input;
    FmtTransport transport;

    fmt_step_record_reset(&record);
    fmt_pattern_input_reset(&input);
    fmt_transport_reset(&transport);

    assert(sizeof record == 2u);
    assert(!fmt_step_record_is_active(&record));
    assert(record.gesture_latched == 0u);

    input.page = 2u;
    input.local_step = 7u;
    input.selected_track = 3u;

    fmt_transport_start(&transport);
    assert(!fmt_step_record_enter(&record, &input, &transport));
    assert(!fmt_step_record_is_active(&record));
    assert(!fmt_pattern_input_is_pattern(&input));
    assert(input.page == 2u && input.local_step == 7u && input.selected_track == 3u);

    fmt_transport_stop(&transport);
    assert(fmt_step_record_enter(&record, &input, &transport));
    assert(fmt_step_record_is_active(&record));
    assert(fmt_pattern_input_is_pattern(&input));
    assert(input.page == 2u && input.local_step == 7u && input.selected_track == 3u);
    assert(!fmt_step_record_enter(&record, &input, &transport));

    record.gesture_latched = 1u;
    fmt_step_record_exit(&record);
    assert(!fmt_step_record_is_active(&record));
    assert(record.gesture_latched == 0u);

    fmt_step_record_reset(NULL);
    fmt_step_record_exit(NULL);
    fmt_step_record_release_gesture(NULL);
    assert(!fmt_step_record_is_active(NULL));
    assert(!fmt_step_record_enter(NULL, &input, &transport));
    assert(!fmt_step_record_enter(&record, NULL, &transport));
    assert(!fmt_step_record_enter(&record, &input, NULL));
}

static void test_lowest_key_rule(void)
{
    uint8_t key = 0xA5u;
    uint32_t k;

    assert(!fmt_step_record_lowest_key(0u, &key));
    assert(key == 0u);

    for (k = 0u; k < FMT_PATTERN_INPUT_KEY_COUNT; k++) {
        key = 0xA5u;
        assert(fmt_step_record_lowest_key(1u << k, &key));
        assert(key == k);
    }

    assert(fmt_step_record_lowest_key((1u << 2) | (1u << 7) | (1u << 19), &key));
    assert(key == 2u);
    assert(fmt_step_record_lowest_key((1u << 1) | (1u << 3) | (1u << 26), &key));
    assert(key == 1u);

    key = 0xA5u;
    assert(!fmt_step_record_lowest_key(1u << 31, &key));
    assert(key == 0u);
    assert(fmt_step_record_lowest_key((1u << 31) | (1u << 26), &key));
    assert(key == 26u);
    assert(fmt_step_record_lowest_key(1u, NULL));
}

static void decorate_step(FmtStep *step)
{
    assert(step);
    step->instrument = 3u;
    step->volume = 77u;
    step->effect = 4u;
    step->effect_value = 91u;
    step->flags = 0xA5u;
    step->lock_head = 123u;
}

static void assert_decorations_preserved(const FmtStep *step)
{
    assert(step);
    assert(step->instrument == 3u);
    assert(step->volume == 77u);
    assert(step->effect == 4u);
    assert(step->effect_value == 91u);
    assert(step->flags == 0xA5u);
    assert(step->lock_head == 123u);
}

static void test_exact_target_write_and_field_preservation(void)
{
    FmtStepRecord record;
    FmtPatternInput input;
    FmtTransport transport;
    FmtSong *expected;
    FmtStep *target;
    FmtStep *expected_target;

    fmt_store_reset();
    fmt_step_record_reset(&record);
    fmt_pattern_input_reset(&input);
    fmt_transport_reset(&transport);

    input.page = 1u;
    input.local_step = 7u;
    input.selected_track = 5u;

    target = fmt_store_step(FMT_STEP_RECORD_PATTERN, 5u, 23u);
    decorate_step(target);

    expected = malloc(sizeof *expected);
    assert(expected);
    memcpy(expected, fmt_store_song_const(), sizeof *expected);
    expected_target = fmt_song_step(expected, FMT_STEP_RECORD_PATTERN, 5u, 23u);
    assert(expected_target);
    expected_target->note = 72u;

    assert(fmt_step_record_enter(&record, &input, &transport));
    assert(fmt_step_record_write_note(&record, &input, &transport, 72u));
    assert(record.gesture_latched == 1u);
    assert(input.selected_track == 5u);
    assert(fmt_pattern_input_global_step(&input) == 24u);
    assert(memcmp(expected, fmt_store_song_const(), sizeof *expected) == 0);
    assert_decorations_preserved(fmt_store_step_const(0u, 5u, 23u));

    free(expected);
}

static void test_special_values_mutate_only_note_and_advance_once(void)
{
    static const uint8_t special[3] = {FMT_NOTE_EMPTY, FMT_NOTE_TIE, FMT_NOTE_OFF};
    uint32_t i;

    for (i = 0u; i < 3u; i++) {
        FmtStepRecord record;
        FmtPatternInput input;
        FmtTransport transport;
        FmtSong *expected;
        FmtStep *target;
        FmtStep *expected_target;

        fmt_store_reset();
        fmt_step_record_reset(&record);
        fmt_pattern_input_reset(&input);
        fmt_transport_reset(&transport);
        input.page = 2u;
        input.local_step = 3u; /* global 35 */
        input.selected_track = 6u;

        target = fmt_store_step(0u, 6u, 35u);
        assert(target);
        target->note = 71u;
        decorate_step(target);

        expected = malloc(sizeof *expected);
        assert(expected);
        memcpy(expected, fmt_store_song_const(), sizeof *expected);
        expected_target = fmt_song_step(expected, 0u, 6u, 35u);
        assert(expected_target);
        expected_target->note = special[i];

        assert(fmt_step_record_enter(&record, &input, &transport));
        assert(fmt_step_record_write_special(&record, &input, &transport, special[i]));
        assert(record.gesture_latched == 1u);
        assert(input.selected_track == 6u);
        assert(fmt_pattern_input_global_step(&input) == 36u);
        assert(memcmp(expected, fmt_store_song_const(), sizeof *expected) == 0);
        assert_decorations_preserved(fmt_store_step_const(0u, 6u, 35u));
        free(expected);
    }
}

static void test_gesture_latch_blocks_note_and_special_double_writes(void)
{
    FmtStepRecord record;
    FmtPatternInput input;
    FmtTransport transport;
    FmtSong *before;

    fmt_store_reset();
    fmt_step_record_reset(&record);
    fmt_pattern_input_reset(&input);
    fmt_transport_reset(&transport);
    input.selected_track = 2u;

    assert(fmt_step_record_enter(&record, &input, &transport));
    assert(fmt_step_record_write_note(&record, &input, &transport, 60u));
    assert(fmt_pattern_input_global_step(&input) == 1u);

    before = malloc(sizeof *before);
    assert(before);
    memcpy(before, fmt_store_song_const(), sizeof *before);

    assert(!fmt_step_record_write_note(&record, &input, &transport, 64u));
    assert(!fmt_step_record_write_special(&record, &input, &transport, FMT_NOTE_TIE));
    assert(fmt_pattern_input_global_step(&input) == 1u);
    assert(memcmp(before, fmt_store_song_const(), sizeof *before) == 0);

    fmt_step_record_release_gesture(&record);
    assert(fmt_step_record_write_special(&record, &input, &transport, FMT_NOTE_OFF));
    assert(fmt_pattern_input_global_step(&input) == 2u);
    assert(fmt_store_step_const(0u, 2u, 0u)->note == 60u);
    assert(fmt_store_step_const(0u, 2u, 1u)->note == FMT_NOTE_OFF);

    memcpy(before, fmt_store_song_const(), sizeof *before);
    assert(!fmt_step_record_write_note(&record, &input, &transport, 67u));
    assert(!fmt_step_record_write_special(&record, &input, &transport, FMT_NOTE_EMPTY));
    assert(memcmp(before, fmt_store_song_const(), sizeof *before) == 0);

    free(before);
}

static void test_rejection_paths_do_not_mutate_or_advance(void)
{
    FmtStepRecord record;
    FmtPatternInput input;
    FmtTransport transport;
    FmtSong *before;

    fmt_store_reset();
    fmt_step_record_reset(&record);
    fmt_pattern_input_reset(&input);
    fmt_transport_reset(&transport);

    before = malloc(sizeof *before);
    assert(before);
    memcpy(before, fmt_store_song_const(), sizeof *before);

    assert(!fmt_step_record_write_note(&record, &input, &transport, 60u));
    assert(!fmt_step_record_write_special(&record, &input, &transport, FMT_NOTE_EMPTY));
    assert(memcmp(before, fmt_store_song_const(), sizeof *before) == 0);
    assert(fmt_pattern_input_global_step(&input) == 0u);

    assert(fmt_step_record_enter(&record, &input, &transport));
    assert(!fmt_step_record_write_note(&record, &input, &transport, 128u));
    assert(!fmt_step_record_write_note(&record, &input, &transport, UINT32_MAX));
    assert(!fmt_step_record_write_special(&record, &input, &transport, 60u));
    assert(!fmt_step_record_write_special(&record, &input, &transport, 0xFCu));
    assert(!fmt_step_record_write_special(&record, &input, &transport, UINT32_MAX));
    assert(memcmp(before, fmt_store_song_const(), sizeof *before) == 0);

    fmt_transport_start(&transport);
    assert(!fmt_step_record_write_note(&record, &input, &transport, 60u));
    assert(!fmt_step_record_write_special(&record, &input, &transport, FMT_NOTE_TIE));
    assert(memcmp(before, fmt_store_song_const(), sizeof *before) == 0);
    fmt_transport_stop(&transport);

    fmt_pattern_input_enter_performance(&input);
    assert(!fmt_step_record_write_note(&record, &input, &transport, 60u));
    assert(!fmt_step_record_write_special(&record, &input, &transport, FMT_NOTE_OFF));
    assert(memcmp(before, fmt_store_song_const(), sizeof *before) == 0);
    fmt_pattern_input_enter_pattern(&input);

    input.selected_track = FMT_PATTERN_INPUT_TRACK_COUNT;
    assert(!fmt_step_record_write_special(&record, &input, &transport, FMT_NOTE_EMPTY));
    assert(memcmp(before, fmt_store_song_const(), sizeof *before) == 0);

    input.selected_track = 0u;
    input.page = FMT_PATTERN_INPUT_PAGE_COUNT;
    assert(!fmt_step_record_write_special(&record, &input, &transport, FMT_NOTE_TIE));
    assert(memcmp(before, fmt_store_song_const(), sizeof *before) == 0);

    assert(!fmt_step_record_write_special(NULL, &input, &transport, FMT_NOTE_EMPTY));
    assert(!fmt_step_record_write_special(&record, NULL, &transport, FMT_NOTE_TIE));
    assert(!fmt_step_record_write_special(&record, &input, NULL, FMT_NOTE_OFF));

    free(before);
}

static void test_step_63_wraps_to_zero_for_note_and_specials(void)
{
    static const uint8_t values[4] = {103u, FMT_NOTE_EMPTY, FMT_NOTE_TIE, FMT_NOTE_OFF};
    uint32_t i;

    for (i = 0u; i < 4u; i++) {
        FmtStepRecord record;
        FmtPatternInput input;
        FmtTransport transport;

        fmt_store_reset();
        fmt_step_record_reset(&record);
        fmt_pattern_input_reset(&input);
        fmt_transport_reset(&transport);
        input.page = 3u;
        input.local_step = 15u;
        input.selected_track = 7u;

        assert(fmt_step_record_enter(&record, &input, &transport));
        assert(fmt_pattern_input_global_step(&input) == 63u);
        if (i == 0u)
            assert(fmt_step_record_write_note(&record, &input, &transport, values[i]));
        else
            assert(fmt_step_record_write_special(&record, &input, &transport, values[i]));
        assert(fmt_store_step_const(0u, 7u, 63u)->note == values[i]);
        assert(input.page == 0u);
        assert(input.local_step == 0u);
        assert(input.selected_track == 7u);
        assert(fmt_pattern_input_global_step(&input) == 0u);
    }
}

int main(void)
{
    test_reset_entry_exit_and_transport_gate();
    test_lowest_key_rule();
    test_exact_target_write_and_field_preservation();
    test_special_values_mutate_only_note_and_advance_once();
    test_gesture_latch_blocks_note_and_special_double_writes();
    test_rejection_paths_do_not_mutate_or_advance();
    test_step_63_wraps_to_zero_for_note_and_specials();

    puts("FMT_STEP_RECORD_TESTS_OK=1");
    printf("state_bytes=%u pattern=%u note_min=%u note_max=%u wrap=63_to_0 chord=lowest_key specials=empty,tie,off\n",
           (unsigned)sizeof(FmtStepRecord),
           (unsigned)FMT_STEP_RECORD_PATTERN,
           (unsigned)FMT_NOTE_MIN,
           (unsigned)FMT_NOTE_MAX);
    return 0;
}
