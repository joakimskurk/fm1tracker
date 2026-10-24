from pathlib import Path
p = Path('tests/host.c')
s = p.read_text()

def once(old, new):
    global s
    n = s.count(old)
    assert n == 1, (old[:100], n)
    s = s.replace(old, new, 1)

once('#include "fmt_transport.h"\n', '#include "fmt_transport.h"\n#include "fmt_rt_queue.h"\n')
once('extern Fmt8BitEngine fmt_8bit_engine_runtime;\n',
     'extern Fmt8BitEngine fmt_8bit_engine_runtime;\nextern FmtRtQueue fmt_rt_queue_runtime;\nextern volatile uint32_t fmt_rt_queue_overflow_seen;\n')
once('static int16_t *wav;\nstatic uint32_t wav_n;\n', '''static int16_t *wav;
static uint32_t wav_n;

/* D046 host-only benchmark accounting. This never changes firmware state or
 * production source. Wall-clock speed is deliberately not an acceptance gate. */
static uint32_t host_bench_target_rows;
static uint32_t host_bench_rows;
static uint32_t host_bench_ticks;
static uint32_t host_bench_sample_hash = 2166136261u;
static uint8_t host_bench_no_wav;

static uint32_t host_fnv_byte(uint32_t hash, uint8_t value)
{
    return (hash ^ value) * 16777619u;
}

static uint32_t host_fnv_u16(uint32_t hash, uint16_t value)
{
    hash = host_fnv_byte(hash, (uint8_t)value);
    return host_fnv_byte(hash, (uint8_t)(value >> 8));
}

static uint32_t host_fnv_u32(uint32_t hash, uint32_t value)
{
    hash = host_fnv_u16(hash, (uint16_t)value);
    return host_fnv_u16(hash, (uint16_t)(value >> 16));
}

static FmtTrackerEffectState *host_effect_state(uint32_t track)
{
    if (track >= FMT_TRACKS) return NULL;
    return track == 0u ? &fmt_tracker_effect_runtime
                       : &fmt_tracker_effect_runtime_extra[track - 1u];
}

static FmtTrackerRowEventState *host_row_event_state(uint32_t track)
{
    if (track >= FMT_TRACKS) return NULL;
    return track == 0u ? &fmt_tracker_row_event_runtime
                       : &fmt_tracker_row_event_runtime_extra[track - 1u];
}

static uint32_t host_d046_state_hash(void)
{
    uint32_t hash = 2166136261u;
    uint32_t track, row, slot, ordinal;

    hash = host_fnv_u32(hash, fmt_transport_runtime.step);
    hash = host_fnv_u32(hash, fmt_transport_runtime.phase);
    hash = host_fnv_u32(hash, fmt_transport_runtime.residual);
    hash = host_fnv_u32(hash, fmt_lock_store_used_count());
    hash = host_fnv_u32(hash, fmt_lock_store_free_count());

    for (track = 0u; track < FMT_TRACKS; track++) {
        FmtTrackerEffectState *fx = host_effect_state(track);
        FmtTrackerRowEventState *ev = host_row_event_state(track);
        if (fx) {
            hash = host_fnv_u16(hash, fx->pitch_q7);
            hash = host_fnv_u16(hash, fx->porta_target_q7);
            hash = host_fnv_u16(hash, (uint16_t)fx->transient_q7);
            hash = host_fnv_byte(hash, fx->command);
            hash = host_fnv_byte(hash, fx->value);
            hash = host_fnv_byte(hash, fx->tick);
            hash = host_fnv_byte(hash, fx->vibrato_phase);
            hash = host_fnv_byte(hash, fx->porta_valid);
            hash = host_fnv_byte(hash, fx->pitch_valid);
        }
        if (ev) {
            hash = host_fnv_byte(hash, ev->command);
            hash = host_fnv_byte(hash, ev->value);
            hash = host_fnv_byte(hash, ev->pending_note);
            hash = host_fnv_byte(hash, ev->pending_valid);
        }
        for (row = 0u; row < FMT_STEPS_PER_PATTERN; row++) {
            uint8_t count = 0u;
            hash = host_fnv_u32(hash, fmt_playback_params_read_track(track, row));
            hash = host_fnv_u32(hash, fmt_tracker_effect_cache_read_track(track, row));
            hash = host_fnv_u32(hash, fmt_8bit_linked_params_read(track, row));
            if (fmt_lock_count(FMT_PLAYBACK_PATTERN, track, row, &count) != FMT_LOCK_STATUS_OK)
                exit(3);
            hash = host_fnv_byte(hash, count);
            for (ordinal = 0u; ordinal < count; ordinal++) {
                FmtParamId id = 0u;
                uint16_t value = 0u;
                if (fmt_lock_at(FMT_PLAYBACK_PATTERN, track, row, ordinal, &id, &value) != FMT_LOCK_STATUS_OK)
                    exit(3);
                hash = host_fnv_byte(hash, id);
                hash = host_fnv_u16(hash, value);
            }
        }
    }

    for (slot = 0u; slot < FMT_VOICE_ALLOCATOR_SLOTS; slot++) {
        const Fmt8BitVoice *v = &fmt_8bit_engine_runtime.voice[slot];
        hash = host_fnv_byte(hash, fmt_8bit_engine_runtime.allocator.voice_owner[slot]);
        hash = host_fnv_u32(hash, fmt_8bit_engine_runtime.allocator.age[slot]);
        hash = host_fnv_byte(hash, v->engine);
        hash = host_fnv_byte(hash, v->instrument);
        hash = host_fnv_u16(hash, v->pitch_q7);
        hash = host_fnv_u16(hash, v->gain_q15);
        hash = host_fnv_u32(hash, v->page_word);
        hash = host_fnv_byte(hash, v->envelope.stage);
        hash = host_fnv_u32(hash, v->envelope.level_q16);
        hash = host_fnv_u32(hash, v->osc.noise.phase);
        hash = host_fnv_u32(hash, v->engine == FMT_8BIT_ENGINE_NOISE ? v->osc.noise.lfsr : 0u);
    }
    hash = host_fnv_u32(hash, fmt_rt_queue_depth(&fmt_rt_queue_runtime));
    hash = host_fnv_u32(hash, fmt_rt_queue_overflow_seq(&fmt_rt_queue_runtime));
    hash = host_fnv_u32(hash, fmt_rt_queue_overflow_seen);
    return hash;
}

static void host_d046_summary(void)
{
    printf("D046_HOST_BENCH rows=%u ticks=%u frames=%u sample_fnv=%08x state_fnv=%08x "
           "lock_used=%u lock_free=%u queue_depth=%u overflow=%u seen=%u step=%u phase=%u\\n",
           host_bench_rows, host_bench_ticks, wav_n,
           host_bench_sample_hash, host_d046_state_hash(),
           (unsigned)fmt_lock_store_used_count(),
           (unsigned)fmt_lock_store_free_count(),
           (unsigned)fmt_rt_queue_depth(&fmt_rt_queue_runtime),
           (unsigned)fmt_rt_queue_overflow_seq(&fmt_rt_queue_runtime),
           (unsigned)fmt_rt_queue_overflow_seen,
           (unsigned)fmt_transport_runtime.step,
           (unsigned)fmt_transport_runtime.phase);
}
''')

