/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "fmt_tracker_effect.h"
#include "fmt_tracker_event.h"
#include "fmt_transport.h"

static uint32_t mix(uint32_t h, uint32_t v)
{
    h ^= v + 0x9E3779B9u + (h << 6) + (h >> 2);
    return h;
}

static uint32_t word(uint8_t command, uint8_t value)
{
    return fmt_tracker_effect_pack(command, value, 0u);
}

static void test_all_effect_values_bounded(uint32_t *hash)
{
    uint32_t command, value, tick;

    for (command = FMT_TRACKER_EFFECT_ARPEGGIO;
         command <= FMT_TRACKER_EFFECT_VIBRATO; command++) {
        for (value = 0u; value <= 255u; value++) {
            FmtTrackerEffectState state;
            uint16_t previous;

            fmt_tracker_effect_state_reset(&state);
            fmt_tracker_effect_begin_row(&state,
                                         FMT_PLAYBACK_EVENT_NOTE,
                                         60u,
                                         word((uint8_t)command, (uint8_t)value));
            assert(state.tick == 0u);
            assert(state.pitch_valid == 1u);
            previous = state.pitch_q7;
            assert(fmt_tracker_effect_final_pitch_q7(&state, 0u) <= 16383u);
            assert(fmt_tracker_effect_final_pitch_q7(&state, 255u) <= 16383u);

            for (tick = 1u; tick <= 5u; tick++) {
                assert(fmt_tracker_effect_advance_tick(&state));
                assert(state.tick == tick);
                assert(state.pitch_q7 <= 16383u);
                assert(fmt_tracker_effect_final_pitch_q7(&state, 0u) <= 16383u);
                assert(fmt_tracker_effect_final_pitch_q7(&state, 255u) <= 16383u);
                previous = state.pitch_q7;
                *hash = mix(*hash, (uint32_t)state.pitch_q7);
                *hash = mix(*hash, (uint16_t)state.transient_q7);
            }
            assert(!fmt_tracker_effect_advance_tick(&state));
            assert(state.tick == 5u);
            assert(state.pitch_q7 == previous);
        }
    }
}

static void test_event_value_domain(uint32_t *hash)
{
    uint32_t command, value, tick;

    for (command = FMT_TRACKER_EFFECT_RETRIG;
         command <= FMT_TRACKER_EFFECT_NOTE_CUT; command++) {
        for (value = 0u; value <= 255u; value++) {
            FmtTrackerRowEventState state;
            uint8_t kind, note, out;
            uint32_t actions = 0u;

            fmt_tracker_row_event_begin(&state,
                                        FMT_PLAYBACK_EVENT_NOTE,
                                        64u,
                                        word((uint8_t)command, (uint8_t)value),
                                        &kind,
                                        &note);

            for (tick = 0u; tick <= 6u; tick++) {
                uint8_t action = fmt_tracker_row_event_action(&state,
                                                              (uint8_t)tick,
                                                              1u,
                                                              &out);
                if (action != FMT_TRACKER_ROW_ACTION_NONE)
                    actions++;
                /* D033 has no tick 6 action under any command/value. */
                if (tick == 6u)
                    assert(action == FMT_TRACKER_ROW_ACTION_NONE);
                /* Values >5 never wrap modulo six into work. */
                if (value > 5u)
                    assert(action == FMT_TRACKER_ROW_ACTION_NONE);
                *hash = mix(*hash, ((uint32_t)action << 8) | out);
            }

            if (command == FMT_TRACKER_EFFECT_NOTE_DELAY && value >= 1u && value <= 5u)
                assert(actions == 1u);
            if (command == FMT_TRACKER_EFFECT_NOTE_CUT && value <= 5u)
                assert(actions == 1u);
            if (command == FMT_TRACKER_EFFECT_RETRIG && value >= 1u && value <= 5u)
                assert(actions == 5u / value);
        }
    }
}

