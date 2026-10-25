/* SPDX-License-Identifier: GPL-3.0-only */
/* Dinghy: the smallest FM-1 firmware to build your own on. A sine wave on the keys, and that is all.
 * This is the file to change. Everything below it (platform.c) is done: updates, the USB rescue,
 * USB-MIDI, USB audio, keys, knobs, LEDs, the screen, a settings sector.
 *
 *   keys       play sines, 4 voices; the notes go out over USB-MIDI too
 *   OCT- OCT+  the octave (kept in flash)
 *   MASTER     the volume (platform.c)
 *   USB-MIDI   notes in play the sines, with their velocity
 *
 * Rules of the road (the chip has no FPU and no libgcc): integers only, no float, no 64-bit division.
 * tests/host.c runs this file on a computer: the screen as a PNG, the sound as a WAV.
 * What platform.c offers: docs/API.md. The chip's rules: docs/RULES.md. */
#include "platform.c"
#include "fmt_store.c"                         /* WP2.2: one bounds-safe pool-backed editable song */
#include "fmt_param_registry.c"                /* WP3.1: stable one-byte semantic parameter IDs */
#include "fmt_lock_store.c"                    /* WP3.2: bounded global sparse p-lock storage */
#include "fmt_lock_editor.c"                   /* WP3.3/3.4: held-step p-lock edit/browse/delete */
#include "fmt_transport.c"                     /* WP2.3: exact integer 1/16-note transport timing */
#include "fmt_playback.c"                      /* WP2.4/2.8: exact one-lane row/special dispatch */
#include "fmt_playback_params.c"               /* WP3.5: D031 fixed row-local p-lock snapshots */
#include "fmt_tracker_effect.c"                /* WP3.6: D032 bounded tracker-effect snapshots/state */
#include "fmt_tracker_event.c"                 /* WP3.7: D033 fixed-state retrig/delay/cut scheduler */
#include "fmt_rt_queue.c"                      /* WP3.8: D034 bounded main-to-audio event ingress */
#include "fmt_pattern_input.c"                 /* WP2.5/2.6: bounded SEQ step/track cursor addressing */
#include "fmt_step_record.c"                   /* WP2.7/2.8: stopped STEP REC note/special entry */
#include "fmt_tracker_screen.c"                /* WP2.9: bounded read-only tracker presentation */
#include "fmt_engine_pitch.c"                  /* WP4.2: shared Q7 pitch-to-phase helper */
#include "fmt_osc_pulse.c"                     /* WP4.2: D038 PULSE */
#include "fmt_osc_noise.c"                     /* WP4.3: D039 NOISE */
#include "fmt_osc_triangle.c"                  /* WP4.4: D040 TRIANGLE */
#include "fmt_osc_wavetable8.c"                /* WP4.5: D041 WAVETABLE-8 */
#include "fmt_envelope.c"                      /* WP4.6: D042 common envelope */
#include "fmt_voice_allocator.c"               /* WP4.7: D043 deterministic allocator */
#include "fmt_8bit_params.c"                   /* WP4.8: D044 engine pages/resolver */
#include "fmt_8bit_engine.c"                   /* WP4.9 Gate 2: fixed linked engine */
#include "fmt_8bit_linked_params.c"            /* WP4.9 Gate 3: linked base/cache/editor */
#include "fmt_fm_params.c"                     /* WP5.8 D056: normalized FM parameter pages */
#include "fmt_fm_linked_params.c"              /* WP5.8 D056: base + row override publication */
#include "fmt_fm_operator.c"                   /* WP5.1 D049: fixed-point FM operator */
#include "fmt_fm_algorithm.c"                  /* WP5.2/D055: FM topology + external OP4 seam */
#include "fmt_fm_ratio.c"                      /* WP5.3 D051: bounded ratio mapping */
#include "fmt_fm_controls.c"                   /* WP5.4 D052: LEVEL/INDEX mapping */
#include "fmt_fm_feedback.c"                   /* WP5.5 D053: bounded OP4 feedback */
#include "fmt_fm_envelope.c"                   /* WP5.6 D054: four operator ADSRs */
#include "fmt_fm_engine.c"                     /* WP5.7 D055: fixed eight-slot FM family */
#include "fmt_engine_router.c"                 /* WP5.0/D055: per-instrument family router */

/* Externally visible so CI can verify the small runtime control states stay in
 * ordinary RAM and never consume editable-song pool space. */
