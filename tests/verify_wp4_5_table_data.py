#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
import hashlib
import re
from pathlib import Path

EXPECTED = 'f95b092a5106a8b05acb57d2960b0162b48ea42e2f8217b8923b0b7346046aee'
text = Path('firmware/src/fmt_osc_wavetable8.c').read_text()
start_marker = '/* D041_TABLE_DATA_BEGIN */'
end_marker = '/* D041_TABLE_DATA_END */'
assert text.count(start_marker) == 1
assert text.count(end_marker) == 1
block = text.split(start_marker, 1)[1].split(end_marker, 1)[0]
initializer = block.split('= {', 1)[1].rsplit('};', 1)[0]
values = [int(x) for x in re.findall(r'(?<![A-Za-z0-9_])-?\d+', initializer)]
assert len(values) == 256, len(values)
assert min(values) >= -127
assert max(values) <= 127
assert -128 not in values
blob = bytes(v & 0xFF for v in values)
digest = hashlib.sha256(blob).hexdigest()
assert digest == EXPECTED, digest
print(f'WP4_5_TABLE_DATA_SHA_OK=1 bytes={len(blob)} sha256={digest}')
