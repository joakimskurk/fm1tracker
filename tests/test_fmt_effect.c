/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdio.h>
#include <string.h>

#include "fmt_effect.h"
#include "fmt_store.h"

#define CHECK(expr) do { \
    if (!(expr)) { \
        fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); \
        return 1; \
    } \
} while (0)

static int test_cache(void)
{
    FmtSong before;
    FmtStep *row;
    uint32_t i, word;

    fmt_store_reset();
    fmt_effect_cache_reset();
    CHECK(sizeof(fmt_effect_cache) == 256u);
    CHECK(sizeof(FmtEffectState) <= 16u);
    CHECK(sizeof(FmtPlaybackEvent) == 4u);

    for (i = 0u; i < FMT_EFFECT_ROWS; i++)
        CHECK(fmt_effect_cache_read(i) == fmt_effect_default_word());

    for (i = 0u; i <= FMT_FX_VIBRATO; i++) {
        row = fmt_store_step(FMT_PLAYBACK_PATTERN, FMT_PLAYBACK_TRACK, i);
        CHECK(row != NULL);
        row->effect = (uint8_t)i;
        row->effect_value = (uint8_t)(0xA0u + i);
    }
    row = fmt_store_step(FMT_PLAYBACK_PATTERN, FMT_PLAYBACK_TRACK, 6u);
    CHECK(row != NULL);
    row->effect = 0x80u;
    row->effect_value = 0x55u;

    memcpy(&before, fmt_store_song_const(), sizeof(before));
    fmt_effect_cache_rebuild();
    CHECK(memcmp(&before, fmt_store_song_const(), sizeof(before)) == 0);

    word = fmt_effect_cache_read(0u);
    CHECK(fmt_effect_command(word) == FMT_FX_NONE);
    CHECK(fmt_effect_value(word) == 0u);
    CHECK(fmt_effect_flags(word) == FMT_EFFECT_FLAG_NONE);

    for (i = 1u; i <= FMT_FX_VIBRATO; i++) {
        word = fmt_effect_cache_read(i);
        CHECK(fmt_effect_command(word) == i);
        CHECK(fmt_effect_value(word) == (uint8_t)(0xA0u + i));
        CHECK(fmt_effect_flags(word) == FMT_EFFECT_FLAG_NONE);
    }

    word = fmt_effect_cache_read(6u);
    CHECK(fmt_effect_command(word) == FMT_FX_NONE);
    CHECK(fmt_effect_value(word) == 0u);
    CHECK(fmt_effect_flags(word) == FMT_EFFECT_FLAG_INVALID_COMMAND);
    CHECK(fmt_effect_cache_refresh_step(FMT_EFFECT_ROWS) == 0);
    CHECK(fmt_effect_cache_read(FMT_EFFECT_ROWS) == fmt_effect_default_word());
    return 0;
}

static int test_tick_grid(void)
{
    FmtTransport t;
    uint32_t frame;
    uint8_t seen = 0u;
    const uint32_t expected[5] = {919u, 1838u, 2757u, 3675u, 4594u};

    CHECK(FMT_EFFECT_PHASE_PER_TICK == 110250u);
    CHECK(fmt_effect_tick_for_phase(0u) == 0u);
    CHECK(fmt_effect_tick_for_phase(110249u) == 0u);
    CHECK(fmt_effect_tick_for_phase(110250u) == 1u);
    CHECK(fmt_effect_tick_for_phase(220499u) == 1u);
    CHECK(fmt_effect_tick_for_phase(220500u) == 2u);
    CHECK(fmt_effect_tick_for_phase(330750u) == 3u);
    CHECK(fmt_effect_tick_for_phase(441000u) == 4u);
    CHECK(fmt_effect_tick_for_phase(551250u) == 5u);
    CHECK(fmt_effect_tick_for_phase(661499u) == 5u);

    fmt_transport_reset(&t);
    fmt_transport_start(&t);
    for (frame = 0u; frame < 5513u; frame++) {
        uint8_t old_tick = fmt_effect_tick_for_phase(t.phase);
        uint8_t boundary = fmt_transport_tick_frame(&t);
        if (!boundary) {
            uint8_t new_tick = fmt_effect_tick_for_phase(t.phase);
            if (new_tick != old_tick) {
                CHECK(seen < 5u);
                CHECK(frame + 1u == expected[seen]);
                seen++;
            }
        }
    }
    CHECK(seen == 5u);
    CHECK(t.step == 1u);
    return 0;
}

