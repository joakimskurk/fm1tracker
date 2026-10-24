/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_tracker_effects.h"

volatile uint32_t fmt_tracker_effect_cache[FMT_TRACKER_EFFECT_ROWS]
    __attribute__((aligned(4)));

FMT_STATIC_ASSERT(tracker_effect_cache_is_256,
                  sizeof(fmt_tracker_effect_cache) == 256u);

static uint16_t clamp_q7(int32_t value)
{
    if (value < 0)
        return 0u;
    if (value > 16383)
        return 16383u;
    return (uint16_t)value;
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

int fmt_tracker_effect_command_valid(uint8_t command)
{
    return command <= FMT_TRACKER_EFFECT_VIBRATO;
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

    row = fmt_store_step_const(FMT_PLAYBACK_PATTERN, FMT_PLAYBACK_TRACK, step);
    if (!row)
        return 0;

    command = row->effect;
    value = row->effect_value;
    if (!fmt_tracker_effect_command_valid(command)) {
        command = FMT_TRACKER_EFFECT_NONE;
        value = 0u;
        flags = FMT_TRACKER_EFFECT_FLAG_INVALID_COMMAND;
    }

    fmt_tracker_effect_cache[step] =
        fmt_tracker_effect_pack(command, value, flags);
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
    state->persistent_pitch_q7 = 0u;
    state->portamento_target_q7 = 0u;
    state->transient_mod_q7 = 0;
    state->command = FMT_TRACKER_EFFECT_NONE;
    state->value = 0u;
    state->tick = 0u;
    state->vibrato_phase = 0u;
    state->portamento_valid = 0u;
    state->fine_value = FMT_PLAYBACK_PARAM_FINE_DEFAULT;
}

int16_t fmt_tracker_effect_triangle64(uint8_t phase)
{
    uint8_t p = (uint8_t)(phase & 63u);

    if (p < 16u)
        return (int16_t)((int16_t)p * 2);
    if (p < 32u)
        return (int16_t)((int16_t)(32u - p) * 2);
    if (p < 48u)
        return (int16_t)(-((int16_t)(p - 32u) * 2));
    return (int16_t)(-((int16_t)(64u - p) * 2));
}

static int16_t vibrato_mod(uint8_t phase, uint8_t depth)
{
    int32_t product = (int32_t)fmt_tracker_effect_triangle64(phase) *
                      (int32_t)depth;
    if (product >= 0)
        return (int16_t)(product / 8);
    return (int16_t)(-((-product) / 8));
}

static void set_arp_transient(FmtTrackerEffectState *state)
{
    uint8_t semis = 0u;

    if (state->tick == 1u || state->tick == 4u)
        semis = (uint8_t)(state->value >> 4);
    else if (state->tick == 2u || state->tick == 5u)
        semis = (uint8_t)(state->value & 0x0Fu);
    state->transient_mod_q7 = (int16_t)((uint16_t)semis * 128u);
}

void fmt_tracker_effect_apply_row(FmtTrackerEffectState *state,
                                  uint8_t event_kind,
                                  uint8_t event_note,
                                  uint32_t effect_word,
                                  uint8_t fine_value,
                                  uint8_t was_active)
{
    uint8_t old_command, command, row_active;

    if (!state)
        return;

    old_command = state->command;
    command = fmt_tracker_effect_command(effect_word);
    if (!fmt_tracker_effect_command_valid(command))
        command = FMT_TRACKER_EFFECT_NONE;

    state->command = command;
    state->value = fmt_tracker_effect_value(effect_word);
    state->fine_value = fine_value;
    state->tick = 0u;
    state->transient_mod_q7 = 0;

    if (event_kind == FMT_PLAYBACK_EVENT_OFF) {
        state->command = FMT_TRACKER_EFFECT_NONE;
        state->value = 0u;
        state->portamento_valid = 0u;
        state->vibrato_phase = 0u;
        return;
    }

    row_active = was_active;
    if (event_kind == FMT_PLAYBACK_EVENT_NOTE && event_note <= FMT_NOTE_MAX) {
        uint16_t target = (uint16_t)event_note * 128u;
        row_active = 1u;
        state->vibrato_phase = 0u;
        if (command == FMT_TRACKER_EFFECT_PORTAMENTO && was_active) {
            state->portamento_target_q7 = target;
            state->portamento_valid = state->persistent_pitch_q7 != target;
        } else {
            state->persistent_pitch_q7 = target;
            state->portamento_target_q7 = target;
            state->portamento_valid = 0u;
        }
    }

    if (command == FMT_TRACKER_EFFECT_SLIDE_UP ||
        command == FMT_TRACKER_EFFECT_SLIDE_DOWN)
        state->portamento_valid = 0u;
    else if (event_kind == FMT_PLAYBACK_EVENT_NOTE &&
             command != FMT_TRACKER_EFFECT_PORTAMENTO)
        state->portamento_valid = 0u;

    if (command != FMT_TRACKER_EFFECT_VIBRATO) {
        state->vibrato_phase = 0u;
    } else {
        if (old_command != FMT_TRACKER_EFFECT_VIBRATO ||
            event_kind == FMT_PLAYBACK_EVENT_NOTE)
            state->vibrato_phase = 0u;
        if (row_active)
            state->transient_mod_q7 =
                vibrato_mod(state->vibrato_phase,
                             (uint8_t)(state->value & 0x0Fu));
    }
}

static void move_portamento(FmtTrackerEffectState *state)
{
    uint16_t rate, center, target;

    if (!state->portamento_valid)
        return;
    rate = state->value;
    if (!rate)
        return;

    center = state->persistent_pitch_q7;
    target = state->portamento_target_q7;
    if (center < target) {
        uint32_t next = (uint32_t)center + rate;
        if (next >= target) {
            state->persistent_pitch_q7 = target;
            state->portamento_valid = 0u;
        } else {
            state->persistent_pitch_q7 = (uint16_t)next;
        }
    } else if (center > target) {
        if ((uint32_t)center <= (uint32_t)target + rate) {
            state->persistent_pitch_q7 = target;
            state->portamento_valid = 0u;
        } else {
            state->persistent_pitch_q7 = (uint16_t)(center - rate);
        }
    } else {
        state->portamento_valid = 0u;
    }
}

int fmt_tracker_effect_advance(FmtTrackerEffectState *state,
                               uint32_t transport_phase,
                               uint8_t note_active)
{
    uint32_t threshold;

    if (!state || state->tick >= 5u)
        return 0;
    threshold = (uint32_t)(state->tick + 1u) * FMT_TRACKER_EFFECT_TICK_PHASE;
    if (transport_phase < threshold)
        return 0;

    state->tick++;
    state->transient_mod_q7 = 0;

    if (state->command == FMT_TRACKER_EFFECT_ARPEGGIO) {
        if (note_active)
            set_arp_transient(state);
    } else if (state->command == FMT_TRACKER_EFFECT_SLIDE_UP) {
        if (note_active)
            state->persistent_pitch_q7 =
                clamp_q7((int32_t)state->persistent_pitch_q7 + state->value);
    } else if (state->command == FMT_TRACKER_EFFECT_SLIDE_DOWN) {
        if (note_active)
            state->persistent_pitch_q7 =
                clamp_q7((int32_t)state->persistent_pitch_q7 - state->value);
    } else if (state->command == FMT_TRACKER_EFFECT_PORTAMENTO) {
        if (note_active)
            move_portamento(state);
    } else if (state->command == FMT_TRACKER_EFFECT_VIBRATO) {
        state->vibrato_phase =
            (uint8_t)((state->vibrato_phase + (state->value >> 4)) & 63u);
        if (note_active)
            state->transient_mod_q7 =
                vibrato_mod(state->vibrato_phase,
                             (uint8_t)(state->value & 0x0Fu));
    }

    return 1;
}

uint16_t fmt_tracker_effect_final_pitch_q7(const FmtTrackerEffectState *state)
{
    int32_t pitch;

    if (!state)
        return 0u;
    pitch = (int32_t)state->persistent_pitch_q7 +
            ((int32_t)state->fine_value - 128) +
            (int32_t)state->transient_mod_q7;
    return clamp_q7(pitch);
}
