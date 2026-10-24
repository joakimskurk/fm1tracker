/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_osc_noise.h"

#include <stdint.h>

static uint32_t fmt_noise_lfsr_step(uint32_t state)
{
    uint32_t feedback;

    state &= FMT_NOISE_LFSR_MASK;
    feedback = (state ^ (state >> 1)) & 1u;
    return (state >> 1) | (feedback << 14);
}

void fmt_noise_reset(FmtNoiseOsc *osc)
{
    if (!osc)
        return;
    osc->phase = 0u;
    osc->lfsr = FMT_NOISE_SEED;
}

void fmt_noise_retrigger(FmtNoiseOsc *osc)
{
    fmt_noise_reset(osc);
}

uint32_t fmt_noise_clock_inc(uint32_t pitch_phase_inc)
{
    if (pitch_phase_inc > (UINT32_MAX >> FMT_NOISE_CLOCK_SHIFT))
        return UINT32_MAX;
    return pitch_phase_inc << FMT_NOISE_CLOCK_SHIFT;
}

int32_t fmt_noise_render(FmtNoiseOsc *osc, uint32_t noise_clock_inc)
{
    uint32_t phase;
    uint32_t next_phase;
    int32_t sample;

    if (!osc)
        return 0;

    sample = (osc->lfsr & 1u) ? FMT_NOISE_SAMPLE_HIGH : FMT_NOISE_SAMPLE_LOW;
    phase = osc->phase;
    next_phase = phase + noise_clock_inc;
    if (next_phase < phase)
        osc->lfsr = fmt_noise_lfsr_step(osc->lfsr);
    osc->phase = next_phase;
    return sample;
}
