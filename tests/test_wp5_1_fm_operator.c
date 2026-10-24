/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_fm_operator.h"

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint32_t phase;
    int32_t sample;
} SineVector;

static int32_t u32_bits_to_i32(uint32_t x)
{
    if (x <= (uint32_t)INT32_MAX)
        return (int32_t)x;
    return -1 - (int32_t)(UINT32_MAX - x);
}

static void test_sine_vectors(void)
{
    static const SineVector vectors[] = {
        {0x00000000u, 0}, {0x08000000u, 6393}, {0x10000000u, 12539},
        {0x20000000u, 23170}, {0x30000000u, 30273}, {0x40000000u, 32767},
        {0x60000000u, 23170}, {0x7fffffffu, 0}, {0x80000000u, 0},
        {0xa0000000u, -23170}, {0xc0000000u, -32767},
        {0xe0000000u, -23170}, {0xffffffffu, 0},
        {0x01234567u, 915}, {0x05555555u, 4277}, {0x15555555u, 16383},
        {0x23456789u, 24953}, {0x55555555u, 28377}, {0x76543210u, 7705},
        {0x87654321u, -5915}, {0xabcdef01u, -28824}, {0xfedcba98u, -915}
    };
    uint32_t i;

    for (i = 0u; i < (uint32_t)(sizeof(vectors) / sizeof(vectors[0])); i++)
        assert(fmt_fm_operator_sine_q15(vectors[i].phase) == vectors[i].sample);
}

static void test_symmetry(void)
{
    uint32_t i;

    for (i = 0u; i < 65536u; i++) {
        uint32_t phase = i * 0x00010001u;
        int32_t a = fmt_fm_operator_sine_q15(phase);
        int32_t b = fmt_fm_operator_sine_q15(phase + 0x80000000u);
        assert(b == -a);
        assert(a >= -32767 && a <= 32767);
    }
}

static void test_level_vectors(void)
{
    static const uint32_t input[] = {
        0u, 1u, 8192u, 16384u, 24576u, 32766u, 32767u, 65535u
    };
    static const uint32_t sanitized[] = {
        0u, 1u, 8192u, 16384u, 24576u, 32766u, 32767u, 32767u
    };
    static const int32_t positive[] = {
        0, 1, 8192, 16384, 24575, 32765, 32767, 32767
    };
    uint32_t i;
    FmtFmOperator op;

    for (i = 0u; i < (uint32_t)(sizeof(input) / sizeof(input[0])); i++) {
        assert(fmt_fm_operator_clamp_level_q15(input[i]) == sanitized[i]);
        op.phase = 0x40000000u;
        assert(fmt_fm_operator_render(&op, 0u, 0, input[i]) == positive[i]);
        assert(op.phase == 0x40000000u);
        op.phase = 0xc0000000u;
        assert(fmt_fm_operator_render(&op, 0u, 0, input[i]) == -positive[i]);
    }
}

static void test_modulation_vectors(void)
{
    static const struct {
        uint32_t phase;
        int32_t modulation;
        int32_t sample;
    } vectors[] = {
        {0x10000000u, 0x10000000, 23170},
        {0x10000000u, -0x10000000, 0},
        {0x70000000u, 0x20000000, -12539},
        {0x00000000u, 0x40000000, 32767},
        {0x00000000u, -0x40000000, -32767},
        {0x00000000u, INT32_MIN, 0},
        {0x00000000u, INT32_MAX, 0}
    };
    FmtFmOperator op;
    uint32_t i;

    for (i = 0u; i < (uint32_t)(sizeof(vectors) / sizeof(vectors[0])); i++) {
        op.phase = vectors[i].phase;
        assert(fmt_fm_operator_render(&op, 0u, vectors[i].modulation, 32767u) ==
               vectors[i].sample);
        assert(op.phase == vectors[i].phase);
    }
}

static void test_state_and_render_order(void)
{
    static const int32_t samples[] = {0, 32767, 0, -32767};
    static const uint32_t phases[] = {
        0x40000000u, 0x80000000u, 0xc0000000u, 0x00000000u
    };
    FmtFmOperator op;
    uint32_t i;

    assert(sizeof(FmtFmOperator) == 4u);
    fmt_fm_operator_reset((FmtFmOperator *)0);
    fmt_fm_operator_retrigger((FmtFmOperator *)0);
    assert(fmt_fm_operator_render((FmtFmOperator *)0, 1u, 1, 1u) == 0);

    op.phase = 0xdeadbeefu;
    fmt_fm_operator_reset(&op);
    assert(op.phase == 0u);
    op.phase = 0x12345678u;
    fmt_fm_operator_retrigger(&op);
    assert(op.phase == 0u);

    for (i = 0u; i < 4u; i++) {
        assert(fmt_fm_operator_render(&op, 0x40000000u, 0, 32767u) == samples[i]);
        assert(op.phase == phases[i]);
    }

    op.phase = 0xf0000000u;
    (void)fmt_fm_operator_render(&op, 0x20000000u, 0, 32767u);
    assert(op.phase == 0x10000000u);

    op.phase = 0x20000000u;
    assert(fmt_fm_operator_render(&op, 0u, 0x10000000, 32767u) == 30273);
    assert(op.phase == 0x20000000u);
    assert(fmt_fm_operator_render(&op, 0u, -0x10000000, 32767u) == 12539);
    assert(op.phase == 0x20000000u);
}

static void write_long_vector(const char *path)
{
    static const int32_t first12[] = {
        0, 248, 437, 146, -609, -1277, -1165, -56, 1466, 2310, 1650, -361
    };
    FmtFmOperator op;
    FILE *f;
    uint32_t i;

    op.phase = 0x13579bdfu;
    f = fopen(path, "wb");
    assert(f != NULL);

    for (i = 0u; i < 4096u; i++) {
        uint32_t mod_u = i * 0x1f123bb5u;
        int32_t modulation = u32_bits_to_i32(mod_u);
        uint32_t level = (i * 257u) & 0x7fffu;
        int32_t sample = fmt_fm_operator_render(&op, 42852281u, modulation, level);
        uint32_t bits = (uint32_t)sample;
        unsigned char bytes[4];

        if (i < 12u)
            assert(sample == first12[i]);

        bytes[0] = (unsigned char)(bits & 0xffu);
        bytes[1] = (unsigned char)((bits >> 8) & 0xffu);
        bytes[2] = (unsigned char)((bits >> 16) & 0xffu);
        bytes[3] = (unsigned char)((bits >> 24) & 0xffu);
        assert(fwrite(bytes, 1u, 4u, f) == 4u);
    }

    assert(fclose(f) == 0);
    assert(op.phase == 0xf1532bdfu);
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    test_sine_vectors();
    test_symmetry();
    test_level_vectors();
    test_modulation_vectors();
    test_state_and_render_order();
    write_long_vector(argv[1]);
    puts("WP5_1_FM_OPERATOR_VECTORS_OK=1 state_bytes=4 symmetry_probes=65536");
    puts("WP5_1_FM_OPERATOR_RENDER_OK=1 sample_before_advance=1 modulation=full_int32 level_q15=0..32767");
    puts("WP5_1_FM_OPERATOR_LONG_OK=1 samples=4096 final_phase=f1532bdf");
    return 0;
}
