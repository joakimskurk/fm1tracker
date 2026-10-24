/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmt_tracker_screen.h"

static void drain_all(FmtTrackerScreen *screen, uint32_t *count)
{
    uint8_t region;
    uint32_t n = 0u;

    while (fmt_tracker_screen_take_region(screen, &region)) {
        assert(region < FMT_TRACKER_SCREEN_REGION_COUNT);
        n++;
    }
    if (count)
        *count = n;
}

static void prepare(FmtTrackerScreen *screen,
                    FmtPatternInput *input,
                    FmtStepRecord *record,
                    FmtTransport *transport)
{
    fmt_store_reset();
    fmt_tracker_screen_reset(screen);
    fmt_pattern_input_reset(input);
    fmt_step_record_reset(record);
    fmt_transport_reset(transport);
    fmt_pattern_input_enter_pattern(input);
}

static void test_state_budget_and_reset(void)
{
    FmtTrackerScreen screen;
    uint8_t region = 99u;

    memset(&screen, 0xA5, sizeof screen);
    fmt_tracker_screen_reset(&screen);

    assert(sizeof screen <= 64u);
    assert(screen.active == 0u);
    assert(screen.dirty == 0u);
    assert(screen.track == 0xFFu);
    assert(screen.page == 0xFFu);
    assert(screen.window_start == 0xFFu);
    assert(!fmt_tracker_screen_take_region(&screen, &region));
    assert(region == 99u);
    assert(!fmt_tracker_screen_take_region(NULL, &region));
    assert(!fmt_tracker_screen_take_region(&screen, NULL));
    fmt_tracker_screen_reset(NULL);
}

static void test_all_eight_cursor_owned_windows(void)
{
    FmtPatternInput input;
    uint32_t page, local;

    fmt_pattern_input_reset(&input);
    fmt_pattern_input_enter_pattern(&input);

    for (page = 0u; page < FMT_PATTERN_INPUT_PAGE_COUNT; page++) {
        input.page = (uint8_t)page;
        for (local = 0u; local < FMT_PATTERN_INPUT_STEPS_PER_PAGE; local++) {
            uint8_t start, selected;
            uint8_t expected = (uint8_t)(page * 16u + (local >= 8u ? 8u : 0u));
            input.local_step = (uint8_t)local;
            start = fmt_tracker_screen_window_start(&input);
            selected = fmt_pattern_input_global_step(&input);
            assert(start == expected);
            assert(selected >= start);
            assert(selected < (uint8_t)(start + FMT_TRACKER_SCREEN_ROWS));
        }
    }

    input.page = FMT_PATTERN_INPUT_PAGE_COUNT;
    assert(fmt_tracker_screen_window_start(&input) == 0u);
    input.page = 0u;
    input.local_step = FMT_PATTERN_INPUT_STEPS_PER_PAGE;
    assert(fmt_tracker_screen_window_start(&input) == 0u);
    assert(fmt_tracker_screen_window_start(NULL) == 0u);
}

