/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_PLAYBACK_H
#define FM1TRACKER_FMT_PLAYBACK_H

#include <stdint.h>

#include "fmt_store.h"
#include "fmt_transport.h"

/* The original D021 proof lane remains pattern 0 / track 0 for inherited API
 * regression. D045 adds a separate bounded eight-track dispatch API without
 * enlarging or reinterpreting FmtPlaybackEvent. */
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

/* D045 multi-track row event. Track identity is the fixed events[track] index;
 * the fourth byte captures the row instrument at the same bounded store read as
 * note classification. */
typedef struct {
    uint8_t step;
    uint8_t kind;
    uint8_t note;
    uint8_t instrument;
} FmtTrackPlaybackEvent;

FMT_STATIC_ASSERT(playback_state_size_is_1, sizeof(FmtPlayback) == 1u);
FMT_STATIC_ASSERT(playback_event_size_is_4, sizeof(FmtPlaybackEvent) == 4u);
FMT_STATIC_ASSERT(track_playback_event_size_is_4,
                  sizeof(FmtTrackPlaybackEvent) == 4u);

void fmt_playback_reset(FmtPlayback *playback);

/* Temporary deterministic audible fixture for the proof pattern. The store
 * must have been reset first. This is startup/test work, never audio-ISR work. */
void fmt_playback_seed_proof_pattern(void);

/* Legacy D021 one-lane API retained exactly for regression. */
int fmt_playback_start(FmtPlayback *playback,
                       FmtTransport *transport,
                       FmtPlaybackEvent *event);
int fmt_playback_end_frame(FmtPlayback *playback,
                           FmtTransport *transport,
                           FmtPlaybackEvent *event);

/* D045 Gate 1: emit pattern-0 rows for all eight fixed logical tracks in one
 * bounded call. events[0]..events[7] are always written in ascending track
 * order. The caller owns a fixed FMT_TRACKS array. */
int fmt_playback_start_tracks(FmtPlayback *playback,
                              FmtTransport *transport,
                              FmtTrackPlaybackEvent events[FMT_TRACKS]);
int fmt_playback_end_frame_tracks(FmtPlayback *playback,
                                  FmtTransport *transport,
                                  FmtTrackPlaybackEvent events[FMT_TRACKS]);

#endif /* FM1TRACKER_FMT_PLAYBACK_H */
