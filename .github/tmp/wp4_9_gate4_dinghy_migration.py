from pathlib import Path

p = Path('firmware/src/dinghy.c')
s = p.read_text()

# Link the already verified Phase 4 primitives into the single-TU FM-1 app.
anchor = '#include "fmt_tracker_screen.c"                /* WP2.9: bounded read-only tracker presentation */\n'
addition = '''#include "fmt_engine_pitch.c"                  /* WP4.2: shared Q7 pitch-to-phase helper */\n#include "fmt_osc_pulse.c"                     /* WP4.2: D038 PULSE */\n#include "fmt_osc_noise.c"                     /* WP4.3: D039 NOISE */\n#include "fmt_osc_triangle.c"                  /* WP4.4: D040 TRIANGLE */\n#include "fmt_osc_wavetable8.c"                /* WP4.5: D041 WAVETABLE-8 */\n#include "fmt_envelope.c"                      /* WP4.6: D042 common envelope */\n#include "fmt_voice_allocator.c"               /* WP4.7: D043 deterministic allocator */\n#include "fmt_8bit_params.c"                   /* WP4.8: D044 engine pages/resolver */\n#include "fmt_8bit_engine.c"                   /* WP4.9 Gate 2: fixed linked engine */\n#include "fmt_8bit_linked_params.c"            /* WP4.9 Gate 3: linked base/cache/editor */\n'''
assert anchor in s
s = s.replace(anchor, anchor + addition, 1)

# Preserve the externally verified track-0 runtime symbols and add fixed state
# only for tracks 1..7 plus the single shared D045 engine.
old = '''FmtTrackerEffectState fmt_tracker_effect_runtime;\nFmtTrackerRowEventState fmt_tracker_row_event_runtime;\nFmtRtQueue fmt_rt_queue_runtime;\n'''
new = '''FmtTrackerEffectState fmt_tracker_effect_runtime;\nFmtTrackerEffectState fmt_tracker_effect_runtime_extra[FMT_TRACKS - 1u];\nFmtTrackerRowEventState fmt_tracker_row_event_runtime;\nFmtTrackerRowEventState fmt_tracker_row_event_runtime_extra[FMT_TRACKS - 1u];\nFmt8BitEngine fmt_8bit_engine_runtime;\nFmtRtQueue fmt_rt_queue_runtime;\n'''
assert old in s
s = s.replace(old, new, 1)

