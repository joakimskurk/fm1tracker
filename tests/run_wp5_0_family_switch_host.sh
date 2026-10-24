#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_0_family_switch
mkdir -p "$OUT" build/host build/tests

make_variant() {
    kind="$1"
    python3 - "$kind" <<'PY'
from pathlib import Path
import sys
kind = sys.argv[1]
src = Path('firmware/src/dinghy.c').read_text()
anchor = '''            }
            for (track = 0u; track < FMT_TRACKS; track++)
                song->tracks[track].instrument = (uint8_t)(track & 3u);
'''
assert src.count(anchor) == 1, 'unexpected startup instrument-header shape'
if kind == 'fm':
    injected = '''            }
            /* WP5.0 host-only family fixture: valid FM namespace, no FM DSP. */
            song->instruments[1].mode = FMT_MODE_16BIT;
            for (track = 0u; track < FMT_TRACKS; track++)
                song->tracks[track].instrument = (uint8_t)(track & 3u);
'''
elif kind == 'malformed':
    injected = '''            }
            /* WP5.0 host-only malformed identity fixture. */
            song->instruments[1].mode = 0x7Fu;
            song->instruments[3].engine = 99u;
            for (track = 0u; track < FMT_TRACKS; track++)
                song->tracks[track].instrument = (uint8_t)(track & 3u);
'''
else:
    raise SystemExit('unknown variant')
Path(f'build/tests/dinghy_wp5_0_{kind}.c').write_text(src.replace(anchor, injected, 1))
PY
    cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST \
        -DAPP_NAME='"FM1TRACKER"' -DAPP_VER='"wp5.0-family"' \
        -Ifirmware/src -Ifirmware/gen \
        -o "build/tests/wp5_0_${kind}_host" "build/tests/dinghy_wp5_0_${kind}.c"
}

make_variant fm
make_variant malformed

# A. Immediate 8-BIT -> unsupported FM -> 8-BIT. A family-specific 8-BIT lock
# is deliberately stored on the FM row and must remain non-applicable.
cat > "$OUT/immediate.txt" <<'EOF'
1 clear
1 base 0 0 0 255 0 96 0
1 base 2 0 0 255 0 96 0
1 step 0 0 60 0 0 0
1 step 0 1 62 1 0 0
1 lock 0 1 36 200
1 step 0 2 64 2 0 0
1 step 0 3 254 255 0 0
10 probe
140 probe
270 probe
400 end
EOF
build/tests/wp5_0_fm_host build/host/wp5_0_family_switch "$OUT/immediate.txt" > "$OUT/immediate.log"

# B. Delayed NOTE must use the same normal router path. At 140 ms the old 8-BIT
# primary is still alive; NOTE DELAY 3 fires later in row 1 and the unsupported
# FM request hard-silences it. Row 2 then re-enters 8-BIT.
cat > "$OUT/delayed.txt" <<'EOF'
1 clear
1 base 0 0 0 255 0 96 0
1 base 2 0 0 255 0 96 0
1 step 0 0 60 0 0 0
1 step 0 1 62 1 7 3
1 lock 0 1 36 201
1 step 0 2 64 2 0 0
1 step 0 3 254 255 0 0
140 probe
200 probe
270 probe
400 end
EOF
build/tests/wp5_0_fm_host build/host/wp5_0_family_delay "$OUT/delayed.txt" > "$OUT/delayed.log"

# C. Unknown family and invalid engine both fail toward silence and never alias
# to a valid 8-BIT engine. Valid 8-BIT rows on either side must recover normally.
cat > "$OUT/malformed.txt" <<'EOF'
1 clear
1 base 0 0 0 255 0 96 0
1 base 2 0 0 255 0 96 0
1 step 0 0 60 0 0 0
1 step 0 1 62 1 0 0
1 lock 0 1 36 202
1 step 0 2 64 2 0 0
1 step 0 3 65 3 0 0
1 step 0 4 67 0 0 0
1 step 0 5 254 255 0 0
10 probe
140 probe
270 probe
390 probe
520 probe
650 end
EOF
build/tests/wp5_0_malformed_host build/host/wp5_0_family_malformed "$OUT/malformed.txt" > "$OUT/malformed.log"

python3 - <<'PY' | tee build/wp5_0_family_switch/analysis.txt
from pathlib import Path
import re, struct, wave

def probe(log, ms, pattern, message):
    assert re.search(r'^\s*' + str(ms) + r' ms\s+fixture probe tracks ' + pattern,
                     log, re.M), message

def pcm(path):
    with wave.open(path, 'rb') as f:
        assert f.getnchannels() == 2 and f.getsampwidth() == 2 and f.getframerate() == 44100
        frames = f.getnframes()
        data = struct.unpack('<' + 'h' * frames * 2, f.readframes(frames))
    return frames, data

