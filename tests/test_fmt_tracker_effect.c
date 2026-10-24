/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_tracker_effect.h"
#include "fmt_store.h"

static FmtSong song_before;

static uint32_t effect_word(uint8_t command, uint8_t value)
{
    return fmt_tracker_effect_pack(command, value, 0u);
}

static void reset_all(void)
{
    fmt_store_reset();
    fmt_tracker_effect_cache_reset();
}

static void test_cache_layout_registry_and_fallback(void)
{
    uint32_t step, word;
    FmtStep *row;

    reset_all();
    assert(sizeof(fmt_tracker_effect_cache) == 256u);
    assert(((uintptr_t)&fmt_tracker_effect_cache[0] & 3u) == 0u);
    assert(sizeof(FmtTrackerEffectState) <= 16u);
    assert(FMT_TRANSPORT_PHASE_PER_STEP == 661500u);
    assert(FMT_TRACKER_EFFECT_TICK_PHASE == 110250u);
    assert(FMT_TRACKER_EFFECT_TICKS_PER_ROW == 6u);

    for (step = 0u; step <= 255u; step++) {
        uint8_t expected = step <= FMT_TRACKER_EFFECT_VIBRATO;
        assert(fmt_tracker_effect_command_valid((uint8_t)step) == expected);
    }

    word = fmt_tracker_effect_default_word();
    assert(fmt_tracker_effect_command(word) == FMT_TRACKER_EFFECT_NONE);
    assert(fmt_tracker_effect_value(word) == 0u);
    assert(fmt_tracker_effect_flags(word) == 0u);
    for (step = 0u; step < FMT_TRACKER_EFFECT_ROWS; step++)
        assert(fmt_tracker_effect_cache_read(step) == word);
    assert(fmt_tracker_effect_cache_read(FMT_TRACKER_EFFECT_ROWS) == word);
    assert(!fmt_tracker_effect_cache_refresh_step(FMT_TRACKER_EFFECT_ROWS));
    assert(!fmt_tracker_effect_cache_refresh_step(UINT32_MAX));

    for (step = 0u; step < FMT_TRACKER_EFFECT_ROWS; step++) {
        row = fmt_store_step(FMT_PLAYBACK_PATTERN, FMT_PLAYBACK_TRACK, step);
        assert(row);
        row->effect = (uint8_t)(step % 6u);
        row->effect_value = (uint8_t)(step * 3u);
    }
    fmt_tracker_effect_cache_rebuild();
    for (step = 0u; step < FMT_TRACKER_EFFECT_ROWS; step++) {
        word = fmt_tracker_effect_cache_read(step);
        assert(fmt_tracker_effect_command(word) == (uint8_t)(step % 6u));
        assert(fmt_tracker_effect_value(word) == (uint8_t)(step * 3u));
        assert(fmt_tracker_effect_flags(word) == 0u);
    }

    row = fmt_store_step(FMT_PLAYBACK_PATTERN, FMT_PLAYBACK_TRACK, 17u);
    assert(row);
    row->effect = 0x80u;
    row->effect_value = 0xA5u;
    assert(fmt_tracker_effect_cache_refresh_step(17u));
    word = fmt_tracker_effect_cache_read(17u);
    assert(fmt_tracker_effect_command(word) == FMT_TRACKER_EFFECT_NONE);
    assert(fmt_tracker_effect_value(word) == 0u);
    assert(fmt_tracker_effect_flags(word) == FMT_TRACKER_EFFECT_FLAG_INVALID_COMMAND);
}

