/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "fmt_engine_pitch.h"
#include "fmt_osc_pulse.h"

static void test_layout_reset_and_null(void)
{
    FmtPulseOsc osc;

    assert(sizeof(FmtPulseOsc) == 4u);
    assert(FMT_PULSE_WIDTH_MIN == 32u);
    assert(FMT_PULSE_WIDTH_DEFAULT == 128u);
    assert(FMT_PULSE_WIDTH_MAX == 224u);
    assert(FMT_PULSE_SAMPLE_HIGH == 32767);
    assert(FMT_PULSE_SAMPLE_LOW == -32767);

    osc.phase = 0xDEADBEEFu;
    fmt_pulse_reset(&osc);
    assert(osc.phase == 0u);

    osc.phase = 0x12345678u;
    fmt_pulse_retrigger(&osc);
    assert(osc.phase == 0u);

    fmt_pulse_reset((FmtPulseOsc *)0);
    fmt_pulse_retrigger((FmtPulseOsc *)0);
    assert(fmt_pulse_render((FmtPulseOsc *)0, 0x12345678u, 128u) == 0);
}

static void test_width_clamp(void)
{
    assert(fmt_pulse_clamp_width(0u) == 32u);
    assert(fmt_pulse_clamp_width(31u) == 32u);
    assert(fmt_pulse_clamp_width(32u) == 32u);
    assert(fmt_pulse_clamp_width(64u) == 64u);
    assert(fmt_pulse_clamp_width(128u) == 128u);
    assert(fmt_pulse_clamp_width(192u) == 192u);
    assert(fmt_pulse_clamp_width(224u) == 224u);
    assert(fmt_pulse_clamp_width(225u) == 224u);
    assert(fmt_pulse_clamp_width(255u) == 224u);
    assert(fmt_pulse_clamp_width(UINT32_MAX) == 224u);
}

static void assert_vector(uint8_t width, const int32_t expected[4])
{
    FmtPulseOsc osc;
    uint32_t i;

    fmt_pulse_reset(&osc);
    for (i = 0u; i < 4u; i++)
        assert(fmt_pulse_render(&osc, 0x40000000u, width) == expected[i]);
    assert(osc.phase == 0u);
}

static void test_reference_vectors(void)
{
    static const int32_t q25[4] = {32767, -32767, -32767, -32767};
    static const int32_t q50[4] = {32767, 32767, -32767, -32767};
    static const int32_t q75[4] = {32767, 32767, 32767, -32767};
    FmtPulseOsc osc;

    assert_vector(64u, q25);
    assert_vector(128u, q50);
    assert_vector(192u, q75);

    fmt_pulse_reset(&osc);
    assert(fmt_pulse_render(&osc, 0u, 32u) == 32767);
    assert(osc.phase == 0u);
    assert(fmt_pulse_render(&osc, 0u, 224u) == 32767);
    assert(osc.phase == 0u);
}

static void test_wrap_retrigger_and_guards(void)
{
    struct {
        uint32_t before;
        FmtPulseOsc osc;
        uint32_t after;
    } guarded;
    int32_t sample;

    guarded.before = 0x13579BDFu;
    guarded.after = 0x2468ACE0u;
    guarded.osc.phase = 0xF0000000u;
    sample = fmt_pulse_render(&guarded.osc, 0x30000000u, 128u);
    assert(sample == -32767);
    assert(guarded.osc.phase == 0x20000000u);
    assert(guarded.before == 0x13579BDFu);
    assert(guarded.after == 0x2468ACE0u);

    fmt_pulse_retrigger(&guarded.osc);
    assert(guarded.osc.phase == 0u);
    assert(fmt_pulse_render(&guarded.osc, 1u, 224u) == 32767);
    assert(guarded.osc.phase == 1u);
    assert(guarded.before == 0x13579BDFu);
    assert(guarded.after == 0x2468ACE0u);
}

static void test_pitch_vectors_and_domain(void)
{
    static const struct {
        uint16_t pitch;
        uint32_t increment;
    } vectors[] = {
        {0u, 796253u},
        {128u, 843601u},
        {7680u, 25480118u},
        {8832u, 42852281u},
        {8896u, 44126346u},
        {16256u, 1221665363u},
        {16383u, 1293741834u}
    };
    uint32_t i, previous;

    for (i = 0u; i < (uint32_t)(sizeof vectors / sizeof vectors[0]); i++)
        assert(fmt_engine_pitch_inc_q7(vectors[i].pitch) == vectors[i].increment);

    assert(fmt_engine_pitch_inc_q7(16384u) == 1293741834u);
    assert(fmt_engine_pitch_inc_q7(UINT16_MAX) == 1293741834u);

    previous = fmt_engine_pitch_inc_q7(0u);
    assert(previous != 0u);
    for (i = 1u; i <= FMT_ENGINE_PITCH_MAX_Q7; i++) {
        uint32_t current = fmt_engine_pitch_inc_q7((uint16_t)i);
        assert(current > previous);
        previous = current;
    }

    previous = fmt_engine_pitch_inc_q7(0u);
    for (i = 1u; i <= 127u; i++) {
        uint32_t current = fmt_engine_pitch_inc_q7((uint16_t)(i << 7));
        assert(current > previous);
        previous = current;
    }
}

static uint32_t pulse_long_hash(void)
{
    FmtPulseOsc osc;
    uint32_t hash = 2166136261u;
    uint32_t inc = fmt_engine_pitch_inc_q7(8832u);
    uint32_t i;

    fmt_pulse_reset(&osc);
    for (i = 0u; i < 65536u; i++) {
        int32_t sample = fmt_pulse_render(&osc, inc, 96u);
        hash ^= (uint32_t)sample;
        hash *= 16777619u;
    }
    hash ^= osc.phase;
    hash *= 16777619u;
    assert(osc.phase == 0xDFB90000u);
    return hash;
}

static void test_repeatability_and_long_hash(void)
{
    FmtPulseOsc a, b;
    uint32_t inc = fmt_engine_pitch_inc_q7(8896u);
    uint32_t i;

    fmt_pulse_reset(&a);
    fmt_pulse_reset(&b);
    for (i = 0u; i < 4096u; i++) {
        int32_t sa = fmt_pulse_render(&a, inc, 173u);
        int32_t sb = fmt_pulse_render(&b, inc, 173u);
        assert(sa == sb);
        assert(a.phase == b.phase);
    }

    assert(pulse_long_hash() == 0x8AB03EE1u);
}

int main(void)
{
    test_layout_reset_and_null();
    test_width_clamp();
    test_reference_vectors();
    test_wrap_retrigger_and_guards();
    test_pitch_vectors_and_domain();
    test_repeatability_and_long_hash();

    puts("WP4_2_PULSE_TESTS_OK=1");
    puts("state_bytes=4 width=32..224 phase_order=sample_before_advance hard_edge=1");
    puts("pitch_q7=0..16383 strict_monotonic=1 max_inc=1293741834");
    puts("long_hash_fnv1a32=8ab03ee1 samples=65536 pitch_q7=8832 width=96 final_phase=dfb90000");
    return 0;
}
