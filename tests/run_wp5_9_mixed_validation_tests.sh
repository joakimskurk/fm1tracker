#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

BASE=849c8b27d8c1afa09f9c093b695985c64e0c6013
FIX=bcab28cb8e7f7d774114008eeba54c18837b9bb7
OUT=build/wp5_9_mixed
SRC="$OUT/src"
mkdir -p "$OUT" "$SRC"
rm -f "$OUT"/*.wav "$OUT"/*.log

# Defect 001 amended the original validation-only source freeze. Relative to
# accepted D056 exactly dinghy.c may differ, and the current production tree
# must remain byte-for-byte the accepted narrow fix checkpoint.
actual="$(git diff --name-only "$BASE" "$FIX" -- firmware/src firmware/gen tools | sort)"
test "$actual" = 'firmware/src/dinghy.c'
git diff --exit-code "$FIX" -- firmware/src firmware/gen tools
python3 - <<'PY' > build/wp5_9_mixed/source_static.txt
from pathlib import Path
s = Path('firmware/src/dinghy.c').read_text()
assert s.count('WP5.9 Defect 001: NOTE_DELAY keeps the old route alive') == 1
assert s.count('uint8_t fm_snapshot_valid =') == 1
assert 'if (fm_snapshot_valid ||' in s
assert '!row_event->pending_valid ||' in s
assert 'FMT_ENGINE_FAMILY_FM)' in s
print('WP5_9_DEFECT_001_SOURCE_OK=1 production_delta=dinghy_only delayed_patch_validity=preserved state_growth=0')
PY
cat "$OUT/source_static.txt"
printf '%s\n' 'WP5_9_PRODUCTION_FIX_FREEZE_OK=1 base=d056 production_delta=dinghy_only fix=bcab28cb8e7f7d774114008eeba54c18837b9bb7' > "$OUT/source.txt"

# Build a WP5.9-only host platform copy. The target tree stays untouched and
# historical tests/host.c remains byte-for-byte unchanged. The generated host
# adds only public FM/router observations to the existing PROBE command.
python3 - <<'PY'
from pathlib import Path
out = Path('build/wp5_9_mixed/src')
out.mkdir(parents=True, exist_ok=True)
platform = Path('firmware/src/platform.c').read_text()
host = Path('tests/host.c').read_text()
needle = '#include "../../tests/host.c"                  /* on a computer: a fake FM-1 (tests/host.c) */'
assert platform.count(needle) == 1
platform = platform.replace(needle, '#include "host_wp5_9.c"                  /* WP5.9 generated mixed probe host */', 1)

inc = '#include "fmt_8bit_engine.h"\n'
assert host.count(inc) == 1
host = host.replace(inc, inc + '#include "fmt_fm_engine.h"\n#include "fmt_engine_router.h"\n', 1)
ext = 'extern Fmt8BitEngine fmt_8bit_engine_runtime;\n'
assert host.count(ext) == 1
host = host.replace(ext, ext + 'extern FmtFmEngine fmt_fm_engine_runtime;\nextern FmtEngineRouter fmt_engine_router_runtime;\n', 1)
probe = '''        } else if (script[step].fixture_action == HOST_FIX_PROBE) {
            uint32_t track, slot;
'''
assert host.count(probe) == 1
mixed = '''        } else if (script[step].fixture_action == HOST_FIX_PROBE) {
            uint32_t track, slot;
            printf("%6u ms  fixture mixed", fm1_ms);
            for (track = 0u; track < FMT_TRACKS; track++) {
                uint8_t route = fmt_engine_router_family(&fmt_engine_router_runtime, (uint8_t)track);
                uint8_t bv = fmt_8bit_engine_track_voice(&fmt_8bit_engine_runtime, (uint8_t)track);
                uint8_t fv = fmt_fm_engine_track_voice(&fmt_fm_engine_runtime, (uint8_t)track);
                printf(" t%u=r%u/b%u/f%u", track, route, bv, fv);
            }
            putchar('\\n');
'''
host = host.replace(probe, mixed, 1)
(out / 'platform.c').write_text(platform)
(out / 'host_wp5_9.c').write_text(host)
PY

