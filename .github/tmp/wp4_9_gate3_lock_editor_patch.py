from pathlib import Path

p = Path('firmware/src/fmt_lock_editor.c')
s = p.read_text()

old = '''    case FMT_PARAM_TRACK_PAN:\n        return "PAN";\n    default:\n        return "?";\n'''
new = '''    case FMT_PARAM_TRACK_PAN:\n        return "PAN";\n    case FMT_PARAM_BIT8_ATTACK:\n        return "ATTACK";\n    case FMT_PARAM_BIT8_DECAY:\n        return "DECAY";\n    case FMT_PARAM_BIT8_SUSTAIN:\n        return "SUSTAIN";\n    case FMT_PARAM_BIT8_RELEASE:\n        return "RELEASE";\n    case FMT_PARAM_BIT8_PULSE_WIDTH:\n        return "WIDTH";\n    case FMT_PARAM_BIT8_WAVETABLE:\n        return "TABLE";\n    default:\n        return "?";\n'''
assert old in s
s = s.replace(old, new, 1)

old = '''    case FMT_PARAM_TRACK_PAN:\n        return 4u;\n    default:\n        return 0u;\n'''
new = '''    case FMT_PARAM_TRACK_PAN:\n        return 4u;\n    case FMT_PARAM_BIT8_ATTACK:\n    case FMT_PARAM_BIT8_PULSE_WIDTH:\n    case FMT_PARAM_BIT8_WAVETABLE:\n        return 1u;\n    case FMT_PARAM_BIT8_DECAY:\n        return 2u;\n    case FMT_PARAM_BIT8_SUSTAIN:\n        return 3u;\n    case FMT_PARAM_BIT8_RELEASE:\n        return 4u;\n    default:\n        return 0u;\n'''
assert old in s
s = s.replace(old, new, 1)

start = s.index('int fmt_lock_editor_apply_delta(')
end = s.index('int fmt_lock_editor_browse(', start)
replacement = '''int fmt_lock_editor_apply_param_delta(FmtLockEditor *editor,\n                                      FmtParamId param_id,\n                                      uint16_t reference_value,\n                                      int32_t delta)\n{\n    uint16_t current, next;\n    uint8_t replaced;\n    FmtLockStatus status;\n\n    if (!editor || !editor->active || editor->delete_armed || delta == 0)\n        return 0;\n    if (!fmt_param_is_lockable(param_id) ||\n        reference_value > FMT_LOCK_EDITOR_VALUE_MAX)\n        return 0;\n\n    editor->last_param = param_id;\n    status = fmt_lock_get(FMT_LOCK_EDITOR_PATTERN,\n                          editor->track,\n                          editor->step,\n                          param_id,\n                          &current);\n\n    if (status == FMT_LOCK_STATUS_NOT_FOUND) {\n        current = reference_value;\n    } else if (status != FMT_LOCK_STATUS_OK) {\n        fmt_lock_editor_set_storage_error(editor, status);\n        return 1;\n    } else if (current > FMT_LOCK_EDITOR_VALUE_MAX) {\n        editor->last_status = (uint8_t)FMT_LOCK_EDITOR_STATUS_RANGE;\n        editor->last_value = current;\n        return 1;\n    }\n\n    if (delta > 0) {\n        uint32_t room = FMT_LOCK_EDITOR_VALUE_MAX - current;\n        next = (uint32_t)delta >= room ? FMT_LOCK_EDITOR_VALUE_MAX\n                                      : (uint16_t)(current + (uint16_t)delta);\n    } else {\n        int32_t floor_delta = -(int32_t)current;\n        next = delta <= floor_delta ? 0u : (uint16_t)((int32_t)current + delta);\n    }\n\n    status = fmt_lock_set(FMT_LOCK_EDITOR_PATTERN,\n                          editor->track,\n                          editor->step,\n                          param_id,\n                          next,\n                          &replaced);\n    (void)replaced;\n    editor->last_value = next;\n    editor->last_status = (uint8_t)fmt_lock_editor_storage_status(status);\n    if (status == FMT_LOCK_STATUS_OK)\n        editor->selected_param = param_id;\n    else\n        editor->delete_armed = 0u;\n    return 1;\n}\n\nint fmt_lock_editor_apply_delta(FmtLockEditor *editor,\n                                uint32_t knob,\n                                int32_t delta)\n{\n    FmtParamId param_id;\n    uint16_t reference;\n\n    if (!fmt_lock_editor_knob_param(knob, &param_id, &reference))\n        return 0;\n    return fmt_lock_editor_apply_param_delta(editor,\n                                             param_id,\n                                             reference,\n                                             delta);\n}\n\n'''
s = s[:start] + replacement + s[end:]
p.write_text(s)
print('WP4_9_GATE3_LOCK_EDITOR_PATCH_OK=1 semantic_helper=1 d044_labels=6 legacy_delegate=1')
