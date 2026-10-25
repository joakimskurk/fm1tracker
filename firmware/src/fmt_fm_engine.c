/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_fm_engine.h"

#include "fmt_engine_pitch.h"
#include "fmt_envelope.h"
#include "fmt_fm_ratio.h"

static const FmtFmPatch fmt_fm_default_patch = {
    { FMT_FM_RATIO_UNITY, FMT_FM_RATIO_TWO,
      FMT_FM_RATIO_UNITY, FMT_FM_RATIO_TWO },
    { 255u, 96u, 255u, 96u },
    {
        { 0u, 0u, 0u, 0u },
        { 0u, 0u, 0u, 0u },
        { 255u, 255u, 255u, 255u },
        { 96u, 96u, 96u, 96u }
    },
    FMT_FM_ALGO_DUAL2,
    64u,
    0u,
    0u
};

static uint16_t fmt_fm_engine_gain_clamp(uint16_t gain_q15)
{
    return gain_q15 > FMT_FM_ENGINE_GAIN_MAX_Q15
        ? (uint16_t)FMT_FM_ENGINE_GAIN_MAX_Q15
        : gain_q15;
}

static uint16_t fmt_fm_engine_pitch_clamp(uint16_t pitch_q7)
{
    return pitch_q7 > FMT_ENGINE_PITCH_MAX_Q7
        ? (uint16_t)FMT_ENGINE_PITCH_MAX_Q7
        : pitch_q7;
}

static uint8_t fmt_fm_voice_stage(const FmtFmVoice *voice)
{
    uint32_t i;
    uint8_t any_release = 0u;

    if (!voice)
        return FMT_ENVELOPE_IDLE;

    for (i = 0u; i < FMT_FM_ENVELOPE_OPERATOR_COUNT; i++) {
        uint8_t stage = voice->envelope_state.stage[i];

        if (stage == FMT_FM_ENVELOPE_STAGE_RELEASE)
            any_release = 1u;
        else if (stage != FMT_FM_ENVELOPE_STAGE_OFF)
            return FMT_ENVELOPE_SUSTAIN;
    }

    return any_release ? FMT_ENVELOPE_RELEASE : FMT_ENVELOPE_IDLE;
}

static void fmt_fm_stage_snapshot(const FmtFmEngine *engine,
                                  uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS])
{
    uint32_t i;

    for (i = 0u; i < FMT_VOICE_ALLOCATOR_SLOTS; i++)
        stage[i] = fmt_fm_voice_stage(&engine->voice[i]);
}

static uint8_t fmt_fm_engine_state_valid(const FmtFmEngine *engine)
{
    uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS];

    if (!engine)
        return 0u;
    fmt_fm_stage_snapshot(engine, stage);
    return fmt_voice_allocator_validate(&engine->allocator, stage);
}

static void fmt_fm_envelope_controls_copy_sanitized(
    FmtFmEnvelopeControls *dst,
    const FmtFmEnvelopeControls *src)
{
    uint32_t i;

    for (i = 0u; i < FMT_FM_ENVELOPE_OPERATOR_COUNT; i++) {
        dst->attack[i] = fmt_fm_envelope_control_sanitize(src->attack[i]);
        dst->decay[i] = fmt_fm_envelope_control_sanitize(src->decay[i]);
        dst->sustain[i] = fmt_fm_envelope_control_sanitize(src->sustain[i]);
        dst->release[i] = fmt_fm_envelope_control_sanitize(src->release[i]);
    }
}

static void fmt_fm_voice_clear(FmtFmVoice *voice)
{
    uint32_t i;

    if (!voice)
        return;

    fmt_fm_algorithm_reset(&voice->algorithm_state);
    fmt_fm_envelope_reset(&voice->envelope_state);
    fmt_fm_feedback_reset(&voice->feedback_state);
    voice->alignment_pad = 0u;
    for (i = 0u; i < 4u; i++) {
        voice->phase_increment[i] = 0u;
        voice->static_level_q15[i] = 0u;
        voice->ratio_id[i] = FMT_FM_RATIO_UNITY;
        voice->envelope_controls.attack[i] = 0u;
        voice->envelope_controls.decay[i] = 0u;
        voice->envelope_controls.sustain[i] = 0u;
        voice->envelope_controls.release[i] = 0u;
    }
    voice->mod_depth_q14 = 0u;
    voice->gain_q15 = 0u;
    voice->feedback_control = 0u;
    voice->algorithm = FMT_FM_ALGO_STACK4;
    voice->instrument = 0u;
    voice->reserved = 0u;
}

static void fmt_fm_voice_apply_patch(FmtFmVoice *voice,
                                     uint8_t instrument,
                                     const FmtFmPatch *patch)
{
    uint32_t i;

    for (i = 0u; i < 4u; i++) {
        voice->ratio_id[i] = fmt_fm_ratio_sanitize(patch->ratio_id[i]);
        voice->static_level_q15[i] = fmt_fm_level_q15(patch->level_control[i]);
    }
    fmt_fm_envelope_controls_copy_sanitized(&voice->envelope_controls,
                                             &patch->envelope);
    voice->mod_depth_q14 = fmt_fm_index_q14(patch->index_control);
    voice->feedback_control = fmt_fm_feedback_sanitize(patch->feedback_control);
    voice->algorithm = fmt_fm_algorithm_sanitize(patch->algorithm);
    voice->instrument = instrument;
    voice->reserved = 0u;
}

