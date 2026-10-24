/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FMT_OSC_TRIANGLE_H
#define FMT_OSC_TRIANGLE_H

#include <stdint.h>

#define FMT_TRIANGLE_PEAK 32767

typedef struct {
    uint32_t phase;
} FmtTriangleOsc;

/* D040 deterministic phase-only integer triangle primitive. */
void fmt_triangle_reset(FmtTriangleOsc *osc);
void fmt_triangle_retrigger(FmtTriangleOsc *osc);
int32_t fmt_triangle_render(FmtTriangleOsc *osc, uint32_t phase_increment);

#endif
