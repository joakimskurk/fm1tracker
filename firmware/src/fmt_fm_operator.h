/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FMT_FM_OPERATOR_H
#define FMT_FM_OPERATOR_H

#include <stdint.h>

#define FMT_FM_OPERATOR_LEVEL_MAX_Q15 32767u
#define FMT_FM_SINE_QUARTER_ENTRIES 257u
#define FMT_FM_SINE_INDEX_SHIFT 22u
#define FMT_FM_SINE_FRAC_MASK 0x003FFFFFu
#define FMT_FM_SINE_HALF_ROUND 0x00200000u

typedef struct {
    uint32_t phase;
} FmtFmOperator;

void fmt_fm_operator_reset(FmtFmOperator *op);
void fmt_fm_operator_retrigger(FmtFmOperator *op);
uint32_t fmt_fm_operator_clamp_level_q15(uint32_t level_q15);
int32_t fmt_fm_operator_sine_q15(uint32_t phase);
int32_t fmt_fm_operator_render(FmtFmOperator *op,
                               uint32_t phase_increment,
                               int32_t phase_modulation,
                               uint32_t level_q15);

#endif
