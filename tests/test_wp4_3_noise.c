/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "fmt_engine_pitch.h"
#include "fmt_osc_noise.h"

static void test_layout_reset_and_null(void)
{
    FmtNoiseOsc osc;

    assert(sizeof(FmtNoiseOsc) == 8u);
    assert(FMT_NOISE_LFSR_MASK == 0x7FFFu);
    assert(FMT_NOISE_SEED == 0x5A3Du);
    assert(FMT_NOISE_SAMPLE_HIGH == 32767);
    assert(FMT_NOISE_SAMPLE_LOW == -32767);
    assert(FMT_NOISE_CLOCK_SHIFT == 4u);

    osc.phase = 0xDEADBEEFu;
    osc.lfsr = 0x1234u;
    fmt_noise_reset(&osc);
    assert(osc.phase == 0u);
    assert(osc.lfsr == FMT_NOISE_SEED);

    osc.phase = 0x12345678u;
    osc.lfsr = 1u;
    fmt_noise_retrigger(&osc);
    assert(osc.phase == 0u);
    assert(osc.lfsr == FMT_NOISE_SEED);

    fmt_noise_reset((FmtNoiseOsc *)0);
    fmt_noise_retrigger((FmtNoiseOsc *)0);
    assert(fmt_noise_render((FmtNoiseOsc *)0, 0x12345678u) == 0);
}

static void test_clock_vectors_and_domain(void)
{
    static const struct {
        uint16_t pitch_q7;
        uint32_t pitch_inc;
        uint32_t noise_inc;
    } vectors[] = {
        {0u, 796253u, 12740048u},
        {7680u, 25480118u, 407681888u},
        {8832u, 42852281u, 685636496u},
        {8896u, 44126346u, 706021536u},
        {16256u, 1221665363u, UINT32_MAX},
        {16383u, 1293741834u, UINT32_MAX}
    };
    uint32_t i;
    uint32_t previous;
    int saturated = 0;

    assert(fmt_noise_clock_inc(0u) == 0u);
    assert(fmt_noise_clock_inc(UINT32_MAX >> 4) == 0xFFFFFFF0u);
    assert(fmt_noise_clock_inc((UINT32_MAX >> 4) + 1u) == UINT32_MAX);
    assert(fmt_noise_clock_inc(UINT32_MAX) == UINT32_MAX);

    for (i = 0u; i < (uint32_t)(sizeof vectors / sizeof vectors[0]); i++) {
        uint32_t pitch_inc = fmt_engine_pitch_inc_q7(vectors[i].pitch_q7);
        assert(pitch_inc == vectors[i].pitch_inc);
        assert(fmt_noise_clock_inc(pitch_inc) == vectors[i].noise_inc);
    }

    previous = fmt_noise_clock_inc(fmt_engine_pitch_inc_q7(0u));
    for (i = 1u; i <= FMT_ENGINE_PITCH_MAX_Q7; i++) {
        uint32_t current = fmt_noise_clock_inc(fmt_engine_pitch_inc_q7((uint16_t)i));
        assert(current >= previous);
        if (previous == UINT32_MAX)
            assert(current == UINT32_MAX);
        if (current == UINT32_MAX)
            saturated = 1;
        previous = current;
    }
    assert(saturated);
}

static void test_lfsr_vector_and_period(void)
{
    static const uint32_t states[] = {
        0x5A3Du, 0x6D1Eu, 0x768Fu, 0x3B47u,
        0x1DA3u, 0x0ED1u, 0x4768u, 0x23B4u
    };
    FmtNoiseOsc osc;
    uint32_t i;

    fmt_noise_reset(&osc);
    for (i = 0u; i < (uint32_t)(sizeof states / sizeof states[0]); i++) {
        int32_t expected = (states[i] & 1u) ? 32767 : -32767;
        assert(osc.lfsr == states[i]);
        osc.phase = UINT32_MAX;
        assert(fmt_noise_render(&osc, 1u) == expected);
        assert(osc.phase == 0u);
        assert(osc.lfsr != 0u);
        assert((osc.lfsr & ~FMT_NOISE_LFSR_MASK) == 0u);
    }

    fmt_noise_reset(&osc);
    for (i = 0u; i < 32767u; i++) {
        osc.phase = UINT32_MAX;
        (void)fmt_noise_render(&osc, 1u);
        assert(osc.lfsr != 0u);
        if (i + 1u < 32767u)
            assert(osc.lfsr != FMT_NOISE_SEED);
    }
    assert(osc.lfsr == FMT_NOISE_SEED);
}

