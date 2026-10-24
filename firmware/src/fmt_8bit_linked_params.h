/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_8BIT_LINKED_PARAMS_H
#define FM1TRACKER_FMT_8BIT_LINKED_PARAMS_H

#include <stdint.h>

#include "fmt_8bit_params.h"
#include "fmt_lock_editor.h"
#include "fmt_model.h"

/* D045 Gate 3 main-owned state. These exact objects are linked into the app in
 * Gate 4; Gate 3 verifies them in isolation first. */
extern Fmt8BitParamSet fmt_8bit_instrument_base[FMT_INSTRUMENTS];
extern volatile Fmt8BitRowParamCache fmt_8bit_row_param_cache;

FMT_STATIC_ASSERT(bit8_base_bank_is_96,
                  sizeof(fmt_8bit_instrument_base) == 96u);
FMT_STATIC_ASSERT(bit8_linked_row_cache_is_2048,
                  sizeof(fmt_8bit_row_param_cache) == 2048u);

/* Startup/main only. Reset initializes all 16 bases to frozen D044 defaults and
 * clears publication words. Rebuild traverses fixed pattern-0 rows/locks. */
void fmt_8bit_linked_params_reset(void);
void fmt_8bit_linked_params_rebuild(void);
int fmt_8bit_linked_params_rebuild_track(uint32_t track);
int fmt_8bit_linked_params_refresh_track_row(uint32_t track, uint32_t row);
uint32_t fmt_8bit_linked_params_read(uint32_t track, uint32_t row);

/* Base mutation is stopped-transport/test/startup work only. A successful set
 * rebuilds the bounded publication cache immediately so no stale base-derived
 * row word survives main-context publication. */
int fmt_8bit_linked_params_set_base(uint32_t instrument,
                                    const Fmt8BitParamSet *base);
const Fmt8BitParamSet *fmt_8bit_linked_params_base(uint32_t instrument);

/* Gate 3 D029/D044 adapter. It reconstructs the selected row's NOTE-only
 * instrument context with at most one 64-row scan, maps K1..4 through that
 * engine's D044 page, delegates mutation to fmt_lock_editor_apply_param_delta,
 * then refreshes D031 + D044 for the affected track-row on successful writes.
 * No D032 refresh occurs because effect bytes are not p-lock storage. */
int fmt_8bit_linked_editor_apply_delta(FmtLockEditor *editor,
                                       uint32_t knob,
                                       int32_t delta);

/* D030 browse/arm grammar stays generic. Gate 3 uses this confirmation wrapper
 * so a successful deletion refreshes both D031 and D044 publication words. */
int fmt_8bit_linked_editor_confirm_delete(FmtLockEditor *editor);

#endif /* FM1TRACKER_FMT_8BIT_LINKED_PARAMS_H */