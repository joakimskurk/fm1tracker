/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_lock_editor.h"

static FmtLockEditorStatus fmt_lock_editor_storage_status(FmtLockStatus status)
{
    switch (status) {
    case FMT_LOCK_STATUS_OK:
        return FMT_LOCK_EDITOR_STATUS_OK;
    case FMT_LOCK_STATUS_INVALID_PARAM:
        return FMT_LOCK_EDITOR_STATUS_INVALID_PARAM;
    case FMT_LOCK_STATUS_STEP_FULL:
        return FMT_LOCK_EDITOR_STATUS_STEP_FULL;
    case FMT_LOCK_STATUS_POOL_FULL:
        return FMT_LOCK_EDITOR_STATUS_POOL_FULL;
    case FMT_LOCK_STATUS_BAD_ADDRESS:
        return FMT_LOCK_EDITOR_STATUS_BAD_ADDRESS;
    case FMT_LOCK_STATUS_CORRUPT:
        return FMT_LOCK_EDITOR_STATUS_CORRUPT;
    default:
        return FMT_LOCK_EDITOR_STATUS_INTERNAL;
    }
}

static void fmt_lock_editor_copy(char **dst, const char *src)
{
    while (*src)
        *(*dst)++ = *src++;
}

static void fmt_lock_editor_two_digits(char **dst, uint32_t value)
{
    uint32_t tens = 0u;

    while (value >= 10u) {
        value -= 10u;
        tens++;
    }
    *(*dst)++ = (char)('0' + tens);
    *(*dst)++ = (char)('0' + value);
}

static void fmt_lock_editor_u8_dec(char **dst, uint16_t value)
{
    uint16_t hundreds = 0u, tens = 0u;

    while (value >= 100u) {
        value = (uint16_t)(value - 100u);
        hundreds++;
    }
    while (value >= 10u) {
        value = (uint16_t)(value - 10u);
        tens++;
    }
    if (hundreds)
        *(*dst)++ = (char)('0' + hundreds);
    if (hundreds || tens)
        *(*dst)++ = (char)('0' + tens);
    *(*dst)++ = (char)('0' + value);
}

static const char *fmt_lock_editor_param_short(FmtParamId param_id)
{
    switch (param_id) {
    case FMT_PARAM_VOICE_LEVEL:
        return "VOICE";
    case FMT_PARAM_PITCH_FINE:
        return "FINE";
    case FMT_PARAM_TRACK_LEVEL:
        return "TRKLEV";
    case FMT_PARAM_TRACK_PAN:
        return "PAN";
    default:
        return "?";
    }
}

static uint8_t fmt_lock_editor_param_knob(FmtParamId param_id)
{
    switch (param_id) {
    case FMT_PARAM_VOICE_LEVEL:
        return 1u;
    case FMT_PARAM_PITCH_FINE:
        return 2u;
    case FMT_PARAM_TRACK_LEVEL:
        return 3u;
    case FMT_PARAM_TRACK_PAN:
        return 4u;
    default:
        return 0u;
    }
}

void fmt_lock_editor_reset(FmtLockEditor *editor)
{
    if (!editor)
        return;
    editor->last_value = 0u;
    editor->active = 0u;
    editor->held_key = FMT_LOCK_EDITOR_NO_KEY;
    editor->track = 0u;
    editor->step = 0u;
    editor->last_param = (FmtParamId)FMT_PARAM_NONE;
    editor->last_status = (uint8_t)FMT_LOCK_EDITOR_STATUS_NONE;
}

void fmt_lock_editor_cancel(FmtLockEditor *editor)
{
    if (!editor)
        return;
    editor->active = 0u;
    editor->held_key = FMT_LOCK_EDITOR_NO_KEY;
    editor->last_param = (FmtParamId)FMT_PARAM_NONE;
    editor->last_status = (uint8_t)FMT_LOCK_EDITOR_STATUS_NONE;
    editor->last_value = 0u;
}

int fmt_lock_editor_is_active(const FmtLockEditor *editor)
{
    return editor && editor->active != 0u;
}

int fmt_lock_editor_begin(FmtLockEditor *editor,
                          FmtPatternInput *input,
                          uint32_t key_index)
{
    uint8_t local;

    if (!editor || !input || editor->active ||
        !fmt_pattern_input_is_pattern(input) ||
        input->selected_track >= FMT_TRACKS ||
        !fmt_pattern_input_key_to_local(key_index, &local))
        return 0;

    (void)local;
    if (!fmt_pattern_input_select_key(input, key_index))
        return 0;

    editor->last_value = 0u;
    editor->active = 1u;
    editor->held_key = (uint8_t)key_index;
    editor->track = input->selected_track;
    editor->step = fmt_pattern_input_global_step(input);
    editor->last_param = (FmtParamId)FMT_PARAM_NONE;
    editor->last_status = (uint8_t)FMT_LOCK_EDITOR_STATUS_NONE;
    return 1;
}

int fmt_lock_editor_key_held(const FmtLockEditor *editor, uint32_t notes_mask)
{
    if (!editor || !editor->active || editor->held_key >= FMT_PATTERN_INPUT_KEY_COUNT)
        return 0;
    return (notes_mask & (1u << editor->held_key)) != 0u;
}

