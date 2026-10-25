#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

BASE=81d3464059270130747e26fcc950b7a2e1bb03e1
OUT=build/wp5_10_benchmark
SRC="$OUT/src"
mkdir -p "$OUT" "$SRC"
rm -f "$OUT"/*.log "$OUT"/*.summary "$OUT"/*.wav

# Benchmark implementation is test-only. Production remains the accepted WP5.9
# checkpoint byte-for-byte until a separately frozen optimization contract exists.
git diff --exit-code "$BASE" -- \
    firmware/src firmware/gen firmware/hal firmware/loader \
    firmware/app.ld build.sh tools
printf '%s\n' 'WP5_10_BENCH_SOURCE_FREEZE_OK=1 base=81d3464 production_delta=0'

python3 - <<'PY'
from pathlib import Path

out = Path('build/wp5_10_benchmark/src')
out.mkdir(parents=True, exist_ok=True)
platform = Path('firmware/src/platform.c').read_text()
host = Path('tests/host.c').read_text()

needle = '#include "../../tests/host.c"                  /* on a computer: a fake FM-1 (tests/host.c) */'
assert platform.count(needle) == 1
platform = platform.replace(needle, '#include "host_wp5_10.c"                 /* generated WP5.10 benchmark host */', 1)

forward = 'static void app_audio(int32_t *out, uint32_t n);\n'
assert host.count(forward) == 1
host = host.replace(forward, forward + 'static void wp5_10_host_prepare_callback(void);\n', 1)
call = '        app_audio(blk, HALF_FRAMES);\n'
assert host.count(call) == 1
host = host.replace(call, '        wp5_10_host_prepare_callback();\n' + call, 1)

(out / 'platform.c').write_text(platform)
(out / 'host_wp5_10.c').write_text(host)
PY

python3 - <<'PY'
from pathlib import Path

out = Path('build/wp5_10_benchmark/src')
base = Path('firmware/src/dinghy.c').read_text()

# Freeze the ordinary tracker fixture through normal store/base APIs at startup.
seed = '    fmt_playback_seed_proof_pattern();\n'
assert base.count(seed) == 1
fixture = r'''    fmt_playback_seed_proof_pattern();
    {
        FmtSong *wp5_10_song = fmt_store_song();
        FmtPattern *wp5_10_pattern = fmt_store_pattern(FMT_PLAYBACK_PATTERN);
        uint32_t wp5_10_track, wp5_10_row;
        if (!wp5_10_song || !wp5_10_pattern) {
            fprintf(stderr, "WP5.10 fixture store unavailable\n");
            exit(1);
        }
        for (wp5_10_track = 0u; wp5_10_track < FMT_TRACKS; wp5_10_track++) {
            for (wp5_10_row = 0u; wp5_10_row < FMT_STEPS_PER_PATTERN; wp5_10_row++)
                fmt_step_init(&wp5_10_pattern->steps[wp5_10_track][wp5_10_row]);
            wp5_10_song->tracks[wp5_10_track].instrument = 0u;
            wp5_10_pattern->steps[wp5_10_track][0].note = (uint8_t)(48u + wp5_10_track);
            wp5_10_pattern->steps[wp5_10_track][0].instrument = 0u;
            wp5_10_pattern->steps[wp5_10_track][1].note = FMT_NOTE_OFF;
            wp5_10_pattern->steps[wp5_10_track][2].note = (uint8_t)(60u + wp5_10_track);
            wp5_10_pattern->steps[wp5_10_track][2].instrument = 1u;
        }
        wp5_10_song->instruments[0].mode = FMT_MODE_16BIT;
        wp5_10_song->instruments[0].engine = FMT_FM_ENGINE_FM4;
        wp5_10_song->instruments[0].version = 0u;
        wp5_10_song->instruments[0].flags = 0u;
        wp5_10_song->instruments[1].mode = FMT_MODE_8BIT;
        wp5_10_song->instruments[1].engine = FMT_8BIT_ENGINE_WAVETABLE8;
        wp5_10_song->instruments[1].version = 0u;
        wp5_10_song->instruments[1].flags = 0u;
    }
'''
base = base.replace(seed, fixture, 1)

