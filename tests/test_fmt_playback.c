/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "fmt_playback.h"

static const uint8_t expected_notes[FMT_PLAYBACK_PROOF_NOTE_COUNT] = {
    60u, 64u, 67u, 72u,
    67u, 64u, 62u, 67u,
    60u, 64u, 67u, 72u,
    74u, 72u, 67u, 64u
};

static void prepare(FmtPlayback *playback, FmtTransport *transport)
{
    fmt_store_reset();
    fmt_playback_seed_proof_pattern();
    fmt_transport_reset(transport);
    fmt_playback_reset(playback);
}

static void test_reset_and_null_safety(void)
{
    FmtPlayback p = {1u};
    FmtTransport t;
    FmtPlaybackEvent e;

    fmt_playback_reset(&p);
    assert(p.started == 0u);
    assert(sizeof(FmtPlayback) == 1u);
    assert(sizeof(FmtPlaybackEvent) == 4u);

    fmt_playback_reset((FmtPlayback *)0);
    fmt_transport_reset(&t);
    assert(fmt_playback_start((FmtPlayback *)0, &t, &e) == 0);
    assert(fmt_playback_start(&p, (FmtTransport *)0, &e) == 0);
    assert(fmt_playback_start(&p, &t, (FmtPlaybackEvent *)0) == 0);
    assert(t.running == 0u && p.started == 0u);
    assert(fmt_playback_end_frame((FmtPlayback *)0, &t, &e) == 0);
    assert(fmt_playback_end_frame(&p, (FmtTransport *)0, &e) == 0);
    assert(fmt_playback_end_frame(&p, &t, (FmtPlaybackEvent *)0) == 0);
}

static void test_proof_pattern_is_one_lane_and_deterministic(void)
{
    uint32_t step, note_index = 0u;

    fmt_store_reset();
    fmt_playback_seed_proof_pattern();

    for (step = 0u; step < FMT_STEPS_PER_PATTERN; step++) {
        const FmtStep *row = fmt_store_step_const(FMT_PLAYBACK_PATTERN,
                                                  FMT_PLAYBACK_TRACK,
                                                  step);
        assert(row != 0);
        if ((step % FMT_PLAYBACK_PROOF_NOTE_SPACING) == 0u) {
            assert(note_index < FMT_PLAYBACK_PROOF_NOTE_COUNT);
            assert(row->note == expected_notes[note_index++]);
        } else {
            assert(row->note == FMT_NOTE_EMPTY);
        }
    }
    assert(note_index == FMT_PLAYBACK_PROOF_NOTE_COUNT);

    /* WP2.4 may not seed or alias any other logical lane. */
    for (step = 0u; step < FMT_STEPS_PER_PATTERN; step++)
        assert(fmt_store_step_const(0u, 1u, step)->note == FMT_NOTE_EMPTY);
    assert(fmt_store_step_const(1u, 0u, 0u)->note == FMT_NOTE_EMPTY);
}

static void test_step_zero_dispatches_before_frame_zero(void)
{
    FmtPlayback p;
    FmtTransport t;
    FmtPlaybackEvent e = {9u, 9u, 9u, 9u};

    prepare(&p, &t);
    assert(fmt_playback_start(&p, &t, &e) == 1);
    assert(p.started == 1u);
    assert(t.running == 1u);
    assert(t.step == 0u && t.phase == 0u);
    assert(e.boundary == 1u);
    assert(e.step == 0u);
    assert(e.has_note == 1u);
    assert(e.note == expected_notes[0]);

    /* Initial dispatch is one-shot; a second start must not retrigger step 0. */
    assert(fmt_playback_start(&p, &t, &e) == 0);
}

static void test_120_bpm_fractional_boundary_applies_to_next_frame(void)
{
    FmtPlayback p;
    FmtTransport t;
    FmtPlaybackEvent e;
    uint32_t rendered;

    prepare(&p, &t);
    assert(fmt_playback_start(&p, &t, &e) == 1);

    for (rendered = 0u; rendered < 5512u; rendered++)
        assert(fmt_playback_end_frame(&p, &t, &e) == 0);
    assert(t.step == 0u && t.phase == 661440u);

    /* The 5513th elapsed frame crosses the boundary, so step 1 is active from
     * playback frame 5513. Step 1 is EMPTY in the proof fixture. */
    assert(fmt_playback_end_frame(&p, &t, &e) == 1);
    assert(t.step == 1u && t.phase == 60u);
    assert(e.boundary == 1u && e.step == 1u);
    assert(e.has_note == 0u && e.note == FMT_NOTE_EMPTY);

    for (rendered = 0u; rendered < 5511u; rendered++)
        assert(fmt_playback_end_frame(&p, &t, &e) == 0);
    assert(fmt_playback_end_frame(&p, &t, &e) == 1);
    assert(t.step == 2u && t.phase == 0u);
}

