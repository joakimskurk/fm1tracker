/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_tracker_effect.h"

volatile uint32_t fmt_tracker_effect_cache[FMT_TRACKER_EFFECT_ROWS]
    __attribute__((aligned(4)));

FMT_STATIC_ASSERT(tracker_effect_cache_is_256,
                  sizeof(fmt_tracker_effect_cache) == 256u);

static int32_t clamp_pitch_q7(int32_t pitch)
{
    if (pitch < 0)
        return 0;
    if (pitch > 16383)
        return 16383;
    return pitch;
}

uint8_t fmt_tracker_effect_command_valid(uint8_t command)
{
    return command <= FMT_TRACKER_EFFECT_NOTE_CUT;
}

uint32_t fmt_tracker_effect_pack(uint8_t command,
                                 uint8_t value,
                                 uint8_t flags)
{
    return (uint32_t)command |
           ((uint32_t)value << 8) |
           ((uint32_t)flags << 16);
}

uint8_t fmt_tracker_effect_command(uint32_t word)
{
    return (uint8_t)(word & 0xFFu);
}

uint8_t fmt_tracker_effect_value(uint32_t word)
{
    return (uint8_t)((word >> 8) & 0xFFu);
}

uint8_t fmt_tracker_effect_flags(uint32_t word)
{
    return (uint8_t)((word >> 16) & 0xFFu);
}

uint32_t fmt_tracker_effect_default_word(void)
{
    return fmt_tracker_effect_pack(FMT_TRACKER_EFFECT_NONE, 0u,
                                   FMT_TRACKER_EFFECT_FLAG_NONE);
}

void fmt_tracker_effect_cache_reset(void)
{
    uint32_t step;
    uint32_t word = fmt_tracker_effect_default_word();

    for (step = 0u; step < FMT_TRACKER_EFFECT_ROWS; step++)
        fmt_tracker_effect_cache[step] = word;
}

int fmt_tracker_effect_cache_refresh_step(uint32_t step)
{
    const FmtStep *row;
    uint8_t command, value, flags = FMT_TRACKER_EFFECT_FLAG_NONE;

    if (step >= FMT_TRACKER_EFFECT_ROWS)
        return 0;

    row = fmt_store_step_const(FMT_PLAYBACK_PATTERN,
                               FMT_PLAYBACK_TRACK,
                               step);
    if (!row) {
        fmt_tracker_effect_cache[step] = fmt_tracker_effect_default_word();
        return 1;
    }

    command = row->effect;
    value = row->effect_value;
    if (!fmt_tracker_effect_command_valid(command)) {
        command = FMT_TRACKER_EFFECT_NONE;
        value = 0u;
        flags = FMT_TRACKER_EFFECT_FLAG_INVALID_COMMAND;
    }

    fmt_tracker_effect_cache[step] = fmt_tracker_effect_pack(command, value, flags);
    return 1;
}

void fmt_tracker_effect_cache_rebuild(void)
{
    uint32_t step;

    for (step = 0u; step < FMT_TRACKER_EFFECT_ROWS; step++)
        (void)fmt_tracker_effect_cache_refresh_step(step);
}

uint32_t fmt_tracker_effect_cache_read(uint32_t step)
{
    if (step >= FMT_TRACKER_EFFECT_ROWS)
        return fmt_tracker_effect_default_word();
    return fmt_tracker_effect_cache[step];
}

void fmt_tracker_effect_state_reset(FmtTrackerEffectState *state)
{
    if (!state)
        return;
    state->pitch_q7 = 0u;
    state->porta_target_q7 = 0u;
    state->transient_q7 = 0;
    state->command = FMT_TRACKER_EFFECT_NONE;
    state->value = 0u;
    state->tick = 0u;
    state->vibrato_phase = 0u;
    state->porta_valid = 0u;
    state->pitch_valid = 0u;
}