static void test_exact_text_contract(void)
{
    char note[5], inst[3], vol[4], fx[4];

    fmt_tracker_screen_note_text(60u, note);
    assert(strcmp(note, "C-4 ") == 0);
    fmt_tracker_screen_note_text(61u, note);
    assert(strcmp(note, "C#4 ") == 0);
    fmt_tracker_screen_note_text(0u, note);
    assert(strcmp(note, "C--1") == 0);
    fmt_tracker_screen_note_text(127u, note);
    assert(strcmp(note, "G-9 ") == 0);
    fmt_tracker_screen_note_text(FMT_NOTE_EMPTY, note);
    assert(strcmp(note, "--- ") == 0);
    fmt_tracker_screen_note_text(FMT_NOTE_TIE, note);
    assert(strcmp(note, "=== ") == 0);
    fmt_tracker_screen_note_text(FMT_NOTE_OFF, note);
    assert(strcmp(note, "OFF ") == 0);
    fmt_tracker_screen_note_text(0xFCu, note);
    assert(strcmp(note, "??? ") == 0);

    fmt_tracker_screen_instrument_text(FMT_INSTRUMENT_INHERIT, inst);
    assert(strcmp(inst, "..") == 0);
    fmt_tracker_screen_instrument_text(0u, inst);
    assert(strcmp(inst, "01") == 0);
    fmt_tracker_screen_instrument_text(15u, inst);
    assert(strcmp(inst, "16") == 0);
    fmt_tracker_screen_instrument_text(16u, inst);
    assert(strcmp(inst, "??") == 0);

    fmt_tracker_screen_volume_text(FMT_VOLUME_INHERIT, vol);
    assert(strcmp(vol, "...") == 0);
    fmt_tracker_screen_volume_text(0u, vol);
    assert(strcmp(vol, "000") == 0);
    fmt_tracker_screen_volume_text(7u, vol);
    assert(strcmp(vol, "007") == 0);
    fmt_tracker_screen_volume_text(254u, vol);
    assert(strcmp(vol, "254") == 0);
    fmt_tracker_screen_volume_text(256u, vol);
    assert(strcmp(vol, "???") == 0);

    fmt_tracker_screen_effect_text(FMT_EFFECT_NONE, fx);
    assert(strcmp(fx, "---") == 0);
    fmt_tracker_screen_effect_text(1u, fx);
    assert(strcmp(fx, "???") == 0);
}

static void test_initial_full_dirty_and_priority_drain(void)
{
    FmtTrackerScreen screen;
    FmtPatternInput input;
    FmtStepRecord record;
    FmtTransport transport;
    uint8_t region;
    uint32_t count;

    prepare(&screen, &input, &record, &transport);
    assert(fmt_tracker_screen_sync(&screen, &input, &record, &transport));
    assert(screen.active == 1u);
    assert(fmt_tracker_screen_dirty(&screen) == FMT_TRACKER_SCREEN_ALL_DIRTY);

    assert(fmt_tracker_screen_take_region(&screen, &region));
    assert(region == FMT_TRACKER_SCREEN_REGION_HEADER);
    assert(fmt_tracker_screen_take_region(&screen, &region));
    assert(region == FMT_TRACKER_SCREEN_REGION_FOOTER);

    drain_all(&screen, &count);
    assert(count == FMT_TRACKER_SCREEN_ROWS);
    assert(fmt_tracker_screen_dirty(&screen) == 0u);

    assert(fmt_tracker_screen_sync(&screen, &input, &record, &transport));
    assert(fmt_tracker_screen_dirty(&screen) == 0u);
}

