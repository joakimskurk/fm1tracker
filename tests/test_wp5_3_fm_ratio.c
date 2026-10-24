/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "fmt_engine_pitch.h"
#include "fmt_fm_ratio.h"

static void fail(const char *what)
{
    fprintf(stderr, "WP5.3 ratio test failed: %s\n", what);
    exit(1);
}

static void expect_u32(const char *what, uint32_t actual, uint32_t expected)
{
    if (actual != expected) {
        fprintf(stderr, "%s: got %u expected %u\n",
                what, (unsigned)actual, (unsigned)expected);
        exit(1);
    }
}

static void write_le32(FILE *f, uint32_t value)
{
    unsigned char b[4];
    b[0] = (unsigned char)(value & 255u);
    b[1] = (unsigned char)((value >> 8) & 255u);
    b[2] = (unsigned char)((value >> 16) & 255u);
    b[3] = (unsigned char)((value >> 24) & 255u);
    if (fwrite(b, 1u, sizeof(b), f) != sizeof(b))
        fail("write exhaustive payload");
}

static void verify_table(void)
{
    static const uint16_t expected[FMT_FM_RATIO_COUNT] = {
        128u, 256u, 320u, 362u,
        384u, 512u, 640u, 768u,
        896u, 1024u, 1280u, 1536u,
        1792u, 2048u, 3072u, 4096u
    };
    uint32_t i;

    for (i = 0u; i < FMT_FM_RATIO_COUNT; ++i) {
        if (fmt_fm_ratio_sanitize(i) != i)
            fail("sanitize legal ID");
        if (fmt_fm_ratio_q8(i) != expected[i])
            fail("Q8 table");
    }

    if (fmt_fm_ratio_sanitize(16u) != FMT_FM_RATIO_UNITY ||
        fmt_fm_ratio_sanitize(255u) != FMT_FM_RATIO_UNITY ||
        fmt_fm_ratio_sanitize(0xffffffffu) != FMT_FM_RATIO_UNITY)
        fail("invalid ID fallback");

    if (fmt_fm_ratio_q8(0xffffffffu) != 256u)
        fail("invalid ID Q8 fallback");
}

static void verify_anchors(void)
{
    static const uint16_t pitches[6] = {0u, 128u, 7680u, 15360u, 16000u, 16383u};
    static const uint32_t bases[6] = {
        796253u, 843601u, 25480118u, 815363807u, 1088380105u, 1293741834u
    };
    static const uint8_t ids[7] = {0u, 1u, 3u, 5u, 9u, 13u, 15u};
    static const uint32_t expected[6][7] = {
        {398127u, 796253u, 1125952u, 1592506u, 3185012u, 6370024u, 12740048u},
        {421801u, 843601u, 1192905u, 1687202u, 3374404u, 6748808u, 13497616u},
        {12740059u, 25480118u, 36030479u, 50960236u, 101920472u, 203840944u, 407681888u},
        {407681904u, 815363807u, 1152975383u, 1630727614u, 2147483647u, 2147483647u, 2147483647u},
        {544190053u, 1088380105u, 1539037492u, 2147483647u, 2147483647u, 2147483647u, 2147483647u},
        {646870917u, 1293741834u, 1829431812u, 2147483647u, 2147483647u, 2147483647u, 2147483647u}
    };
    uint32_t p;
    uint32_t r;

    for (p = 0u; p < 6u; ++p) {
        uint32_t base = fmt_engine_pitch_inc_q7(pitches[p]);
        expect_u32("D038 base anchor", base, bases[p]);
        for (r = 0u; r < 7u; ++r)
            expect_u32("D051 ratio anchor",
                       fmt_fm_ratio_apply(base, ids[r]),
                       expected[p][r]);
    }
}