reset = '    fmt_fm_linked_params_reset();\n'
assert base.count(reset) == 1
bases = r'''    fmt_fm_linked_params_reset();
    {
        FmtFmParamSet wp5_10_fm;
        Fmt8BitParamSet wp5_10_bit;
        uint32_t wp5_10_op;
        fmt_fm_param_set_defaults(&wp5_10_fm);
        wp5_10_fm.algorithm = 64u; /* 64 >> 6 == FMT_FM_ALGO_FAN3 */
        wp5_10_fm.index = 255u;
        wp5_10_fm.feedback = 255u;
        for (wp5_10_op = 0u; wp5_10_op < 4u; wp5_10_op++) {
            wp5_10_fm.ratio[wp5_10_op] = (uint8_t)(16u + wp5_10_op * 16u);
            wp5_10_fm.level[wp5_10_op] = 255u;
            wp5_10_fm.attack[wp5_10_op] = 0u;
            wp5_10_fm.decay[wp5_10_op] = 0u;
            wp5_10_fm.sustain[wp5_10_op] = 255u;
            wp5_10_fm.release[wp5_10_op] = 255u;
        }
        if (!fmt_fm_linked_params_set_base(0u, &wp5_10_fm)) {
            fprintf(stderr, "WP5.10 FM base publish failed\n");
            exit(1);
        }
        fmt_8bit_param_set_defaults(&wp5_10_bit);
        wp5_10_bit.attack = 0u;
        wp5_10_bit.decay = 0u;
        wp5_10_bit.sustain = 255u;
        wp5_10_bit.release = 255u;
        wp5_10_bit.wavetable = 255u;
        if (!fmt_8bit_linked_params_set_base(1u, &wp5_10_bit)) {
            fprintf(stderr, "WP5.10 8-BIT base publish failed\n");
            exit(1);
        }
    }
'''
base = base.replace(reset, bases, 1)

