/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FMT_OSC_PULSE_H
#define FMT_OSC_PULSE_H

#include <stdint.h>

#define FMT_PULSE_WIDTH_MIN     32u
#define FMT_PULSE_WIDTH_DEFAULT 128u
#define FMT_PULSE_WIDTH_MAX     224u
#define FMT_PULSE_SAMPLE_HIGH   32767
#define FMT_PULSE_SAMPLE_LOW    (-32767)

typedef struct {
    uint32_t phase;
} FmtPulseOsc;

typedef char fmt_pulse_state_must_be_4_bytes[(sizeof(FmtPulseOsc) == 4u) ? 1 : -1];

void fmt_pulse_reset(FmtPulseOsc *osc);
void fmt_pulse_retrigger(FmtPulseOsc *osc);
uint8_t fmt_pulse_clamp_width(uint32_t width);
int32_t fmt_pulse_render(FmtPulseOsc *osc,
                         uint32_t phase_increment,
                         uint8_t width);

#endif