static void test_render_order_wrap_and_zero(void)
{
    static const int32_t half_clock[16] = {
        32767, 32767, -32767, -32767,
        32767, 32767, 32767, 32767,
        32767, 32767, 32767, 32767,
        -32767, -32767, -32767, -32767
    };
    FmtNoiseOsc osc;
    uint32_t i;
    uint32_t seed;

    fmt_noise_reset(&osc);
    seed = osc.lfsr;
    for (i = 0u; i < 32u; i++)
        assert(fmt_noise_render(&osc, 0u) == 32767);
    assert(osc.phase == 0u);
    assert(osc.lfsr == seed);

    fmt_noise_reset(&osc);
    for (i = 0u; i < 16u; i++)
        assert(fmt_noise_render(&osc, 0x80000000u) == half_clock[i]);
    assert(osc.phase == 0u);
    assert(osc.lfsr == 0x11DAu);

    fmt_noise_reset(&osc);
    osc.phase = 0x10000000u;
    seed = osc.lfsr;
    assert(fmt_noise_render(&osc, 0x10000000u) == 32767);
    assert(osc.phase == 0x20000000u);
    assert(osc.lfsr == seed);

    osc.phase = 0xF0000000u;
    assert(fmt_noise_render(&osc, 0x20000000u) == 32767);
    assert(osc.phase == 0x10000000u);
    assert(osc.lfsr == 0x6D1Eu);

    fmt_noise_reset(&osc);
    assert(fmt_noise_render(&osc, UINT32_MAX) == 32767);
    assert(osc.phase == UINT32_MAX);
    assert(osc.lfsr == FMT_NOISE_SEED);
    assert(fmt_noise_render(&osc, UINT32_MAX) == 32767);
    assert(osc.phase == UINT32_MAX - 1u);
    assert(osc.lfsr == 0x6D1Eu);
    assert(fmt_noise_render(&osc, UINT32_MAX) == -32767);
    assert(osc.phase == UINT32_MAX - 2u);
    assert(osc.lfsr == 0x768Fu);
}

static void test_guards_and_replay(void)
{
    struct {
        uint32_t before;
        FmtNoiseOsc osc;
        uint32_t after;
    } guarded;
    FmtNoiseOsc a, b;
    uint32_t inc = fmt_noise_clock_inc(fmt_engine_pitch_inc_q7(8896u));
    uint32_t i;

    guarded.before = 0x13579BDFu;
    guarded.after = 0x2468ACE0u;
    fmt_noise_reset(&guarded.osc);
    for (i = 0u; i < 4096u; i++)
        (void)fmt_noise_render(&guarded.osc, inc);
    assert(guarded.before == 0x13579BDFu);
    assert(guarded.after == 0x2468ACE0u);

    fmt_noise_reset(&a);
    fmt_noise_reset(&b);
    for (i = 0u; i < 4096u; i++) {
        int32_t sa = fmt_noise_render(&a, inc);
        int32_t sb = fmt_noise_render(&b, inc);
        assert(sa == sb);
        assert(a.phase == b.phase);
        assert(a.lfsr == b.lfsr);
    }
}

static uint32_t noise_long_hash(FmtNoiseOsc *out)
{
    FmtNoiseOsc osc;
    uint32_t hash = 2166136261u;
    uint32_t pitch_inc = fmt_engine_pitch_inc_q7(8832u);
    uint32_t inc = fmt_noise_clock_inc(pitch_inc);
    uint32_t i;

    assert(pitch_inc == 42852281u);
    assert(inc == 685636496u);
    fmt_noise_reset(&osc);
    for (i = 0u; i < 65536u; i++) {
        int32_t sample = fmt_noise_render(&osc, inc);
        hash ^= (uint32_t)sample;
        hash *= 16777619u;
    }
    hash ^= osc.phase;
    hash *= 16777619u;
    hash ^= osc.lfsr;
    hash *= 16777619u;
    if (out)
        *out = osc;
    return hash;
}

static void test_long_hash(void)
{
    FmtNoiseOsc final_state;

    assert(noise_long_hash(&final_state) == 0xF50385B6u);
    assert(final_state.phase == 0xFB900000u);
    assert(final_state.lfsr == 0x5DC1u);
}

int main(void)
{
    test_layout_reset_and_null();
    test_clock_vectors_and_domain();
    test_lfsr_vector_and_period();
    test_render_order_wrap_and_zero();
    test_guards_and_replay();
    test_long_hash();

    puts("WP4_3_NOISE_TESTS_OK=1");
    puts("state_bytes=8 seed=5a3d lfsr_period=32767 output=bit0_q15 max_steps_per_frame=1");
    puts("clock=pitch_inc_x16_saturating q7=0..16383 half_clock_vector=++--++++++++----");
    puts("long_hash_fnv1a32=f50385b6 samples=65536 pitch_q7=8832 final_phase=fb900000 final_lfsr=5dc1");
    return 0;
}