static void test_special_notes_are_not_accidental_pitches(void)
{
    FmtPlayback p;
    FmtTransport t;
    FmtPlaybackEvent e;
    FmtStep *row;
    uint32_t i;

    fmt_store_reset();
    row = fmt_store_step(0u, 0u, 0u);
    assert(row != 0);
    row->note = FMT_NOTE_TIE;
    fmt_transport_reset(&t);
    fmt_playback_reset(&p);
    assert(fmt_playback_start(&p, &t, &e) == 1);
    assert(e.has_note == 0u && e.note == FMT_NOTE_EMPTY);

    fmt_store_reset();
    row = fmt_store_step(0u, 0u, 1u);
    assert(row != 0);
    row->note = FMT_NOTE_OFF;
    fmt_transport_reset(&t);
    fmt_playback_reset(&p);
    assert(fmt_playback_start(&p, &t, &e) == 1);
    for (i = 0u; i < 5513u; i++) {
        int crossed = fmt_playback_end_frame(&p, &t, &e);
        if (i < 5512u)
            assert(crossed == 0);
        else
            assert(crossed == 1);
    }
    assert(e.step == 1u && e.has_note == 0u && e.note == FMT_NOTE_EMPTY);
}

static void test_complete_loop_and_note_trace(void)
{
    FmtPlayback p;
    FmtTransport t;
    FmtPlaybackEvent e;
    uint32_t rendered = 0u, boundaries = 0u, notes = 0u;
    uint32_t expected_note_index = 1u; /* step 0 was emitted at frame 0 */

    prepare(&p, &t);
    assert(fmt_playback_start(&p, &t, &e) == 1);
    assert(e.note == expected_notes[0]);
    notes = 1u;

    while (boundaries < FMT_STEPS_PER_PATTERN) {
        uint32_t block_left = 256u; /* mirror normal host/device callback grouping */
        while (block_left-- && boundaries < FMT_STEPS_PER_PATTERN) {
            rendered++;
            if (fmt_playback_end_frame(&p, &t, &e)) {
                boundaries++;
                if (e.has_note) {
                    uint32_t event_frame = rendered;
                    uint32_t expected_frame;

                    if (e.step == 0u) {
                        assert(boundaries == FMT_STEPS_PER_PATTERN);
                        assert(e.note == expected_notes[0]);
                        expected_frame = 352800u;
                    } else {
                        assert((e.step % FMT_PLAYBACK_PROOF_NOTE_SPACING) == 0u);
                        assert(expected_note_index < FMT_PLAYBACK_PROOF_NOTE_COUNT);
                        assert(e.note == expected_notes[expected_note_index]);
                        expected_frame = (uint32_t)e.step * 5512u + (uint32_t)e.step / 2u;
                        expected_note_index++;
                    }
                    assert(event_frame == expected_frame);
                    notes++;
                }
            }
        }
    }

    assert(rendered == 352800u);
    assert(boundaries == 64u);
    assert(t.step == 0u && t.phase == 0u);
    assert(expected_note_index == FMT_PLAYBACK_PROOF_NOTE_COUNT);
    assert(notes == FMT_PLAYBACK_PROOF_NOTE_COUNT + 1u); /* initial + 15 + loop step 0 */
}

int main(void)
{
    test_reset_and_null_safety();
    test_proof_pattern_is_one_lane_and_deterministic();
    test_step_zero_dispatches_before_frame_zero();
    test_120_bpm_fractional_boundary_applies_to_next_frame();
    test_special_notes_are_not_accidental_pitches();
    test_complete_loop_and_note_trace();

    printf("FMT_PLAYBACK_TESTS_OK=1\n");
    printf("pattern=0 track=0 proof_notes=16 spacing=4 loop_frames_120=352800 state_bytes=%zu event_bytes=%zu\n",
           sizeof(FmtPlayback), sizeof(FmtPlaybackEvent));
    return 0;
}
