/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_lock_store.h"
#include "fmt_store.h"

/* Deliberately not part of the production public API. Tests inspect the exact
 * D028 pool/runtime invariants and CI inspects these same external symbols. */
extern FmtLock fmt_lock_store_storage[FMT_LOCK_POOL_MAX];
extern FmtLockStoreRuntime fmt_lock_store_runtime;

static void reset_all(void)
{
    fmt_store_reset();
    fmt_lock_store_reset();
}

static void owner_to_address(uint32_t owner,
                             uint32_t *pattern,
                             uint32_t *track,
                             uint32_t *step)
{
    uint32_t per_pattern = FMT_TRACKS * FMT_STEPS_PER_PATTERN;
    uint32_t local = owner % per_pattern;

    *pattern = owner / per_pattern;
    *track = local / FMT_STEPS_PER_PATTERN;
    *step = local % FMT_STEPS_PER_PATTERN;
}

static void assert_non_lock_fields_equal(const FmtStep *a, const FmtStep *b)
{
    assert(a->note == b->note);
    assert(a->instrument == b->instrument);
    assert(a->volume == b->volume);
    assert(a->effect == b->effect);
    assert(a->effect_value == b->effect_value);
    assert(a->flags == b->flags);
}

static void test_layout_and_reset_free_list(void)
{
    uint8_t seen[FMT_LOCK_POOL_MAX];
    uint16_t idx;
    uint32_t i, count = 0u;

    assert(sizeof(FmtLock) == 8u);
    assert(sizeof(FmtLockStoreRuntime) == 4u);
    assert(sizeof fmt_lock_store_storage == 16384u);

    memset(seen, 0, sizeof seen);
    reset_all();
    assert(fmt_lock_store_used_count() == 0u);
    assert(fmt_lock_store_free_count() == FMT_LOCK_POOL_MAX);
    assert(fmt_lock_store_runtime.free_head == 0u);

    idx = fmt_lock_store_runtime.free_head;
    while (idx != FMT_LOCK_NONE) {
        const FmtLock *lock;
        assert(idx < FMT_LOCK_POOL_MAX);
        assert(seen[idx] == 0u);
        seen[idx] = 1u;
        lock = &fmt_lock_store_storage[idx];
        assert(lock->state == FMT_LOCK_STATE_FREE);
        assert(lock->owner == FMT_LOCK_OWNER_NONE);
        assert(lock->param_id == FMT_PARAM_NONE);
        assert(lock->value == 0u);
        if (idx + 1u < FMT_LOCK_POOL_MAX)
            assert(lock->next == (uint16_t)(idx + 1u));
        else
            assert(lock->next == FMT_LOCK_NONE);
        idx = lock->next;
        count++;
        assert(count <= FMT_LOCK_POOL_MAX);
    }
    assert(count == FMT_LOCK_POOL_MAX);
    for (i = 0u; i < FMT_LOCK_POOL_MAX; i++)
        assert(seen[i] == 1u);
}

static void test_all_owner_mappings(void)
{
    uint8_t seen[FMT_LOCK_TOTAL_STEPS];
    uint32_t pattern, track, step, count = 0u;

    memset(seen, 0, sizeof seen);
    reset_all();
    for (pattern = 0u; pattern < FMT_PATTERN_SLOTS; pattern++) {
        for (track = 0u; track < FMT_TRACKS; track++) {
            for (step = 0u; step < FMT_STEPS_PER_PATTERN; step++) {
                uint16_t owner = FMT_LOCK_OWNER_NONE;
                assert(fmt_lock_owner_id(pattern, track, step, &owner) == FMT_LOCK_STATUS_OK);
                assert(owner < FMT_LOCK_TOTAL_STEPS);
                assert(seen[owner] == 0u);
                seen[owner] = 1u;
                count++;
            }
        }
    }
    assert(count == FMT_LOCK_TOTAL_STEPS);
    for (count = 0u; count < FMT_LOCK_TOTAL_STEPS; count++)
        assert(seen[count] == 1u);

    assert(fmt_lock_owner_id(FMT_PATTERN_SLOTS, 0u, 0u, (uint16_t *)&count) ==
           FMT_LOCK_STATUS_BAD_ADDRESS);
    assert(fmt_lock_owner_id(0u, FMT_TRACKS, 0u, (uint16_t *)&count) ==
           FMT_LOCK_STATUS_BAD_ADDRESS);
    assert(fmt_lock_owner_id(0u, 0u, FMT_STEPS_PER_PATTERN, (uint16_t *)&count) ==
           FMT_LOCK_STATUS_BAD_ADDRESS);
    assert(fmt_lock_owner_id(0u, 0u, 0u, NULL) == FMT_LOCK_STATUS_BAD_ADDRESS);
}

