/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_lock_editor.h"
#include "fmt_store.h"

extern FmtLock fmt_lock_store_storage[FMT_LOCK_POOL_MAX];

static const uint8_t WHITE_KEYS[16] = {
    0u, 2u, 4u, 6u, 7u, 9u, 11u, 12u,
    14u, 16u, 18u, 19u, 21u, 23u, 24u, 26u
};

static const FmtParamId PARAMS[4] = {
    (FmtParamId)FMT_PARAM_VOICE_LEVEL,
    (FmtParamId)FMT_PARAM_PITCH_FINE,
    (FmtParamId)FMT_PARAM_TRACK_LEVEL,
    (FmtParamId)FMT_PARAM_TRACK_PAN
};

static void reset_all(FmtPatternInput *input, FmtLockEditor *editor)
{
    fmt_store_reset();
    fmt_lock_store_reset();
    fmt_pattern_input_reset(input);
    fmt_pattern_input_enter_pattern(input);
    fmt_lock_editor_reset(editor);
}

static void add_lock(uint32_t track, uint32_t step, FmtParamId param, uint16_t value)
{
    uint8_t replaced = 0u;
    assert(fmt_lock_set(0u, track, step, param, value, &replaced) == FMT_LOCK_STATUS_OK);
    assert(replaced == 0u);
}

static void add_four_unsorted(uint32_t track, uint32_t step)
{
    add_lock(track, step, (FmtParamId)FMT_PARAM_TRACK_PAN, 40u);
    add_lock(track, step, (FmtParamId)FMT_PARAM_VOICE_LEVEL, 10u);
    add_lock(track, step, (FmtParamId)FMT_PARAM_TRACK_LEVEL, 30u);
    add_lock(track, step, (FmtParamId)FMT_PARAM_PITCH_FINE, 20u);
}

static void test_layout_mapping_and_footer(void)
{
    FmtPatternInput input;
    FmtLockEditor editor;
    char text[32];
    uint32_t k;
    const uint16_t reference[4] = {255u, 128u, 255u, 128u};

    reset_all(&input, &editor);
    assert(sizeof(FmtLockEditor) == 10u);
    assert(!fmt_lock_editor_is_active(&editor));
    assert(editor.held_key == FMT_LOCK_EDITOR_NO_KEY);
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_NONE);
    assert(!editor.delete_armed);

    for (k = 0u; k < 4u; k++) {
        FmtParamId id = (FmtParamId)FMT_PARAM_INVALID;
        uint16_t ref = 0u;
        assert(fmt_lock_editor_knob_param(k, &id, &ref));
        assert(id == PARAMS[k]);
        assert(ref == reference[k]);
    }
    assert(!fmt_lock_editor_knob_param(4u, (FmtParamId *)0, (uint16_t *)0));
    assert(!fmt_lock_editor_knob_param(UINT32_MAX, (FmtParamId *)0, (uint16_t *)0));

    assert(fmt_lock_editor_begin(&editor, &input, WHITE_KEYS[4]));
    assert(editor.track == 0u);
    assert(editor.step == 4u);
    assert(editor.held_key == WHITE_KEYS[4]);
    assert(fmt_lock_editor_key_held(&editor, 1u << WHITE_KEYS[4]));
    assert(!fmt_lock_editor_key_held(&editor, 0u));

    fmt_lock_editor_footer(&editor, text);
    assert(strcmp(text, "PLOCK S05 T1 K1-4") == 0);

    assert(fmt_lock_editor_apply_delta(&editor, 1u, -3));
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_PITCH_FINE);
    fmt_lock_editor_footer(&editor, text);
    assert(strcmp(text, "K2 FINE 125 LOCK") == 0);

    fmt_lock_editor_cancel(&editor);
    assert(!fmt_lock_editor_is_active(&editor));
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_NONE);
    assert(!editor.delete_armed);
    fmt_lock_editor_footer(&editor, text);
    assert(text[0] == 0);
}