# Replace only the tracker proof-sine lane. The D034 audition voice model,
# note_inc() and sine() remain available and behaviorally unchanged.
start = s.index('#define TRACKER_TARGET_AMP 6000')
end = s.index('static void audition_release_all(void)', start)
new_block = r'''static struct {
    volatile uint32_t inc;                      /* phase step per frame: f * 2^32 / 44100 */
    volatile int32_t gate;                      /* target level, Q15: 0 = released */
    volatile int32_t amp;                       /* audio-owned; main reads only for UI/quiet */
    uint32_t ph, age;
    volatile uint8_t note;                      /* audio-owned; main reads only for LEDs */
} voice[NV];
static uint32_t audition_ages;

/* D045 Gate 4 fixed tracker snapshots. Audio owns these together with the
 * linked engine/effect/event state. Main owns only the aligned publication
 * caches and instrument base bank. 32+32+8+8 = 80 B ordinary RAM. */
static uint32_t tracker_common_row_word[FMT_TRACKS];
static uint32_t tracker_page_row_word[FMT_TRACKS];
static uint8_t tracker_current_instrument[FMT_TRACKS];
static uint8_t tracker_note_instrument[FMT_TRACKS];

static const uint32_t INC_C9[12] = {            /* MIDI 120..131 (C9..B9); lower octaves shift right */
    815363807u, 863847862u, 915214929u, 969636441u, 1027294024u, 1088380105u,
    1153098554u, 1221665363u, 1294309365u, 1371273005u, 1452813141u, 1539201906u};

static uint32_t note_inc(uint32_t note)
{
    return INC_C9[note % 12u] >> (10u - note / 12u);
}

static int32_t sine(uint32_t ph)                /* a full cycle over 2^32 -> -32767..32767 (a refined parabola, ~0.1 %) */
{
    int32_t x = (int32_t)ph >> 16, ax = x < 0 ? -x : x, y, ay;
    y = (x * (32768 - ax)) >> 13;               /* 4x(1-|x|), Q15 */
    ay = y < 0 ? -y : y;
    y += ((((y * ay) >> 15) - y) * 7373) >> 15; /* + 0.225 (y|y| - y) */
    return y > 32767 ? 32767 : y < -32767 ? -32767 : y;
}

static FmtTrackerEffectState *tracker_effect_state(uint32_t track)
{
    if (track >= FMT_TRACKS)
        return (FmtTrackerEffectState *)0;
    if (track == 0u)
        return &fmt_tracker_effect_runtime;
    return &fmt_tracker_effect_runtime_extra[track - 1u];
}

static FmtTrackerRowEventState *tracker_row_event_state(uint32_t track)
{
    if (track >= FMT_TRACKS)
        return (FmtTrackerRowEventState *)0;
    if (track == 0u)
        return &fmt_tracker_row_event_runtime;
    return &fmt_tracker_row_event_runtime_extra[track - 1u];
}

static uint16_t tracker_row_gain_q15(uint32_t track)
{
    uint32_t word;

    if (track >= FMT_TRACKS)
        return 0u;
    word = tracker_common_row_word[track];
    return fmt_playback_level_target(32767u,
                                     fmt_playback_params_voice(word),
                                     fmt_playback_params_track(word));
}

static uint16_t tracker_row_pitch_q7(uint32_t track)
{
    FmtTrackerEffectState *state;
    uint8_t fine;

    if (track >= FMT_TRACKS)
        return 0u;
    state = tracker_effect_state(track);
    if (!state || !state->pitch_valid)
        return 0u;
    fine = fmt_playback_params_fine(tracker_common_row_word[track]);
    return fmt_tracker_effect_final_pitch_q7(state, fine);
}

static int tracker_instrument_info(uint8_t instrument,
                                   uint8_t *engine_id,
                                   const Fmt8BitParamSet **base)
{
    const FmtSong *song = fmt_store_song_const();
    const FmtInstrumentHeader *header;
    const Fmt8BitParamSet *params;

    if (!song || song->meta.mode != FMT_MODE_8BIT || instrument >= FMT_INSTRUMENTS)
        return 0;
    header = &song->instruments[instrument];
    if (header->mode != FMT_MODE_8BIT || !fmt_8bit_engine_supported(header->engine))
        return 0;
    params = fmt_8bit_linked_params_base(instrument);
    if (!params)
        return 0;
    if (engine_id)
        *engine_id = header->engine;
    if (base)
        *base = params;
    return 1;
}

static uint8_t tracker_capture_row_instrument(uint32_t track,
                                              const FmtTrackPlaybackEvent *event)
{
    uint8_t instrument;

    if (track >= FMT_TRACKS || !event)
        return FMT_INSTRUMENT_INHERIT;
    instrument = tracker_current_instrument[track];
    if (event->kind == FMT_PLAYBACK_EVENT_NOTE) {
        if (event->instrument < FMT_INSTRUMENTS) {
            instrument = event->instrument;
            tracker_current_instrument[track] = instrument;
        } else if (event->instrument != FMT_INSTRUMENT_INHERIT) {
            return FMT_INSTRUMENT_INHERIT;
        }
    }
    return instrument;
}

static uint32_t tracker_live_page_word(uint32_t track)
{
    uint8_t voice_index;

    if (track >= FMT_TRACKS)
        return 0u;
    voice_index = fmt_8bit_engine_track_voice(&fmt_8bit_engine_runtime,
                                              (uint8_t)track);
    if (voice_index != FMT_VOICE_NONE &&
        fmt_8bit_engine_runtime.voice[voice_index].instrument !=
            tracker_current_instrument[track])
        return fmt_8bit_engine_runtime.voice[voice_index].page_word;
    return tracker_page_row_word[track];
}

static void tracker_apply_effect_pitch(uint32_t track)
{
    FmtTrackerEffectState *state;

    if (track >= FMT_TRACKS ||
        fmt_8bit_engine_track_voice(&fmt_8bit_engine_runtime, (uint8_t)track) ==
            FMT_VOICE_NONE)
        return;
    state = tracker_effect_state(track);
    if (!state || !state->pitch_valid)
        return;
    (void)fmt_8bit_engine_update_row(&fmt_8bit_engine_runtime,
                                     (uint8_t)track,
                                     tracker_row_pitch_q7(track),
                                     tracker_row_gain_q15(track),
                                     tracker_live_page_word(track));
}

static void tracker_apply_synthetic_off(uint32_t track)
{
    FmtTrackerEffectState *effect = tracker_effect_state(track);

    if (track >= FMT_TRACKS || !effect)
        return;
    (void)fmt_8bit_engine_release(&fmt_8bit_engine_runtime,
                                  (uint8_t)track,
                                  tracker_row_gain_q15(track),
                                  tracker_page_row_word[track]);
    fmt_tracker_effect_apply_cut(effect);
}

static void tracker_apply_row_action(uint32_t track, uint8_t action, uint8_t note)
{
    FmtTrackerEffectState *effect;
    uint8_t instrument, engine_id, voice_index;
    const Fmt8BitParamSet *base;

    if (track >= FMT_TRACKS)
        return;
    effect = tracker_effect_state(track);
    if (!effect)
        return;

    switch (action) {
    case FMT_TRACKER_ROW_ACTION_DELAYED_NOTE:
        fmt_tracker_effect_apply_delayed_note(effect, note);
        instrument = tracker_note_instrument[track];
        if (tracker_instrument_info(instrument, &engine_id, &base)) {
            (void)fmt_8bit_engine_note(&fmt_8bit_engine_runtime,
                                       (uint8_t)track,
                                       engine_id,
                                       instrument,
                                       tracker_row_pitch_q7(track),
                                       tracker_row_gain_q15(track),
                                       tracker_page_row_word[track],
                                       base);
        } else {
            (void)fmt_8bit_engine_silence_track(&fmt_8bit_engine_runtime,
                                                (uint8_t)track);
        }
        break;

    case FMT_TRACKER_ROW_ACTION_CUT:
        tracker_apply_synthetic_off(track);
        break;

    case FMT_TRACKER_ROW_ACTION_RETRIG:
        voice_index = fmt_8bit_engine_track_voice(&fmt_8bit_engine_runtime,
                                                  (uint8_t)track);
        if (voice_index != FMT_VOICE_NONE) {
            instrument = fmt_8bit_engine_runtime.voice[voice_index].instrument;
            if (tracker_instrument_info(instrument, &engine_id, &base))
                (void)fmt_8bit_engine_retrigger(&fmt_8bit_engine_runtime,
                                                 (uint8_t)track,
                                                 tracker_row_pitch_q7(track),
                                                 tracker_row_gain_q15(track),
                                                 tracker_page_row_word[track],
                                                 base);
        }
        break;

    default:
        break;
    }
}

static void tracker_apply_event(uint32_t track,
                                const FmtTrackPlaybackEvent *event)
{
    FmtTrackerEffectState *effect;
    FmtTrackerRowEventState *row_event;
    uint32_t common_word, effect_word, page_word;
    uint8_t immediate_kind, immediate_note, action_note, action;
    uint8_t instrument, engine_id;
    const Fmt8BitParamSet *base;

    if (track >= FMT_TRACKS || !event)
        return;
    effect = tracker_effect_state(track);
    row_event = tracker_row_event_state(track);
    if (!effect || !row_event)
        return;

    common_word = fmt_playback_params_read_track(track, event->step);
    effect_word = fmt_tracker_effect_cache_read_track(track, event->step);
    page_word = fmt_8bit_linked_params_read(track, event->step);
    tracker_common_row_word[track] = common_word;
    tracker_page_row_word[track] = page_word;
    instrument = tracker_capture_row_instrument(track, event);
    tracker_note_instrument[track] = instrument;

    fmt_tracker_row_event_begin(row_event,
                                event->kind,
                                event->note,
                                effect_word,
                                &immediate_kind,
                                &immediate_note);
    fmt_tracker_effect_begin_row(effect,
                                 immediate_kind,
                                 immediate_note,
                                 effect_word);

    /* D045 ordering: live row gain/pitch/timbre reaches an already-owned voice
     * before immediate NOTE/TIE/OFF semantics. A delayed instrument change does
     * not reinterpret the old oscillator's page while it waits for its tick. */
    tracker_apply_effect_pitch(track);

    if (immediate_kind == FMT_PLAYBACK_EVENT_NOTE) {
        if (tracker_instrument_info(instrument, &engine_id, &base)) {
            (void)fmt_8bit_engine_note(&fmt_8bit_engine_runtime,
                                       (uint8_t)track,
                                       engine_id,
                                       instrument,
                                       tracker_row_pitch_q7(track),
                                       tracker_row_gain_q15(track),
                                       page_word,
                                       base);
        } else {
            (void)fmt_8bit_engine_silence_track(&fmt_8bit_engine_runtime,
                                                (uint8_t)track);
        }
    } else if (immediate_kind == FMT_PLAYBACK_EVENT_OFF) {
        (void)fmt_8bit_engine_release(&fmt_8bit_engine_runtime,
                                      (uint8_t)track,
                                      tracker_row_gain_q15(track),
                                      page_word);
    }

    /* Tick 0 belongs to row dispatch. Only NOTE CUT may act here. */
    action = fmt_tracker_row_event_action(row_event,
                                          0u,
                                          fmt_8bit_engine_track_voice(
                                              &fmt_8bit_engine_runtime,
                                              (uint8_t)track) != FMT_VOICE_NONE,
                                          &action_note);
    tracker_apply_row_action(track, action, action_note);
    tracker_apply_effect_pitch(track);
}

static void tracker_apply_tick(uint32_t track)
{
    FmtTrackerEffectState *effect = tracker_effect_state(track);
    FmtTrackerRowEventState *row_event = tracker_row_event_state(track);
    uint8_t action_note, action;

    if (!effect || !row_event)
        return;
    (void)fmt_tracker_effect_advance_tick(effect);
    action = fmt_tracker_row_event_action(row_event,
                                          effect->tick,
                                          fmt_8bit_engine_track_voice(
                                              &fmt_8bit_engine_runtime,
                                              (uint8_t)track) != FMT_VOICE_NONE,
                                          &action_note);
    tracker_apply_row_action(track, action, action_note);
    tracker_apply_effect_pitch(track);
}

'''
s = s[:start] + new_block + s[end:]

