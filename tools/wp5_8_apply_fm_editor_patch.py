#!/usr/bin/env python3
from pathlib import Path


def replace_once(text: str, old: str, new: str, label: str) -> str:
    count = text.count(old)
    if count != 1:
        raise SystemExit(f"{label}: expected exactly one anchor, found {count}")
    return text.replace(old, new, 1)


dinghy_path = Path("firmware/src/dinghy.c")
dinghy = dinghy_path.read_text()

dinghy = replace_once(
    dinghy,
    "static uint8_t tracker_fm_row_patch_valid[FMT_TRACKS];\n",
    "static uint8_t tracker_fm_row_patch_valid[FMT_TRACKS];\n"
    "/* D056 main/UI-only provisional page selection. It never enters audio. */\n"
    "static uint8_t tracker_fm_lock_page;\n",
    "FM page state",
)

old_delete = '''            if (delete_was_armed && fmt_lock_editor_runtime.delete_armed &&
                (b & (1u << BTN_SEL))) {
                if (fmt_8bit_linked_editor_confirm_delete(&fmt_lock_editor_runtime)) {
                    delete_activity = 1;
                    tracker_footer_dirty();
                }
            } else if (!delete_was_armed &&
'''
new_delete = '''            if (delete_was_armed && fmt_lock_editor_runtime.delete_armed &&
                (b & (1u << BTN_SEL))) {
                int handled = fmt_fm_linked_editor_is_fm(&fmt_lock_editor_runtime)
                            ? fmt_fm_linked_editor_confirm_delete(&fmt_lock_editor_runtime)
                            : fmt_8bit_linked_editor_confirm_delete(&fmt_lock_editor_runtime);
                if (handled) {
                    delete_activity = 1;
                    tracker_footer_dirty();
                }
            } else if (!delete_was_armed &&
'''
dinghy = replace_once(dinghy, old_delete, new_delete, "family delete dispatch")

anchor = '''        /* D025's pending FX state lives in FmtStepRecord so modifier ordering,
         * cancellation and writes are covered by the same editor unit tests. */
'''
insert = '''        /* D056 provisional FM page gesture. During a held-step edit OCT-/OCT+
         * were already suppressed from ordinary pattern-page navigation. Reuse
         * them only for valid FM rows; 8BIT held-step behavior is unchanged. */
        if (fmt_lock_editor_is_active(&fmt_lock_editor_runtime) &&
            !lock_began && !lock_ended && !delete_was_armed && !delete_activity &&
            fmt_fm_linked_editor_is_fm(&fmt_lock_editor_runtime)) {
            if ((b & (1u << BTN_OCT_DOWN)) && tracker_fm_lock_page > 0u) {
                tracker_fm_lock_page--;
                tracker_footer_dirty();
            } else if ((b & (1u << BTN_OCT_UP)) &&
                       tracker_fm_lock_page + 1u < FMT_FM_PAGE_COUNT) {
                tracker_fm_lock_page++;
                tracker_footer_dirty();
            }
        }

''' + anchor
dinghy = replace_once(dinghy, anchor, insert, "FM page gesture")

old_knobs = '''            for (k = 0u; k < FMT_LOCK_EDITOR_KNOBS; k++) {
                if (lock_delta[k] != 0 &&
                    fmt_8bit_linked_editor_apply_delta(&fmt_lock_editor_runtime,
                                                       k,
                                                       lock_delta[k])) {
                    tracker_footer_dirty();
                }
            }
'''
new_knobs = '''            {
                int fm_editor = fmt_fm_linked_editor_is_fm(&fmt_lock_editor_runtime);
                for (k = 0u; k < FMT_LOCK_EDITOR_KNOBS; k++) {
                    int handled = 0;
                    if (lock_delta[k] != 0) {
                        handled = fm_editor
                            ? fmt_fm_linked_editor_apply_delta(&fmt_lock_editor_runtime,
                                                              tracker_fm_lock_page,
                                                              k,
                                                              lock_delta[k])
                            : fmt_8bit_linked_editor_apply_delta(&fmt_lock_editor_runtime,
                                                                k,
                                                                lock_delta[k]);
                    }
                    if (handled)
                        tracker_footer_dirty();
                }
            }
'''
dinghy = replace_once(dinghy, old_knobs, new_knobs, "family knob dispatch")

dinghy = replace_once(
    dinghy,
    "    fmt_lock_editor_reset(&fmt_lock_editor_runtime);\n",
    "    fmt_lock_editor_reset(&fmt_lock_editor_runtime);\n"
    "    tracker_fm_lock_page = FMT_FM_PAGE_CORE;\n",
    "FM page reset",
)

