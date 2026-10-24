#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_1_operator
EXPECTED_TABLE_SHA=6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68
EXPECTED_LONG_SHA=499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66
EXPECTED_WAV_SHA=17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076
mkdir -p "$OUT" build/tests

python3 tools/gen_wp5_1_fm_sine.py "$OUT/fmt_fm_sine_q15.inc" | tee "$OUT/generator.log"
cmp -s "$OUT/fmt_fm_sine_q15.inc" firmware/gen/fmt_fm_sine_q15.inc
grep -F "sha256=$EXPECTED_TABLE_SHA" "$OUT/generator.log"
printf '%s\n' 'WP5_1_GENERATED_TABLE_IDENTITY_OK=1 checked_in=generator_exact payload_bytes=514'

cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
    tests/test_wp5_1_fm_operator.c firmware/src/fmt_fm_operator.c \
    -o build/tests/test_wp5_1_fm_operator

build/tests/test_wp5_1_fm_operator "$OUT/long_vector.bin" | tee "$OUT/focused.log"
grep -F 'WP5_1_FM_OPERATOR_VECTORS_OK=1 state_bytes=4 symmetry_probes=65536' "$OUT/focused.log"
grep -F 'WP5_1_FM_OPERATOR_RENDER_OK=1 sample_before_advance=1 modulation=full_int32 level_q15=0..32767' "$OUT/focused.log"
grep -F 'WP5_1_FM_OPERATOR_LONG_OK=1 samples=4096 final_phase=f1532bdf' "$OUT/focused.log"
LONG_SHA="$(sha256sum "$OUT/long_vector.bin" | awk '{print $1}')"
test "$LONG_SHA" = "$EXPECTED_LONG_SHA"
printf 'WP5_1_FM_OPERATOR_LONG_SHA_OK=1 sha256=%s\n' "$LONG_SHA" | tee "$OUT/long_sha.txt"

FILES='firmware/src/fmt_fm_operator.h firmware/src/fmt_fm_operator.c firmware/gen/fmt_fm_sine_q15.inc'
! grep -nE '\b(float|double|malloc|calloc|realloc|free)\b' $FILES
! grep -nE '\b(uint64_t|int64_t|long[[:space:]]+long)\b' $FILES
! grep -nE '\b(for|while|do)[[:space:]]*\(' firmware/src/fmt_fm_operator.c
BODY="$(sed -n '/void fmt_fm_operator_reset/,$p' firmware/src/fmt_fm_operator.c)"
! printf '%s\n' "$BODY" | grep -nE '[/%]'
! grep -nE 'Fmt(Song|Step|Instrument|Lock|Tracker|RtQueue)|fmt_lock_|fmt_store_|fmt_engine_router' firmware/src/fmt_fm_operator.c
grep -F 'uint32_t phase;' firmware/src/fmt_fm_operator.h
grep -F 'sample_phase = phase + (uint32_t)phase_modulation;' firmware/src/fmt_fm_operator.c
grep -F 'op->phase = phase + phase_increment;' firmware/src/fmt_fm_operator.c
printf '%s\n' 'WP5_1_FM_OPERATOR_STATIC_AUDIT_OK=1 render_loops=0 alloc=0 float=0 int64=0 divide=0 modulo=0 store_router_access=0'

sh tests/run_host.sh > "$OUT/default_host.log"
HOST_SHA="$(sha256sum build/host/fm1tracker.wav | awk '{print $1}')"
test "$HOST_SHA" = "$EXPECTED_WAV_SHA"
printf 'WP5_1_FULL_APP_WAV_IDENTITY_OK=1 sha256=%s\n' "$HOST_SHA" | tee "$OUT/default_wav.txt"

sh tests/run_wp5_0_router_host.sh | tee "$OUT/router.log"
grep -F 'WP5_0_ROUTER_CORE_OK=1 tracks=8 families=2 unknown_family=safe' "$OUT/router.log"
sh tests/run_wp5_0_representative_host.sh | tee "$OUT/representative.log"
grep -F 'WP5_0_REPRESENTATIVE_WAV_IDENTITY_OK=1 sha256=33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9' "$OUT/representative.log"

printf '%s\n' 'WP5_1_FM_OPERATOR_HOST_GATE_OK=1 d049=exact generator=exact long_vector=exact full_app_wav=wp5_0_identical router=pass representative=pass'
