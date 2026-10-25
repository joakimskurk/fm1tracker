/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_8BIT_LINKED_PARAMS_H
#define FM1TRACKER_FMT_8BIT_LINKED_PARAMS_H

#include <stddef.h>
#include <stdint.h>

#include "fmt_8bit_params.h"
#include "fmt_lock_editor.h"
#include "fmt_model.h"

/* D056 generalizes the one D045 physical row-cache allocation. The first
 * 2,048 bytes remain the exact historical Fmt8BitRowParamCache view. */
typedef struct {
    Fmt8BitRowParamCache legacy;
    uint32_t bank0_high[FMT_8BIT_PARAM_TRACKS][FMT_8BIT_PARAM_ROWS];
    uint32_t bank1_low[FMT_8BIT_PARAM_TRACKS][FMT_8BIT_PARAM_ROWS];
    uint32_t bank1_high[FMT_8BIT_PARAM_TRACKS][FMT_8BIT_PARAM_ROWS];
    uint8_t active_bank[FMT_8BIT_PARAM_TRACKS][FMT_8BIT_PARAM_ROWS];
} FmtEngineRowParamCache;

extern Fmt8BitParamSet fmt_8bit_instrument_base[FMT_INSTRUMENTS];
extern volatile FmtEngineRowParamCache fmt_engine_row_param_cache;

/* Source-compatible D045 view. There is no second legacy mirror allocation. */
#define fmt_8bit_row_param_cache (fmt_engine_row_param_cache.legacy)

FMT_STATIC_ASSERT(bit8_base_bank_is_96,
                  sizeof(fmt_8bit_instrument_base) == 96u);
FMT_STATIC_ASSERT(bit8_linked_row_cache_is_2048,
                  sizeof(fmt_8bit_row_param_cache) == 2048u);
FMT_STATIC_ASSERT(engine_row_cache_is_8704,
                  sizeof(FmtEngineRowParamCache) == 8704u);
FMT_STATIC_ASSERT(engine_row_cache_legacy_at_zero,
                  offsetof(FmtEngineRowParamCache, legacy) == 0u);
FMT_STATIC_ASSERT(engine_row_cache_bank0_high_at_2048,
                  offsetof(FmtEngineRowParamCache, bank0_high) == 2048u);
FMT_STATIC_ASSERT(engine_row_cache_bank1_low_at_4096,
                  offsetof(FmtEngineRowParamCache, bank1_low) == 4096u);
FMT_STATIC_ASSERT(engine_row_cache_bank1_high_at_6144,
                  offsetof(FmtEngineRowParamCache, bank1_high) == 6144u);
FMT_STATIC_ASSERT(engine_row_cache_selector_at_8192,
                  offsetof(FmtEngineRowParamCache, active_bank) == 8192u);

/* Startup/main only. Reset initializes all 16 bases to frozen D044 defaults and
 * clears the complete generalized publication allocation. */
void fmt_8bit_linked_params_reset(void);
void fmt_8bit_linked_params_rebuild(void);
int fmt_8bit_linked_params_rebuild_track(uint32_t track);
int fmt_8bit_linked_params_refresh_track_row(uint32_t track, uint32_t row);
uint32_t fmt_8bit_linked_params_read(uint32_t track, uint32_t row);

int fmt_8bit_linked_params_set_base(uint32_t instrument,
                                    const Fmt8BitParamSet *base);
const Fmt8BitParamSet *fmt_8bit_linked_params_base(uint32_t instrument);

int fmt_8bit_linked_editor_apply_delta(FmtLockEditor *editor,
                                       uint32_t knob,
                                       int32_t delta);
int fmt_8bit_linked_editor_confirm_delete(FmtLockEditor *editor);

#endif /* FM1TRACKER_FMT_8BIT_LINKED_PARAMS_H */