/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "fmt_engine_pitch.h"
#include "fmt_osc_triangle.h"

static int32_t sample_at(uint32_t phase)
{
    FmtTriangleOsc osc;
    osc.phase = phase;
    return fmt_triangle_render(&osc, 0u);
}

static void test_layout_reset_and_null(void)
{
    FmtTriangleOsc osc;

    assert(sizeof(FmtTriangleOsc) == 4u);
    assert(FMT_TRIANGLE_PEAK == 32767);

    osc.phase = 0xDEADBEEFu;
    fmt_triangle_reset(&osc);
    assert(osc.phase == 0u);
    assert(fmt_triangle_render(&osc, 0u) == 0);
    assert(osc.phase == 0u);

    osc.phase = 0x12345678u;
    fmt_triangle_retrigger(&osc);
    assert(osc.phase == 0u);

    fmt_triangle_reset((FmtTriangleOsc *)0);
    fmt_triangle_retrigger((FmtTriangleOsc *)0);
    assert(fmt_triangle_render((FmtTriangleOsc *)0, 0x12345678u) == 0);
}

static void test_canonical_and_boundaries(void)
{
    assert(sample_at(0x00000000u) == 0);
    assert(sample_at(0x40000000u) == 32767);
    assert(sample_at(0x80000000u) == 0);
    assert(sample_at(0xC0000000u) == -32767);

    assert(sample_at(0x3FFF8000u) == 32767);
    assert(sample_at(0x40000000u) == 32767);
    assert(sample_at(0x7FFF8000u) == 0);
    assert(sample_at(0x80000000u) == 0);
    assert(sample_at(0xBFFF8000u) == -32767);
    assert(sample_at(0xC0000000u) == -32767);
    assert(sample_at(0xFFFF8000u) == 0);
    assert(sample_at(0xFFFFFFFFu) == 0);
}

static void test_short_vectors(void)
{
    static const int32_t quarter[4] = {0, 32767, 0, -32767};
    static const int32_t eighth[8] = {
        0, 16384, 32767, 16383,
        0, -16384, -32767, -16383
    };
    FmtTriangleOsc osc;
    uint32_t i;

    fmt_triangle_reset(&osc);
    for (i = 0u; i < 4u; i++)
        assert(fmt_triangle_render(&osc, 0x40000000u) == quarter[i]);
    assert(osc.phase == 0u);

    fmt_triangle_reset(&osc);
    for (i = 0u; i < 8u; i++)
        assert(fmt_triangle_render(&osc, 0x20000000u) == eighth[i]);
    assert(osc.phase == 0u);
}

static void test_exhaustive_quantized_surface(void)
{
    uint32_t high16;
    int32_t min_sample = 32767;
    int32_t max_sample = -32767;

    for (high16 = 0u; high16 < 65536u; high16++) {
        uint32_t phase0 = high16 << 16;
        uint32_t phase1 = phase0 | 0x00008000u;
        int32_t s0 = sample_at(phase0);
        int32_t s1 = sample_at(phase1);
        int32_t h0 = sample_at(phase0 + 0x80000000u);
        int32_t h1 = sample_at(phase1 + 0x80000000u);

        assert(s0 >= -32767 && s0 <= 32767);
        assert(s1 >= -32767 && s1 <= 32767);
        assert(h0 == -s0);
        assert(h1 == -s1);

        if (s0 < min_sample) min_sample = s0;
        if (s1 < min_sample) min_sample = s1;
        if (s0 > max_sample) max_sample = s0;
        if (s1 > max_sample) max_sample = s1;
    }

    assert(min_sample == -32767);
    assert(max_sample == 32767);
}

static void test_zero_increment_and_wrap(void)
{
    FmtTriangleOsc osc;
    uint32_t i;

    osc.phase = 0x40000000u;
    for (i = 0u; i < 32u; i++)
        assert(fmt_triangle_render(&osc, 0u) == 32767);
    assert(osc.phase == 0x40000000u);

    osc.phase = 0xFFFF0000u;
    assert(fmt_triangle_render(&osc, 0x00020000u) == -1);
    assert(osc.phase == 0x00010000u);
    assert(fmt_triangle_render(&osc, 0u) == 2);
}

static void test_pitch_compatibility(void)
{
    static const struct {
        uint16_t pitch_q7;
        uint32_t inc;
    } vectors[] = {
        {0u, 796253u},
        {128u, 843601u},
        {7680u, 25480118u},
        {8832u, 42852281u},
        {8896u, 44126346u},
        {16256u, 1221665363u},
        {16383u, 1293741834u}
    };
    uint32_t i;

    for (i = 0u; i < (uint32_t)(sizeof vectors / sizeof vectors[0]); i++)
        assert(fmt_engine_pitch_inc_q7(vectors[i].pitch_q7) == vectors[i].inc);
}

static void test_guards_and_replay(void)
{
    struct {
        uint32_t before;
        FmtTriangleOsc osc;
        uint32_t after;
    } guarded;
    FmtTriangleOsc a, b;
    uint32_t inc = fmt_engine_pitch_inc_q7(8896u);
    uint32_t i;

    guarded.before = 0x13579BDFu;
    guarded.after = 0x2468ACE0u;
    fmt_triangle_reset(&guarded.osc);
    for (i = 0u; i < 4096u; i++)
        (void)fmt_triangle_render(&guarded.osc, inc);
    assert(guarded.before == 0x13579BDFu);
    assert(guarded.after == 0x2468ACE0u);

    fmt_triangle_reset(&a);
    fmt_triangle_reset(&b);
    for (i = 0u; i < 4096u; i++) {
        int32_t sa = fmt_triangle_render(&a, inc);
        int32_t sb = fmt_triangle_render(&b, inc);
        assert(sa == sb);
        assert(a.phase == b.phase);
    }
}

static uint32_t triangle_long_hash(FmtTriangleOsc *out)
{
    FmtTriangleOsc osc;
    uint32_t hash = 2166136261u;
    uint32_t inc = fmt_engine_pitch_inc_q7(8832u);
    uint32_t i;

    assert(inc == 42852281u);
    fmt_triangle_reset(&osc);
    for (i = 0u; i < 65536u; i++) {
        int32_t sample = fmt_triangle_render(&osc, inc);
        hash ^= (uint32_t)sample;
        hash *= 16777619u;
    }
    hash ^= osc.phase;
    hash *= 16777619u;
    if (out)
        *out = osc;
    return hash;
}

static void test_long_hash(void)
{
    FmtTriangleOsc final_state;

    assert(triangle_long_hash(&final_state) == 0xBC0F75EDu);
    assert(final_state.phase == 0xDFB90000u);
}

int main(void)
{
    test_layout_reset_and_null();
    test_canonical_and_boundaries();
    test_short_vectors();
    test_exhaustive_quantized_surface();
    test_zero_increment_and_wrap();
    test_pitch_compatibility();
    test_guards_and_replay();
    test_long_hash();

    puts("WP4_4_TRIANGLE_TESTS_OK=1");
    puts("state_bytes=4 q15=-32767..32767 canonical=0,+32767,0,-32767 phase_order=sample_before_advance");
    puts("quantized_phase_bins=131072 half_cycle_antisymmetry=1 eighth_vector=0,+16384,+32767,+16383,0,-16384,-32767,-16383");
    puts("long_hash_fnv1a32=bc0f75ed samples=65536 pitch_q7=8832 final_phase=dfb90000");
    return 0;
}
