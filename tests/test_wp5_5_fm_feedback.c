/* SPDX-License-Identifier: GPL-3.0-only */
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "fmt_fm_feedback.h"

#define TEST_ALGORITHM_COUNT 4u
#define TEST_Q14_MAX 16383u

#define TEST_ALGO_STACK4 0u
#define TEST_ALGO_FAN3 1u
#define TEST_ALGO_DUAL2 2u
#define TEST_ALGO_PARALLEL4 3u

typedef struct {
    FmtFmOperator op[4];
    FmtFmFeedbackState feedback;
} TestFmState;

static const uint32_t fixture_phase[4] = {
    0x10203040u, 0x50607080u, 0x90A0B0C0u, 0xD0E0F000u
};

static const uint32_t fixture_increment[4] = {
    0x01234567u, 0x02345678u, 0x03456789u, 0x0456789Au
};

static const uint16_t fixture_level[4] = {
    32767u, 28000u, 22000u, 18000u
};

static const uint32_t extreme_increment[4] = {
    407681904u, 1223045711u, 2147483647u, 2147483647u
};

static const uint16_t extreme_level[4] = {
    8224u, 16448u, 24672u, 32767u
};

static void fail(const char *what)
{
    fprintf(stderr, "WP5.5 FM feedback test failed: %s\n", what);
    exit(1);
}

static void expect_i32(const char *what, int32_t actual, int32_t expected)
{
    if (actual != expected) {
        fprintf(stderr, "%s: got %ld expected %ld\n",
                what, (long)actual, (long)expected);
        exit(1);
    }
}

static void expect_u32(const char *what, uint32_t actual, uint32_t expected)
{
    if (actual != expected) {
        fprintf(stderr, "%s: got %lu expected %lu\n",
                what, (unsigned long)actual, (unsigned long)expected);
        exit(1);
    }
}

static FILE *open_output(const char *dir, const char *name)
{
    char path[512];
    int n = snprintf(path, sizeof(path), "%s/%s", dir, name);
    FILE *f;

    if (n < 0 || (size_t)n >= sizeof(path))
        fail("output path too long");
    f = fopen(path, "wb");
    if (!f)
        fail("open output file");
    return f;
}

static void write_le16(FILE *f, uint16_t value)
{
    unsigned char b[2];
    b[0] = (unsigned char)(value & 255u);
    b[1] = (unsigned char)((value >> 8) & 255u);
    if (fwrite(b, 1u, sizeof(b), f) != sizeof(b))
        fail("write uint16");
}

static void write_s32(FILE *f, int32_t value)
{
    uint32_t bits = (uint32_t)value;
    unsigned char b[4];
    b[0] = (unsigned char)(bits & 255u);
    b[1] = (unsigned char)((bits >> 8) & 255u);
    b[2] = (unsigned char)((bits >> 16) & 255u);
    b[3] = (unsigned char)((bits >> 24) & 255u);
    if (fwrite(b, 1u, sizeof(b), f) != sizeof(b))
        fail("write int32");
}

static int32_t edge_mod(int32_t sample, uint16_t depth)
{
    int32_t scaled = sample * (int32_t)depth;
    return scaled * 4;
}

static int32_t phase_add(int32_t a, int32_t b)
{
    uint32_t bits = (uint32_t)a + (uint32_t)b;

    if (bits <= (uint32_t)INT32_MAX)
        return (int32_t)bits;
    return -1 - (int32_t)(UINT32_MAX - bits);
}

static int32_t normalize(int32_t sum, uint8_t shift)
{
    uint32_t magnitude;

    if (sum < 0) {
        magnitude = (uint32_t)(-sum);
        return -(int32_t)(magnitude >> shift);
    }
    return (int32_t)((uint32_t)sum >> shift);
}

static void init_fixture(TestFmState *state)
{
    uint32_t i;

    for (i = 0u; i < 4u; ++i)
        state->op[i].phase = fixture_phase[i];
    fmt_fm_feedback_reset(&state->feedback);
}

static void init_extreme(TestFmState *state)
{
    uint32_t i;

    for (i = 0u; i < 4u; ++i)
        state->op[i].phase = 0u;
    fmt_fm_feedback_reset(&state->feedback);
}