void fmt_tracker_effect_apply_delayed_note(FmtTrackerEffectState *state,
                                           uint8_t note)
{
    if (!state || note > FMT_NOTE_MAX)
        return;

    state->pitch_q7 = (uint16_t)((uint16_t)note << 7);
    state->pitch_valid = 1u;
    state->porta_valid = 0u;
    state->transient_q7 = 0;
    state->vibrato_phase = 0u;
}

void fmt_tracker_effect_apply_cut(FmtTrackerEffectState *state)
{
    if (!state)
        return;

    state->pitch_valid = 0u;
    state->porta_valid = 0u;
    state->transient_q7 = 0;
    state->vibrato_phase = 0u;
}

int16_t fmt_tracker_effect_vibrato_triangle(uint8_t phase)
{
    uint8_t p = phase & 63u;

    if (p < 16u)
        return (int16_t)(p * 2u);
    if (p < 32u)
        return (int16_t)((32u - p) * 2u);
    if (p < 48u)
        return (int16_t)(-((int16_t)(p - 32u) * 2));
    return (int16_t)(-((int16_t)(64u - p) * 2));
}

static int16_t vibrato_mod_q7(uint8_t phase, uint8_t depth)
{
    int32_t product = (int32_t)fmt_tracker_effect_vibrato_triangle(phase) *
                      (int32_t)depth;

    if (product >= 0)
        return (int16_t)(product >> 3);
    return (int16_t)(-((-product) >> 3));
}

static int16_t arpeggio_mod_q7(uint8_t tick, uint8_t value)
{
    uint8_t offset;

    switch (tick % 3u) {
    case 1u:
        offset = (uint8_t)(value >> 4);
        break;
    case 2u:
        offset = (uint8_t)(value & 0x0Fu);
        break;
    default:
        offset = 0u;
        break;
    }
    return (int16_t)((uint16_t)offset << 7);
}

static void portamento_step(FmtTrackerEffectState *state)
{
    uint16_t amount, distance;

    if (!state->porta_valid || !state->value)
        return;

    amount = state->value;
    if (state->pitch_q7 < state->porta_target_q7) {
        distance = (uint16_t)(state->porta_target_q7 - state->pitch_q7);
        if (amount >= distance) {
            state->pitch_q7 = state->porta_target_q7;
            state->porta_valid = 0u;
        } else {
            state->pitch_q7 = (uint16_t)(state->pitch_q7 + amount);
        }
    } else if (state->pitch_q7 > state->porta_target_q7) {
        distance = (uint16_t)(state->pitch_q7 - state->porta_target_q7);
        if (amount >= distance) {
            state->pitch_q7 = state->porta_target_q7;
            state->porta_valid = 0u;
        } else {
            state->pitch_q7 = (uint16_t)(state->pitch_q7 - amount);
        }
    } else {
        state->porta_valid = 0u;
    }
}

