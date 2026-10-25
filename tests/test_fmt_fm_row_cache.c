/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_fm_linked_params.h"

/* Isolated host fixture for the one production allocation. */
volatile FmtEngineRowParamCache fmt_engine_row_param_cache;

/* The row-cache unit stays independent of the frozen D055 implementation. */
uint8_t fmt_fm_engine_supported(uint8_t engine_id)
{
    return engine_id == FMT_FM_ENGINE_FM4;
}

/* D056's main-only editor adapter lives in the same production translation
 * unit. Keep this cache-only host focused by satisfying those unused seams. */
int fmt_playback_params_refresh_track_step(uint32_t track, uint32_t step)
{
    (void)track;
    (void)step;
    return 1;
}

int fmt_lock_editor_apply_param_delta(FmtLockEditor *editor,
                                      FmtParamId param_id,
                                      uint16_t reference_value,
                                      int32_t delta)
{
    (void)editor;
    (void)param_id;
    (void)reference_value;
    (void)delta;
    return 0;
}

int fmt_lock_editor_confirm_delete(FmtLockEditor *editor)
{
    (void)editor;
    return 0;
}

static uint32_t pack(FmtParamId a_id, uint8_t a_value,
                     FmtParamId b_id, uint8_t b_value)
{
    return (uint32_t)a_id |
           ((uint32_t)a_value << 8) |
           ((uint32_t)b_id << 16) |
           ((uint32_t)b_value << 24);
}

static void test_exact_layout(void)
{
    assert(sizeof(Fmt8BitRowParamCache) == 2048u);
    assert(sizeof(FmtEngineRowParamCache) == 8704u);
    assert(offsetof(FmtEngineRowParamCache, legacy) == 0u);
    assert(offsetof(FmtEngineRowParamCache, bank0_high) == 2048u);
    assert(offsetof(FmtEngineRowParamCache, bank1_low) == 4096u);
    assert(offsetof(FmtEngineRowParamCache, bank1_high) == 6144u);
    assert(offsetof(FmtEngineRowParamCache, active_bank) == 8192u);
    assert(sizeof(fmt_fm_instrument_base) == 432u);
    assert((const volatile void *)&fmt_8bit_row_param_cache.word[0][0] ==
           (const volatile void *)&fmt_engine_row_param_cache);
}

static void test_publish_sort_and_flip(void)
{
    FmtFmOverride input[4] = {
        { FMT_PARAM_FM_OP1_RELEASE, 11u },
        { FMT_PARAM_FM_ALGORITHM, 255u },
        { FMT_PARAM_FM_OP1_RATIO, 33u },
        { FMT_PARAM_FM_FEEDBACK, 44u }
    };
    FmtFmRowOverrides out;
    uint8_t before;

    memset((void *)&fmt_engine_row_param_cache, 0,
           sizeof fmt_engine_row_param_cache);
    fmt_engine_row_param_cache.legacy.word[2][3] = 0xDEADBEEFu;
    assert(fmt_engine_row_param_cache.active_bank[2][3] == 0u);

    assert(fmt_fm_linked_params_publish(2u, 3u, input, 4u));
    assert(fmt_engine_row_param_cache.active_bank[2][3] == 1u);
    assert(fmt_engine_row_param_cache.legacy.word[2][3] == 0xDEADBEEFu);
    assert(fmt_fm_linked_params_read(2u, 3u, &out));
    assert(out.slot[0].id == FMT_PARAM_FM_ALGORITHM && out.slot[0].value == 255u);
    assert(out.slot[1].id == FMT_PARAM_FM_FEEDBACK && out.slot[1].value == 44u);
    assert(out.slot[2].id == FMT_PARAM_FM_OP1_RATIO && out.slot[2].value == 33u);
    assert(out.slot[3].id == FMT_PARAM_FM_OP1_RELEASE && out.slot[3].value == 11u);

    assert(fmt_fm_linked_params_publish(2u, 3u, input, 1u));
    assert(fmt_engine_row_param_cache.active_bank[2][3] == 0u);
    assert(fmt_fm_linked_params_read(2u, 3u, &out));
    assert(out.slot[0].id == FMT_PARAM_FM_OP1_RELEASE && out.slot[0].value == 11u);
    assert(out.slot[1].id == FMT_PARAM_NONE);
    assert(out.slot[2].id == FMT_PARAM_NONE);
    assert(out.slot[3].id == FMT_PARAM_NONE);

    before = fmt_engine_row_param_cache.active_bank[2][3];
    input[0].id = FMT_PARAM_BIT8_ATTACK;
    assert(!fmt_fm_linked_params_publish(2u, 3u, input, 1u));
    assert(fmt_engine_row_param_cache.active_bank[2][3] == before);
    input[0].id = FMT_PARAM_FM_INDEX;
    input[1] = input[0];
    assert(!fmt_fm_linked_params_publish(2u, 3u, input, 2u));
    assert(fmt_engine_row_param_cache.active_bank[2][3] == before);
    assert(!fmt_fm_linked_params_publish(2u, 3u, input, 5u));
    assert(!fmt_fm_linked_params_publish(FMT_TRACKS, 0u, input, 1u));
}

