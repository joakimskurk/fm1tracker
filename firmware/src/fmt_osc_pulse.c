/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_osc_pulse.h"

void fmt_pulse_reset(FmtPulseOsc *osc)
{
    if (osc)
        osc->phase = 0u;
}

void fmt_pulse_retrigger(FmtPulseOsc *osc)
{
    fmt_pulse_reset(osc);
}

uint8_t fmt_pulse_clamp_width(uint32_t width)
{
    if (width < FMT_PULSE_WIDTH_MIN)
        return (uint8_t)FMT_PULSE_WIDTH_MIN;
    if (width > FMT_PULSE_WIDTH_MAX)
        return (uint8_t)FMT_PULSE_WIDTH_MAX;
    return (uint8_t)width;
}

int32_t fmt_pulse_render(FmtPulseOsc *osc,
                         uint32_t phase_increment,
                         uint8_t width)
{
    uint32_t phase, threshold;
    uint8_t safe_width;
    int32_t sample;

    if (!osc)
        return 0;

    safe_width = fmt_pulse_clamp_width(width);
    phase = osc->phase;
    threshold = (uint32_t)safe_width << 24;
    sample = phase < threshold ? FMT_PULSE_SAMPLE_HIGH : FMT_PULSE_SAMPLE_LOW;
    osc->phase = phase + phase_increment;
    return sample;
}
