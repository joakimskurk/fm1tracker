#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Apply the frozen D048 mixed-engine routing migration to dinghy.c.

This is an exact, assertion-heavy transform used first by the transient WP5.0
integration gate. It deliberately adds no FM DSP. The transform is kept in the
repo as migration evidence until WP5.0 closes.
"""
from pathlib import Path

PATH = Path("firmware/src/dinghy.c")
src = PATH.read_text()

if '#include "fmt_engine_router.c"' in src:
    raise SystemExit("D048 dinghy integration already applied")

include_anchor = '#include "fmt_8bit_linked_params.c"            /* WP4.9 Gate 3: linked base/cache/editor */\n'
if src.count(include_anchor) != 1:
    raise SystemExit("unexpected D045 include anchor shape")
src = src.replace(
    include_anchor,
    include_anchor +
    '#include "fmt_engine_router.c"                 /* WP5.0 D048: per-instrument family router */\n',
    1,
)

global_anchor = 'Fmt8BitEngine fmt_8bit_engine_runtime;\nFmtRtQueue fmt_rt_queue_runtime;\n'
if src.count(global_anchor) != 1:
    raise SystemExit("unexpected D045 runtime-global anchor shape")
src = src.replace(
    global_anchor,
    'Fmt8BitEngine fmt_8bit_engine_runtime;\n'
    'FmtEngineRouter fmt_engine_router_runtime;\n'
    'FmtInstrumentIdentityPublication fmt_instrument_identity_publication\n'
    '    __attribute__((aligned(4)));\n'
    'FmtRtQueue fmt_rt_queue_runtime;\n',
    1,
)

start_token = 'static int tracker_instrument_info('
end_token = '\nstatic void audition_release_all(void)'
if src.count(start_token) != 1 or src.count(end_token) != 1:
    raise SystemExit("unexpected D045 routing block shape")
start = src.index(start_token)
end = src.index(end_token, start)

routing = r'''static int tracker_8bit_instrument_info(uint8_t instrument,
                                        uint8_t *engine_id,
                                        const Fmt8BitParamSet **base)
{
    uint8_t family = FMT_ENGINE_FAMILY_NONE;
    uint8_t engine = 0xFFu;
    const Fmt8BitParamSet *params;

    if (!fmt_instrument_identity_read(&fmt_instrument_identity_publication,
                                      instrument,
                                      &family,
                                      &engine,
                                      (uint8_t *)0,
                                      (uint8_t *)0) ||
        family != FMT_ENGINE_FAMILY_8BIT ||
        !fmt_8bit_engine_supported(engine))
        return 0;
    params = fmt_8bit_linked_params_base(instrument);
    if (!params)
        return 0;
    if (engine_id)
        *engine_id = engine;
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

static void tracker_hard_silence_family(uint32_t track, uint8_t family)
{
    if (track >= FMT_TRACKS)
        return;

    switch (family) {
    case FMT_ENGINE_FAMILY_8BIT:
        (void)fmt_8bit_engine_silence_track(&fmt_8bit_engine_runtime,
                                            (uint8_t)track);
        break;
    case FMT_ENGINE_FAMILY_FM:
        /* D048 reserves the FM family identity, but WP5.0 contains no FM DSP. */
        break;
    default:
        break;
    }
}

static void tracker_hard_silence_route(uint32_t track)
{
    uint8_t family;

    if (track >= FMT_TRACKS)
        return;
    family = fmt_engine_router_family(&fmt_engine_router_runtime, (uint8_t)track);
    tracker_hard_silence_family(track, family);
    fmt_engine_router_clear(&fmt_engine_router_runtime, (uint8_t)track);
}

static uint8_t tracker_route_active(uint32_t track)
{
    uint8_t family;

    if (track >= FMT_TRACKS)
        return 0u;
    family = fmt_engine_router_family(&fmt_engine_router_runtime, (uint8_t)track);
    switch (family) {
    case FMT_ENGINE_FAMILY_8BIT:
        return fmt_8bit_engine_track_voice(&fmt_8bit_engine_runtime,
                                           (uint8_t)track) != FMT_VOICE_NONE;
    case FMT_ENGINE_FAMILY_FM:
        return 0u;
    default:
        return 0u;
    }
}

static uint32_t tracker_live_page_word(uint32_t track)
{
    uint8_t family, voice_index;

    if (track >= FMT_TRACKS)
        return 0u;
    family = fmt_engine_router_family(&fmt_engine_router_runtime, (uint8_t)track);
    if (family != FMT_ENGINE_FAMILY_8BIT)
        return tracker_page_row_word[track];
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
    uint8_t family;

    if (track >= FMT_TRACKS)
        return;
    family = fmt_engine_router_family(&fmt_engine_router_runtime, (uint8_t)track);
    if (family != FMT_ENGINE_FAMILY_8BIT ||
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

static uint8_t tracker_note_route(uint32_t track,
                                  uint8_t instrument,
                                  uint32_t page_word)
{
    uint8_t requested_family = FMT_ENGINE_FAMILY_NONE;
    uint8_t engine_id = 0xFFu;
    uint8_t current_family;
    uint8_t voice_index;
    const Fmt8BitParamSet *base;

    if (track >= FMT_TRACKS || instrument >= FMT_INSTRUMENTS) {
        tracker_hard_silence_route(track);
        return 0u;
    }

    if (!fmt_instrument_identity_read(&fmt_instrument_identity_publication,
                                      instrument,
                                      &requested_family,
                                      &engine_id,
                                      (uint8_t *)0,
                                      (uint8_t *)0)) {
        tracker_hard_silence_route(track);
        return 0u;
    }

    current_family = fmt_engine_router_family(&fmt_engine_router_runtime,
                                               (uint8_t)track);
    if (current_family != FMT_ENGINE_FAMILY_NONE &&
        current_family != requested_family) {
        tracker_hard_silence_family(track, current_family);
        fmt_engine_router_clear(&fmt_engine_router_runtime, (uint8_t)track);
        current_family = FMT_ENGINE_FAMILY_NONE;
    }

    switch (requested_family) {
    case FMT_ENGINE_FAMILY_8BIT:
        if (!fmt_8bit_engine_supported(engine_id)) {
            tracker_hard_silence_route(track);
            return 0u;
        }
        base = fmt_8bit_linked_params_base(instrument);
        if (!base) {
            tracker_hard_silence_route(track);
            return 0u;
        }
        voice_index = fmt_8bit_engine_note(&fmt_8bit_engine_runtime,
                                           (uint8_t)track,
                                           engine_id,
                                           instrument,
                                           tracker_row_pitch_q7(track),
                                           tracker_row_gain_q15(track),
                                           page_word,
                                           base);
        if (voice_index == FMT_VOICE_NONE) {
            fmt_engine_router_clear(&fmt_engine_router_runtime, (uint8_t)track);
            return 0u;
        }
        if (!fmt_engine_router_commit(&fmt_engine_router_runtime,
                                      (uint8_t)track,
                                      FMT_ENGINE_FAMILY_8BIT)) {
            (void)fmt_8bit_engine_silence_track(&fmt_8bit_engine_runtime,
                                                (uint8_t)track);
            fmt_engine_router_clear(&fmt_engine_router_runtime, (uint8_t)track);
            return 0u;
        }
        return 1u;

    case FMT_ENGINE_FAMILY_FM:
        /* Identity is valid but the family adapter is intentionally absent in
         * WP5.0. Unsupported production dispatch always fails toward silence. */
        if (current_family == FMT_ENGINE_FAMILY_FM)
            tracker_hard_silence_family(track, current_family);
        fmt_engine_router_clear(&fmt_engine_router_runtime, (uint8_t)track);
        return 0u;

    default:
        tracker_hard_silence_route(track);
        return 0u;
    }
}

static void tracker_release_route(uint32_t track, uint32_t page_word)
{
    uint8_t family;

    if (track >= FMT_TRACKS)
        return;
    family = fmt_engine_router_family(&fmt_engine_router_runtime, (uint8_t)track);
    switch (family) {
    case FMT_ENGINE_FAMILY_8BIT:
        (void)fmt_8bit_engine_release(&fmt_8bit_engine_runtime,
                                      (uint8_t)track,
                                      tracker_row_gain_q15(track),
                                      page_word);
        break;
    case FMT_ENGINE_FAMILY_FM:
        break;
    default:
        break;
    }
    fmt_engine_router_clear(&fmt_engine_router_runtime, (uint8_t)track);
}

static void tracker_apply_synthetic_off(uint32_t track)
{
    FmtTrackerEffectState *effect = tracker_effect_state(track);

    if (track >= FMT_TRACKS || !effect)
        return;
    tracker_release_route(track, tracker_page_row_word[track]);
    fmt_tracker_effect_apply_cut(effect);
}

static void tracker_apply_row_action(uint32_t track, uint8_t action, uint8_t note)
{
    FmtTrackerEffectState *effect;
    uint8_t instrument, voice_index;
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
        (void)tracker_note_route(track,
                                 instrument,
                                 tracker_page_row_word[track]);
        break;

    case FMT_TRACKER_ROW_ACTION_CUT:
        tracker_apply_synthetic_off(track);
        break;

    case FMT_TRACKER_ROW_ACTION_RETRIG:
        if (fmt_engine_router_family(&fmt_engine_router_runtime,
                                     (uint8_t)track) !=
            FMT_ENGINE_FAMILY_8BIT)
            break;
        voice_index = fmt_8bit_engine_track_voice(&fmt_8bit_engine_runtime,
                                                  (uint8_t)track);
        if (voice_index != FMT_VOICE_NONE) {
            instrument = fmt_8bit_engine_runtime.voice[voice_index].instrument;
            if (tracker_8bit_instrument_info(instrument,
                                             (uint8_t *)0,
                                             &base))
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
    uint8_t instrument;

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

    /* D048 preserves D045 ordering. The currently owned family receives the
     * live row update before immediate NOTE/TIE/OFF semantics. */
    tracker_apply_effect_pitch(track);

    if (immediate_kind == FMT_PLAYBACK_EVENT_NOTE) {
        (void)tracker_note_route(track, instrument, page_word);
    } else if (immediate_kind == FMT_PLAYBACK_EVENT_OFF) {
        tracker_release_route(track, page_word);
    }

    /* Tick 0 belongs to row dispatch. Only NOTE CUT may act here. */
    action = fmt_tracker_row_event_action(row_event,
                                          0u,
                                          tracker_route_active(track),
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
                                          tracker_route_active(track),
                                          &action_note);
    tracker_apply_row_action(track, action, action_note);
    tracker_apply_effect_pitch(track);
}
'''

src = src[:start] + routing + src[end:]

reset_anchor = '    fmt_8bit_linked_params_rebuild();\n    fmt_8bit_engine_reset(&fmt_8bit_engine_runtime);\n'
if src.count(reset_anchor) != 1:
    raise SystemExit("unexpected D045 engine-reset anchor shape")
src = src.replace(
    reset_anchor,
    '    fmt_8bit_linked_params_rebuild();\n'
    '    fmt_instrument_identity_publication_reset(\n'
    '        &fmt_instrument_identity_publication);\n'
    '    fmt_instrument_identity_publish_song(\n'
    '        &fmt_instrument_identity_publication, fmt_store_song_const());\n'
    '    fmt_8bit_engine_reset(&fmt_8bit_engine_runtime);\n'
    '    fmt_engine_router_reset(&fmt_engine_router_runtime);\n',
    1,
)

# Guard the core D048 invariants directly in the generated source.
required = [
    '#include "fmt_engine_router.c"',
    'FmtEngineRouter fmt_engine_router_runtime;',
    'FmtInstrumentIdentityPublication fmt_instrument_identity_publication',
    'fmt_instrument_identity_read(&fmt_instrument_identity_publication',
    'tracker_hard_silence_family(track, current_family);',
    'fmt_engine_router_clear(&fmt_engine_router_runtime, (uint8_t)track);',
    'fmt_engine_router_commit(&fmt_engine_router_runtime,',
    'fmt_instrument_identity_publish_song(',
]
for token in required:
    if token not in src:
        raise SystemExit(f"missing generated D048 token: {token}")

# The obsolete audio eligibility read must be gone from dinghy.c. The startup
# assignment to song->meta.mode remains intentionally present as legacy/default metadata.
if 'song->meta.mode != FMT_MODE_8BIT' in src:
    raise SystemExit("obsolete project-wide playback gate survived D048 transform")

PATH.write_text(src)
print("WP5_0_DINGHY_TRANSFORM_OK=1 fm_dsp=absent song_mode_audio_gate=removed")
