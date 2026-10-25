/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "fmt_fm_linked_params.h"
#include "fmt_lock_store.h"
#include "fmt_store.h"

/* Test-only access to D028's externally visible pool lets this gate prove the
 * resolver's structural-corruption fail-safe without adding a production API. */
extern FmtLock fmt_lock_store_storage[FMT_LOCK_POOL_MAX];

/* Keep the focused resolver binary independent of the frozen D055 engine
 * implementation while preserving its exact one-engine identity contract. */
uint8_t fmt_fm_engine_supported(uint8_t engine_id)
{
    return engine_id == FMT_FM_ENGINE_FM4;
}

/* fmt_8bit_linked_params.c contains editor adapters outside this gate's scope.
 * Narrow stubs satisfy those unused references so the real shared-cache code is
 * linked and exercised here. */
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

static void expect_empty(uint32_t track, uint32_t row)
{
    FmtFmRowOverrides out;
    uint32_t i;

    assert(fmt_fm_linked_params_read(track, row, &out));
    for (i = 0u; i < FMT_FM_MAX_ROW_OVERRIDES; i++) {
        assert(out.slot[i].id == FMT_PARAM_NONE);
        assert(out.slot[i].value == 0u);
    }
}

static void setup_song(void)
{
    FmtSong *song;
    FmtStep *step;

    fmt_store_reset();
    fmt_lock_store_reset();
    fmt_8bit_linked_params_reset();
    fmt_fm_linked_params_reset();

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

    /* EMPTY/TIE/OFF rows deliberately carry explicit instrument bytes. They
     * must not alter NOTE-only inheritance. */
    step = fmt_store_step(FMT_PLAYBACK_PATTERN, 0u, 1u);
    assert(step != NULL);
    step->note = FMT_NOTE_EMPTY;
    step->instrument = 1u;
    step = fmt_store_step(FMT_PLAYBACK_PATTERN, 0u, 2u);
    assert(step != NULL);
    step->note = FMT_NOTE_TIE;
    step->instrument = 1u;
    step = fmt_store_step(FMT_PLAYBACK_PATTERN, 0u, 3u);
    assert(step != NULL);
    step->note = FMT_NOTE_OFF;
    step->instrument = 1u;

    step = fmt_store_step(FMT_PLAYBACK_PATTERN, 0u, 4u);
    assert(step != NULL);
    step->note = 62u;
    step->instrument = 1u;

    step = fmt_store_step(FMT_PLAYBACK_PATTERN, 0u, 6u);
    assert(step != NULL);
    step->note = 64u;
    step->instrument = 0u;
}

