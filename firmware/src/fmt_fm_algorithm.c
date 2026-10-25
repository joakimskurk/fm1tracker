/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_fm_algorithm.h"

#include <limits.h>

void fmt_fm_algorithm_reset(FmtFmAlgorithmState *state)
{
    if (!state)
        return;
    fmt_fm_operator_reset(&state->op[0]);
    fmt_fm_operator_reset(&state->op[1]);
    fmt_fm_operator_reset(&state->op[2]);
    fmt_fm_operator_reset(&state->op[3]);
}

void fmt_fm_algorithm_retrigger(FmtFmAlgorithmState *state)
{
    fmt_fm_algorithm_reset(state);
}

uint8_t fmt_fm_algorithm_sanitize(uint32_t algorithm)
{
    if (algorithm >= FMT_FM_ALGORITHM_COUNT)
        return FMT_FM_ALGO_STACK4;
    return (uint8_t)algorithm;
}

uint16_t fmt_fm_algorithm_clamp_mod_depth_q14(uint32_t depth)
{
    if (depth > FMT_FM_MOD_DEPTH_MAX_Q14)
        return (uint16_t)FMT_FM_MOD_DEPTH_MAX_Q14;
    return (uint16_t)depth;
}

static int32_t fmt_fm_algorithm_edge_mod(int32_t sample, uint16_t depth)
{
    int32_t scaled = sample * (int32_t)depth;
    return scaled * 4;
}

static int32_t fmt_fm_algorithm_phase_add(int32_t a, int32_t b)
{
    uint32_t bits = (uint32_t)a + (uint32_t)b;

    if (bits <= (uint32_t)INT32_MAX)
        return (int32_t)bits;
    return -1 - (int32_t)(UINT32_MAX - bits);
}

static int32_t fmt_fm_algorithm_normalize(int32_t sum, uint8_t shift)
{
    uint32_t magnitude;

    if (sum < 0) {
        magnitude = (uint32_t)(-sum);
        return -(int32_t)(magnitude >> shift);
    }
    return (int32_t)((uint32_t)sum >> shift);
}

int32_t fmt_fm_algorithm_render_from_op4(FmtFmAlgorithmState *state,
                                         uint8_t algorithm,
                                         const uint32_t phase_increment[4],
                                         const uint16_t level_q15[4],
                                         uint16_t mod_depth_q14,
                                         int32_t op4_sample)
{
    uint8_t selected;
    uint16_t depth;
    int32_t s1, s2, s3, s4;
    int32_t modulation;

    if (!state || !phase_increment || !level_q15)
        return 0;

    selected = fmt_fm_algorithm_sanitize(algorithm);
    depth = fmt_fm_algorithm_clamp_mod_depth_q14(mod_depth_q14);
    s4 = op4_sample;

    if (selected == FMT_FM_ALGO_STACK4) {
        s3 = fmt_fm_operator_render(&state->op[2], phase_increment[2],
                                    fmt_fm_algorithm_edge_mod(s4, depth),
                                    level_q15[2]);
        s2 = fmt_fm_operator_render(&state->op[1], phase_increment[1],
                                    fmt_fm_algorithm_edge_mod(s3, depth),
                                    level_q15[1]);
        s1 = fmt_fm_operator_render(&state->op[0], phase_increment[0],
                                    fmt_fm_algorithm_edge_mod(s2, depth),
                                    level_q15[0]);
        return s1;
    }

    if (selected == FMT_FM_ALGO_FAN3) {
        s3 = fmt_fm_operator_render(&state->op[2], phase_increment[2], 0,
                                    level_q15[2]);
        s2 = fmt_fm_operator_render(&state->op[1], phase_increment[1], 0,
                                    level_q15[1]);
        modulation = fmt_fm_algorithm_phase_add(
            fmt_fm_algorithm_edge_mod(s4, depth),
            fmt_fm_algorithm_edge_mod(s3, depth));
        modulation = fmt_fm_algorithm_phase_add(
            modulation, fmt_fm_algorithm_edge_mod(s2, depth));
        s1 = fmt_fm_operator_render(&state->op[0], phase_increment[0], modulation,
                                    level_q15[0]);
        return s1;
    }

    if (selected == FMT_FM_ALGO_DUAL2) {
        s3 = fmt_fm_operator_render(&state->op[2], phase_increment[2],
                                    fmt_fm_algorithm_edge_mod(s4, depth),
                                    level_q15[2]);
        s2 = fmt_fm_operator_render(&state->op[1], phase_increment[1], 0,
                                    level_q15[1]);
        s1 = fmt_fm_operator_render(&state->op[0], phase_increment[0],
                                    fmt_fm_algorithm_edge_mod(s2, depth),
                                    level_q15[0]);
        return fmt_fm_algorithm_normalize(s1 + s3, 1u);
    }

    s3 = fmt_fm_operator_render(&state->op[2], phase_increment[2], 0,
                                level_q15[2]);
    s2 = fmt_fm_operator_render(&state->op[1], phase_increment[1], 0,
                                level_q15[1]);
    s1 = fmt_fm_operator_render(&state->op[0], phase_increment[0], 0,
                                level_q15[0]);
    return fmt_fm_algorithm_normalize(s1 + s2 + s3 + s4, 2u);
}

int32_t fmt_fm_algorithm_render(FmtFmAlgorithmState *state,
                                uint8_t algorithm,
                                const uint32_t phase_increment[4],
                                const uint16_t level_q15[4],
                                uint16_t mod_depth_q14)
{
    int32_t s4;

    if (!state || !phase_increment || !level_q15)
        return 0;

    s4 = fmt_fm_operator_render(&state->op[3], phase_increment[3], 0,
                                level_q15[3]);
    return fmt_fm_algorithm_render_from_op4(state,
                                            algorithm,
                                            phase_increment,
                                            level_q15,
                                            mod_depth_q14,
                                            s4);
}