# Generate source-injected fixtures after the normal proof-pattern seed but
# before linked caches/publications are rebuilt. No target source is changed.
python3 - <<'PY'
from pathlib import Path

out = Path('build/wp5_9_mixed/src')
base = Path('firmware/src/dinghy.c').read_text()
anchor = '    fmt_playback_seed_proof_pattern();\n'
assert base.count(anchor) == 1

prefix = r'''    fmt_playback_seed_proof_pattern();
    {
        FmtSong *wp5_9_song = fmt_store_song();
        FmtPattern *wp5_9_pattern = fmt_store_pattern(FMT_PLAYBACK_PATTERN);
        FmtStep *wp5_9_step;
        uint32_t wp5_9_track, wp5_9_row;
        uint8_t wp5_9_replaced = 0u;
        if (wp5_9_song && wp5_9_pattern) {
            for (wp5_9_track = 0u; wp5_9_track < FMT_TRACKS; wp5_9_track++)
                for (wp5_9_row = 0u; wp5_9_row < FMT_STEPS_PER_PATTERN; wp5_9_row++)
                    fmt_step_init(&wp5_9_pattern->steps[wp5_9_track][wp5_9_row]);
            wp5_9_song->instruments[0].mode = FMT_MODE_16BIT;
            wp5_9_song->instruments[0].engine = FMT_FM_ENGINE_FM4;
            wp5_9_song->instruments[1].mode = FMT_MODE_8BIT;
            wp5_9_song->instruments[1].engine = FMT_8BIT_ENGINE_PULSE;
            wp5_9_song->instruments[2].mode = FMT_MODE_16BIT;
            wp5_9_song->instruments[2].engine = 99u;
'''
suffix = r'''        }
    }
'''

def step(track, row, note, inst='FMT_INSTRUMENT_INHERIT', fx='FMT_TRACKER_EFFECT_NONE', value='0u'):
    return f'''            wp5_9_step = fmt_store_step(FMT_PLAYBACK_PATTERN, {track}u, {row}u);\n            if (wp5_9_step) {{ wp5_9_step->note = {note}; wp5_9_step->instrument = {inst}; wp5_9_step->effect = {fx}; wp5_9_step->effect_value = {value}; }}\n'''

def lock(track, row, param, value):
    return f'''            (void)fmt_lock_set(FMT_PLAYBACK_PATTERN, {track}u, {row}u, {param}, {value}, &wp5_9_replaced);\n'''

def write(name, body):
    text = base.replace(anchor, prefix + body + suffix, 1)
    (out / f'dinghy_{name}.c').write_text(text)

fm_note = step(0, 0, '60u', '0u') + step(0, 8, 'FMT_NOTE_OFF')
bit_note = step(1, 0, '67u', '1u') + step(1, 8, 'FMT_NOTE_OFF')
fm_valid = lock(0, 0, 'FMT_PARAM_FM_INDEX', '255u')
fm_wrong8 = lock(0, 0, 'FMT_PARAM_BIT8_PULSE_WIDTH', '1u')
bit_valid = lock(1, 0, 'FMT_PARAM_BIT8_PULSE_WIDTH', '64u')
bit_wrongfm = lock(1, 0, 'FMT_PARAM_FM_INDEX', '255u')

write('fm_base', fm_note)
write('fm_valid', fm_note + fm_valid)
write('fm_valid_wrong8', fm_note + fm_valid + fm_wrong8)
write('bit_base', bit_note)
write('bit_valid', bit_note + bit_valid)
write('bit_valid_wrongfm', bit_note + bit_valid + bit_wrongfm)
write('combined', fm_note + bit_note + fm_valid + bit_valid)

