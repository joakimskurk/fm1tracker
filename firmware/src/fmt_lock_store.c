/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_lock_store.h"

#include "fmt_store.h"

/* D028: exactly 2,048 x 8 B in the large NOLOAD pool. Keep this symbol
 * externally visible so CI can prove section, address and exact size. */
FmtLock fmt_lock_store_storage[FMT_LOCK_POOL_MAX]
    __attribute__((section(".pool"), aligned(4)));

/* Only allocator head/count are ordinary runtime state. */
FmtLockStoreRuntime fmt_lock_store_runtime;

FMT_STATIC_ASSERT(lock_storage_size_is_16384,
                  sizeof(fmt_lock_store_storage) == 16384u);

#define FMT_LOCK_SCAN_NOT_FOUND 0xFFu

typedef struct {
    uint16_t index[FMT_LOCKS_PER_STEP];
    uint8_t count;
    uint8_t found;
} FmtLockScan;

static int fmt_lock_allocator_basic_valid(void)
{
    uint16_t used = fmt_lock_store_runtime.used_count;
    uint16_t head = fmt_lock_store_runtime.free_head;

    if (used > FMT_LOCK_POOL_MAX)
        return 0;
    if (used == FMT_LOCK_POOL_MAX)
        return head == FMT_LOCK_NONE;
    return head < FMT_LOCK_POOL_MAX;
}

static FmtLockStatus fmt_lock_resolve_step(uint32_t pattern,
                                           uint32_t track,
                                           uint32_t step,
                                           FmtStep **step_out,
                                           uint16_t *owner_out)
{
    FmtStep *target;
    uint32_t owner;

    if (!step_out || !owner_out)
        return FMT_LOCK_STATUS_BAD_ADDRESS;
    target = fmt_store_step(pattern, track, step);
    if (!target)
        return FMT_LOCK_STATUS_BAD_ADDRESS;

    owner = pattern * (FMT_TRACKS * FMT_STEPS_PER_PATTERN) +
            track * FMT_STEPS_PER_PATTERN + step;
    if (owner >= FMT_LOCK_TOTAL_STEPS || owner >= FMT_LOCK_OWNER_NONE)
        return FMT_LOCK_STATUS_BAD_ADDRESS;

    *step_out = target;
    *owner_out = (uint16_t)owner;
    return FMT_LOCK_STATUS_OK;
}

static FmtLockStatus fmt_lock_scan_step(const FmtStep *step,
                                        uint16_t owner,
                                        FmtParamId wanted,
                                        FmtLockScan *scan)
{
    uint16_t idx;
    FmtParamId previous = FMT_PARAM_NONE;
    uint8_t count = 0u;

    if (!step || !scan)
        return FMT_LOCK_STATUS_BAD_ADDRESS;

    scan->count = 0u;
    scan->found = FMT_LOCK_SCAN_NOT_FOUND;
    idx = step->lock_head;

    while (idx != FMT_LOCK_NONE) {
        const FmtLock *lock;

        if (count >= FMT_LOCKS_PER_STEP)
            return FMT_LOCK_STATUS_CORRUPT;
        if (idx >= FMT_LOCK_POOL_MAX)
            return FMT_LOCK_STATUS_CORRUPT;

        lock = &fmt_lock_store_storage[idx];
        if (lock->state != FMT_LOCK_STATE_ACTIVE)
            return FMT_LOCK_STATUS_CORRUPT;
        if (lock->owner != owner)
            return FMT_LOCK_STATUS_CORRUPT;
        if (!fmt_param_is_lockable(lock->param_id))
            return FMT_LOCK_STATUS_CORRUPT;
        if (count != 0u && lock->param_id <= previous)
            return FMT_LOCK_STATUS_CORRUPT;

        scan->index[count] = idx;
        if (lock->param_id == wanted)
            scan->found = count;
        previous = lock->param_id;
        count++;
        idx = lock->next;
    }

    scan->count = count;
    return FMT_LOCK_STATUS_OK;
}

