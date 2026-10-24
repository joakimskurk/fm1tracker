#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_2_algorithm
mkdir -p "$OUT" build/tests

cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
    tests/test_wp5_2_fm_algorithm.c \
    firmware/src/fmt_fm_algorithm.c \
    firmware/src/fmt_fm_operator.c \
    -o build/tests/test_wp5_2_fm_algorithm

build/tests/test_wp5_2_fm_algorithm "$OUT" | tee "$OUT/focused.log"
grep -F 'WP5_2_FM_ALGORITHM_VECTORS_OK=1 algorithms=4 state_bytes=16 order=op4_op3_op2_op1' "$OUT/focused.log"
grep -F 'WP5_2_FM_ALGORITHM_SWITCH_OK=1 phase_continuity=exact operators_per_sample=4' "$OUT/focused.log"
grep -F 'WP5_2_FM_ALGORITHM_STRESS_OK=1 algorithms=4 renders_each=65536 output_q15=bounded phase_advance=exact' "$OUT/focused.log"

check_hash() {
    file="$1"
    expected="$2"
    actual="$(sha256sum "$file" | awk '{print $1}')"
    test "$actual" = "$expected"
    printf '%s %s\n' "$actual" "$file"
}

{
    check_hash "$OUT/algorithm0.bin" 25dfde1bb29de2c70305d6abafee91fd7480e69eaa71fd1a3fbde3c1e6e855cd
    check_hash "$OUT/algorithm1.bin" 8d9ae5310369e3d5a36bcf8c44d5e74049d3d6e109362a844be2796b26330653
    check_hash "$OUT/algorithm2.bin" c4c31cabf4b8e121591c4c1e0923aee67523eaf7d5b60b60ca1a545c13fc2269
    check_hash "$OUT/algorithm3.bin" d316c3e5467cbfc606c788f669a3b19dba13535969dcce3b67b2821f0977a882
} | tee "$OUT/hashes.txt"
printf '%s\n' 'WP5_2_FM_ALGORITHM_HASHES_OK=1 stack4=exact fan3=exact dual2=exact parallel4=exact final_phases=4476a040,95c7f080,e71940c0,386a9000'

FILES='firmware/src/fmt_fm_algorithm.h firmware/src/fmt_fm_algorithm.c'
! grep -nE '\b(float|double|malloc|calloc|realloc|free)\b' $FILES
! grep -nE '\b(uint64_t|int64_t|long[[:space:]]+long)\b' $FILES
! grep -nE '\b(for|while|do)[[:space:]]*\(' firmware/src/fmt_fm_algorithm.c
BODY="$(sed -n '/int32_t fmt_fm_algorithm_render/,$p' firmware/src/fmt_fm_algorithm.c)"
! printf '%s\n' "$BODY" | grep -nE '[/%]'
! grep -nE 'Fmt(Song|Step|Instrument|Lock|Tracker|RtQueue)|fmt_lock_|fmt_store_|fmt_engine_router|lcd_|draw_' firmware/src/fmt_fm_algorithm.c
grep -F 'FmtFmOperator op[4];' firmware/src/fmt_fm_algorithm.h
grep -F 'selected = fmt_fm_algorithm_sanitize(algorithm);' firmware/src/fmt_fm_algorithm.c
grep -F 'depth = fmt_fm_algorithm_clamp_mod_depth_q14(mod_depth_q14);' firmware/src/fmt_fm_algorithm.c
printf '%s\n' 'WP5_2_FM_ALGORITHM_STATIC_AUDIT_OK=1 render_loops=0 graph_traversal=0 alloc=0 float=0 int64=0 divide=0 modulo=0 store_router_ui=0'

# Re-prove the complete D049 implementation and inherited WP5.0/D046 host baseline.
sh tests/run_wp5_1_fm_operator_tests.sh | tee "$OUT/d049_inherited.log"
grep -F 'WP5_1_FM_OPERATOR_HOST_GATE_OK=1 d049=exact generator=exact long_vector=exact full_app_wav=wp5_0_identical router=pass representative=pass' "$OUT/d049_inherited.log"

printf '%s\n' 'WP5_2_FM_ALGORITHM_HOST_GATE_OK=1 d050=exact algorithms=4 hashes=exact stress=pass d049=exact full_app_wav=wp5_0_identical'