# Same-track family switch plus TIE, EMPTY, RETRIG and OFF while track 1 stays
# on 8-BIT throughout. A second variant omits RETRIG for the audio oracle.
def switch_body(with_retrig):
    b = ''
    b += step(0, 0, '60u', '0u')
    b += step(0, 1, 'FMT_NOTE_TIE')
    b += step(0, 2, '64u', '1u')
    # row 3 intentionally remains EMPTY
    b += step(0, 4, '67u', '0u')
    if with_retrig:
        b += step(0, 5, 'FMT_NOTE_TIE', fx='FMT_TRACKER_EFFECT_RETRIG', value='2u')
    else:
        b += step(0, 5, 'FMT_NOTE_TIE')
    b += step(0, 6, 'FMT_NOTE_OFF')
    b += step(1, 0, '48u', '1u')
    b += fm_valid
    return b
write('switch_retrig', switch_body(True))
write('switch_plain', switch_body(False))

# Defect 001 successful direction: FM -> delayed 8-BIT, followed by a real FM
# NOTE CUT. Track 1 remains an unrelated active 8-BIT owner throughout.
d = ''
d += step(0, 0, '60u', '0u')
d += step(0, 1, '64u', '1u', 'FMT_TRACKER_EFFECT_NOTE_DELAY', '3u')
d += step(0, 2, '67u', '0u', 'FMT_TRACKER_EFFECT_NOTE_CUT', '3u')
d += step(1, 0, '48u', '1u')
d += fm_valid
write('delayed_cut', d)

# Reverse successful direction required by F05: 8-BIT -> delayed FM4.
r = ''
r += step(0, 0, '60u', '1u')
r += step(0, 1, '64u', '0u', 'FMT_TRACKER_EFFECT_NOTE_DELAY', '3u')
r += step(1, 0, '48u', '1u')
r += lock(0, 1, 'FMT_PARAM_FM_INDEX', '255u')
write('delayed_reverse', r)

# Fail-to-silence timing required by F05: old FM survives until delay tick, then
# delayed NOTE targets an FM-family instrument with an unsupported engine.
bad = ''
bad += step(0, 0, '60u', '0u')
bad += step(0, 1, '64u', '2u', 'FMT_TRACKER_EFFECT_NOTE_DELAY', '3u')
bad += step(1, 0, '48u', '1u')
bad += fm_valid
write('delayed_bad', bad)
PY

compile_fixture() {
    name="$1"
    cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST \
      -DAPP_NAME='"FM1TRACKER"' -DAPP_VER='"wp5.9-mixed"' \
      -I"$SRC" -Ifirmware/src -Ifirmware/gen \
      -o "$OUT/$name" "$SRC/dinghy_$name.c"
}

for fixture in fm_base fm_valid fm_valid_wrong8 bit_base bit_valid bit_valid_wrongfm combined switch_retrig switch_plain delayed_cut delayed_reverse delayed_bad; do
    compile_fixture "$fixture"
done

cat > "$OUT/quiet.script" <<'EOF'
1200 end
EOF
cat > "$OUT/combined.script" <<'EOF'
10 probe
200 probe
900 probe
1200 end
EOF
cat > "$OUT/switch.script" <<'EOF'
10 probe
140 probe
270 probe
390 probe
520 probe
650 probe
770 probe
900 end
EOF
cat > "$OUT/delayed_cut.script" <<'EOF'
10 probe
140 probe
200 probe
270 probe
340 probe
500 end
EOF
cat > "$OUT/delayed_switch.script" <<'EOF'
10 probe
140 probe
200 probe
300 end
EOF

run_quiet() {
    exe="$1"
    name="$2"
    "$OUT/$exe" "$OUT/$name" "$OUT/quiet.script" > "$OUT/$name.log" 2>&1
    test -s "$OUT/$name.wav"
}

