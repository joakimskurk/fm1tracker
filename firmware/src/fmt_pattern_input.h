/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_PATTERN_INPUT_H
#define FM1TRACKER_FMT_PATTERN_INPUT_H

#include <stdint.h>

#define FMT_PATTERN_INPUT_PAGE_COUNT      4u
#define FMT_PATTERN_INPUT_STEPS_PER_PAGE 16u
#define FMT_PATTERN_INPUT_KEY_COUNT      27u

#define FMT_PATTERN_CONTEXT_PERFORMANCE 0u
#define FMT_PATTERN_CONTEXT_PATTERN     1u

typedef struct {
    uint8_t context;
    uint8_t page;
    uint8_t local_step;
} FmtPatternInput;

void fmt_pattern_input_reset(FmtPatternInput *input);
void fmt_pattern_input_enter_pattern(FmtPatternInput *input);
void fmt_pattern_input_enter_performance(FmtPatternInput *input);
int fmt_pattern_input_is_pattern(const FmtPatternInput *input);

/* Returns 1 and writes local step 0..15 for one of the sixteen physical white
 * note keys. Black keys and indexes outside the FM-1's 27-key span return 0. */
int fmt_pattern_input_key_to_local(uint32_t key_index, uint8_t *local_step);

/* Pattern-context selection only. Performance context and black/invalid keys
 * are rejected without changing cursor state. */
int fmt_pattern_input_select_key(FmtPatternInput *input, uint32_t key_index);

/* D022 page navigation is clamped, not wrapped. A successful move preserves
 * local_step and therefore changes the derived global step by exactly 16. */
int fmt_pattern_input_page_prev(FmtPatternInput *input);
int fmt_pattern_input_page_next(FmtPatternInput *input);

/* Always returns 0..63 for a valid/reset state. Null is safely treated as 0. */
uint8_t fmt_pattern_input_global_step(const FmtPatternInput *input);

#endif /* FM1TRACKER_FMT_PATTERN_INPUT_H */