FmtTransport fmt_transport_runtime;
FmtPlayback fmt_playback_runtime;
FmtPatternInput fmt_pattern_input_runtime;
FmtStepRecord fmt_step_record_runtime;
FmtTrackerScreen fmt_tracker_screen_runtime;
FmtLockEditor fmt_lock_editor_runtime;
FmtTrackerEffectState fmt_tracker_effect_runtime;
FmtTrackerEffectState fmt_tracker_effect_runtime_extra[FMT_TRACKS - 1u];
FmtTrackerRowEventState fmt_tracker_row_event_runtime;
FmtTrackerRowEventState fmt_tracker_row_event_runtime_extra[FMT_TRACKS - 1u];
Fmt8BitEngine fmt_8bit_engine_runtime;
FmtFmEngine fmt_fm_engine_runtime;
FmtEngineRouter fmt_engine_router_runtime;
FmtInstrumentIdentityPublication fmt_instrument_identity_publication
    __attribute__((aligned(4)));
FmtRtQueue fmt_rt_queue_runtime;
volatile uint32_t fmt_rt_queue_overflow_seen;

/* app_main seeds the proof pattern before setting this one-way handoff. Audio
 * IRQs exist before app_main(), so the ISR must not read the song store earlier. */
static volatile uint32_t tracker_ready;

/* D024 transport handoff. Main owns play_requested; audio owns stopped_ack and
 * the actual FmtTransport running field. This keeps the ISR transport one-writer
 * while STEP REC waits for a real stop before mutating the shared song store. */
static volatile uint32_t tracker_play_requested;
static volatile uint32_t tracker_transport_stopped;

/* ---- the sound (runs in the audio ISR: keep it short, no flash, no screen) */
#define NV 4u                                   /* manual/audition voices */
static struct {
    volatile uint32_t inc;                      /* phase step per frame: f * 2^32 / 44100 */
    volatile int32_t gate;                      /* target level, Q15: 0 = released */
    volatile int32_t amp;                       /* audio-owned; main reads only for UI/quiet */
    uint32_t ph, age;
    volatile uint8_t note;                      /* audio-owned; main reads only for LEDs */
} voice[NV];
static uint32_t audition_ages;

/* D045 Gate 4 fixed tracker snapshots. D056 adds exactly one current FM patch
 * snapshot per active track, never one patch per row. The 224 B patch bank plus
 * eight validity bytes stay inside D056's frozen remaining runtime margin. */
static uint32_t tracker_common_row_word[FMT_TRACKS];
static uint32_t tracker_page_row_word[FMT_TRACKS];
static uint8_t tracker_current_instrument[FMT_TRACKS];
static uint8_t tracker_note_instrument[FMT_TRACKS];
static FmtFmPatch tracker_fm_row_patch[FMT_TRACKS];
static uint8_t tracker_fm_row_patch_valid[FMT_TRACKS];

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

