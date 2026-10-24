/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_fm_operator.h"

#include "../gen/fmt_fm_sine_q15.inc"

void fmt_fm_operator_reset(FmtFmOperator *op)
{
    if (op)
        op->phase = 0u;
}

void fmt_fm_operator_retrigger(FmtFmOperator *op)
{
    fmt_fm_operator_reset(op);
}

uint32_t fmt_fm_operator_clamp_level_q15(uint32_t level_q15)
{
    if (level_q15 > FMT_FM_OPERATOR_LEVEL_MAX_Q15)
        return FMT_FM_OPERATOR_LEVEL_MAX_Q15;
    return level_q15;
}

int32_t fmt_fm_operator_sine_q15(uint32_t phase)
{
    uint32_t quadrant = phase >> 30;
    uint32_t quarter_offset = phase & 0x3FFFFFFFu;
    uint32_t coord;
    uint32_t index;
    uint32_t magnitude;

    if ((quadrant & 1u) == 0u)
        coord = quarter_offset;
    else
        coord = 0x40000000u - quarter_offset;

    index = coord >> FMT_FM_SINE_INDEX_SHIFT;
    if (index == 256u) {
        magnitude = (uint32_t)fmt_fm_sine_quarter_q15[256];
    } else {
        uint32_t frac = coord & FMT_FM_SINE_FRAC_MASK;
        uint32_t lo = (uint32_t)fmt_fm_sine_quarter_q15[index];
        uint32_t hi = (uint32_t)fmt_fm_sine_quarter_q15[index + 1u];
        uint32_t delta = hi - lo;
        magnitude = lo + ((delta * frac + FMT_FM_SINE_HALF_ROUND) >>
                          FMT_FM_SINE_INDEX_SHIFT);
    }

    if ((quadrant & 2u) != 0u)
        return -(int32_t)magnitude;
    return (int32_t)magnitude;
}

static int32_t fmt_fm_operator_apply_level(int32_t sample, uint32_t level_q15)
{
    uint32_t level = fmt_fm_operator_clamp_level_q15(level_q15);
    uint32_t magnitude;
    uint32_t scaled;

    if (sample == 0 || level == 0u)
        return 0;
    if (level == FMT_FM_OPERATOR_LEVEL_MAX_Q15)
        return sample;

    magnitude = sample < 0 ? (uint32_t)(-sample) : (uint32_t)sample;
    scaled = (magnitude * level + 16384u) >> 15;
    return sample < 0 ? -(int32_t)scaled : (int32_t)scaled;
}

int32_t fmt_fm_operator_render(FmtFmOperator *op,
                               uint32_t phase_increment,
                               int32_t phase_modulation,
                               uint32_t level_q15)
{
    uint32_t phase;
    uint32_t sample_phase;
    int32_t sample;

    if (!op)
        return 0;

    phase = op->phase;
    sample_phase = phase + (uint32_t)phase_modulation;
    sample = fmt_fm_operator_sine_q15(sample_phase);
    sample = fmt_fm_operator_apply_level(sample, level_q15);
    op->phase = phase + phase_increment;
    return sample;
}
