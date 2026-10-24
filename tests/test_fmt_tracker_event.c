/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "fmt_tracker_event.h"

static uint32_t word(uint8_t command, uint8_t value)
{
    return fmt_tracker_effect_pack(command, value, 0u);
}

static void test_size_and_command_domain(void)
{
    uint32_t c;

    assert(sizeof(FmtTrackerRowEventState) == 4u);
    for (c = 0u; c <= FMT_TRACKER_EFFECT_NOTE_CUT; c++)
        assert(fmt_tracker_effect_command_valid((uint8_t)c));
    assert(!fmt_tracker_effect_command_valid(0x09u));
    assert(!fmt_tracker_effect_command_valid(0xFFu));
}

static void test_delay(void)
{
    FmtTrackerRowEventState s;
    uint8_t kind, note, out, action;
    uint32_t v, t;

    for (v = 0u; v <= 6u; v++) {
        fmt_tracker_row_event_begin(&s, FMT_PLAYBACK_EVENT_NOTE, 64u,
                                    word(FMT_TRACKER_EFFECT_NOTE_DELAY,
                                         (uint8_t)v),
                                    &kind, &note);
        if (v >= 1u && v <= 5u) {
            assert(kind == FMT_PLAYBACK_EVENT_NONE);
            assert(note == FMT_NOTE_EMPTY);
            assert(s.pending_valid == 1u);
        } else {
            assert(kind == FMT_PLAYBACK_EVENT_NOTE);
            assert(note == 64u);
            assert(s.pending_valid == 0u);
        }

        for (t = 0u; t <= 6u; t++) {
            action = fmt_tracker_row_event_action(&s, (uint8_t)t, 0u, &out);
            if (v >= 1u && v <= 5u && t == v) {
                assert(action == FMT_TRACKER_ROW_ACTION_DELAYED_NOTE);
                assert(out == 64u);
            } else {
                assert(action == FMT_TRACKER_ROW_ACTION_NONE);
                assert(out == FMT_NOTE_EMPTY);
            }
        }
        assert(s.pending_valid == 0u);
    }

    /* Delay never suppresses TIE/NONE/OFF. */
    fmt_tracker_row_event_begin(&s, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE,
                                word(FMT_TRACKER_EFFECT_NOTE_DELAY, 3u),
                                &kind, &note);
    assert(kind == FMT_PLAYBACK_EVENT_TIE && note == FMT_NOTE_TIE);
    assert(!s.pending_valid);

    fmt_tracker_row_event_begin(&s, FMT_PLAYBACK_EVENT_NONE, FMT_NOTE_EMPTY,
                                word(FMT_TRACKER_EFFECT_NOTE_DELAY, 3u),
                                &kind, &note);
    assert(kind == FMT_PLAYBACK_EVENT_NONE && note == FMT_NOTE_EMPTY);
    assert(!s.pending_valid);

    fmt_tracker_row_event_begin(&s, FMT_PLAYBACK_EVENT_OFF, FMT_NOTE_OFF,
                                word(FMT_TRACKER_EFFECT_NOTE_DELAY, 3u),
                                &kind, &note);
    assert(kind == FMT_PLAYBACK_EVENT_OFF && note == FMT_NOTE_OFF);
    assert(!s.pending_valid);
}

static void test_cut(void)
{
    FmtTrackerRowEventState s;
    uint8_t kind, note, out, action;
    uint32_t v, t;

    for (v = 0u; v <= 6u; v++) {
        fmt_tracker_row_event_begin(&s, FMT_PLAYBACK_EVENT_NONE, FMT_NOTE_EMPTY,
                                    word(FMT_TRACKER_EFFECT_NOTE_CUT,
                                         (uint8_t)v),
                                    &kind, &note);
        assert(kind == FMT_PLAYBACK_EVENT_NONE);
        for (t = 0u; t <= 6u; t++) {
            action = fmt_tracker_row_event_action(&s, (uint8_t)t, 1u, &out);
            if (v <= 5u && t == v)
                assert(action == FMT_TRACKER_ROW_ACTION_CUT);
            else
                assert(action == FMT_TRACKER_ROW_ACTION_NONE);
        }
    }

    fmt_tracker_row_event_begin(&s, FMT_PLAYBACK_EVENT_NONE, FMT_NOTE_EMPTY,
                                word(FMT_TRACKER_EFFECT_NOTE_CUT, 2u),
                                &kind, &note);
    assert(fmt_tracker_row_event_action(&s, 2u, 0u, &out) ==
           FMT_TRACKER_ROW_ACTION_NONE);
}

