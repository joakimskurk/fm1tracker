from pathlib import Path

p = Path('firmware/src/fmt_lock_editor.c')
s = p.read_text()
old = '''    case FMT_PARAM_TRACK_PAN:\n        return 4u;\n    default:\n        return 0u;\n'''
new = '''    case FMT_PARAM_TRACK_PAN:\n        return 4u;\n    case FMT_PARAM_BIT8_ATTACK:\n    case FMT_PARAM_BIT8_PULSE_WIDTH:\n    case FMT_PARAM_BIT8_WAVETABLE:\n        return 1u;\n    case FMT_PARAM_BIT8_DECAY:\n        return 2u;\n    case FMT_PARAM_BIT8_SUSTAIN:\n        return 3u;\n    case FMT_PARAM_BIT8_RELEASE:\n        return 4u;\n    default:\n        return 0u;\n'''
assert old in s
s = s.replace(old, new, 1)
p.write_text(s)

p = Path('tests/test_fmt_lock_store.c')
s = p.read_text()
old = 'FMT_PARAM_NONE, FMT_PARAM_INVALID, 0x03u, 0x20u, 0x60u, 0xC0u, 0xE0u, 0xF0u'
new = 'FMT_PARAM_NONE, FMT_PARAM_INVALID, 0x03u, 0x26u, 0x60u, 0xC0u, 0xE0u, 0xF0u'
assert old in s
s = s.replace(old, new, 1)
p.write_text(s)
