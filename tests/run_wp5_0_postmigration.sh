#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_0_postmigration
EXPECTED_HOST_WAV=17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076
EXPECTED_SDK=d179b4484759423312073f5fbb232501aa491047
mkdir -p "$OUT" build/host build/tests

# Production source must now contain the verified D048 adapter directly.
grep -F '#include "fmt_engine_router.c"' firmware/src/dinghy.c
grep -F 'FmtEngineRouter fmt_engine_router_runtime;' firmware/src/dinghy.c
grep -F 'FmtInstrumentIdentityPublication fmt_instrument_identity_publication' firmware/src/dinghy.c
grep -F 'fmt_instrument_identity_publish_song(' firmware/src/dinghy.c
grep -F 'tracker_hard_silence_family(track, current_family);' firmware/src/dinghy.c
grep -F 'fmt_engine_router_commit(&fmt_engine_router_runtime,' firmware/src/dinghy.c
if grep -F 'song->meta.mode != FMT_MODE_8BIT' firmware/src/dinghy.c; then
    echo 'obsolete song-mode playback gate survived' >&2
    exit 1
fi
if grep -R -E 'fmt_fm|FM4 operator|fm_operator|phase modulation' firmware/src; then
    echo 'FM DSP entered WP5.0 unexpectedly' >&2
    exit 1
fi
printf '%s\n' 'WP5_0_POSTMIGRATION_SOURCE_OK=1 router=production song_mode_gate=absent fm_dsp=absent'

CACHE_DEFS="$(grep -R -h '^volatile Fmt8BitRowParamCache fmt_8bit_row_param_cache' firmware/src | wc -l | tr -d ' ')"
CACHE_ASSERTS="$(grep -R -h -F 'sizeof(fmt_8bit_row_param_cache) == 2048u' firmware/src | wc -l | tr -d ' ')"
test "$CACHE_DEFS" -eq 1
test "$CACHE_ASSERTS" -eq 1
printf 'WP5_0_POSTMIGRATION_SHARED_CACHE_OK=1 definitions=%s size_asserts=%s bytes=2048\n' "$CACHE_DEFS" "$CACHE_ASSERTS"

# Pure router/publication semantics remain green against production headers.
sh tests/run_wp5_0_router_host.sh | tee "$OUT/router_core.log"
grep -F 'WP5_0_ROUTER_SIZE_OK=1 bytes=8' "$OUT/router_core.log"
grep -F 'WP5_0_IDENTITY_PUBLICATION_OK=1 instruments=16 bytes=64' "$OUT/router_core.log"
grep -F 'WP5_0_ROUTER_CORE_OK=1 tracks=8 families=2 unknown_family=safe' "$OUT/router_core.log"
grep -F 'WP5_0_SONG_MODE_NONGATING_PUBLICATION_OK=1 mode0_mode1_identity_words=identical' "$OUT/router_core.log"

# Every canonical D045 linked engine/effect waveform must remain byte-identical.
sh tests/run_wp4_9_linked_audio_host.sh | tee "$OUT/linked_audio.log"
grep -F 'WP4_9_GATE5_CANONICAL_OK=1' "$OUT/linked_audio.log"
grep -F 'WP4_9_GATE5_HOST_MATRIX_CANONICAL_OK=1' "$OUT/linked_audio.log"

# Normal committed app must retain the exact D046 full-app stream.
sh tests/run_host.sh | tee "$OUT/default_host.log"
HOST_SHA="$(sha256sum build/host/fm1tracker.wav | awk '{print $1}')"
HOST_FRAMES="$(python3 - <<'PY'
import wave
with wave.open('build/host/fm1tracker.wav', 'rb') as f:
    print(f.getnframes())
PY
)"
test "$HOST_SHA" = "$EXPECTED_HOST_WAV"
test "$HOST_FRAMES" = '286464'
printf 'WP5_0_POSTMIGRATION_D046_WAV_OK=1 sha256=%s frames=%s\n' "$HOST_SHA" "$HOST_FRAMES" | tee "$OUT/wav_identity.txt"

# Actual-app legacy metadata independence: only startup FmtSongMeta.mode changes.
python3 - <<'PY'
from pathlib import Path
src = Path('firmware/src/dinghy.c').read_text()
old = '            song->meta.mode = FMT_MODE_8BIT;\n'
new = '            song->meta.mode = FMT_MODE_16BIT;\n'
assert src.count(old) == 1, 'unexpected startup song-mode assignment shape'
Path('build/tests/dinghy_wp5_0_songmode16.c').write_text(src.replace(old, new, 1))
PY
cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME='"FM1TRACKER"' -DAPP_VER='"dev"' \
    -Ifirmware/src -Ifirmware/gen \
    -o build/host/fm1tracker_songmode16 build/tests/dinghy_wp5_0_songmode16.c
