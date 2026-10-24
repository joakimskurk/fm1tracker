/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_effect.h"
#include "fmt_store.h"

volatile uint32_t fmt_effect_cache[FMT_EFFECT_ROWS]
    __attribute__((aligned(4)));

FMT_STATIC_ASSERT(effect_cache_is_256, sizeof(fmt_effect_cache) == 256u);

static uint16_t fmt_effect_clamp_pitch(int32_t pitch)
{
    if (pitch < 0)
        return 0u;
    if (pitch > 16383)
        return 16383u;
    return (uint16_t)pitch;
}

uint8_t fmt_effect_command_valid(uint8_t command)
{
    return command <= (uint8_t)FMT_FX_VIBRATO;
}

uint32_t fmt_effect_pack(uint8_t command, uint8_t value, uint8_t flags)
{
    return (uint32_t)command |
           ((uint32_t)value << 8) |
           ((uint32_t)flags << 16);
}

uint32_t fmt_effect_default_word(void)
{
    return fmt_effect_pack(FMT_FX_NONE, 0u, FMT_EFFECT_FLAG_NONE);
}

uint8_t fmt_effect_command(uint32_t word)
{
    return (uint8_t)(word & 0xFFu);
}

uint8_t fmt_effect_value(uint32_t word)
{
    return (uint8_t)((word >> 8) & 0xFFu);
}

uint8_t fmt_effect_flags(uint32_t word)
{
    return (uint8_t)((word >> 16) & 0xFFu);
}

void fmt_effect_cache_reset(void)
{
    uint32_t step;
    const uint32_t word = fmt_effect_default_word();

    for (step = 0u; step < FMT_EFFECT_ROWS; step++)
        fmt_effect_cache[step] = word;
}

int fmt_effect_cache_refresh_step(uint32_t step)
{
    const FmtStep *row;
    uint8_t command, value, flags = FMT_EFFECT_FLAG_NONE;

    if (step >= FMT_EFFECT_ROWS)
        return 0;

    row = fmt_store_step_const(FMT_PLAYBACK_PATTERN, FMT_PLAYBACK_TRACK, step);
    if (!row) {
        fmt_effect_cache[step] = fmt_effect_default_word();
        return 1;
    }

    command = row->effect;
    value = row->effect_value;
    if (!fmt_effect_command_valid(command)) {
        command = FMT_FX_NONE;
        value = 0u;
        flags = FMT_EFFECT_FLAG_INVALID_COMMAND;
    }

    if (command == FMT_FX_NONE)
        value = 0u;

    fmt_effect_cache[step] = fmt_effect_pack(command, value, flags);
    return 1;
}

void fmt_effect_cache_rebuild(void)
{
    uint32_t step;

    for (step = 0u; step < FMT_EFFECT_ROWS; step++)
        (void)fmt_effect_cache_refresh_step(step);
}

uint32_t fmt_effect_cache_read(uint32_t step)
{
    if (step >= FMT_EFFECT_ROWS)
        return fmt_effect_default_word();
    return fmt_effect_cache[step];
}

uint8_t fmt_effect_tick_for_phase(uint32_t phase)
{
    if (phase >= 5u * FMT_EFFECT_PHASE_PER_TICK)
        return 5u;
    if (phase >= 4u * FMT_EFFECT_PHASE_PER_TICK)
        return 4u;
    if (phase >= 3u * FMT_EFFECT_PHASE_PER_TICK)
        return 3u;
    if (phase >= 2u * FMT_EFFECT_PHASE_PER_TICK)
        return 2u;
    if (phase >= FMT_EFFECT_PHASE_PER_TICK)
        return 1u;
    return 0u;
}

int16_t fmt_effect_vibrato_triangle(uint8_t phase)
{
    uint8_t p = (uint8_t)(phase & 63u);

    if (p <= 16u)
        return (int16_t)(2 * (int16_t)p);
    if (p <= 32u)
        return (int16_t)(64 - 2 * (int16_t)p);
    if (p <= 48u)
        return (int16_t)(-2 * ((int16_t)p - 32));
    return (int16_t)(-32 + 2 * ((int16_t)p - 48));
}

static void fmt_effect_update_transient(FmtEffectState *state)
{
    uint8_t x, y;
    int16_t tri;

    state->transient_q7 = 0;
    if (!state->note_active)
        return;

    if (state->command == FMT_FX_ARPEGGIO) {
        x = (uint8_t)(state->value >> 4);
        y = (uint8_t)(state->value & 0x0Fu);
        if (state->tick == 1u || state->tick == 4u)
            state->transient_q7 = (int16_t)((uint16_t)x * 128u);
        else if (state->tick == 2u || state->tick == 5u)
            state->transient_q7 = (int16_t)((uint16_t)y * 128u);
    } else if (state->command == FMT_FX_VIBRATO) {
        y = (uint8_t)(state->value & 0x0Fu);
        tri = fmt_effect_vibrato_triangle(state->vibrato_phase);
        state->transient_q7 = (int16_t)((tri * (int16_t)y) / 8);
    }
}

