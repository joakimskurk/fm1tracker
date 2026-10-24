#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_0_router
mkdir -p "$OUT"

cc -O2 -Wall -Wextra -Werror \
    -Ifirmware/src \
    tests/wp5_0_router_host.c \
    firmware/src/fmt_engine_router.c \
    -o "$OUT/wp5_0_router_host"

"$OUT/wp5_0_router_host" | tee "$OUT/router_host.log"
grep -F 'WP5_0_ROUTER_SIZE_OK=1 bytes=8' "$OUT/router_host.log"
grep -F 'WP5_0_IDENTITY_PUBLICATION_OK=1 instruments=16 bytes=64' "$OUT/router_host.log"
grep -F 'WP5_0_ROUTER_CORE_OK=1 tracks=8 families=2 unknown_family=safe' "$OUT/router_host.log"
grep -F 'WP5_0_SONG_MODE_NONGATING_PUBLICATION_OK=1 mode0_mode1_identity_words=identical' "$OUT/router_host.log"

# The new common routing core must stay tiny and must not allocate or pull in
# family-specific DSP.
if grep -E '\b(malloc|calloc|realloc|free)\s*\(' firmware/src/fmt_engine_router.c; then
    echo 'WP5.0 router core unexpectedly uses heap allocation' >&2
    exit 2
fi
if grep -E '#include ".*(8bit|osc|envelope|voice_allocator).*"' firmware/src/fmt_engine_router.c firmware/src/fmt_engine_router.h; then
    echo 'WP5.0 common router unexpectedly depends on family DSP' >&2
    exit 3
fi
printf '%s\n' 'WP5_0_ROUTER_CORE_BOUND_OK=1 heap=0 family_dsp_dependencies=0 fixed_tracks=8 fixed_instruments=16'
