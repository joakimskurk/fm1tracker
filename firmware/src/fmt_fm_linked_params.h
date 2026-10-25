/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_FM_LINKED_PARAMS_H
#define FM1TRACKER_FMT_FM_LINKED_PARAMS_H

#include <stdint.h>

#include "fmt_8bit_linked_params.h"
#include "fmt_fm_params.h"
#include "fmt_model.h"

#define FMT_FM_MAX_ROW_OVERRIDES 4u

typedef struct {
    FmtParamId id;
    uint8_t value;
} FmtFmOverride;

typedef struct {
    FmtFmOverride slot[FMT_FM_MAX_ROW_OVERRIDES];
} FmtFmRowOverrides;

extern FmtFmParamSet fmt_fm_instrument_base[FMT_INSTRUMENTS];

FMT_STATIC_ASSERT(fm_override_is_2, sizeof(FmtFmOverride) == 2u);
FMT_STATIC_ASSERT(fm_row_overrides_is_8, sizeof(FmtFmRowOverrides) == 8u);
FMT_STATIC_ASSERT(fm_base_bank_is_432,
                  sizeof(fmt_fm_instrument_base) == 432u);

/* Startup/main only. Base reset does not allocate another row cache. */
void fmt_fm_linked_params_reset(void);
int fmt_fm_linked_params_set_base(uint32_t instrument,
                                  const FmtFmParamSet *base);
const FmtFmParamSet *fmt_fm_linked_params_base(uint32_t instrument);

/* Main-context publication. Input order may be arbitrary; successful publish
 * sorts ascending semantic ID, rejects duplicates/non-FM IDs, writes the
 * inactive low/high words, then flips exactly one byte of selector state. */
int fmt_fm_linked_params_publish(uint32_t track,
                                 uint32_t row,
                                 const FmtFmOverride *overrides,
                                 uint8_t count);

/* Row-boundary reader. The active selector is sampled exactly once. */
int fmt_fm_linked_params_read(uint32_t track,
                              uint32_t row,
                              FmtFmRowOverrides *out);

/* Bounded composition from one normalized instrument base plus at most four
 * already-published overrides. No lock/store/editor traversal occurs here. */
int fmt_fm_linked_params_compose(uint32_t instrument,
                                 const FmtFmRowOverrides *overrides,
                                 FmtFmPatch *patch_out);

#endif /* FM1TRACKER_FMT_FM_LINKED_PARAMS_H */