static FmtLockStatus fmt_lock_alloc(uint16_t *index_out)
{
    uint16_t idx, next;
    FmtLock *lock;

    if (!index_out)
        return FMT_LOCK_STATUS_BAD_ADDRESS;
    if (!fmt_lock_allocator_basic_valid())
        return FMT_LOCK_STATUS_CORRUPT;
    if (fmt_lock_store_runtime.used_count == FMT_LOCK_POOL_MAX)
        return FMT_LOCK_STATUS_POOL_FULL;

    idx = fmt_lock_store_runtime.free_head;
    if (idx >= FMT_LOCK_POOL_MAX)
        return FMT_LOCK_STATUS_CORRUPT;
    lock = &fmt_lock_store_storage[idx];
    if (lock->state != FMT_LOCK_STATE_FREE ||
        lock->owner != FMT_LOCK_OWNER_NONE ||
        lock->param_id != FMT_PARAM_NONE)
        return FMT_LOCK_STATUS_CORRUPT;

    next = lock->next;
    if (next != FMT_LOCK_NONE && next >= FMT_LOCK_POOL_MAX)
        return FMT_LOCK_STATUS_CORRUPT;

    fmt_lock_store_runtime.free_head = next;
    fmt_lock_store_runtime.used_count++;
    *index_out = idx;
    return FMT_LOCK_STATUS_OK;
}

static FmtLockStatus fmt_lock_free(uint16_t idx)
{
    FmtLock *lock;

    if (idx >= FMT_LOCK_POOL_MAX)
        return FMT_LOCK_STATUS_CORRUPT;
    if (!fmt_lock_allocator_basic_valid())
        return FMT_LOCK_STATUS_CORRUPT;
    if (fmt_lock_store_runtime.used_count == 0u)
        return FMT_LOCK_STATUS_CORRUPT;

    lock = &fmt_lock_store_storage[idx];
    if (lock->state != FMT_LOCK_STATE_ACTIVE)
        return FMT_LOCK_STATUS_CORRUPT;

    lock->next = fmt_lock_store_runtime.free_head;
    lock->value = 0u;
    lock->owner = FMT_LOCK_OWNER_NONE;
    lock->param_id = FMT_PARAM_NONE;
    lock->state = FMT_LOCK_STATE_FREE;
    fmt_lock_store_runtime.free_head = idx;
    fmt_lock_store_runtime.used_count--;
    return FMT_LOCK_STATUS_OK;
}

void fmt_lock_store_reset(void)
{
    uint32_t i;

    for (i = 0u; i < FMT_LOCK_POOL_MAX; i++) {
        FmtLock *lock = &fmt_lock_store_storage[i];
        lock->next = (i + 1u < FMT_LOCK_POOL_MAX) ? (uint16_t)(i + 1u) : FMT_LOCK_NONE;
        lock->value = 0u;
        lock->owner = FMT_LOCK_OWNER_NONE;
        lock->param_id = FMT_PARAM_NONE;
        lock->state = FMT_LOCK_STATE_FREE;
    }
    fmt_lock_store_runtime.free_head = 0u;
    fmt_lock_store_runtime.used_count = 0u;
}

uint16_t fmt_lock_store_used_count(void)
{
    return fmt_lock_store_runtime.used_count;
}

uint16_t fmt_lock_store_free_count(void)
{
    if (fmt_lock_store_runtime.used_count > FMT_LOCK_POOL_MAX)
        return 0u;
    return (uint16_t)(FMT_LOCK_POOL_MAX - fmt_lock_store_runtime.used_count);
}

FmtLockStatus fmt_lock_owner_id(uint32_t pattern,
                                uint32_t track,
                                uint32_t step,
                                uint16_t *owner)
{
    FmtStep *unused;
    return fmt_lock_resolve_step(pattern, track, step, &unused, owner);
}

