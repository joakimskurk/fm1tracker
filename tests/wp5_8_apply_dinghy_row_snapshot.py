#!/usr/bin/env python3
"""One-shot guarded D056 dinghy integration patch.

Every replacement requires the exact accepted D055/D056 source text. This file
exists only to make the large single-file dinghy edit fail closed through the
GitHub workflow; it is removed after the resulting production commit is gated.
"""
from pathlib import Path

path = Path("firmware/src/dinghy.c")
src = path.read_text()


def replace_once(old: str, new: str, label: str) -> None:
    global src
    count = src.count(old)
    if count != 1:
        raise SystemExit(f"{label}: expected one anchor, found {count}")
    src = src.replace(old, new, 1)


replace_once(
'''#include "fmt_8bit_linked_params.c"            /* WP4.9 Gate 3: linked base/cache/editor */
#include "fmt_fm_operator.c"                   /* WP5.1 D049: fixed-point FM operator */''',
'''#include "fmt_8bit_linked_params.c"            /* WP4.9 Gate 3: linked base/cache/editor */
#include "fmt_fm_params.c"                     /* WP5.8 D056: normalized FM parameter pages */
#include "fmt_fm_linked_params.c"              /* WP5.8 D056: base + row override publication */
#include "fmt_fm_operator.c"                   /* WP5.1 D049: fixed-point FM operator */''',
"source includes")

replace_once(
'''/* D045 Gate 4 fixed tracker snapshots. Audio owns these together with the
 * linked engine/effect/event state. Main owns only the aligned publication
 * caches and instrument base bank. 32+32+8+8 = 80 B ordinary RAM. */
static uint32_t tracker_common_row_word[FMT_TRACKS];
static uint32_t tracker_page_row_word[FMT_TRACKS];
static uint8_t tracker_current_instrument[FMT_TRACKS];
static uint8_t tracker_note_instrument[FMT_TRACKS];''',
'''/* D045 Gate 4 fixed tracker snapshots. D056 adds exactly one current FM patch
 * snapshot per active track, never one patch per row. The 224 B patch bank plus
 * eight validity bytes stay inside D056's frozen remaining runtime margin. */
static uint32_t tracker_common_row_word[FMT_TRACKS];
static uint32_t tracker_page_row_word[FMT_TRACKS];
static uint8_t tracker_current_instrument[FMT_TRACKS];
static uint8_t tracker_note_instrument[FMT_TRACKS];
static FmtFmPatch tracker_fm_row_patch[FMT_TRACKS];
static uint8_t tracker_fm_row_patch_valid[FMT_TRACKS];''',
"tracker snapshot bank")

replace_once(
'''static uint8_t tracker_capture_row_instrument(uint32_t track,
                                              const FmtTrackPlaybackEvent *event)
{''',
'''static int tracker_fm_snapshot_row(uint32_t track,
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
{''',
"row snapshot helper")

replace_once(
'''    } else if (family == FMT_ENGINE_FAMILY_FM) {
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
    }''',
'''    } else if (family == FMT_ENGINE_FAMILY_FM) {
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
    }''',
"live FM row update")

replace_once(
'''    const Fmt8BitParamSet *base;
    const FmtFmPatch *fm_patch;''',
'''    const Fmt8BitParamSet *base;''',
"note route local patch")

