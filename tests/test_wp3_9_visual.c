/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_tracker_screen.h"

static void drain(FmtTrackerScreen *screen)
{
    uint8_t region;
    while (fmt_tracker_screen_take_region(screen, &region))
        assert(region < FMT_TRACKER_SCREEN_REGION_COUNT);
}

static void prepare(FmtTrackerScreen *screen,
                    FmtPatternInput *input,
                    FmtStepRecord *record,
                    FmtTransport *transport,
                    FmtLockEditor *editor)
{
    fmt_store_reset();
    fmt_lock_store_reset();
    fmt_tracker_screen_reset(screen);
    fmt_pattern_input_reset(input);
    fmt_step_record_reset(record);
    fmt_transport_reset(transport);
    fmt_lock_editor_reset(editor);
    fmt_pattern_input_enter_pattern(input);
}

static void test_exact_state_and_effect_text(void)
{
    char fx[5];

    assert(sizeof(FmtTrackerScreen) == 62u);
    assert(FMT_TRACKER_SCREEN_REGION_COUNT == 10u);
    assert(FMT_TRACKER_SCREEN_MAX_REGIONS_PER_POLL == 2u);

    fmt_tracker_screen_effect_value_text(FMT_TRACKER_EFFECT_NONE, 0xABu, fx);
    assert(strcmp(fx, "----") == 0);
    fmt_tracker_screen_effect_value_text(FMT_TRACKER_EFFECT_ARPEGGIO, 0x37u, fx);
    assert(strcmp(fx, "AR37") == 0);
    fmt_tracker_screen_effect_value_text(FMT_TRACKER_EFFECT_SLIDE_UP, 0x04u, fx);
    assert(strcmp(fx, "SU04") == 0);
    fmt_tracker_screen_effect_value_text(FMT_TRACKER_EFFECT_SLIDE_DOWN, 0xA5u, fx);
    assert(strcmp(fx, "SDA5") == 0);
    fmt_tracker_screen_effect_value_text(FMT_TRACKER_EFFECT_PORTAMENTO, 0x10u, fx);
    assert(strcmp(fx, "PO10") == 0);
    fmt_tracker_screen_effect_value_text(FMT_TRACKER_EFFECT_VIBRATO, 0x2Fu, fx);
    assert(strcmp(fx, "VI2F") == 0);
    fmt_tracker_screen_effect_value_text(FMT_TRACKER_EFFECT_RETRIG, 0x02u, fx);
    assert(strcmp(fx, "RT02") == 0);
    fmt_tracker_screen_effect_value_text(FMT_TRACKER_EFFECT_NOTE_DELAY, 0x03u, fx);
    assert(strcmp(fx, "DL03") == 0);
    fmt_tracker_screen_effect_value_text(FMT_TRACKER_EFFECT_NOTE_CUT, 0x00u, fx);
    assert(strcmp(fx, "CT00") == 0);
    fmt_tracker_screen_effect_value_text(0x09u, 0x55u, fx);
    assert(strcmp(fx, "????") == 0);
}

