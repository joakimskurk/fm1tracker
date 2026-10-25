/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FMT_FM_FEEDBACK_H
#define FMT_FM_FEEDBACK_H

#include <stdint.h>

#include "fmt_fm_operator.h"

#define FMT_FM_FEEDBACK_CONTROL_MAX 255u

typedef struct {
    int16_t previous_op4_q15;
} FmtFmFeedbackState;

void fmt_fm_feedback_reset(FmtFmFeedbackState *state);
void fmt_fm_feedback_retrigger(FmtFmFeedbackState *state);
uint8_t fmt_fm_feedback_sanitize(uint32_t control);
uint16_t fmt_fm_feedback_depth_q14(uint32_t control);
int32_t fmt_fm_feedback_phase_modulation(const FmtFmFeedbackState *state,
                                         uint32_t feedback_control);
int32_t fmt_fm_feedback_render_op4(FmtFmFeedbackState *state,
                                   FmtFmOperator *op4,
                                   uint32_t phase_increment,
                                   uint32_t level_q15,
                                   uint32_t feedback_control);

#endif