static void test_set_get_sort_replace_and_isolation(void)
{
    static const FmtParamId ids[4] = {
        FMT_PARAM_TRACK_PAN,
        FMT_PARAM_VOICE_LEVEL,
        FMT_PARAM_TRACK_LEVEL,
        FMT_PARAM_PITCH_FINE
    };
    static const FmtParamId sorted[4] = {
        FMT_PARAM_VOICE_LEVEL,
        FMT_PARAM_PITCH_FINE,
        FMT_PARAM_TRACK_LEVEL,
        FMT_PARAM_TRACK_PAN
    };
    static const uint16_t sorted_values[4] = { 200u, 400u, 300u, 100u };
    FmtStep *target;
    FmtStep *neighbor;
    FmtStep target_before, neighbor_before;
    uint8_t replaced, count;
    uint32_t i;

    reset_all();
    target = fmt_store_step(3u, 5u, 17u);
    neighbor = fmt_store_step(3u, 5u, 18u);
    assert(target && neighbor);
    target->note = 64u;
    target->instrument = 3u;
    target->volume = 99u;
    target->effect = 7u;
    target->effect_value = 42u;
    target->flags = 0xA5u;
    target_before = *target;
    neighbor_before = *neighbor;

    replaced = 9u;
    assert(fmt_lock_set(3u, 5u, 17u, ids[0], 100u, &replaced) == FMT_LOCK_STATUS_OK);
    assert(replaced == 0u);
    assert(fmt_lock_set(3u, 5u, 17u, ids[1], 200u, NULL) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(3u, 5u, 17u, ids[2], 300u, NULL) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(3u, 5u, 17u, ids[3], 400u, NULL) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_store_used_count() == 4u);
    assert(fmt_lock_count(3u, 5u, 17u, &count) == FMT_LOCK_STATUS_OK);
    assert(count == 4u);

    for (i = 0u; i < 4u; i++) {
        FmtParamId id = FMT_PARAM_INVALID;
        uint16_t value = 0u;
        assert(fmt_lock_at(3u, 5u, 17u, i, &id, &value) == FMT_LOCK_STATUS_OK);
        assert(id == sorted[i]);
        assert(value == sorted_values[i]);
    }
    {
        FmtParamId id = FMT_PARAM_INVALID;
        uint16_t value = 0u;
        assert(fmt_lock_at(3u, 5u, 17u, 4u, &id, &value) == FMT_LOCK_STATUS_NOT_FOUND);
    }

    replaced = 0u;
    assert(fmt_lock_set(3u, 5u, 17u, FMT_PARAM_TRACK_LEVEL, 999u, &replaced) ==
           FMT_LOCK_STATUS_OK);
    assert(replaced == 1u);
    assert(fmt_lock_store_used_count() == 4u);
    {
        uint16_t value = 0u;
        assert(fmt_lock_get(3u, 5u, 17u, FMT_PARAM_TRACK_LEVEL, &value) == FMT_LOCK_STATUS_OK);
        assert(value == 999u);
    }

    assert_non_lock_fields_equal(target, &target_before);
    assert(memcmp(neighbor, &neighbor_before, sizeof neighbor_before) == 0);
}

static void test_invalid_parameters_and_addresses(void)
{
    static const FmtParamId invalid_ids[] = {
        FMT_PARAM_NONE, FMT_PARAM_INVALID, 0x03u, 0x26u, 0x63u, 0x7Cu,
        0xC0u, 0xE0u, 0xF0u
    };
    uint32_t i;
    uint8_t replaced = 7u;
    uint16_t value = 0u;

    reset_all();
    for (i = 0u; i < sizeof invalid_ids / sizeof invalid_ids[0]; i++) {
        replaced = 7u;
        assert(fmt_lock_set(0u, 0u, 0u, invalid_ids[i], 123u, &replaced) ==
               FMT_LOCK_STATUS_INVALID_PARAM);
        assert(replaced == 0u);
    }
    assert(fmt_lock_store_used_count() == 0u);
    assert(fmt_store_step(0u, 0u, 0u)->lock_head == FMT_LOCK_NONE);

    assert(fmt_lock_set(FMT_PATTERN_SLOTS, 0u, 0u, FMT_PARAM_VOICE_LEVEL, 1u, NULL) ==
           FMT_LOCK_STATUS_BAD_ADDRESS);
    assert(fmt_lock_set(0u, FMT_TRACKS, 0u, FMT_PARAM_VOICE_LEVEL, 1u, NULL) ==
           FMT_LOCK_STATUS_BAD_ADDRESS);
    assert(fmt_lock_set(0u, 0u, FMT_STEPS_PER_PATTERN, FMT_PARAM_VOICE_LEVEL, 1u, NULL) ==
           FMT_LOCK_STATUS_BAD_ADDRESS);
    assert(fmt_lock_get(0u, 0u, 0u, FMT_PARAM_VOICE_LEVEL, NULL) ==
           FMT_LOCK_STATUS_BAD_ADDRESS);
    assert(fmt_lock_get(0u, 0u, 0u, 0x03u, &value) == FMT_LOCK_STATUS_INVALID_PARAM);
}