static void test_cache_never_mutates_song(void)
{
    FmtStep *row;

    reset_all();
    row = fmt_store_step(0u, 0u, 3u);
    assert(row);
    row->note = 61u;
    row->effect = FMT_TRACKER_EFFECT_ARPEGGIO;
    row->effect_value = 0x47u;
    row = fmt_store_step(2u, 5u, 40u);
    assert(row);
    row->effect = 0xE1u;
    row->effect_value = 0x99u;

    memcpy(&song_before, fmt_store_song_const(), sizeof song_before);
    fmt_tracker_effect_cache_rebuild();
    assert(memcmp(&song_before, fmt_store_song_const(), sizeof song_before) == 0);
    assert(fmt_tracker_effect_cache_refresh_step(3u));
    assert(memcmp(&song_before, fmt_store_song_const(), sizeof song_before) == 0);
}

static void test_tick_thresholds(void)
{
    FmtTrackerEffectState state;
    uint8_t tick;

    fmt_tracker_effect_state_reset(&state);
    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_NOTE, 60u,
                                 effect_word(FMT_TRACKER_EFFECT_ARPEGGIO, 0x47u));
    for (tick = 0u; tick < 5u; tick++) {
        uint32_t threshold = ((uint32_t)tick + 1u) * FMT_TRACKER_EFFECT_TICK_PHASE;
        assert(state.tick == tick);
        assert(!fmt_tracker_effect_tick_due(&state, threshold - 1u));
        assert(fmt_tracker_effect_tick_due(&state, threshold));
        assert(fmt_tracker_effect_tick_due(&state, threshold + 1000u));
        assert(fmt_tracker_effect_advance_tick(&state));
    }
    assert(state.tick == 5u);
    assert(!fmt_tracker_effect_tick_due(&state, FMT_TRANSPORT_PHASE_PER_STEP - 1u));
    assert(!fmt_tracker_effect_advance_tick(&state));
}

static void test_arpeggio_and_fine_precedence(void)
{
    FmtTrackerEffectState state;
    static const int16_t expected_mod[6] = {0, 512, 896, 0, 512, 896};
    uint8_t tick;

    fmt_tracker_effect_state_reset(&state);
    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_NOTE, 60u,
                                 effect_word(FMT_TRACKER_EFFECT_ARPEGGIO, 0x47u));
    assert(state.pitch_q7 == 7680u && state.pitch_valid);
    assert(state.transient_q7 == expected_mod[0]);
    assert(fmt_tracker_effect_final_pitch_q7(&state, 128u) == 7680u);
    assert(fmt_tracker_effect_final_pitch_q7(&state, 192u) == 7744u);

    for (tick = 1u; tick < 6u; tick++) {
        assert(fmt_tracker_effect_advance_tick(&state));
        assert(state.transient_q7 == expected_mod[tick]);
        assert(state.pitch_q7 == 7680u);
        assert(fmt_tracker_effect_final_pitch_q7(&state, 128u) ==
               (uint16_t)(7680 + expected_mod[tick]));
    }
    assert(fmt_tracker_effect_final_pitch_q7(&state, 192u) ==
           (uint16_t)(7680 + 64 + expected_mod[5]));
}

static void test_slide_up_down_and_saturation(void)
{
    FmtTrackerEffectState state;
    uint8_t i;

    fmt_tracker_effect_state_reset(&state);
    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_NOTE, 60u,
                                 effect_word(FMT_TRACKER_EFFECT_SLIDE_UP, 128u));
    assert(state.pitch_q7 == 7680u);
    for (i = 0u; i < 5u; i++)
        assert(fmt_tracker_effect_advance_tick(&state));
    assert(state.pitch_q7 == 8320u);
    assert(state.transient_q7 == 0);

    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE,
                                 effect_word(FMT_TRACKER_EFFECT_SLIDE_DOWN, 64u));
    for (i = 0u; i < 5u; i++)
        assert(fmt_tracker_effect_advance_tick(&state));
    assert(state.pitch_q7 == 8000u);

    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_NOTE, 127u,
                                 effect_word(FMT_TRACKER_EFFECT_SLIDE_UP, 255u));
    assert(fmt_tracker_effect_advance_tick(&state));
    assert(state.pitch_q7 == 16383u);
    for (i = 0u; i < 4u; i++)
        assert(fmt_tracker_effect_advance_tick(&state));
    assert(state.pitch_q7 == 16383u);

    fmt_tracker_effect_state_reset(&state);
    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_NOTE, 0u,
                                 effect_word(FMT_TRACKER_EFFECT_SLIDE_DOWN, 255u));
    assert(fmt_tracker_effect_advance_tick(&state));
    assert(state.pitch_q7 == 0u);
}

