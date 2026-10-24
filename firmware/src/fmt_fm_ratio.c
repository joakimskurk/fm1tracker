/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_fm_ratio.h"

static const uint16_t fmt_fm_ratio_q8_table[FMT_FM_RATIO_COUNT] = {
    128u, 256u, 320u, 362u,
    384u, 512u, 640u, 768u,
    896u, 1024u, 1280u, 1536u,
    1792u, 2048u, 3072u, 4096u
};

static const uint32_t fmt_fm_ratio_max_base[FMT_FM_RATIO_COUNT] = {
    1293741834u, 1293741834u, 1293741834u, 1293741834u,
    1293741834u, 1073741823u, 858993458u, 715827882u,
    613566756u, 536870911u, 429496729u, 357913941u,
    306783378u, 268435455u, 178956970u, 134217727u
};

uint8_t fmt_fm_ratio_sanitize(uint32_t ratio_id)
{
    if (ratio_id < FMT_FM_RATIO_COUNT)
        return (uint8_t)ratio_id;
    return FMT_FM_RATIO_UNITY;
}

uint16_t fmt_fm_ratio_q8(uint32_t ratio_id)
{
    return fmt_fm_ratio_q8_table[fmt_fm_ratio_sanitize(ratio_id)];
}

uint32_t fmt_fm_ratio_apply(uint32_t base_increment, uint32_t ratio_id)
{
    uint8_t selected = fmt_fm_ratio_sanitize(ratio_id);
    uint32_t ratio;
    uint32_t whole;
    uint32_t remainder;

    if (base_increment > FMT_FM_RATIO_BASE_INCREMENT_MAX)
        base_increment = FMT_FM_RATIO_BASE_INCREMENT_MAX;

    if (base_increment > fmt_fm_ratio_max_base[selected])
        return FMT_FM_RATIO_INCREMENT_CEILING;

    ratio = fmt_fm_ratio_q8_table[selected];
    whole = base_increment >> 8;
    remainder = base_increment & 255u;

    return whole * ratio + ((remainder * ratio + 128u) >> 8);
}

void fmt_fm_ratio_map4(uint32_t base_increment,
                       const uint8_t ratio_id[4],
                       uint32_t out_increment[4])
{
    if (!out_increment)
        return;

    if (!ratio_id) {
        out_increment[0] = fmt_fm_ratio_apply(base_increment, FMT_FM_RATIO_UNITY);
        out_increment[1] = fmt_fm_ratio_apply(base_increment, FMT_FM_RATIO_UNITY);
        out_increment[2] = fmt_fm_ratio_apply(base_increment, FMT_FM_RATIO_UNITY);
        out_increment[3] = fmt_fm_ratio_apply(base_increment, FMT_FM_RATIO_UNITY);
        return;
    }

    out_increment[0] = fmt_fm_ratio_apply(base_increment, ratio_id[0]);
    out_increment[1] = fmt_fm_ratio_apply(base_increment, ratio_id[1]);
    out_increment[2] = fmt_fm_ratio_apply(base_increment, ratio_id[2]);
    out_increment[3] = fmt_fm_ratio_apply(base_increment, ratio_id[3]);
}