static void test_remove_clear_and_reset_recovery(void)
{
    uint8_t cleared = 99u, count;
    uint16_t value;

    reset_all();
    assert(fmt_lock_set(0u, 0u, 0u, FMT_PARAM_VOICE_LEVEL, 1u, NULL) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(0u, 0u, 0u, FMT_PARAM_PITCH_FINE, 2u, NULL) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(0u, 0u, 0u, FMT_PARAM_TRACK_LEVEL, 3u, NULL) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(0u, 0u, 0u, FMT_PARAM_TRACK_PAN, 4u, NULL) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_store_used_count() == 4u);

    assert(fmt_lock_remove(0u, 0u, 0u, FMT_PARAM_PITCH_FINE) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_store_used_count() == 3u);
    assert(fmt_lock_get(0u, 0u, 0u, FMT_PARAM_PITCH_FINE, &value) ==
           FMT_LOCK_STATUS_NOT_FOUND);
    assert(fmt_lock_remove(0u, 0u, 0u, FMT_PARAM_PITCH_FINE) ==
           FMT_LOCK_STATUS_NOT_FOUND);
    assert(fmt_lock_count(0u, 0u, 0u, &count) == FMT_LOCK_STATUS_OK);
    assert(count == 3u);

    assert(fmt_lock_clear(0u, 0u, 0u, &cleared) == FMT_LOCK_STATUS_OK);
    assert(cleared == 3u);
    assert(fmt_lock_store_used_count() == 0u);
    assert(fmt_lock_store_free_count() == FMT_LOCK_POOL_MAX);
    assert(fmt_store_step(0u, 0u, 0u)->lock_head == FMT_LOCK_NONE);

    cleared = 99u;
    assert(fmt_lock_clear(0u, 0u, 0u, &cleared) == FMT_LOCK_STATUS_OK);
    assert(cleared == 0u);

    assert(fmt_lock_set(15u, 7u, 63u, FMT_PARAM_TRACK_PAN, 555u, NULL) == FMT_LOCK_STATUS_OK);
    fmt_store_reset();
    fmt_lock_store_reset();
    assert(fmt_store_step(15u, 7u, 63u)->lock_head == FMT_LOCK_NONE);
    assert(fmt_lock_store_used_count() == 0u);
    assert(fmt_lock_store_free_count() == FMT_LOCK_POOL_MAX);
}

static void test_global_pool_exhaustion_and_exact_reuse(void)
{
    uint32_t owner;
    uint16_t freed_index;

    reset_all();
    for (owner = 0u; owner < FMT_LOCK_POOL_MAX; owner++) {
        uint32_t pattern, track, step;
        owner_to_address(owner, &pattern, &track, &step);
        assert(fmt_lock_set(pattern, track, step, FMT_PARAM_VOICE_LEVEL,
                            (uint16_t)owner, NULL) == FMT_LOCK_STATUS_OK);
    }
    assert(fmt_lock_store_used_count() == FMT_LOCK_POOL_MAX);
    assert(fmt_lock_store_free_count() == 0u);
    assert(fmt_lock_store_runtime.free_head == FMT_LOCK_NONE);

    {
        uint32_t pattern, track, step;
        uint16_t value = 0u;
        owner_to_address(FMT_LOCK_POOL_MAX, &pattern, &track, &step);
        assert(fmt_lock_set(pattern, track, step, FMT_PARAM_VOICE_LEVEL, 777u, NULL) ==
               FMT_LOCK_STATUS_POOL_FULL);
        assert(fmt_store_step(pattern, track, step)->lock_head == FMT_LOCK_NONE);
        assert(fmt_lock_get(0u, 0u, 0u, FMT_PARAM_VOICE_LEVEL, &value) == FMT_LOCK_STATUS_OK);
        assert(value == 0u);
    }

    {
        uint32_t pattern, track, step;
        owner_to_address(1000u, &pattern, &track, &step);
        freed_index = fmt_store_step(pattern, track, step)->lock_head;
        assert(freed_index < FMT_LOCK_POOL_MAX);
        assert(fmt_lock_remove(pattern, track, step, FMT_PARAM_VOICE_LEVEL) == FMT_LOCK_STATUS_OK);
    }
    assert(fmt_lock_store_used_count() == FMT_LOCK_POOL_MAX - 1u);
    assert(fmt_lock_store_runtime.free_head == freed_index);

    {
        uint32_t pattern, track, step;
        owner_to_address(FMT_LOCK_POOL_MAX, &pattern, &track, &step);
        assert(fmt_lock_set(pattern, track, step, FMT_PARAM_VOICE_LEVEL, 777u, NULL) ==
               FMT_LOCK_STATUS_OK);
        assert(fmt_store_step(pattern, track, step)->lock_head == freed_index);
    }
    assert(fmt_lock_store_used_count() == FMT_LOCK_POOL_MAX);
    assert(fmt_lock_store_free_count() == 0u);
}

