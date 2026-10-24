from pathlib import Path

# The semantic footer knob mapping is already present on current Gate 3 HEAD.
# Only the inherited D028 test still uses 0x20 as an unassigned sentinel; D045
# assigns 0x20..0x25, so move that sentinel to the first still-unassigned ID.
p = Path('tests/test_fmt_lock_store.c')
s = p.read_text()
old = 'FMT_PARAM_NONE, FMT_PARAM_INVALID, 0x03u, 0x20u, 0x60u, 0xC0u, 0xE0u, 0xF0u'
new = 'FMT_PARAM_NONE, FMT_PARAM_INVALID, 0x03u, 0x26u, 0x60u, 0xC0u, 0xE0u, 0xF0u'
if old in s:
    s = s.replace(old, new, 1)
else:
    assert new in s
p.write_text(s)
