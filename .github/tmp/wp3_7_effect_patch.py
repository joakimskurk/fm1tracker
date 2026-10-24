from pathlib import Path

p = Path('firmware/src/fmt_tracker_effect.c')
s = p.read_text()
old = 'uint8_t fmt_tracker_effect_command_valid(uint8_t command)\n{\n    return command <= FMT_TRACKER_EFFECT_VIBRATO;\n}\n'
new = 'uint8_t fmt_tracker_effect_command_valid(uint8_t command)\n{\n    return command <= FMT_TRACKER_EFFECT_NOTE_CUT;\n}\n'
assert s.count(old) == 1, s.count(old)
s = s.replace(old, new, 1)

anchor = '''void fmt_tracker_effect_state_reset(FmtTrackerEffectState *state)\n{\n    if (!state)\n        return;\n    state->pitch_q7 = 0u;\n    state->porta_target_q7 = 0u;\n    state->transient_q7 = 0;\n    state->command = FMT_TRACKER_EFFECT_NONE;\n    state->value = 0u;\n    state->tick = 0u;\n    state->vibrato_phase = 0u;\n    state->porta_valid = 0u;\n    state->pitch_valid = 0u;\n}\n'''
assert s.count(anchor) == 1, s.count(anchor)
helpers = anchor + '''\nvoid fmt_tracker_effect_apply_delayed_note(FmtTrackerEffectState *state,\n                                           uint8_t note)\n{\n    if (!state || note > FMT_NOTE_MAX)\n        return;\n\n    state->pitch_q7 = (uint16_t)((uint16_t)note << 7);\n    state->pitch_valid = 1u;\n    state->porta_valid = 0u;\n    state->transient_q7 = 0;\n    state->vibrato_phase = 0u;\n}\n\nvoid fmt_tracker_effect_apply_cut(FmtTrackerEffectState *state)\n{\n    if (!state)\n        return;\n\n    state->pitch_valid = 0u;\n    state->porta_valid = 0u;\n    state->transient_q7 = 0;\n    state->vibrato_phase = 0u;\n}\n'''
s = s.replace(anchor, helpers, 1)
p.write_text(s)

p = Path('tests/test_fmt_tracker_effect.c')
s = p.read_text()
old = 'uint8_t expected = step <= FMT_TRACKER_EFFECT_VIBRATO;'
new = 'uint8_t expected = step <= FMT_TRACKER_EFFECT_NOTE_CUT;'
assert s.count(old) == 1, s.count(old)
p.write_text(s.replace(old, new, 1))

print('WP3_7_EFFECT_PATCHED=1')
print('WP3_7_D032_EXPECTATION_UPDATED=1')
