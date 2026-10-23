/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "fmt_lock_store.h"
#include "fmt_store.h"

/* D027 intentionally exposes only four production-lockable IDs today. This
 * focused harness stubs lockability so WP3.2 can exercise the future fifth-ID
 * rejection branch without assigning a fifth production semantic ID. */
int fmt_param_is_lockable(FmtParamId id)
{
    return id >= 1u && id <= 5u;
}

int main(void)
{
    uint8_t replaced = 0u;
    uint8_t count = 0u;
    uint16_t used_before;
    FmtStep *step;

    fmt_store_reset();
    fmt_lock_store_reset();
    step = fmt_store_step(0u, 0u, 0u);
    assert(step != NULL);

    assert(fmt_lock_set(0u, 0u, 0u, 1u, 10u, NULL) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(0u, 0u, 0u, 2u, 20u, NULL) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(0u, 0u, 0u, 3u, 30u, NULL) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(0u, 0u, 0u, 4u, 40u, NULL) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_count(0u, 0u, 0u, &count) == FMT_LOCK_STATUS_OK);
    assert(count == FMT_LOCKS_PER_STEP);

    used_before = fmt_lock_store_used_count();
    assert(fmt_lock_set(0u, 0u, 0u, 5u, 50u, &replaced) == FMT_LOCK_STATUS_STEP_FULL);
    assert(replaced == 0u);
    assert(fmt_lock_store_used_count() == used_before);
    assert(fmt_lock_count(0u, 0u, 0u, &count) == FMT_LOCK_STATUS_OK);
    assert(count == FMT_LOCKS_PER_STEP);

    printf("FMT_LOCK_LIMIT_TESTS_OK=1\n");
    printf("max_per_step=%u fifth_prevalidated=STEP_FULL used_unchanged=1\n",
           (unsigned)FMT_LOCKS_PER_STEP);
    return 0;
}
