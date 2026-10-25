#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/wp5_8_params

EXPECTED_WAV=17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076

sh tests/run_param_registry_tests.sh | tee build/wp5_8_params/registry.log
grep -F 'FMT_PARAM_REGISTRY_TESTS_OK=1' build/wp5_8_params/registry.log
grep -F 'assigned=37' build/wp5_8_params/registry.log

cc -std=c99 -O2 -Wall -Wextra -Werror \
    -Ifirmware/src \
    tests/test_fmt_fm_params.c \
    firmware/src/fmt_fm_params.c \
    -o build/host/test_fmt_fm_params

build/host/test_fmt_fm_params | tee build/wp5_8_params/fm_params.log
grep -F 'WP5_8_FM_PARAMS_OK=1 ids=27 pages=7 param_set=27 default_patch=exact' build/wp5_8_params/fm_params.log

DEFAULT_SHA=$(sha256sum build/host/wp5_8_default_patch.bin | awk '{print $1}')
test "$DEFAULT_SHA" = '3d2ef849d345d12f79312af92950062db0dbfd5b56966c6f7a2d557251edac52'
printf '%s\n' "WP5_8_DEFAULT_PATCH_SHA_OK=1 sha256=$DEFAULT_SHA"

# D055 production sources are frozen by D056. Re-run the behavioral engine host
# without WP5.7's historical source-diff assertion, which correctly rejects new
# WP5.8 production files and therefore is not a valid post-activation gate.
cc -std=c11 -Wall -Wextra -Werror -Ifirmware/src \
  tests/wp5_7_engine_host.c \
  firmware/src/fmt_fm_operator.c \
  firmware/src/fmt_fm_algorithm.c \
  firmware/src/fmt_fm_ratio.c \
  firmware/src/fmt_fm_controls.c \
  firmware/src/fmt_fm_feedback.c \
  firmware/src/fmt_fm_envelope.c \
  firmware/src/fmt_fm_engine.c \
  firmware/src/fmt_engine_pitch.c \
  firmware/src/fmt_voice_allocator.c \
  -o build/host/wp5_8_d055_engine_host
build/host/wp5_8_d055_engine_host | tee build/wp5_8_params/d055_engine.log
grep -F 'WP5_7_D050_SEAM_EQUIVALENT_OK=1 algorithms=4 frames_each=4096 op4_advance=once' build/wp5_8_params/d055_engine.log
grep -F 'WP5_7_FM_ENGINE_OWNERSHIP_OK=1 slots=8 active_cross_track_steals=0 release_tail=reused malformed=safe_silence' build/wp5_8_params/d055_engine.log
grep -F 'WP5_7_FM_ENGINE_EVENT_OK=1 note=hard_retrigger retrig=hard_reset off_cut=detached_release' build/wp5_8_params/d055_engine.log
grep -F 'WP5_7_FM_ENGINE_HOST_OK=1 d050=compatible engine=788 voice=92 slots=8' build/wp5_8_params/d055_engine.log
printf '%s\n' 'WP5_8_D055_BEHAVIOR_OK=1 engine=788 voice=92 slots=8 source=frozen'

# Default production startup remains all-8BIT. Registry activation and the new
# standalone FM parameter model must not alter one byte of the accepted stream.
sh tests/run_host.sh > build/wp5_8_params/all8bit_host.log
ALL8BIT_SHA=$(sha256sum build/host/fm1tracker.wav | awk '{print $1}')
test "$ALL8BIT_SHA" = "$EXPECTED_WAV"
printf '%s\n' "WP5_8_ALL8BIT_REGRESSION_OK=1 sha256=$ALL8BIT_SHA legacy_cache=unchanged"

printf '%s\n' 'WP5_8_PARAM_MODEL_GATE_OK=1 decision=D056 ids=27 pages=7 registry=37 defaults=d055_exact legacy8bit=unchanged hardware_margin_claim=none' | tee build/wp5_8_params/summary.txt
