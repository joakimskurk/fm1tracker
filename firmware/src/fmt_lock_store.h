/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_LOCK_STORE_H
#define FM1TRACKER_FMT_LOCK_STORE_H

#include <stddef.h>
#include <stdint.h>

#include "fmt_model.h"
#include "fmt_param_registry.h"

/* D028: one global sparse pool shared by the complete song. */
#define FMT_LOCK_OWNER_NONE FMT_LOCK_NONE
#define FMT_LOCK_TOTAL_STEPS (FMT_PATTERN_SLOTS * FMT_TRACKS * FMT_STEPS_PER_PATTERN)

enum {
    FMT_LOCK_STATE_FREE = 0u,
    FMT_LOCK_STATE_ACTIVE = 1u
};

typedef struct {
    uint16_t next;
    uint16_t value;
    uint16_t owner;
    FmtParamId param_id;
    uint8_t state;
} FmtLock;

typedef struct {
    uint16_t free_head;
    uint16_t used_count;
} FmtLockStoreRuntime;

typedef enum {
    FMT_LOCK_STATUS_OK = 0,
    FMT_LOCK_STATUS_NOT_FOUND = 1,
    FMT_LOCK_STATUS_INVALID_PARAM = 2,
    FMT_LOCK_STATUS_STEP_FULL = 3,
    FMT_LOCK_STATUS_POOL_FULL = 4,
    FMT_LOCK_STATUS_BAD_ADDRESS = 5,
    FMT_LOCK_STATUS_CORRUPT = 6
} FmtLockStatus;

FMT_STATIC_ASSERT(lock_record_size_is_8, sizeof(FmtLock) == 8u);
FMT_STATIC_ASSERT(lock_next_offset_is_0, offsetof(FmtLock, next) == 0u);
FMT_STATIC_ASSERT(lock_value_offset_is_2, offsetof(FmtLock, value) == 2u);
FMT_STATIC_ASSERT(lock_owner_offset_is_4, offsetof(FmtLock, owner) == 4u);
FMT_STATIC_ASSERT(lock_param_offset_is_6, offsetof(FmtLock, param_id) == 6u);
FMT_STATIC_ASSERT(lock_state_offset_is_7, offsetof(FmtLock, state) == 7u);
FMT_STATIC_ASSERT(lock_runtime_size_is_4, sizeof(FmtLockStoreRuntime) == 4u);
FMT_STATIC_ASSERT(lock_records_match_reserved_budget,
                  sizeof(FmtLock) * FMT_LOCK_POOL_MAX == FMT_LOCK_STORAGE_BUDGET);
FMT_STATIC_ASSERT(lock_total_steps_fit_owner,
                  FMT_LOCK_TOTAL_STEPS < FMT_LOCK_OWNER_NONE);

/* Low-level allocator reset. The normal complete-song reset is fmt_store_reset(),
 * which resets all step heads and this allocator together. */
void fmt_lock_store_reset(void);

uint16_t fmt_lock_store_used_count(void);
uint16_t fmt_lock_store_free_count(void);

/* Exact D028 linear owner mapping. */
FmtLockStatus fmt_lock_owner_id(uint32_t pattern,
                                uint32_t track,
                                uint32_t step,
                                uint16_t *owner);

/* All public operations address the one runtime song by bounded coordinates.
 * A mutating operation validates the complete existing <=4-record step list
 * before changing either the step or allocator. */
FmtLockStatus fmt_lock_set(uint32_t pattern,
                           uint32_t track,
                           uint32_t step,
                           FmtParamId param_id,
                           uint16_t value,
                           uint8_t *replaced);

FmtLockStatus fmt_lock_get(uint32_t pattern,
                           uint32_t track,
                           uint32_t step,
                           FmtParamId param_id,
                           uint16_t *value);

FmtLockStatus fmt_lock_remove(uint32_t pattern,
                              uint32_t track,
                              uint32_t step,
                              FmtParamId param_id);

FmtLockStatus fmt_lock_clear(uint32_t pattern,
                             uint32_t track,
                             uint32_t step,
                             uint8_t *cleared_count);

FmtLockStatus fmt_lock_count(uint32_t pattern,
                             uint32_t track,
                             uint32_t step,
                             uint8_t *count);

/* Ordinal iteration follows the frozen strict ascending parameter-ID order. */
FmtLockStatus fmt_lock_at(uint32_t pattern,
                          uint32_t track,
                          uint32_t step,
                          uint32_t ordinal,
                          FmtParamId *param_id,
                          uint16_t *value);

#endif /* FM1TRACKER_FMT_LOCK_STORE_H */
