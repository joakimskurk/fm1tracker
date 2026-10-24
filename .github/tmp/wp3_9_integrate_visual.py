from pathlib import Path

# App: both tracker sync sites use the D035 feedback-aware sync.
p = Path('firmware/src/dinghy.c')
s = p.read_text()
old = '''fmt_tracker_screen_sync(&fmt_tracker_screen_runtime,
                                          &fmt_pattern_input_runtime,
                                          &fmt_step_record_runtime,
                                          &fmt_transport_runtime)'''
new = '''fmt_tracker_screen_sync_feedback(&fmt_tracker_screen_runtime,
                                                   &fmt_pattern_input_runtime,
                                                   &fmt_step_record_runtime,
                                                   &fmt_transport_runtime,
                                                   &fmt_lock_editor_runtime)'''
assert s.count(old) == 2, f'dinghy sync count={s.count(old)}'
s = s.replace(old, new)
p.write_text(s)

# Footer: mark the one D031 storage-only parameter whenever detailed feedback names it.
p = Path('firmware/src/fmt_lock_editor.c')
s = p.read_text()
anchor = '''static uint8_t fmt_lock_editor_param_knob(FmtParamId param_id)
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
'''
helper = anchor + '''
static void fmt_lock_editor_storage_marker(FmtParamId param_id, char **p)
{
    if (param_id == (FmtParamId)FMT_PARAM_TRACK_PAN)
        fmt_lock_editor_copy(p, " STORE");
}
'''
assert s.count(anchor) == 1
s = s.replace(anchor, helper)

pairs = [
    ('fmt_lock_editor_copy(&p, "? SEL YES");\n        *p = 0;',
     'fmt_lock_editor_copy(&p, "? SEL YES");\n        fmt_lock_editor_storage_marker(editor->selected_param, &p);\n        *p = 0;'),
    ('fmt_lock_editor_copy(&p, fmt_lock_editor_param_short(editor->last_param));\n        *p = 0;\n        return;\n    }\n\n    if (editor->last_status == (uint8_t)FMT_LOCK_EDITOR_STATUS_BROWSE)',
     'fmt_lock_editor_copy(&p, fmt_lock_editor_param_short(editor->last_param));\n        fmt_lock_editor_storage_marker(editor->last_param, &p);\n        *p = 0;\n        return;\n    }\n\n    if (editor->last_status == (uint8_t)FMT_LOCK_EDITOR_STATUS_BROWSE)'),
    ('else\n                fmt_lock_editor_copy(&p, "RANGE");\n            *p = 0;',
     'else\n                fmt_lock_editor_copy(&p, "RANGE");\n            fmt_lock_editor_storage_marker(editor->selected_param, &p);\n            *p = 0;'),
    ('fmt_lock_editor_copy(&p, " LOCK");\n        *p = 0;',
     'fmt_lock_editor_copy(&p, " LOCK");\n        fmt_lock_editor_storage_marker(editor->last_param, &p);\n        *p = 0;'),
]
for old, new in pairs:
    assert s.count(old) == 1, old
    s = s.replace(old, new)
p.write_text(s)

# Existing editor regression documents the intentional storage-only label.
p = Path('tests/test_fmt_lock_editor.c')
s = p.read_text()
old = 'assert(strcmp(text, "P4/4 K4 PAN 40") == 0);'
new = 'assert(strcmp(text, "P4/4 K4 PAN 40 STORE") == 0);'
assert s.count(old) == 1
s = s.replace(old, new)
p.write_text(s)

print('WP3_9_INTEGRATION_PATCH_OK=1')
