/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "fmt_lock_editor.h"
#include "fmt_playback_params.h"
#include "fmt_rt_queue.h"
#include "fmt_step_record.h"
#include "fmt_tracker_effect.h"
#include "fmt_tracker_screen.h"

static const FmtParamId params[4] = {
    (FmtParamId)FMT_PARAM_VOICE_LEVEL,
    (FmtParamId)FMT_PARAM_PITCH_FINE,
    (FmtParamId)FMT_PARAM_TRACK_LEVEL,
    (FmtParamId)FMT_PARAM_TRACK_PAN
};

static uint16_t value_a(uint32_t track, uint32_t step, uint32_t param)
{
    return (uint16_t)((track * 64u + step + param * 17u) & 0xFFu);
}

static uint16_t value_b(uint32_t track, uint32_t step, uint32_t param)
{
    return (uint16_t)((255u - ((track * 29u + step * 3u + param * 11u) & 0xFFu)) & 0xFFu);
}

static uint32_t fnv_byte(uint32_t hash, uint8_t value)
{
    return (hash ^ value) * 16777619u;
}

static uint32_t lock_state_hash(void)
{
    uint32_t track, step, ordinal;
    uint32_t hash = 2166136261u;

    for (track = 0u; track < FMT_TRACKS; track++) {
        for (step = 0u; step < FMT_STEPS_PER_PATTERN; step++) {
            uint8_t count = 0u;
            assert(fmt_lock_count(0u, track, step, &count) == FMT_LOCK_STATUS_OK);
            hash = fnv_byte(hash, count);
            for (ordinal = 0u; ordinal < count; ordinal++) {
                FmtParamId id = 0u;
                uint16_t value = 0u;
                assert(fmt_lock_at(0u, track, step, ordinal, &id, &value) ==
                       FMT_LOCK_STATUS_OK);
                hash = fnv_byte(hash, id);
                hash = fnv_byte(hash, (uint8_t)value);
                hash = fnv_byte(hash, (uint8_t)(value >> 8));
            }
        }
    }
    return hash;
}

static void fill_full_lock_pool(void)
{
    uint32_t track, step, p;

    fmt_store_reset();
    fmt_lock_store_reset();

    for (track = 0u; track < FMT_TRACKS; track++) {
        for (step = 0u; step < FMT_STEPS_PER_PATTERN; step++) {
            for (p = 0u; p < 4u; p++) {
                uint8_t replaced = 0xFFu;
                assert(fmt_lock_set(0u, track, step, params[p],
                                    value_a(track, step, p), &replaced) ==
                       FMT_LOCK_STATUS_OK);
                assert(replaced == 0u);
            }
        }
    }

    assert(fmt_lock_store_used_count() == FMT_LOCK_POOL_MAX);
    assert(fmt_lock_store_free_count() == 0u);
}

static void test_full_pool_replace_exhaust_recover(void)
{
    uint32_t track, step, p;
    uint32_t hash_before, hash_after;
    uint8_t replaced = 0u;

    fill_full_lock_pool();
    hash_before = lock_state_hash();

    /* A valid lock on an empty owner fails only because the global pool is full. */
    assert(fmt_lock_set(1u, 0u, 0u, (FmtParamId)FMT_PARAM_VOICE_LEVEL,
                        77u, &replaced) == FMT_LOCK_STATUS_POOL_FULL);
    assert(fmt_lock_store_used_count() == FMT_LOCK_POOL_MAX);

    /* Replacing every existing record is capacity-neutral even at full pool. */
    for (track = 0u; track < FMT_TRACKS; track++) {
        for (step = 0u; step < FMT_STEPS_PER_PATTERN; step++) {
            for (p = 0u; p < 4u; p++) {
                replaced = 0u;
                assert(fmt_lock_set(0u, track, step, params[p],
                                    value_b(track, step, p), &replaced) ==
                       FMT_LOCK_STATUS_OK);
                assert(replaced == 1u);
            }
        }
    }
    assert(fmt_lock_store_used_count() == FMT_LOCK_POOL_MAX);
    assert(fmt_lock_store_free_count() == 0u);
    hash_after = lock_state_hash();
    assert(hash_after != hash_before);

    /* Free first/middle/last semantic positions on distinct owners, consume the
     * exact three records elsewhere, release them again, then restore owners. */
    assert(fmt_lock_remove(0u, 0u, 0u, params[0]) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_remove(0u, 3u, 31u, params[1]) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_remove(0u, 7u, 63u, params[3]) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_store_free_count() == 3u);

    for (p = 0u; p < 3u; p++) {
        replaced = 0u;
        assert(fmt_lock_set(1u, 0u, p, params[p], (uint16_t)(90u + p), &replaced) ==
               FMT_LOCK_STATUS_OK);
        assert(replaced == 0u);
    }
    assert(fmt_lock_store_used_count() == FMT_LOCK_POOL_MAX);
    assert(fmt_lock_set(1u, 0u, 3u, params[0], 99u, &replaced) ==
           FMT_LOCK_STATUS_POOL_FULL);

    for (p = 0u; p < 3u; p++)
        assert(fmt_lock_remove(1u, 0u, p, params[p]) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_store_free_count() == 3u);

    replaced = 0u;
    assert(fmt_lock_set(0u, 0u, 0u, params[0], value_b(0u, 0u, 0u), &replaced) ==
           FMT_LOCK_STATUS_OK);
    assert(!replaced);
    assert(fmt_lock_set(0u, 3u, 31u, params[1], value_b(3u, 31u, 1u), &replaced) ==
           FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(0u, 7u, 63u, params[3], value_b(7u, 63u, 3u), &replaced) ==
           FMT_LOCK_STATUS_OK);
    assert(fmt_lock_store_used_count() == FMT_LOCK_POOL_MAX);
    assert(fmt_lock_store_free_count() == 0u);
    assert(lock_state_hash() == hash_after);

    printf("WP3_10_LOCK_POOL_STRESS_OK=1 records=%u owners=%u hash=%08x\n",
           (unsigned)FMT_LOCK_POOL_MAX,
           (unsigned)(FMT_TRACKS * FMT_STEPS_PER_PATTERN),
           (unsigned)hash_after);
}