run_quiet fm_base fm_base
run_quiet fm_valid fm_valid_a
run_quiet fm_valid fm_valid_b
run_quiet fm_valid_wrong8 fm_valid_wrong8
run_quiet bit_base bit_base
run_quiet bit_valid bit_valid_a
run_quiet bit_valid bit_valid_b
run_quiet bit_valid_wrongfm bit_valid_wrongfm
"$OUT/combined" "$OUT/combined_a" "$OUT/combined.script" > "$OUT/combined_a.log" 2>&1
"$OUT/combined" "$OUT/combined_b" "$OUT/combined.script" > "$OUT/combined_b.log" 2>&1
"$OUT/switch_retrig" "$OUT/switch_a" "$OUT/switch.script" > "$OUT/switch_a.log" 2>&1
"$OUT/switch_retrig" "$OUT/switch_b" "$OUT/switch.script" > "$OUT/switch_b.log" 2>&1
"$OUT/switch_plain" "$OUT/switch_plain" "$OUT/switch.script" > "$OUT/switch_plain.log" 2>&1
"$OUT/delayed_cut" "$OUT/delayed_cut_a" "$OUT/delayed_cut.script" > "$OUT/delayed_cut_a.log" 2>&1
"$OUT/delayed_cut" "$OUT/delayed_cut_b" "$OUT/delayed_cut.script" > "$OUT/delayed_cut_b.log" 2>&1
"$OUT/delayed_reverse" "$OUT/delayed_reverse_a" "$OUT/delayed_switch.script" > "$OUT/delayed_reverse_a.log" 2>&1
"$OUT/delayed_reverse" "$OUT/delayed_reverse_b" "$OUT/delayed_switch.script" > "$OUT/delayed_reverse_b.log" 2>&1
"$OUT/delayed_bad" "$OUT/delayed_bad_a" "$OUT/delayed_switch.script" > "$OUT/delayed_bad_a.log" 2>&1
"$OUT/delayed_bad" "$OUT/delayed_bad_b" "$OUT/delayed_switch.script" > "$OUT/delayed_bad_b.log" 2>&1

cmp -s "$OUT/fm_valid_a.wav" "$OUT/fm_valid_b.wav"
cmp -s "$OUT/bit_valid_a.wav" "$OUT/bit_valid_b.wav"
cmp -s "$OUT/combined_a.wav" "$OUT/combined_b.wav"
cmp -s "$OUT/switch_a.wav" "$OUT/switch_b.wav"
cmp -s "$OUT/delayed_cut_a.wav" "$OUT/delayed_cut_b.wav"
cmp -s "$OUT/delayed_reverse_a.wav" "$OUT/delayed_reverse_b.wav"
cmp -s "$OUT/delayed_bad_a.wav" "$OUT/delayed_bad_b.wav"

python3 - <<'PY' > build/wp5_9_mixed/analysis.txt
from pathlib import Path
import re, struct, wave, hashlib

ROOT = Path('build/wp5_9_mixed')
NONE = 255
FM = 1
BIT = 0

def wav(name):
    p = ROOT / f'{name}.wav'
    with wave.open(str(p), 'rb') as f:
        assert f.getnchannels() == 2
        assert f.getsampwidth() == 2
        assert f.getframerate() == 44100
        frames = f.getnframes()
        data = struct.unpack('<' + 'h' * frames * 2, f.readframes(frames))
    return frames, data

def sha(name):
    return hashlib.sha256((ROOT / f'{name}.wav').read_bytes()).hexdigest()

def clamp(v):
    if v > 32767: return 32767
    if v < -32767: return -32767
    return v

def mixed_rows(log_name):
    text = (ROOT / log_name).read_text()
    rows = {}
    for line in text.splitlines():
        if ' fixture mixed ' not in line:
            continue
        m = re.match(r'\s*(\d+) ms\s+fixture mixed(.*)', line)
        assert m, line
        t = int(m.group(1))
        tracks = {}
        for tm in re.finditer(r't(\d+)=r(\d+)/b(\d+)/f(\d+)', m.group(2)):
            tracks[int(tm.group(1))] = tuple(map(int, tm.groups()[1:]))
        assert len(tracks) == 8, line
        rows[t] = tracks
    return rows

def owner(tracks, track, route, want_bit, want_fm):
    r, b, f = tracks[track]
    assert r == route, (track, tracks[track], route)
    assert (b != NONE) == want_bit, (track, tracks[track], 'bit', want_bit)
    assert (f != NONE) == want_fm, (track, tracks[track], 'fm', want_fm)
    assert not (b != NONE and f != NONE), (track, tracks[track], 'dual owner')