# Add benchmark-only pool/queue probe without changing historical probe text.
once('''            putchar('\\n');
            for (slot = 0u; slot < FMT_VOICE_ALLOCATOR_SLOTS; slot++) {''', '''            putchar('\\n');
            if (host_bench_target_rows) {
                printf("%6u ms  D046 probe lock_used=%u lock_free=%u queue_depth=%u overflow=%u seen=%u\\n",
                       fm1_ms, (unsigned)fmt_lock_store_used_count(),
                       (unsigned)fmt_lock_store_free_count(),
                       (unsigned)fmt_rt_queue_depth(&fmt_rt_queue_runtime),
                       (unsigned)fmt_rt_queue_overflow_seq(&fmt_rt_queue_runtime),
                       (unsigned)fmt_rt_queue_overflow_seen);
            }
            for (slot = 0u; slot < FMT_VOICE_ALLOCATOR_SLOTS; slot++) {''')

# Replace audio-buffer accumulation with optional no-WAV sink plus deterministic sample hash and row/tick accounting.
once('''        memset(blk, 0, sizeof blk);
        app_audio(blk, HALF_FRAMES);
        wav = realloc(wav, (wav_n + HALF_FRAMES) * 4u);
        for (i = 0; i < 2u * HALF_FRAMES; i++) {
            int32_t v = blk[i];                 /* (USB audio at the MASTER knob all the way up) */
            wav[2u * wav_n + i] = (int16_t)(v > 32767 ? 32767 : v < -32767 ? -32767 : v);
        }
        wav_n += HALF_FRAMES;
''', '''        uint32_t transport_step_before = fmt_transport_runtime.step;
        uint8_t tick_before[FMT_TRACKS];
        uint32_t track;
        for (track = 0u; track < FMT_TRACKS; track++) {
            FmtTrackerEffectState *fx = host_effect_state(track);
            tick_before[track] = fx ? fx->tick : 0u;
        }
        memset(blk, 0, sizeof blk);
        app_audio(blk, HALF_FRAMES);
        if (host_bench_target_rows) {
            if (fmt_transport_runtime.step != transport_step_before) {
                host_bench_rows++;
            } else {
                for (track = 0u; track < FMT_TRACKS; track++) {
                    FmtTrackerEffectState *fx = host_effect_state(track);
                    if (fx && fx->tick != tick_before[track]) host_bench_ticks++;
                }
            }
        }
        if (!host_bench_no_wav) {
            int16_t *next = realloc(wav, (wav_n + HALF_FRAMES) * 4u);
            if (!next) { fprintf(stderr, "host wav realloc failed\\n"); exit(1); }
            wav = next;
        }
        for (i = 0; i < 2u * HALF_FRAMES; i++) {
            int32_t v = blk[i];                 /* (USB audio at the MASTER knob all the way up) */
            int16_t q = (int16_t)(v > 32767 ? 32767 : v < -32767 ? -32767 : v);
            if (host_bench_target_rows) {
                host_bench_sample_hash = host_fnv_byte(host_bench_sample_hash, (uint8_t)q);
                host_bench_sample_hash = host_fnv_byte(host_bench_sample_hash, (uint8_t)((uint16_t)q >> 8));
            }
            if (!host_bench_no_wav) wav[2u * wav_n + i] = q;
        }
        wav_n += HALF_FRAMES;
        if (host_bench_target_rows && host_bench_rows >= host_bench_target_rows) {
            host_d046_summary();
            if (!host_bench_no_wav) write_wav();
            else printf("sound  -> suppressed by D046 no-WAV mode (%u frames)\\n", wav_n);
            exit(0);
        }
''')

