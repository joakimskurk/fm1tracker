/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_fm_algorithm.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static const uint32_t k_initial_phase[4] = {
    0x10203040u, 0x50607080u, 0x90A0B0C0u, 0xD0E0F000u
};

static const uint32_t k_increment[4] = {
    0x01234567u, 0x02345678u, 0x03456789u, 0x0456789Au
};

static const uint16_t k_level[4] = {32767u, 28000u, 22000u, 18000u};

static const uint32_t k_final_phase[4] = {
    0x4476A040u, 0x95C7F080u, 0xE71940C0u, 0x386A9000u
};

static const int32_t k_first12[4][12] = {
    {31709, 32144, 32478, 32686, 32765, 32732,
     32615, 32445, 32245, 32031, 31808, 31583},
    {13322, 12487, 11858, 11434, 11223, 11214,
     11408, 11786, 12337, 13043, 13887, 14841},
    {19166, 17927, 16580, 15148, 13662, 12161,
     10686, 9284, 8003, 6886, 5971, 5288},
    {3298, 3168, 3078, 3028, 3017, 3043,
     3106, 3203, 3331, 3488, 3670, 3874}
};

static void set_fixture(FmtFmAlgorithmState *state)
{
    state->op[0].phase = k_initial_phase[0];
    state->op[1].phase = k_initial_phase[1];
    state->op[2].phase = k_initial_phase[2];
    state->op[3].phase = k_initial_phase[3];
}

static void assert_phases(const FmtFmAlgorithmState *state,
                          const uint32_t expected[4])
{
    assert(state->op[0].phase == expected[0]);
    assert(state->op[1].phase == expected[1]);
    assert(state->op[2].phase == expected[2]);
    assert(state->op[3].phase == expected[3]);
}

static void write_i32_le(FILE *file, int32_t value)
{
    uint32_t bits = (uint32_t)value;
    unsigned char bytes[4];

    bytes[0] = (unsigned char)(bits & 0xFFu);
    bytes[1] = (unsigned char)((bits >> 8) & 0xFFu);
    bytes[2] = (unsigned char)((bits >> 16) & 0xFFu);
    bytes[3] = (unsigned char)((bits >> 24) & 0xFFu);
    assert(fwrite(bytes, 1u, 4u, file) == 4u);
}

static void test_state_and_sanitizers(void)
{
    FmtFmAlgorithmState state;
    uint32_t i;

    assert(sizeof(FmtFmAlgorithmState) == 16u);
    assert(FMT_FM_ALGORITHM_COUNT == 4u);
    assert(FMT_FM_ALGO_STACK4 == 0u);
    assert(FMT_FM_ALGO_FAN3 == 1u);
    assert(FMT_FM_ALGO_DUAL2 == 2u);
    assert(FMT_FM_ALGO_PARALLEL4 == 3u);

    assert(fmt_fm_algorithm_sanitize(0u) == 0u);
    assert(fmt_fm_algorithm_sanitize(1u) == 1u);
    assert(fmt_fm_algorithm_sanitize(2u) == 2u);
    assert(fmt_fm_algorithm_sanitize(3u) == 3u);
    assert(fmt_fm_algorithm_sanitize(4u) == 0u);
    assert(fmt_fm_algorithm_sanitize(255u) == 0u);
    assert(fmt_fm_algorithm_sanitize(0xFFFFFFFFu) == 0u);

    assert(fmt_fm_algorithm_clamp_mod_depth_q14(0u) == 0u);
    assert(fmt_fm_algorithm_clamp_mod_depth_q14(1u) == 1u);
    assert(fmt_fm_algorithm_clamp_mod_depth_q14(4096u) == 4096u);
    assert(fmt_fm_algorithm_clamp_mod_depth_q14(8192u) == 8192u);
    assert(fmt_fm_algorithm_clamp_mod_depth_q14(16383u) == 16383u);
    assert(fmt_fm_algorithm_clamp_mod_depth_q14(16384u) == 16383u);
    assert(fmt_fm_algorithm_clamp_mod_depth_q14(65535u) == 16383u);
    assert(fmt_fm_algorithm_clamp_mod_depth_q14(0xFFFFFFFFu) == 16383u);

    for (i = 0u; i < 4u; i++)
        state.op[i].phase = 0x11111111u * (i + 1u);
    fmt_fm_algorithm_reset(&state);
    assert(state.op[0].phase == 0u && state.op[1].phase == 0u &&
           state.op[2].phase == 0u && state.op[3].phase == 0u);

    for (i = 0u; i < 4u; i++)
        state.op[i].phase = 0x22222222u * (i + 1u);
    fmt_fm_algorithm_retrigger(&state);
    assert(state.op[0].phase == 0u && state.op[1].phase == 0u &&
           state.op[2].phase == 0u && state.op[3].phase == 0u);

    fmt_fm_algorithm_reset((FmtFmAlgorithmState *)0);
    fmt_fm_algorithm_retrigger((FmtFmAlgorithmState *)0);
    assert(fmt_fm_algorithm_render((FmtFmAlgorithmState *)0, 0u,
                                   k_increment, k_level, 7000u) == 0);
    assert(fmt_fm_algorithm_render(&state, 0u, (const uint32_t *)0,
                                   k_level, 7000u) == 0);
    assert(fmt_fm_algorithm_render(&state, 0u, k_increment,
                                   (const uint16_t *)0, 7000u) == 0);
}