static void fmt_fm_voice_apply_row(FmtFmVoice *voice,
                                   uint16_t pitch_q7,
                                   uint16_t gain_q15)
{
    uint16_t pitch = fmt_fm_engine_pitch_clamp(pitch_q7);
    uint32_t base_increment = fmt_engine_pitch_inc_q7(pitch);

    fmt_fm_ratio_map4(base_increment,
                      voice->ratio_id,
                      voice->phase_increment);
    voice->gain_q15 = fmt_fm_engine_gain_clamp(gain_q15);
}

uint8_t fmt_fm_engine_supported(uint8_t engine_id)
{
    return engine_id == FMT_FM_ENGINE_FM4;
}

const FmtFmPatch *fmt_fm_engine_default_patch(void)
{
    return &fmt_fm_default_patch;
}

void fmt_fm_engine_reset(FmtFmEngine *engine)
{
    uint32_t i;

    if (!engine)
        return;
    fmt_voice_allocator_reset(&engine->allocator);
    for (i = 0u; i < FMT_FM_ENGINE_SLOTS; i++)
        fmt_fm_voice_clear(&engine->voice[i]);
}

uint8_t fmt_fm_engine_silence_track(FmtFmEngine *engine, uint8_t track)
{
    uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS];
    uint8_t voice_index;
    uint8_t detached;

    if (!engine || track >= FMT_FM_ENGINE_TRACKS)
        return FMT_VOICE_NONE;
    if (!fmt_fm_engine_state_valid(engine)) {
        fmt_fm_engine_reset(engine);
        return FMT_VOICE_NONE;
    }

    voice_index = engine->allocator.track_voice[track];
    if (voice_index == FMT_VOICE_NONE)
        return FMT_VOICE_NONE;

    fmt_fm_algorithm_reset(&engine->voice[voice_index].algorithm_state);
    fmt_fm_feedback_reset(&engine->voice[voice_index].feedback_state);
    fmt_fm_envelope_reset(&engine->voice[voice_index].envelope_state);
    fmt_fm_stage_snapshot(engine, stage);
    detached = fmt_voice_allocator_detach(&engine->allocator, track, stage);
    if (detached != voice_index) {
        fmt_fm_engine_reset(engine);
        return FMT_VOICE_NONE;
    }
    fmt_fm_voice_clear(&engine->voice[voice_index]);
    return voice_index;
}

uint8_t fmt_fm_engine_note(FmtFmEngine *engine,
                           uint8_t track,
                           uint8_t engine_id,
                           uint8_t instrument,
                           uint16_t pitch_q7,
                           uint16_t gain_q15,
                           const FmtFmPatch *patch)
{
    uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS];
    FmtVoiceAllocation allocation;
    FmtFmVoice *voice;

    if (!engine || track >= FMT_FM_ENGINE_TRACKS)
        return FMT_VOICE_NONE;
    if (!patch || instrument >= FMT_FM_ENGINE_INSTRUMENTS ||
        !fmt_fm_engine_supported(engine_id)) {
        (void)fmt_fm_engine_silence_track(engine, track);
        return FMT_VOICE_NONE;
    }
    if (!fmt_fm_engine_state_valid(engine)) {
        fmt_fm_engine_reset(engine);
        return FMT_VOICE_NONE;
    }

    fmt_fm_stage_snapshot(engine, stage);
    allocation = fmt_voice_allocator_acquire(&engine->allocator, track, stage);
    if (allocation.voice == FMT_VOICE_NONE)
        return FMT_VOICE_NONE;

    voice = &engine->voice[allocation.voice];
    if (allocation.reset_required)
        fmt_fm_voice_clear(voice);

    fmt_fm_voice_apply_patch(voice, instrument, patch);
    fmt_fm_voice_apply_row(voice, pitch_q7, gain_q15);
    fmt_fm_algorithm_retrigger(&voice->algorithm_state);
    fmt_fm_feedback_retrigger(&voice->feedback_state);
    fmt_fm_envelope_retrigger(&voice->envelope_state,
                              &voice->envelope_controls);
    return allocation.voice;
}

uint8_t fmt_fm_engine_update_row(FmtFmEngine *engine,
                                 uint8_t track,
                                 uint8_t instrument,
                                 uint16_t pitch_q7,
                                 uint16_t gain_q15,
                                 const FmtFmPatch *patch)
{
    uint8_t voice_index;
    FmtFmVoice *voice;

    if (!engine || !patch || track >= FMT_FM_ENGINE_TRACKS ||
        instrument >= FMT_FM_ENGINE_INSTRUMENTS)
        return FMT_VOICE_NONE;
    if (!fmt_fm_engine_state_valid(engine)) {
        fmt_fm_engine_reset(engine);
        return FMT_VOICE_NONE;
    }

    voice_index = engine->allocator.track_voice[track];
    if (voice_index == FMT_VOICE_NONE)
        return FMT_VOICE_NONE;
    voice = &engine->voice[voice_index];
    fmt_fm_voice_apply_patch(voice, instrument, patch);
    fmt_fm_voice_apply_row(voice, pitch_q7, gain_q15);
    return voice_index;
}

