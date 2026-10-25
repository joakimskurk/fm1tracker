/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_fm_linked_params.h"
#include "fmt_lock_editor.h"
#include "fmt_lock_store.h"
#include "fmt_playback.h"
#include "fmt_playback_params.h"
#include "fmt_store.h"

uint8_t fmt_fm_engine_supported(uint8_t engine_id)
{
    return engine_id == FMT_FM_ENGINE_FM4;
}

static void setup_fm_row(FmtLockEditor *editor)
{
    FmtSong *song;
    FmtStep *step;

    fmt_store_reset();
    fmt_lock_store_reset();
    fmt_8bit_linked_params_reset();
    fmt_fm_linked_params_reset();
    fmt_playback_params_reset();
    fmt_playback_params_rebuild();

    song = fmt_store_song();
    assert(song != NULL);
    song->tracks[0].instrument = 0u;
    song->instruments[0].mode = FMT_MODE_16BIT;
    song->instruments[0].engine = FMT_FM_ENGINE_FM4;
    song->instruments[1].mode = FMT_MODE_8BIT;
    song->instruments[1].engine = FMT_8BIT_ENGINE_PULSE;

    step = fmt_store_step(FMT_PLAYBACK_PATTERN, 0u, 0u);
    assert(step != NULL);
    step->note = 60u;
    step->instrument = 0u;

    assert(fmt_fm_linked_params_refresh_track_row(0u, 0u));

    fmt_lock_editor_reset(editor);
    editor->active = 1u;
    editor->held_key = 0u;
    editor->track = 0u;
    editor->step = 0u;
}

static void assert_lock(FmtParamId id, uint16_t expected)
{
    uint16_t value = 0u;
    assert(fmt_lock_get(FMT_PLAYBACK_PATTERN, 0u, 0u, id, &value) ==
           FMT_LOCK_STATUS_OK);
    assert(value == expected);
}

static void test_page_mapping_reference_and_footer(void)
{
    FmtLockEditor editor;
    FmtFmParamSet base;
    FmtFmRowOverrides row;
    char footer[32];

    setup_fm_row(&editor);
    assert(fmt_fm_linked_editor_is_fm(&editor));

    /* CORE K1 starts from frozen normalized base 128. */
    assert(fmt_fm_linked_editor_apply_delta(&editor,
                                            FMT_FM_PAGE_CORE,
                                            0u,
                                            1));
    assert(editor.last_status == (uint8_t)FMT_LOCK_EDITOR_STATUS_OK);
    assert(editor.last_param == FMT_PARAM_FM_ALGORITHM);
    assert_lock(FMT_PARAM_FM_ALGORITHM, 129u);
    fmt_lock_editor_footer(&editor, footer);
    assert(strcmp(footer, "K1 ALG 129 LOCK") == 0);

    assert(fmt_fm_linked_params_read(0u, 0u, &row));
    assert(row.slot[0].id == FMT_PARAM_FM_ALGORITHM);
    assert(row.slot[0].value == 129u);

    /* CORE K4 is the frozen NONE slot and cannot create a lock. */
    assert(!fmt_fm_linked_editor_apply_delta(&editor,
                                             FMT_FM_PAGE_CORE,
                                             3u,
                                             1));

    /* RATIO K2 uses the mutable instrument base when no lock exists. */
    base = *fmt_fm_linked_params_base(0u);
    base.ratio[1] = 200u;
    assert(fmt_fm_linked_params_set_base(0u, &base));
    assert(fmt_fm_linked_editor_apply_delta(&editor,
                                            FMT_FM_PAGE_RATIO,
                                            1u,
                                            -10));
    assert_lock(FMT_PARAM_FM_OP2_RATIO, 190u);
    fmt_lock_editor_footer(&editor, footer);
    assert(strcmp(footer, "K2 RATIO 190 LOCK") == 0);

    /* Existing lock value, not the base, is the next delta reference. */
    assert(fmt_fm_linked_editor_apply_delta(&editor,
                                            FMT_FM_PAGE_RATIO,
                                            1u,
                                            5));
    assert_lock(FMT_PARAM_FM_OP2_RATIO, 195u);
}

static void test_four_lock_limit_and_delete_refresh(void)
{
    FmtLockEditor editor;
    FmtFmRowOverrides row;
    uint8_t count = 0u;
    uint16_t deleted_value = 0u;

    setup_fm_row(&editor);
    assert(fmt_fm_linked_editor_apply_delta(&editor, FMT_FM_PAGE_CORE, 0u, 1));
    assert(fmt_fm_linked_editor_apply_delta(&editor, FMT_FM_PAGE_CORE, 1u, 1));
    assert(fmt_fm_linked_editor_apply_delta(&editor, FMT_FM_PAGE_CORE, 2u, 1));
    assert(fmt_fm_linked_editor_apply_delta(&editor, FMT_FM_PAGE_ATTACK, 0u, 1));
    assert(fmt_lock_count(FMT_PLAYBACK_PATTERN, 0u, 0u, &count) == FMT_LOCK_STATUS_OK);
    assert(count == 4u);

    assert(fmt_fm_linked_editor_apply_delta(&editor, FMT_FM_PAGE_ATTACK, 1u, 1));
    assert(editor.last_status == (uint8_t)FMT_LOCK_EDITOR_STATUS_STEP_FULL);
    assert(fmt_lock_count(FMT_PLAYBACK_PATTERN, 0u, 0u, &count) == FMT_LOCK_STATUS_OK);
    assert(count == 4u);

    editor.selected_param = FMT_PARAM_FM_INDEX;
    assert(fmt_lock_editor_arm_delete(&editor));
    assert(fmt_fm_linked_editor_confirm_delete(&editor));
    assert(editor.last_status == (uint8_t)FMT_LOCK_EDITOR_STATUS_DELETED);
    assert(fmt_lock_get(FMT_PLAYBACK_PATTERN, 0u, 0u,
                        FMT_PARAM_FM_INDEX, &deleted_value) ==
           FMT_LOCK_STATUS_NOT_FOUND);

    assert(fmt_fm_linked_params_read(0u, 0u, &row));
    assert(row.slot[0].id == FMT_PARAM_FM_ALGORITHM);
    assert(row.slot[1].id == FMT_PARAM_FM_FEEDBACK);
    assert(row.slot[2].id == FMT_PARAM_FM_OP1_ATTACK);
    assert(row.slot[3].id == FMT_PARAM_NONE);
}

static void test_other_family_isolation(void)
{
    FmtLockEditor editor;
    FmtSong *song;
    FmtStep *step;
    uint8_t count = 0u;

    setup_fm_row(&editor);
    song = fmt_store_song();
    step = fmt_store_step(FMT_PLAYBACK_PATTERN, 0u, 0u);
    assert(song != NULL && step != NULL);
    step->instrument = 1u;

    assert(!fmt_fm_linked_editor_is_fm(&editor));
    assert(!fmt_fm_linked_editor_apply_delta(&editor,
                                             FMT_FM_PAGE_LEVEL,
                                             0u,
                                             1));
    assert(fmt_lock_count(FMT_PLAYBACK_PATTERN, 0u, 0u, &count) == FMT_LOCK_STATUS_OK);
    assert(count == 0u);
}

int main(void)
{
    test_page_mapping_reference_and_footer();
    test_four_lock_limit_and_delete_refresh();
    test_other_family_isolation();
    printf("WP5_8_FM_EDITOR_OK=1 pages=7 slots=4 base_reference=exact existing_lock=exact limit=4 delete_refresh=one_row footer=semantic other_family=unchanged\n");
    return 0;
}