# Replace the proof-lane callback with the D045 eight-track engine while keeping
# the manual/audition render loop byte-for-byte in policy.
start = s.index('static void app_audio(int32_t *out, uint32_t n)')
end = s.index('/* ---- notes (main loop) */', start)
new_audio = r'''static void app_audio(int32_t *out, uint32_t n)
{
    uint32_t track, v, i;
    FmtTrackPlaybackEvent event[FMT_TRACKS];

    /* D034 remains a separate bounded main->audio audition ingress. */
    audition_consume_queue();

    if (tracker_ready) {
        if (!tracker_play_requested) {
            if (fmt_transport_runtime.running)
                fmt_transport_stop(&fmt_transport_runtime);
            tracker_transport_stopped = 1u;
        } else {
            tracker_transport_stopped = 0u;
            if (!fmt_playback_runtime.started) {
                if (fmt_playback_start_tracks(&fmt_playback_runtime,
                                              &fmt_transport_runtime,
                                              event)) {
                    for (track = 0u; track < FMT_TRACKS; track++)
                        tracker_apply_event(track, &event[track]);
                }
            } else if (!fmt_transport_runtime.running) {
                fmt_transport_start(&fmt_transport_runtime);
            }
        }
    }

    /* D045 preserves D020/D021 frame ordering: render current engine state,
     * advance transport, then apply a crossed row to the next frame. A D032
     * tick is processed only when no row boundary crossed on that frame. */
    if (fmt_playback_runtime.started) {
        for (i = 0u; i < n; i++) {
            int32_t sample = fmt_8bit_engine_render(&fmt_8bit_engine_runtime);
            out[2u * i] += sample;
            out[2u * i + 1u] += sample;

            if (fmt_playback_end_frame_tracks(&fmt_playback_runtime,
                                              &fmt_transport_runtime,
                                              event)) {
                for (track = 0u; track < FMT_TRACKS; track++)
                    tracker_apply_event(track, &event[track]);
            } else if (fmt_transport_runtime.running) {
                for (track = 0u; track < FMT_TRACKS; track++) {
                    FmtTrackerEffectState *effect = tracker_effect_state(track);
                    if (effect && fmt_tracker_effect_tick_due(
                                      effect, fmt_transport_runtime.phase))
                        tracker_apply_tick(track);
                }
            }
        }
    }

    /* D034 manual/audition rendering remains the original four sine voices. */
    for (v = 0; v < NV; v++) {
        uint32_t ph = voice[v].ph, step = voice[v].inc;
        int32_t amp = voice[v].amp, g = voice[v].gate;
        if (!amp && !g)
            continue;
        for (i = 0; i < n; i++) {
            int32_t sample;
            if (amp < g) { amp += 48; if (amp > g) amp = g; }
            else if (amp > g) { amp -= 6; if (amp < g) amp = g; }
            ph += step;
            sample = (sine(ph) * amp) >> 15;
            out[2u * i] += sample;
            out[2u * i + 1u] += sample;
        }
        voice[v].ph = ph;
        voice[v].amp = amp;
    }
}

'''
s = s[:start] + new_audio + s[end:]