replace_once(
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
                                         fm_patch);''',
'''    case FMT_ENGINE_FAMILY_FM:
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
                                         &tracker_fm_row_patch[track]);''',
"FM note snapshot")

replace_once(
'''    case FMT_ENGINE_FAMILY_FM:
        (void)fmt_fm_engine_release(&fmt_fm_engine_runtime,
                                    (uint8_t)track,
                                    tracker_row_gain_q15(track),
                                    fmt_fm_engine_default_patch());
        break;''',
'''    case FMT_ENGINE_FAMILY_FM:
        if (!tracker_fm_row_patch_valid[track]) {
            tracker_hard_silence_family(track, FMT_ENGINE_FAMILY_FM);
            break;
        }
        (void)fmt_fm_engine_release(&fmt_fm_engine_runtime,
                                    (uint8_t)track,
                                    tracker_row_gain_q15(track),
                                    &tracker_fm_row_patch[track]);
        break;''',
"FM release snapshot")

replace_once(
'''                (void)fmt_fm_engine_retrigger(&fmt_fm_engine_runtime,
                                               (uint8_t)track,
                                               instrument,
                                               tracker_row_pitch_q7(track),
                                               tracker_row_gain_q15(track),
                                               fmt_fm_engine_default_patch());''',
'''                if (!tracker_fm_row_patch_valid[track]) {
                    tracker_hard_silence_route(track);
                    break;
                }
                (void)fmt_fm_engine_retrigger(&fmt_fm_engine_runtime,
                                               (uint8_t)track,
                                               instrument,
                                               tracker_row_pitch_q7(track),
                                               tracker_row_gain_q15(track),
                                               &tracker_fm_row_patch[track]);''',
"FM retrigger snapshot")

replace_once(
'''    /* D048 preserves D045 ordering. The currently owned family receives the
     * live row update before immediate NOTE/TIE/OFF semantics. */
    tracker_apply_effect_pitch(track);

    if (immediate_kind == FMT_PLAYBACK_EVENT_NOTE) {''',
'''    /* D056 keeps D048's pre-NOTE update on the previously owned patch, then
     * snapshots the new row exactly once from the published FM override cache.
     * TIE/EMPTY/OFF snapshot first so the live voice receives the new controls
     * without phase/envelope reset. No lock/store traversal occurs here. */
    if (event->kind == FMT_PLAYBACK_EVENT_NOTE)
        tracker_apply_effect_pitch(track);

    tracker_fm_row_patch_valid[track] =
        (uint8_t)tracker_fm_snapshot_row(track, event->step, instrument);

    if (event->kind != FMT_PLAYBACK_EVENT_NOTE)
        tracker_apply_effect_pitch(track);

    if (immediate_kind == FMT_PLAYBACK_EVENT_NOTE) {''',
"row-boundary snapshot ordering")

replace_once(
'''    fmt_playback_seed_proof_pattern();
    fmt_8bit_linked_params_reset();
    fmt_playback_params_reset();''',
'''    fmt_playback_seed_proof_pattern();
    fmt_8bit_linked_params_reset();
    fmt_fm_linked_params_reset();
    fmt_playback_params_reset();''',
"FM base reset")

replace_once(
'''    fmt_tracker_effect_cache_rebuild();
    fmt_8bit_linked_params_rebuild();
    fmt_instrument_identity_publication_reset(''',
'''    fmt_tracker_effect_cache_rebuild();
    fmt_8bit_linked_params_rebuild();
    fmt_fm_linked_params_rebuild();
    fmt_instrument_identity_publication_reset(''',
"FM row rebuild")

replace_once(
'''            tracker_note_instrument[track] = tracker_current_instrument[track];
        }
    }''',
'''            tracker_note_instrument[track] = tracker_current_instrument[track];
            tracker_fm_row_patch[track] = *fmt_fm_engine_default_patch();
            tracker_fm_row_patch_valid[track] = 0u;
        }
    }''',
"FM snapshot init")

replace_once(
'''            if (fmt_step_record_special_input(&fmt_step_record_runtime,
                                              &fmt_pattern_input_runtime,
                                              &fmt_transport_runtime,
                                              (b & (1u << BTN_FX)) != 0u,
                                              (rel & (1u << BTN_FX)) != 0u,
                                              (b & (1u << BTN_SEL)) != 0u))
                (void)fmt_8bit_linked_params_rebuild_track(write_track);''',
'''            if (fmt_step_record_special_input(&fmt_step_record_runtime,
                                              &fmt_pattern_input_runtime,
                                              &fmt_transport_runtime,
                                              (b & (1u << BTN_FX)) != 0u,
                                              (rel & (1u << BTN_FX)) != 0u,
                                              (b & (1u << BTN_SEL)) != 0u)) {
                (void)fmt_8bit_linked_params_rebuild_track(write_track);
                (void)fmt_fm_linked_params_rebuild_track(write_track);
            }''',
"special step rebuild")

replace_once(
'''            if (fmt_step_record_write_note(&fmt_step_record_runtime,
                                           &fmt_pattern_input_runtime,
                                           &fmt_transport_runtime,
                                           base + write_key))
                (void)fmt_8bit_linked_params_rebuild_track(write_track);''',
'''            if (fmt_step_record_write_note(&fmt_step_record_runtime,
                                           &fmt_pattern_input_runtime,
                                           &fmt_transport_runtime,
                                           base + write_key)) {
                (void)fmt_8bit_linked_params_rebuild_track(write_track);
                (void)fmt_fm_linked_params_rebuild_track(write_track);
            }''',
"note step rebuild")

# Production audio must no longer request the D055 hard-coded default patch.
# One default copy remains intentionally at initialization for fail-safe state.
if src.count("fmt_fm_engine_default_patch()") != 1:
    raise SystemExit(
        f"default patch call count after patch is {src.count('fmt_fm_engine_default_patch()')}, expected 1")

path.write_text(src)
print("WP5_8_DINGHY_PATCH_APPLIED=1 row_snapshot_bytes=224 valid_bytes=8 store_reads_audio=0")