static void verify_map4(void)
{
    uint32_t out[4] = {0u, 0u, 0u, 0u};
    const uint8_t a[4] = {1u, 5u, 3u, 13u};
    const uint8_t b[4] = {0u, 4u, 9u, 15u};
    const uint8_t c[4] = {1u, 3u, 5u, 15u};

    fmt_fm_ratio_map4(25480118u, a, out);
    expect_u32("map4 a0", out[0], 25480118u);
    expect_u32("map4 a1", out[1], 50960236u);
    expect_u32("map4 a2", out[2], 36030479u);
    expect_u32("map4 a3", out[3], 203840944u);

    fmt_fm_ratio_map4(815363807u, b, out);
    expect_u32("map4 b0", out[0], 407681904u);
    expect_u32("map4 b1", out[1], 1223045711u);
    expect_u32("map4 b2", out[2], FMT_FM_RATIO_INCREMENT_CEILING);
    expect_u32("map4 b3", out[3], FMT_FM_RATIO_INCREMENT_CEILING);

    fmt_fm_ratio_map4(1293741834u, c, out);
    expect_u32("map4 c0", out[0], 1293741834u);
    expect_u32("map4 c1", out[1], 1829431812u);
    expect_u32("map4 c2", out[2], FMT_FM_RATIO_INCREMENT_CEILING);
    expect_u32("map4 c3", out[3], FMT_FM_RATIO_INCREMENT_CEILING);

    fmt_fm_ratio_map4(25480118u, NULL, out);
    expect_u32("map4 null 0", out[0], 25480118u);
    expect_u32("map4 null 1", out[1], 25480118u);
    expect_u32("map4 null 2", out[2], 25480118u);
    expect_u32("map4 null 3", out[3], 25480118u);

    fmt_fm_ratio_map4(25480118u, a, NULL);
}

static void verify_malformed(void)
{
    expect_u32("invalid ratio unity",
               fmt_fm_ratio_apply(25480118u, 0xffffffffu),
               25480118u);
    expect_u32("malformed base unity",
               fmt_fm_ratio_apply(0xffffffffu, FMT_FM_RATIO_UNITY),
               FMT_FM_RATIO_BASE_INCREMENT_MAX);
    expect_u32("malformed base half",
               fmt_fm_ratio_apply(0xffffffffu, FMT_FM_RATIO_HALF),
               646870917u);
    expect_u32("malformed base 16x",
               fmt_fm_ratio_apply(0xffffffffu, FMT_FM_RATIO_SIXTEEN),
               FMT_FM_RATIO_INCREMENT_CEILING);
}

static uint32_t write_exhaustive(const char *path)
{
    FILE *f = fopen(path, "wb");
    uint32_t pitch;
    uint32_t ratio;
    uint32_t clipped = 0u;

    if (!f)
        fail("open exhaustive payload");

    for (pitch = 0u; pitch <= FMT_ENGINE_PITCH_MAX_Q7; ++pitch) {
        uint32_t base = fmt_engine_pitch_inc_q7((uint16_t)pitch);
        for (ratio = 0u; ratio < FMT_FM_RATIO_COUNT; ++ratio) {
            uint32_t out = fmt_fm_ratio_apply(base, ratio);
            if (out == FMT_FM_RATIO_INCREMENT_CEILING)
                ++clipped;
            write_le32(f, out);
        }
    }

    if (fclose(f) != 0)
        fail("close exhaustive payload");
    return clipped;
}

int main(int argc, char **argv)
{
    uint32_t clipped;

    if (argc != 2)
        fail("expected output payload path");

    verify_table();
    verify_anchors();
    verify_map4();
    verify_malformed();
    clipped = write_exhaustive(argv[1]);
    expect_u32("exhaustive clipped count", clipped, 27615u);

    printf("WP5_3_FM_RATIO_C_ANCHORS_OK=1 ids=16 unity=1 map4=exact malformed=exact\n");
    printf("WP5_3_FM_RATIO_C_EXHAUSTIVE_OK=1 pitches=16384 ratios=16 outputs=262144 bytes=1048576 clipped=%u\n",
           (unsigned)clipped);
    return 0;
}