FmtLockStatus fmt_lock_set(uint32_t pattern,
                           uint32_t track,
                           uint32_t step,
                           FmtParamId param_id,
                           uint16_t value,
                           uint8_t *replaced)
{
    FmtStep *target;
    FmtLockScan scan;
    uint16_t owner, idx, previous, next;
    uint8_t i;
    FmtLockStatus status;

    if (replaced)
        *replaced = 0u;
    if (!fmt_param_is_lockable(param_id))
        return FMT_LOCK_STATUS_INVALID_PARAM;
    status = fmt_lock_resolve_step(pattern, track, step, &target, &owner);
    if (status != FMT_LOCK_STATUS_OK)
        return status;
    if (!fmt_lock_allocator_basic_valid())
        return FMT_LOCK_STATUS_CORRUPT;
    status = fmt_lock_scan_step(target, owner, param_id, &scan);
    if (status != FMT_LOCK_STATUS_OK)
        return status;

    if (scan.found != FMT_LOCK_SCAN_NOT_FOUND) {
        fmt_lock_store_storage[scan.index[scan.found]].value = value;
        if (replaced)
            *replaced = 1u;
        return FMT_LOCK_STATUS_OK;
    }
    if (scan.count >= FMT_LOCKS_PER_STEP)
        return FMT_LOCK_STATUS_STEP_FULL;

    status = fmt_lock_alloc(&idx);
    if (status != FMT_LOCK_STATUS_OK)
        return status;

    previous = FMT_LOCK_NONE;
    next = target->lock_head;
    for (i = 0u; i < scan.count; i++) {
        FmtLock *existing = &fmt_lock_store_storage[scan.index[i]];
        if (existing->param_id > param_id) {
            next = scan.index[i];
            break;
        }
        previous = scan.index[i];
        next = existing->next;
    }

    fmt_lock_store_storage[idx].next = next;
    fmt_lock_store_storage[idx].value = value;
    fmt_lock_store_storage[idx].owner = owner;
    fmt_lock_store_storage[idx].param_id = param_id;
    fmt_lock_store_storage[idx].state = FMT_LOCK_STATE_ACTIVE;

    if (previous == FMT_LOCK_NONE)
        target->lock_head = idx;
    else
        fmt_lock_store_storage[previous].next = idx;
    return FMT_LOCK_STATUS_OK;
}

FmtLockStatus fmt_lock_get(uint32_t pattern,
                           uint32_t track,
                           uint32_t step,
                           FmtParamId param_id,
                           uint16_t *value)
{
    FmtStep *target;
    FmtLockScan scan;
    uint16_t owner;
    FmtLockStatus status;

    if (!value)
        return FMT_LOCK_STATUS_BAD_ADDRESS;
    if (!fmt_param_is_lockable(param_id))
        return FMT_LOCK_STATUS_INVALID_PARAM;
    status = fmt_lock_resolve_step(pattern, track, step, &target, &owner);
    if (status != FMT_LOCK_STATUS_OK)
        return status;
    if (!fmt_lock_allocator_basic_valid())
        return FMT_LOCK_STATUS_CORRUPT;
    status = fmt_lock_scan_step(target, owner, param_id, &scan);
    if (status != FMT_LOCK_STATUS_OK)
        return status;
    if (scan.found == FMT_LOCK_SCAN_NOT_FOUND)
        return FMT_LOCK_STATUS_NOT_FOUND;
    *value = fmt_lock_store_storage[scan.index[scan.found]].value;
    return FMT_LOCK_STATUS_OK;
}

FmtLockStatus fmt_lock_remove(uint32_t pattern,
                              uint32_t track,
                              uint32_t step,
                              FmtParamId param_id)
{
    FmtStep *target;
    FmtLockScan scan;
    uint16_t owner, idx, previous, next;
    FmtLockStatus status;

    if (!fmt_param_is_lockable(param_id))
        return FMT_LOCK_STATUS_INVALID_PARAM;
    status = fmt_lock_resolve_step(pattern, track, step, &target, &owner);
    if (status != FMT_LOCK_STATUS_OK)
        return status;
    if (!fmt_lock_allocator_basic_valid())
        return FMT_LOCK_STATUS_CORRUPT;
    status = fmt_lock_scan_step(target, owner, param_id, &scan);
    if (status != FMT_LOCK_STATUS_OK)
        return status;
    if (scan.found == FMT_LOCK_SCAN_NOT_FOUND)
        return FMT_LOCK_STATUS_NOT_FOUND;
    if (fmt_lock_store_runtime.used_count < scan.count)
        return FMT_LOCK_STATUS_CORRUPT;

    idx = scan.index[scan.found];
    previous = scan.found == 0u ? FMT_LOCK_NONE : scan.index[scan.found - 1u];
    next = fmt_lock_store_storage[idx].next;

    if (previous == FMT_LOCK_NONE)
        target->lock_head = next;
    else
        fmt_lock_store_storage[previous].next = next;

    status = fmt_lock_free(idx);
    if (status != FMT_LOCK_STATUS_OK)
        return status;
    return FMT_LOCK_STATUS_OK;
}

