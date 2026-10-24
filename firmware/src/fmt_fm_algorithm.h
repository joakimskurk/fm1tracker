/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FMT_FM_ALGORITHM_H
#define FMT_FM_ALGORITHM_H

#include <stdint.h>

#include "fmt_fm_operator.h"

#define FMT_FM_ALGORITHM_COUNT 4u
#define FMT_FM_MOD_DEPTH_MAX_Q14 16383u

#define FMT_FM_ALGO_STACK4 0u
#define FMT_FM_ALGO_FAN3 1u
#define FMT_FM_ALGO_DUAL2 2u
#define FMT_FM_ALGO_PARALLEL4 3u

typedef struct {
    FmtFmOperator op[4];
} FmtFmAlgorithmState;

void fmt_fm_algorithm_reset(FmtFmAlgorithmState *state);
void fmt_fm_algorithm_retrigger(FmtFmAlgorithmState *state);
uint8_t fmt_fm_algorithm_sanitize(uint32_t algorithm);
uint16_t fmt_fm_algorithm_clamp_mod_depth_q14(uint32_t depth);
int32_t fmt_fm_algorithm_render(FmtFmAlgorithmState *state,
                                uint8_t algorithm,
                                const uint32_t phase_increment[4],
                                const uint16_t level_q15[4],
                                uint16_t mod_depth_q14);

#endif
