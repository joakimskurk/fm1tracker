/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_STORE_H
#define FM1TRACKER_FMT_STORE_H

#include <stdint.h>

#include "fmt_model.h"

/* Reset the one editable V0.1 song to deterministic empty defaults.
 * This is bounded startup/editor work, never audio-ISR work. */
void fmt_store_reset(void);

/* Narrow access to the one runtime song. Pattern/step accessors reject every
 * out-of-range index and return null rather than performing pointer arithmetic
 * outside the fixed-capacity model. */
FmtSong *fmt_store_song(void);
const FmtSong *fmt_store_song_const(void);
FmtPattern *fmt_store_pattern(uint32_t pattern);
const FmtPattern *fmt_store_pattern_const(uint32_t pattern);
FmtStep *fmt_store_step(uint32_t pattern, uint32_t track, uint32_t step);
const FmtStep *fmt_store_step_const(uint32_t pattern, uint32_t track, uint32_t step);

#endif /* FM1TRACKER_FMT_STORE_H */
