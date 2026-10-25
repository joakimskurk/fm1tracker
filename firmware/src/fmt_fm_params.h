/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_FM_PARAMS_H
#define FM1TRACKER_FMT_FM_PARAMS_H

#include <stdint.h>

#include "fmt_fm_engine.h"
#include "fmt_param_registry.h"

#define FMT_FM_PAGE_SLOTS 4u
#define FMT_FM_PAGE_COUNT 7u
#define FMT_FM_PARAM_COUNT 27u

#define FMT_FM_DEFAULT_ALGORITHM 128u
#define FMT_FM_DEFAULT_INDEX      64u
#define FMT_FM_DEFAULT_FEEDBACK    0u

enum {
    FMT_FM_PAGE_CORE = 0u,
    FMT_FM_PAGE_RATIO = 1u,
    FMT_FM_PAGE_LEVEL = 2u,
    FMT_FM_PAGE_ATTACK = 3u,
    FMT_FM_PAGE_DECAY = 4u,
    FMT_FM_PAGE_SUSTAIN = 5u,
    FMT_FM_PAGE_RELEASE = 6u
};

typedef struct {
    uint8_t algorithm;
    uint8_t index;
    uint8_t feedback;
    uint8_t ratio[4];
    uint8_t level[4];
    uint8_t attack[4];
    uint8_t decay[4];
    uint8_t sustain[4];
    uint8_t release[4];
} FmtFmParamSet;

#define FMT_FM_PARAM_STATIC_ASSERT(name, expr) \
    typedef char fmt_fm_param_static_assert_##name[(expr) ? 1 : -1]
FMT_FM_PARAM_STATIC_ASSERT(param_set_is_27, sizeof(FmtFmParamSet) == 27u);
FMT_FM_PARAM_STATIC_ASSERT(page_count_is_7, FMT_FM_PAGE_COUNT == 7u);
FMT_FM_PARAM_STATIC_ASSERT(algorithm_id_is_60, FMT_PARAM_FM_ALGORITHM == 0x60u);
FMT_FM_PARAM_STATIC_ASSERT(feedback_id_is_62, FMT_PARAM_FM_FEEDBACK == 0x62u);
FMT_FM_PARAM_STATIC_ASSERT(op1_ratio_id_is_64, FMT_PARAM_FM_OP1_RATIO == 0x64u);
FMT_FM_PARAM_STATIC_ASSERT(op4_release_id_is_7b, FMT_PARAM_FM_OP4_RELEASE == 0x7Bu);

void fmt_fm_param_set_defaults(FmtFmParamSet *set);
FmtParamId fmt_fm_page_param(uint8_t page, uint8_t slot);
int fmt_fm_param_value(const FmtFmParamSet *set,
                       FmtParamId id,
                       uint8_t *value_out);
int fmt_fm_param_set_value(FmtFmParamSet *set,
                           FmtParamId id,
                           uint8_t value);
int fmt_fm_page_value_from_set(uint8_t page,
                               uint8_t slot,
                               const FmtFmParamSet *set,
                               uint8_t *value_out);
void fmt_fm_params_to_patch(const FmtFmParamSet *set, FmtFmPatch *patch_out);

#endif /* FM1TRACKER_FMT_FM_PARAMS_H */