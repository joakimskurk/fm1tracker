/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "fmt_fm_controls.h"

static void fail(const char *what)
{
    fprintf(stderr, "WP5.4 FM controls test failed: %s\n", what);
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

static void write_le16(FILE *f, uint16_t value)
{
    unsigned char b[2];
    b[0] = (unsigned char)(value & 255u);
    b[1] = (unsigned char)((value >> 8) & 255u);
    if (fwrite(b, 1u, sizeof(b), f) != sizeof(b))
        fail("write uint16 payload");
}

static void write_le32(FILE *f, uint32_t value)
{
    unsigned char b[4];
    b[0] = (unsigned char)(value & 255u);
    b[1] = (unsigned char)((value >> 8) & 255u);
    b[2] = (unsigned char)((value >> 16) & 255u);
    b[3] = (unsigned char)((value >> 24) & 255u);
    if (fwrite(b, 1u, sizeof(b), f) != sizeof(b))
        fail("write uint32 payload");
}

static void verify_anchors(void)
{
    static const uint8_t level_control[] = {
        0u, 1u, 32u, 64u, 96u, 127u, 128u, 160u, 192u, 224u, 254u, 255u
    };
    static const uint16_t level_expected[] = {
        0u, 128u, 4112u, 8224u, 12336u, 16319u,
        16448u, 20560u, 24672u, 28784u, 32639u, 32767u
    };
    static const uint8_t index_control[] = {
        0u, 1u, 3u, 4u, 32u, 64u, 96u, 127u, 128u, 160u, 192u, 224u, 254u, 255u
    };
    static const uint16_t index_expected[] = {
        0u, 64u, 192u, 257u, 2056u, 4112u, 6168u,
        8159u, 8224u, 10280u, 12336u, 14392u, 16319u, 16383u
    };
    uint32_t i;

    if (sizeof(FmtFmMappedControls) != 10u)
        fail("mapped controls struct size");

    for (i = 0u; i < sizeof(level_control) / sizeof(level_control[0]); ++i)
        expect_u32("LEVEL anchor", fmt_fm_level_q15(level_control[i]), level_expected[i]);

    for (i = 0u; i < sizeof(index_control) / sizeof(index_control[0]); ++i)
        expect_u32("INDEX anchor", fmt_fm_index_q14(index_control[i]), index_expected[i]);

    expect_u32("sanitize zero", fmt_fm_control_sanitize(0u), 0u);
    expect_u32("sanitize max", fmt_fm_control_sanitize(255u), 255u);
    expect_u32("sanitize 256", fmt_fm_control_sanitize(256u), 255u);
    expect_u32("sanitize 257", fmt_fm_control_sanitize(257u), 255u);
    expect_u32("sanitize 65535", fmt_fm_control_sanitize(65535u), 255u);
    expect_u32("sanitize uint32 max", fmt_fm_control_sanitize(0xffffffffu), 255u);
    expect_u32("malformed LEVEL", fmt_fm_level_q15(0xffffffffu), 32767u);
    expect_u32("malformed INDEX", fmt_fm_index_q14(0xffffffffu), 16383u);
}

static void verify_map(void)
{
    const uint8_t a[4] = {255u, 192u, 128u, 64u};
    const uint8_t b[4] = {64u, 128u, 192u, 255u};
    FmtFmMappedControls out;

    fmt_fm_controls_map(a, 128u, &out);
    expect_u32("map a op1", out.level_q15[0], 32767u);
    expect_u32("map a op2", out.level_q15[1], 24672u);
    expect_u32("map a op3", out.level_q15[2], 16448u);
    expect_u32("map a op4", out.level_q15[3], 8224u);
    expect_u32("map a index", out.mod_depth_q14, 8224u);

    fmt_fm_controls_map(b, 255u, &out);
    expect_u32("map b op1", out.level_q15[0], 8224u);
    expect_u32("map b op2", out.level_q15[1], 16448u);
    expect_u32("map b op3", out.level_q15[2], 24672u);
    expect_u32("map b op4", out.level_q15[3], 32767u);
    expect_u32("map b index", out.mod_depth_q14, 16383u);

    out.level_q15[0] = 1u;
    out.level_q15[1] = 1u;
    out.level_q15[2] = 1u;
    out.level_q15[3] = 1u;
    out.mod_depth_q14 = 1u;
    fmt_fm_controls_map(NULL, 64u, &out);
    expect_u32("null levels op1", out.level_q15[0], 0u);
    expect_u32("null levels op2", out.level_q15[1], 0u);
    expect_u32("null levels op3", out.level_q15[2], 0u);
    expect_u32("null levels op4", out.level_q15[3], 0u);
    expect_u32("null levels index", out.mod_depth_q14, 4112u);

    fmt_fm_controls_map(a, 128u, NULL);
}

static void write_mappings(const char *path)
{
    FILE *f = fopen(path, "wb");
    uint32_t v;

    if (!f)
        fail("open mapping payload");

    for (v = 0u; v <= 255u; ++v)
        write_le16(f, fmt_fm_level_q15(v));
    for (v = 0u; v <= 255u; ++v)
        write_le16(f, fmt_fm_index_q14(v));

    if (fclose(f) != 0)
        fail("close mapping payload");
}

static uint32_t d049_peak(uint16_t level_q15)
{
    if (level_q15 == 0u)
        return 0u;
    if (level_q15 >= 32767u)
        return 32767u;
    return (32767u * (uint32_t)level_q15 + 16384u) >> 15;
}

static uint32_t write_edge_surface(const char *path)
{
    FILE *f = fopen(path, "wb");
    uint32_t level_control;
    uint32_t index_control;
    uint32_t max_edge = 0u;

    if (!f)
        fail("open edge payload");

    for (level_control = 0u; level_control <= 255u; ++level_control) {
        uint32_t sample = d049_peak(fmt_fm_level_q15(level_control));
        for (index_control = 0u; index_control <= 255u; ++index_control) {
            uint32_t depth = fmt_fm_index_q14(index_control);
            uint32_t edge = sample * depth * 4u;
            if (edge > 2147483647u)
                fail("signed-32 edge overflow");
            if (edge > max_edge)
                max_edge = edge;
            write_le32(f, edge);
        }
    }

    if (fclose(f) != 0)
        fail("close edge payload");
    return max_edge;
}

static void write_composition(const char *path)
{
    static const uint32_t increments[3][4] = {
        {25480118u, 50960236u, 36030479u, 203840944u},
        {407681904u, 1223045711u, 2147483647u, 2147483647u},
        {796253u, 1592506u, 1125952u, 6370024u}
    };
    static const uint8_t levels[3][4] = {
        {255u, 192u, 128u, 64u},
        {64u, 128u, 192u, 255u},
        {0u, 64u, 128u, 255u}
    };
    static const uint8_t indices[3] = {128u, 255u, 64u};
    FILE *f = fopen(path, "wb");
    uint32_t vector;

    if (!f)
        fail("open composition payload");

    for (vector = 0u; vector < 3u; ++vector) {
        FmtFmMappedControls mapped;
        uint32_t op;
        fmt_fm_controls_map(levels[vector], indices[vector], &mapped);
        for (op = 0u; op < 4u; ++op)
            write_le32(f, increments[vector][op]);
        for (op = 0u; op < 4u; ++op)
            write_le16(f, mapped.level_q15[op]);
        write_le16(f, mapped.mod_depth_q14);
    }

    if (fclose(f) != 0)
        fail("close composition payload");
}

int main(int argc, char **argv)
{
    uint32_t max_edge;

    if (argc != 4)
        fail("expected mapping, edge and composition output paths");

    verify_anchors();
    verify_map();
    write_mappings(argv[1]);
    max_edge = write_edge_surface(argv[2]);
    write_composition(argv[3]);

    expect_u32("edge maximum", max_edge, 2147287044u);
    expect_u32("edge margin", 2147483647u - max_edge, 196603u);

    printf("WP5_4_FM_CONTROLS_C_MAP_OK=1 controls=256 struct_bytes=10 malformed=saturate null_levels=silence\n");
    printf("WP5_4_FM_CONTROLS_C_EDGE_OK=1 pairs=65536 max=%u int32_margin=%u\n",
           (unsigned)max_edge, (unsigned)(2147483647u - max_edge));
    printf("WP5_4_FM_CONTROLS_C_COMPOSITION_OK=1 vectors=3 bytes=78\n");
    return 0;
}