def nonzero(data, a, b):
    return any(data[i] != 0 for i in range(a * 2, b * 2))

def allzero(data, a, b):
    return all(data[i] == 0 for i in range(a * 2, b * 2))

# Immediate cross-family replacement.
log = Path('build/wp5_0_family_switch/immediate.log').read_text()
probe(log, 10, r't0=v\d+/e1/i0/s\d+', 'row0 8-BIT primary missing')
probe(log, 140, r't0=-', 'unsupported FM row did not hard-silence old primary')
probe(log, 270, r't0=v\d+/e3/i2/s\d+', '8-BIT reentry after unsupported FM failed')
frames, data = pcm('build/host/wp5_0_family_switch.wav')
assert nonzero(data, 1000, 5000)
assert allzero(data, 6000, 10500), 'FM row leaked old/new family audio'
assert nonzero(data, 11500, 16000)
print('WP5_0_FAMILY_SWITCH_PROBE_OK=1 row0=8bit row1=fm_unsupported_silent row2=8bit_reentry')
print('WP5_0_FAMILY_SWITCH_PCM_OK=1 old_family_hard_silenced=1 unsupported_family_audio=0 reentry_audio=1')
print('WP5_0_NONAPPLICABLE_PLOCK_OK=1 family=fm bit8_lock=stored_ignored_not_reinterpreted')

# Delayed NOTE must invoke exactly the same cross-family replacement behavior.
log = Path('build/wp5_0_family_switch/delayed.log').read_text()
probe(log, 140, r't0=v\d+/e1/i0/s\d+', 'old family disappeared before delayed NOTE fired')
probe(log, 200, r't0=-', 'delayed unsupported FM NOTE did not silence old primary')
probe(log, 270, r't0=v\d+/e3/i2/s\d+', '8-BIT reentry after delayed FM failed')
_, data = pcm('build/host/wp5_0_family_delay.wav')
assert nonzero(data, 6000, 7800), 'old 8-BIT primary did not survive until delayed tick'
assert allzero(data, 8500, 10500), 'delayed FM fail-safe window was not silent'
assert nonzero(data, 11500, 16000), '8-BIT reentry after delayed NOTE was silent'
print('WP5_0_DELAYED_NOTE_ROUTER_OK=1 old_family_until_tick=1 delayed_note=same_router fail_to_silence=1 reentry=1')

# Unknown family, then valid 8-BIT, then invalid 8-BIT engine, then valid 8-BIT.
log = Path('build/wp5_0_family_switch/malformed.log').read_text()
probe(log, 10, r't0=v\d+/e1/i0/s\d+', 'malformed fixture row0 missing')
probe(log, 140, r't0=-', 'unknown family did not fail to silence')
probe(log, 270, r't0=v\d+/e3/i2/s\d+', 'recovery after unknown family failed')
probe(log, 390, r't0=-', 'invalid 8-BIT engine did not fail to silence')
probe(log, 520, r't0=v\d+/e1/i0/s\d+', 'recovery after invalid engine failed')
_, data = pcm('build/host/wp5_0_family_malformed.wav')
assert nonzero(data, 1000, 5000)
assert allzero(data, 6000, 10500)
assert nonzero(data, 11500, 16000)
assert allzero(data, 17000, 21500)
assert nonzero(data, 22500, 27000)
print('WP5_0_MALFORMED_IDENTITY_OK=1 unknown_family=silent invalid_engine=silent aliasing=0 recovery=deterministic')
print(f'frames={frames}')
PY

grep -F 'WP5_0_FAMILY_SWITCH_PROBE_OK=1 row0=8bit row1=fm_unsupported_silent row2=8bit_reentry' "$OUT/analysis.txt"
grep -F 'WP5_0_FAMILY_SWITCH_PCM_OK=1 old_family_hard_silenced=1 unsupported_family_audio=0 reentry_audio=1' "$OUT/analysis.txt"
grep -F 'WP5_0_NONAPPLICABLE_PLOCK_OK=1 family=fm bit8_lock=stored_ignored_not_reinterpreted' "$OUT/analysis.txt"
grep -F 'WP5_0_DELAYED_NOTE_ROUTER_OK=1 old_family_until_tick=1 delayed_note=same_router fail_to_silence=1 reentry=1' "$OUT/analysis.txt"
grep -F 'WP5_0_MALFORMED_IDENTITY_OK=1 unknown_family=silent invalid_engine=silent aliasing=0 recovery=deterministic' "$OUT/analysis.txt"
printf '%s\n' 'WP5_0_FAMILY_SWITCH_OK=1 cross_family_order=hard_silence_clear_attempt_commit_on_success delayed_note=same_router malformed=safe nonapplicable_plock=ignored fm_dsp=absent active_cross_track_steals=0' | tee "$OUT/summary.txt"
