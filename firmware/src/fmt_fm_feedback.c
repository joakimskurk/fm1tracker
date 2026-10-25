/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_fm_feedback.h"

void fmt_fm_feedback_reset(FmtFmFeedbackState *state)
{
    if (!state)
        return;
    state->previous_op4_q15 = 0;
}

void fmt_fm_feedback_retrigger(FmtFmFeedbackState *state)
{
    fmt_fm_feedback_reset(state);
}

uint8_t fmt_fm_feedback_sanitize(uint32_t control)
{
    if (control > FMT_FM_FEEDBACK_CONTROL_MAX)
        return FMT_FM_FEEDBACK_CONTROL_MAX;
    return (uint8_t)control;
}

uint16_t fmt_fm_feedback_depth_q14(uint32_t control)
{
    uint16_t v = fmt_fm_feedback_sanitize(control);
    return (uint16_t)((v << 6) | (v >> 2));
}

int32_t fmt_fm_feedback_phase_modulation(const FmtFmFeedbackState *state,
                                         uint32_t feedback_control)
{
    int32_t previous;
    int32_t depth;

    if (!state)
        return 0;

    previous = (int32_t)state->previous_op4_q15;
    depth = (int32_t)fmt_fm_feedback_depth_q14(feedback_control);
    return previous * depth * 4;
}

int32_t fmt_fm_feedback_render_op4(FmtFmFeedbackState *state,
                                   FmtFmOperator *op4,
                                   uint32_t phase_increment,
                                   uint32_t level_q15,
                                   uint32_t feedback_control)
{
    int32_t sample;
    int32_t phase_modulation;

    if (!op4)
        return 0;

    if (!state)
        return fmt_fm_operator_render(op4, phase_increment, 0, level_q15);

    phase_modulation = fmt_fm_feedback_phase_modulation(state,
                                                        feedback_control);
    sample = fmt_fm_operator_render(op4,
                                    phase_increment,
                                    phase_modulation,
                                    level_q15);
    state->previous_op4_q15 = (int16_t)sample;
    return sample;
}
