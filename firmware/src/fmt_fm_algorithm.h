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

/* D055 composition seam. OP4 has already been rendered and advanced exactly
 * once by the caller (normally D053 feedback). This function never touches
 * state->op[3]; it renders OP3/OP2/OP1 and applies the frozen D050 carrier
 * normalization for the selected topology. */
int32_t fmt_fm_algorithm_render_from_op4(FmtFmAlgorithmState *state,
                                         uint8_t algorithm,
                                         const uint32_t phase_increment[4],
                                         const uint16_t level_q15[4],
                                         uint16_t mod_depth_q14,
                                         int32_t op4_sample);

/* Original D050 public path remains bit-exact: render OP4 without feedback,
 * then delegate the remaining topology to the D055 seam. */
int32_t fmt_fm_algorithm_render(FmtFmAlgorithmState *state,
                                uint8_t algorithm,
                                const uint32_t phase_increment[4],
                                const uint16_t level_q15[4],
                                uint16_t mod_depth_q14);

#endif
