/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_PLAYBACK_H
#define FM1TRACKER_FMT_PLAYBACK_H

#include <stdint.h>

#include "fmt_store.h"
#include "fmt_transport.h"

/* WP2.4/WP2.8 remain deliberately one lane only. General track/pattern
 * selection belongs to later packages; keeping these fixed keeps semantics
 * narrow while special-note playback is frozen. */
#define FMT_PLAYBACK_PATTERN 0u
#define FMT_PLAYBACK_TRACK   0u
#define FMT_PLAYBACK_PROOF_NOTE_COUNT 16u
#define FMT_PLAYBACK_PROOF_NOTE_SPACING 4u

enum {
    FMT_PLAYBACK_EVENT_NONE = 0u,
    FMT_PLAYBACK_EVENT_NOTE = 1u,
    FMT_PLAYBACK_EVENT_TIE  = 2u,
    FMT_PLAYBACK_EVENT_OFF  = 3u
};

typedef struct {
    uint8_t started;
} FmtPlayback;

typedef struct {
    uint8_t boundary;
    uint8_t step;
    uint8_t kind;
    uint8_t note;
} FmtPlaybackEvent;

FMT_STATIC_ASSERT(playback_state_size_is_1, sizeof(FmtPlayback) == 1u);
FMT_STATIC_ASSERT(playback_event_size_is_4, sizeof(FmtPlaybackEvent) == 4u);

void fmt_playback_reset(FmtPlayback *playback);

/* Temporary deterministic audible fixture for the one-lane proof. The store
 * must have been reset first. This is startup/test work, never audio-ISR work. */
void fmt_playback_seed_proof_pattern(void);

/* Start from the transport's current step. For the D021 contract the caller
 * supplies a freshly reset transport, so this emits step 0 before audio frame 0
 * and then starts the transport. Returns 1 on successful initial dispatch. */
int fmt_playback_start(FmtPlayback *playback,
                       FmtTransport *transport,
                       FmtPlaybackEvent *event);

/* Called after one audio frame has been rendered. If that elapsed frame crosses
 * a D020 boundary, returns 1 and emits the newly selected row for the next
 * rendered frame. D025 classifies rows as NONE/NOTE/TIE/OFF without enlarging
 * the event object or interpreting special sentinels as pitches. */
int fmt_playback_end_frame(FmtPlayback *playback,
                           FmtTransport *transport,
                           FmtPlaybackEvent *event);

#endif /* FM1TRACKER_FMT_PLAYBACK_H */
