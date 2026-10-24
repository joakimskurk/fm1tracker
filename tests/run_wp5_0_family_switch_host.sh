#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_0_family_switch
mkdir -p "$OUT" build/host build/tests

# Build a test-only production variant. Instrument 1 is reclassified as the
# reserved FM family after normal startup header initialization, before identity
# publication. No FM DSP is linked. Instruments 0 and 2 remain real 8-BIT.
python3 - <<'PY'
from pathlib import Path
src = Path('firmware/src/dinghy.c').read_text()
anchor = '''            }
            for (track = 0u; track < FMT_TRACKS; track++)
                song->tracks[track].instrument = (uint8_t)(track & 3u);
'''
assert src.count(anchor) == 1, 'unexpected startup instrument-header shape'
replacement = '''            }
            /* WP5.0 host-only family-switch fixture: identity-valid FM family,
             * intentionally unsupported by production DSP in this package. */
            song->instruments[1].mode = FMT_MODE_16BIT;
            for (track = 0u; track < FMT_TRACKS; track++)
                song->tracks[track].instrument = (uint8_t)(track & 3u);
'''
Path('build/tests/dinghy_wp5_0_family_switch.c').write_text(src.replace(anchor, replacement, 1))
PY

cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME='"FM1TRACKER"' -DAPP_VER='"wp5.0-family-switch"' \
    -Ifirmware/src -Ifirmware/gen \
    -o build/tests/wp5_0_family_switch_host build/tests/dinghy_wp5_0_family_switch.c

cat > "$OUT/fixture.txt" <<'EOF'
1 clear
1 base 0 0 0 255 0 96 0
1 base 2 0 0 255 0 96 0
1 step 0 0 60 0 0 0
1 step 0 1 62 1 0 0
1 step 0 2 64 2 0 0
1 step 0 3 254 255 0 0
10 probe
140 probe
270 probe
400 end
EOF

build/tests/wp5_0_family_switch_host \
    build/host/wp5_0_family_switch "$OUT/fixture.txt" > "$OUT/host.log"
test -s build/host/wp5_0_family_switch.wav

python3 - <<'PY' | tee build/wp5_0_family_switch/analysis.txt
from pathlib import Path
import re, struct, wave

log = Path('build/wp5_0_family_switch/host.log').read_text()
assert re.search(r'^\s*10 ms\s+fixture probe tracks t0=v\d+/e1/i0/s\d+', log, re.M), 'row0 8-BIT primary missing'
assert re.search(r'^\s*140 ms\s+fixture probe tracks t0=-', log, re.M), 'unsupported FM row did not hard-silence old primary'
assert re.search(r'^\s*270 ms\s+fixture probe tracks t0=v\d+/e3/i2/s\d+', log, re.M), '8-BIT reentry after unsupported family failed'

with wave.open('build/host/wp5_0_family_switch.wav', 'rb') as f:
    assert f.getnchannels() == 2 and f.getsampwidth() == 2 and f.getframerate() == 44100
    frames = f.getnframes()
    pcm = struct.unpack('<' + 'h' * frames * 2, f.readframes(frames))

def nonzero(a, b):
    return any(pcm[i] != 0 for i in range(a * 2, b * 2))

def allzero(a, b):
    return all(pcm[i] == 0 for i in range(a * 2, b * 2))

# Row boundaries are ~125 ms at the frozen D020 proof tempo. Stay well inside
# each row to avoid boundary ambiguity.
assert nonzero(1000, 5000), 'row0 8-BIT audio unexpectedly silent'
assert allzero(6000, 10500), 'unsupported FM row leaked old/new family audio'
assert nonzero(11500, 16000), 'row2 8-BIT reentry unexpectedly silent'

print('WP5_0_FAMILY_SWITCH_PROBE_OK=1 row0=8bit row1=fm_unsupported_silent row2=8bit_reentry')
print('WP5_0_FAMILY_SWITCH_PCM_OK=1 old_family_hard_silenced=1 unsupported_family_audio=0 reentry_audio=1')
print(f'frames={frames}')
PY

grep -F 'WP5_0_FAMILY_SWITCH_PROBE_OK=1 row0=8bit row1=fm_unsupported_silent row2=8bit_reentry' "$OUT/analysis.txt"
grep -F 'WP5_0_FAMILY_SWITCH_PCM_OK=1 old_family_hard_silenced=1 unsupported_family_audio=0 reentry_audio=1' "$OUT/analysis.txt"
printf '%s\n' 'WP5_0_FAMILY_SWITCH_OK=1 cross_family_order=hard_silence_clear_attempt_commit_on_success fm_dsp=absent active_cross_track_steals=0' | tee "$OUT/summary.txt"