static void test_first_vectors(void)
{
    uint32_t algorithm;

    for (algorithm = 0u; algorithm < 4u; algorithm++) {
        FmtFmAlgorithmState state;
        uint32_t i;
        set_fixture(&state);
        for (i = 0u; i < 12u; i++) {
            int32_t output = fmt_fm_algorithm_render(&state, (uint8_t)algorithm,
                                                     k_increment, k_level, 7000u);
            assert(output == k_first12[algorithm][i]);
        }
    }
}

static void test_invalid_fallback(void)
{
    static const uint8_t invalid[] = {4u, 7u, 255u};
    uint32_t index;

    for (index = 0u; index < (uint32_t)(sizeof(invalid) / sizeof(invalid[0])); index++) {
        FmtFmAlgorithmState a;
        FmtFmAlgorithmState b;
        int32_t ao;
        int32_t bo;
        set_fixture(&a);
        set_fixture(&b);
        ao = fmt_fm_algorithm_render(&a, invalid[index], k_increment, k_level, 7000u);
        bo = fmt_fm_algorithm_render(&b, FMT_FM_ALGO_STACK4,
                                     k_increment, k_level, 7000u);
        assert(ao == bo);
        assert(memcmp(&a, &b, sizeof(a)) == 0);
    }
}

static void test_algorithm_switch_phase_continuity(void)
{
    static const uint8_t sequence[8] = {0u, 3u, 1u, 2u, 3u, 0u, 2u, 1u};
    FmtFmAlgorithmState state;
    uint32_t expected[4];
    uint32_t step;
    uint32_t op;

    set_fixture(&state);
    memcpy(expected, k_initial_phase, sizeof(expected));

    for (step = 0u; step < 8u; step++) {
        (void)fmt_fm_algorithm_render(&state, sequence[step],
                                      k_increment, k_level, 7000u);
        for (op = 0u; op < 4u; op++)
            expected[op] += k_increment[op];
        assert_phases(&state, expected);
    }
}

static void write_long_vectors(const char *out_dir)
{
    uint32_t algorithm;

    for (algorithm = 0u; algorithm < 4u; algorithm++) {
        char path[256];
        FmtFmAlgorithmState state;
        FILE *file;
        uint32_t i;

        assert(snprintf(path, sizeof(path), "%s/algorithm%u.bin", out_dir,
                        (unsigned)algorithm) > 0);
        file = fopen(path, "wb");
        assert(file != NULL);
        set_fixture(&state);

        for (i = 0u; i < 4096u; i++) {
            int32_t output = fmt_fm_algorithm_render(&state, (uint8_t)algorithm,
                                                     k_increment, k_level, 7000u);
            write_i32_le(file, output);
        }
        assert(fclose(file) == 0);
        assert_phases(&state, k_final_phase);
    }
}

static void test_broad_stress(void)
{
    uint32_t algorithm;

    for (algorithm = 0u; algorithm < 4u; algorithm++) {
        FmtFmAlgorithmState state;
        uint32_t i;
        state.op[0].phase = 0x00000000u;
        state.op[1].phase = 0x40000000u;
        state.op[2].phase = 0x80000000u;
        state.op[3].phase = 0xC0000000u;

        for (i = 0u; i < 65536u; i++) {
            uint32_t increments[4];
            uint16_t levels[4];
            uint32_t old[4];
            uint16_t depth;
            int32_t output;

            increments[0] = 0x01020304u + i * 3u;
            increments[1] = 0x02030405u + i * 5u;
            increments[2] = 0x03040506u + i * 7u;
            increments[3] = 0x04050607u + i * 11u;
            levels[0] = (uint16_t)(i * 257u);
            levels[1] = (uint16_t)(i * 911u);
            levels[2] = (uint16_t)(i * 1237u);
            levels[3] = (uint16_t)(i * 1999u);
            depth = (uint16_t)(i * 37u);
            old[0] = state.op[0].phase;
            old[1] = state.op[1].phase;
            old[2] = state.op[2].phase;
            old[3] = state.op[3].phase;

            output = fmt_fm_algorithm_render(&state, (uint8_t)algorithm,
                                             increments, levels, depth);
            assert(output >= -32767 && output <= 32767);
            assert(state.op[0].phase == old[0] + increments[0]);
            assert(state.op[1].phase == old[1] + increments[1]);
            assert(state.op[2].phase == old[2] + increments[2]);
            assert(state.op[3].phase == old[3] + increments[3]);
        }
    }
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    test_state_and_sanitizers();
    test_first_vectors();
    test_invalid_fallback();
    test_algorithm_switch_phase_continuity();
    write_long_vectors(argv[1]);
    test_broad_stress();

    puts("WP5_2_FM_ALGORITHM_VECTORS_OK=1 algorithms=4 state_bytes=16 order=op4_op3_op2_op1");
    puts("WP5_2_FM_ALGORITHM_SWITCH_OK=1 phase_continuity=exact operators_per_sample=4");
    puts("WP5_2_FM_ALGORITHM_STRESS_OK=1 algorithms=4 renders_each=65536 output_q15=bounded phase_advance=exact");
    return 0;
}