static void test_all_white_targets_and_tracks(void)
{
    FmtPatternInput input;
    FmtLockEditor editor;
    uint32_t page, local, track;

    for (page = 0u; page < FMT_PATTERN_INPUT_PAGE_COUNT; page++) {
        for (local = 0u; local < 16u; local++) {
            reset_all(&input, &editor);
            input.page = (uint8_t)page;
            assert(fmt_lock_editor_begin(&editor, &input, WHITE_KEYS[local]));
            assert(editor.step == page * 16u + local);
            assert(input.local_step == local);
            fmt_lock_editor_cancel(&editor);
        }
    }

    for (track = 0u; track < FMT_TRACKS; track++) {
        reset_all(&input, &editor);
        input.selected_track = (uint8_t)track;
        input.page = 3u;
        assert(fmt_lock_editor_begin(&editor, &input, WHITE_KEYS[15]));
        assert(editor.track == track);
        assert(editor.step == 63u);
    }

    reset_all(&input, &editor);
    assert(!fmt_lock_editor_begin(&editor, &input, 1u));
    assert(!fmt_lock_editor_begin(&editor, &input, 27u));
    fmt_pattern_input_enter_performance(&input);
    assert(!fmt_lock_editor_begin(&editor, &input, WHITE_KEYS[0]));
}

static void test_first_write_replace_clamp_and_isolation(void)
{
    FmtPatternInput input;
    FmtLockEditor editor;
    FmtStep *target, before, neighbor_before;
    const FmtStep *neighbor;
    uint16_t value;
    uint16_t head_after_first;
    uint32_t used;

    reset_all(&input, &editor);
    input.selected_track = 3u;
    input.page = 1u;
    target = fmt_store_step(0u, 3u, 18u);
    neighbor = fmt_store_step_const(0u, 3u, 19u);
    assert(target && neighbor);
    target->note = 64u;
    target->instrument = 7u;
    target->volume = 99u;
    target->effect = 6u;
    target->effect_value = 44u;
    target->flags = 0xA5u;
    before = *target;
    neighbor_before = *neighbor;

    assert(fmt_lock_editor_begin(&editor, &input, WHITE_KEYS[2]));
    assert(editor.track == 3u && editor.step == 18u);
    assert(!fmt_lock_editor_apply_delta(&editor, 0u, 0));
    assert(fmt_lock_store_used_count() == 0u);

    assert(fmt_lock_editor_apply_delta(&editor, 0u, -1));
    assert(editor.last_status == FMT_LOCK_EDITOR_STATUS_OK);
    assert(editor.last_value == 254u);
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_VOICE_LEVEL);
    assert(fmt_lock_get(0u, 3u, 18u, (FmtParamId)FMT_PARAM_VOICE_LEVEL, &value) == FMT_LOCK_STATUS_OK);
    assert(value == 254u);
    assert(fmt_lock_store_used_count() == 1u);
    head_after_first = target->lock_head;

    assert(fmt_lock_editor_apply_delta(&editor, 0u, 7));
    assert(editor.last_value == 255u);
    assert(fmt_lock_get(0u, 3u, 18u, (FmtParamId)FMT_PARAM_VOICE_LEVEL, &value) == FMT_LOCK_STATUS_OK);
    assert(value == 255u);
    assert(fmt_lock_store_used_count() == 1u);
    assert(target->lock_head == head_after_first);

    assert(fmt_lock_editor_apply_delta(&editor, 1u, -200));
    assert(fmt_lock_get(0u, 3u, 18u, (FmtParamId)FMT_PARAM_PITCH_FINE, &value) == FMT_LOCK_STATUS_OK);
    assert(value == 0u);
    assert(fmt_lock_editor_apply_delta(&editor, 2u, -500));
    assert(fmt_lock_get(0u, 3u, 18u, (FmtParamId)FMT_PARAM_TRACK_LEVEL, &value) == FMT_LOCK_STATUS_OK);
    assert(value == 0u);
    assert(fmt_lock_editor_apply_delta(&editor, 3u, 500));
    assert(fmt_lock_get(0u, 3u, 18u, (FmtParamId)FMT_PARAM_TRACK_PAN, &value) == FMT_LOCK_STATUS_OK);
    assert(value == 255u);
    assert(fmt_lock_store_used_count() == 4u);
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_TRACK_PAN);

    used = fmt_lock_store_used_count();
    assert(target->note == before.note);
    assert(target->instrument == before.instrument);
    assert(target->volume == before.volume);
    assert(target->effect == before.effect);
    assert(target->effect_value == before.effect_value);
    assert(target->flags == before.flags);
    assert(target->lock_head != before.lock_head);
    assert(memcmp(neighbor, &neighbor_before, sizeof neighbor_before) == 0);
    assert(used == 4u);
}