void fmt_tracker_effect_begin_row(FmtTrackerEffectState *state,
                                  uint8_t event_kind,
                                  uint8_t event_note,
                                  uint32_t effect_word)
{
    uint8_t previous_command;
    uint8_t command;
    uint8_t value;

    if (!state)
        return;

    previous_command = state->command;
    command = fmt_tracker_effect_command(effect_word);
    value = fmt_tracker_effect_value(effect_word);
    if (!fmt_tracker_effect_command_valid(command)) {
        command = FMT_TRACKER_EFFECT_NONE;
        value = 0u;
    }

    state->command = command;
    state->value = value;
    state->tick = 0u;
    state->transient_q7 = 0;

    if (event_kind == FMT_PLAYBACK_EVENT_OFF) {
        state->pitch_valid = 0u;
        state->porta_valid = 0u;
        state->vibrato_phase = 0u;
        state->command = FMT_TRACKER_EFFECT_NONE;
        state->value = 0u;
        return;
    }

    if (event_kind == FMT_PLAYBACK_EVENT_NOTE && event_note <= FMT_NOTE_MAX) {
        uint16_t note_pitch = (uint16_t)((uint16_t)event_note << 7);

        if (command == FMT_TRACKER_EFFECT_PORTAMENTO && state->pitch_valid) {
            state->porta_target_q7 = note_pitch;
            state->porta_valid = state->pitch_q7 != note_pitch;
        } else {
            state->pitch_q7 = note_pitch;
            state->pitch_valid = 1u;
            state->porta_valid = 0u;
        }
        state->vibrato_phase = 0u;
    } else if (event_kind != FMT_PLAYBACK_EVENT_TIE &&
               event_kind != FMT_PLAYBACK_EVENT_NONE) {
        return;
    }

    if (command == FMT_TRACKER_EFFECT_SLIDE_UP ||
        command == FMT_TRACKER_EFFECT_SLIDE_DOWN) {
        state->porta_valid = 0u;
    } else if (event_kind == FMT_PLAYBACK_EVENT_NOTE &&
               command != FMT_TRACKER_EFFECT_PORTAMENTO) {
        state->porta_valid = 0u;
    }

    if (command == FMT_TRACKER_EFFECT_VIBRATO) {
        if (event_kind == FMT_PLAYBACK_EVENT_NOTE ||
            previous_command != FMT_TRACKER_EFFECT_VIBRATO)
            state->vibrato_phase = 0u;
        if (state->pitch_valid)
            state->transient_q7 = vibrato_mod_q7(state->vibrato_phase,
                                                  (uint8_t)(value & 0x0Fu));
    } else {
        state->vibrato_phase = 0u;
        if (command == FMT_TRACKER_EFFECT_ARPEGGIO && state->pitch_valid)
            state->transient_q7 = arpeggio_mod_q7(0u, value);
    }
}

uint8_t fmt_tracker_effect_tick_due(const FmtTrackerEffectState *state,
                                    uint32_t transport_phase)
{
    uint32_t threshold;

    if (!state || state->tick >= (FMT_TRACKER_EFFECT_TICKS_PER_ROW - 1u))
        return 0u;
    threshold = ((uint32_t)state->tick + 1u) * FMT_TRACKER_EFFECT_TICK_PHASE;
    return transport_phase >= threshold;
}

uint8_t fmt_tracker_effect_advance_tick(FmtTrackerEffectState *state)
{
    int32_t pitch;

    if (!state || state->tick >= (FMT_TRACKER_EFFECT_TICKS_PER_ROW - 1u))
        return 0u;

    state->tick++;
    state->transient_q7 = 0;

    if (!state->pitch_valid)
        return 1u;

    switch (state->command) {
    case FMT_TRACKER_EFFECT_ARPEGGIO:
        state->transient_q7 = arpeggio_mod_q7(state->tick, state->value);
        break;

    case FMT_TRACKER_EFFECT_SLIDE_UP:
        pitch = (int32_t)state->pitch_q7 + (int32_t)state->value;
        state->pitch_q7 = (uint16_t)clamp_pitch_q7(pitch);
        break;

    case FMT_TRACKER_EFFECT_SLIDE_DOWN:
        pitch = (int32_t)state->pitch_q7 - (int32_t)state->value;
        state->pitch_q7 = (uint16_t)clamp_pitch_q7(pitch);
        break;

    case FMT_TRACKER_EFFECT_PORTAMENTO:
        portamento_step(state);
        break;

    case FMT_TRACKER_EFFECT_VIBRATO:
        state->vibrato_phase =
            (uint8_t)((state->vibrato_phase + (state->value >> 4)) & 63u);
        state->transient_q7 =
            vibrato_mod_q7(state->vibrato_phase,
                            (uint8_t)(state->value & 0x0Fu));
        break;

    default:
        break;
    }

    return 1u;
}

uint16_t fmt_tracker_effect_final_pitch_q7(const FmtTrackerEffectState *state,
                                           uint8_t fine_pitch)
{
    int32_t pitch;

    if (!state || !state->pitch_valid)
        return 0u;

    pitch = (int32_t)state->pitch_q7 +
            ((int32_t)fine_pitch - 128) +
            (int32_t)state->transient_q7;
    return (uint16_t)clamp_pitch_q7(pitch);
}