int fmt_lock_editor_knob_param(uint32_t knob,
                               FmtParamId *param_id,
                               uint16_t *reference_value)
{
    FmtParamId id;
    uint16_t reference;

    switch (knob) {
    case 0u:
        id = (FmtParamId)FMT_PARAM_VOICE_LEVEL;
        reference = 255u;
        break;
    case 1u:
        id = (FmtParamId)FMT_PARAM_PITCH_FINE;
        reference = 128u;
        break;
    case 2u:
        id = (FmtParamId)FMT_PARAM_TRACK_LEVEL;
        reference = 255u;
        break;
    case 3u:
        id = (FmtParamId)FMT_PARAM_TRACK_PAN;
        reference = 128u;
        break;
    default:
        return 0;
    }

    if (param_id)
        *param_id = id;
    if (reference_value)
        *reference_value = reference;
    return 1;
}

int fmt_lock_editor_apply_delta(FmtLockEditor *editor,
                                uint32_t knob,
                                int32_t delta)
{
    FmtParamId param_id;
    uint16_t reference, current, next;
    uint8_t replaced;
    FmtLockStatus status;

    if (!editor || !editor->active || delta == 0)
        return 0;
    if (!fmt_lock_editor_knob_param(knob, &param_id, &reference))
        return 0;

    editor->last_param = param_id;
    status = fmt_lock_get(FMT_LOCK_EDITOR_PATTERN,
                          editor->track,
                          editor->step,
                          param_id,
                          &current);

    if (status == FMT_LOCK_STATUS_NOT_FOUND) {
        current = reference;
    } else if (status != FMT_LOCK_STATUS_OK) {
        editor->last_status = (uint8_t)fmt_lock_editor_storage_status(status);
        editor->last_value = 0u;
        return 1;
    } else if (current > FMT_LOCK_EDITOR_VALUE_MAX) {
        editor->last_status = (uint8_t)FMT_LOCK_EDITOR_STATUS_RANGE;
        editor->last_value = current;
        return 1;
    }

    /* Saturate without ever forming an overflowing signed sum. This is wider
     * than the current int16 HAL detent source so the public editor API remains
     * deterministic for any int32 caller/test value. */
    if (delta > 0) {
        uint32_t room = FMT_LOCK_EDITOR_VALUE_MAX - current;
        next = (uint32_t)delta >= room ? FMT_LOCK_EDITOR_VALUE_MAX
                                      : (uint16_t)(current + (uint16_t)delta);
    } else {
        int32_t floor_delta = -(int32_t)current;
        next = delta <= floor_delta ? 0u : (uint16_t)((int32_t)current + delta);
    }

    status = fmt_lock_set(FMT_LOCK_EDITOR_PATTERN,
                          editor->track,
                          editor->step,
                          param_id,
                          next,
                          &replaced);
    (void)replaced;
    editor->last_value = next;
    editor->last_status = (uint8_t)fmt_lock_editor_storage_status(status);
    return 1;
}

void fmt_lock_editor_footer(const FmtLockEditor *editor, char out[32])
{
    char *p;
    uint8_t knob;

    if (!out)
        return;
    out[0] = 0;
    if (!editor || !editor->active)
        return;

    p = out;
    if (editor->last_param == (FmtParamId)FMT_PARAM_NONE ||
        editor->last_status == (uint8_t)FMT_LOCK_EDITOR_STATUS_NONE) {
        fmt_lock_editor_copy(&p, "PLOCK S");
        fmt_lock_editor_two_digits(&p, (uint32_t)editor->step + 1u);
        fmt_lock_editor_copy(&p, " T");
        *p++ = (char)('1' + editor->track);
        fmt_lock_editor_copy(&p, " K1-4");
        *p = 0;
        return;
    }

    if (editor->last_status == (uint8_t)FMT_LOCK_EDITOR_STATUS_OK) {
        knob = fmt_lock_editor_param_knob(editor->last_param);
        *p++ = 'K';
        *p++ = (char)('0' + knob);
        *p++ = ' ';
        fmt_lock_editor_copy(&p, fmt_lock_editor_param_short(editor->last_param));
        *p++ = ' ';
        fmt_lock_editor_u8_dec(&p, editor->last_value);
        fmt_lock_editor_copy(&p, " LOCK");
        *p = 0;
        return;
    }

    fmt_lock_editor_copy(&p, "PLOCK ");
    switch ((FmtLockEditorStatus)editor->last_status) {
    case FMT_LOCK_EDITOR_STATUS_RANGE:
        fmt_lock_editor_copy(&p, "RANGE");
        break;
    case FMT_LOCK_EDITOR_STATUS_STEP_FULL:
        fmt_lock_editor_copy(&p, "STEP FULL");
        break;
    case FMT_LOCK_EDITOR_STATUS_POOL_FULL:
        fmt_lock_editor_copy(&p, "POOL FULL");
        break;
    case FMT_LOCK_EDITOR_STATUS_BAD_ADDRESS:
        fmt_lock_editor_copy(&p, "BAD ADDR");
        break;
    case FMT_LOCK_EDITOR_STATUS_CORRUPT:
        fmt_lock_editor_copy(&p, "CORRUPT");
        break;
    case FMT_LOCK_EDITOR_STATUS_INVALID_PARAM:
        fmt_lock_editor_copy(&p, "INVALID");
        break;
    default:
        fmt_lock_editor_copy(&p, "ERROR");
        break;
    }
    *p = 0;
}