app = 'static void app_audio(int32_t *out, uint32_t n)\n'
assert base.count(app) == 1
instrument = r'''
#ifndef WP5_10_SCENARIO_S2
#define WP5_10_SCENARIO_S2 0
#endif

static uint32_t wp5_10_fnv_byte(uint32_t h, uint8_t v)
{
    return (h ^ v) * 16777619u;
}

static uint32_t wp5_10_fnv_mem(uint32_t h, const void *ptr, uint32_t n)
{
    const uint8_t *p = (const uint8_t *)ptr;
    while (n--) h = wp5_10_fnv_byte(h, *p++);
    return h;
}

static uint8_t wp5_10_bit_non_idle(void)
{
    uint32_t i;
    uint8_t n = 0u;
    for (i = 0u; i < FMT_VOICE_ALLOCATOR_SLOTS; i++)
        if (fmt_8bit_engine_runtime.voice[i].envelope.stage != FMT_ENVELOPE_IDLE) n++;
    return n;
}

static uint8_t wp5_10_fm_non_idle(void)
{
    uint32_t i, op;
    uint8_t n = 0u;
    for (i = 0u; i < FMT_FM_ENGINE_SLOTS; i++) {
        uint8_t active = 0u;
        for (op = 0u; op < FMT_FM_ENVELOPE_OPERATOR_COUNT; op++)
            if (fmt_fm_engine_runtime.voice[i].envelope_state.stage[op] != FMT_FM_ENVELOPE_STAGE_OFF)
                active = 1u;
        if (active) n++;
    }
    return n;
}

static uint8_t wp5_10_bit_owned(void)
{
    uint32_t track;
    uint8_t n = 0u;
    for (track = 0u; track < FMT_TRACKS; track++)
        if (fmt_8bit_engine_track_voice(&fmt_8bit_engine_runtime, (uint8_t)track) != FMT_VOICE_NONE) n++;
    return n;
}

static uint8_t wp5_10_bit_routes(void)
{
    uint32_t track;
    uint8_t n = 0u;
    for (track = 0u; track < FMT_TRACKS; track++)
        if (fmt_engine_router_family(&fmt_engine_router_runtime, (uint8_t)track) == FMT_ENGINE_FAMILY_8BIT) n++;
    return n;
}

static uint8_t wp5_10_exact_8p8(void)
{
    return wp5_10_fm_non_idle() == 8u &&
           wp5_10_bit_non_idle() == 8u &&
           wp5_10_bit_owned() == 8u &&
           wp5_10_bit_routes() == 8u;
}

static uint8_t wp5_10_s1_started;
static uint8_t wp5_10_s1_done;
static uint8_t wp5_10_s1_failed;
static uint32_t wp5_10_s1_warmup;
static uint32_t wp5_10_s1_frames;
static uint32_t wp5_10_s1_sample_hash = 2166136261u;
static uint32_t wp5_10_s1_occ_hash = 2166136261u;

static uint8_t wp5_10_s2_primed;
static uint8_t wp5_10_s2_active;
static uint8_t wp5_10_s2_done;
static uint8_t wp5_10_s2_failed;
static uint32_t wp5_10_s2_pre_depth;
static uint32_t wp5_10_s2_post_depth;
static uint32_t wp5_10_s2_frames;
static uint32_t wp5_10_s2_sine_calls;
static uint32_t wp5_10_s2_sample_hash = 2166136261u;

static void wp5_10_print_s1(void)
{
    uint32_t tracker_hash = 2166136261u;
    uint32_t engine_hash = 2166136261u;
    tracker_hash = wp5_10_fnv_mem(tracker_hash, &fmt_transport_runtime, sizeof fmt_transport_runtime);
    tracker_hash = wp5_10_fnv_mem(tracker_hash, &fmt_playback_runtime, sizeof fmt_playback_runtime);
    tracker_hash = wp5_10_fnv_mem(tracker_hash, &fmt_engine_router_runtime, sizeof fmt_engine_router_runtime);
    engine_hash = wp5_10_fnv_mem(engine_hash, &fmt_8bit_engine_runtime, sizeof fmt_8bit_engine_runtime);
    engine_hash = wp5_10_fnv_mem(engine_hash, &fmt_fm_engine_runtime, sizeof fmt_fm_engine_runtime);
    printf("WP5_10_S1_RESULT pass=%u warmup=256 measured=%u fm_min=8 bit_min=8 bit_owned=8 routes8=8 sample=%08x occupancy=%08x tracker=%08x engine=%08x hardware_margin_claim=none\n",
           wp5_10_s1_failed ? 0u : 1u, wp5_10_s1_frames,
           wp5_10_s1_sample_hash, wp5_10_s1_occ_hash, tracker_hash, engine_hash);
}

static void wp5_10_observe_tracker_frame(int32_t sample)
{
    uint8_t fm = wp5_10_fm_non_idle();
    uint8_t bit = wp5_10_bit_non_idle();
    uint8_t owned = wp5_10_bit_owned();
    uint8_t routes = wp5_10_bit_routes();

#if WP5_10_SCENARIO_S2
    if (wp5_10_s2_active && !wp5_10_s2_done) {
        if (fm != 8u || bit != 8u || owned != 8u || routes != 8u)
            wp5_10_s2_failed = 1u;
        wp5_10_s2_sample_hash = wp5_10_fnv_byte(wp5_10_s2_sample_hash, (uint8_t)sample);
        wp5_10_s2_sample_hash = wp5_10_fnv_byte(wp5_10_s2_sample_hash, (uint8_t)((uint16_t)sample >> 8));
        wp5_10_s2_frames++;
    }
#else
    if (!wp5_10_s1_started && fm == 8u && bit == 8u && owned == 8u && routes == 8u) {
        wp5_10_s1_started = 1u;
        wp5_10_s1_warmup = 256u;
    }
    if (wp5_10_s1_started && !wp5_10_s1_done) {
        if (fm != 8u || bit != 8u || owned != 8u || routes != 8u) {
            wp5_10_s1_failed = 1u;
            return;
        }
        if (wp5_10_s1_warmup) {
            wp5_10_s1_warmup--;
            return;
        }
        wp5_10_s1_sample_hash = wp5_10_fnv_byte(wp5_10_s1_sample_hash, (uint8_t)sample);
        wp5_10_s1_sample_hash = wp5_10_fnv_byte(wp5_10_s1_sample_hash, (uint8_t)((uint16_t)sample >> 8));
        wp5_10_s1_occ_hash = wp5_10_fnv_byte(wp5_10_s1_occ_hash, fm);
        wp5_10_s1_occ_hash = wp5_10_fnv_byte(wp5_10_s1_occ_hash, bit);
        wp5_10_s1_occ_hash = wp5_10_fnv_byte(wp5_10_s1_occ_hash, owned);
        wp5_10_s1_occ_hash = wp5_10_fnv_byte(wp5_10_s1_occ_hash, routes);
        wp5_10_s1_frames++;
        if (wp5_10_s1_frames == 4096u) {
            wp5_10_s1_done = 1u;
            wp5_10_print_s1();
        }
    }
#endif
}

static void wp5_10_host_prepare_callback(void)
{
#if WP5_10_SCENARIO_S2
    uint32_t i;
    if (wp5_10_s2_primed || !wp5_10_exact_8p8()) return;
    if (fmt_rt_queue_depth(&fmt_rt_queue_runtime) != 0u) return;
    for (i = 0u; i < FMT_RT_QUEUE_CAPACITY; i++) {
        FmtRtQueueStatus st = fmt_rt_queue_enqueue_note_on(
            &fmt_rt_queue_runtime, (uint8_t)(i & 7u), (uint8_t)(48u + i), 127u);
        if (st != FMT_RT_QUEUE_OK) {
            fprintf(stderr, "WP5.10 S2 enqueue failed at %u status %u\n", i, (unsigned)st);
            exit(1);
        }
    }
    wp5_10_s2_primed = 1u;
#endif
}

static void wp5_10_callback_begin(uint32_t n)
{
#if WP5_10_SCENARIO_S2
    if (wp5_10_s2_primed && !wp5_10_s2_done && !wp5_10_s2_active) {
        wp5_10_s2_pre_depth = fmt_rt_queue_depth(&fmt_rt_queue_runtime);
        if (n != 256u || wp5_10_s2_pre_depth != 32u || !wp5_10_exact_8p8())
            wp5_10_s2_failed = 1u;
        wp5_10_s2_active = 1u;
    }
#else
    (void)n;
#endif
}

static void wp5_10_callback_after_queue(void)
{
#if WP5_10_SCENARIO_S2
    if (wp5_10_s2_active && !wp5_10_s2_done) {
        wp5_10_s2_post_depth = fmt_rt_queue_depth(&fmt_rt_queue_runtime);
        if (wp5_10_s2_post_depth != 0u) wp5_10_s2_failed = 1u;
    }
#endif
}

static void wp5_10_callback_end(void)
{
#if WP5_10_SCENARIO_S2
    if (wp5_10_s2_active && !wp5_10_s2_done) {
        uint32_t tracker_hash = 2166136261u;
        uint32_t engine_hash = 2166136261u;
        if (wp5_10_s2_frames != 256u || wp5_10_s2_sine_calls != 1024u ||
            !wp5_10_exact_8p8()) wp5_10_s2_failed = 1u;
        tracker_hash = wp5_10_fnv_mem(tracker_hash, &fmt_transport_runtime, sizeof fmt_transport_runtime);
        tracker_hash = wp5_10_fnv_mem(tracker_hash, &fmt_playback_runtime, sizeof fmt_playback_runtime);
        tracker_hash = wp5_10_fnv_mem(tracker_hash, &fmt_engine_router_runtime, sizeof fmt_engine_router_runtime);
        engine_hash = wp5_10_fnv_mem(engine_hash, &fmt_8bit_engine_runtime, sizeof fmt_8bit_engine_runtime);
        engine_hash = wp5_10_fnv_mem(engine_hash, &fmt_fm_engine_runtime, sizeof fmt_fm_engine_runtime);
        wp5_10_s2_done = 1u;
        printf("WP5_10_S2_RESULT pass=%u pre_depth=%u post_depth=%u measured=%u fm=8 bit=8 bit_owned=8 routes8=8 audition_sines=%u sample=%08x tracker=%08x engine=%08x hardware_margin_claim=none\n",
               wp5_10_s2_failed ? 0u : 1u, wp5_10_s2_pre_depth,
               wp5_10_s2_post_depth, wp5_10_s2_frames,
               wp5_10_s2_sine_calls, wp5_10_s2_sample_hash,
               tracker_hash, engine_hash);
    }
#endif
}

'''
base = base.replace(app, instrument + app, 1)

