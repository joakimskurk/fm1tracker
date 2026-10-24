/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_fm_controls.h"

uint8_t fmt_fm_control_sanitize(uint32_t control)
{
    if (control > FMT_FM_CONTROL_MAX)
        return FMT_FM_CONTROL_MAX;
    return (uint8_t)control;
}

uint16_t fmt_fm_level_q15(uint32_t control)
{
    uint16_t v = fmt_fm_control_sanitize(control);
    return (uint16_t)((v << 7) | (v >> 1));
}

uint16_t fmt_fm_index_q14(uint32_t control)
{
    uint16_t v = fmt_fm_control_sanitize(control);
    return (uint16_t)((v << 6) | (v >> 2));
}

void fmt_fm_controls_map(const uint8_t level_control[4],
                         uint8_t index_control,
                         FmtFmMappedControls *out)
{
    if (!out)
        return;

    if (!level_control) {
        out->level_q15[0] = 0u;
        out->level_q15[1] = 0u;
        out->level_q15[2] = 0u;
        out->level_q15[3] = 0u;
    } else {
        out->level_q15[0] = fmt_fm_level_q15(level_control[0]);
        out->level_q15[1] = fmt_fm_level_q15(level_control[1]);
        out->level_q15[2] = fmt_fm_level_q15(level_control[2]);
        out->level_q15[3] = fmt_fm_level_q15(level_control[3]);
    }

    out->mod_depth_q14 = fmt_fm_index_q14(index_control);
}