static void test_long_row_tick_order_and_wrap(uint32_t *hash)
{
    static const uint8_t commands[8] = {
        FMT_TRACKER_EFFECT_ARPEGGIO,
        FMT_TRACKER_EFFECT_SLIDE_UP,
        FMT_TRACKER_EFFECT_SLIDE_DOWN,
        FMT_TRACKER_EFFECT_PORTAMENTO,
        FMT_TRACKER_EFFECT_VIBRATO,
        FMT_TRACKER_EFFECT_RETRIG,
        FMT_TRACKER_EFFECT_NOTE_DELAY,
        FMT_TRACKER_EFFECT_NOTE_CUT
    };
    FmtTransport transport;
    FmtTrackerEffectState effect;
    FmtTrackerRowEventState row_event;
    uint32_t rows = 0u, frames = 0u, effect_ticks = 0u, row_actions = 0u;
    uint8_t immediate_kind, immediate_note;

    fmt_transport_reset(&transport);
    fmt_tracker_effect_state_reset(&effect);
    fmt_tracker_row_event_reset(&row_event);
    fmt_transport_start(&transport);

    /* 4096 exact row transitions = 64 full pattern wraps. Every new row rotates
     * through all D032/D033 command classes and boundary/no-op values. */
    while (rows < 4096u) {
        uint8_t command = commands[transport.step & 7u];
        static const uint8_t values[8] = {0u, 1u, 5u, 6u, 255u, 2u, 3u, 5u};
        uint8_t value = values[(rows + transport.step) & 7u];
        uint8_t action_note;
        uint8_t action;
        uint8_t kind = (rows & 3u) == 0u ? FMT_PLAYBACK_EVENT_NOTE
                     : (rows & 3u) == 1u ? FMT_PLAYBACK_EVENT_TIE
                     : (rows & 3u) == 2u ? FMT_PLAYBACK_EVENT_NONE
                                         : FMT_PLAYBACK_EVENT_OFF;
        uint8_t note = kind == FMT_PLAYBACK_EVENT_NOTE ? (uint8_t)(48u + (rows % 36u))
                     : kind == FMT_PLAYBACK_EVENT_TIE ? FMT_NOTE_TIE
                     : kind == FMT_PLAYBACK_EVENT_OFF ? FMT_NOTE_OFF
                                                     : FMT_NOTE_EMPTY;
        uint32_t packed = word(command, value);

        fmt_tracker_row_event_begin(&row_event, kind, note, packed,
                                    &immediate_kind, &immediate_note);
        fmt_tracker_effect_begin_row(&effect, immediate_kind, immediate_note, packed);
        action = fmt_tracker_row_event_action(&row_event, 0u,
                                              effect.pitch_valid,
                                              &action_note);
        if (action != FMT_TRACKER_ROW_ACTION_NONE)
            row_actions++;

        for (;;) {
            uint8_t boundary = fmt_transport_tick_frame(&transport);
            frames++;
            if (boundary) {
                rows++;
                *hash = mix(*hash, transport.step);
                *hash = mix(*hash, transport.phase);
                /* Boundary must win before a hypothetical tick 6. */
                assert(effect.tick <= 5u);
                break;
            }
            if (fmt_tracker_effect_tick_due(&effect, transport.phase)) {
                assert(fmt_tracker_effect_advance_tick(&effect));
                effect_ticks++;
                assert(effect.tick >= 1u && effect.tick <= 5u);
                action = fmt_tracker_row_event_action(&row_event,
                                                      effect.tick,
                                                      effect.pitch_valid,
                                                      &action_note);
                if (action != FMT_TRACKER_ROW_ACTION_NONE)
                    row_actions++;
                *hash = mix(*hash, effect.tick);
                *hash = mix(*hash, effect.pitch_q7);
            }
        }
    }

    assert(transport.step == 0u);
    assert(rows == 4096u);
    assert(effect_ticks == rows * 5u);
    assert(frames == 4096u * 5512u + 2048u); /* 5512.5 frames/row exactly. */
    assert(transport.phase == 0u);

    printf("WP3_10_LONG_TICK_MATRIX_OK=1 rows=%u wraps=64 frames=%u effect_ticks=%u actions=%u final_step=%u final_phase=%u\n",
           (unsigned)rows, (unsigned)frames, (unsigned)effect_ticks,
           (unsigned)row_actions, (unsigned)transport.step, (unsigned)transport.phase);
}

int main(void)
{
    uint32_t hash = 0xD0362026u;

    assert(FMT_TRANSPORT_PHASE_PER_STEP == 661500u);
    assert(FMT_TRACKER_EFFECT_TICK_PHASE == 110250u);
    assert(FMT_TRACKER_EFFECT_TICKS_PER_ROW == 6u);

    test_all_effect_values_bounded(&hash);
    test_event_value_domain(&hash);
    test_long_row_tick_order_and_wrap(&hash);

    printf("WP3_10_TIMING_MATRIX_OK=1 d032_commands=5 values_each=256 d033_commands=3 values_each=256 no_tick6=1 hash=%08x\n",
           (unsigned)hash);
    return 0;
}