static void test_cursor_playhead_markers_and_dirty_rows(void)
{
    FmtTrackerScreen screen;
    FmtPatternInput input;
    FmtStepRecord record;
    FmtTransport transport;
    char markers[3], line1[32], line2[32], line3[32], footer[32];
    uint32_t count;

    prepare(&screen, &input, &record, &transport);
    assert(fmt_tracker_screen_sync(&screen, &input, &record, &transport));
    drain_all(&screen, NULL);

    input.local_step = 1u;
    assert(fmt_tracker_screen_sync(&screen, &input, &record, &transport));
    assert(fmt_tracker_screen_dirty(&screen) ==
           ((1u << FMT_TRACKER_SCREEN_REGION_HEADER) |
            (1u << (FMT_TRACKER_SCREEN_REGION_ROW0 + 0u)) |
            (1u << (FMT_TRACKER_SCREEN_REGION_ROW0 + 1u))));
    drain_all(&screen, NULL);

    transport.running = 1u;
    transport.step = 0u;
    assert(fmt_tracker_screen_sync(&screen, &input, &record, &transport));
    fmt_tracker_screen_markers(&screen, 0u, markers);
    assert(strcmp(markers, " *") == 0);
    fmt_tracker_screen_markers(&screen, 1u, markers);
    assert(strcmp(markers, "> ") == 0);
    drain_all(&screen, NULL);

    transport.step = 1u;
    assert(fmt_tracker_screen_sync(&screen, &input, &record, &transport));
    fmt_tracker_screen_markers(&screen, 1u, markers);
    assert(strcmp(markers, ">*") == 0);
    drain_all(&screen, NULL);

    transport.step = 20u;
    assert(fmt_tracker_screen_sync(&screen, &input, &record, &transport));
    for (count = 0u; count < FMT_TRACKER_SCREEN_ROWS; count++) {
        fmt_tracker_screen_markers(&screen, count, markers);
        assert(markers[1] == ' ');
    }
    fmt_tracker_screen_header_line1(&screen, line1);
    assert(strstr(line1, "CUR02") != NULL);
    assert(strstr(line1, "PLY21") != NULL);
    fmt_tracker_screen_header_line2(&screen, line2);
    assert(strcmp(line2, "P01 T1 PG1/4 01-08") == 0);
    fmt_tracker_screen_header_line3(line3);
    assert(strcmp(line3, "ST NOTE IN VOL LK FX") == 0);
    fmt_tracker_screen_footer(&screen, footer);
    assert(strcmp(footer, "KEYS STEP/TRK  OCT PAGE") == 0);
}

static void test_step_rec_header_footer_and_row_format(void)
{
    FmtTrackerScreen screen;
    FmtPatternInput input;
    FmtStepRecord record;
    FmtTransport transport;
    FmtStep *step;
    char line1[32], line2[32], row[32], footer[32];

    prepare(&screen, &input, &record, &transport);
    input.page = 1u;
    input.local_step = 8u;
    input.selected_track = 2u;
    transport.bpm = 137u;
    assert(fmt_step_record_enter(&record, &input, &transport));

    step = fmt_store_step(FMT_TRACKER_SCREEN_PATTERN, 2u, 24u);
    assert(step);
    step->note = 61u;
    step->instrument = 4u;
    step->volume = 87u;
    step->effect = 0u;

    assert(fmt_tracker_screen_sync(&screen, &input, &record, &transport));
    assert(screen.window_start == 24u);
    fmt_tracker_screen_header_line1(&screen, line1);
    assert(strcmp(line1, "STEP REC CUR25 STOP 137") == 0);
    fmt_tracker_screen_header_line2(&screen, line2);
    assert(strcmp(line2, "P01 T3 PG2/4 25-32") == 0);
    fmt_tracker_screen_footer(&screen, footer);
    assert(strcmp(footer, "FX RST  SEL TIE  FX+SEL OFF") == 0);
    fmt_tracker_screen_row_text(&screen, 0u, row);
    assert(strcmp(row, "25 C#4  05 087 -- ----") == 0);
}

static void test_visible_data_change_dirties_only_that_row_and_never_mutates_song(void)
{
    FmtTrackerScreen screen;
    FmtPatternInput input;
    FmtStepRecord record;
    FmtTransport transport;
    FmtSong *before;
    FmtStep *step;

    prepare(&screen, &input, &record, &transport);
    assert(fmt_tracker_screen_sync(&screen, &input, &record, &transport));
    drain_all(&screen, NULL);

    before = malloc(sizeof *before);
    assert(before);
    memcpy(before, fmt_store_song_const(), sizeof *before);
    assert(fmt_tracker_screen_sync(&screen, &input, &record, &transport));
    assert(memcmp(before, fmt_store_song_const(), sizeof *before) == 0);
    free(before);

    step = fmt_store_step(FMT_TRACKER_SCREEN_PATTERN, 0u, 3u);
    assert(step);
    step->note = FMT_NOTE_TIE;
    assert(fmt_tracker_screen_sync(&screen, &input, &record, &transport));
    assert(fmt_tracker_screen_dirty(&screen) ==
           (1u << (FMT_TRACKER_SCREEN_REGION_ROW0 + 3u)));
}

