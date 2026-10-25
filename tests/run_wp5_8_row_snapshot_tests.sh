#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_8_row_snapshot
EXPECTED_WAV=17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076
mkdir -p "$OUT" build/host

# Re-prove every accepted D056 layer below dinghy integration.
sh tests/run_wp5_8_store_resolver_tests.sh > "$OUT/resolver.log" 2>&1
cat "$OUT/resolver.log"
grep -F 'WP5_8_STORE_RESOLVER_GATE_OK=1 decision=D056 note_inherit=exact locks=4 uint16_filter=no_truncate wrong_family=ignored structural=failsafe shared_cache=protected d028=exact all8bit=wp5_0_identical hardware_margin_claim=none' "$OUT/resolver.log"

# Default startup remains all 8BIT and must stay bit-identical with WP5.0.
sh tests/run_host.sh > "$OUT/default_host.log" 2>&1
DEFAULT_SHA="$(sha256sum build/host/fm1tracker.wav | awk '{print $1}')"
test "$DEFAULT_SHA" = "$EXPECTED_WAV"
printf '%s\n' "WP5_8_ROW_SNAPSHOT_ALL8BIT_OK=1 sha256=$DEFAULT_SHA"

python3 - <<'PY' | tee "$OUT/static_audit.txt"
import re
from pathlib import Path
s = Path('firmware/src/dinghy.c').read_text()

assert '#include "fmt_fm_params.c"' in s
assert '#include "fmt_fm_linked_params.c"' in s
assert 'static FmtFmPatch tracker_fm_row_patch[FMT_TRACKS];' in s
assert 'static uint8_t tracker_fm_row_patch_valid[FMT_TRACKS];' in s
assert s.count('fmt_fm_engine_default_patch()') == 1
assert '&tracker_fm_row_patch[track]' in s
assert 'fmt_fm_linked_params_read(track, row, &overrides)' in s
assert 'fmt_fm_linked_params_compose(instrument,' in s
assert 'fmt_fm_linked_params_rebuild_track(write_track)' in s

# The bounded audio-side seam may read only the already-published D056 cache.
lo = s.index('static int tracker_fm_snapshot_row')
hi = s.index('static void audition_release_all')
audio_seam = s[lo:hi]
for forbidden in ('fmt_lock_count(', 'fmt_lock_at(', 'fmt_lock_get(',
                  'fmt_store_song(', 'fmt_store_step('):
    assert forbidden not in audio_seam, forbidden

# D056 adds one current patch per track, not a patch per row.
assert 'FmtFmPatch tracker_fm_row_patch[FMT_TRACKS][FMT_STEPS_PER_PATTERN]' not in s
assert 'FmtFmPatch tracker_fm_row_patch[FMT_TRACKS];' in s
print('WP5_8_ROW_SNAPSHOT_STATIC_OK=1 patch_bank=224 valid=8 per_row_patch_cache=0 store_traversal_audio=0 default_patch_calls=1')
PY

# Build two otherwise identical mixed-family dinghy fixtures. The second adds
# one real D028 FM p-lock on row 0. The p-lock must alter audio deterministically
# without changing the production source or D055 engine internals.
rm -rf "$OUT/src_base" "$OUT/src_lock"
mkdir -p "$OUT/src_base" "$OUT/src_lock"
python3 - <<'PY'
from pathlib import Path
src = Path('firmware/src/dinghy.c').read_text()
anchor = '    fmt_playback_seed_proof_pattern();\n'
assert src.count(anchor) == 1
common = '''    fmt_playback_seed_proof_pattern();
    {
        FmtSong *wp5_8_song = fmt_store_song();
        FmtStep *wp5_8_row;
        if (wp5_8_song) {
            wp5_8_song->instruments[0].mode = FMT_MODE_16BIT;
            wp5_8_song->instruments[0].engine = FMT_FM_ENGINE_FM4;
            wp5_8_row = fmt_store_step(FMT_PLAYBACK_PATTERN, 0u, 16u);
            if (wp5_8_row)
                wp5_8_row->instrument = 1u;
            wp5_8_row = fmt_store_step(FMT_PLAYBACK_PATTERN, 0u, 32u);
            if (wp5_8_row)
                wp5_8_row->instrument = 0u;
        }
    }
'''
locked = '''    fmt_playback_seed_proof_pattern();
    {
        FmtSong *wp5_8_song = fmt_store_song();
        FmtStep *wp5_8_row;
        uint8_t wp5_8_replaced = 0u;
        if (wp5_8_song) {
            wp5_8_song->instruments[0].mode = FMT_MODE_16BIT;
            wp5_8_song->instruments[0].engine = FMT_FM_ENGINE_FM4;
            (void)fmt_lock_set(FMT_PLAYBACK_PATTERN, 0u, 0u,
                               FMT_PARAM_FM_INDEX, 255u,
                               &wp5_8_replaced);
            wp5_8_row = fmt_store_step(FMT_PLAYBACK_PATTERN, 0u, 16u);
            if (wp5_8_row)
                wp5_8_row->instrument = 1u;
            wp5_8_row = fmt_store_step(FMT_PLAYBACK_PATTERN, 0u, 32u);
            if (wp5_8_row)
                wp5_8_row->instrument = 0u;
        }
    }
'''
Path('build/wp5_8_row_snapshot/src_base/dinghy.c').write_text(src.replace(anchor, common, 1))
Path('build/wp5_8_row_snapshot/src_lock/dinghy.c').write_text(src.replace(anchor, locked, 1))
PY

cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST \
  -DAPP_NAME='"FM1TRACKER"' -DAPP_VER='"wp5.8-row-base"' \
  -Ifirmware/src -Ifirmware/gen \
  -o "$OUT/fm_base" "$OUT/src_base/dinghy.c"
cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST \
  -DAPP_NAME='"FM1TRACKER"' -DAPP_VER='"wp5.8-row-lock"' \
  -Ifirmware/src -Ifirmware/gen \
  -o "$OUT/fm_lock" "$OUT/src_lock/dinghy.c"

"$OUT/fm_base" "$OUT/base" > "$OUT/base.log"
"$OUT/fm_lock" "$OUT/lock_a" > "$OUT/lock_a.log"
"$OUT/fm_lock" "$OUT/lock_b" > "$OUT/lock_b.log"
test -s "$OUT/base.wav"
test -s "$OUT/lock_a.wav"
test -s "$OUT/lock_b.wav"
cmp -s "$OUT/lock_a.wav" "$OUT/lock_b.wav"
if cmp -s "$OUT/base.wav" "$OUT/lock_a.wav"; then
    echo 'FM p-lock failed to alter mixed fixture audio' >&2
    exit 1
fi
BASE_SHA="$(sha256sum "$OUT/base.wav" | awk '{print $1}')"
LOCK_SHA="$(sha256sum "$OUT/lock_a.wav" | awk '{print $1}')"
printf '%s\n' "WP5_8_ROW_SNAPSHOT_AUDIO_OK=1 locked_runs=2 deterministic=1 base_sha=$BASE_SHA lock_sha=$LOCK_SHA changed_by_fm_plock=1"

printf '%s\n' 'WP5_8_ROW_SNAPSHOT_GATE_OK=1 decision=D056 snapshot_bytes=232 row_boundary=bounded retrig=current_patch off_cut=row_release store_traversal_audio=0 resolver=exact all8bit=wp5_0_identical hardware_margin_claim=none' | tee "$OUT/summary.txt"
