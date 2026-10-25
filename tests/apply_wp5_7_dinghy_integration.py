#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
from pathlib import Path

path = Path("firmware/src/dinghy.c")
s = path.read_text()


def replace_once(old, new, label):
    global s
    count = s.count(old)
    if count != 1:
        raise SystemExit(f"{label}: expected exactly one anchor, got {count}")
    s = s.replace(old, new, 1)


replace_once(
'''#include "fmt_8bit_linked_params.c"            /* WP4.9 Gate 3: linked base/cache/editor */
#include "fmt_engine_router.c"                 /* WP5.0 D048: per-instrument family router */''',
'''#include "fmt_8bit_linked_params.c"            /* WP4.9 Gate 3: linked base/cache/editor */
#include "fmt_fm_operator.c"                   /* WP5.1 D049: fixed-point FM operator */
#include "fmt_fm_algorithm.c"                  /* WP5.2/D055: FM topology + external OP4 seam */
#include "fmt_fm_ratio.c"                      /* WP5.3 D051: bounded ratio mapping */
#include "fmt_fm_controls.c"                   /* WP5.4 D052: LEVEL/INDEX mapping */
#include "fmt_fm_feedback.c"                   /* WP5.5 D053: bounded OP4 feedback */
#include "fmt_fm_envelope.c"                   /* WP5.6 D054: four operator ADSRs */
#include "fmt_fm_engine.c"                     /* WP5.7 D055: fixed eight-slot FM family */
#include "fmt_engine_router.c"                 /* WP5.0/D055: per-instrument family router */''',
"include block")

replace_once(
'''Fmt8BitEngine fmt_8bit_engine_runtime;
FmtEngineRouter fmt_engine_router_runtime;''',
'''Fmt8BitEngine fmt_8bit_engine_runtime;
FmtFmEngine fmt_fm_engine_runtime;
FmtEngineRouter fmt_engine_router_runtime;''',
"runtime globals")

replace_once(
'''    case FMT_ENGINE_FAMILY_FM:
        /* D048 reserves the FM family identity, but WP5.0 contains no FM DSP. */
        break;''',
'''    case FMT_ENGINE_FAMILY_FM:
        (void)fmt_fm_engine_silence_track(&fmt_fm_engine_runtime,
                                          (uint8_t)track);
        break;''',
"hard silence FM")

replace_once(
'''    case FMT_ENGINE_FAMILY_FM:
        return 0u;''',
'''    case FMT_ENGINE_FAMILY_FM:
        return fmt_fm_engine_track_voice(&fmt_fm_engine_runtime,
                                         (uint8_t)track) != FMT_VOICE_NONE;''',
"route active FM")

replace_once(
'''static void tracker_apply_effect_pitch(uint32_t track)
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
}''',
'''static void tracker_apply_effect_pitch(uint32_t track)
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
        (void)fmt_fm_engine_update_row(
            &fmt_fm_engine_runtime,
            (uint8_t)track,
            fmt_fm_engine_runtime.voice[voice_index].instrument,
            tracker_row_pitch_q7(track),
            tracker_row_gain_q15(track),
            fmt_fm_engine_default_patch());
    }
}''',
"effect pitch dispatch")

replace_once(
'''    uint8_t voice_index;
    const Fmt8BitParamSet *base;''',
'''    uint8_t voice_index;
    const Fmt8BitParamSet *base;
    const FmtFmPatch *fm_patch;''',
"note declarations")

