/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmt_playback.h"
#include "fmt_tracker_screen.h"

static const uint8_t white_keys[FMT_PATTERN_INPUT_STEPS_PER_PAGE] = {
    0u, 2u, 4u, 6u, 7u, 9u, 11u, 12u,
    14u, 16u, 18u, 19u, 21u, 23u, 24u, 26u
};

static const uint8_t track_keys[FMT_PATTERN_INPUT_TRACK_COUNT] = {
    1u, 3u, 5u, 8u, 10u, 13u, 15u, 17u
};

static void release_write_gesture(FmtStepRecord *record)
{
    assert(record != NULL);
    fmt_step_record_release_gesture(record);
    assert(record->gesture_latched == 0u);
}

static void write_note(FmtStepRecord *record,
                       FmtPatternInput *input,
                       const FmtTransport *transport,
                       uint32_t note)
{
    assert(fmt_step_record_write_note(record, input, transport, note));
    assert(record->gesture_latched != 0u);
    release_write_gesture(record);
}

static void write_rest(FmtStepRecord *record,
                       FmtPatternInput *input,
                       const FmtTransport *transport)
{
    assert(!fmt_step_record_special_input(record, input, transport, 1, 0, 0));
    assert(record->fx_pending != 0u);
    assert(fmt_step_record_special_input(record, input, transport, 0, 1, 0));
    assert(record->fx_pending == 0u);
    release_write_gesture(record);
}

static void write_tie(FmtStepRecord *record,
                      FmtPatternInput *input,
                      const FmtTransport *transport)
{
    assert(fmt_step_record_special_input(record, input, transport, 0, 0, 1));
    release_write_gesture(record);
}

static void write_off(FmtStepRecord *record,
                      FmtPatternInput *input,
                      const FmtTransport *transport)
{
    assert(!fmt_step_record_special_input(record, input, transport, 1, 0, 0));
    assert(record->fx_pending != 0u);
    assert(fmt_step_record_special_input(record, input, transport, 0, 0, 1));
    assert(record->fx_pending == 0u);
    release_write_gesture(record);

    /* D025 modifier-first OFF consumes the pending REST. The later FX release
     * is therefore a strict no-op and cannot advance a second time. */
    assert(!fmt_step_record_special_input(record, input, transport, 0, 1, 0));
}

static void expect_note(FmtSong *song, uint32_t track, uint32_t step, uint8_t note)
{
    FmtStep *row = fmt_song_step(song, 0u, track, step);
    assert(row != NULL);
    row->note = note;
}