old_leds = '''        fm1_led_key(BTN_EDIT, pattern_mode && !step_rec_mode && fmt_lock_editor_runtime.delete_armed);
        fm1_led_key(BTN_OCT_DOWN, pattern_mode && !step_rec_mode ? fmt_pattern_input_runtime.page > 0u : set.oct < 0);
        fm1_led_key(BTN_OCT_UP, pattern_mode && !step_rec_mode ? fmt_pattern_input_runtime.page + 1u < FMT_PATTERN_INPUT_PAGE_COUNT : set.oct > 0);
'''
new_leds = '''        fm1_led_key(BTN_EDIT, pattern_mode && !step_rec_mode && fmt_lock_editor_runtime.delete_armed);
        if (pattern_mode && !step_rec_mode &&
            fmt_lock_editor_is_active(&fmt_lock_editor_runtime) &&
            fmt_fm_linked_editor_is_fm(&fmt_lock_editor_runtime)) {
            fm1_led_key(BTN_OCT_DOWN, tracker_fm_lock_page > 0u);
            fm1_led_key(BTN_OCT_UP, tracker_fm_lock_page + 1u < FMT_FM_PAGE_COUNT);
        } else {
            fm1_led_key(BTN_OCT_DOWN, pattern_mode && !step_rec_mode ? fmt_pattern_input_runtime.page > 0u : set.oct < 0);
            fm1_led_key(BTN_OCT_UP, pattern_mode && !step_rec_mode ? fmt_pattern_input_runtime.page + 1u < FMT_PATTERN_INPUT_PAGE_COUNT : set.oct > 0);
        }
'''
dinghy = replace_once(dinghy, old_leds, new_leds, "FM page LEDs")

dinghy_path.write_text(dinghy)

editor_path = Path("firmware/src/fmt_lock_editor.c")
editor = editor_path.read_text()

old_short = '''    case FMT_PARAM_BIT8_WAVETABLE:
        return "TABLE";
    default:
        return "?";
    }
}
'''
new_short = '''    case FMT_PARAM_BIT8_WAVETABLE:
        return "TABLE";
    case FMT_PARAM_FM_ALGORITHM:
        return "ALG";
    case FMT_PARAM_FM_INDEX:
        return "INDEX";
    case FMT_PARAM_FM_FEEDBACK:
        return "FDBK";
    default:
        if (param_id >= FMT_PARAM_FM_OP1_RATIO &&
            param_id <= FMT_PARAM_FM_OP4_RATIO)
            return "RATIO";
        if (param_id >= FMT_PARAM_FM_OP1_LEVEL &&
            param_id <= FMT_PARAM_FM_OP4_LEVEL)
            return "LEVEL";
        if (param_id >= FMT_PARAM_FM_OP1_ATTACK &&
            param_id <= FMT_PARAM_FM_OP4_ATTACK)
            return "ATTACK";
        if (param_id >= FMT_PARAM_FM_OP1_DECAY &&
            param_id <= FMT_PARAM_FM_OP4_DECAY)
            return "DECAY";
        if (param_id >= FMT_PARAM_FM_OP1_SUSTAIN &&
            param_id <= FMT_PARAM_FM_OP4_SUSTAIN)
            return "SUSTAIN";
        if (param_id >= FMT_PARAM_FM_OP1_RELEASE &&
            param_id <= FMT_PARAM_FM_OP4_RELEASE)
            return "RELEASE";
        return "?";
    }
}
'''
editor = replace_once(editor, old_short, new_short, "FM footer names")

old_knob_map = '''    case FMT_PARAM_BIT8_RELEASE:
        return 4u;
    default:
        return 0u;
    }
}
'''
new_knob_map = '''    case FMT_PARAM_BIT8_RELEASE:
        return 4u;
    case FMT_PARAM_FM_ALGORITHM:
        return 1u;
    case FMT_PARAM_FM_INDEX:
        return 2u;
    case FMT_PARAM_FM_FEEDBACK:
        return 3u;
    default:
        if (param_id >= FMT_PARAM_FM_OP1_RATIO &&
            param_id <= FMT_PARAM_FM_OP4_RATIO)
            return (uint8_t)(param_id - FMT_PARAM_FM_OP1_RATIO + 1u);
        if (param_id >= FMT_PARAM_FM_OP1_LEVEL &&
            param_id <= FMT_PARAM_FM_OP4_LEVEL)
            return (uint8_t)(param_id - FMT_PARAM_FM_OP1_LEVEL + 1u);
        if (param_id >= FMT_PARAM_FM_OP1_ATTACK &&
            param_id <= FMT_PARAM_FM_OP4_ATTACK)
            return (uint8_t)(param_id - FMT_PARAM_FM_OP1_ATTACK + 1u);
        if (param_id >= FMT_PARAM_FM_OP1_DECAY &&
            param_id <= FMT_PARAM_FM_OP4_DECAY)
            return (uint8_t)(param_id - FMT_PARAM_FM_OP1_DECAY + 1u);
        if (param_id >= FMT_PARAM_FM_OP1_SUSTAIN &&
            param_id <= FMT_PARAM_FM_OP4_SUSTAIN)
            return (uint8_t)(param_id - FMT_PARAM_FM_OP1_SUSTAIN + 1u);
        if (param_id >= FMT_PARAM_FM_OP1_RELEASE &&
            param_id <= FMT_PARAM_FM_OP4_RELEASE)
            return (uint8_t)(param_id - FMT_PARAM_FM_OP1_RELEASE + 1u);
        return 0u;
    }
}
'''
editor = replace_once(editor, old_knob_map, new_knob_map, "FM footer knob map")
editor_path.write_text(editor)

print("WP5_8_FM_EDITOR_PATCH_OK=1 files=2 page_state=1 oct_gesture=fm_only family_dispatch=1 footer_semantics=27")
