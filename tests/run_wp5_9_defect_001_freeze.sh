#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

BASE=849c8b27d8c1afa09f9c093b695985c64e0c6013
OUT=build/wp5_9_defect_001_freeze
mkdir -p "$OUT"

# The amendment must be frozen before the production fix. Target source is
# still exactly the accepted D056 checkpoint here.
git diff --exit-code "$BASE" -- firmware/src firmware/gen tools
printf '%s\n' 'WP5_9_DEFECT_001_SOURCE_PREFIX_OK=1 production_delta=0 base=d056'

python3 - <<'PY' > build/wp5_9_defect_001_freeze/static.txt
from pathlib import Path
s = Path('firmware/src/dinghy.c').read_text()
needle = '''    tracker_fm_row_patch_valid[track] =
        (uint8_t)tracker_fm_snapshot_row(track, event->step, instrument);
'''
assert s.count(needle) == 1
assert '''        if (!tracker_fm_row_patch_valid[track]) {
            tracker_hard_silence_route(track);
            return;
        }
''' in s
print('WP5_9_DEFECT_001_STATIC_OK=1 validity=unconditional current_fm_invalid=hard_silence')
PY
cat "$OUT/static.txt"
grep -F 'WP5_9_DEFECT_001_STATIC_OK=1 validity=unconditional current_fm_invalid=hard_silence' "$OUT/static.txt"

grep -F '140 ms  t0 = NONE' docs/WP5_9_DEFECT_001_DELAYED_CROSS_FAMILY.md >/dev/null
grep -F 'firmware/src/dinghy.c' docs/WP5_9_DEFECT_001_FIX_CONTRACT.md >/dev/null
grep -F '0 B target normal RAM' docs/WP5_9_DEFECT_001_FIX_CONTRACT.md >/dev/null
grep -F 'image       <= 80828 B' docs/WP5_9_DEFECT_001_FIX_CONTRACT.md >/dev/null
grep -F 'normal RAM   = 27792 B' docs/WP5_9_DEFECT_001_FIX_CONTRACT.md >/dev/null
grep -F '8-BIT -> delayed FM4' docs/WP5_9_DEFECT_001_FIX_CONTRACT.md >/dev/null
grep -F 'delayed malformed/unsupported replacement' docs/WP5_9_DEFECT_001_FIX_CONTRACT.md >/dev/null
printf '%s\n' 'WP5_9_DEFECT_001_DOCS_OK=1 source=dinghy.c bidirectional=required malformed_delay=required ram_growth=0'

printf '%s\n' 'WP5_9_DEFECT_001_FREEZE_OK=1 defect=delayed_fm_to_8bit_early_silence authorized_source=dinghy.c ram_growth=0 image_fix_ceiling=80828 hardware_margin_claim=none' > "$OUT/summary.txt"
cat "$OUT/summary.txt"