static void test_track_page_half_and_mode_changes_force_full_surface(void)
{
    FmtTrackerScreen screen;
    FmtPatternInput input;
    FmtStepRecord record;
    FmtTransport transport;

    prepare(&screen, &input, &record, &transport);
    assert(fmt_tracker_screen_sync(&screen, &input, &record, &transport));
    drain_all(&screen, NULL);

    input.selected_track = 1u;
    assert(fmt_tracker_screen_sync(&screen, &input, &record, &transport));
    assert(screen.dirty == FMT_TRACKER_SCREEN_ALL_DIRTY);
    drain_all(&screen, NULL);

    input.page = 1u;
    assert(fmt_tracker_screen_sync(&screen, &input, &record, &transport));
    assert(screen.dirty == FMT_TRACKER_SCREEN_ALL_DIRTY);
    drain_all(&screen, NULL);

    input.local_step = 8u;
    assert(fmt_tracker_screen_sync(&screen, &input, &record, &transport));
    assert(screen.dirty == FMT_TRACKER_SCREEN_ALL_DIRTY);
    drain_all(&screen, NULL);

    assert(fmt_step_record_enter(&record, &input, &transport));
    assert(fmt_tracker_screen_sync(&screen, &input, &record, &transport));
    assert(screen.dirty == FMT_TRACKER_SCREEN_ALL_DIRTY);
    drain_all(&screen, NULL);

    fmt_step_record_exit(&record);
    fmt_pattern_input_enter_performance(&input);
    assert(!fmt_tracker_screen_sync(&screen, &input, &record, &transport));
    assert(screen.active == 0u);
    assert(screen.dirty == 0u);
}

static void test_region_mapping_and_two_region_budget_constant(void)
{
    uint8_t row = 99u;
    uint32_t region;

    assert(FMT_TRACKER_SCREEN_MAX_REGIONS_PER_POLL == 2u);
    assert(!fmt_tracker_screen_region_to_row(FMT_TRACKER_SCREEN_REGION_HEADER, &row));
    assert(row == 0u);
    assert(!fmt_tracker_screen_region_to_row(FMT_TRACKER_SCREEN_REGION_FOOTER, &row));
    for (region = FMT_TRACKER_SCREEN_REGION_ROW0;
         region < FMT_TRACKER_SCREEN_REGION_COUNT;
         region++) {
        assert(fmt_tracker_screen_region_to_row(region, &row));
        assert(row == region - FMT_TRACKER_SCREEN_REGION_ROW0);
    }
    assert(!fmt_tracker_screen_region_to_row(FMT_TRACKER_SCREEN_REGION_COUNT, &row));
}

int main(void)
{
    test_state_budget_and_reset();
    test_all_eight_cursor_owned_windows();
    test_exact_text_contract();
    test_initial_full_dirty_and_priority_drain();
    test_cursor_playhead_markers_and_dirty_rows();
    test_step_rec_header_footer_and_row_format();
    test_visible_data_change_dirties_only_that_row_and_never_mutates_song();
    test_track_page_half_and_mode_changes_force_full_surface();
    test_region_mapping_and_two_region_budget_constant();

    puts("FMT_TRACKER_SCREEN_TESTS_OK=1");
    printf("state_bytes=%u rows=%u regions=%u max_regions_per_poll=%u pattern=%u\n",
           (unsigned)sizeof(FmtTrackerScreen),
           (unsigned)FMT_TRACKER_SCREEN_ROWS,
           (unsigned)FMT_TRACKER_SCREEN_REGION_COUNT,
           (unsigned)FMT_TRACKER_SCREEN_MAX_REGIONS_PER_POLL,
           (unsigned)FMT_TRACKER_SCREEN_PATTERN);
    return 0;
}
