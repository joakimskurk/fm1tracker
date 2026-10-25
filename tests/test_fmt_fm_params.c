/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_fm_params.h"

static void test_exact_pages(void)
{
    static const FmtParamId expected[FMT_FM_PAGE_COUNT][FMT_FM_PAGE_SLOTS] = {
        { FMT_PARAM_FM_ALGORITHM, FMT_PARAM_FM_INDEX,
          FMT_PARAM_FM_FEEDBACK, FMT_PARAM_NONE },
        { FMT_PARAM_FM_OP1_RATIO, FMT_PARAM_FM_OP2_RATIO,
          FMT_PARAM_FM_OP3_RATIO, FMT_PARAM_FM_OP4_RATIO },
        { FMT_PARAM_FM_OP1_LEVEL, FMT_PARAM_FM_OP2_LEVEL,
          FMT_PARAM_FM_OP3_LEVEL, FMT_PARAM_FM_OP4_LEVEL },
        { FMT_PARAM_FM_OP1_ATTACK, FMT_PARAM_FM_OP2_ATTACK,
          FMT_PARAM_FM_OP3_ATTACK, FMT_PARAM_FM_OP4_ATTACK },
        { FMT_PARAM_FM_OP1_DECAY, FMT_PARAM_FM_OP2_DECAY,
          FMT_PARAM_FM_OP3_DECAY, FMT_PARAM_FM_OP4_DECAY },
        { FMT_PARAM_FM_OP1_SUSTAIN, FMT_PARAM_FM_OP2_SUSTAIN,
          FMT_PARAM_FM_OP3_SUSTAIN, FMT_PARAM_FM_OP4_SUSTAIN },
        { FMT_PARAM_FM_OP1_RELEASE, FMT_PARAM_FM_OP2_RELEASE,
          FMT_PARAM_FM_OP3_RELEASE, FMT_PARAM_FM_OP4_RELEASE }
    };
    uint32_t page, slot;

    assert(FMT_FM_PAGE_COUNT == 7u);
    assert(FMT_FM_PAGE_SLOTS == 4u);
    for (page = 0u; page < FMT_FM_PAGE_COUNT; page++)
        for (slot = 0u; slot < FMT_FM_PAGE_SLOTS; slot++)
            assert(fmt_fm_page_param((uint8_t)page, (uint8_t)slot) ==
                   expected[page][slot]);

    assert(fmt_fm_page_param(FMT_FM_PAGE_COUNT, 0u) == FMT_PARAM_NONE);
    assert(fmt_fm_page_param(0u, FMT_FM_PAGE_SLOTS) == FMT_PARAM_NONE);
}

static void test_defaults_and_access(void)
{
    static const uint8_t ratio[4] = { 16u, 80u, 16u, 80u };
    static const uint8_t level[4] = { 255u, 96u, 255u, 96u };
    FmtFmParamSet set;
    uint32_t i;
    uint8_t value = 0u;

    assert(sizeof(FmtFmParamSet) == 27u);
    fmt_fm_param_set_defaults(&set);
    assert(set.algorithm == 128u);
    assert(set.index == 64u);
    assert(set.feedback == 0u);
    for (i = 0u; i < 4u; i++) {
        assert(set.ratio[i] == ratio[i]);
        assert(set.level[i] == level[i]);
        assert(set.attack[i] == 0u);
        assert(set.decay[i] == 0u);
        assert(set.sustain[i] == 255u);
        assert(set.release[i] == 96u);
    }

    assert(fmt_fm_param_value(&set, FMT_PARAM_FM_ALGORITHM, &value));
    assert(value == 128u);
    assert(fmt_fm_param_set_value(&set, FMT_PARAM_FM_OP3_DECAY, 77u));
    assert(fmt_fm_param_value(&set, FMT_PARAM_FM_OP3_DECAY, &value));
    assert(value == 77u);
    assert(!fmt_fm_param_value(&set, FMT_PARAM_NONE, &value));
    assert(!fmt_fm_param_value(&set, (FmtParamId)0x63u, &value));
    assert(!fmt_fm_param_set_value(&set, (FmtParamId)0x7Cu, 1u));
}

static void test_default_patch(void)
{
    static const uint8_t expected[28] = {
        1u, 5u, 1u, 5u,
        255u, 96u, 255u, 96u,
        0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u,
        255u, 255u, 255u, 255u,
        96u, 96u, 96u, 96u,
        2u, 64u, 0u, 0u
    };
    FmtFmParamSet set;
    FmtFmPatch patch;
    FILE *fp;

    fmt_fm_param_set_defaults(&set);
    memset(&patch, 0xA5, sizeof patch);
    fmt_fm_params_to_patch(&set, &patch);
    assert(sizeof patch == sizeof expected);
    assert(memcmp(&patch, expected, sizeof expected) == 0);

    fp = fopen("build/host/wp5_8_default_patch.bin", "wb");
    assert(fp != NULL);
    assert(fwrite(&patch, 1u, sizeof patch, fp) == sizeof patch);
    assert(fclose(fp) == 0);
}

int main(void)
{
    test_exact_pages();
    test_defaults_and_access();
    test_default_patch();
    printf("WP5_8_FM_PARAMS_OK=1 ids=27 pages=7 param_set=27 default_patch=exact\n");
    return 0;
}
