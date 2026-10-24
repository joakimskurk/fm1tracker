/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_osc_wavetable8.h"

#include <stdint.h>

/* D041_TABLE_DATA_BEGIN */
static const int8_t fmt_wavetable8_tables[FMT_WAVETABLE8_TABLE_COUNT][FMT_WAVETABLE8_TABLE_LENGTH] = {
    {
        0, 25, 49, 71, 90, 106, 117, 125,
        127, 125, 117, 106, 90, 71, 49, 25,
        0, -25, -49, -71, -90, -106, -117, -125,
        -127, -125, -117, -106, -90, -71, -49, -25
    },
    {
        0, 3, 16, 27, 28, 33, 48, 57,
        56, 64, 83, 87, 79, 96, 127, 103,
        0, -103, -127, -96, -79, -87, -83, -64,
        -56, -57, -48, -33, -28, -27, -16, -3
    },
    {
        0, 127, 97, 115, 102, 113, 103, 112,
        103, 112, 103, 113, 102, 115, 97, 127,
        0, -127, -97, -115, -102, -113, -103, -112,
        -103, -112, -103, -113, -102, -115, -97, -127
    },
    {
        0, 75, 103, 107, 123, 127, 101, 81,
        98, 110, 80, 35, 15, 1, -28, -36,
        0, 36, 28, -1, -15, -35, -80, -110,
        -98, -81, -101, -127, -123, -107, -103, -75
    },
    {
        0, 43, 73, 97, 121, 127, 90, 27,
        -4, 27, 90, 127, 121, 97, 73, 43,
        0, -43, -73, -97, -121, -127, -90, -27,
        4, -27, -90, -127, -121, -97, -73, -43
    },
    {
        0, 78, 123, 127, 108, 87, 70, 52,
        34, 24, 29, 38, 35, 15, -8, -14,
        0, 14, 8, -15, -35, -38, -29, -24,
        -34, -52, -70, -87, -108, -127, -123, -78
    },
    {
        0, 127, 79, 26, 3, 10, 102, 118,
        74, 118, 102, 10, 3, 26, 79, 127,
        0, -127, -79, -26, -3, -10, -102, -118,
        -74, -118, -102, -10, -3, -26, -79, -127
    },
    {
        0, 99, 124, 122, 123, 126, 117, 84,
        72, 117, 127, 86, 50, 1, -53, -58,
        0, 58, 53, -1, -50, -86, -127, -117,
        -72, -84, -117, -126, -123, -122, -124, -99
    }
};
/* D041_TABLE_DATA_END */

void fmt_wavetable8_reset(FmtWavetable8Osc *osc)
{
    if (!osc)
        return;
    osc->phase = 0u;
}

void fmt_wavetable8_retrigger(FmtWavetable8Osc *osc)
{
    fmt_wavetable8_reset(osc);
}

uint8_t fmt_wavetable8_sanitize_table(uint32_t table)
{
    if (table >= FMT_WAVETABLE8_TABLE_COUNT)
        return FMT_WAVETABLE8_SINE;
    return (uint8_t)table;
}

int32_t fmt_wavetable8_render(FmtWavetable8Osc *osc,
                              uint32_t phase_increment,
                              uint32_t table)
{
    uint8_t selector;
    uint32_t index;
    int32_t sample;

    if (!osc)
        return 0;

    selector = fmt_wavetable8_sanitize_table(table);
    index = osc->phase >> FMT_WAVETABLE8_INDEX_SHIFT;
    sample = (int32_t)fmt_wavetable8_tables[selector][index] * FMT_WAVETABLE8_Q15_SCALE;
    osc->phase += phase_increment;
    return sample;
}