static void test_portamento_target_resume_and_clear(void)
{
    FmtTrackerEffectState state;

    fmt_tracker_effect_state_reset(&state);
    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_NOTE, 60u,
                                 effect_word(FMT_TRACKER_EFFECT_NONE, 0u));
    assert(state.pitch_q7 == 7680u && state.pitch_valid);

    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_NOTE, 64u,
                                 effect_word(FMT_TRACKER_EFFECT_PORTAMENTO, 128u));
    assert(state.pitch_q7 == 7680u);
    assert(state.porta_target_q7 == 8192u && state.porta_valid);
    assert(fmt_tracker_effect_advance_tick(&state));
    assert(state.pitch_q7 == 7808u && state.porta_valid);

    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_NONE, FMT_NOTE_EMPTY,
                                 effect_word(FMT_TRACKER_EFFECT_NONE, 0u));
    assert(state.pitch_q7 == 7808u && state.porta_valid);
    assert(fmt_tracker_effect_advance_tick(&state));
    assert(state.pitch_q7 == 7808u);

    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE,
                                 effect_word(FMT_TRACKER_EFFECT_PORTAMENTO, 128u));
    assert(fmt_tracker_effect_advance_tick(&state));
    assert(state.pitch_q7 == 7936u && state.porta_valid);
    assert(fmt_tracker_effect_advance_tick(&state));
    assert(state.pitch_q7 == 8064u && state.porta_valid);
    assert(fmt_tracker_effect_advance_tick(&state));
    assert(state.pitch_q7 == 8192u && !state.porta_valid);

    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_NOTE, 72u,
                                 effect_word(FMT_TRACKER_EFFECT_PORTAMENTO, 0u));
    assert(state.pitch_q7 == 8192u && state.porta_valid);
    assert(fmt_tracker_effect_advance_tick(&state));
    assert(state.pitch_q7 == 8192u && state.porta_valid);

    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_NOTE, 67u,
                                 effect_word(FMT_TRACKER_EFFECT_NONE, 0u));
    assert(state.pitch_q7 == (67u << 7));
    assert(!state.porta_valid);

    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_NOTE, 72u,
                                 effect_word(FMT_TRACKER_EFFECT_PORTAMENTO, 32u));
    assert(state.porta_valid);
    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE,
                                 effect_word(FMT_TRACKER_EFFECT_SLIDE_UP, 1u));
    assert(!state.porta_valid);

    fmt_tracker_effect_state_reset(&state);
    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_NOTE, 64u,
                                 effect_word(FMT_TRACKER_EFFECT_PORTAMENTO, 128u));
    assert(state.pitch_q7 == 8192u && state.pitch_valid && !state.porta_valid);
}