FmtLockStatus fmt_lock_clear(uint32_t pattern,
                             uint32_t track,
                             uint32_t step,
                             uint8_t *cleared_count)
{
    FmtStep *target;
    FmtLockScan scan;
    uint16_t owner;
    uint8_t i;
    FmtLockStatus status;

    if (cleared_count)
        *cleared_count = 0u;
    status = fmt_lock_resolve_step(pattern, track, step, &target, &owner);
    if (status != FMT_LOCK_STATUS_OK)
        return status;
    if (!fmt_lock_allocator_basic_valid())
        return FMT_LOCK_STATUS_CORRUPT;
    status = fmt_lock_scan_step(target, owner, FMT_PARAM_NONE, &scan);
    if (status != FMT_LOCK_STATUS_OK)
        return status;
    if (fmt_lock_store_runtime.used_count < scan.count)
        return FMT_LOCK_STATUS_CORRUPT;

    /* Validate all records before mutating so a corrupt list cannot partially
     * clear. The scan above already checked every bounded active record. */
    target->lock_head = FMT_LOCK_NONE;
    for (i = 0u; i < scan.count; i++) {
        status = fmt_lock_free(scan.index[i]);
        if (status != FMT_LOCK_STATUS_OK)
            return status;
    }
    if (cleared_count)
        *cleared_count = scan.count;
    return FMT_LOCK_STATUS_OK;
}

FmtLockStatus fmt_lock_count(uint32_t pattern,
                             uint32_t track,
                             uint32_t step,
                             uint8_t *count)
{
    FmtStep *target;
    FmtLockScan scan;
    uint16_t owner;
    FmtLockStatus status;

    if (!count)
        return FMT_LOCK_STATUS_BAD_ADDRESS;
    status = fmt_lock_resolve_step(pattern, track, step, &target, &owner);
    if (status != FMT_LOCK_STATUS_OK)
        return status;
    if (!fmt_lock_allocator_basic_valid())
        return FMT_LOCK_STATUS_CORRUPT;
    status = fmt_lock_scan_step(target, owner, FMT_PARAM_NONE, &scan);
    if (status != FMT_LOCK_STATUS_OK)
        return status;
    *count = scan.count;
    return FMT_LOCK_STATUS_OK;
}

FmtLockStatus fmt_lock_at(uint32_t pattern,
                          uint32_t track,
                          uint32_t step,
                          uint32_t ordinal,
                          FmtParamId *param_id,
                          uint16_t *value)
{
    FmtStep *target;
    FmtLockScan scan;
    uint16_t owner, idx;
    FmtLockStatus status;

    if (!param_id || !value)
        return FMT_LOCK_STATUS_BAD_ADDRESS;
    status = fmt_lock_resolve_step(pattern, track, step, &target, &owner);
    if (status != FMT_LOCK_STATUS_OK)
        return status;
    if (!fmt_lock_allocator_basic_valid())
        return FMT_LOCK_STATUS_CORRUPT;
    status = fmt_lock_scan_step(target, owner, FMT_PARAM_NONE, &scan);
    if (status != FMT_LOCK_STATUS_OK)
        return status;
    if (ordinal >= scan.count)
        return FMT_LOCK_STATUS_NOT_FOUND;

    idx = scan.index[ordinal];
    *param_id = fmt_lock_store_storage[idx].param_id;
    *value = fmt_lock_store_storage[idx].value;
    return FMT_LOCK_STATUS_OK;
}
