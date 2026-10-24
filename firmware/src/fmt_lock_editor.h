/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_LOCK_EDITOR_H
#define FM1TRACKER_FMT_LOCK_EDITOR_H

#include <stdint.h>

#include "fmt_lock_store.h"
#include "fmt_pattern_input.h"

#define FMT_LOCK_EDITOR_PATTERN 0u
#define FMT_LOCK_EDITOR_KNOBS 4u
#define FMT_LOCK_EDITOR_VALUE_MAX 255u
#define FMT_LOCK_EDITOR_NO_KEY 0xFFu

typedef enum {
    FMT_LOCK_EDITOR_STATUS_NONE = 0,
    FMT_LOCK_EDITOR_STATUS_OK = 1,
    FMT_LOCK_EDITOR_STATUS_RANGE = 2,
    FMT_LOCK_EDITOR_STATUS_STEP_FULL = 3,
    FMT_LOCK_EDITOR_STATUS_POOL_FULL = 4,
    FMT_LOCK_EDITOR_STATUS_BAD_ADDRESS = 5,
    FMT_LOCK_EDITOR_STATUS_CORRUPT = 6,
    FMT_LOCK_EDITOR_STATUS_INVALID_PARAM = 7,
    FMT_LOCK_EDITOR_STATUS_INTERNAL = 8
} FmtLockEditorStatus;

/* D029: one held-step target plus the last bounded feedback state. */
typedef struct {
    uint16_t last_value;
    uint8_t active;
    uint8_t held_key;
    uint8_t track;
    uint8_t step;
    FmtParamId last_param;
    uint8_t last_status;
} FmtLockEditor;

FMT_STATIC_ASSERT(lock_editor_state_is_8, sizeof(FmtLockEditor) == 8u);

void fmt_lock_editor_reset(FmtLockEditor *editor);
void fmt_lock_editor_cancel(FmtLockEditor *editor);
int fmt_lock_editor_is_active(const FmtLockEditor *editor);

/* Begin from an ordinary D022 white step key. The existing selected track and
 * the resulting global step are snapshotted and remain the D029 storage target
 * until cancel/release. The key is selected through D022 as part of begin. */
int fmt_lock_editor_begin(FmtLockEditor *editor,
                          FmtPatternInput *input,
                          uint32_t key_index);

/* True only while the gesture-owning physical white key remains down. */
int fmt_lock_editor_key_held(const FmtLockEditor *editor, uint32_t notes_mask);

/* D029 KNOB1..4 semantic mapping. knob is 0..3, not the HAL encoder id. */
int fmt_lock_editor_knob_param(uint32_t knob,
                               FmtParamId *param_id,
                               uint16_t *reference_value);

/* Apply one already-intent-qualified relative encoder delta. Returns 1 when a
 * nonzero delta was handled/attempted, 0 for inactive/zero/invalid knob input.
 * The editor itself never deletes a lock and never touches playback/base params. */
int fmt_lock_editor_apply_delta(FmtLockEditor *editor,
                                uint32_t knob,
                                int32_t delta);

/* Minimal D029 footer feedback. Inactive editors return an empty string. */
void fmt_lock_editor_footer(const FmtLockEditor *editor, char out[32]);

#endif /* FM1TRACKER_FMT_LOCK_EDITOR_H */