# Quiet now includes all eight linked engine envelopes, including detached tails.
old = '''    /* settings_save() erases flash and waits for silence. A sustained tracker\n     * gate or a release tail therefore still blocks flash just like WP2.4 did. */\n    if (tracker_voice.inc || tracker_voice.amp || tracker_voice.gate)\n        return 0;\n'''
new = '''    /* D045: settings_save() also waits for all eight sequenced envelopes,\n     * including detached D043 release tails, to reach IDLE. */\n    if (!fmt_8bit_engine_quiet(&fmt_8bit_engine_runtime))\n        return 0;\n'''
assert old in s
s = s.replace(old, new, 1)

# D045 startup ordering and deterministic four-engine seed.
start = s.index('    fmt_store_reset();\n', s.index('static void app_main(void)'))
end_marker = '    tracker_ready = 1u;\n'
end = s.index(end_marker, start) + len(end_marker)
startup = r'''    fmt_store_reset();
    fmt_lock_store_reset();
    {
        FmtSong *song = fmt_store_song();
        uint32_t track, instrument;

        if (song) {
            song->meta.mode = FMT_MODE_8BIT;
            for (instrument = 0u; instrument < FMT_INSTRUMENTS; instrument++) {
                song->instruments[instrument].mode = FMT_MODE_8BIT;
                song->instruments[instrument].engine =
                    instrument < 4u ? (uint8_t)(instrument + 1u)
                                    : (uint8_t)FMT_8BIT_ENGINE_SAFE;
                song->instruments[instrument].version = 0u;
                song->instruments[instrument].flags = 0u;
            }
            for (track = 0u; track < FMT_TRACKS; track++)
                song->tracks[track].instrument = (uint8_t)(track & 3u);
        }
    }
    fmt_playback_seed_proof_pattern();
    fmt_8bit_linked_params_reset();
    fmt_playback_params_reset();
    fmt_playback_params_rebuild();
    fmt_tracker_effect_cache_reset();
    fmt_tracker_effect_cache_rebuild();
    fmt_8bit_linked_params_rebuild();
    fmt_8bit_engine_reset(&fmt_8bit_engine_runtime);
    {
        const FmtSong *song = fmt_store_song_const();
        uint32_t track;
        for (track = 0u; track < FMT_TRACKS; track++) {
            FmtTrackerEffectState *effect = tracker_effect_state(track);
            FmtTrackerRowEventState *row_event = tracker_row_event_state(track);
            if (effect)
                fmt_tracker_effect_state_reset(effect);
            if (row_event)
                fmt_tracker_row_event_reset(row_event);
            tracker_common_row_word[track] = fmt_playback_params_default_word();
            tracker_page_row_word[track] = 0u;
            tracker_current_instrument[track] =
                song ? song->tracks[track].instrument : 0u;
            tracker_note_instrument[track] = tracker_current_instrument[track];
        }
    }
    fmt_rt_queue_reset(&fmt_rt_queue_runtime);
    fmt_rt_queue_overflow_seen = 0u;
    audition_ages = 0u;
    fmt_transport_reset(&fmt_transport_runtime);
    fmt_playback_reset(&fmt_playback_runtime);
    fmt_pattern_input_reset(&fmt_pattern_input_runtime);
    fmt_step_record_reset(&fmt_step_record_runtime);
    fmt_tracker_screen_reset(&fmt_tracker_screen_runtime);
    fmt_lock_editor_reset(&fmt_lock_editor_runtime);
    tracker_play_requested = 1u;
    tracker_transport_stopped = 1u;
    tracker_ready = 1u;
'''
s = s[:start] + startup + s[end:]