static void test_lock_counts_target_and_dirtying(void)
{
    FmtTrackerScreen screen;
    FmtPatternInput input;
    FmtStepRecord record;
    FmtTransport transport;
    FmtLockEditor editor;
    char lock[3], row[32];
    uint8_t replaced;

    prepare(&screen, &input, &record, &transport, &editor);
    assert(fmt_tracker_screen_sync_feedback(&screen, &input, &record,
                                            &transport, &editor));
    drain(&screen);

    fmt_tracker_screen_lock_text(&screen, 0u, lock);
    assert(strcmp(lock, "--") == 0);

    assert(fmt_lock_set(0u, 0u, 0u, FMT_PARAM_VOICE_LEVEL, 200u, &replaced) ==
           FMT_LOCK_STATUS_OK);
    assert(fmt_tracker_screen_sync_feedback(&screen, &input, &record,
                                            &transport, &editor));
    assert(screen.dirty == (1u << FMT_TRACKER_SCREEN_REGION_ROW0));
    fmt_tracker_screen_lock_text(&screen, 0u, lock);
    assert(strcmp(lock, "L1") == 0);
    drain(&screen);

    assert(fmt_lock_set(0u, 0u, 0u, FMT_PARAM_PITCH_FINE, 120u, &replaced) ==
           FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(0u, 0u, 0u, FMT_PARAM_TRACK_LEVEL, 220u, &replaced) ==
           FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(0u, 0u, 0u, FMT_PARAM_TRACK_PAN, 128u, &replaced) ==
           FMT_LOCK_STATUS_OK);
    assert(fmt_tracker_screen_sync_feedback(&screen, &input, &record,
                                            &transport, &editor));
    fmt_tracker_screen_lock_text(&screen, 0u, lock);
    assert(strcmp(lock, "L4") == 0);
    drain(&screen);

    assert(fmt_lock_editor_begin(&editor, &input, 0u));
    assert(fmt_tracker_screen_sync_feedback(&screen, &input, &record,
                                            &transport, &editor));
    assert(screen.dirty == (1u << FMT_TRACKER_SCREEN_REGION_ROW0));
    fmt_tracker_screen_lock_text(&screen, 0u, lock);
    assert(strcmp(lock, "P4") == 0);
    fmt_tracker_screen_row_text(&screen, 0u, row);
    assert(strstr(row, " P4 ") != NULL);
    drain(&screen);

    fmt_lock_editor_cancel(&editor);
    assert(fmt_tracker_screen_sync_feedback(&screen, &input, &record,
                                            &transport, &editor));
    assert(screen.dirty == (1u << FMT_TRACKER_SCREEN_REGION_ROW0));
    fmt_tracker_screen_lock_text(&screen, 0u, lock);
    assert(strcmp(lock, "L4") == 0);

    screen.row_lock_info[0] = FMT_TRACKER_SCREEN_LOCK_ERROR;
    screen.lock_target_active = 0u;
    fmt_tracker_screen_lock_text(&screen, 0u, lock);
    assert(strcmp(lock, "L!") == 0);
    screen.lock_target_active = 1u;
    screen.lock_target_step = 0u;
    fmt_tracker_screen_lock_text(&screen, 0u, lock);
    assert(strcmp(lock, "P!") == 0);
}

static void test_effect_value_dirty_and_stored_truth(void)
{
    FmtTrackerScreen screen;
    FmtPatternInput input;
    FmtStepRecord record;
    FmtTransport transport;
    FmtLockEditor editor;
    FmtStep *step;
    char row[32];
    uint16_t used_before;

    prepare(&screen, &input, &record, &transport, &editor);
    assert(fmt_tracker_screen_sync_feedback(&screen, &input, &record,
                                            &transport, &editor));
    drain(&screen);
    used_before = fmt_lock_store_used_count();

    step = fmt_store_step(0u, 0u, 0u);
    assert(step);
    step->effect = FMT_TRACKER_EFFECT_ARPEGGIO;
    step->effect_value = 0x37u;
    assert(fmt_tracker_screen_sync_feedback(&screen, &input, &record,
                                            &transport, &editor));
    assert(screen.dirty == (1u << FMT_TRACKER_SCREEN_REGION_ROW0));
    fmt_tracker_screen_row_text(&screen, 0u, row);
    assert(strstr(row, " AR37") != NULL);
    drain(&screen);

    step->effect_value = 0x38u;
    assert(fmt_tracker_screen_sync_feedback(&screen, &input, &record,
                                            &transport, &editor));
    assert(screen.dirty == (1u << FMT_TRACKER_SCREEN_REGION_ROW0));
    fmt_tracker_screen_row_text(&screen, 0u, row);
    assert(strstr(row, " AR38") != NULL);
    drain(&screen);

    step->effect = 0x09u;
    assert(fmt_tracker_screen_sync_feedback(&screen, &input, &record,
                                            &transport, &editor));
    fmt_tracker_screen_row_text(&screen, 0u, row);
    assert(strstr(row, " ????") != NULL);
    assert(step->effect == 0x09u);
    assert(step->effect_value == 0x38u);
    assert(fmt_lock_store_used_count() == used_before);
}

int main(void)
{
    test_exact_state_and_effect_text();
    test_lock_counts_target_and_dirtying();
    test_effect_value_dirty_and_stored_truth();

    puts("WP3_9_VISUAL_TESTS_OK=1");
    printf("screen_bytes=%u rows=%u regions=%u max_regions_per_poll=%u\n",
           (unsigned)sizeof(FmtTrackerScreen),
           (unsigned)FMT_TRACKER_SCREEN_ROWS,
           (unsigned)FMT_TRACKER_SCREEN_REGION_COUNT,
           (unsigned)FMT_TRACKER_SCREEN_MAX_REGIONS_PER_POLL);
    return 0;
}