static int32_t render_test_composition(TestFmState *state,
                                       uint8_t algorithm,
                                       const uint32_t increment[4],
                                       const uint16_t level[4],
                                       uint16_t mod_depth_q14,
                                       uint32_t feedback_control)
{
    uint16_t depth = mod_depth_q14 > TEST_Q14_MAX ? TEST_Q14_MAX : mod_depth_q14;
    int32_t s1, s2, s3, s4;
    int32_t modulation;

    if (algorithm >= TEST_ALGORITHM_COUNT)
        algorithm = TEST_ALGO_STACK4;

    s4 = fmt_fm_feedback_render_op4(&state->feedback,
                                    &state->op[3],
                                    increment[3],
                                    level[3],
                                    feedback_control);

    if (algorithm == TEST_ALGO_STACK4) {
        s3 = fmt_fm_operator_render(&state->op[2], increment[2],
                                    edge_mod(s4, depth), level[2]);
        s2 = fmt_fm_operator_render(&state->op[1], increment[1],
                                    edge_mod(s3, depth), level[1]);
        return fmt_fm_operator_render(&state->op[0], increment[0],
                                      edge_mod(s2, depth), level[0]);
    }

    if (algorithm == TEST_ALGO_FAN3) {
        s3 = fmt_fm_operator_render(&state->op[2], increment[2], 0, level[2]);
        s2 = fmt_fm_operator_render(&state->op[1], increment[1], 0, level[1]);
        modulation = phase_add(edge_mod(s4, depth), edge_mod(s3, depth));
        modulation = phase_add(modulation, edge_mod(s2, depth));
        return fmt_fm_operator_render(&state->op[0], increment[0],
                                      modulation, level[0]);
    }

    if (algorithm == TEST_ALGO_DUAL2) {
        s3 = fmt_fm_operator_render(&state->op[2], increment[2],
                                    edge_mod(s4, depth), level[2]);
        s2 = fmt_fm_operator_render(&state->op[1], increment[1], 0, level[1]);
        s1 = fmt_fm_operator_render(&state->op[0], increment[0],
                                    edge_mod(s2, depth), level[0]);
        return normalize(s1 + s3, 1u);
    }

    s3 = fmt_fm_operator_render(&state->op[2], increment[2], 0, level[2]);
    s2 = fmt_fm_operator_render(&state->op[1], increment[1], 0, level[1]);
    s1 = fmt_fm_operator_render(&state->op[0], increment[0], 0, level[0]);
    return normalize(s1 + s2 + s3 + s4, 2u);
}

static void verify_state_control_and_nulls(const char *out_dir)
{
    FmtFmFeedbackState feedback;
    FmtFmOperator op;
    FmtFmOperator direct;
    FILE *map = open_output(out_dir, "feedback_map.bin");
    uint32_t control;
    int32_t sample;
    int32_t direct_sample;

    if (sizeof(FmtFmFeedbackState) != 2u)
        fail("feedback state size");

    feedback.previous_op4_q15 = 1234;
    fmt_fm_feedback_reset(&feedback);
    expect_i32("reset history", feedback.previous_op4_q15, 0);
    feedback.previous_op4_q15 = -1234;
    fmt_fm_feedback_retrigger(&feedback);
    expect_i32("retrigger history", feedback.previous_op4_q15, 0);
    fmt_fm_feedback_reset(NULL);
    fmt_fm_feedback_retrigger(NULL);

    expect_u32("sanitize zero", fmt_fm_feedback_sanitize(0u), 0u);
    expect_u32("sanitize max", fmt_fm_feedback_sanitize(255u), 255u);
    expect_u32("sanitize 256", fmt_fm_feedback_sanitize(256u), 255u);
    expect_u32("sanitize uint32 max", fmt_fm_feedback_sanitize(0xffffffffu), 255u);
    expect_u32("depth zero", fmt_fm_feedback_depth_q14(0u), 0u);
    expect_u32("depth one", fmt_fm_feedback_depth_q14(1u), 64u);
    expect_u32("depth 192", fmt_fm_feedback_depth_q14(192u), 12336u);
    expect_u32("depth max", fmt_fm_feedback_depth_q14(255u), 16383u);
    expect_u32("depth malformed", fmt_fm_feedback_depth_q14(0xffffffffu), 16383u);

    for (control = 0u; control <= 255u; ++control)
        write_le16(map, fmt_fm_feedback_depth_q14(control));
    if (fclose(map) != 0)
        fail("close map payload");

    expect_i32("null phase modulation",
               fmt_fm_feedback_phase_modulation(NULL, 255u), 0);
    feedback.previous_op4_q15 = 32767;
    expect_i32("positive maximum",
               fmt_fm_feedback_phase_modulation(&feedback, 255u),
               2147287044);
    feedback.previous_op4_q15 = (int16_t)-32768;
    expect_i32("negative int16 maximum",
               fmt_fm_feedback_phase_modulation(&feedback, 255u),
               (int32_t)-2147352576);

    feedback.previous_op4_q15 = 777;
    sample = fmt_fm_feedback_render_op4(&feedback, NULL, 1u, 32767u, 255u);
    expect_i32("null op sample", sample, 0);
    expect_i32("null op history unchanged", feedback.previous_op4_q15, 777);

    op.phase = 0x12345678u;
    direct = op;
    direct_sample = fmt_fm_operator_render(&direct, 0x01020304u, 0, 18000u);
    sample = fmt_fm_feedback_render_op4(NULL, &op, 0x01020304u, 18000u, 255u);
    expect_i32("null state ordinary render", sample, direct_sample);
    expect_u32("null state phase advance", op.phase, direct.phase);

    feedback.previous_op4_q15 = -2222;
    op.phase = 0xD0E0F000u;
    sample = fmt_fm_feedback_render_op4(&feedback, &op,
                                        0x0456789Au, 18000u, 0u);
    expect_i32("feedback zero phase offset",
               fmt_fm_feedback_phase_modulation(&feedback, 0u), 0);
    expect_i32("feedback zero tracks history", feedback.previous_op4_q15, sample);
}