void fmt_effect_state_reset(FmtEffectState *state)
{
    if (!state)
        return;
    state->persistent_pitch_q7 = 0u;
    state->porta_target_q7 = 0u;
    state->transient_q7 = 0;
    state->command = FMT_FX_NONE;
    state->value = 0u;
    state->tick = 0u;
    state->vibrato_phase = 0u;
    state->porta_valid = 0u;
    state->note_active = 0u;
}

void fmt_effect_state_begin_row(FmtEffectState *state,
                                uint8_t event_kind,
                                uint8_t event_note,
                                uint32_t effect_word)
{
    uint8_t command, value, was_active, previous_command;

    if (!state)
        return;

    command = fmt_effect_command(effect_word);
    value = fmt_effect_value(effect_word);
    if (!fmt_effect_command_valid(command))
        command = FMT_FX_NONE;

    was_active = state->note_active;
    previous_command = state->command;
    state->command = command;
    state->value = value;
    state->tick = 0u;
    state->transient_q7 = 0;

    if (event_kind == FMT_PLAYBACK_EVENT_OFF) {
        state->note_active = 0u;
        state->porta_valid = 0u;
        state->vibrato_phase = 0u;
        state->command = FMT_FX_NONE;
        state->value = 0u;
        return;
    }

    if (event_kind == FMT_PLAYBACK_EVENT_NOTE && event_note <= FMT_NOTE_MAX) {
        uint16_t note_pitch = (uint16_t)event_note * 128u;

        state->note_active = 1u;
        if (command == FMT_FX_PORTAMENTO && was_active) {
            state->porta_target_q7 = note_pitch;
            state->porta_valid = 1u;
        } else {
            state->persistent_pitch_q7 = note_pitch;
            state->porta_valid = 0u;
        }
    } else if (event_kind != FMT_PLAYBACK_EVENT_TIE &&
               event_kind != FMT_PLAYBACK_EVENT_NONE) {
        state->command = FMT_FX_NONE;
        state->value = 0u;
        state->transient_q7 = 0;
        return;
    }

    if (command == FMT_FX_SLIDE_UP || command == FMT_FX_SLIDE_DOWN)
        state->porta_valid = 0u;

    if (command == FMT_FX_VIBRATO) {
        if (previous_command != FMT_FX_VIBRATO ||
            event_kind == FMT_PLAYBACK_EVENT_NOTE)
            state->vibrato_phase = 0u;
    } else {
        state->vibrato_phase = 0u;
    }

    fmt_effect_update_transient(state);
}

static void fmt_effect_apply_tick(FmtEffectState *state)
{
    uint16_t delta, distance;
    uint8_t speed;

    if (!state || !state->note_active)
        return;

    if (state->command == FMT_FX_SLIDE_UP) {
        state->persistent_pitch_q7 =
            fmt_effect_clamp_pitch((int32_t)state->persistent_pitch_q7 + state->value);
    } else if (state->command == FMT_FX_SLIDE_DOWN) {
        state->persistent_pitch_q7 =
            fmt_effect_clamp_pitch((int32_t)state->persistent_pitch_q7 - state->value);
    } else if (state->command == FMT_FX_PORTAMENTO && state->porta_valid) {
        speed = state->value;
        if (speed != 0u) {
            if (state->persistent_pitch_q7 < state->porta_target_q7) {
                distance = (uint16_t)(state->porta_target_q7 - state->persistent_pitch_q7);
                delta = distance < speed ? distance : speed;
                state->persistent_pitch_q7 = (uint16_t)(state->persistent_pitch_q7 + delta);
            } else if (state->persistent_pitch_q7 > state->porta_target_q7) {
                distance = (uint16_t)(state->persistent_pitch_q7 - state->porta_target_q7);
                delta = distance < speed ? distance : speed;
                state->persistent_pitch_q7 = (uint16_t)(state->persistent_pitch_q7 - delta);
            }
            if (state->persistent_pitch_q7 == state->porta_target_q7)
                state->porta_valid = 0u;
        }
    } else if (state->command == FMT_FX_VIBRATO) {
        state->vibrato_phase =
            (uint8_t)((state->vibrato_phase + (state->value >> 4)) & 63u);
    }

    fmt_effect_update_transient(state);
}

uint8_t fmt_effect_state_advance_phase(FmtEffectState *state, uint32_t phase)
{
    uint8_t next_tick;

    if (!state)
        return 0u;

    next_tick = fmt_effect_tick_for_phase(phase);
    if (next_tick <= state->tick)
        return 0u;

    /* D032 and D020 guarantee at most one transition per rendered frame.
     * If a caller jumps farther, remain bounded and apply one transition only. */
    state->tick = next_tick;
    fmt_effect_apply_tick(state);
    return 1u;
}

uint16_t fmt_effect_state_final_pitch_q7(const FmtEffectState *state,
                                         uint8_t fine_pitch)
{
    int32_t pitch;

    if (!state)
        return 0u;

    pitch = (int32_t)state->persistent_pitch_q7 +
            ((int32_t)fine_pitch - 128) +
            (int32_t)state->transient_q7;
    return fmt_effect_clamp_pitch(pitch);
}