static int test_arpeggio_and_fine(void)
{
    FmtEffectState s;
    uint32_t word = fmt_effect_pack(FMT_FX_ARPEGGIO, 0x47u, 0u);

    fmt_effect_state_reset(&s);
    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_NOTE, 60u, word);
    CHECK(s.note_active == 1u);
    CHECK(s.persistent_pitch_q7 == 60u * 128u);
    CHECK(fmt_effect_state_final_pitch_q7(&s, 128u) == 60u * 128u);
    CHECK(fmt_effect_state_advance_phase(&s, 110250u) == 1u);
    CHECK(fmt_effect_state_final_pitch_q7(&s, 128u) == 64u * 128u);
    CHECK(fmt_effect_state_final_pitch_q7(&s, 192u) == 64u * 128u + 64u);
    CHECK(fmt_effect_state_advance_phase(&s, 220500u) == 1u);
    CHECK(fmt_effect_state_final_pitch_q7(&s, 128u) == 67u * 128u);
    CHECK(fmt_effect_state_advance_phase(&s, 330750u) == 1u);
    CHECK(fmt_effect_state_final_pitch_q7(&s, 128u) == 60u * 128u);
    CHECK(s.persistent_pitch_q7 == 60u * 128u);
    return 0;
}

static int test_slide(void)
{
    FmtEffectState s;
    uint32_t up = fmt_effect_pack(FMT_FX_SLIDE_UP, 128u, 0u);
    uint32_t down = fmt_effect_pack(FMT_FX_SLIDE_DOWN, 255u, 0u);
    uint32_t none = fmt_effect_default_word();
    uint32_t phase;

    fmt_effect_state_reset(&s);
    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_NOTE, 60u, up);
    for (phase = 110250u; phase <= 551250u; phase += 110250u)
        CHECK(fmt_effect_state_advance_phase(&s, phase) == 1u);
    CHECK(s.persistent_pitch_q7 == 65u * 128u);

    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_NONE, FMT_NOTE_EMPTY, none);
    CHECK(s.persistent_pitch_q7 == 65u * 128u);
    CHECK(fmt_effect_state_final_pitch_q7(&s, 128u) == 65u * 128u);

    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_NOTE, 62u, down);
    CHECK(s.persistent_pitch_q7 == 62u * 128u);
    CHECK(fmt_effect_state_advance_phase(&s, 110250u) == 1u);
    CHECK(s.persistent_pitch_q7 == 62u * 128u - 255u);

    fmt_effect_state_reset(&s);
    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_NOTE, 0u, down);
    CHECK(fmt_effect_state_advance_phase(&s, 110250u) == 1u);
    CHECK(s.persistent_pitch_q7 == 0u);
    return 0;
}

static int test_portamento(void)
{
    FmtEffectState s;
    uint32_t none = fmt_effect_default_word();
    uint32_t porta = fmt_effect_pack(FMT_FX_PORTAMENTO, 128u, 0u);
    uint32_t hold = fmt_effect_pack(FMT_FX_PORTAMENTO, 0u, 0u);

    fmt_effect_state_reset(&s);
    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_NOTE, 60u, none);
    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_NOTE, 64u, porta);
    CHECK(s.persistent_pitch_q7 == 60u * 128u);
    CHECK(s.porta_valid == 1u);
    CHECK(s.porta_target_q7 == 64u * 128u);
    CHECK(fmt_effect_state_advance_phase(&s, 110250u) == 1u);
    CHECK(s.persistent_pitch_q7 == 61u * 128u);
    CHECK(fmt_effect_state_advance_phase(&s, 220500u) == 1u);
    CHECK(s.persistent_pitch_q7 == 62u * 128u);
    CHECK(fmt_effect_state_advance_phase(&s, 330750u) == 1u);
    CHECK(s.persistent_pitch_q7 == 63u * 128u);
    CHECK(fmt_effect_state_advance_phase(&s, 441000u) == 1u);
    CHECK(s.persistent_pitch_q7 == 64u * 128u);
    CHECK(s.porta_valid == 0u);

    fmt_effect_state_reset(&s);
    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_NOTE, 60u, none);
    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_NOTE, 67u, hold);
    CHECK(s.porta_valid == 1u);
    CHECK(fmt_effect_state_advance_phase(&s, 551250u) == 1u);
    CHECK(s.persistent_pitch_q7 == 60u * 128u);
    CHECK(s.porta_valid == 1u);

    /* A non-portamento TIE/EMPTY row pauses but retains the target. */
    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_NONE, FMT_NOTE_EMPTY, none);
    CHECK(s.porta_valid == 1u);
    CHECK(s.porta_target_q7 == 67u * 128u);
    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE, porta);
    CHECK(fmt_effect_state_advance_phase(&s, 110250u) == 1u);
    CHECK(s.persistent_pitch_q7 == 61u * 128u);

    /* Slide explicitly cancels a remembered portamento target. */
    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE,
                               fmt_effect_pack(FMT_FX_SLIDE_UP, 1u, 0u));
    CHECK(s.porta_valid == 0u);

    /* NOTE+porta with no prior active note starts directly at target. */
    fmt_effect_state_reset(&s);
    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_NOTE, 72u, porta);
    CHECK(s.persistent_pitch_q7 == 72u * 128u);
    CHECK(s.porta_valid == 0u);
    return 0;
}