static void test_filtering_and_note_inheritance(void)
{
    FmtFmRowOverrides out;
    uint8_t replaced = 0u;

    setup_song();

    /* Four stored locks are the frozen maximum. Only two are legal FM byte
     * overrides: wrong-family 0x20 and raw 256 remain stored but are filtered. */
    assert(fmt_lock_set(FMT_PLAYBACK_PATTERN, 0u, 0u,
                        FMT_PARAM_BIT8_ATTACK, 12u, &replaced) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(FMT_PLAYBACK_PATTERN, 0u, 0u,
                        FMT_PARAM_FM_ALGORITHM, 256u, &replaced) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(FMT_PLAYBACK_PATTERN, 0u, 0u,
                        FMT_PARAM_FM_INDEX, 77u, &replaced) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(FMT_PLAYBACK_PATTERN, 0u, 0u,
                        FMT_PARAM_FM_FEEDBACK, 33u, &replaced) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(FMT_PLAYBACK_PATTERN, 0u, 0u,
                        FMT_PARAM_FM_OP1_RATIO, 31u, &replaced) == FMT_LOCK_STATUS_STEP_FULL);

    assert(fmt_fm_linked_params_refresh_track_row(0u, 0u));
    assert(fmt_fm_linked_params_read(0u, 0u, &out));
    assert(out.slot[0].id == FMT_PARAM_FM_INDEX && out.slot[0].value == 77u);
    assert(out.slot[1].id == FMT_PARAM_FM_FEEDBACK && out.slot[1].value == 33u);
    assert(out.slot[2].id == FMT_PARAM_NONE);
    assert(out.slot[3].id == FMT_PARAM_NONE);

    /* EMPTY, TIE and OFF ignore their explicit instrument bytes and inherit FM. */
    assert(fmt_lock_set(FMT_PLAYBACK_PATTERN, 0u, 1u,
                        FMT_PARAM_FM_OP1_RATIO, 31u, &replaced) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(FMT_PLAYBACK_PATTERN, 0u, 2u,
                        FMT_PARAM_FM_OP2_LEVEL, 88u, &replaced) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(FMT_PLAYBACK_PATTERN, 0u, 3u,
                        FMT_PARAM_FM_OP3_RELEASE, 55u, &replaced) == FMT_LOCK_STATUS_OK);
    assert(fmt_fm_linked_params_refresh_track_row(0u, 1u));
    assert(fmt_fm_linked_params_refresh_track_row(0u, 2u));
    assert(fmt_fm_linked_params_refresh_track_row(0u, 3u));
    assert(fmt_fm_linked_params_read(0u, 1u, &out));
    assert(out.slot[0].id == FMT_PARAM_FM_OP1_RATIO && out.slot[0].value == 31u);
    assert(fmt_fm_linked_params_read(0u, 2u, &out));
    assert(out.slot[0].id == FMT_PARAM_FM_OP2_LEVEL && out.slot[0].value == 88u);
    assert(fmt_fm_linked_params_read(0u, 3u, &out));
    assert(out.slot[0].id == FMT_PARAM_FM_OP3_RELEASE && out.slot[0].value == 55u);
}

static void test_other_family_no_publish_and_shared_low_word(void)
{
    FmtFmOverride sentinel = { FMT_PARAM_FM_INDEX, 123u };
    FmtFmRowOverrides out;
    uint32_t low_before;
    uint8_t bank_before;
    uint8_t replaced = 0u;

    setup_song();

    /* Row 4 switches to 8BIT. FM refresh must not touch an existing publication
     * because this row's shared low word belongs to the other family. */
    assert(fmt_fm_linked_params_publish(0u, 4u, &sentinel, 1u));
    bank_before = fmt_engine_row_param_cache.active_bank[0][4];
    assert(fmt_lock_set(FMT_PLAYBACK_PATTERN, 0u, 4u,
                        FMT_PARAM_FM_FEEDBACK, 99u, &replaced) == FMT_LOCK_STATUS_OK);
    assert(fmt_fm_linked_params_refresh_track_row(0u, 4u));
    assert(fmt_engine_row_param_cache.active_bank[0][4] == bank_before);
    assert(fmt_fm_linked_params_read(0u, 4u, &out));
    assert(out.slot[0].id == FMT_PARAM_FM_INDEX && out.slot[0].value == 123u);

    /* Row 6 switches back to FM. Two refreshes leave the active payload in
     * bank0-low, i.e. the exact legacy 8BIT word. An 8BIT refresh must no-op. */
    assert(fmt_lock_set(FMT_PLAYBACK_PATTERN, 0u, 6u,
                        FMT_PARAM_FM_INDEX, 201u, &replaced) == FMT_LOCK_STATUS_OK);
    assert(fmt_fm_linked_params_refresh_track_row(0u, 6u));
    assert(fmt_fm_linked_params_refresh_track_row(0u, 6u));
    assert((fmt_engine_row_param_cache.active_bank[0][6] & 1u) == 0u);
    low_before = fmt_engine_row_param_cache.legacy.word[0][6];
    assert(low_before != 0u);
    assert(fmt_8bit_linked_params_refresh_track_row(0u, 6u));
    assert(fmt_engine_row_param_cache.legacy.word[0][6] == low_before);
    assert((fmt_engine_row_param_cache.active_bank[0][6] & 1u) == 0u);
    assert(fmt_fm_linked_params_read(0u, 6u, &out));
    assert(out.slot[0].id == FMT_PARAM_FM_INDEX && out.slot[0].value == 201u);
}