static int tracker_8bit_instrument_info(uint8_t instrument,
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

static int tracker_fm_snapshot_row(uint32_t track,
                                   uint32_t row,
                                   uint8_t instrument)
{
    uint8_t family = FMT_ENGINE_FAMILY_NONE;
    uint8_t engine = 0xFFu;
    FmtFmRowOverrides overrides;

    if (track >= FMT_TRACKS || row >= FMT_STEPS_PER_PATTERN ||
        instrument >= FMT_INSTRUMENTS)
        return 0;
    if (!fmt_instrument_identity_read(&fmt_instrument_identity_publication,
                                      instrument,
                                      &family,
                                      &engine,
                                      (uint8_t *)0,
                                      (uint8_t *)0) ||
        family != FMT_ENGINE_FAMILY_FM ||
        !fmt_fm_engine_supported(engine))
        return 0;
    if (!fmt_fm_linked_params_read(track, row, &overrides))
        return 0;
    return fmt_fm_linked_params_compose(instrument,
                                        &overrides,
                                        &tracker_fm_row_patch[track]);
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
        (void)fmt_fm_engine_silence_track(&fmt_fm_engine_runtime,
                                          (uint8_t)track);
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
        return fmt_fm_engine_track_voice(&fmt_fm_engine_runtime,
                                         (uint8_t)track) != FMT_VOICE_NONE;
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
    state = tracker_effect_state(track);
    if (!state || !state->pitch_valid)
        return;

    family = fmt_engine_router_family(&fmt_engine_router_runtime, (uint8_t)track);
    if (family == FMT_ENGINE_FAMILY_8BIT) {
        if (fmt_8bit_engine_track_voice(&fmt_8bit_engine_runtime,
                                        (uint8_t)track) == FMT_VOICE_NONE)
            return;
        (void)fmt_8bit_engine_update_row(&fmt_8bit_engine_runtime,
                                         (uint8_t)track,
                                         tracker_row_pitch_q7(track),
                                         tracker_row_gain_q15(track),
                                         tracker_live_page_word(track));
    } else if (family == FMT_ENGINE_FAMILY_FM) {
        uint8_t voice_index = fmt_fm_engine_track_voice(&fmt_fm_engine_runtime,
                                                        (uint8_t)track);
        if (voice_index == FMT_VOICE_NONE)
            return;
        if (!tracker_fm_row_patch_valid[track]) {
            tracker_hard_silence_route(track);
            return;
        }
        (void)fmt_fm_engine_update_row(
            &fmt_fm_engine_runtime,
            (uint8_t)track,
            fmt_fm_engine_runtime.voice[voice_index].instrument,
            tracker_row_pitch_q7(track),
            tracker_row_gain_q15(track),
            &tracker_fm_row_patch[track]);
    }
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
        if (!fmt_fm_engine_supported(engine_id) ||
            !tracker_fm_row_patch_valid[track]) {
            tracker_hard_silence_route(track);
            return 0u;
        }
        voice_index = fmt_fm_engine_note(&fmt_fm_engine_runtime,
                                         (uint8_t)track,
                                         engine_id,
                                         instrument,
                                         tracker_row_pitch_q7(track),
                                         tracker_row_gain_q15(track),
                                         &tracker_fm_row_patch[track]);
        if (voice_index == FMT_VOICE_NONE) {
            fmt_engine_router_clear(&fmt_engine_router_runtime, (uint8_t)track);
            return 0u;
        }
        if (!fmt_engine_router_commit(&fmt_engine_router_runtime,
                                      (uint8_t)track,
                                      FMT_ENGINE_FAMILY_FM)) {
            (void)fmt_fm_engine_silence_track(&fmt_fm_engine_runtime,
                                              (uint8_t)track);
            fmt_engine_router_clear(&fmt_engine_router_runtime, (uint8_t)track);
            return 0u;
        }
        return 1u;

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
        if (!tracker_fm_row_patch_valid[track]) {
            tracker_hard_silence_family(track, FMT_ENGINE_FAMILY_FM);
            break;
        }
        (void)fmt_fm_engine_release(&fmt_fm_engine_runtime,
                                    (uint8_t)track,
                                    tracker_row_gain_q15(track),
                                    &tracker_fm_row_patch[track]);
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
    uint8_t family, instrument, voice_index;
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
        family = fmt_engine_router_family(&fmt_engine_router_runtime,
                                          (uint8_t)track);
        if (family == FMT_ENGINE_FAMILY_8BIT) {
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
        } else if (family == FMT_ENGINE_FAMILY_FM) {
            voice_index = fmt_fm_engine_track_voice(&fmt_fm_engine_runtime,
                                                    (uint8_t)track);
            if (voice_index != FMT_VOICE_NONE) {
                instrument = fmt_fm_engine_runtime.voice[voice_index].instrument;
                if (!tracker_fm_row_patch_valid[track]) {
                    tracker_hard_silence_route(track);
                    break;
                }
                (void)fmt_fm_engine_retrigger(&fmt_fm_engine_runtime,
                                               (uint8_t)track,
                                               instrument,
                                               tracker_row_pitch_q7(track),
                                               tracker_row_gain_q15(track),
                                               &tracker_fm_row_patch[track]);
            }
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

    /* D056 keeps D048's pre-NOTE update on the previously owned patch, then
     * snapshots the new row exactly once from the published FM override cache.
     * TIE/EMPTY/OFF snapshot first so the live voice receives the new controls
     * without phase/envelope reset. No lock/store traversal occurs here. */
    if (event->kind == FMT_PLAYBACK_EVENT_NOTE)
        tracker_apply_effect_pitch(track);

    tracker_fm_row_patch_valid[track] =
        (uint8_t)tracker_fm_snapshot_row(track, event->step, instrument);

    if (event->kind != FMT_PLAYBACK_EVENT_NOTE)
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

static void audition_release_all(void)
{
    uint32_t v;
    for (v = 0u; v < NV; v++)
        voice[v].gate = 0;
}

static void audition_note_on(uint8_t note, uint8_t vel)
{
    uint32_t v, pick = 0u, best = 0xFFFFFFFFu;

    if (note > 127u || !vel || vel > 127u)
        return;
    for (v = 0u; v < NV; v++) {
        uint32_t score = voice[v].note == note && voice[v].gate ? 0u
                       : voice[v].gate ? 0x80000000u + voice[v].age
                       : (uint32_t)voice[v].amp + 1u;
        if (score < best) {
            best = score;
            pick = v;
        }
    }
    voice[pick].note = note;
    voice[pick].age = ++audition_ages;
    voice[pick].inc = note_inc(note);
    voice[pick].gate = (int32_t)((uint32_t)vel * 90u);
}

static void audition_note_off(uint8_t note)
{
    uint32_t v;
    for (v = 0u; v < NV; v++)
        if (voice[v].note == note)
            voice[v].gate = 0;
}

static void audition_consume_queue(void)
{
    uint32_t i, overflow;
    FmtRtEvent event;

    overflow = fmt_rt_queue_overflow_seq(&fmt_rt_queue_runtime);
    if (overflow != fmt_rt_queue_overflow_seen) {
        audition_release_all();
        fmt_rt_queue_consumer_discard_all(&fmt_rt_queue_runtime);
        fmt_rt_queue_overflow_seen = overflow;
        return;
    }

    for (i = 0u; i < FMT_RT_QUEUE_CAPACITY; i++) {
        uint8_t note, vel;

        if (!fmt_rt_queue_dequeue(&fmt_rt_queue_runtime, &event))
            break;
        if (event.track >= FMT_TRACKS)
            continue;
        note = fmt_rt_event_note(&event);
        if (note > FMT_NOTE_MAX)
            continue;
        if (event.type == FMT_RT_EVENT_NOTE_ON) {
            vel = fmt_rt_event_velocity(&event);
            if (vel != 0u && vel <= 127u)
                audition_note_on(note, vel);
        } else if (event.type == FMT_RT_EVENT_NOTE_OFF) {
            audition_note_off(note);
        }
    }
}

static void app_audio(int32_t *out, uint32_t n)
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
            sample += fmt_fm_engine_render(&fmt_fm_engine_runtime);
            if (sample > 32767)
                sample = 32767;
            else if (sample < -32767)
                sample = -32767;
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

/* ---- notes (main loop) */
static uint32_t last_note = 255u;

static void play(uint32_t note, uint32_t vel)
{
    FmtRtQueueStatus status;
    uint8_t track = fmt_pattern_input_runtime.selected_track;

    if (note > 127u || !vel || vel > 127u)
        return;
    status = fmt_rt_queue_enqueue_note_on(&fmt_rt_queue_runtime,
                                          track,
                                          (uint8_t)note,
                                          (uint8_t)vel);
    if (status == FMT_RT_QUEUE_OK)
        last_note = note;
}
static void stop(uint32_t note)
{
    if (note > 127u)
        return;
    (void)fmt_rt_queue_enqueue_note_off(&fmt_rt_queue_runtime,
                                        fmt_pattern_input_runtime.selected_track,
                                        (uint8_t)note);
}
static int quiet(void)
{
    uint32_t v;

    /* D034 includes pending ingress and unresolved overflow recovery in the
     * silence contract so flash work cannot overtake a queued NOTE_ON/OFF. */
    if (fmt_rt_queue_depth(&fmt_rt_queue_runtime) != 0u ||
        fmt_rt_queue_overflow_seq(&fmt_rt_queue_runtime) != fmt_rt_queue_overflow_seen)
        return 0;

    /* D045: settings_save() also waits for all eight sequenced envelopes,
     * including detached D043 release tails, to reach IDLE. */
    if (!fmt_8bit_engine_quiet(&fmt_8bit_engine_runtime) ||
        !fmt_fm_engine_quiet(&fmt_fm_engine_runtime))
        return 0;

    for (v = 0; v < NV; v++)
        if (voice[v].gate || voice[v].amp) return 0;
    return 1;
}

/* ---- screens ---- */
#define C_ACC RGB(180, 140, 255)
#define C_KEY RGB(210, 210, 210)
#define C_OFF RGB(90, 90, 90)
#define KEY0 53u
#define TRACKER_HEADER_H 64u
#define TRACKER_ROW_H 18u
#define TRACKER_FOOTER_Y 208u
#define TRACKER_FOOTER_H 32u
static int black(uint32_t note) { return (0x54Au >> (note % 12u)) & 1u; }

static void draw_note(void)
{
    static const char *const NAME[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    char s[8] = "-";
    if (last_note < 128u) {
        const char *p = NAME[last_note % 12u];
        uint32_t i = 0;
        while (*p) s[i++] = *p++;
        if (last_note >= 12u) s[i++] = (char)('0' + last_note / 12u - 1u);
        s[i] = 0;
    }
    draw_text_box(0, 82, 240, &FONT_L, s, C_WHITE, 1);
}

static void draw_keys(uint32_t lit)
{
    uint32_t k, w = 0;
    cv_begin(240, 56, C_BLACK);
    for (k = 0; k < 27u; k++)
        if (!black(KEY0 + k)) {
            cv_rect(8 + (int32_t)w * 14, 0, 13, 56, lit >> k & 1u ? C_ACC : C_KEY);
            w++;
        }
    for (k = 0, w = 0; k < 27u; k++)
        if (black(KEY0 + k))
            cv_rect(8 + (int32_t)w * 14 - 5, 0, 9, 34, lit >> k & 1u ? C_ACC : C_BLACK);
        else
            w++;
    cv_blit(0, 140);
}

static void draw_status(int32_t oct)
{
    char s[12] = "OCTAVE +0";
    s[7] = oct < 0 ? '-' : '+';
    s[8] = (char)('0' + (oct < 0 ? -oct : oct));
    draw_text_box(0, 212, 240, &FONT_S, s, C_OFF, 1);
}

static void draw_home_static(void)
{
    lcd_fill(0, 0, 240, 240, C_BLACK);
    draw_text_box(0, 18, 240, &FONT_L, APP_NAME, C_ACC, 1);
    draw_text_box(0, 52, 240, &FONT_S, "a sine to build on  v" APP_VER, C_OFF, 1);
}

static void draw_tracker_header(void)
{
    char line1[32], line2[32], line3[32];

    fmt_tracker_screen_header_line1(&fmt_tracker_screen_runtime, line1);
    fmt_tracker_screen_header_line2(&fmt_tracker_screen_runtime, line2);
    fmt_tracker_screen_header_line3(line3);

    cv_begin(240, TRACKER_HEADER_H, C_BLACK);
    cv_text(5, 1, &FONT_S, line1,
            fmt_tracker_screen_runtime.step_rec ? C_WHITE : C_ACC);
    cv_text(5, 21, &FONT_S, line2, C_KEY);
    cv_text(25, 41, &FONT_S, line3, C_OFF);
    cv_rect(0, 62, 240, 1, C_OFF);
    cv_blit(0, 0);
}

static void draw_tracker_row(uint8_t row)
{
    char markers[3], text[32], one[2];
    uint16_t text_color;

    if (row >= FMT_TRACKER_SCREEN_ROWS)
        return;

    fmt_tracker_screen_markers(&fmt_tracker_screen_runtime, row, markers);
    fmt_tracker_screen_row_text(&fmt_tracker_screen_runtime, row, text);
    text_color = markers[0] == '>' ? C_WHITE : C_KEY;

    cv_begin(240, TRACKER_ROW_H, C_BLACK);
    one[1] = 0;
    one[0] = markers[0];
    cv_text(2, 1, &FONT_S, one, C_WHITE);
    one[0] = markers[1];
    cv_text(12, 1, &FONT_S, one, C_ACC);
    cv_text(25, 1, &FONT_S, text, text_color);
    cv_blit(0, TRACKER_HEADER_H + (uint32_t)row * TRACKER_ROW_H);
}

static void tracker_footer_dirty(void)
{
    fmt_tracker_screen_runtime.dirty |=
        (uint16_t)(1u << FMT_TRACKER_SCREEN_REGION_FOOTER);
}

static void draw_tracker_footer(void)
{
    char text[32];
    int lock_active = fmt_lock_editor_is_active(&fmt_lock_editor_runtime);

    if (lock_active)
        fmt_lock_editor_footer(&fmt_lock_editor_runtime, text);
    else
        fmt_tracker_screen_footer(&fmt_tracker_screen_runtime, text);
    cv_begin(240, TRACKER_FOOTER_H, C_BLACK);
    cv_rect(0, 0, 240, 1, C_OFF);
    cv_text(5, 8, &FONT_S, text,
            (fmt_tracker_screen_runtime.step_rec || lock_active) ? C_WHITE : C_OFF);
    cv_blit(0, TRACKER_FOOTER_Y);
}

static void draw_tracker_region(uint8_t region)
{
    uint8_t row;

    if (region == FMT_TRACKER_SCREEN_REGION_HEADER)
        draw_tracker_header();
    else if (region == FMT_TRACKER_SCREEN_REGION_FOOTER)
        draw_tracker_footer();
    else if (fmt_tracker_screen_region_to_row(region, &row))
        draw_tracker_row(row);
}

static void app_main(void)
{
    struct { int8_t oct; } set = {0};
    uint32_t prev = 0, k, t_change = 0, dirty = 0, lit_drawn = ~0u, note_drawn = ~0u;
    int32_t oct_drawn = 99;
    uint8_t held[27] = {0};

    fmt_store_reset();
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
    fmt_fm_linked_params_reset();
    fmt_playback_params_reset();
    fmt_playback_params_rebuild();
    fmt_tracker_effect_cache_reset();
    fmt_tracker_effect_cache_rebuild();
    fmt_8bit_linked_params_rebuild();
    fmt_fm_linked_params_rebuild();
    fmt_instrument_identity_publication_reset(
        &fmt_instrument_identity_publication);
    fmt_instrument_identity_publish_song(
        &fmt_instrument_identity_publication, fmt_store_song_const());
    fmt_8bit_engine_reset(&fmt_8bit_engine_runtime);
    fmt_fm_engine_reset(&fmt_fm_engine_runtime);
    fmt_engine_router_reset(&fmt_engine_router_runtime);
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
            tracker_fm_row_patch[track] = *fmt_fm_engine_default_patch();
            tracker_fm_row_patch_valid[track] = 0u;
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

    settings_load(&set, sizeof set);
    if (set.oct < -2 || set.oct > 2) set.oct = 0;

    draw_home_static();

    for (;;) {
        uint32_t keys, down, up, rel, b, lit = 0, base, write_key;
        uint8_t st, d1, d2, lowest_key;
        int32_t lock_select_delta, lock_delta[FMT_LOCK_EDITOR_KNOBS];
        int pattern_mode, step_rec_mode, lock_began = 0, lock_ended = 0;
        int delete_was_armed, delete_activity = 0;
        platform_poll();

        /* D029/D030: always drain every encoder stream owned by the held-step
         * editor. Only a stable eligible poll may consume these deltas later. */
        lock_select_delta = fm1_enc_take(ENC_SELECT);
        for (k = 0u; k < FMT_LOCK_EDITOR_KNOBS; k++)
            lock_delta[k] = fm1_enc_take(ENC_KNOB1 + k);

        b = fm1_input_edges(&rel);
        delete_was_armed = fmt_lock_editor_runtime.delete_armed != 0u;

        /* D030: PLAY is allowed during a hold, but any pending destructive arm
         * is cancelled before the transport edge is handled. The poll started
         * armed, so all already-drained editor deltas remain discarded. */
        if (delete_was_armed && (b & (1u << BTN_PLAY))) {
            if (fmt_lock_editor_cancel_delete(&fmt_lock_editor_runtime)) {
                delete_activity = 1;
                tracker_footer_dirty();
            }
        }

        if (b & (1u << BTN_HOME)) {
            if (fmt_lock_editor_is_active(&fmt_lock_editor_runtime)) {
                fmt_lock_editor_cancel(&fmt_lock_editor_runtime);
                lock_ended = 1;
            }
            fmt_step_record_exit(&fmt_step_record_runtime);
            fmt_pattern_input_enter_performance(&fmt_pattern_input_runtime);
        } else {
            if (b & (1u << BTN_SEQ))
                fmt_pattern_input_enter_pattern(&fmt_pattern_input_runtime);

            if (b & (1u << BTN_PLAY)) {
                if (fmt_step_record_is_active(&fmt_step_record_runtime)) {
                    fmt_step_record_exit(&fmt_step_record_runtime);
                    tracker_play_requested = 1u;
                } else {
                    tracker_play_requested = tracker_play_requested ? 0u : 1u;
                }
            }

            if (b & (1u << BTN_REC)) {
                if (fmt_step_record_is_active(&fmt_step_record_runtime)) {
                    fmt_step_record_exit(&fmt_step_record_runtime);
                } else if (!tracker_play_requested && tracker_transport_stopped) {
                    if (fmt_step_record_enter(&fmt_step_record_runtime,
                                              &fmt_pattern_input_runtime,
                                              &fmt_transport_runtime)) {
                        if (fmt_lock_editor_is_active(&fmt_lock_editor_runtime)) {
                            fmt_lock_editor_cancel(&fmt_lock_editor_runtime);
                            lock_ended = 1;
                        }
                    }
                }
            }
        }

        pattern_mode = fmt_pattern_input_is_pattern(&fmt_pattern_input_runtime);
        step_rec_mode = fmt_step_record_is_active(&fmt_step_record_runtime);

        base = (uint32_t)(KEY0 + 12 * set.oct);
        keys = fm1_in.notes;
        down = keys & ~prev;
        up = prev & ~keys;
        prev = keys;

        /* D029 release/context precedence. Ending a gesture discards this poll's
         * already-drained SELECT + KNOB deltas and all D030 destructive state. */
        if (fmt_lock_editor_is_active(&fmt_lock_editor_runtime) &&
            (!pattern_mode || step_rec_mode ||
             !fmt_lock_editor_key_held(&fmt_lock_editor_runtime, keys))) {
            fmt_lock_editor_cancel(&fmt_lock_editor_runtime);
            lock_ended = 1;
            if (pattern_mode)
                tracker_footer_dirty();
        }

        /* Arm from the lowest newly pressed white key before ordinary black-key
         * selection. This freezes the selected track as it existed at poll start. */
        if (!lock_ended && pattern_mode && !step_rec_mode &&
            !fmt_lock_editor_is_active(&fmt_lock_editor_runtime)) {
            uint8_t local;
            for (k = 0u; k < FMT_PATTERN_INPUT_KEY_COUNT; k++) {
                if ((down & (1u << k)) &&
                    fmt_pattern_input_key_to_local(k, &local)) {
                    (void)local;
                    if (fmt_lock_editor_begin(&fmt_lock_editor_runtime,
                                              &fmt_pattern_input_runtime,
                                              k)) {
                        lock_began = 1;
                        tracker_footer_dirty();
                    }
                    break;
                }
            }
        }

        /* D030 cancellation precedence for a pending delete arm. EDIT release,
         * OCT intent or any additional note-key down cancels before SEL can
         * confirm. D029 still suppresses those focus/navigation actions. */
        if (fmt_lock_editor_is_active(&fmt_lock_editor_runtime) &&
            fmt_lock_editor_runtime.delete_armed) {
            uint32_t owner_mask = fmt_lock_editor_runtime.held_key < FMT_PATTERN_INPUT_KEY_COUNT
                                ? 1u << fmt_lock_editor_runtime.held_key : 0u;
            int cancel_delete = 0;

            if (rel & (1u << BTN_EDIT))
                cancel_delete = 1;
            if (b & ((1u << BTN_OCT_DOWN) | (1u << BTN_OCT_UP)))
                cancel_delete = 1;
            if (down & ~owner_mask)
                cancel_delete = 1;

            if (cancel_delete && fmt_lock_editor_cancel_delete(&fmt_lock_editor_runtime)) {
                delete_activity = 1;
                tracker_footer_dirty();
            }
        }

        /* D030 destructive grammar. A confirm is accepted only when the poll
         * began armed and cancellation above did not invalidate that arm.
         * Therefore EDIT+SEL in the same poll can arm but can never delete. */
        if (fmt_lock_editor_is_active(&fmt_lock_editor_runtime) &&
            !lock_began && !lock_ended) {
            if (delete_was_armed && fmt_lock_editor_runtime.delete_armed &&
                (b & (1u << BTN_SEL))) {
                if (fmt_8bit_linked_editor_confirm_delete(&fmt_lock_editor_runtime)) {
                    delete_activity = 1;
                    tracker_footer_dirty();
                }
            } else if (!delete_was_armed &&
                       (b & (1u << BTN_EDIT))) {
                if (fmt_lock_editor_arm_delete(&fmt_lock_editor_runtime)) {
                    delete_activity = 1;
                    tracker_footer_dirty();
                }
            }
        }

        /* D025's pending FX state lives in FmtStepRecord so modifier ordering,
         * cancellation and writes are covered by the same editor unit tests. */
        if (step_rec_mode) {
            uint8_t write_track = fmt_pattern_input_runtime.selected_track;
            if (fmt_step_record_special_input(&fmt_step_record_runtime,
                                              &fmt_pattern_input_runtime,
                                              &fmt_transport_runtime,
                                              (b & (1u << BTN_FX)) != 0u,
                                              (rel & (1u << BTN_FX)) != 0u,
                                              (b & (1u << BTN_SEL)) != 0u)) {
                (void)fmt_8bit_linked_params_rebuild_track(write_track);
                (void)fmt_fm_linked_params_rebuild_track(write_track);
            }
        }

        if (pattern_mode && !step_rec_mode) {
            if (!fmt_lock_editor_is_active(&fmt_lock_editor_runtime)) {
                if (b & (1u << BTN_OCT_DOWN))
                    (void)fmt_pattern_input_page_prev(&fmt_pattern_input_runtime);
                if (b & (1u << BTN_OCT_UP))
                    (void)fmt_pattern_input_page_next(&fmt_pattern_input_runtime);
            }
        } else {
            if (b & (1u << BTN_OCT_DOWN) && set.oct > -2) { set.oct--; dirty = 1; t_change = fm1_ms; }
            if (b & (1u << BTN_OCT_UP) && set.oct < 2) { set.oct++; dirty = 1; t_change = fm1_ms; }
        }

        write_key = FMT_PATTERN_INPUT_KEY_COUNT;
        if (step_rec_mode && !fmt_step_record_runtime.gesture_latched &&
            fmt_step_record_lowest_key(down, &lowest_key))
            write_key = lowest_key;

        for (k = 0; k < 27u; k++) {
            if ((up >> k & 1u) && held[k]) {
                stop(held[k]);
                midi_send(0x80, held[k], 0);
                held[k] = 0u;
            }
            if (down >> k & 1u) {
                if (step_rec_mode) {
                    held[k] = (uint8_t)(base + k);
                    play(held[k], 100);
                    midi_send(0x90, held[k], 100);
                } else if (pattern_mode) {
                    /* D029 hold owns ordinary SEQ note-key input. Its owner key
                     * was already selected before this loop; all other downs are
                     * suppressed until release so target/cursor cannot diverge. */
                    if (!fmt_lock_editor_is_active(&fmt_lock_editor_runtime) && !lock_ended) {
                        if (!fmt_pattern_input_select_key(&fmt_pattern_input_runtime, k))
                            (void)fmt_pattern_input_select_track_key(&fmt_pattern_input_runtime, k);
                    }
                } else {
                    held[k] = (uint8_t)(base + k);
                    play(held[k], 100);
                    midi_send(0x90, held[k], 100);
                }
            }
        }

        /* D029/D030 encoder ordering. Begin/release/cancel polls discard every
         * editor delta. A poll that began armed also discards every delta even
         * when SEL confirms or EDIT release cancels during that poll. New arm
         * activity likewise consumes no browse/write movement. Otherwise SELECT
         * browse runs before the existing fixed K1 -> K4 writes. */
        if (fmt_lock_editor_is_active(&fmt_lock_editor_runtime) &&
            !lock_began && !lock_ended && !delete_was_armed && !delete_activity) {
            if (lock_select_delta != 0 &&
                fmt_lock_editor_browse(&fmt_lock_editor_runtime,
                                       lock_select_delta))
                tracker_footer_dirty();

            for (k = 0u; k < FMT_LOCK_EDITOR_KNOBS; k++) {
                if (lock_delta[k] != 0 &&
                    fmt_8bit_linked_editor_apply_delta(&fmt_lock_editor_runtime,
                                                       k,
                                                       lock_delta[k])) {
                    tracker_footer_dirty();
                }
            }
        }

        if (step_rec_mode && write_key < FMT_PATTERN_INPUT_KEY_COUNT) {
            uint8_t write_track = fmt_pattern_input_runtime.selected_track;
            if (fmt_step_record_write_note(&fmt_step_record_runtime,
                                           &fmt_pattern_input_runtime,
                                           &fmt_transport_runtime,
                                           base + write_key)) {
                (void)fmt_8bit_linked_params_rebuild_track(write_track);
                (void)fmt_fm_linked_params_rebuild_track(write_track);
            }
        }
        if (step_rec_mode && keys == 0u)
            fmt_step_record_release_gesture(&fmt_step_record_runtime);

        while (midi_get(&st, &d1, &d2)) {
            if ((st & 0xF0u) == 0x90u && d2) play(d1, d2);
            else if ((st & 0xF0u) == 0x80u || (st & 0xF0u) == 0x90u) stop(d1);
        }

        base = (uint32_t)(KEY0 + 12 * set.oct);
        for (k = 0; k < NV; k++)
            if (voice[k].gate && voice[k].note >= base && voice[k].note < base + 27u)
                lit |= 1u << (voice[k].note - base);
        for (k = 0; k < 27u; k++)
            fm1_led_key(KEY_LED(k), lit >> k & 1u);

        fm1_led_key(BTN_HOME, !pattern_mode);
        fm1_led_key(BTN_SEQ, pattern_mode);
        fm1_led_key(BTN_PLAY, tracker_play_requested != 0u);
        fm1_led_key(BTN_REC, step_rec_mode);
        fm1_led_key(BTN_FX, step_rec_mode && fmt_step_record_runtime.fx_pending);
        fm1_led_key(BTN_EDIT, pattern_mode && !step_rec_mode && fmt_lock_editor_runtime.delete_armed);
        fm1_led_key(BTN_OCT_DOWN, pattern_mode && !step_rec_mode ? fmt_pattern_input_runtime.page > 0u : set.oct < 0);
        fm1_led_key(BTN_OCT_UP, pattern_mode && !step_rec_mode ? fmt_pattern_input_runtime.page + 1u < FMT_PATTERN_INPUT_PAGE_COUNT : set.oct > 0);

        /* D026 screen ownership. Tracker drawing is strictly main-loop work and
         * at most two of the ten bounded regions can be blitted per poll. */
        if (pattern_mode) {
            uint32_t draws;
            uint8_t region;

            (void)fmt_tracker_screen_sync_feedback(&fmt_tracker_screen_runtime,
                                                   &fmt_pattern_input_runtime,
                                                   &fmt_step_record_runtime,
                                                   &fmt_transport_runtime,
                                                   &fmt_lock_editor_runtime);
            for (draws = 0u; draws < FMT_TRACKER_SCREEN_MAX_REGIONS_PER_POLL; draws++) {
                if (!fmt_tracker_screen_take_region(&fmt_tracker_screen_runtime, &region))
                    break;
                draw_tracker_region(region);
            }
        } else {
            int was_tracker_screen = fmt_tracker_screen_runtime.active != 0u;

            if (was_tracker_screen) {
                (void)fmt_tracker_screen_sync_feedback(&fmt_tracker_screen_runtime,
                                                   &fmt_pattern_input_runtime,
                                                   &fmt_step_record_runtime,
                                                   &fmt_transport_runtime,
                                                   &fmt_lock_editor_runtime);
                draw_home_static();
                lit_drawn = ~0u;
                note_drawn = ~0u;
                oct_drawn = 99;
            }

            if (lit != lit_drawn) { lit_drawn = lit; draw_keys(lit); }
            if (last_note != note_drawn) { note_drawn = last_note; draw_note(); }
            if (set.oct != oct_drawn) { oct_drawn = set.oct; draw_status(set.oct); }
        }

        if (dirty && quiet() && fm1_ms - t_change > 2000u) {
            settings_save(&set, sizeof set);
            dirty = 0;
        }
    }
}