static void test_snapshot_survives_cursor_track_changes(void)
{
    FmtPatternInput input;
    FmtLockEditor editor;
    uint16_t value;

    reset_all(&input, &editor);
    input.selected_track = 5u;
    input.page = 2u;
    assert(fmt_lock_editor_begin(&editor, &input, WHITE_KEYS[6]));
    assert(editor.track == 5u && editor.step == 38u);

    input.selected_track = 1u;
    input.page = 0u;
    input.local_step = 0u;
    assert(fmt_lock_editor_apply_delta(&editor, 3u, -1));
    assert(fmt_lock_get(0u, 5u, 38u, (FmtParamId)FMT_PARAM_TRACK_PAN, &value) == FMT_LOCK_STATUS_OK);
    assert(value == 127u);
    assert(fmt_lock_get(0u, 1u, 0u, (FmtParamId)FMT_PARAM_TRACK_PAN, &value) == FMT_LOCK_STATUS_NOT_FOUND);
}

static void test_range_rejection(void)
{
    FmtPatternInput input;
    FmtLockEditor editor;
    uint8_t replaced;
    uint16_t value;
    uint16_t used;
    char text[32];

    reset_all(&input, &editor);
    assert(fmt_lock_set(0u, 0u, 0u, (FmtParamId)FMT_PARAM_PITCH_FINE, 500u, &replaced) == FMT_LOCK_STATUS_OK);
    used = fmt_lock_store_used_count();
    assert(fmt_lock_editor_begin(&editor, &input, WHITE_KEYS[0]));
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_PITCH_FINE);
    assert(fmt_lock_editor_apply_delta(&editor, 1u, 1));
    assert(editor.last_status == FMT_LOCK_EDITOR_STATUS_RANGE);
    assert(editor.last_value == 500u);
    assert(fmt_lock_store_used_count() == used);
    assert(fmt_lock_get(0u, 0u, 0u, (FmtParamId)FMT_PARAM_PITCH_FINE, &value) == FMT_LOCK_STATUS_OK);
    assert(value == 500u);
    fmt_lock_editor_footer(&editor, text);
    assert(strcmp(text, "PLOCK RANGE") == 0);
}

static void fill_pool_except_target_pan(void)
{
    uint32_t pattern, track, step, p;
    uint8_t replaced;

    for (p = 0u; p < 3u; p++)
        assert(fmt_lock_set(0u, 0u, 0u, PARAMS[p], (uint16_t)(10u + p), &replaced) == FMT_LOCK_STATUS_OK);

    for (pattern = 0u; pattern < FMT_PATTERN_SLOTS && fmt_lock_store_used_count() < FMT_LOCK_POOL_MAX; pattern++) {
        for (track = 0u; track < FMT_TRACKS && fmt_lock_store_used_count() < FMT_LOCK_POOL_MAX; track++) {
            for (step = 0u; step < FMT_STEPS_PER_PATTERN && fmt_lock_store_used_count() < FMT_LOCK_POOL_MAX; step++) {
                if (pattern == 0u && track == 0u && step == 0u)
                    continue;
                for (p = 0u; p < 4u && fmt_lock_store_used_count() < FMT_LOCK_POOL_MAX; p++)
                    assert(fmt_lock_set(pattern, track, step, PARAMS[p], (uint16_t)p, &replaced) == FMT_LOCK_STATUS_OK);
            }
        }
    }
    assert(fmt_lock_store_used_count() == FMT_LOCK_POOL_MAX);
}

