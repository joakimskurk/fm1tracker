/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FMT_OSC_WAVETABLE8_H
#define FMT_OSC_WAVETABLE8_H

#include <stdint.h>

#define FMT_WAVETABLE8_TABLE_COUNT 8u
#define FMT_WAVETABLE8_TABLE_LENGTH 32u
#define FMT_WAVETABLE8_INDEX_SHIFT 27u
#define FMT_WAVETABLE8_Q15_SCALE 258

#define FMT_WAVETABLE8_SINE 0u
#define FMT_WAVETABLE8_SAW8 1u
#define FMT_WAVETABLE8_SQUARE8 2u
#define FMT_WAVETABLE8_ORGAN 3u
#define FMT_WAVETABLE8_HOLLOW 4u
#define FMT_WAVETABLE8_NASAL 5u
#define FMT_WAVETABLE8_METAL 6u
#define FMT_WAVETABLE8_DIGI 7u

typedef struct {
    uint32_t phase;
} FmtWavetable8Osc;

/* D041 deterministic eight-table, 32-step, zero-order-hold primitive. */
void fmt_wavetable8_reset(FmtWavetable8Osc *osc);
void fmt_wavetable8_retrigger(FmtWavetable8Osc *osc);
uint8_t fmt_wavetable8_sanitize_table(uint32_t table);
int32_t fmt_wavetable8_render(FmtWavetable8Osc *osc,
                              uint32_t phase_increment,
                              uint32_t table);

#endif