static void test_structural_and_malformed_failsafe(void)
{
    FmtFmOverride sentinel = { FMT_PARAM_FM_FEEDBACK, 222u };
    FmtStep *step;
    uint16_t idx;
    uint8_t replaced = 0u;

    setup_song();

    /* Row 7 inherits FM from row 6. Corrupt its one D028 node after a valid
     * write; resolver must replace stale FM data with one empty publication. */
    assert(fmt_lock_set(FMT_PLAYBACK_PATTERN, 0u, 7u,
                        FMT_PARAM_FM_INDEX, 90u, &replaced) == FMT_LOCK_STATUS_OK);
    assert(fmt_fm_linked_params_publish(0u, 7u, &sentinel, 1u));
    step = fmt_store_step(FMT_PLAYBACK_PATTERN, 0u, 7u);
    assert(step != NULL && step->lock_head < FMT_LOCK_POOL_MAX);
    idx = step->lock_head;
    fmt_lock_store_storage[idx].state = FMT_LOCK_STATE_FREE;
    assert(fmt_fm_linked_params_refresh_track_row(0u, 7u));
    expect_empty(0u, 7u);

    /* A malformed explicit instrument on a normal NOTE is fail-safe for an
     * inherited FM row. It must also clear stale FM overrides. */
    step = fmt_store_step(FMT_PLAYBACK_PATTERN, 0u, 8u);
    assert(step != NULL);
    step->note = 65u;
    step->instrument = (uint8_t)(FMT_INSTRUMENTS + 1u);
    assert(fmt_fm_linked_params_publish(0u, 8u, &sentinel, 1u));
    assert(fmt_fm_linked_params_refresh_track_row(0u, 8u));
    expect_empty(0u, 8u);
}

static void test_rebuild_and_default_composition(void)
{
    FmtFmRowOverrides row;
    FmtFmPatch patch;
    uint8_t replaced = 0u;

    setup_song();
    assert(fmt_lock_set(FMT_PLAYBACK_PATTERN, 0u, 0u,
                        FMT_PARAM_FM_INDEX, 111u, &replaced) == FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(FMT_PLAYBACK_PATTERN, 0u, 6u,
                        FMT_PARAM_FM_FEEDBACK, 44u, &replaced) == FMT_LOCK_STATUS_OK);
    assert(fmt_fm_linked_params_rebuild_track(0u));

    assert(fmt_fm_linked_params_read(0u, 0u, &row));
    assert(row.slot[0].id == FMT_PARAM_FM_INDEX && row.slot[0].value == 111u);
    assert(fmt_fm_linked_params_read(0u, 6u, &row));
    assert(row.slot[0].id == FMT_PARAM_FM_FEEDBACK && row.slot[0].value == 44u);

    /* Empty published row plus normalized base still reproduces exact D055. */
    assert(fmt_fm_linked_params_publish(0u, 9u, NULL, 0u));
    assert(fmt_fm_linked_params_read(0u, 9u, &row));
    assert(fmt_fm_linked_params_compose(0u, &row, &patch));
    assert(patch.ratio_id[0] == 1u && patch.ratio_id[1] == 5u);
    assert(patch.ratio_id[2] == 1u && patch.ratio_id[3] == 5u);
    assert(patch.algorithm == 2u);
    assert(patch.index_control == 64u);
    assert(patch.feedback_control == 0u);
}

int main(void)
{
    test_filtering_and_note_inheritance();
    test_other_family_no_publish_and_shared_low_word();
    test_structural_and_malformed_failsafe();
    test_rebuild_and_default_composition();
    printf("WP5_8_STORE_RESOLVER_OK=1 note_inherit=exact locks=4 uint16_filter=no_truncate wrong_family=ignored structural=failsafe shared_low=protected\n");
    return 0;
}