begin = '    /* D034 remains a separate bounded main->audio audition ingress. */\n    audition_consume_queue();\n'
assert base.count(begin) == 1
base = base.replace(begin,
'''    wp5_10_callback_begin(n);\n    /* D034 remains a separate bounded main->audio audition ingress. */\n    audition_consume_queue();\n    wp5_10_callback_after_queue();\n''', 1)

sample = '            out[2u * i + 1u] += sample;\n\n            if (fmt_playback_end_frame_tracks('
assert base.count(sample) == 1
base = base.replace(sample,
'            out[2u * i + 1u] += sample;\n            wp5_10_observe_tracker_frame(sample);\n\n            if (fmt_playback_end_frame_tracks(', 1)

sine = '            sample = (sine(ph) * amp) >> 15;\n'
assert base.count(sine) == 1
base = base.replace(sine,
'''#if WP5_10_SCENARIO_S2\n            if (wp5_10_s2_active && !wp5_10_s2_done) wp5_10_s2_sine_calls++;\n#endif\n            sample = (sine(ph) * amp) >> 15;\n''', 1)

end = '        voice[v].amp = amp;\n    }\n}\n\n/* ---- notes (main loop) */'
assert base.count(end) == 1
base = base.replace(end,
'''        voice[v].amp = amp;\n    }\n    wp5_10_callback_end();\n}\n\n/* ---- notes (main loop) */''', 1)

