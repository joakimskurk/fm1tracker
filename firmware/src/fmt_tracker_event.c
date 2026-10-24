/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_tracker_event.h"

static uint8_t event_command(uint8_t command)
{
    return command == FMT_TRACKER_EFFECT_RETRIG ||
           command == FMT_TRACKER_EFFECT_NOTE_DELAY ||
           command == FMT_TRACKER_EFFECT_NOTE_CUT;
}

void fmt_tracker_row_event_reset(FmtTrackerRowEventState *state)
{
    if (!state)
        return;
    state->command = FMT_TRACKER_EFFECT_NONE;
    state->value = 0u;
    state->pending_note = FMT_NOTE_EMPTY;
    state->pending_valid = 0u;
}

void fmt_tracker_row_event_begin(FmtTrackerRowEventState *state,
                                 uint8_t event_kind,
                                 uint8_t event_note,
                                 uint32_t effect_word,
                                 uint8_t *immediate_kind,
                                 uint8_t *immediate_note)
{
    uint8_t command;
    uint8_t value;

    if (immediate_kind)
        *immediate_kind = event_kind;
    if (immediate_note)
        *immediate_note = event_note;
    if (!state)
        return;

    fmt_tracker_row_event_reset(state);
    command = fmt_tracker_effect_command(effect_word);
    value = fmt_tracker_effect_value(effect_word);
    if (!event_command(command))
        return;

    state->command = command;
    state->value = value;

    if (command == FMT_TRACKER_EFFECT_NOTE_DELAY &&
        value >= 1u && value <= 5u &&
        event_kind == FMT_PLAYBACK_EVENT_NOTE &&
        event_note <= FMT_NOTE_MAX) {
        state->pending_note = event_note;
        state->pending_valid = 1u;
        if (immediate_kind)
            *immediate_kind = FMT_PLAYBACK_EVENT_NONE;
        if (immediate_note)
            *immediate_note = FMT_NOTE_EMPTY;
    }
}

uint8_t fmt_tracker_row_event_action(FmtTrackerRowEventState *state,
                                     uint8_t tick,
                                     uint8_t logical_active,
                                     uint8_t *note_out)
{
    if (note_out)
        *note_out = FMT_NOTE_EMPTY;
    if (!state || tick >= FMT_TRACKER_EFFECT_TICKS_PER_ROW)
        return FMT_TRACKER_ROW_ACTION_NONE;

    switch (state->command) {
    case FMT_TRACKER_EFFECT_NOTE_DELAY:
        if (state->value <= 5u && state->pending_valid &&
            tick == state->value) {
            if (note_out)
                *note_out = state->pending_note;
            state->pending_valid = 0u;
            return FMT_TRACKER_ROW_ACTION_DELAYED_NOTE;
        }
        break;

    case FMT_TRACKER_EFFECT_NOTE_CUT:
        if (state->value <= 5u && tick == state->value && logical_active)
            return FMT_TRACKER_ROW_ACTION_CUT;
        break;

    case FMT_TRACKER_EFFECT_RETRIG:
        if (tick > 0u && state->value >= 1u && state->value <= 5u &&
            logical_active && (tick % state->value) == 0u)
            return FMT_TRACKER_ROW_ACTION_RETRIG;
        break;

    default:
        break;
    }

    return FMT_TRACKER_ROW_ACTION_NONE;
}
