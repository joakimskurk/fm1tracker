#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

BASE=c93c39ad344760b4da0dfbc76829ca5fef886174
OUT=build/wp5_0_premigration
mkdir -p "$OUT"

# WP5.0 production migration is not allowed until this evidence is frozen.
git diff --exit-code "$BASE" -- \
    firmware/src firmware/hal firmware/loader firmware/app.ld build.sh tools
printf 'WP5_0_PREMIGRATION_PRODUCTION_FREEZE_OK=1 baseline=%s\n' "$BASE"

# Pull the actual D046/D045 implementation from the frozen production commit.
# The probe compiles the exact old row-context function rather than a hand-copied
# approximation, then changes only FmtSongMeta.mode.
git show "$BASE:firmware/src/fmt_8bit_linked_params.c" > "$OUT/fmt_8bit_linked_params_baseline.c"
git show "$BASE:firmware/src/fmt_8bit_params.c" > "$OUT/fmt_8bit_params_baseline.c"

python3 - "$OUT/fmt_8bit_linked_params_baseline.c" "$OUT/wp5_0_gate_probe.c" <<'PY'
from pathlib import Path
import sys

src = Path(sys.argv[1]).read_text()
start_token = 'static uint8_t fmt_8bit_linked_row_context('
end_token = '\n}\n\nstatic uint32_t fmt_8bit_linked_resolve_row('
if src.count(start_token) != 1 or src.count(end_token) != 1:
    raise SystemExit('D048 pre-migration extraction shape changed')
start = src.index(start_token)
end = src.index(end_token, start) + 3
fn = src[start:end]

probe = r'''/* Generated from the frozen D046 production source by run_wp5_0_premigration.sh. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_model.h"
#include "fmt_playback.h"
#include "fmt_8bit_params.h"

static FmtSong wp5_song;

const FmtStep *fmt_store_step_const(uint32_t pattern,
                                    uint32_t track,
                                    uint32_t step)
{
    return fmt_song_step_const(&wp5_song, pattern, track, step);
}

''' + fn + r'''

static void prepare_valid_8bit_song(void)
{
    FmtStep *step;

    memset(&wp5_song, 0, sizeof(wp5_song));
    wp5_song.meta.mode = FMT_MODE_8BIT;
    wp5_song.tracks[0].instrument = 0u;
    wp5_song.instruments[0].mode = FMT_MODE_8BIT;
    wp5_song.instruments[0].engine = FMT_8BIT_ENGINE_PULSE;
    step = fmt_song_step(&wp5_song, FMT_PLAYBACK_PATTERN, 0u, 0u);
    fmt_step_init(step);
}

int main(void)
{
    uint8_t instrument = 0x55u;
    uint8_t engine = 0x55u;
    uint8_t valid_8bit;
    uint8_t blocked_by_song_mode;
    uint8_t blocked_by_instrument_mode;

    prepare_valid_8bit_song();
    valid_8bit = fmt_8bit_linked_row_context(&wp5_song,
                                             0u,
                                             0u,
                                             &instrument,
                                             &engine);
    if (valid_8bit != 1u || instrument != 0u ||
        engine != FMT_8BIT_ENGINE_PULSE) {
        fprintf(stderr, "baseline valid 8-BIT context did not resolve\n");
        return 2;
    }

    /* Change exactly one byte: song metadata mode. Instrument remains valid 8-BIT. */
    wp5_song.meta.mode = FMT_MODE_16BIT;
    instrument = 0x55u;
    engine = 0x55u;
    blocked_by_song_mode = (uint8_t)!fmt_8bit_linked_row_context(&wp5_song,
                                                                 0u,
                                                                 0u,
                                                                 &instrument,
                                                                 &engine);
    if (!blocked_by_song_mode || instrument != FMT_INSTRUMENT_INHERIT ||
        engine != FMT_8BIT_ENGINE_SAFE ||
        wp5_song.instruments[0].mode != FMT_MODE_8BIT ||
        wp5_song.instruments[0].engine != FMT_8BIT_ENGINE_PULSE) {
        fprintf(stderr, "global song-mode gate was not reproduced exactly\n");
        return 3;
    }

    /* Restore song metadata and prove the separate per-instrument family gate exists. */
    wp5_song.meta.mode = FMT_MODE_8BIT;
    wp5_song.instruments[0].mode = FMT_MODE_16BIT;
    blocked_by_instrument_mode = (uint8_t)!fmt_8bit_linked_row_context(&wp5_song,
                                                                       0u,
                                                                       0u,
                                                                       &instrument,
                                                                       &engine);
    if (!blocked_by_instrument_mode) {
        fprintf(stderr, "per-instrument family gate unexpectedly absent\n");
        return 4;
    }

    printf("WP5_0_PREMIGRATION_GATE_REPRODUCED=1 valid_8bit=1 song_mode_only_change=blocked instrument_gate=present\n");
    return 0;
}
'''
Path(sys.argv[2]).write_text(probe)
PY

