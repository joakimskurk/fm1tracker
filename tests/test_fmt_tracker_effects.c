/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "fmt_tracker_effects.h"

static FmtSong song_before;

static void tick(FmtTrackerEffectState *state, uint8_t n)
{
    uint32_t phase = (uint32_t)n * FMT_TRACKER_EFFECT_TICK_PHASE;
    assert(n >= 1u && n <= 5u);
    assert(fmt_tracker_effect_advance(state, phase, 1u));
    assert(state->tick == n);
}

static void test_cache(void)
{
    uint32_t i, w;
    FmtStep *row;

    fmt_store_reset();
    fmt_tracker_effect_cache_reset();
    for (i = 0u; i < FMT_TRACKER_EFFECT_ROWS; i++)
        assert(fmt_tracker_effect_cache_read(i) == fmt_tracker_effect_default_word());

    row = fmt_store_step(0u, 0u, 3u);
    assert(row);
    row->effect = FMT_TRACKER_EFFECT_ARPEGGIO;
    row->effect_value = 0x47u;
    row = fmt_store_step(0u, 0u, 4u);
    assert(row);
    row->effect = 0x80u;
    row->effect_value = 99u;

    memcpy(&song_before, fmt_store_song_const(), sizeof(song_before));
    fmt_tracker_effect_cache_rebuild();
    assert(memcmp(&song_before, fmt_store_song_const(), sizeof(song_before)) == 0);

    w = fmt_tracker_effect_cache_read(3u);
    assert(fmt_tracker_effect_command(w) == FMT_TRACKER_EFFECT_ARPEGGIO);
    assert(fmt_tracker_effect_value(w) == 0x47u);
    assert(fmt_tracker_effect_flags(w) == FMT_TRACKER_EFFECT_FLAG_NONE);

    w = fmt_tracker_effect_cache_read(4u);
    assert(fmt_tracker_effect_command(w) == FMT_TRACKER_EFFECT_NONE);
    assert(fmt_tracker_effect_value(w) == 0u);
    assert(fmt_tracker_effect_flags(w) == FMT_TRACKER_EFFECT_FLAG_INVALID_COMMAND);
    assert((w >> 24) == 0u);

    assert(!fmt_tracker_effect_cache_refresh_step(64u));
    assert(fmt_tracker_effect_cache_read(64u) == fmt_tracker_effect_default_word());
}

static void test_arpeggio_and_fine(void)
{
    FmtTrackerEffectState s;
    uint32_t arp = fmt_tracker_effect_pack(FMT_TRACKER_EFFECT_ARPEGGIO, 0x47u, 0u);

    fmt_tracker_effect_state_reset(&s);
    fmt_tracker_effect_apply_row(&s, FMT_PLAYBACK_EVENT_NOTE, 60u,
                                 fmt_tracker_effect_default_word(), 128u, 0u);
    assert(s.persistent_pitch_q7 == 60u * 128u);
    assert(fmt_tracker_effect_final_pitch_q7(&s) == 60u * 128u);

    fmt_tracker_effect_apply_row(&s, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE,
                                 arp, 192u, 1u);
    assert(!fmt_tracker_effect_advance(&s, FMT_TRACKER_EFFECT_TICK_PHASE - 1u, 1u));
    tick(&s, 1u);
    assert(s.transient_mod_q7 == 4 * 128);
    assert(fmt_tracker_effect_final_pitch_q7(&s) == 60u * 128u + 64u + 4u * 128u);
    tick(&s, 2u);
    assert(s.transient_mod_q7 == 7 * 128);
    tick(&s, 3u);
    assert(s.transient_mod_q7 == 0);
    tick(&s, 4u);
    assert(s.transient_mod_q7 == 4 * 128);
    tick(&s, 5u);
    assert(s.transient_mod_q7 == 7 * 128);
    assert(!fmt_tracker_effect_advance(&s, FMT_TRANSPORT_PHASE_PER_STEP - 1u, 1u));
}

static void test_slides(void)
{
    FmtTrackerEffectState s;
    uint32_t up = fmt_tracker_effect_pack(FMT_TRACKER_EFFECT_SLIDE_UP, 64u, 0u);
    uint32_t down = fmt_tracker_effect_pack(FMT_TRACKER_EFFECT_SLIDE_DOWN, 64u, 0u);
    uint8_t i;

    fmt_tracker_effect_state_reset(&s);
    fmt_tracker_effect_apply_row(&s, FMT_PLAYBACK_EVENT_NOTE, 60u,
                                 fmt_tracker_effect_default_word(), 128u, 0u);
    fmt_tracker_effect_apply_row(&s, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE,
                                 up, 128u, 1u);
    for (i = 1u; i <= 5u; i++) tick(&s, i);
    assert(s.persistent_pitch_q7 == 60u * 128u + 5u * 64u);

    fmt_tracker_effect_apply_row(&s, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE,
                                 down, 128u, 1u);
    for (i = 1u; i <= 5u; i++) tick(&s, i);
    assert(s.persistent_pitch_q7 == 60u * 128u);

    fmt_tracker_effect_apply_row(&s, FMT_PLAYBACK_EVENT_NOTE, 127u,
                                 up, 128u, 1u);
    for (i = 1u; i <= 5u; i++) tick(&s, i);
    assert(s.persistent_pitch_q7 == 16383u);

    fmt_tracker_effect_apply_row(&s, FMT_PLAYBACK_EVENT_NOTE, 0u,
                                 down, 128u, 1u);
    for (i = 1u; i <= 5u; i++) tick(&s, i);
    assert(s.persistent_pitch_q7 == 0u);
}