build/host/fm1tracker_songmode16 build/host/fm1tracker_songmode16 > "$OUT/songmode16_host.log"
MODE16_SHA="$(sha256sum build/host/fm1tracker_songmode16.wav | awk '{print $1}')"
MODE16_FRAMES="$(python3 - <<'PY'
import wave
with wave.open('build/host/fm1tracker_songmode16.wav', 'rb') as f:
    print(f.getnframes())
PY
)"
test "$MODE16_SHA" = "$EXPECTED_HOST_WAV"
test "$MODE16_FRAMES" = "$HOST_FRAMES"
cmp -s build/host/fm1tracker.wav build/host/fm1tracker_songmode16.wav
printf 'WP5_0_POSTMIGRATION_SONG_META_NONGATING_OK=1 mode8_mode16_wav=byte_identical sha256=%s frames=%s\n' \
    "$MODE16_SHA" "$MODE16_FRAMES" | tee "$OUT/song_meta_nongating.txt"

# Pinned real target evidence on committed production source.
: "${RUNNER_TEMP:?RUNNER_TEMP is required for the pinned WP5.0 post-migration gate}"
./tools/get_toolchain.sh "$RUNNER_TEMP/jieli"
test -x "$RUNNER_TEMP/jieli/toolchain/pi32v2/bin/clang"
export JIELI_TOOLCHAIN="$RUNNER_TEMP/jieli/toolchain"
"$JIELI_TOOLCHAIN/pi32v2/bin/clang" --version | head -n 2 | tee "$OUT/toolchain.txt"

SDK="$RUNNER_TEMP/ac79-sdk"
rm -rf "$SDK"
git clone --depth 1 --branch AC79NN_SDK_V1.2.1_2023-12-13 \
    https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK.git "$SDK"
ACTUAL_SDK="$(git -C "$SDK" rev-parse HEAD)"
test "$ACTUAL_SDK" = "$EXPECTED_SDK"
printf 'AC79_SDK_COMMIT=%s\n' "$ACTUAL_SDK" | tee "$OUT/sdk.txt"
export AC79_SDK="$SDK"

./build.sh --release 0.1 | tee "$OUT/device.log"
test -s build/fm1tracker.bin
test -s build/fm1tracker-0.1.fwsc
test -s build/fm1tracker.elf
"$JIELI_TOOLCHAIN/common/bin/objdump" -t build/fm1tracker.elf > "$OUT/symbols.txt"

python3 - <<'PY' | tee "$OUT/metrics.txt"
import hashlib, re
from pathlib import Path
log = Path('build/wp5_0_postmigration/device.log').read_text()
m = re.search(r'image (\d+) B of (\d+); RAM \.data\+\.bss (\d+) B of 98304; pool (\d+) B of 344064', log)
assert m, log[-4000:]
image, image_limit, ram, pool = map(int, m.groups())
assert image <= 76600, image
assert ram <= 19860, ram
assert pool <= 113000, pool
assert image - 72504 <= 4096, image
assert ram - 19604 <= 256, ram
assert pool - 113000 <= 0, pool

def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()
print('WP5_0_POSTMIGRATION_TARGET_OK=1')
print(f'image_bytes={image}')
print(f'normal_ram_bytes={ram}')
print(f'pool_ram_bytes={pool}')
print(f'image_delta_vs_d046={image-72504}')
print(f'normal_ram_delta_vs_d046={ram-19604}')
print(f'pool_delta_vs_d046={pool-113000}')
print(f'fm1tracker_bin_sha256={sha("build/fm1tracker.bin")}')
print(f'fm1tracker_fwsc_sha256={sha("build/fm1tracker-0.1.fwsc")}')
PY

grep -F 'FM1Tracker package' "$OUT/device.log"
grep -F 'identity FM-1_987' "$OUT/device.log"
grep -F 'WP5_0_POSTMIGRATION_TARGET_OK=1' "$OUT/metrics.txt"
printf '%s\n' 'WP5_0_POSTMIGRATION_OK=1 production=committed d046_wav=byte_identical song_meta=nongating canonical_8bit=pass target=pinned fm_dsp=absent hardware_margin_claim=none' | tee "$OUT/summary.txt"