static void test_corruption_rejected_without_neighbor_mutation(void)
{
    FmtStep *a, *b;
    FmtLock b_record_before;
    uint16_t b_index;
    uint16_t used_before;
    uint8_t replaced = 9u;

    reset_all();
    assert(fmt_lock_set(0u, 0u, 0u, FMT_PARAM_VOICE_LEVEL, 10u, NULL) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(0u, 0u, 1u, FMT_PARAM_VOICE_LEVEL, 20u, NULL) == FMT_LOCK_STATUS_OK);
    a = fmt_store_step(0u, 0u, 0u);
    b = fmt_store_step(0u, 0u, 1u);
    b_index = b->lock_head;
    b_record_before = fmt_lock_store_storage[b_index];
    used_before = fmt_lock_store_used_count();

    /* In-range cross-owner corruption must not touch B or allocator state. */
    a->lock_head = b_index;
    assert(fmt_lock_set(0u, 0u, 0u, FMT_PARAM_TRACK_PAN, 30u, &replaced) ==
           FMT_LOCK_STATUS_CORRUPT);
    assert(replaced == 0u);
    assert(a->lock_head == b_index);
    assert(b->lock_head == b_index);
    assert(memcmp(&fmt_lock_store_storage[b_index], &b_record_before, sizeof b_record_before) == 0);
    assert(fmt_lock_store_used_count() == used_before);

    reset_all();
    a = fmt_store_step(0u, 0u, 0u);
    a->lock_head = FMT_LOCK_POOL_MAX;
    assert(fmt_lock_remove(0u, 0u, 0u, FMT_PARAM_VOICE_LEVEL) == FMT_LOCK_STATUS_CORRUPT);
    assert(a->lock_head == FMT_LOCK_POOL_MAX);
    assert(fmt_lock_store_used_count() == 0u);

    /* Duplicate/unsorted active IDs are corruption, not silently normalized. */
    reset_all();
    assert(fmt_lock_set(0u, 0u, 0u, FMT_PARAM_VOICE_LEVEL, 1u, NULL) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(0u, 0u, 0u, FMT_PARAM_PITCH_FINE, 2u, NULL) == FMT_LOCK_STATUS_OK);
    a = fmt_store_step(0u, 0u, 0u);
    assert(a->lock_head < FMT_LOCK_POOL_MAX);
    {
        uint16_t second = fmt_lock_store_storage[a->lock_head].next;
        assert(second < FMT_LOCK_POOL_MAX);
        fmt_lock_store_storage[second].param_id = FMT_PARAM_VOICE_LEVEL;
    }
    used_before = fmt_lock_store_used_count();
    assert(fmt_lock_clear(0u, 0u, 0u, NULL) == FMT_LOCK_STATUS_CORRUPT);
    assert(fmt_lock_store_used_count() == used_before);
    assert(a->lock_head != FMT_LOCK_NONE);

    /* Allocator head/count disagreement is also rejected before mutation. */
    reset_all();
    fmt_lock_store_runtime.used_count = FMT_LOCK_POOL_MAX;
    fmt_lock_store_runtime.free_head = 0u;
    a = fmt_store_step(0u, 0u, 0u);
    assert(fmt_lock_set(0u, 0u, 0u, FMT_PARAM_VOICE_LEVEL, 1u, NULL) ==
           FMT_LOCK_STATUS_CORRUPT);
    assert(a->lock_head == FMT_LOCK_NONE);
}

int main(void)
{
    test_layout_and_reset_free_list();
    test_all_owner_mappings();
    test_set_get_sort_replace_and_isolation();
    test_invalid_parameters_and_addresses();
    test_remove_clear_and_reset_recovery();
    test_global_pool_exhaustion_and_exact_reuse();
    test_corruption_rejected_without_neighbor_mutation();

    printf("FMT_LOCK_STORE_TESTS_OK=1\n");
    printf("record_bytes=%zu capacity=%u storage_bytes=%zu runtime_bytes=%zu owners=%u max_per_step=%u\n",
           sizeof(FmtLock),
           (unsigned)FMT_LOCK_POOL_MAX,
           sizeof fmt_lock_store_storage,
           sizeof(FmtLockStoreRuntime),
           (unsigned)FMT_LOCK_TOTAL_STEPS,
           (unsigned)FMT_LOCKS_PER_STEP);
    printf("pool_exhaustion=explicit owner_check=1 sorted_by_param=1 replace_in_place=1 corruption_no_partial_mutation=1\n");
    return 0;
}
