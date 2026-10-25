/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "fmt_fm_envelope.h"

static void fail(const char *what)
{
    fprintf(stderr, "WP5.6 FM envelope test failed: %s\n", what);
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

static void verify_shape_and_anchors(void)
{
    expect_u32("state size", sizeof(FmtFmEnvelopeState), 20u);
    expect_u32("controls size", sizeof(FmtFmEnvelopeControls), 16u);
    expect_u32("stage off", FMT_FM_ENVELOPE_STAGE_OFF, 0u);
    expect_u32("stage attack", FMT_FM_ENVELOPE_STAGE_ATTACK, 1u);
    expect_u32("stage decay", FMT_FM_ENVELOPE_STAGE_DECAY, 2u);
    expect_u32("stage sustain", FMT_FM_ENVELOPE_STAGE_SUSTAIN, 3u);
    expect_u32("stage release", FMT_FM_ENVELOPE_STAGE_RELEASE, 4u);
    expect_u32("level max q15.16", FMT_FM_ENVELOPE_LEVEL_MAX_Q15_16,
               0x7fff0000u);

    expect_u32("sanitize 0", fmt_fm_envelope_control_sanitize(0u), 0u);
    expect_u32("sanitize 255", fmt_fm_envelope_control_sanitize(255u), 255u);
    expect_u32("sanitize 256", fmt_fm_envelope_control_sanitize(256u), 255u);
    expect_u32("sanitize max", fmt_fm_envelope_control_sanitize(0xffffffffu),
               255u);

    expect_u32("sustain 0", fmt_fm_envelope_sustain_q15(0u), 0u);
    expect_u32("sustain 1", fmt_fm_envelope_sustain_q15(1u), 128u);
    expect_u32("sustain 127", fmt_fm_envelope_sustain_q15(127u), 16319u);
    expect_u32("sustain 128", fmt_fm_envelope_sustain_q15(128u), 16448u);
    expect_u32("sustain 255", fmt_fm_envelope_sustain_q15(255u), 32767u);
    expect_u32("sustain malformed", fmt_fm_envelope_sustain_q15(0xffffffffu),
               32767u);

    expect_u32("time 0", fmt_fm_envelope_time_step_q15_16(0u), 0u);
    expect_u32("time 1", fmt_fm_envelope_time_step_q15_16(1u), 48804958u);
    expect_u32("time 16", fmt_fm_envelope_time_step_q15_16(16u), 25263743u);
    expect_u32("time 32", fmt_fm_envelope_time_step_q15_16(32u), 12558001u);
    expect_u32("time 64", fmt_fm_envelope_time_step_q15_16(64u), 3148707u);
    expect_u32("time 96", fmt_fm_envelope_time_step_q15_16(96u), 787177u);
    expect_u32("time 127", fmt_fm_envelope_time_step_q15_16(127u), 203354u);
    expect_u32("time 128", fmt_fm_envelope_time_step_q15_16(128u), 196795u);
    expect_u32("time 160", fmt_fm_envelope_time_step_q15_16(160u), 49199u);
    expect_u32("time 192", fmt_fm_envelope_time_step_q15_16(192u), 12300u);
    expect_u32("time 224", fmt_fm_envelope_time_step_q15_16(224u), 3075u);
    expect_u32("time 254", fmt_fm_envelope_time_step_q15_16(254u), 822u);
    expect_u32("time 255", fmt_fm_envelope_time_step_q15_16(255u), 795u);
    expect_u32("time malformed", fmt_fm_envelope_time_step_q15_16(0xffffffffu),
               795u);
}

static void write_time_steps(const char *path)
{
    FILE *f = fopen(path, "wb");
    uint32_t control;
    uint32_t previous = 0u;

    if (!f)
        fail("open time step payload");

    for (control = 0u; control <= 255u; ++control) {
        uint32_t step = fmt_fm_envelope_time_step_q15_16(control);
        if (control > 1u && previous <= step)
            fail("time step monotonicity");
        if (control > 0u)
            previous = step;
        write_le32(f, step);
    }

    if (fclose(f) != 0)
        fail("close time step payload");
}

static void write_sustain_map(const char *path)
{
    FILE *f = fopen(path, "wb");
    uint32_t control;

    if (!f)
        fail("open sustain payload");

    for (control = 0u; control <= 255u; ++control)
        write_le16(f, fmt_fm_envelope_sustain_q15(control));

    if (fclose(f) != 0)
        fail("close sustain payload");
}

static void write_effective_map(const char *path)
{
    FILE *f = fopen(path, "wb");
    uint32_t static_control;
    uint32_t envelope_control;

    if (!f)
        fail("open effective-level payload");

    for (static_control = 0u; static_control <= 255u; ++static_control) {
        uint32_t static_level = fmt_fm_envelope_sustain_q15(static_control);
        for (envelope_control = 0u; envelope_control <= 255u;
             ++envelope_control) {
            uint32_t envelope_level =
                fmt_fm_envelope_sustain_q15(envelope_control);
            uint16_t out = fmt_fm_envelope_effective_level_q15(
                static_level, envelope_level);
            if (out > 32767u)
                fail("effective level range");
            write_le16(f, out);
        }
    }

    if (fclose(f) != 0)
        fail("close effective-level payload");

    expect_u32("full envelope identity max",
               fmt_fm_envelope_effective_level_q15(32767u, 32767u), 32767u);
    expect_u32("full envelope identity mid",
               fmt_fm_envelope_effective_level_q15(16448u, 32767u), 16448u);
    expect_u32("effective malformed clamp",
               fmt_fm_envelope_effective_level_q15(0xffffffffu, 0xffffffffu),
               32767u);
}

static FmtFmEnvelopeControls zero_controls(void)
{
    FmtFmEnvelopeControls controls = {
        {0u, 0u, 0u, 0u},
        {0u, 0u, 0u, 0u},
        {0u, 0u, 0u, 0u},
        {0u, 0u, 0u, 0u}
    };
    return controls;
}

static void verify_zero_retrigger_and_nulls(void)
{
    FmtFmEnvelopeControls controls = {
        {0u, 0u, 1u, 32u},
        {0u, 64u, 0u, 96u},
        {64u, 128u, 192u, 255u},
        {0u, 1u, 64u, 255u}
    };
    FmtFmEnvelopeState state;
    uint16_t static_levels[4] = {32767u, 24672u, 16448u, 8224u};
    uint16_t out[4] = {1u, 1u, 1u, 1u};
    uint32_t i;

    fmt_fm_envelope_reset(&state);
    expect_u32("reset op1 level", state.level_q15_16[0], 0u);
    expect_u32("reset op4 stage", state.stage[3], FMT_FM_ENVELOPE_STAGE_OFF);

    fmt_fm_envelope_retrigger(&state, &controls);
    expect_u32("retrigger op1 stage", state.stage[0], FMT_FM_ENVELOPE_STAGE_SUSTAIN);
    expect_u32("retrigger op2 stage", state.stage[1], FMT_FM_ENVELOPE_STAGE_DECAY);
    expect_u32("retrigger op3 stage", state.stage[2], FMT_FM_ENVELOPE_STAGE_ATTACK);
    expect_u32("retrigger op4 stage", state.stage[3], FMT_FM_ENVELOPE_STAGE_ATTACK);
    expect_u32("retrigger op1 level", fmt_fm_envelope_level_q15(&state, 0u), 8224u);
    expect_u32("retrigger op2 level", fmt_fm_envelope_level_q15(&state, 1u), 32767u);
    expect_u32("retrigger op3 level", fmt_fm_envelope_level_q15(&state, 2u), 0u);
    expect_u32("retrigger op4 level", fmt_fm_envelope_level_q15(&state, 3u), 0u);

    for (i = 0u; i < 37u; ++i)
        fmt_fm_envelope_advance(&state, &controls);
    fmt_fm_envelope_retrigger(&state, &controls);
    expect_u32("hard retrigger op3 level", fmt_fm_envelope_level_q15(&state, 2u), 0u);
    expect_u32("hard retrigger op4 level", fmt_fm_envelope_level_q15(&state, 3u), 0u);

    fmt_fm_envelope_note_off(&state, &controls);
    expect_u32("release0 off", state.stage[0], FMT_FM_ENVELOPE_STAGE_OFF);
    expect_u32("release op2", state.stage[1], FMT_FM_ENVELOPE_STAGE_RELEASE);
    expect_u32("zero attack op3 off", state.stage[2], FMT_FM_ENVELOPE_STAGE_OFF);
    expect_u32("zero attack op4 off", state.stage[3], FMT_FM_ENVELOPE_STAGE_OFF);

    state.stage[0] = 99u;
    state.level_q15_16[0] = 0xffffffffu;
    fmt_fm_envelope_advance(&state, &controls);
    expect_u32("corrupt stage off", state.stage[0], FMT_FM_ENVELOPE_STAGE_OFF);
    expect_u32("corrupt stage zero", state.level_q15_16[0], 0u);

    state.level_q15_16[1] = 0xffffffffu;
    expect_u32("read clamps corrupt level", fmt_fm_envelope_level_q15(&state, 1u),
               32767u);
    expect_u32("invalid operator read", fmt_fm_envelope_level_q15(&state, 4u), 0u);
    expect_u32("null state read", fmt_fm_envelope_level_q15(NULL, 0u), 0u);

    fmt_fm_envelope_map_levels(NULL, static_levels, out);
    expect_u32("null state map op1", out[0], 0u);
    expect_u32("null state map op4", out[3], 0u);
    out[0] = 1u; out[1] = 1u; out[2] = 1u; out[3] = 1u;
    fmt_fm_envelope_map_levels(&state, NULL, out);
    expect_u32("null static map op1", out[0], 0u);
    expect_u32("null static map op4", out[3], 0u);
    fmt_fm_envelope_map_levels(&state, static_levels, NULL);

    fmt_fm_envelope_retrigger(&state, NULL);
    expect_u32("null controls retrigger off", state.stage[1], FMT_FM_ENVELOPE_STAGE_OFF);
    expect_u32("null controls retrigger zero", state.level_q15_16[1], 0u);

    state.level_q15_16[0] = FMT_FM_ENVELOPE_LEVEL_MAX_Q15_16;
    state.stage[0] = FMT_FM_ENVELOPE_STAGE_SUSTAIN;
    fmt_fm_envelope_note_off(&state, NULL);
    expect_u32("null controls off stage", state.stage[0], FMT_FM_ENVELOPE_STAGE_OFF);
    expect_u32("null controls off level", state.level_q15_16[0], 0u);

    state.level_q15_16[0] = FMT_FM_ENVELOPE_LEVEL_MAX_Q15_16;
    state.stage[0] = FMT_FM_ENVELOPE_STAGE_SUSTAIN;
    fmt_fm_envelope_advance(&state, NULL);
    expect_u32("null controls advance stage", state.stage[0], FMT_FM_ENVELOPE_STAGE_OFF);
    expect_u32("null controls advance level", state.level_q15_16[0], 0u);

    fmt_fm_envelope_reset(NULL);
    fmt_fm_envelope_retrigger(NULL, &controls);
    fmt_fm_envelope_note_off(NULL, &controls);
    fmt_fm_envelope_advance(NULL, &controls);

    printf("WP5_6_FM_ENVELOPE_C_SEMANTICS_OK=1 hard_retrigger=zero attack0=instant decay0=instant release0=instant corrupt_stage=off nulls=safe\n");
}

static uint32_t verify_max_release(void)
{
    FmtFmEnvelopeControls controls = zero_controls();
    FmtFmEnvelopeState state;
    uint32_t count = 0u;

    controls.release[0] = 255u;
    fmt_fm_envelope_reset(&state);
    state.level_q15_16[0] = FMT_FM_ENVELOPE_LEVEL_MAX_Q15_16;
    state.stage[0] = FMT_FM_ENVELOPE_STAGE_SUSTAIN;
    fmt_fm_envelope_note_off(&state, &controls);
    expect_u32("max release starts release", state.stage[0],
               FMT_FM_ENVELOPE_STAGE_RELEASE);

    while (state.stage[0] != FMT_FM_ENVELOPE_STAGE_OFF) {
        fmt_fm_envelope_advance(&state, &controls);
        ++count;
        if (count > 2701155u)
            fail("max release failed to terminate");
    }

    expect_u32("max release count", count, 2701155u);
    expect_u32("max release zero", state.level_q15_16[0], 0u);
    return count;
}

static void write_stress(const char *path)
{
    FmtFmEnvelopeControls controls = {
        {1u, 32u, 64u, 0u},
        {64u, 96u, 128u, 0u},
        {255u, 192u, 128u, 64u},
        {96u, 128u, 160u, 0u}
    };
    const uint16_t static_levels[4] = {32767u, 24672u, 16448u, 8224u};
    FmtFmEnvelopeState state;
    uint16_t effective[4];
    FILE *f = fopen(path, "wb");
    uint32_t sample;
    uint32_t op;

    if (!f)
        fail("open stress payload");

    fmt_fm_envelope_reset(&state);
    fmt_fm_envelope_retrigger(&state, &controls);

    for (sample = 0u; sample < 65536u; ++sample) {
        if (sample == 32768u)
            fmt_fm_envelope_note_off(&state, &controls);

        fmt_fm_envelope_map_levels(&state, static_levels, effective);
        for (op = 0u; op < 4u; ++op)
            write_le16(f, effective[op]);
        if (fwrite(state.stage, 1u, 4u, f) != 4u)
            fail("write stress stages");

        fmt_fm_envelope_advance(&state, &controls);
    }

    if (fclose(f) != 0)
        fail("close stress payload");

    for (op = 0u; op < 4u; ++op) {
        expect_u32("stress final level", state.level_q15_16[op], 0u);
        expect_u32("stress final stage", state.stage[op],
                   FMT_FM_ENVELOPE_STAGE_OFF);
    }
}

int main(int argc, char **argv)
{
    uint32_t max_release_steps;

    if (argc != 5)
        fail("expected time, sustain, effective and stress output paths");

    verify_shape_and_anchors();
    write_time_steps(argv[1]);
    write_sustain_map(argv[2]);
    write_effective_map(argv[3]);
    verify_zero_retrigger_and_nulls();
    max_release_steps = verify_max_release();
    write_stress(argv[4]);

    printf("WP5_6_FM_ENVELOPE_C_SHAPE_OK=1 operators=4 state_bytes=20 controls_bytes=16 stages=5\n");
    printf("WP5_6_FM_ENVELOPE_C_MAX_RELEASE_OK=1 control=255 advances=%u\n",
           (unsigned)max_release_steps);
    printf("WP5_6_FM_ENVELOPE_C_STRESS_OK=1 samples=65536 final=off\n");
    return 0;
}