static void verify_full_history_surface(void)
{
    FmtFmFeedbackState state;
    int32_t history;
    uint32_t control;
    int32_t minimum = 0;
    int32_t maximum = 0;

    for (history = -32768; history <= 32767; ++history) {
        state.previous_op4_q15 = (int16_t)history;
        for (control = 0u; control <= 255u; ++control) {
            int32_t depth = (int32_t)(((uint16_t)control << 6) |
                                      ((uint16_t)control >> 2));
            int32_t expected = history * depth * 4;
            int32_t actual = fmt_fm_feedback_phase_modulation(&state, control);
            if (actual != expected)
                fail("full history/control surface mismatch");
            if (actual < minimum)
                minimum = actual;
            if (actual > maximum)
                maximum = actual;
        }
    }

    expect_i32("surface minimum", minimum, (int32_t)-2147352576);
    expect_i32("surface maximum", maximum, 2147287044);
}

static void write_op4_fixture(const char *out_dir)
{
    static const int32_t first_expected[12] = {
        -16477, -14123, -16632, -16072, -17085, -17264,
        -17664, -17880, -17994, -17951, -17698, -17154
    };
    FmtFmOperator op;
    FmtFmFeedbackState feedback;
    FILE *f = open_output(out_dir, "op4_4096.bin");
    uint32_t i;

    op.phase = 0xD0E0F000u;
    fmt_fm_feedback_reset(&feedback);

    for (i = 0u; i < 4096u; ++i) {
        int32_t sample = fmt_fm_feedback_render_op4(&feedback, &op,
                                                    0x0456789Au,
                                                    18000u,
                                                    192u);
        if (i < 12u)
            expect_i32("OP4 first outputs", sample, first_expected[i]);
        write_s32(f, sample);
    }

    if (fclose(f) != 0)
        fail("close OP4 payload");
    expect_u32("OP4 final phase", op.phase, 0x386A9000u);
    expect_i32("OP4 final history", feedback.previous_op4_q15, 13151);
}

static void write_algorithm_fixtures(const char *out_dir)
{
    static const char *names[4] = {
        "algo_stack4_4096.bin", "algo_fan3_4096.bin",
        "algo_dual2_4096.bin", "algo_parallel4_4096.bin"
    };
    uint32_t algorithm;

    for (algorithm = 0u; algorithm < 4u; ++algorithm) {
        TestFmState state;
        FILE *f = open_output(out_dir, names[algorithm]);
        uint32_t i;

        init_fixture(&state);
        for (i = 0u; i < 4096u; ++i) {
            int32_t sample = render_test_composition(&state,
                                                     (uint8_t)algorithm,
                                                     fixture_increment,
                                                     fixture_level,
                                                     7000u,
                                                     192u);
            write_s32(f, sample);
        }
        if (fclose(f) != 0)
            fail("close algorithm payload");

        expect_u32("algorithm final OP1 phase", state.op[0].phase, 0x4476A040u);
        expect_u32("algorithm final OP2 phase", state.op[1].phase, 0x95C7F080u);
        expect_u32("algorithm final OP3 phase", state.op[2].phase, 0xE71940C0u);
        expect_u32("algorithm final OP4 phase", state.op[3].phase, 0x386A9000u);
        expect_i32("algorithm final history", state.feedback.previous_op4_q15, 13151);
    }
}

