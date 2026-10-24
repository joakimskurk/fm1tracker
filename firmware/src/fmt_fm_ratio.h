/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FMT_FM_RATIO_H
#define FMT_FM_RATIO_H

#include <stdint.h>

#define FMT_FM_RATIO_COUNT 16u

#define FMT_FM_RATIO_HALF 0u
#define FMT_FM_RATIO_UNITY 1u
#define FMT_FM_RATIO_FIVE_QUARTERS 2u
#define FMT_FM_RATIO_INHARMONIC_1P414 3u
#define FMT_FM_RATIO_THREE_HALVES 4u
#define FMT_FM_RATIO_TWO 5u
#define FMT_FM_RATIO_FIVE_HALVES 6u
#define FMT_FM_RATIO_THREE 7u
#define FMT_FM_RATIO_SEVEN_HALVES 8u
#define FMT_FM_RATIO_FOUR 9u
#define FMT_FM_RATIO_FIVE 10u
#define FMT_FM_RATIO_SIX 11u
#define FMT_FM_RATIO_SEVEN 12u
#define FMT_FM_RATIO_EIGHT 13u
#define FMT_FM_RATIO_TWELVE 14u
#define FMT_FM_RATIO_SIXTEEN 15u

#define FMT_FM_RATIO_BASE_INCREMENT_MAX 1293741834u
#define FMT_FM_RATIO_INCREMENT_CEILING 2147483647u

uint8_t fmt_fm_ratio_sanitize(uint32_t ratio_id);
uint16_t fmt_fm_ratio_q8(uint32_t ratio_id);
uint32_t fmt_fm_ratio_apply(uint32_t base_increment, uint32_t ratio_id);
void fmt_fm_ratio_map4(uint32_t base_increment,
                       const uint8_t ratio_id[4],
                       uint32_t out_increment[4]);

#endif