# Valid family-local p-locks alter their own real engine deterministically.
_, fm_base = wav('fm_base')
_, fm_valid = wav('fm_valid_a')
_, bit_base = wav('bit_base')
_, bit_valid = wav('bit_valid_a')
assert fm_base != fm_valid, 'FM lock did not alter FM audio'
assert bit_base != bit_valid, '8BIT lock did not alter pulse audio'
# Wrong-family locks are stored but non-applicable in the real integrated path.
assert wav('fm_valid_wrong8')[1] == fm_valid, '8BIT lock aliased into FM'
assert wav('bit_valid_wrongfm')[1] == bit_valid, 'FM lock aliased into 8BIT'
print('WP5_9_PLOCK_FAMILY_SAFE_OK=1 fm_lock=audible bit8_lock=audible wrong8_on_fm=ignored wrongfm_on_bit8=ignored')

# Exact simultaneous-family mixer oracle.
ff, fm = wav('fm_valid_a')
bf, bit = wav('bit_valid_a')
cf, combined = wav('combined_a')
assert ff == bf == cf
lo, hi = 1000, min(40000, cf)
both_nonzero = 0
for frame in range(lo, hi):
    for ch in (0, 1):
        i = frame * 2 + ch
        if fm[i] and bit[i]:
            both_nonzero += 1
        assert combined[i] == clamp(fm[i] + bit[i]), (frame, ch, fm[i], bit[i], combined[i])
assert both_nonzero > 1000, both_nonzero
rows = mixed_rows('combined_a.log')
for t in (10, 200, 900):
    owner(rows[t], 0, FM, False, True)
    owner(rows[t], 1, BIT, True, False)
print(f'WP5_9_SIMULTANEOUS_AUDIO_OK=1 frames={cf} overlap_both_nonzero={both_nonzero} oracle=saturated_sum repeats=byte_identical sha256={sha("combined_a")}')
print('WP5_9_SIMULTANEOUS_OWNERSHIP_OK=1 track0=fm track1=8bit dual_owner=0 active_cross_track_steals=0')

# Same-track successful family switching with an unrelated 8BIT owner alive.
sw = mixed_rows('switch_a.log')
for t in (10, 140):
    owner(sw[t], 0, FM, False, True)
    owner(sw[t], 1, BIT, True, False)
for t in (270, 390):
    owner(sw[t], 0, BIT, True, False)
    owner(sw[t], 1, BIT, True, False)
for t in (520, 650):
    owner(sw[t], 0, FM, False, True)
    owner(sw[t], 1, BIT, True, False)
owner(sw[770], 0, 255, False, False)
owner(sw[770], 1, BIT, True, False)
assert wav('switch_a')[1] != wav('switch_plain')[1], 'integrated FM retrig did not alter audio'
print('WP5_9_REAL_SWITCH_OK=1 sequence=fm_to_8bit_to_fm tie=route_stable empty=route_stable off=route_clear unrelated_track=stable dual_owner=0')
print('WP5_9_RETRIG_INTEGRATED_OK=1 family=fm route=stable audio_changed=1 deterministic=1')

# Defect 001: FM -> delayed 8BIT keeps FM until tick 3, then commits 8BIT.
# Row 2 immediately returns to FM and NOTE CUT clears it after tick 3.
dc = mixed_rows('delayed_cut_a.log')
owner(dc[10], 0, FM, False, True)
owner(dc[140], 0, FM, False, True)
owner(dc[200], 0, BIT, True, False)
owner(dc[270], 0, FM, False, True)
owner(dc[340], 0, 255, False, False)
for t in (10, 140, 200, 270, 340):
    owner(dc[t], 1, BIT, True, False)
print('WP5_9_DELAY_CUT_OK=1 direction=fm_to_8bit old_family_until_tick=1 cut=route_clear unrelated_track=stable')

# Reverse direction: 8BIT stays active until tick 3, then normal route commits FM.
dr = mixed_rows('delayed_reverse_a.log')
owner(dr[10], 0, BIT, True, False)
owner(dr[140], 0, BIT, True, False)
owner(dr[200], 0, FM, False, True)
for t in (10, 140, 200):
    owner(dr[t], 1, BIT, True, False)