# Gate 3 editor wrappers become the app's primary p-lock mutation path.
old = '''                if (fmt_lock_editor_confirm_delete(&fmt_lock_editor_runtime)) {\n                    if (fmt_lock_editor_runtime.track == FMT_PLAYBACK_TRACK)\n                        (void)fmt_playback_params_refresh_step(fmt_lock_editor_runtime.step);\n                    delete_activity = 1;\n                    tracker_footer_dirty();\n                }\n'''
new = '''                if (fmt_8bit_linked_editor_confirm_delete(&fmt_lock_editor_runtime)) {\n                    delete_activity = 1;\n                    tracker_footer_dirty();\n                }\n'''
assert old in s
s = s.replace(old, new, 1)

old = '''                if (lock_delta[k] != 0 &&\n                    fmt_lock_editor_apply_delta(&fmt_lock_editor_runtime,\n                                                k,\n                                                lock_delta[k])) {\n                    if (fmt_lock_editor_runtime.track == FMT_PLAYBACK_TRACK)\n                        (void)fmt_playback_params_refresh_step(fmt_lock_editor_runtime.step);\n                    tracker_footer_dirty();\n                }\n'''
new = '''                if (lock_delta[k] != 0 &&\n                    fmt_8bit_linked_editor_apply_delta(&fmt_lock_editor_runtime,\n                                                       k,\n                                                       lock_delta[k])) {\n                    tracker_footer_dirty();\n                }\n'''
assert old in s
s = s.replace(old, new, 1)

