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
    input.local_step = 7u; /* global 23 */
    input.selected_track = 5u;

    target = fmt_store_step(FMT_STEP_RECORD_PATTERN, 5u, 23u);
    assert(target);
    target->instrument = 3u;
    target->volume = 77u;
    target->effect = 4u;
    target->effect_value = 91u;
    target->flags = 0xA5u;
    target->lock_head = 123u;

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

    target = fmt_store_step(FMT_STEP_RECORD_PATTERN, 5u, 23u);
    assert(target->instrument == 3u);
    assert(target->volume == 77u);
    assert(target->effect == 4u);
    assert(target->effect_value == 91u);
    assert(target->flags == 0xA5u);
    assert(target->lock_head == 123u);

    free(expected);
}

static void test_gesture_latch_blocks_extra_writes_until_all_released(void)
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
    assert(fmt_pattern_input_global_step(&input) == 1u);
    assert(memcmp(before, fmt_store_song_const(), sizeof *before) == 0);

    fmt_step_record_release_gesture(&record);
    assert(record.gesture_latched == 0u);
    assert(fmt_step_record_write_note(&record, &input, &transport, 64u));
    assert(fmt_pattern_input_global_step(&input) == 2u);
    assert(fmt_store_step_const(0u, 2u, 0u)->note == 60u);
    assert(fmt_store_step_const(0u, 2u, 1u)->note == 64u);

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
    assert(memcmp(before, fmt_store_song_const(), sizeof *before) == 0);
    assert(fmt_pattern_input_global_step(&input) == 0u);

    assert(fmt_step_record_enter(&record, &input, &transport));
    assert(!fmt_step_record_write_note(&record, &input, &transport, 128u));
    assert(!fmt_step_record_write_note(&record, &input, &transport, UINT32_MAX));
    assert(memcmp(before, fmt_store_song_const(), sizeof *before) == 0);
    assert(fmt_pattern_input_global_step(&input) == 0u);

    fmt_transport_start(&transport);
    assert(!fmt_step_record_write_note(&record, &input, &transport, 60u));
    assert(memcmp(before, fmt_store_song_const(), sizeof *before) == 0);
    fmt_transport_stop(&transport);

    fmt_pattern_input_enter_performance(&input);
    assert(!fmt_step_record_write_note(&record, &input, &transport, 60u));
    assert(memcmp(before, fmt_store_song_const(), sizeof *before) == 0);
    fmt_pattern_input_enter_pattern(&input);

    input.selected_track = FMT_PATTERN_INPUT_TRACK_COUNT;
    assert(!fmt_step_record_write_note(&record, &input, &transport, 60u));
    assert(memcmp(before, fmt_store_song_const(), sizeof *before) == 0);

    input.selected_track = 0u;
    input.page = FMT_PATTERN_INPUT_PAGE_COUNT;
    assert(!fmt_step_record_write_note(&record, &input, &transport, 60u));
    assert(memcmp(before, fmt_store_song_const(), sizeof *before) == 0);

    assert(!fmt_step_record_write_note(NULL, &input, &transport, 60u));
    assert(!fmt_step_record_write_note(&record, NULL, &transport, 60u));
    assert(!fmt_step_record_write_note(&record, &input, NULL, 60u));

    free(before);
}

static void test_step_63_wraps_to_zero_and_preserves_track(void)
{
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
    assert(fmt_step_record_write_note(&record, &input, &transport, 103u));
    assert(fmt_store_step_const(0u, 7u, 63u)->note == 103u);
    assert(input.page == 0u);
    assert(input.local_step == 0u);
    assert(input.selected_track == 7u);
    assert(fmt_pattern_input_global_step(&input) == 0u);
}

int main(void)
{
    test_reset_entry_exit_and_transport_gate();
    test_exact_target_write_and_field_preservation();
    test_gesture_latch_blocks_extra_writes_until_all_released();
    test_rejection_paths_do_not_mutate_or_advance();
    test_step_63_wraps_to_zero_and_preserves_track();

    puts("FMT_STEP_RECORD_TESTS_OK=1");
    printf("state_bytes=%u pattern=%u note_min=%u note_max=%u wrap=63_to_0\n",
           (unsigned)sizeof(FmtStepRecord),
           (unsigned)FMT_STEP_RECORD_PATTERN,
           (unsigned)FMT_NOTE_MIN,
           (unsigned)FMT_NOTE_MAX);
    return 0;
}