cc -O2 -Wall -Wextra -Werror \
    -Ifirmware/src \
    "$OUT/wp5_0_gate_probe.c" \
    "$OUT/fmt_8bit_params_baseline.c" \
    -o "$OUT/wp5_0_gate_probe"
"$OUT/wp5_0_gate_probe" | tee "$OUT/gate.log"
grep -F 'WP5_0_PREMIGRATION_GATE_REPRODUCED=1 valid_8bit=1 song_mode_only_change=blocked instrument_gate=present' "$OUT/gate.log"

# Freeze D048 router semantics independently of production implementation.
# This model deliberately contains no FM DSP and cannot affect firmware state.
cc -O2 -Wall -Wextra -Werror \
    -Ifirmware/src \
    tests/wp5_0_router_contract.c \
    -o "$OUT/wp5_0_router_contract"
"$OUT/wp5_0_router_contract" | tee "$OUT/router_model.log"
grep -F 'WP5_0_PREMIGRATION_ROUTER_MODEL_SIZE_OK=1 bytes=8' "$OUT/router_model.log"
grep -F 'WP5_0_PREMIGRATION_IDENTITY_MODEL_OK=1 instruments=16 bytes=64' "$OUT/router_model.log"
grep -F 'WP5_0_PREMIGRATION_SHARED_CACHE_MODEL_OK=1 rows=512 bytes=2048 copies=1' "$OUT/router_model.log"
grep -F 'WP5_0_PREMIGRATION_MIXED_ROUTE_MODEL_OK=1 tracks=8 active_cross_track_steals=0' "$OUT/router_model.log"
grep -F 'WP5_0_PREMIGRATION_SWITCH_ORDER_MODEL_OK=1 old_silence=before_new_note commit_on_success=1 failure_route=none' "$OUT/router_model.log"
grep -F 'WP5_0_PREMIGRATION_NONNOTE_MODEL_OK=1 tie_alloc=0 retrig_alloc=0 off_clears_route=1' "$OUT/router_model.log"

# Record the physical D044 starting shape that D048 will generalize rather than
# duplicate. One definition, one exact 2,048-byte type assertion.
CACHE_DEFS="$(grep -R -h -F 'volatile Fmt8BitRowParamCache fmt_8bit_row_param_cache' firmware/src \
    | wc -l | tr -d ' ')"
CACHE_ASSERTS="$(grep -R -h -F 'sizeof(fmt_8bit_row_param_cache) == 2048u' firmware/src \
    | wc -l | tr -d ' ')"
test "$CACHE_DEFS" -eq 1
test "$CACHE_ASSERTS" -eq 1
printf 'WP5_0_PREMIGRATION_SHARED_CACHE_SOURCE_OK=1 definitions=%s size_asserts=%s bytes=2048\n' \
    "$CACHE_DEFS" "$CACHE_ASSERTS" | tee "$OUT/cache_shape.log"

# Freeze the source evidence that the obsolete song-level gate is present in the
# D046 baseline in addition to the behavioral probe above.
SONG_GATE_COUNT="$(grep -F -c 'song->meta.mode != FMT_MODE_8BIT' "$OUT/fmt_8bit_linked_params_baseline.c")"
test "$SONG_GATE_COUNT" -eq 1
printf 'WP5_0_PREMIGRATION_SOURCE_GATE_OK=1 baseline_song_mode_checks=%s\n' "$SONG_GATE_COUNT" \
    | tee "$OUT/source_gate.log"

printf 'WP5_0_PREMIGRATION_OK=1 production=frozen gate=reproduced router_model=frozen shared_cache=2048 fm_dsp=absent\n' \
    | tee "$OUT/summary.txt"