once('''    if (fm1_ms >= end_ms) {
        write_wav();
        exit(0);
    }
''', '''    if (fm1_ms >= end_ms) {
        if (host_bench_target_rows) host_d046_summary();
        if (!host_bench_no_wav) write_wav();
        else printf("sound  -> suppressed by D046 no-WAV mode (%u frames)\\n", wav_n);
        exit(0);
    }
''')

once('''int main(int argc, char **argv)
{
    if (argc > 1) host_out = argv[1];
    if (argc > 2) read_script(argv[2]);
    if (argc > 3) frame_fps = (uint32_t)atoi(argv[3]);
    app_main();
''', '''int main(int argc, char **argv)
{
    const char *bench_rows = getenv("FM1TRACKER_HOST_BENCH_ROWS");
    const char *no_wav = getenv("FM1TRACKER_HOST_NO_WAV");
    if (bench_rows) host_bench_target_rows = (uint32_t)strtoul(bench_rows, NULL, 10);
    if (no_wav && *no_wav && strcmp(no_wav, "0")) host_bench_no_wav = 1u;
    if (argc > 1) host_out = argv[1];
    if (argc > 2) read_script(argv[2]);
    if (argc > 3) frame_fps = (uint32_t)atoi(argv[3]);
    app_main();
''')

p.write_text(s)
print('WP4_10_HOST_BENCH_PATCH_OK=1')