static void test_pool_full_and_corruption_propagation(void)
{
    FmtPatternInput input;
    FmtLockEditor editor;
    uint16_t used, value, idx;
    FmtLock corrupt_before;
    char text[32];
    uint8_t replaced;

    reset_all(&input, &editor);
    fill_pool_except_target_pan();
    used = fmt_lock_store_used_count();
    assert(fmt_lock_editor_begin(&editor, &input, WHITE_KEYS[0]));
    assert(fmt_lock_editor_apply_delta(&editor, 3u, 1));
    assert(editor.last_status == FMT_LOCK_EDITOR_STATUS_POOL_FULL);
    assert(fmt_lock_store_used_count() == used);
    assert(fmt_lock_get(0u, 0u, 0u, (FmtParamId)FMT_PARAM_TRACK_PAN, &value) == FMT_LOCK_STATUS_NOT_FOUND);
    fmt_lock_editor_footer(&editor, text);
    assert(strcmp(text, "PLOCK POOL FULL") == 0);

    reset_all(&input, &editor);
    assert(fmt_lock_set(0u, 0u, 0u, (FmtParamId)FMT_PARAM_VOICE_LEVEL, 100u, &replaced) == FMT_LOCK_STATUS_OK);
    idx = fmt_store_step(0u, 0u, 0u)->lock_head;
    assert(idx < FMT_LOCK_POOL_MAX);
    fmt_lock_store_storage[idx].owner = 1u;
    corrupt_before = fmt_lock_store_storage[idx];
    used = fmt_lock_store_used_count();
    assert(fmt_lock_editor_begin(&editor, &input, WHITE_KEYS[0]));
    assert(editor.last_status == FMT_LOCK_EDITOR_STATUS_CORRUPT);
    assert(fmt_lock_editor_apply_delta(&editor, 0u, 1));
    assert(editor.last_status == FMT_LOCK_EDITOR_STATUS_CORRUPT);
    assert(fmt_lock_store_used_count() == used);
    assert(memcmp(&fmt_lock_store_storage[idx], &corrupt_before, sizeof corrupt_before) == 0);
    fmt_lock_editor_footer(&editor, text);
    assert(strcmp(text, "PLOCK CORRUPT") == 0);
}

static void test_reference_boundary_still_creates_lock(void)
{
    FmtPatternInput input;
    FmtLockEditor editor;
    uint16_t value;

    reset_all(&input, &editor);
    assert(fmt_lock_editor_begin(&editor, &input, WHITE_KEYS[0]));
    assert(fmt_lock_editor_apply_delta(&editor, 0u, 1));
    assert(fmt_lock_get(0u, 0u, 0u, (FmtParamId)FMT_PARAM_VOICE_LEVEL, &value) == FMT_LOCK_STATUS_OK);
    assert(value == 255u);
    assert(fmt_lock_store_used_count() == 1u);
}

static void test_browse_sorted_clamped_and_zero_lock(void)
{
    FmtPatternInput input;
    FmtLockEditor editor;
    char text[32];

    reset_all(&input, &editor);
    add_four_unsorted(0u, 0u);
    assert(fmt_lock_editor_begin(&editor, &input, WHITE_KEYS[0]));
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_VOICE_LEVEL);
    assert(editor.last_status == FMT_LOCK_EDITOR_STATUS_BROWSE);
    fmt_lock_editor_footer(&editor, text);
    assert(strcmp(text, "P1/4 K1 VOICE 10") == 0);

    assert(fmt_lock_editor_browse(&editor, 1));
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_PITCH_FINE);
    fmt_lock_editor_footer(&editor, text);
    assert(strcmp(text, "P2/4 K2 FINE 20") == 0);

    assert(fmt_lock_editor_browse(&editor, INT32_MAX));
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_TRACK_PAN);
    fmt_lock_editor_footer(&editor, text);
    assert(strcmp(text, "P4/4 K4 PAN 40") == 0);

    assert(fmt_lock_editor_browse(&editor, 1));
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_TRACK_PAN);
    assert(fmt_lock_editor_browse(&editor, INT32_MIN));
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_VOICE_LEVEL);
    assert(fmt_lock_editor_browse(&editor, -1));
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_VOICE_LEVEL);

    reset_all(&input, &editor);
    assert(fmt_lock_editor_begin(&editor, &input, WHITE_KEYS[0]));
    assert(fmt_lock_editor_browse(&editor, 1));
    assert(editor.last_status == FMT_LOCK_EDITOR_STATUS_NOT_FOUND);
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_NONE);
    fmt_lock_editor_footer(&editor, text);
    assert(strcmp(text, "PLOCK NO LOCKS") == 0);
}

