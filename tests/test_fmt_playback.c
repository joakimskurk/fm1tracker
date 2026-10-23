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

static void advance_one_step(FmtPlayback *p, FmtTransport *t, FmtPlaybackEvent *e)
{
    uint32_t i;
    for (i = 0u; i < 5512u; i++)
        assert(fmt_playback_end_frame(p, t, e) == 0);
    assert(fmt_playback_end_frame(p, t, e) == 1);
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
    assert(FMT_PLAYBACK_EVENT_NONE == 0u);
    assert(FMT_PLAYBACK_EVENT_NOTE == 1u);
    assert(FMT_PLAYBACK_EVENT_TIE == 2u);
    assert(FMT_PLAYBACK_EVENT_OFF == 3u);

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
    assert(e.kind == FMT_PLAYBACK_EVENT_NOTE);
    assert(e.note == expected_notes[0]);

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

    assert(fmt_playback_end_frame(&p, &t, &e) == 1);
    assert(t.step == 1u && t.phase == 60u);
    assert(e.boundary == 1u && e.step == 1u);
    assert(e.kind == FMT_PLAYBACK_EVENT_NONE && e.note == FMT_NOTE_EMPTY);

    for (rendered = 0u; rendered < 5511u; rendered++)
        assert(fmt_playback_end_frame(&p, &t, &e) == 0);
    assert(fmt_playback_end_frame(&p, &t, &e) == 1);
    assert(t.step == 2u && t.phase == 0u);
}

static void test_special_event_kinds_and_exact_boundaries(void)
{
    FmtPlayback p;
    FmtTransport t;
    FmtPlaybackEvent e;
    FmtStep *row;

    /* TIE at frame 0 is explicit and never becomes pitch 0xFD. */
    fmt_store_reset();
    row = fmt_store_step(0u, 0u, 0u);
    assert(row);
    row->note = FMT_NOTE_TIE;
    fmt_transport_reset(&t);
    fmt_playback_reset(&p);
    assert(fmt_playback_start(&p, &t, &e) == 1);
    assert(e.boundary == 1u && e.step == 0u);
    assert(e.kind == FMT_PLAYBACK_EVENT_TIE);
    assert(e.note == FMT_NOTE_TIE);

    /* OFF on step 1 arrives at the exact D021 boundary. */
    fmt_store_reset();
    row = fmt_store_step(0u, 0u, 0u);
    assert(row);
    row->note = 60u;
    row = fmt_store_step(0u, 0u, 1u);
    assert(row);
    row->note = FMT_NOTE_OFF;
    fmt_transport_reset(&t);
    fmt_playback_reset(&p);
    assert(fmt_playback_start(&p, &t, &e) == 1);
    assert(e.kind == FMT_PLAYBACK_EVENT_NOTE && e.note == 60u);
    advance_one_step(&p, &t, &e);
    assert(t.step == 1u && t.phase == 60u);
    assert(e.boundary == 1u && e.step == 1u);
    assert(e.kind == FMT_PLAYBACK_EVENT_OFF);
    assert(e.note == FMT_NOTE_OFF);

    /* EMPTY is an explicit row boundary with no voice command. */
    fmt_store_reset();
    fmt_transport_reset(&t);
    fmt_playback_reset(&p);
    assert(fmt_playback_start(&p, &t, &e) == 1);
    assert(e.kind == FMT_PLAYBACK_EVENT_NONE);
    assert(e.note == FMT_NOTE_EMPTY);

    /* Reserved/malformed non-note sentinel is also safe NONE. */
    fmt_store_reset();
    row = fmt_store_step(0u, 0u, 0u);
    assert(row);
    row->note = 0xFCu;
    fmt_transport_reset(&t);
    fmt_playback_reset(&p);
    assert(fmt_playback_start(&p, &t, &e) == 1);
    assert(e.kind == FMT_PLAYBACK_EVENT_NONE);
    assert(e.note == FMT_NOTE_EMPTY);
}

static void test_complete_loop_and_note_trace(void)
{
    FmtPlayback p;
    FmtTransport t;
    FmtPlaybackEvent e;
    uint32_t rendered = 0u, boundaries = 0u, notes = 0u;
    uint32_t expected_note_index = 1u;

    prepare(&p, &t);
    assert(fmt_playback_start(&p, &t, &e) == 1);
    assert(e.kind == FMT_PLAYBACK_EVENT_NOTE);
    assert(e.note == expected_notes[0]);
    notes = 1u;

    while (boundaries < FMT_STEPS_PER_PATTERN) {
        uint32_t block_left = 256u;
        while (block_left-- && boundaries < FMT_STEPS_PER_PATTERN) {
            rendered++;
            if (fmt_playback_end_frame(&p, &t, &e)) {
                boundaries++;
                if (e.kind == FMT_PLAYBACK_EVENT_NOTE) {
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
                } else {
                    assert(e.kind == FMT_PLAYBACK_EVENT_NONE);
                    assert(e.note == FMT_NOTE_EMPTY);
                }
            }
        }
    }

    assert(rendered == 352800u);
    assert(boundaries == 64u);
    assert(t.step == 0u && t.phase == 0u);
    assert(expected_note_index == FMT_PLAYBACK_PROOF_NOTE_COUNT);
    assert(notes == FMT_PLAYBACK_PROOF_NOTE_COUNT + 1u);
}

int main(void)
{
    test_reset_and_null_safety();
    test_proof_pattern_is_one_lane_and_deterministic();
    test_step_zero_dispatches_before_frame_zero();
    test_120_bpm_fractional_boundary_applies_to_next_frame();
    test_special_event_kinds_and_exact_boundaries();
    test_complete_loop_and_note_trace();

    printf("FMT_PLAYBACK_TESTS_OK=1\n");
    printf("pattern=0 track=0 proof_notes=16 spacing=4 loop_frames_120=352800 state_bytes=%zu event_bytes=%zu kinds=none,note,tie,off\n",
           sizeof(FmtPlayback), sizeof(FmtPlaybackEvent));
    return 0;
}