static void test_vibrato_phase_depth_and_reset(void)
{
    FmtTrackerEffectState state;
    static const uint8_t phases[6] = {0u, 4u, 8u, 12u, 16u, 20u};
    static const int16_t mods[6] = {0, 8, 16, 24, 32, 24};
    uint8_t tick;

    assert(fmt_tracker_effect_vibrato_triangle(0u) == 0);
    assert(fmt_tracker_effect_vibrato_triangle(16u) == 32);
    assert(fmt_tracker_effect_vibrato_triangle(32u) == 0);
    assert(fmt_tracker_effect_vibrato_triangle(48u) == -32);
    assert(fmt_tracker_effect_vibrato_triangle(63u) == -2);

    fmt_tracker_effect_state_reset(&state);
    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_NOTE, 60u,
                                 effect_word(FMT_TRACKER_EFFECT_VIBRATO, 0x48u));
    for (tick = 0u; tick < 6u; tick++) {
        assert(state.vibrato_phase == phases[tick]);
        assert(state.transient_q7 == mods[tick]);
        if (tick < 5u)
            assert(fmt_tracker_effect_advance_tick(&state));
    }

    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE,
                                 effect_word(FMT_TRACKER_EFFECT_VIBRATO, 0x48u));
    assert(state.vibrato_phase == 20u);
    assert(state.transient_q7 == 24);

    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_NOTE, 62u,
                                 effect_word(FMT_TRACKER_EFFECT_VIBRATO, 0x48u));
    assert(state.vibrato_phase == 0u && state.transient_q7 == 0);

    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE,
                                 effect_word(FMT_TRACKER_EFFECT_NONE, 0u));
    assert(state.vibrato_phase == 0u && state.transient_q7 == 0);
}

static void test_note_tie_empty_off_and_invalid_cannot_resurrect(void)
{
    FmtTrackerEffectState state;

    fmt_tracker_effect_state_reset(&state);
    assert(!state.pitch_valid);

    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE,
                                 effect_word(FMT_TRACKER_EFFECT_SLIDE_UP, 255u));
    assert(!state.pitch_valid);
    assert(fmt_tracker_effect_advance_tick(&state));
    assert(!state.pitch_valid && state.transient_q7 == 0);

    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_NOTE, 60u,
                                 effect_word(0x99u, 0xFFu));
    assert(state.pitch_valid);
    assert(state.command == FMT_TRACKER_EFFECT_NONE);
    assert(state.pitch_q7 == 7680u);

    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_NONE, FMT_NOTE_EMPTY,
                                 effect_word(FMT_TRACKER_EFFECT_SLIDE_UP, 10u));
    assert(fmt_tracker_effect_advance_tick(&state));
    assert(state.pitch_q7 == 7690u);

    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_OFF, FMT_NOTE_OFF,
                                 effect_word(FMT_TRACKER_EFFECT_VIBRATO, 0xFFu));
    assert(!state.pitch_valid);
    assert(!state.porta_valid);
    assert(state.transient_q7 == 0);
    assert(state.command == FMT_TRACKER_EFFECT_NONE);
    assert(fmt_tracker_effect_final_pitch_q7(&state, 128u) == 0u);

    fmt_tracker_effect_begin_row(&state, FMT_PLAYBACK_EVENT_NONE, FMT_NOTE_EMPTY,
                                 effect_word(FMT_TRACKER_EFFECT_ARPEGGIO, 0xFFu));
    assert(!state.pitch_valid);
    assert(fmt_tracker_effect_advance_tick(&state));
    assert(!state.pitch_valid);
}

int main(void)
{
    test_cache_layout_registry_and_fallback();
    test_cache_never_mutates_song();
    test_tick_thresholds();
    test_arpeggio_and_fine_precedence();
    test_slide_up_down_and_saturation();
    test_portamento_target_resume_and_clear();
    test_vibrato_phase_depth_and_reset();
    test_note_tie_empty_off_and_invalid_cannot_resurrect();

    printf("FMT_TRACKER_EFFECT_TESTS_OK=1\n");
    printf("cache_bytes=%zu rows=%u commands=none,arp,slide_up,slide_down,porta,vibrato ticks_per_row=%u tick_phase=%u\n",
           sizeof(fmt_tracker_effect_cache),
           (unsigned)FMT_TRACKER_EFFECT_ROWS,
           (unsigned)FMT_TRACKER_EFFECT_TICKS_PER_ROW,
           (unsigned)FMT_TRACKER_EFFECT_TICK_PHASE);
    printf("state_bytes=%zu pitch_q7=1 fine_precedence=1 porta_memory=1 invalid_fallback=1 no_store_mutation=1\n",
           sizeof(FmtTrackerEffectState));
    return 0;
}
