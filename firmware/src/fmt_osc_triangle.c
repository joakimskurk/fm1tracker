/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_osc_triangle.h"

#include <stdint.h>

static int32_t fmt_triangle_sample(uint32_t phase)
{
    uint32_t quadrant = phase >> 30;
    int32_t offset = (int32_t)((phase >> 15) & 0x7FFFu);

    if (quadrant == 0u)
        return offset;
    if (quadrant == 1u)
        return FMT_TRIANGLE_PEAK - offset;
    if (quadrant == 2u)
        return -offset;
    return -FMT_TRIANGLE_PEAK + offset;
}

void fmt_triangle_reset(FmtTriangleOsc *osc)
{
    if (!osc)
        return;
    osc->phase = 0u;
}

void fmt_triangle_retrigger(FmtTriangleOsc *osc)
{
    fmt_triangle_reset(osc);
}

int32_t fmt_triangle_render(FmtTriangleOsc *osc, uint32_t phase_increment)
{
    int32_t sample;

    if (!osc)
        return 0;

    sample = fmt_triangle_sample(osc->phase);
    osc->phase += phase_increment;
    return sample;
}
