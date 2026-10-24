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
    FMT_LOCK_EDITOR_STATUS_INTERNAL = 8,
    FMT_LOCK_EDITOR_STATUS_BROWSE = 9,
    FMT_LOCK_EDITOR_STATUS_DELETE_ARMED = 10,
    FMT_LOCK_EDITOR_STATUS_DELETED = 11,
    FMT_LOCK_EDITOR_STATUS_NOT_FOUND = 12
} FmtLockEditorStatus;

/* D029 target/feedback plus D030 semantic selection and destructive arm. */
typedef struct {
    uint16_t last_value;
    uint8_t active;
    uint8_t held_key;
    uint8_t track;
    uint8_t step;
    FmtParamId last_param;
    uint8_t last_status;
    FmtParamId selected_param;
    uint8_t delete_armed;
} FmtLockEditor;

FMT_STATIC_ASSERT(lock_editor_state_is_10, sizeof(FmtLockEditor) == 10u);

void fmt_lock_editor_reset(FmtLockEditor *editor);
void fmt_lock_editor_cancel(FmtLockEditor *editor);
int fmt_lock_editor_is_active(const FmtLockEditor *editor);

/* Begin from an ordinary D022 white step key. The existing selected track and
 * the resulting global step are snapshotted and remain the D029 storage target
 * until cancel/release. D030 selects the first existing lock, if any. */
int fmt_lock_editor_begin(FmtLockEditor *editor,
                          FmtPatternInput *input,
                          uint32_t key_index);

/* True only while the gesture-owning physical white key remains down. */
int fmt_lock_editor_key_held(const FmtLockEditor *editor, uint32_t notes_mask);

/* D029 KNOB1..4 semantic mapping. knob is 0..3, not the HAL encoder id. */
int fmt_lock_editor_knob_param(uint32_t knob,
                               FmtParamId *param_id,
                               uint16_t *reference_value);

/* D045 semantic write path. Caller supplies the already-resolved stable ID and
 * normalized 0..255 reference. This owns the one D029 mutation algorithm so
 * engine-page adapters do not duplicate lock-store logic. Returns 1 for a
 * nonzero handled/attempted delta, 0 for inactive/armed/zero/invalid input. */
int fmt_lock_editor_apply_param_delta(FmtLockEditor *editor,
                                      FmtParamId param_id,
                                      uint16_t reference_value,
                                      int32_t delta);

/* Legacy D029 common-page helper retained for Phase 3 compatibility. It maps
 * K1..4 to the historical common IDs and delegates to the semantic helper. */
int fmt_lock_editor_apply_delta(FmtLockEditor *editor,
                                uint32_t knob,
                                int32_t delta);

/* D030: browse the existing <=4 locks in D028's sorted parameter order. Delta
 * is relative SELECT-encoder movement and clamps at first/last; it never wraps.
 * Returns 1 when nonzero browse intent was handled, including safe no-lock/error
 * feedback, and 0 when inactive/armed/zero. */
int fmt_lock_editor_browse(FmtLockEditor *editor, int32_t delta);

/* Arm deletion of the currently selected semantic parameter. No storage is
 * mutated here. Returns 1 when the press was handled/feedback changed. */
int fmt_lock_editor_arm_delete(FmtLockEditor *editor);

/* Cancel a pending destructive arm. Returns 1 only when an arm was cleared. */
int fmt_lock_editor_cancel_delete(FmtLockEditor *editor);

/* Confirm one already-armed deletion. The selected parameter is revalidated
 * before one fmt_lock_remove() call. Returns 1 when confirmation was handled;
 * success/failure is reported in last_status. */
int fmt_lock_editor_confirm_delete(FmtLockEditor *editor);

/* Minimal D029/D030 footer feedback. Inactive editors return an empty string. */
void fmt_lock_editor_footer(const FmtLockEditor *editor, char out[32]);

#endif /* FM1TRACKER_FMT_LOCK_EDITOR_H */