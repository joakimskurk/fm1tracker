#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/wp5_8_params

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

# The first production slice must not regress the accepted integrated D055 path.
sh tests/run_wp5_7_integration_tests.sh > build/wp5_8_params/d055.log
cat build/wp5_8_params/d055.log
grep -F 'WP5_7_HOST_GATE_OK=1 d055=integrated engine=788 slots=8 d043=exact d048=integrated d054=exact all8bit=wp5_0_identical mixed=deterministic hardware_margin_claim=none' build/wp5_8_params/d055.log

printf '%s\n' 'WP5_8_PARAM_MODEL_GATE_OK=1 decision=D056 ids=27 pages=7 registry=37 defaults=d055_exact legacy8bit=unchanged hardware_margin_claim=none' | tee build/wp5_8_params/summary.txt