(out / 'dinghy_wp5_10.c').write_text(base)
PY

cat > "$OUT/run.script" <<'EOF'
650 end
EOF

cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST \
  -DAPP_NAME='"FM1TRACKER"' -DAPP_VER='"wp5.10-s1"' \
  -I"$SRC" -Ifirmware/src -Ifirmware/gen \
  -o "$OUT/s1" "$SRC/dinghy_wp5_10.c"
cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST -DWP5_10_SCENARIO_S2=1 \
  -DAPP_NAME='"FM1TRACKER"' -DAPP_VER='"wp5.10-s2"' \
  -I"$SRC" -Ifirmware/src -Ifirmware/gen \
  -o "$OUT/s2" "$SRC/dinghy_wp5_10.c"

run_case() {
    case_name="$1"
    rep="$2"
    "$OUT/$case_name" "$OUT/${case_name}_${rep}" "$OUT/run.script" > "$OUT/${case_name}_${rep}.log" 2>&1
    grep -F "WP5_10_$(printf '%s' "$case_name" | tr '[:lower:]' '[:upper:]')_RESULT pass=1" "$OUT/${case_name}_${rep}.log" > "$OUT/${case_name}_${rep}.summary"
}

for rep in 1 2 3; do
    run_case s1 "$rep"
    run_case s2 "$rep"
done

cmp -s "$OUT/s1_1.summary" "$OUT/s1_2.summary"
cmp -s "$OUT/s1_1.summary" "$OUT/s1_3.summary"
cmp -s "$OUT/s2_1.summary" "$OUT/s2_2.summary"
cmp -s "$OUT/s2_1.summary" "$OUT/s2_3.summary"

cat "$OUT/s1_1.summary"
cat "$OUT/s2_1.summary"
printf '%s\n' 'WP5_10_BENCHMARK_OK=1 s1=8fm+8bit warmup=256 measured=4096 repeats=3 s2=queue32+8fm+8bit audition_sines=1024 measured=256 repeats=3 deterministic=1 production_delta=0 hardware_margin_claim=none' > "$OUT/summary.txt"
cat "$OUT/summary.txt"
