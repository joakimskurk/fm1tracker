/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "fmt_lock_store.h"
#include "fmt_playback.h"
#include "fmt_playback_params.h"
#include "fmt_store.h"
#include "fmt_tracker_effect.h"

static void set_lock_track(uint32_t track,
                           uint32_t step,
                           FmtParamId param,
                           uint16_t value)
{
    uint8_t replaced = 0u;

    assert(fmt_lock_set(FMT_PLAYBACK_PATTERN,
                        track,
                        step,
                        param,
                        value,
                        &replaced) == FMT_LOCK_STATUS_OK);
}

static void test_gate1_layout(void)
{
    assert(FMT_TRACKS == 8u);
    assert(sizeof(FmtPlaybackEvent) == 4u);
    assert(sizeof(FmtTrackPlaybackEvent) == 4u);
    assert(sizeof(fmt_playback_param_cache) == 256u);
    assert(sizeof(fmt_playback_param_cache_extra) == 1792u);
    assert(sizeof(fmt_tracker_effect_cache) == 256u);
    assert(sizeof(fmt_tracker_effect_cache_extra) == 1792u);
    assert(((uintptr_t)&fmt_playback_param_cache[0] & 3u) == 0u);
    assert(((uintptr_t)&fmt_playback_param_cache_extra[0][0] & 3u) == 0u);
    assert(((uintptr_t)&fmt_tracker_effect_cache[0] & 3u) == 0u);
    assert(((uintptr_t)&fmt_tracker_effect_cache_extra[0][0] & 3u) == 0u);
}

static void test_eight_track_playback_dispatch(void)
{
    static const uint8_t notes[FMT_TRACKS] = {
        60u, FMT_NOTE_TIE, FMT_NOTE_OFF, FMT_NOTE_EMPTY,
        127u, 0xFCu, 36u, 84u
    };
    static const uint8_t kinds[FMT_TRACKS] = {
        FMT_PLAYBACK_EVENT_NOTE,
        FMT_PLAYBACK_EVENT_TIE,
        FMT_PLAYBACK_EVENT_OFF,
        FMT_PLAYBACK_EVENT_NONE,
        FMT_PLAYBACK_EVENT_NOTE,
        FMT_PLAYBACK_EVENT_NONE,
        FMT_PLAYBACK_EVENT_NOTE,
        FMT_PLAYBACK_EVENT_NOTE
    };
    FmtTrackPlaybackEvent events[FMT_TRACKS];
    FmtPlayback playback;
    FmtTransport transport;
    uint32_t track, frame;

    fmt_store_reset();
    for (track = 0u; track < FMT_TRACKS; track++) {
        FmtStep *row0 = fmt_store_step(FMT_PLAYBACK_PATTERN, track, 0u);
        FmtStep *row1 = fmt_store_step(FMT_PLAYBACK_PATTERN, track, 1u);
        assert(row0 && row1);
        row0->note = notes[track];
        row0->instrument = (uint8_t)track;
        row1->note = (uint8_t)(70u + track);
        row1->instrument = (uint8_t)(8u + track);
    }

    fmt_transport_reset(&transport);
    fmt_playback_reset(&playback);
    assert(fmt_playback_start_tracks(&playback, &transport, events) == 1);
    assert(playback.started == 1u && transport.running == 1u);
    assert(transport.step == 0u && transport.phase == 0u);

    for (track = 0u; track < FMT_TRACKS; track++) {
        uint8_t expected_note = FMT_NOTE_EMPTY;

        if (kinds[track] == FMT_PLAYBACK_EVENT_NOTE)
            expected_note = notes[track];
        else if (kinds[track] == FMT_PLAYBACK_EVENT_TIE)
            expected_note = FMT_NOTE_TIE;
        else if (kinds[track] == FMT_PLAYBACK_EVENT_OFF)
            expected_note = FMT_NOTE_OFF;

        assert(events[track].step == 0u);
        assert(events[track].kind == kinds[track]);
        assert(events[track].note == expected_note);
        assert(events[track].instrument == (uint8_t)track);
    }

    for (frame = 0u; frame < 5512u; frame++)
        assert(fmt_playback_end_frame_tracks(&playback, &transport, events) == 0);
    assert(fmt_playback_end_frame_tracks(&playback, &transport, events) == 1);
    assert(transport.step == 1u && transport.phase == 60u);

    for (track = 0u; track < FMT_TRACKS; track++) {
        assert(events[track].step == 1u);
        assert(events[track].kind == FMT_PLAYBACK_EVENT_NOTE);
        assert(events[track].note == (uint8_t)(70u + track));
        assert(events[track].instrument == (uint8_t)(8u + track));
    }

    assert(fmt_playback_start_tracks(&playback, &transport,
                                     (FmtTrackPlaybackEvent *)0) == 0);
    assert(fmt_playback_end_frame_tracks(&playback, &transport,
                                         (FmtTrackPlaybackEvent *)0) == 0);
}