static void test_full_row_caches(void)
{
    uint32_t step;

    /* Pool is still full: proof track has all four locks on every row. */
    fmt_playback_params_reset();
    fmt_playback_params_rebuild();
    for (step = 0u; step < FMT_STEPS_PER_PATTERN; step++) {
        uint32_t word = fmt_playback_params_read(step);
        assert(fmt_playback_params_voice(word) == value_b(0u, step, 0u));
        assert(fmt_playback_params_fine(word) == value_b(0u, step, 1u));
        assert(fmt_playback_params_track(word) == value_b(0u, step, 2u));
        assert(fmt_playback_params_flags(word) == FMT_PLAYBACK_PARAM_FLAG_NONE);
    }

    for (step = 0u; step < FMT_STEPS_PER_PATTERN; step++) {
        FmtStep *row = fmt_store_step(FMT_PLAYBACK_PATTERN, FMT_PLAYBACK_TRACK, step);
        uint8_t command = (uint8_t)(1u + (step % 8u));
        assert(row);
        row->effect = command;
        row->effect_value = (uint8_t)(step * 13u + 7u);
    }
    fmt_tracker_effect_cache_reset();
    fmt_tracker_effect_cache_rebuild();
    for (step = 0u; step < FMT_STEPS_PER_PATTERN; step++) {
        uint32_t word = fmt_tracker_effect_cache_read(step);
        assert(fmt_tracker_effect_command(word) == (uint8_t)(1u + (step % 8u)));
        assert(fmt_tracker_effect_value(word) == (uint8_t)(step * 13u + 7u));
        assert(fmt_tracker_effect_flags(word) == FMT_TRACKER_EFFECT_FLAG_NONE);
    }

    printf("WP3_10_CACHE_STRESS_OK=1 rows=%u locks_per_row=4 playback_cache=%u effect_cache=%u\n",
           (unsigned)FMT_STEPS_PER_PATTERN,
           (unsigned)sizeof(fmt_playback_param_cache),
           (unsigned)sizeof(fmt_tracker_effect_cache));
}

static void test_queue_wrap_overflow_storm(void)
{
    FmtRtQueue queue;
    FmtRtEvent out;
    uint32_t cycle, i;
    uint32_t expected_overflow = UINT32_MAX - 1000u;

    fmt_rt_queue_reset(&queue);
    queue.write_seq = UINT32_MAX - 17u;
    queue.read_seq = UINT32_MAX - 17u;
    queue.overflow_seq = expected_overflow;

    for (cycle = 0u; cycle < 2048u; cycle++) {
        for (i = 0u; i < FMT_RT_QUEUE_CAPACITY; i++) {
            uint8_t note = (uint8_t)((cycle + i) & 0x7Fu);
            assert(fmt_rt_queue_enqueue_note_on(&queue,
                                                (uint8_t)(i & 7u),
                                                note,
                                                (uint8_t)(1u + (i % 127u))) ==
                   FMT_RT_QUEUE_OK);
        }
        assert(fmt_rt_queue_depth(&queue) == FMT_RT_QUEUE_CAPACITY);

        for (i = 0u; i < 3u; i++) {
            assert(fmt_rt_queue_enqueue_note_off(&queue, 0u, (uint8_t)i) ==
                   FMT_RT_QUEUE_FULL);
            expected_overflow++;
            assert(fmt_rt_queue_overflow_seq(&queue) == expected_overflow);
        }

        if (cycle & 1u) {
            fmt_rt_queue_consumer_discard_all(&queue);
            assert(fmt_rt_queue_depth(&queue) == 0u);
        } else {
            for (i = 0u; i < FMT_RT_QUEUE_CAPACITY; i++) {
                assert(fmt_rt_queue_dequeue(&queue, &out));
                assert(out.type == FMT_RT_EVENT_NOTE_ON);
                assert(out.track == (uint8_t)(i & 7u));
                assert(fmt_rt_event_note(&out) == (uint8_t)((cycle + i) & 0x7Fu));
            }
            assert(!fmt_rt_queue_dequeue(&queue, &out));
        }
    }

    assert(fmt_rt_queue_depth(&queue) == 0u);
    /* Prove immediate post-storm recovery. */
    assert(fmt_rt_queue_enqueue_note_on(&queue, 7u, 127u, 127u) == FMT_RT_QUEUE_OK);
    assert(fmt_rt_queue_dequeue(&queue, &out));
    assert(out.track == 7u && fmt_rt_event_note(&out) == 127u);
    assert(fmt_rt_queue_depth(&queue) == 0u);

    printf("WP3_10_QUEUE_STRESS_OK=1 cycles=2048 rejected=%u final_write=%u final_read=%u overflow=%u\n",
           (unsigned)(2048u * 3u),
           (unsigned)queue.write_seq,
           (unsigned)queue.read_seq,
           (unsigned)queue.overflow_seq);
}