# Stopped STEP REC mutates note semantics, so rebuild the selected track's D044
# inheritance publication after each successful note/special write.
old = '''        if (step_rec_mode)\n            (void)fmt_step_record_special_input(&fmt_step_record_runtime,\n                                                &fmt_pattern_input_runtime,\n                                                &fmt_transport_runtime,\n                                                (b & (1u << BTN_FX)) != 0u,\n                                                (rel & (1u << BTN_FX)) != 0u,\n                                                (b & (1u << BTN_SEL)) != 0u);\n'''
new = '''        if (step_rec_mode) {\n            uint8_t write_track = fmt_pattern_input_runtime.selected_track;\n            if (fmt_step_record_special_input(&fmt_step_record_runtime,\n                                              &fmt_pattern_input_runtime,\n                                              &fmt_transport_runtime,\n                                              (b & (1u << BTN_FX)) != 0u,\n                                              (rel & (1u << BTN_FX)) != 0u,\n                                              (b & (1u << BTN_SEL)) != 0u))\n                (void)fmt_8bit_linked_params_rebuild_track(write_track);\n        }\n'''
assert old in s
s = s.replace(old, new, 1)

old = '''        if (step_rec_mode && write_key < FMT_PATTERN_INPUT_KEY_COUNT)\n            (void)fmt_step_record_write_note(&fmt_step_record_runtime,\n                                             &fmt_pattern_input_runtime,\n                                             &fmt_transport_runtime,\n                                             base + write_key);\n'''
new = '''        if (step_rec_mode && write_key < FMT_PATTERN_INPUT_KEY_COUNT) {\n            uint8_t write_track = fmt_pattern_input_runtime.selected_track;\n            if (fmt_step_record_write_note(&fmt_step_record_runtime,\n                                           &fmt_pattern_input_runtime,\n                                           &fmt_transport_runtime,\n                                           base + write_key))\n                (void)fmt_8bit_linked_params_rebuild_track(write_track);\n        }\n'''
assert old in s
s = s.replace(old, new, 1)

p.write_text(s)