static void test_retrig(void)
{
    FmtTrackerRowEventState s;
    uint8_t kind, note, out, action;
    uint32_t r, t;

    for (r = 0u; r <= 6u; r++) {
        fmt_tracker_row_event_begin(&s, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE,
                                    word(FMT_TRACKER_EFFECT_RETRIG,
                                         (uint8_t)r),
                                    &kind, &note);
        assert(kind == FMT_PLAYBACK_EVENT_TIE);
        for (t = 0u; t <= 6u; t++) {
            action = fmt_tracker_row_event_action(&s, (uint8_t)t, 1u, &out);
            if (t >= 1u && t <= 5u && r >= 1u && r <= 5u &&
                (t % r) == 0u)
                assert(action == FMT_TRACKER_ROW_ACTION_RETRIG);
            else
                assert(action == FMT_TRACKER_ROW_ACTION_NONE);
        }
    }

    fmt_tracker_row_event_begin(&s, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE,
                                word(FMT_TRACKER_EFFECT_RETRIG, 1u),
                                &kind, &note);
    assert(fmt_tracker_row_event_action(&s, 1u, 0u, &out) ==
           FMT_TRACKER_ROW_ACTION_NONE);
}

static void test_non_event_passthrough(void)
{
    FmtTrackerRowEventState s;
    uint8_t kind, note, out;
    uint32_t c;

    for (c = FMT_TRACKER_EFFECT_NONE;
         c <= FMT_TRACKER_EFFECT_VIBRATO; c++) {
        fmt_tracker_row_event_begin(&s, FMT_PLAYBACK_EVENT_NOTE, 60u,
                                    word((uint8_t)c, 0xABu),
                                    &kind, &note);
        assert(kind == FMT_PLAYBACK_EVENT_NOTE && note == 60u);
        assert(s.pending_valid == 0u);
        assert(fmt_tracker_row_event_action(&s, 3u, 1u, &out) ==
               FMT_TRACKER_ROW_ACTION_NONE);
    }
}

static void test_effect_late_helpers(void)
{
    FmtTrackerEffectState e;

    memset(&e, 0, sizeof(e));
    e.command = FMT_TRACKER_EFFECT_NOTE_DELAY;
    e.value = 3u;
    e.tick = 3u;
    e.pitch_q7 = (uint16_t)(60u << 7);
    e.pitch_valid = 1u;
    e.porta_target_q7 = (uint16_t)(72u << 7);
    e.porta_valid = 1u;
    e.transient_q7 = 123;
    e.vibrato_phase = 17u;

    fmt_tracker_effect_apply_delayed_note(&e, 67u);
    assert(e.command == FMT_TRACKER_EFFECT_NOTE_DELAY);
    assert(e.value == 3u && e.tick == 3u);
    assert(e.pitch_q7 == (uint16_t)(67u << 7));
    assert(e.pitch_valid == 1u);
    assert(e.porta_valid == 0u);
    assert(e.transient_q7 == 0);
    assert(e.vibrato_phase == 0u);

    e.porta_valid = 1u;
    e.transient_q7 = 77;
    e.vibrato_phase = 9u;
    fmt_tracker_effect_apply_cut(&e);
    assert(e.command == FMT_TRACKER_EFFECT_NOTE_DELAY);
    assert(e.value == 3u && e.tick == 3u);
    assert(e.pitch_valid == 0u);
    assert(e.porta_valid == 0u);
    assert(e.transient_q7 == 0);
    assert(e.vibrato_phase == 0u);
}

int main(void)
{
    test_size_and_command_domain();
    test_delay();
    test_cut();
    test_retrig();
    test_non_event_passthrough();
    test_effect_late_helpers();
    puts("FMT_TRACKER_EVENT_TESTS_OK=1");
    puts("state_bytes=4 delay_ticks=0..5 cut_ticks=0..5 retrig_intervals=0..5 invalid_value_noop=1 no_tick6=1 no_queue=1");
    return 0;
}