static void test_portamento(void)
{
    FmtTrackerEffectState s;
    uint32_t porta128 = fmt_tracker_effect_pack(FMT_TRACKER_EFFECT_PORTAMENTO, 128u, 0u);
    uint32_t porta64 = fmt_tracker_effect_pack(FMT_TRACKER_EFFECT_PORTAMENTO, 64u, 0u);
    uint8_t i;
    uint16_t paused;

    fmt_tracker_effect_state_reset(&s);
    fmt_tracker_effect_apply_row(&s, FMT_PLAYBACK_EVENT_NOTE, 60u,
                                 fmt_tracker_effect_default_word(), 128u, 0u);
    fmt_tracker_effect_apply_row(&s, FMT_PLAYBACK_EVENT_NOTE, 64u,
                                 porta128, 128u, 1u);
    assert(s.persistent_pitch_q7 == 60u * 128u);
    assert(s.portamento_target_q7 == 64u * 128u);
    assert(s.portamento_valid);
    for (i = 1u; i <= 4u; i++) tick(&s, i);
    assert(s.persistent_pitch_q7 == 64u * 128u);
    assert(!s.portamento_valid);
    tick(&s, 5u);

    fmt_tracker_effect_apply_row(&s, FMT_PLAYBACK_EVENT_NOTE, 67u,
                                 porta64, 128u, 1u);
    tick(&s, 1u);
    tick(&s, 2u);
    paused = s.persistent_pitch_q7;
    assert(s.portamento_valid);

    fmt_tracker_effect_apply_row(&s, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE,
                                 fmt_tracker_effect_default_word(), 128u, 1u);
    tick(&s, 1u);
    assert(s.persistent_pitch_q7 == paused);
    assert(s.portamento_valid);

    fmt_tracker_effect_apply_row(&s, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE,
                                 porta64, 128u, 1u);
    tick(&s, 1u);
    assert(s.persistent_pitch_q7 == (uint16_t)(paused + 64u));

    /* A normal NOTE clears the remembered target. */
    fmt_tracker_effect_apply_row(&s, FMT_PLAYBACK_EVENT_NOTE, 55u,
                                 fmt_tracker_effect_default_word(), 128u, 1u);
    assert(s.persistent_pitch_q7 == 55u * 128u);
    assert(!s.portamento_valid);

    /* With no previous active note PORTAMENTO starts directly at target. */
    fmt_tracker_effect_state_reset(&s);
    fmt_tracker_effect_apply_row(&s, FMT_PLAYBACK_EVENT_NOTE, 72u,
                                 porta64, 128u, 0u);
    assert(s.persistent_pitch_q7 == 72u * 128u);
    assert(!s.portamento_valid);
}

static void test_vibrato_and_off(void)
{
    FmtTrackerEffectState s;
    uint32_t vib = fmt_tracker_effect_pack(FMT_TRACKER_EFFECT_VIBRATO, 0x42u, 0u);
    uint8_t phase_after;

    assert(fmt_tracker_effect_triangle64(0u) == 0);
    assert(fmt_tracker_effect_triangle64(16u) == 32);
    assert(fmt_tracker_effect_triangle64(32u) == 0);
    assert(fmt_tracker_effect_triangle64(48u) == -32);
    assert(fmt_tracker_effect_triangle64(63u) == -2);

    fmt_tracker_effect_state_reset(&s);
    fmt_tracker_effect_apply_row(&s, FMT_PLAYBACK_EVENT_NOTE, 60u,
                                 vib, 128u, 0u);
    assert(s.vibrato_phase == 0u && s.transient_mod_q7 == 0);
    tick(&s, 1u);
    assert(s.vibrato_phase == 4u);
    assert(s.transient_mod_q7 == 2);
    tick(&s, 2u);
    assert(s.vibrato_phase == 8u);
    assert(s.transient_mod_q7 == 4);
    tick(&s, 3u); tick(&s, 4u); tick(&s, 5u);
    phase_after = s.vibrato_phase;

    fmt_tracker_effect_apply_row(&s, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE,
                                 vib, 128u, 1u);
    assert(s.vibrato_phase == phase_after);

    fmt_tracker_effect_apply_row(&s, FMT_PLAYBACK_EVENT_OFF, FMT_NOTE_OFF,
                                 vib, 128u, 1u);
    assert(s.command == FMT_TRACKER_EFFECT_NONE);
    assert(s.transient_mod_q7 == 0);
    assert(s.vibrato_phase == 0u);
    assert(!s.portamento_valid);

    /* TIE/effect after OFF cannot create audible transient when inactive. */
    fmt_tracker_effect_apply_row(&s, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE,
                                 fmt_tracker_effect_pack(FMT_TRACKER_EFFECT_ARPEGGIO, 0x47u, 0u),
                                 128u, 0u);
    assert(fmt_tracker_effect_advance(&s, FMT_TRACKER_EFFECT_TICK_PHASE, 0u));
    assert(s.transient_mod_q7 == 0);
}

int main(void)
{
    assert(FMT_TRANSPORT_PHASE_PER_STEP == FMT_TRACKER_EFFECT_TICK_PHASE * 6u);
    assert(sizeof(FmtTrackerEffectState) <= 16u);
    assert(sizeof(fmt_tracker_effect_cache) == 256u);

    test_cache();
    test_arpeggio_and_fine();
    test_slides();
    test_portamento();
    test_vibrato_and_off();

    printf("FMT_TRACKER_EFFECT_TESTS_OK=1\n");
    printf("state_bytes=%zu cache_bytes=%zu ticks=6 tick_phase=%u commands=none,arp,slide_up,slide_down,porta,vibrato\n",
           sizeof(FmtTrackerEffectState), sizeof(fmt_tracker_effect_cache),
           FMT_TRACKER_EFFECT_TICK_PHASE);
    printf("persistent_pitch=1 fine_precedence=1 invalid_fallback=1 store_mutation=0\n");
    return 0;
}
