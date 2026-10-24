/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "fmt_engine_pitch.h"
#include "fmt_osc_wavetable8.h"

static const int8_t expected_tables[8][32] = {
    {0,25,49,71,90,106,117,125,127,125,117,106,90,71,49,25,0,-25,-49,-71,-90,-106,-117,-125,-127,-125,-117,-106,-90,-71,-49,-25},
    {0,3,16,27,28,33,48,57,56,64,83,87,79,96,127,103,0,-103,-127,-96,-79,-87,-83,-64,-56,-57,-48,-33,-28,-27,-16,-3},
    {0,127,97,115,102,113,103,112,103,112,103,113,102,115,97,127,0,-127,-97,-115,-102,-113,-103,-112,-103,-112,-103,-113,-102,-115,-97,-127},
    {0,75,103,107,123,127,101,81,98,110,80,35,15,1,-28,-36,0,36,28,-1,-15,-35,-80,-110,-98,-81,-101,-127,-123,-107,-103,-75},
    {0,43,73,97,121,127,90,27,-4,27,90,127,121,97,73,43,0,-43,-73,-97,-121,-127,-90,-27,4,-27,-90,-127,-121,-97,-73,-43},
    {0,78,123,127,108,87,70,52,34,24,29,38,35,15,-8,-14,0,14,8,-15,-35,-38,-29,-24,-34,-52,-70,-87,-108,-127,-123,-78},
    {0,127,79,26,3,10,102,118,74,118,102,10,3,26,79,127,0,-127,-79,-26,-3,-10,-102,-118,-74,-118,-102,-10,-3,-26,-79,-127},
    {0,99,124,122,123,126,117,84,72,117,127,86,50,1,-53,-58,0,58,53,-1,-50,-86,-127,-117,-72,-84,-117,-126,-123,-122,-124,-99}
};

static const uint32_t expected_long_hash[8] = {
    0x40A7DDABu, 0x7C509A67u, 0xA573D449u, 0x1D6F9FCBu,
    0xB8048A69u, 0xA3661307u, 0x27204689u, 0x48F2989Fu
};

static void test_layout_reset_and_null(void)
{
    FmtWavetable8Osc osc;
    uint32_t table;

    assert(sizeof(FmtWavetable8Osc) == 4u);
    assert(FMT_WAVETABLE8_TABLE_COUNT == 8u);
    assert(FMT_WAVETABLE8_TABLE_LENGTH == 32u);
    assert(FMT_WAVETABLE8_INDEX_SHIFT == 27u);
    assert(FMT_WAVETABLE8_Q15_SCALE == 258);

    osc.phase = 0xDEADBEEFu;
    fmt_wavetable8_reset(&osc);
    assert(osc.phase == 0u);
    for (table = 0u; table < 8u; table++) {
        assert(fmt_wavetable8_render(&osc, 0u, table) == 0);
        assert(osc.phase == 0u);
    }

    osc.phase = 0x12345678u;
    fmt_wavetable8_retrigger(&osc);
    assert(osc.phase == 0u);

    fmt_wavetable8_reset((FmtWavetable8Osc *)0);
    fmt_wavetable8_retrigger((FmtWavetable8Osc *)0);
    assert(fmt_wavetable8_render((FmtWavetable8Osc *)0, 1u, 0u) == 0);
}

static void test_selector_domain(void)
{
    static const uint32_t invalid[] = {8u, 9u, 15u, 255u, 65535u, UINT32_MAX};
    uint32_t i;
    uint32_t index;

    for (i = 0u; i < 8u; i++)
        assert(fmt_wavetable8_sanitize_table(i) == i);
    for (i = 0u; i < (uint32_t)(sizeof invalid / sizeof invalid[0]); i++)
        assert(fmt_wavetable8_sanitize_table(invalid[i]) == 0u);

    for (i = 0u; i < (uint32_t)(sizeof invalid / sizeof invalid[0]); i++) {
        for (index = 0u; index < 32u; index += 5u) {
            FmtWavetable8Osc a, b;
            a.phase = index << 27;
            b.phase = a.phase;
            assert(fmt_wavetable8_render(&a, 0u, invalid[i]) ==
                   fmt_wavetable8_render(&b, 0u, 0u));
            assert(a.phase == b.phase);
        }
    }
}

