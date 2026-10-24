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
    event->kind = FMT_PLAYBACK_EVENT_NONE;
    event->note = FMT_NOTE_EMPTY;
}

static uint8_t fmt_playback_kind_for_note(uint8_t note)
{
    if (note <= FMT_NOTE_MAX)
        return FMT_PLAYBACK_EVENT_NOTE;
    if (note == FMT_NOTE_TIE)
        return FMT_PLAYBACK_EVENT_TIE;
    if (note == FMT_NOTE_OFF)
        return FMT_PLAYBACK_EVENT_OFF;
    return FMT_PLAYBACK_EVENT_NONE;
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
    if (!row)
        return;

    event->kind = fmt_playback_kind_for_note(row->note);
    if (event->kind == FMT_PLAYBACK_EVENT_NOTE)
        event->note = row->note;
    else if (event->kind == FMT_PLAYBACK_EVENT_TIE)
        event->note = FMT_NOTE_TIE;
    else if (event->kind == FMT_PLAYBACK_EVENT_OFF)
        event->note = FMT_NOTE_OFF;
    /* EMPTY and malformed/reserved values deliberately remain NONE. */
}

static void fmt_track_playback_event_clear(FmtTrackPlaybackEvent *event)
{
    if (!event)
        return;
    event->step = 0u;
    event->kind = FMT_PLAYBACK_EVENT_NONE;
    event->note = FMT_NOTE_EMPTY;
    event->instrument = FMT_INSTRUMENT_INHERIT;
}

static void fmt_track_playback_event_for_step(uint8_t track,
                                              uint8_t step,
                                              FmtTrackPlaybackEvent *event)
{
    const FmtStep *row;

    fmt_track_playback_event_clear(event);
    if (!event)
        return;
    event->step = step;
    if (track >= FMT_TRACKS)
        return;

    row = fmt_store_step_const(FMT_PLAYBACK_PATTERN, track, step);
    if (!row)
        return;

    event->kind = fmt_playback_kind_for_note(row->note);
    event->instrument = row->instrument;
    if (event->kind == FMT_PLAYBACK_EVENT_NOTE)
        event->note = row->note;
    else if (event->kind == FMT_PLAYBACK_EVENT_TIE)
        event->note = FMT_NOTE_TIE;
    else if (event->kind == FMT_PLAYBACK_EVENT_OFF)
        event->note = FMT_NOTE_OFF;
}

static void fmt_track_playback_events_for_step(
    uint8_t step,
    FmtTrackPlaybackEvent events[FMT_TRACKS])
{
    uint32_t track;

    if (!events)
        return;
    for (track = 0u; track < FMT_TRACKS; track++)
        fmt_track_playback_event_for_step((uint8_t)track, step, &events[track]);
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
    if (!event)
        return 0;
    fmt_playback_event_clear(event);
    if (!playback || !transport || playback->started || transport->running)
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
    if (!event)
        return 0;
    fmt_playback_event_clear(event);
    if (!playback || !transport || !playback->started || !transport->running)
        return 0;

    if (!fmt_transport_tick_frame(transport))
        return 0;

    fmt_playback_event_for_step(transport->step, event);
    return 1;
}

int fmt_playback_start_tracks(FmtPlayback *playback,
                              FmtTransport *transport,
                              FmtTrackPlaybackEvent events[FMT_TRACKS])
{
    uint32_t track;

    if (!events)
        return 0;
    for (track = 0u; track < FMT_TRACKS; track++)
        fmt_track_playback_event_clear(&events[track]);
    if (!playback || !transport || playback->started || transport->running)
        return 0;

    fmt_track_playback_events_for_step(transport->step, events);
    playback->started = 1u;
    fmt_transport_start(transport);
    return 1;
}

int fmt_playback_end_frame_tracks(FmtPlayback *playback,
                                  FmtTransport *transport,
                                  FmtTrackPlaybackEvent events[FMT_TRACKS])
{
    uint32_t track;

    if (!events)
        return 0;
    for (track = 0u; track < FMT_TRACKS; track++)
        fmt_track_playback_event_clear(&events[track]);
    if (!playback || !transport || !playback->started || !transport->running)
        return 0;

    if (!fmt_transport_tick_frame(transport))
        return 0;

    fmt_track_playback_events_for_step(transport->step, events);
    return 1;
}
