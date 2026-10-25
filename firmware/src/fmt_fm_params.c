/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_fm_params.h"

#include <stddef.h>

static const FmtParamId fmt_fm_page_ids[FMT_FM_PAGE_COUNT][FMT_FM_PAGE_SLOTS] = {
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

void fmt_fm_param_set_defaults(FmtFmParamSet *set)
{
    static const uint8_t ratio[4] = { 16u, 80u, 16u, 80u };
    static const uint8_t level[4] = { 255u, 96u, 255u, 96u };
    uint32_t i;

    if (!set)
        return;

    set->algorithm = FMT_FM_DEFAULT_ALGORITHM;
    set->index = FMT_FM_DEFAULT_INDEX;
    set->feedback = FMT_FM_DEFAULT_FEEDBACK;
    for (i = 0u; i < 4u; i++) {
        set->ratio[i] = ratio[i];
        set->level[i] = level[i];
        set->attack[i] = 0u;
        set->decay[i] = 0u;
        set->sustain[i] = 255u;
        set->release[i] = 96u;
    }
}

FmtParamId fmt_fm_page_param(uint8_t page, uint8_t slot)
{
    if (page >= FMT_FM_PAGE_COUNT || slot >= FMT_FM_PAGE_SLOTS)
        return (FmtParamId)FMT_PARAM_NONE;
    return fmt_fm_page_ids[page][slot];
}

static uint8_t *fmt_fm_param_ptr(FmtFmParamSet *set, FmtParamId id)
{
    if (!set)
        return (uint8_t *)0;

    switch (id) {
    case FMT_PARAM_FM_ALGORITHM:
        return &set->algorithm;
    case FMT_PARAM_FM_INDEX:
        return &set->index;
    case FMT_PARAM_FM_FEEDBACK:
        return &set->feedback;
    default:
        break;
    }

    if (id >= FMT_PARAM_FM_OP1_RATIO && id <= FMT_PARAM_FM_OP4_RATIO)
        return &set->ratio[id - FMT_PARAM_FM_OP1_RATIO];
    if (id >= FMT_PARAM_FM_OP1_LEVEL && id <= FMT_PARAM_FM_OP4_LEVEL)
        return &set->level[id - FMT_PARAM_FM_OP1_LEVEL];
    if (id >= FMT_PARAM_FM_OP1_ATTACK && id <= FMT_PARAM_FM_OP4_ATTACK)
        return &set->attack[id - FMT_PARAM_FM_OP1_ATTACK];
    if (id >= FMT_PARAM_FM_OP1_DECAY && id <= FMT_PARAM_FM_OP4_DECAY)
        return &set->decay[id - FMT_PARAM_FM_OP1_DECAY];
    if (id >= FMT_PARAM_FM_OP1_SUSTAIN && id <= FMT_PARAM_FM_OP4_SUSTAIN)
        return &set->sustain[id - FMT_PARAM_FM_OP1_SUSTAIN];
    if (id >= FMT_PARAM_FM_OP1_RELEASE && id <= FMT_PARAM_FM_OP4_RELEASE)
        return &set->release[id - FMT_PARAM_FM_OP1_RELEASE];
    return (uint8_t *)0;
}

int fmt_fm_param_value(const FmtFmParamSet *set,
                       FmtParamId id,
                       uint8_t *value_out)
{
    const uint8_t *value;

    if (!value_out)
        return 0;
    value = fmt_fm_param_ptr((FmtFmParamSet *)(uintptr_t)set, id);
    if (!value)
        return 0;
    *value_out = *value;
    return 1;
}

int fmt_fm_param_set_value(FmtFmParamSet *set,
                           FmtParamId id,
                           uint8_t value)
{
    uint8_t *target = fmt_fm_param_ptr(set, id);

    if (!target)
        return 0;
    *target = value;
    return 1;
}

int fmt_fm_page_value_from_set(uint8_t page,
                               uint8_t slot,
                               const FmtFmParamSet *set,
                               uint8_t *value_out)
{
    FmtParamId id = fmt_fm_page_param(page, slot);

    if (id == FMT_PARAM_NONE)
        return 0;
    return fmt_fm_param_value(set, id, value_out);
}

void fmt_fm_params_to_patch(const FmtFmParamSet *set, FmtFmPatch *patch_out)
{
    uint32_t i;

    if (!set || !patch_out)
        return;

    for (i = 0u; i < 4u; i++) {
        patch_out->ratio_id[i] = (uint8_t)(set->ratio[i] >> 4);
        patch_out->level_control[i] = set->level[i];
        patch_out->envelope.attack[i] = set->attack[i];
        patch_out->envelope.decay[i] = set->decay[i];
        patch_out->envelope.sustain[i] = set->sustain[i];
        patch_out->envelope.release[i] = set->release[i];
    }
    patch_out->algorithm = (uint8_t)(set->algorithm >> 6);
    patch_out->index_control = set->index;
    patch_out->feedback_control = set->feedback;
    patch_out->reserved = 0u;
}
