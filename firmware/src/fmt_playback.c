/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_playback.h"

static const uint8_t fmt_playback_proof_notes[FMT_PLAYBACK_PROOF_NOTE_COUNT] = {
    60u, 64u, 67u, 72u,
    67u, 64u, 62u, 67u,
    60u, 64u, 67u, 72u,
    74u, 72u, 67u, 64u
};

static void fmt_playback_event_clear(FmtPlaybackEvent *event)
{
    if (!event)
        return;
    event->boundary = 0u;
    event->step = 0u;
    event->has_note = 0u;
    event->note = FMT_NOTE_EMPTY;
}

static void fmt_playback_event_for_step(uint8_t step, FmtPlaybackEvent *event)
{
    const FmtStep *row;

    fmt_playback_event_clear(event);
    if (!event)
        return;

    event->boundary = 1u;
    event->step = step;
    row = fmt_store_step_const(FMT_PLAYBACK_PATTERN, FMT_PLAYBACK_TRACK, step);
    if (row && row->note <= FMT_NOTE_MAX) {
        event->has_note = 1u;
        event->note = row->note;
    }
}

void fmt_playback_reset(FmtPlayback *playback)
{
    if (playback)
        playback->started = 0u;
}

void fmt_playback_seed_proof_pattern(void)
{
    uint32_t i;

    for (i = 0u; i < FMT_PLAYBACK_PROOF_NOTE_COUNT; i++) {
        FmtStep *row = fmt_store_step(FMT_PLAYBACK_PATTERN,
                                     FMT_PLAYBACK_TRACK,
                                     i * FMT_PLAYBACK_PROOF_NOTE_SPACING);
        if (row)
            row->note = fmt_playback_proof_notes[i];
    }
}

int fmt_playback_start(FmtPlayback *playback,
                       FmtTransport *transport,
                       FmtPlaybackEvent *event)
{
    if (!playback || !transport || !event || playback->started || transport->running)
        return 0;

    fmt_playback_event_for_step(transport->step, event);
    playback->started = 1u;
    fmt_transport_start(transport);
    return 1;
}

int fmt_playback_end_frame(FmtPlayback *playback,
                           FmtTransport *transport,
                           FmtPlaybackEvent *event)
{
    if (!playback || !transport || !event || !playback->started || !transport->running)
        return 0;

    fmt_playback_event_clear(event);
    if (!fmt_transport_tick_frame(transport))
        return 0;

    fmt_playback_event_for_step(transport->step, event);
    return 1;
}
