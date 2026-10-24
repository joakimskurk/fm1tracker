/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_TRACKER_EVENT_H
#define FM1TRACKER_FMT_TRACKER_EVENT_H

#include <stdint.h>

#include "fmt_tracker_effect.h"

enum {
    FMT_TRACKER_ROW_ACTION_NONE = 0u,
    FMT_TRACKER_ROW_ACTION_DELAYED_NOTE = 1u,
    FMT_TRACKER_ROW_ACTION_CUT = 2u,
    FMT_TRACKER_ROW_ACTION_RETRIG = 3u
};

typedef struct {
    uint8_t command;
    uint8_t value;
    uint8_t pending_note;
    uint8_t pending_valid;
} FmtTrackerRowEventState;

FMT_STATIC_ASSERT(tracker_row_event_state_is_4,
                  sizeof(FmtTrackerRowEventState) == 4u);

void fmt_tracker_row_event_reset(FmtTrackerRowEventState *state);

/* D033 row-start filter. The original D025 event is returned unchanged unless
 * NOTE_DELAY 1..5 owns an ordinary NOTE, in which case only the immediate
 * application is suppressed to NONE and the NOTE is retained in fixed state. */
void fmt_tracker_row_event_begin(FmtTrackerRowEventState *state,
                                 uint8_t event_kind,
                                 uint8_t event_note,
                                 uint32_t effect_word,
                                 uint8_t *immediate_kind,
                                 uint8_t *immediate_note);

/* Evaluate exactly one D032 tick position (0..5). At most one action can be
 * returned. DELAYED_NOTE consumes pending_valid. Values >5 never schedule. */
uint8_t fmt_tracker_row_event_action(FmtTrackerRowEventState *state,
                                     uint8_t tick,
                                     uint8_t logical_active,
                                     uint8_t *note_out);

#endif /* FM1TRACKER_FMT_TRACKER_EVENT_H */