replace_once(
'''    case FMT_ENGINE_FAMILY_FM:
        /* Identity is valid but the family adapter is intentionally absent in
         * WP5.0. Unsupported production dispatch always fails toward silence. */
        if (current_family == FMT_ENGINE_FAMILY_FM)
            tracker_hard_silence_family(track, current_family);
        fmt_engine_router_clear(&fmt_engine_router_runtime, (uint8_t)track);
        return 0u;''',
'''    case FMT_ENGINE_FAMILY_FM:
        if (!fmt_fm_engine_supported(engine_id)) {
            tracker_hard_silence_route(track);
            return 0u;
        }
        fm_patch = fmt_fm_engine_default_patch();
        voice_index = fmt_fm_engine_note(&fmt_fm_engine_runtime,
                                         (uint8_t)track,
                                         engine_id,
                                         instrument,
                                         tracker_row_pitch_q7(track),
                                         tracker_row_gain_q15(track),
                                         fm_patch);
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
        return 1u;''',
"FM note route")

replace_once(
'''    case FMT_ENGINE_FAMILY_FM:
        break;
    default:
        break;
    }
    fmt_engine_router_clear(&fmt_engine_router_runtime, (uint8_t)track);''',
'''    case FMT_ENGINE_FAMILY_FM:
        (void)fmt_fm_engine_release(&fmt_fm_engine_runtime,
                                    (uint8_t)track,
                                    tracker_row_gain_q15(track),
                                    fmt_fm_engine_default_patch());
        break;
    default:
        break;
    }
    fmt_engine_router_clear(&fmt_engine_router_runtime, (uint8_t)track);''',
"FM release route")

replace_once(
'''static void tracker_apply_row_action(uint32_t track, uint8_t action, uint8_t note)
{
    FmtTrackerEffectState *effect;
    uint8_t instrument, voice_index;
    const Fmt8BitParamSet *base;''',
'''static void tracker_apply_row_action(uint32_t track, uint8_t action, uint8_t note)
{
    FmtTrackerEffectState *effect;
    uint8_t family, instrument, voice_index;
    const Fmt8BitParamSet *base;''',
"row action declarations")

replace_once(
'''    case FMT_TRACKER_ROW_ACTION_RETRIG:
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
        break;''',
'''    case FMT_TRACKER_ROW_ACTION_RETRIG:
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
                (void)fmt_fm_engine_retrigger(&fmt_fm_engine_runtime,
                                               (uint8_t)track,
                                               instrument,
                                               tracker_row_pitch_q7(track),
                                               tracker_row_gain_q15(track),
                                               fmt_fm_engine_default_patch());
            }
        }
        break;''',
"FM retrig route")

replace_once(
'''        for (i = 0u; i < n; i++) {
            int32_t sample = fmt_8bit_engine_render(&fmt_8bit_engine_runtime);
            out[2u * i] += sample;
            out[2u * i + 1u] += sample;''',
'''        for (i = 0u; i < n; i++) {
            int32_t sample = fmt_8bit_engine_render(&fmt_8bit_engine_runtime);
            sample += fmt_fm_engine_render(&fmt_fm_engine_runtime);
            if (sample > 32767)
                sample = 32767;
            else if (sample < -32767)
                sample = -32767;
            out[2u * i] += sample;
            out[2u * i + 1u] += sample;''',
"audio render")

replace_once(
'''    if (!fmt_8bit_engine_quiet(&fmt_8bit_engine_runtime))
        return 0;

    for (v = 0; v < NV; v++)''',
'''    if (!fmt_8bit_engine_quiet(&fmt_8bit_engine_runtime) ||
        !fmt_fm_engine_quiet(&fmt_fm_engine_runtime))
        return 0;

    for (v = 0; v < NV; v++)''',
"quiet")

replace_once(
'''    fmt_8bit_engine_reset(&fmt_8bit_engine_runtime);
    fmt_engine_router_reset(&fmt_engine_router_runtime);''',
'''    fmt_8bit_engine_reset(&fmt_8bit_engine_runtime);
    fmt_fm_engine_reset(&fmt_fm_engine_runtime);
    fmt_engine_router_reset(&fmt_engine_router_runtime);''',
"startup reset")

path.write_text(s)
print("WP5_7_DINGHY_PATCH_OK=1 includes=fm_engine route=fm render=dual quiet=dual startup=dual")