static void write_switch_fixture(const char *out_dir)
{
    TestFmState state;
    FILE *f = open_output(out_dir, "switch_4096.bin");
    uint32_t i;

    init_fixture(&state);
    for (i = 0u; i < 4096u; ++i) {
        uint8_t algorithm = (uint8_t)(i / 1024u);
        int32_t sample = render_test_composition(&state,
                                                 algorithm,
                                                 fixture_increment,
                                                 fixture_level,
                                                 7000u,
                                                 192u);
        write_s32(f, sample);
    }
    if (fclose(f) != 0)
        fail("close switch payload");

    expect_u32("switch final OP1 phase", state.op[0].phase, 0x4476A040u);
    expect_u32("switch final OP2 phase", state.op[1].phase, 0x95C7F080u);
    expect_u32("switch final OP3 phase", state.op[2].phase, 0xE71940C0u);
    expect_u32("switch final OP4 phase", state.op[3].phase, 0x386A9000u);
    expect_i32("switch final history", state.feedback.previous_op4_q15, 13151);
}

static void write_extreme_fixtures(const char *out_dir)
{
    static const char *names[4] = {
        "extreme_stack4_65536.bin", "extreme_fan3_65536.bin",
        "extreme_dual2_65536.bin", "extreme_parallel4_65536.bin"
    };
    uint32_t algorithm;

    for (algorithm = 0u; algorithm < 4u; ++algorithm) {
        TestFmState state;
        FILE *f = open_output(out_dir, names[algorithm]);
        uint32_t i;

        init_extreme(&state);
        for (i = 0u; i < 65536u; ++i) {
            int32_t sample = render_test_composition(&state,
                                                     (uint8_t)algorithm,
                                                     extreme_increment,
                                                     extreme_level,
                                                     16383u,
                                                     255u);
            if (sample < -32767 || sample > 32767)
                fail("extreme output outside Q15");
            write_s32(f, sample);
        }
        if (fclose(f) != 0)
            fail("close extreme payload");

        expect_u32("extreme final OP1 phase", state.op[0].phase, 0xBB700000u);
        expect_u32("extreme final OP2 phase", state.op[1].phase, 0x324F0000u);
        expect_u32("extreme final OP3 phase", state.op[2].phase, 0xFFFF0000u);
        expect_u32("extreme final OP4 phase", state.op[3].phase, 0xFFFF0000u);
        expect_i32("extreme final history", state.feedback.previous_op4_q15, 2517);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
        fail("expected output directory");

    verify_state_control_and_nulls(argv[1]);
    verify_full_history_surface();
    write_op4_fixture(argv[1]);
    write_algorithm_fixtures(argv[1]);
    write_switch_fixture(argv[1]);
    write_extreme_fixtures(argv[1]);

    printf("WP5_5_FM_FEEDBACK_C_STATE_OK=1 state_bytes=2 reset=zero retrigger=zero nulls=safe feedback_zero_tracks_history=1\n");
    printf("WP5_5_FM_FEEDBACK_C_MAP_OK=1 controls=256 malformed=saturate q14=0..16383\n");
    printf("WP5_5_FM_FEEDBACK_C_HISTORY_SURFACE_OK=1 history_values=65536 controls=256 pairs=16777216 min=-2147352576 max=2147287044\n");
    printf("WP5_5_FM_FEEDBACK_C_OP4_OK=1 renders=4096 delay_samples=1 final_history=13151\n");
    printf("WP5_5_FM_FEEDBACK_C_ALGORITHMS_OK=1 algorithms=4 renders_each=4096 op4_first=1 history=continuous\n");
    printf("WP5_5_FM_FEEDBACK_C_SWITCH_OK=1 renders=4096 switches=3 reset_on_switch=0 final_history=13151\n");
    printf("WP5_5_FM_FEEDBACK_C_EXTREME_OK=1 algorithms=4 renders_each=65536 output=q15_bounded final_history=2517\n");
    return 0;
}