uint8_t fmt_fm_engine_retrigger(FmtFmEngine *engine,
                                uint8_t track,
                                uint8_t instrument,
                                uint16_t pitch_q7,
                                uint16_t gain_q15,
                                const FmtFmPatch *patch)
{
    uint8_t voice_index;
    FmtFmVoice *voice;

    voice_index = fmt_fm_engine_update_row(engine,
                                           track,
                                           instrument,
                                           pitch_q7,
                                           gain_q15,
                                           patch);
    if (voice_index == FMT_VOICE_NONE)
        return FMT_VOICE_NONE;

    voice = &engine->voice[voice_index];
    fmt_fm_algorithm_retrigger(&voice->algorithm_state);
    fmt_fm_feedback_retrigger(&voice->feedback_state);
    fmt_fm_envelope_retrigger(&voice->envelope_state,
                              &voice->envelope_controls);
    return voice_index;
}

uint8_t fmt_fm_engine_release(FmtFmEngine *engine,
                              uint8_t track,
                              uint16_t gain_q15,
                              const FmtFmPatch *patch)
{
    uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS];
    uint8_t voice_index;
    FmtFmVoice *voice;

    if (!engine || track >= FMT_FM_ENGINE_TRACKS)
        return FMT_VOICE_NONE;
    if (!fmt_fm_engine_state_valid(engine)) {
        fmt_fm_engine_reset(engine);
        return FMT_VOICE_NONE;
    }

    voice_index = engine->allocator.track_voice[track];
    if (voice_index == FMT_VOICE_NONE)
        return FMT_VOICE_NONE;
    voice = &engine->voice[voice_index];
    voice->gain_q15 = fmt_fm_engine_gain_clamp(gain_q15);
    if (patch) {
        fmt_fm_envelope_controls_copy_sanitized(&voice->envelope_controls,
                                                 &patch->envelope);
    }
    fmt_fm_envelope_note_off(&voice->envelope_state,
                             &voice->envelope_controls);
    fmt_fm_stage_snapshot(engine, stage);
    if (fmt_voice_allocator_detach(&engine->allocator, track, stage) !=
        voice_index) {
        fmt_fm_engine_reset(engine);
        return FMT_VOICE_NONE;
    }

    if (fmt_fm_voice_stage(voice) == FMT_ENVELOPE_IDLE)
        fmt_fm_voice_clear(voice);
    return voice_index;
}

int16_t fmt_fm_engine_render(FmtFmEngine *engine)
{
    int32_t mix = 0;
    uint32_t i;

    if (!engine)
        return 0;

    for (i = 0u; i < FMT_FM_ENGINE_SLOTS; i++) {
        FmtFmVoice *voice = &engine->voice[i];
        uint16_t level_q15[4];
        int32_t op4;
        int32_t sample;

        if (fmt_fm_voice_stage(voice) == FMT_ENVELOPE_IDLE)
            continue;

        fmt_fm_envelope_map_levels(&voice->envelope_state,
                                   voice->static_level_q15,
                                   level_q15);
        op4 = fmt_fm_feedback_render_op4(&voice->feedback_state,
                                         &voice->algorithm_state.op[3],
                                         voice->phase_increment[3],
                                         level_q15[3],
                                         voice->feedback_control);
        sample = fmt_fm_algorithm_render_from_op4(&voice->algorithm_state,
                                                  voice->algorithm,
                                                  voice->phase_increment,
                                                  level_q15,
                                                  voice->mod_depth_q14,
                                                  op4);
        sample = (sample * (int32_t)voice->gain_q15) >> 15;
        mix += sample;
        fmt_fm_envelope_advance(&voice->envelope_state,
                                &voice->envelope_controls);
    }

    if (mix > 32767)
        mix = 32767;
    else if (mix < -32767)
        mix = -32767;
    return (int16_t)mix;
}

uint8_t fmt_fm_engine_quiet(const FmtFmEngine *engine)
{
    uint32_t i;

    if (!engine)
        return 1u;
    for (i = 0u; i < FMT_FM_ENGINE_SLOTS; i++) {
        if (fmt_fm_voice_stage(&engine->voice[i]) != FMT_ENVELOPE_IDLE)
            return 0u;
    }
    return 1u;
}

uint8_t fmt_fm_engine_track_voice(const FmtFmEngine *engine, uint8_t track)
{
    if (!engine || track >= FMT_FM_ENGINE_TRACKS)
        return FMT_VOICE_NONE;
    return engine->allocator.track_voice[track];
}