int main(void)
{
    FmtTransport transport;
    FmtPlayback playback;
    FmtPlaybackEvent event;
    FmtPatternInput input;
    FmtStepRecord record;
    FmtTrackerScreen screen;
    FmtSong *baseline;
    FmtSong *expected;
    const FmtSong *song;
    uint8_t seen[FMT_STEPS_PER_PATTERN] = {0};
    uint32_t page, local, track, i, boundaries = 0u, note_boundaries = 0u;
    char note_text[5], header2[32], footer[32], markers[3];

    baseline = (FmtSong *)malloc(sizeof(FmtSong));
    expected = (FmtSong *)malloc(sizeof(FmtSong));
    assert(baseline != NULL && expected != NULL);

    /* Same initial song/runtime construction as the real app. */
    fmt_store_reset();
    fmt_playback_seed_proof_pattern();
    fmt_transport_reset(&transport);
    fmt_playback_reset(&playback);
    fmt_pattern_input_reset(&input);
    fmt_step_record_reset(&record);
    fmt_tracker_screen_reset(&screen);

    song = fmt_store_song_const();
    assert(song != NULL);
    memcpy(baseline, song, sizeof(*baseline));
    memcpy(expected, song, sizeof(*expected));

    /* Sanity-check the audible proof lane we must not disturb. */
    assert(fmt_store_step_const(0u, 0u, 0u)->note == 60u);
    assert(fmt_store_step_const(0u, 0u, 4u)->note == 64u);

    /* D022: visit all 64 editor addresses through the real white-key/page API. */
    fmt_pattern_input_enter_pattern(&input);
    for (page = 0u; page < FMT_PATTERN_INPUT_PAGE_COUNT; page++) {
        assert(input.page == page);
        for (local = 0u; local < FMT_PATTERN_INPUT_STEPS_PER_PAGE; local++) {
            uint8_t global;
            assert(fmt_pattern_input_select_key(&input, white_keys[local]));
            assert(input.local_step == local);
            global = fmt_pattern_input_global_step(&input);
            assert(global == page * FMT_PATTERN_INPUT_STEPS_PER_PAGE + local);
            assert(global < FMT_STEPS_PER_PATTERN);
            assert(seen[global] == 0u);
            seen[global] = 1u;
        }
        if (page + 1u < FMT_PATTERN_INPUT_PAGE_COUNT)
            assert(fmt_pattern_input_page_next(&input));
    }
    for (i = 0u; i < FMT_STEPS_PER_PATTERN; i++)
        assert(seen[i] == 1u);
    assert(input.page == 3u);
    assert(input.local_step == 15u);
    assert(fmt_pattern_input_global_step(&input) == 63u);
    assert(memcmp(song, baseline, sizeof(*baseline)) == 0);

    /* D023: all eight direct selectors are bijective and non-mutating. */
    for (track = 0u; track < FMT_PATTERN_INPUT_TRACK_COUNT; track++) {
        assert(fmt_pattern_input_select_track_key(&input, track_keys[track]));
        assert(input.selected_track == track);
        assert(fmt_pattern_input_global_step(&input) == 63u);
        assert(memcmp(song, baseline, sizeof(*baseline)) == 0);
    }
    assert(input.selected_track == 7u);

    /* D024 real stop gate: running rejects STEP REC; stopped accepts it. */
    fmt_transport_start(&transport);
    assert(transport.running != 0u);
    assert(!fmt_step_record_enter(&record, &input, &transport));
    fmt_transport_stop(&transport);
    assert(transport.running == 0u);
    assert(fmt_step_record_enter(&record, &input, &transport));

    /* Step 63 ordinary note must wrap the edit cursor exactly to step 0. */
    write_note(&record, &input, &transport, 53u);
    assert(input.page == 0u && input.local_step == 0u);
    assert(fmt_pattern_input_global_step(&input) == 0u);

    write_note(&record, &input, &transport, 55u);
    assert(fmt_pattern_input_global_step(&input) == 1u);

    /* Return to ordinary SEQ only to address step 15, then cross 15 -> 16 by
     * automatic STEP REC advance rather than by page navigation. */
    fmt_step_record_exit(&record);
    assert(fmt_pattern_input_is_pattern(&input));
    assert(fmt_pattern_input_select_key(&input, white_keys[15]));
    assert(input.page == 0u && input.local_step == 15u);
    assert(fmt_pattern_input_global_step(&input) == 15u);
    assert(input.selected_track == 7u);
    assert(fmt_step_record_enter(&record, &input, &transport));

    write_note(&record, &input, &transport, 60u);
    assert(input.page == 1u && input.local_step == 0u);
    assert(fmt_pattern_input_global_step(&input) == 16u);

    write_rest(&record, &input, &transport);
    assert(fmt_pattern_input_global_step(&input) == 17u);
    write_tie(&record, &input, &transport);
    assert(fmt_pattern_input_global_step(&input) == 18u);
    write_off(&record, &input, &transport);
    assert(fmt_pattern_input_global_step(&input) == 19u);
    write_note(&record, &input, &transport, 64u);
    assert(input.page == 1u && input.local_step == 4u);
    assert(fmt_pattern_input_global_step(&input) == 20u);
    assert(input.selected_track == 7u);
    assert(fmt_step_record_is_active(&record));

    /* Build the only allowed final-song differences, then compare all 65,896
     * bytes. REST at step 16 is explicitly EMPTY, which equals the reset value. */
    expect_note(expected, 7u, 63u, 53u);
    expect_note(expected, 7u, 0u, 55u);
    expect_note(expected, 7u, 15u, 60u);
    expect_note(expected, 7u, 16u, FMT_NOTE_EMPTY);
    expect_note(expected, 7u, 17u, FMT_NOTE_TIE);
    expect_note(expected, 7u, 18u, FMT_NOTE_OFF);
    expect_note(expected, 7u, 19u, 64u);
    assert(memcmp(song, expected, sizeof(*expected)) == 0);

    /* The proof lane must still be the seeded track-0 data. */
    assert(fmt_store_step_const(0u, 0u, 0u)->note == 60u);
    assert(fmt_store_step_const(0u, 0u, 4u)->note == 64u);

    /* D026 presentation must expose the exact final edit state without writing
     * any song byte. */
    memcpy(baseline, song, sizeof(*baseline));
    assert(fmt_tracker_screen_sync(&screen, &input, &record, &transport));
    assert(memcmp(song, baseline, sizeof(*baseline)) == 0);
    assert(screen.active == 1u);
    assert(screen.step_rec == 1u);
    assert(screen.track == 7u);
    assert(screen.page == 1u);
    assert(screen.window_start == 16u);
    assert(screen.selected_step == 20u);
    assert(screen.playing == 0u);

    fmt_tracker_screen_note_text(screen.row_note[0], note_text);
    assert(strcmp(note_text, "--- ") == 0);
    fmt_tracker_screen_note_text(screen.row_note[1], note_text);
    assert(strcmp(note_text, "=== ") == 0);
    fmt_tracker_screen_note_text(screen.row_note[2], note_text);
    assert(strcmp(note_text, "OFF ") == 0);
    fmt_tracker_screen_note_text(screen.row_note[3], note_text);
    assert(strcmp(note_text, "E-4 ") == 0);
    fmt_tracker_screen_markers(&screen, 4u, markers);
    assert(strcmp(markers, "> ") == 0);
    fmt_tracker_screen_header_line2(&screen, header2);
    assert(strcmp(header2, "P01 T8 PG2/4 17-24") == 0);
    fmt_tracker_screen_footer(&screen, footer);
    assert(strcmp(footer, "FX RST  SEL TIE  FX+SEL OFF") == 0);

    /* Resume the unchanged D021 proof lane. Track-7 edits must not alter its
     * initial event or exact 120 BPM 64-step loop timing. */
    fmt_step_record_exit(&record);
    assert(fmt_playback_start(&playback, &transport, &event));
    assert(event.boundary == 1u);
    assert(event.step == 0u);
    assert(event.kind == FMT_PLAYBACK_EVENT_NOTE);
    assert(event.note == 60u);

    for (i = 0u; i < 352800u; i++) {
        if (fmt_playback_end_frame(&playback, &transport, &event)) {
            boundaries++;
            if (event.kind == FMT_PLAYBACK_EVENT_NOTE)
                note_boundaries++;
        }
    }
    assert(boundaries == 64u);
    assert(note_boundaries == 16u);
    assert(transport.step == 0u);
    assert(transport.phase == 0u);
    assert(transport.running != 0u);
    assert(memcmp(song, expected, sizeof(*expected)) == 0);

    /* HOME/PERFORMANCE relinquishes the tracker presentation cleanly. */
    fmt_transport_stop(&transport);
    fmt_pattern_input_enter_performance(&input);
    assert(!fmt_tracker_screen_sync(&screen, &input, &record, &transport));
    assert(screen.active == 0u);
    assert(fmt_tracker_screen_dirty(&screen) == 0u);
    assert(memcmp(song, expected, sizeof(*expected)) == 0);

    printf("PHASE2_INTEGRATION_TESTS_OK=1\n");
    printf("visited_steps=64 tracks=8 edits_track=7 wrap=63_to_0 page_cross=15_to_16 boundaries_120=%u loop_frames=352800 final_cursor=20 final_page=1 final_track=7\n",
           boundaries);
    printf("store_bytes=%u final_events=note,empty,tie,off proof_lane=track0_unchanged screen_window=16_23\n",
           (unsigned)sizeof(FmtSong));

    free(expected);
    free(baseline);
    return 0;
}