static void test_all_table_samples(void)
{
    uint32_t table;
    uint32_t index;
    int saw_pos_peak = 0;
    int saw_neg_peak = 0;

    for (table = 0u; table < 8u; table++) {
        FmtWavetable8Osc osc;
        fmt_wavetable8_reset(&osc);
        for (index = 0u; index < 32u; index++) {
            int32_t raw = expected_tables[table][index];
            int32_t expected = raw * 258;
            int32_t actual = fmt_wavetable8_render(&osc, 0x08000000u, table);
            assert(raw >= -127 && raw <= 127);
            assert(raw != -128);
            assert(actual == expected);
            if (actual == 32766) saw_pos_peak = 1;
            if (actual == -32766) saw_neg_peak = 1;
        }
        assert(osc.phase == 0u);
    }
    assert(saw_pos_peak);
    assert(saw_neg_peak);
}

static void test_zero_increment_and_wrap(void)
{
    FmtWavetable8Osc osc;
    uint32_t i;

    osc.phase = 8u << 27;
    for (i = 0u; i < 32u; i++)
        assert(fmt_wavetable8_render(&osc, 0u, FMT_WAVETABLE8_SINE) == 127 * 258);
    assert(osc.phase == (8u << 27));

    osc.phase = 31u << 27;
    assert(fmt_wavetable8_render(&osc, 0x08000000u, FMT_WAVETABLE8_SINE) == -25 * 258);
    assert(osc.phase == 0u);
    assert(fmt_wavetable8_render(&osc, 0u, FMT_WAVETABLE8_SINE) == 0);
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
        FmtWavetable8Osc osc;
        uint32_t after;
    } guarded;
    FmtWavetable8Osc a, b;
    uint32_t inc = fmt_engine_pitch_inc_q7(8896u);
    uint32_t i;

    guarded.before = 0x13579BDFu;
    guarded.after = 0x2468ACE0u;
    fmt_wavetable8_reset(&guarded.osc);
    for (i = 0u; i < 4096u; i++)
        (void)fmt_wavetable8_render(&guarded.osc, inc, FMT_WAVETABLE8_METAL);
    assert(guarded.before == 0x13579BDFu);
    assert(guarded.after == 0x2468ACE0u);

    fmt_wavetable8_reset(&a);
    fmt_wavetable8_reset(&b);
    for (i = 0u; i < 4096u; i++) {
        int32_t sa = fmt_wavetable8_render(&a, inc, FMT_WAVETABLE8_DIGI);
        int32_t sb = fmt_wavetable8_render(&b, inc, FMT_WAVETABLE8_DIGI);
        assert(sa == sb);
        assert(a.phase == b.phase);
    }
}

static uint32_t table_long_hash(uint32_t table, FmtWavetable8Osc *out)
{
    FmtWavetable8Osc osc;
    uint32_t hash = 2166136261u;
    uint32_t inc = fmt_engine_pitch_inc_q7(8832u);
    uint32_t i;

    assert(inc == 42852281u);
    fmt_wavetable8_reset(&osc);
    for (i = 0u; i < 65536u; i++) {
        int32_t sample = fmt_wavetable8_render(&osc, inc, table);
        hash ^= (uint32_t)sample;
        hash *= 16777619u;
    }
    hash ^= osc.phase;
    hash *= 16777619u;
    if (out)
        *out = osc;
    return hash;
}

static void test_long_hashes(void)
{
    uint32_t table;

    for (table = 0u; table < 8u; table++) {
        FmtWavetable8Osc final_state;
        assert(table_long_hash(table, &final_state) == expected_long_hash[table]);
        assert(final_state.phase == 0xDFB90000u);
    }
}

int main(void)
{
    test_layout_reset_and_null();
    test_selector_domain();
    test_all_table_samples();
    test_zero_increment_and_wrap();
    test_pitch_compatibility();
    test_guards_and_replay();
    test_long_hashes();

    puts("WP4_5_WAVETABLE8_TESTS_OK=1");
    puts("state_bytes=4 tables=8 length=32 table_bytes=256 sample=int8 q15_scale=258 interpolation=none");
    puts("all_table_samples=256 selector_fallback=sine phase_index=top5 phase_order=sample_before_advance");
    puts("long_hashes=40a7ddab,7c509a67,a573d449,1d6f9fcb,b8048a69,a3661307,27204689,48f2989f final_phase=dfb90000");
    return 0;
}