static int test_vibrato(void)
{
    FmtEffectState s;
    uint32_t vib = fmt_effect_pack(FMT_FX_VIBRATO, 0x4Fu, 0u);
    uint8_t phase_after_row;

    CHECK(fmt_effect_vibrato_triangle(0u) == 0);
    CHECK(fmt_effect_vibrato_triangle(16u) == 32);
    CHECK(fmt_effect_vibrato_triangle(32u) == 0);
    CHECK(fmt_effect_vibrato_triangle(48u) == -32);
    CHECK(fmt_effect_vibrato_triangle(63u) == -2);

    fmt_effect_state_reset(&s);
    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_NOTE, 60u, vib);
    CHECK(s.vibrato_phase == 0u);
    CHECK(s.transient_q7 == 0);
    CHECK(fmt_effect_state_advance_phase(&s, 110250u) == 1u);
    CHECK(s.vibrato_phase == 4u);
    CHECK(s.transient_q7 == 15); /* triangle 8 * depth 15 / 8 */
    CHECK(fmt_effect_state_final_pitch_q7(&s, 128u) == 60u * 128u + 15u);
    phase_after_row = s.vibrato_phase;

    /* Consecutive VIBRATO TIE/EMPTY rows preserve phase. */
    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE, vib);
    CHECK(s.vibrato_phase == phase_after_row);
    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_NOTE, 62u, vib);
    CHECK(s.vibrato_phase == 0u);
    return 0;
}

static int test_off_inactive_and_invalid(void)
{
    FmtEffectState s;
    uint32_t arp = fmt_effect_pack(FMT_FX_ARPEGGIO, 0x37u, 0u);
    uint32_t invalid = fmt_effect_pack(0xE0u, 0xFFu, 0u);

    fmt_effect_state_reset(&s);
    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE, arp);
    CHECK(s.note_active == 0u);
    CHECK(s.transient_q7 == 0);
    CHECK(fmt_effect_state_advance_phase(&s, 110250u) == 1u);
    CHECK(s.persistent_pitch_q7 == 0u);

    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_NOTE, 60u, arp);
    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_OFF, FMT_NOTE_OFF, arp);
    CHECK(s.note_active == 0u);
    CHECK(s.porta_valid == 0u);
    CHECK(s.command == FMT_FX_NONE);
    CHECK(s.transient_q7 == 0);
    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_TIE, FMT_NOTE_TIE, arp);
    CHECK(s.note_active == 0u);

    fmt_effect_state_begin_row(&s, FMT_PLAYBACK_EVENT_NOTE, 60u, invalid);
    CHECK(s.command == FMT_FX_NONE);
    CHECK(fmt_effect_state_final_pitch_q7(&s, 128u) == 60u * 128u);
    return 0;
}

int main(void)
{
    CHECK(test_cache() == 0);
    CHECK(test_tick_grid() == 0);
    CHECK(test_arpeggio_and_fine() == 0);
    CHECK(test_slide() == 0);
    CHECK(test_portamento() == 0);
    CHECK(test_vibrato() == 0);
    CHECK(test_off_inactive_and_invalid() == 0);

    printf("FMT_EFFECT_TESTS_OK=1\n");
    printf("state_bytes=%u cache_bytes=%u ticks=6 phase_per_tick=%u event_bytes=%u commands=none,arp,slide_up,slide_down,porta,vibrato\n",
           (unsigned)sizeof(FmtEffectState),
           (unsigned)sizeof(fmt_effect_cache),
           (unsigned)FMT_EFFECT_PHASE_PER_TICK,
           (unsigned)sizeof(FmtPlaybackEvent));
    printf("arp=0XY0XY slide=persistent_q7 porta=remembered_target vibrato=triangle64 fine=additive invalid=none\n");
    return 0;
}
