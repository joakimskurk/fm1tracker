/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FMT_FM_CONTROLS_H
#define FMT_FM_CONTROLS_H

#include <stdint.h>

#define FMT_FM_CONTROL_MAX 255u

typedef struct {
    uint16_t level_q15[4];
    uint16_t mod_depth_q14;
} FmtFmMappedControls;

uint8_t fmt_fm_control_sanitize(uint32_t control);
uint16_t fmt_fm_level_q15(uint32_t control);
uint16_t fmt_fm_index_q14(uint32_t control);
void fmt_fm_controls_map(const uint8_t level_control[4],
                         uint8_t index_control,
                         FmtFmMappedControls *out);

#endif