static void test_preemption_model(void)
{
    FmtFmRowOverrides out;
    const uint32_t old_low = pack(FMT_PARAM_FM_INDEX, 1u,
                                  FMT_PARAM_NONE, 0u);
    const uint32_t new_low = pack(FMT_PARAM_FM_FEEDBACK, 2u,
                                  FMT_PARAM_FM_OP1_RATIO, 3u);
    const uint32_t new_high = pack(FMT_PARAM_FM_OP1_LEVEL, 4u,
                                   FMT_PARAM_NONE, 0u);

    memset((void *)&fmt_engine_row_param_cache, 0,
           sizeof fmt_engine_row_param_cache);
    fmt_engine_row_param_cache.legacy.word[1][7] = old_low;
    fmt_engine_row_param_cache.active_bank[1][7] = 0u;

    /* Simulate main being preempted after writing the inactive payload but
     * before publishing its selector. Audio must still see bank 0 only. */
    fmt_engine_row_param_cache.bank1_low[1][7] = new_low;
    fmt_engine_row_param_cache.bank1_high[1][7] = new_high;
    assert(fmt_fm_linked_params_read(1u, 7u, &out));
    assert(out.slot[0].id == FMT_PARAM_FM_INDEX && out.slot[0].value == 1u);
    assert(out.slot[1].id == FMT_PARAM_NONE);

    fmt_engine_row_param_cache.active_bank[1][7] = 1u;
    assert(fmt_fm_linked_params_read(1u, 7u, &out));
    assert(out.slot[0].id == FMT_PARAM_FM_FEEDBACK && out.slot[0].value == 2u);
    assert(out.slot[1].id == FMT_PARAM_FM_OP1_RATIO && out.slot[1].value == 3u);
    assert(out.slot[2].id == FMT_PARAM_FM_OP1_LEVEL && out.slot[2].value == 4u);
}

static void test_base_and_compose(void)
{
    FmtFmRowOverrides row;
    FmtFmPatch patch;
    const FmtFmParamSet *base;

    fmt_fm_linked_params_reset();
    base = fmt_fm_linked_params_base(0u);
    assert(base != NULL);
    assert(base->algorithm == 128u && base->index == 64u);

    memset(&row, 0, sizeof row);
    row.slot[0].id = FMT_PARAM_FM_INDEX;
    row.slot[0].value = 201u;
    assert(fmt_fm_linked_params_compose(0u, &row, &patch));
    assert(patch.algorithm == 2u);
    assert(patch.index_control == 201u);
    assert(patch.feedback_control == 0u);
    assert(patch.ratio_id[0] == 1u && patch.ratio_id[1] == 5u);

    row.slot[1].id = FMT_PARAM_BIT8_ATTACK;
    assert(!fmt_fm_linked_params_compose(0u, &row, &patch));
    assert(!fmt_fm_linked_params_compose(FMT_INSTRUMENTS, &row, &patch));
}

int main(void)
{
    test_exact_layout();
    test_publish_sort_and_flip();
    test_preemption_model();
    test_base_and_compose();
    printf("WP5_8_ROW_CACHE_OK=1 total=8704 legacy=2048 base_bank=432 slots=4 banks=2 selector=byte preemption=deterministic\n");
    return 0;
}
