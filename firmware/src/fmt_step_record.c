/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_step_record.h"

void fmt_step_record_reset(FmtStepRecord *record)
{
    if (!record)
        return;
    record->active = 0u;
    record->gesture_latched = 0u;
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
    return 1;
}

void fmt_step_record_exit(FmtStepRecord *record)
{
    if (!record)
        return;
    record->active = 0u;
    record->gesture_latched = 0u;
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

int fmt_step_record_write_note(FmtStepRecord *record,
                               FmtPatternInput *input,
                               const FmtTransport *transport,
                               uint32_t note)
{
    FmtStep *step;
    uint8_t global_step;

    if (!record || !input || !transport || !record->active ||
        record->gesture_latched || transport->running ||
        !fmt_pattern_input_is_pattern(input) || note > FMT_NOTE_MAX ||
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

    step->note = (uint8_t)note;
    if (!fmt_pattern_input_advance_step(input))
        return 0;

    record->gesture_latched = 1u;
    return 1;
}
