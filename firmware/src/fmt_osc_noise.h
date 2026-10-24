/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FMT_OSC_NOISE_H
#define FMT_OSC_NOISE_H

#include <stdint.h>

#define FMT_NOISE_LFSR_MASK 0x7FFFu
#define FMT_NOISE_SEED 0x5A3Du
#define FMT_NOISE_SAMPLE_HIGH 32767
#define FMT_NOISE_SAMPLE_LOW (-32767)
#define FMT_NOISE_CLOCK_SHIFT 4u

typedef struct {
    uint32_t phase;
    uint32_t lfsr;
} FmtNoiseOsc;

/* D039 deterministic 15-bit clocked chip-noise primitive. */
void fmt_noise_reset(FmtNoiseOsc *osc);
void fmt_noise_retrigger(FmtNoiseOsc *osc);
uint32_t fmt_noise_clock_inc(uint32_t pitch_phase_inc);
int32_t fmt_noise_render(FmtNoiseOsc *osc, uint32_t noise_clock_inc);

#endif