static void test_d031_track_publication(void)
{
    const uint32_t step = 9u;
    uint32_t track;

    fmt_store_reset();
    fmt_lock_store_reset();
    fmt_playback_params_reset();

    for (track = 0u; track < FMT_TRACKS; track++) {
        set_lock_track(track, step, (FmtParamId)FMT_PARAM_VOICE_LEVEL,
                       (uint16_t)(20u + track));
        set_lock_track(track, step, (FmtParamId)FMT_PARAM_PITCH_FINE,
                       (uint16_t)(128u + track));
        set_lock_track(track, step, (FmtParamId)FMT_PARAM_TRACK_LEVEL,
                       (uint16_t)(220u - track));
    }

    fmt_playback_params_rebuild();
    for (track = 0u; track < FMT_TRACKS; track++) {
        uint32_t word = fmt_playback_params_read_track(track, step);
        assert(fmt_playback_params_voice(word) == (uint8_t)(20u + track));
        assert(fmt_playback_params_fine(word) == (uint8_t)(128u + track));
        assert(fmt_playback_params_track(word) == (uint8_t)(220u - track));
        assert(fmt_playback_params_flags(word) == FMT_PLAYBACK_PARAM_FLAG_NONE);
    }

    assert(fmt_playback_params_read(step) ==
           fmt_playback_params_read_track(FMT_PLAYBACK_TRACK, step));
    assert(fmt_playback_params_read_track(FMT_TRACKS, step) ==
           fmt_playback_params_default_word());
    assert(fmt_playback_params_read_track(0u, FMT_PLAYBACK_PARAM_ROWS) ==
           fmt_playback_params_default_word());
    assert(!fmt_playback_params_refresh_track_step(FMT_TRACKS, step));
    assert(!fmt_playback_params_refresh_track_step(0u,
                                                    FMT_PLAYBACK_PARAM_ROWS));

    set_lock_track(0u, step, (FmtParamId)FMT_PARAM_VOICE_LEVEL, 99u);
    assert(fmt_playback_params_refresh_step(step));
    assert(fmt_playback_params_voice(fmt_playback_params_read(step)) == 99u);
    assert(fmt_playback_params_voice(fmt_playback_params_read_track(1u, step)) ==
           21u);
}

static void test_d032_track_publication(void)
{
    const uint32_t step = 7u;
    uint32_t track;

    fmt_store_reset();
    fmt_tracker_effect_cache_reset();

    for (track = 0u; track < FMT_TRACKS; track++) {
        FmtStep *row = fmt_store_step(FMT_PLAYBACK_PATTERN, track, step);
        assert(row);
        row->effect = (uint8_t)track;
        row->effect_value = (uint8_t)(0x20u + track);
    }

    fmt_tracker_effect_cache_rebuild();
    for (track = 0u; track < FMT_TRACKS; track++) {
        uint32_t word = fmt_tracker_effect_cache_read_track(track, step);
        assert(fmt_tracker_effect_command(word) == (uint8_t)track);
        assert(fmt_tracker_effect_value(word) == (uint8_t)(0x20u + track));
        assert(fmt_tracker_effect_flags(word) == FMT_TRACKER_EFFECT_FLAG_NONE);
    }

    assert(fmt_tracker_effect_cache_read(step) ==
           fmt_tracker_effect_cache_read_track(FMT_PLAYBACK_TRACK, step));
    assert(fmt_tracker_effect_cache_read_track(FMT_TRACKS, step) ==
           fmt_tracker_effect_default_word());
    assert(fmt_tracker_effect_cache_read_track(0u, FMT_TRACKER_EFFECT_ROWS) ==
           fmt_tracker_effect_default_word());
    assert(!fmt_tracker_effect_cache_refresh_track_step(FMT_TRACKS, step));
    assert(!fmt_tracker_effect_cache_refresh_track_step(0u,
                                                        FMT_TRACKER_EFFECT_ROWS));

    fmt_store_step(FMT_PLAYBACK_PATTERN, 0u, step)->effect =
        FMT_TRACKER_EFFECT_NOTE_CUT;
    fmt_store_step(FMT_PLAYBACK_PATTERN, 0u, step)->effect_value = 0x7Fu;
    assert(fmt_tracker_effect_cache_refresh_step(step));
    assert(fmt_tracker_effect_command(fmt_tracker_effect_cache_read(step)) ==
           FMT_TRACKER_EFFECT_NOTE_CUT);
    assert(fmt_tracker_effect_value(fmt_tracker_effect_cache_read(step)) == 0x7Fu);
    assert(fmt_tracker_effect_command(fmt_tracker_effect_cache_read_track(1u,
                                                                           step)) ==
           1u);
}

int main(void)
{
    test_gate1_layout();
    test_eight_track_playback_dispatch();
    test_d031_track_publication();
    test_d032_track_publication();

    printf("WP4_9_GATE1_TESTS_OK=1\n");
    printf("tracks=8 playback_event_bytes=4 track_event_bytes=4 d031_legacy=256 d031_extra=1792 d032_legacy=256 d032_extra=1792 boundary_step1_frame=5513\n");
    printf("legacy_track0_wrappers=1 track_isolation=1 invalid_address_fallback=1 dinghy_migration=0\n");
    return 0;
}
