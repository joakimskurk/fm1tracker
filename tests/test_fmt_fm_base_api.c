/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_fm_linked_params.h"

volatile FmtEngineRowParamCache fmt_engine_row_param_cache;

uint8_t fmt_fm_engine_supported(uint8_t engine_id)
{
    return engine_id == FMT_FM_ENGINE_FM4;
}

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

static void empty_row(FmtFmRowOverrides *row)
{
    uint32_t i;
    memset(row, 0, sizeof *row);
    for (i = 0u; i < FMT_FM_MAX_ROW_OVERRIDES; i++)
        row->slot[i].id = FMT_PARAM_NONE;
}

int main(void)
{
    FmtFmParamSet changed;
    FmtFmParamSet before_invalid;
    FmtFmRowOverrides row;
    FmtFmPatch patch;
    FmtFmPatch other_patch;
    FmtEngineRowParamCache cache_before;
    const FmtFmParamSet *base0;
    const FmtFmParamSet *base1;

    memset((void *)&fmt_engine_row_param_cache, 0xA5,
           sizeof fmt_engine_row_param_cache);
    fmt_fm_linked_params_reset();

    base0 = fmt_fm_linked_params_base(0u);
    base1 = fmt_fm_linked_params_base(1u);
    assert(base0 != NULL && base1 != NULL);
    assert(sizeof *base0 == 27u);
    assert(memcmp(base0, base1, sizeof *base0) == 0);

    empty_row(&row);
    assert(fmt_fm_linked_params_compose(0u, &row, &patch));
    assert(patch.algorithm == 2u);
    assert(patch.index_control == 64u);
    assert(patch.ratio_id[0] == 1u);
    assert(patch.envelope.release[3] == 96u);

    cache_before = *(const FmtEngineRowParamCache *)&fmt_engine_row_param_cache;
    changed = *base0;
    changed.algorithm = 255u;
    changed.index = 222u;
    changed.ratio[0] = 240u;
    changed.release[3] = 1u;
    assert(fmt_fm_linked_params_set_base(0u, &changed));

    /* Base-only mutation cannot touch the row publication. That cache contains
     * sparse overrides only and is composed with the latest base at row boundary. */
    assert(memcmp((const void *)&fmt_engine_row_param_cache,
                  &cache_before,
                  sizeof cache_before) == 0);

    assert(fmt_fm_linked_params_compose(0u, &row, &patch));
    assert(patch.algorithm == 3u);
    assert(patch.index_control == 222u);
    assert(patch.ratio_id[0] == 15u);
    assert(patch.envelope.release[3] == 1u);

    /* Instrument 1 remains on the frozen D055-compatible default base. */
    assert(fmt_fm_linked_params_compose(1u, &row, &other_patch));
    assert(other_patch.algorithm == 2u);
    assert(other_patch.index_control == 64u);
    assert(other_patch.ratio_id[0] == 1u);
    assert(other_patch.envelope.release[3] == 96u);

    /* Sparse row overrides still win over the new mutable base. */
    row.slot[0].id = FMT_PARAM_FM_INDEX;
    row.slot[0].value = 17u;
    row.slot[1].id = FMT_PARAM_FM_OP1_RATIO;
    row.slot[1].value = 32u;
    assert(fmt_fm_linked_params_compose(0u, &row, &patch));
    assert(patch.algorithm == 3u);
    assert(patch.index_control == 17u);
    assert(patch.ratio_id[0] == 2u);
    assert(patch.envelope.release[3] == 1u);

    before_invalid = *fmt_fm_linked_params_base(0u);
    assert(!fmt_fm_linked_params_set_base(FMT_INSTRUMENTS, &changed));
    assert(!fmt_fm_linked_params_set_base(0u, NULL));
    assert(memcmp(fmt_fm_linked_params_base(0u),
                  &before_invalid,
                  sizeof before_invalid) == 0);
    assert(fmt_fm_linked_params_base(FMT_INSTRUMENTS) == NULL);

    printf("WP5_8_FM_BASE_API_OK=1 bytes=27 instruments=16 mutable=bounded cache_rebuild=none instrument_isolation=exact plock_override=wins invalid=no_mutation\n");
    return 0;
}
