/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_step_record.h"

void fmt_step_record_reset(FmtStepRecord *record)
{
    if (!record)
        return;
    record->active = 0u;
    record->gesture_latched = 0u;
    record->fx_pending = 0u;
}

int fmt_step_record_enter(FmtStepRecord *record,
                          FmtPatternInput *input,
                          const FmtTransport *transport)
{
    if (!record || !input || !transport || record->active || transport->running)
        return 0;

    fmt_pattern_input_enter_pattern(input);
    record->active = 1u;
    record->gesture_latched = 0u;
    record->fx_pending = 0u;
    return 1;
}

void fmt_step_record_exit(FmtStepRecord *record)
{
    if (!record)
        return;
    record->active = 0u;
    record->gesture_latched = 0u;
    record->fx_pending = 0u;
}

int fmt_step_record_is_active(const FmtStepRecord *record)
{
    return record && record->active != 0u;
}

void fmt_step_record_release_gesture(FmtStepRecord *record)
{
    if (record)
        record->gesture_latched = 0u;
}

int fmt_step_record_lowest_key(uint32_t down_mask, uint8_t *key_index)
{
    uint32_t key;

    if (key_index)
        *key_index = 0u;

    for (key = 0u; key < FMT_PATTERN_INPUT_KEY_COUNT; key++) {
        if (down_mask >> key & 1u) {
            if (key_index)
                *key_index = (uint8_t)key;
            return 1;
        }
    }
    return 0;
}

static int fmt_step_record_write_value(FmtStepRecord *record,
                                       FmtPatternInput *input,
                                       const FmtTransport *transport,
                                       uint8_t value)
{
    FmtStep *step;
    uint8_t global_step;

    if (!record || !input || !transport || !record->active ||
        record->gesture_latched || transport->running ||
        !fmt_pattern_input_is_pattern(input) ||
        input->page >= FMT_PATTERN_INPUT_PAGE_COUNT ||
        input->local_step >= FMT_PATTERN_INPUT_STEPS_PER_PAGE ||
        input->selected_track >= FMT_PATTERN_INPUT_TRACK_COUNT)
        return 0;

    global_step = fmt_pattern_input_global_step(input);
    step = fmt_store_step(FMT_STEP_RECORD_PATTERN,
                          input->selected_track,
                          global_step);
    if (!step)
        return 0;

    step->note = value;
    if (!fmt_pattern_input_advance_step(input))
        return 0;

    record->gesture_latched = 1u;
    return 1;
}

int fmt_step_record_write_note(FmtStepRecord *record,
                               FmtPatternInput *input,
                               const FmtTransport *transport,
                               uint32_t note)
{
    if (note > FMT_NOTE_MAX)
        return 0;
    return fmt_step_record_write_value(record, input, transport, (uint8_t)note);
}

int fmt_step_record_write_special(FmtStepRecord *record,
                                  FmtPatternInput *input,
                                  const FmtTransport *transport,
                                  uint32_t special_note)
{
    if (special_note != FMT_NOTE_EMPTY &&
        special_note != FMT_NOTE_TIE &&
        special_note != FMT_NOTE_OFF)
        return 0;

    return fmt_step_record_write_value(record, input, transport, (uint8_t)special_note);
}

int fmt_step_record_special_input(FmtStepRecord *record,
                                  FmtPatternInput *input,
                                  const FmtTransport *transport,
                                  int fx_pressed,
                                  int fx_released,
                                  int select_pressed)
{
    int wrote;

    if (!record)
        return 0;

    if (!record->active) {
        record->fx_pending = 0u;
        return 0;
    }

    if (fx_pressed)
        record->fx_pending = 1u;

    if (select_pressed) {
        if (record->fx_pending) {
            record->fx_pending = 0u;
            return fmt_step_record_write_special(record, input, transport, FMT_NOTE_OFF);
        }
        return fmt_step_record_write_special(record, input, transport, FMT_NOTE_TIE);
    }

    if (fx_released && record->fx_pending) {
        record->fx_pending = 0u;
        wrote = fmt_step_record_write_special(record, input, transport, FMT_NOTE_EMPTY);
        return wrote;
    }

    return 0;
}