static void test_knob_write_selects_and_arm_cancel_is_non_destructive(void)
{
    FmtPatternInput input;
    FmtLockEditor editor;
    uint16_t used, value;
    char text[32];

    reset_all(&input, &editor);
    assert(fmt_lock_editor_begin(&editor, &input, WHITE_KEYS[0]));
    assert(fmt_lock_editor_apply_delta(&editor, 2u, -1));
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_TRACK_LEVEL);
    used = fmt_lock_store_used_count();

    assert(fmt_lock_editor_arm_delete(&editor));
    assert(editor.delete_armed);
    assert(editor.last_status == FMT_LOCK_EDITOR_STATUS_DELETE_ARMED);
    fmt_lock_editor_footer(&editor, text);
    assert(strcmp(text, "DEL K3 TRKLEV? SEL YES") == 0);
    assert(fmt_lock_store_used_count() == used);
    assert(!fmt_lock_editor_apply_delta(&editor, 2u, -20));
    assert(!fmt_lock_editor_browse(&editor, 1));
    assert(fmt_lock_get(0u, 0u, 0u, (FmtParamId)FMT_PARAM_TRACK_LEVEL, &value) == FMT_LOCK_STATUS_OK);
    assert(value == 254u);

    assert(fmt_lock_editor_cancel_delete(&editor));
    assert(!editor.delete_armed);
    assert(editor.last_status == FMT_LOCK_EDITOR_STATUS_BROWSE);
    assert(fmt_lock_store_used_count() == used);
    fmt_lock_editor_footer(&editor, text);
    assert(strcmp(text, "P1/1 K3 TRKLEV 254") == 0);
}

static void test_delete_first_middle_last_only_and_recovery(void)
{
    FmtPatternInput input;
    FmtLockEditor editor;
    FmtStep neighbor_before;
    const FmtStep *neighbor;
    uint16_t value;
    uint16_t free_before;
    char text[32];

    reset_all(&input, &editor);
    add_four_unsorted(0u, 0u);
    add_lock(0u, 1u, (FmtParamId)FMT_PARAM_VOICE_LEVEL, 99u);
    neighbor = fmt_store_step_const(0u, 0u, 1u);
    assert(neighbor);
    neighbor_before = *neighbor;
    assert(fmt_lock_editor_begin(&editor, &input, WHITE_KEYS[0]));

    free_before = fmt_lock_store_free_count();
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_VOICE_LEVEL);
    assert(fmt_lock_editor_arm_delete(&editor));
    assert(fmt_lock_editor_confirm_delete(&editor));
    assert(editor.last_status == FMT_LOCK_EDITOR_STATUS_DELETED);
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_PITCH_FINE);
    assert(fmt_lock_store_free_count() == (uint16_t)(free_before + 1u));
    assert(fmt_lock_get(0u, 0u, 0u, (FmtParamId)FMT_PARAM_VOICE_LEVEL, &value) == FMT_LOCK_STATUS_NOT_FOUND);
    assert(memcmp(neighbor, &neighbor_before, sizeof neighbor_before) == 0);
    fmt_lock_editor_footer(&editor, text);
    assert(strcmp(text, "DELETED K1 VOICE") == 0);

    assert(fmt_lock_editor_browse(&editor, 1));
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_TRACK_LEVEL);
    free_before = fmt_lock_store_free_count();
    assert(fmt_lock_editor_arm_delete(&editor));
    assert(fmt_lock_editor_confirm_delete(&editor));
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_TRACK_PAN);
    assert(fmt_lock_store_free_count() == (uint16_t)(free_before + 1u));
    assert(fmt_lock_get(0u, 0u, 0u, (FmtParamId)FMT_PARAM_TRACK_LEVEL, &value) == FMT_LOCK_STATUS_NOT_FOUND);

    free_before = fmt_lock_store_free_count();
    assert(fmt_lock_editor_arm_delete(&editor));
    assert(fmt_lock_editor_confirm_delete(&editor));
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_PITCH_FINE);
    assert(fmt_lock_store_free_count() == (uint16_t)(free_before + 1u));
    assert(fmt_lock_get(0u, 0u, 0u, (FmtParamId)FMT_PARAM_TRACK_PAN, &value) == FMT_LOCK_STATUS_NOT_FOUND);

    free_before = fmt_lock_store_free_count();
    assert(fmt_lock_editor_arm_delete(&editor));
    assert(fmt_lock_editor_confirm_delete(&editor));
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_NONE);
    assert(fmt_lock_store_free_count() == (uint16_t)(free_before + 1u));
    assert(fmt_lock_count(0u, 0u, 0u, (uint8_t *)&value) == FMT_LOCK_STATUS_OK);
    assert((uint8_t)value == 0u);
    assert(memcmp(neighbor, &neighbor_before, sizeof neighbor_before) == 0);
}