print('WP5_9_DELAY_REVERSE_OK=1 direction=8bit_to_fm old_family_until_tick=1 new_family_after_tick=1 unrelated_track=stable')

# Unsupported delayed FM replacement: old FM stays alive until tick 3, then
# normal note routing fails toward NONE/silence at the scheduled tick.
db = mixed_rows('delayed_bad_a.log')
owner(db[10], 0, FM, False, True)
owner(db[140], 0, FM, False, True)
owner(db[200], 0, NONE, False, False)
for t in (10, 140, 200):
    owner(db[t], 1, BIT, True, False)
print('WP5_9_DELAY_UNSUPPORTED_OK=1 old_family_until_tick=1 unsupported_at_tick=route_none unrelated_track=stable')

print('WP5_9_MIXED_HOST_OK=1 simultaneous=real switch=real events=tie_empty_retrig_off_delay_cut delayed_bidirectional=1 delayed_unsupported=1 plock_family_safe=1 production_delta=dinghy_fix deterministic=1')
PY

cat "$OUT/analysis.txt"
grep -F 'WP5_9_PLOCK_FAMILY_SAFE_OK=1 fm_lock=audible bit8_lock=audible wrong8_on_fm=ignored wrongfm_on_bit8=ignored' "$OUT/analysis.txt"
grep -F 'WP5_9_SIMULTANEOUS_OWNERSHIP_OK=1 track0=fm track1=8bit dual_owner=0 active_cross_track_steals=0' "$OUT/analysis.txt"
grep -F 'WP5_9_REAL_SWITCH_OK=1 sequence=fm_to_8bit_to_fm tie=route_stable empty=route_stable off=route_clear unrelated_track=stable dual_owner=0' "$OUT/analysis.txt"
grep -F 'WP5_9_RETRIG_INTEGRATED_OK=1 family=fm route=stable audio_changed=1 deterministic=1' "$OUT/analysis.txt"
grep -F 'WP5_9_DELAY_CUT_OK=1 direction=fm_to_8bit old_family_until_tick=1 cut=route_clear unrelated_track=stable' "$OUT/analysis.txt"
grep -F 'WP5_9_DELAY_REVERSE_OK=1 direction=8bit_to_fm old_family_until_tick=1 new_family_after_tick=1 unrelated_track=stable' "$OUT/analysis.txt"
grep -F 'WP5_9_DELAY_UNSUPPORTED_OK=1 old_family_until_tick=1 unsupported_at_tick=route_none unrelated_track=stable' "$OUT/analysis.txt"
grep -F 'WP5_9_MIXED_HOST_OK=1 simultaneous=real switch=real events=tie_empty_retrig_off_delay_cut delayed_bidirectional=1 delayed_unsupported=1 plock_family_safe=1 production_delta=dinghy_fix deterministic=1' "$OUT/analysis.txt"

# Re-prove inherited D056 behavior on the amended production seam. D049-D055,
# D056 parameter/cache/editor behavior and exact all-8BIT WAV remain mandatory.
sh tests/run_wp5_8_closeout_host.sh > "$OUT/inherited_d056.log" 2>&1
cat "$OUT/inherited_d056.log"
grep -F 'WP5_8_CLOSEOUT_HOST_OK=1 decision=D056 ids=27 pages=7 locks=4 cache=8704 base_bank=432 snapshot=232 editor=page_aware base_api=bounded d055=exact d054_d049=exact all8bit=wp5_0_identical realtime=pass hardware_margin_claim=none' "$OUT/inherited_d056.log"

printf '%s\n' 'WP5_9_HOST_GATE_OK=1 production_delta=dinghy_fix simultaneous=real saturated_sum=exact switch=real events=covered delayed_bidirectional=1 delayed_unsupported=1 plock_family_safe=exact d056=exact all8bit=wp5_0_identical hardware_margin_claim=none' > "$OUT/summary.txt"
cat "$OUT/summary.txt"