static void drain_two_per_poll(FmtTrackerScreen *screen, uint32_t *polls)
{
    uint8_t region;
    uint32_t guard = 0u;

    while (fmt_tracker_screen_dirty(screen)) {
        uint32_t draws;
        for (draws = 0u; draws < FMT_TRACKER_SCREEN_MAX_REGIONS_PER_POLL; draws++) {
            if (!fmt_tracker_screen_take_region(screen, &region))
                break;
            assert(region < FMT_TRACKER_SCREEN_REGION_COUNT);
        }
        (*polls)++;
        guard++;
        assert(guard <= FMT_TRACKER_SCREEN_REGION_COUNT);
    }
}

static void test_screen_max_inspection_and_backlog(void)
{
    FmtTrackerScreen screen;
    FmtPatternInput input;
    FmtStepRecord record;
    FmtTransport transport;
    FmtLockEditor editor;
    uint32_t row, polls = 0u;

    fmt_tracker_screen_reset(&screen);
    fmt_pattern_input_reset(&input);
    fmt_step_record_reset(&record);
    fmt_transport_reset(&transport);
    fmt_lock_editor_reset(&editor);
    fmt_pattern_input_enter_pattern(&input);

    assert(fmt_tracker_screen_sync_feedback(&screen, &input, &record,
                                            &transport, &editor));
    for (row = 0u; row < FMT_TRACKER_SCREEN_ROWS; row++)
        assert(screen.row_lock_info[row] == 4u);
    assert(fmt_tracker_screen_dirty(&screen) == FMT_TRACKER_SCREEN_ALL_DIRTY);
    drain_two_per_poll(&screen, &polls);
    assert(polls == 5u);

    /* Change every visible effect value simultaneously. All eight row bits must
     * persist and converge in exactly four two-region polls. */
    for (row = 0u; row < FMT_TRACKER_SCREEN_ROWS; row++) {
        FmtStep *step = fmt_store_step(0u, 0u, row);
        assert(step);
        step->effect_value ^= 0x5Au;
    }
    assert(fmt_tracker_screen_sync_feedback(&screen, &input, &record,
                                            &transport, &editor));
    assert(fmt_tracker_screen_dirty(&screen) ==
           (FMT_TRACKER_SCREEN_ALL_DIRTY & (uint16_t)~0x0003u));
    polls = 0u;
    drain_two_per_poll(&screen, &polls);
    assert(polls == 4u);

    /* Active p-lock target overlays P4, then release converges back to L4. */
    assert(fmt_lock_editor_begin(&editor, &input, 0u));
    assert(fmt_tracker_screen_sync_feedback(&screen, &input, &record,
                                            &transport, &editor));
    assert(screen.lock_target_active == 1u);
    assert(screen.row_lock_info[0] == 4u);
    assert(fmt_tracker_screen_dirty(&screen) ==
           (uint16_t)(1u << FMT_TRACKER_SCREEN_REGION_ROW0));
    polls = 0u;
    drain_two_per_poll(&screen, &polls);
    assert(polls == 1u);

    fmt_lock_editor_cancel(&editor);
    assert(fmt_tracker_screen_sync_feedback(&screen, &input, &record,
                                            &transport, &editor));
    assert(screen.lock_target_active == 0u);
    polls = 0u;
    drain_two_per_poll(&screen, &polls);
    assert(polls == 1u && fmt_tracker_screen_dirty(&screen) == 0u);

    printf("WP3_10_SCREEN_STRESS_OK=1 visible_rows=8 locks_inspected_max=32 full_polls=5 row_backlog_polls=4 converged=1\n");
}

int main(void)
{
    test_full_pool_replace_exhaust_recover();
    test_full_row_caches();
    test_queue_wrap_overflow_storm();
    test_screen_max_inspection_and_backlog();

    puts("WP3_10_FOCUSED_WORST_CASE_OK=1");
    return 0;
}