static void test_range_lock_can_be_browsed_and_deleted(void)
{
    FmtPatternInput input;
    FmtLockEditor editor;
    uint16_t value;
    char text[32];

    reset_all(&input, &editor);
    add_lock(0u, 0u, (FmtParamId)FMT_PARAM_PITCH_FINE, 500u);
    assert(fmt_lock_editor_begin(&editor, &input, WHITE_KEYS[0]));
    assert(editor.selected_param == (FmtParamId)FMT_PARAM_PITCH_FINE);
    fmt_lock_editor_footer(&editor, text);
    assert(strcmp(text, "P1/1 K2 FINE RANGE") == 0);
    assert(fmt_lock_editor_arm_delete(&editor));
    assert(fmt_lock_editor_confirm_delete(&editor));
    assert(fmt_lock_get(0u, 0u, 0u, (FmtParamId)FMT_PARAM_PITCH_FINE, &value) == FMT_LOCK_STATUS_NOT_FOUND);
}

static void test_delete_not_found_bad_address_and_corrupt_are_safe(void)
{
    FmtPatternInput input;
    FmtLockEditor editor;
    uint16_t idx, used, value;
    FmtLock before;

    reset_all(&input, &editor);
    add_lock(0u, 0u, (FmtParamId)FMT_PARAM_VOICE_LEVEL, 100u);
    assert(fmt_lock_editor_begin(&editor, &input, WHITE_KEYS[0]));
    assert(fmt_lock_editor_arm_delete(&editor));
    assert(fmt_lock_remove(0u, 0u, 0u, (FmtParamId)FMT_PARAM_VOICE_LEVEL) == FMT_LOCK_STATUS_OK);
    used = fmt_lock_store_used_count();
    assert(fmt_lock_editor_confirm_delete(&editor));
    assert(editor.last_status == FMT_LOCK_EDITOR_STATUS_NOT_FOUND);
    assert(!editor.delete_armed);
    assert(fmt_lock_store_used_count() == used);

    reset_all(&input, &editor);
    add_lock(0u, 0u, (FmtParamId)FMT_PARAM_VOICE_LEVEL, 100u);
    assert(fmt_lock_editor_begin(&editor, &input, WHITE_KEYS[0]));
    editor.track = FMT_TRACKS;
    assert(fmt_lock_editor_arm_delete(&editor));
    assert(editor.last_status == FMT_LOCK_EDITOR_STATUS_BAD_ADDRESS);
    assert(!editor.delete_armed);
    assert(fmt_lock_get(0u, 0u, 0u, (FmtParamId)FMT_PARAM_VOICE_LEVEL, &value) == FMT_LOCK_STATUS_OK);

    reset_all(&input, &editor);
    add_lock(0u, 0u, (FmtParamId)FMT_PARAM_VOICE_LEVEL, 100u);
    idx = fmt_store_step(0u, 0u, 0u)->lock_head;
    assert(idx < FMT_LOCK_POOL_MAX);
    fmt_lock_store_storage[idx].owner = 1u;
    before = fmt_lock_store_storage[idx];
    used = fmt_lock_store_used_count();
    assert(fmt_lock_editor_begin(&editor, &input, WHITE_KEYS[0]));
    assert(editor.last_status == FMT_LOCK_EDITOR_STATUS_CORRUPT);
    assert(fmt_lock_store_used_count() == used);
    assert(memcmp(&fmt_lock_store_storage[idx], &before, sizeof before) == 0);
}

int main(void)
{
    test_layout_mapping_and_footer();
    test_all_white_targets_and_tracks();
    test_first_write_replace_clamp_and_isolation();
    test_snapshot_survives_cursor_track_changes();
    test_range_rejection();
    test_pool_full_and_corruption_propagation();
    test_reference_boundary_still_creates_lock();
    test_browse_sorted_clamped_and_zero_lock();
    test_knob_write_selects_and_arm_cancel_is_non_destructive();
    test_delete_first_middle_last_only_and_recovery();
    test_range_lock_can_be_browsed_and_deleted();
    test_delete_not_found_bad_address_and_corrupt_are_safe();

    printf("FMT_LOCK_EDITOR_TESTS_OK=1\n");
    printf("state_bytes=%zu pattern=0 knobs=4 browse=select_sorted_clamped delete=edit_arm_then_sel_confirm clear_all=0\n",
           sizeof(FmtLockEditor));
    printf("targets=64x8 snapshot=1 replace_in_place=1 delete_first_middle_last_only=1 free_recovery=1 neighbor_isolation=1 range_delete=1\n");
    return 0;
}
