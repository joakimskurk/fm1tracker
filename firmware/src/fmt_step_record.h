/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_STEP_RECORD_H
#define FM1TRACKER_FMT_STEP_RECORD_H

#include <stdint.h>

#include "fmt_pattern_input.h"
#include "fmt_store.h"
#include "fmt_transport.h"

#define FMT_STEP_RECORD_PATTERN 0u

typedef struct {
    uint8_t active;
    uint8_t gesture_latched;
} FmtStepRecord;

FMT_STATIC_ASSERT(step_record_state_size_is_2, sizeof(FmtStepRecord) == 2u);

void fmt_step_record_reset(FmtStepRecord *record);
int fmt_step_record_enter(FmtStepRecord *record,
                          FmtPatternInput *input,
                          const FmtTransport *transport);
void fmt_step_record_exit(FmtStepRecord *record);
int fmt_step_record_is_active(const FmtStepRecord *record);
void fmt_step_record_release_gesture(FmtStepRecord *record);

/* D024 monophonic chord rule. From a physical key-down bitmask, return the
 * lowest valid FM-1 note-key index 0..26. Bits above the physical key span are
 * ignored. Output is reset to 0 on rejection. */
int fmt_step_record_lowest_key(uint32_t down_mask, uint8_t *key_index);

/* D024 note-only write. Returns 1 only when exactly one note byte was written
 * and the cursor advanced one global step. No other FmtStep field is touched. */
int fmt_step_record_write_note(FmtStepRecord *record,
                               FmtPatternInput *input,
                               const FmtTransport *transport,
                               uint32_t note);

#endif /* FM1TRACKER_FMT_STEP_RECORD_H */
