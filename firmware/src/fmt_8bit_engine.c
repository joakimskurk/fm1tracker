/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_8bit_engine.h"

#include "fmt_engine_pitch.h"

#define FMT_8BIT_ENGINE_INSTRUMENT_COUNT 16u
#define FMT_8BIT_GAIN_Q15_MAX 32767u

static uint16_t fmt_8bit_pitch_clamp(uint16_t pitch_q7)
{
    return pitch_q7 > FMT_ENGINE_PITCH_MAX_Q7
        ? (uint16_t)FMT_ENGINE_PITCH_MAX_Q7
        : pitch_q7;
}

static uint16_t fmt_8bit_gain_clamp(uint16_t gain_q15)
{
    return gain_q15 > FMT_8BIT_GAIN_Q15_MAX
        ? (uint16_t)FMT_8BIT_GAIN_Q15_MAX
        : gain_q15;
}

static void fmt_8bit_voice_clear(Fmt8BitVoice *voice)
{
    if (!voice)
        return;

    fmt_envelope_reset(&voice->envelope);
    voice->osc.noise.phase = 0u;
    voice->osc.noise.lfsr = 0u;
    voice->phase_inc = 0u;
    voice->page_word = 0u;
    voice->pitch_q7 = 0u;
    voice->gain_q15 = 0u;
    voice->engine = FMT_8BIT_ENGINE_SAFE;
    voice->instrument = 0u;
    voice->reserved[0] = 0u;
    voice->reserved[1] = 0u;
}

static void fmt_8bit_osc_reset(Fmt8BitVoice *voice)
{
    if (!voice)
        return;

    if (voice->engine == FMT_8BIT_ENGINE_PULSE)
        fmt_pulse_reset(&voice->osc.pulse);
    else if (voice->engine == FMT_8BIT_ENGINE_NOISE)
        fmt_noise_reset(&voice->osc.noise);
    else if (voice->engine == FMT_8BIT_ENGINE_TRIANGLE)
        fmt_triangle_reset(&voice->osc.triangle);
    else if (voice->engine == FMT_8BIT_ENGINE_WAVETABLE8)
        fmt_wavetable8_reset(&voice->osc.wavetable8);
    else {
        voice->osc.noise.phase = 0u;
        voice->osc.noise.lfsr = 0u;
    }
}

static void fmt_8bit_osc_retrigger(Fmt8BitVoice *voice)
{
    if (!voice)
        return;

    if (voice->engine == FMT_8BIT_ENGINE_PULSE)
        fmt_pulse_retrigger(&voice->osc.pulse);
    else if (voice->engine == FMT_8BIT_ENGINE_NOISE)
        fmt_noise_retrigger(&voice->osc.noise);
    else if (voice->engine == FMT_8BIT_ENGINE_TRIANGLE)
        fmt_triangle_retrigger(&voice->osc.triangle);
    else if (voice->engine == FMT_8BIT_ENGINE_WAVETABLE8)
        fmt_wavetable8_retrigger(&voice->osc.wavetable8);
}

static void fmt_8bit_stage_snapshot(const Fmt8BitEngine *engine,
                                    uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS])
{
    uint32_t i;

    for (i = 0u; i < FMT_VOICE_ALLOCATOR_SLOTS; i++)
        stage[i] = engine->voice[i].envelope.stage;
}

static uint8_t fmt_8bit_engine_state_valid(const Fmt8BitEngine *engine)
{
    uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS];

    if (!engine)
        return 0u;
    fmt_8bit_stage_snapshot(engine, stage);
    return fmt_voice_allocator_validate(&engine->allocator, stage);
}

static void fmt_8bit_envelope_params(uint8_t engine_id,
                                     uint32_t page_word,
                                     const Fmt8BitParamSet *base,
                                     FmtEnvelopeParams *params)
{
    uint8_t slot0 = fmt_8bit_page_unpack(page_word, 0u);

    if (engine_id == FMT_8BIT_ENGINE_PULSE ||
        engine_id == FMT_8BIT_ENGINE_WAVETABLE8)
        params->attack = base->attack;
    else
        params->attack = slot0;

    params->decay = fmt_8bit_page_unpack(page_word, 1u);
    params->sustain = fmt_8bit_page_unpack(page_word, 2u);
    params->release = fmt_8bit_page_unpack(page_word, 3u);
}

static void fmt_8bit_voice_apply_row(Fmt8BitVoice *voice,
                                     uint16_t pitch_q7,
                                     uint16_t gain_q15,
                                     uint32_t page_word)
{
    uint16_t pitch = fmt_8bit_pitch_clamp(pitch_q7);

    voice->pitch_q7 = pitch;
    voice->phase_inc = fmt_engine_pitch_inc_q7(pitch);
    voice->gain_q15 = fmt_8bit_gain_clamp(gain_q15);
    voice->page_word = page_word;
    voice->reserved[0] = 0u;
    voice->reserved[1] = 0u;
}

void fmt_8bit_engine_reset(Fmt8BitEngine *engine)
{
    uint32_t i;

    if (!engine)
        return;

    fmt_voice_allocator_reset(&engine->allocator);
    for (i = 0u; i < FMT_VOICE_ALLOCATOR_SLOTS; i++)
        fmt_8bit_voice_clear(&engine->voice[i]);
}

uint8_t fmt_8bit_engine_silence_track(Fmt8BitEngine *engine, uint8_t track)
{
    uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS];
    uint8_t voice;
    uint8_t detached;

    if (!engine || track >= FMT_VOICE_ALLOCATOR_TRACKS)
        return FMT_VOICE_NONE;

    if (!fmt_8bit_engine_state_valid(engine)) {
        fmt_8bit_engine_reset(engine);
        return FMT_VOICE_NONE;
    }

    voice = engine->allocator.track_voice[track];
    if (voice == FMT_VOICE_NONE)
        return FMT_VOICE_NONE;

    fmt_envelope_reset(&engine->voice[voice].envelope);
    fmt_8bit_stage_snapshot(engine, stage);
    detached = fmt_voice_allocator_detach(&engine->allocator, track, stage);
    if (detached == FMT_VOICE_NONE) {
        fmt_8bit_engine_reset(engine);
        return FMT_VOICE_NONE;
    }

    fmt_8bit_voice_clear(&engine->voice[detached]);
    return detached;
}

uint8_t fmt_8bit_engine_note(Fmt8BitEngine *engine,
                             uint8_t track,
                             uint8_t engine_id,
                             uint8_t instrument,
                             uint16_t pitch_q7,
                             uint16_t gain_q15,
                             uint32_t page_word,
                             const Fmt8BitParamSet *base)
{
    uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS];
    FmtVoiceAllocation allocation;
    Fmt8BitVoice *voice;
    FmtEnvelopeParams env_params;
    uint8_t identity_changed;

    if (!engine || track >= FMT_VOICE_ALLOCATOR_TRACKS)
        return FMT_VOICE_NONE;

    if (!base || instrument >= FMT_8BIT_ENGINE_INSTRUMENT_COUNT ||
        !fmt_8bit_engine_supported(engine_id)) {
        (void)fmt_8bit_engine_silence_track(engine, track);
        return FMT_VOICE_NONE;
    }

    if (!fmt_8bit_engine_state_valid(engine)) {
        fmt_8bit_engine_reset(engine);
        return FMT_VOICE_NONE;
    }

    fmt_8bit_stage_snapshot(engine, stage);
    allocation = fmt_voice_allocator_acquire(&engine->allocator, track, stage);
    if (allocation.voice == FMT_VOICE_NONE)
        return FMT_VOICE_NONE;

    voice = &engine->voice[allocation.voice];
    identity_changed = (uint8_t)(voice->engine != engine_id ||
                                 voice->instrument != instrument);

    if (allocation.reset_required || identity_changed) {
        fmt_envelope_reset(&voice->envelope);
        voice->engine = engine_id;
        voice->instrument = instrument;
        fmt_8bit_osc_reset(voice);
    } else {
        voice->engine = engine_id;
        voice->instrument = instrument;
    }

    fmt_8bit_voice_apply_row(voice, pitch_q7, gain_q15, page_word);
    fmt_8bit_envelope_params(engine_id, page_word, base, &env_params);
    fmt_envelope_note_on(&voice->envelope, &env_params);
    return allocation.voice;
}

uint8_t fmt_8bit_engine_retrigger(Fmt8BitEngine *engine,
                                  uint8_t track,
                                  uint16_t pitch_q7,
                                  uint16_t gain_q15,
                                  uint32_t page_word,
                                  const Fmt8BitParamSet *base)
{
    uint8_t voice_index;
    Fmt8BitVoice *voice;
    FmtEnvelopeParams env_params;

    if (!engine || !base || track >= FMT_VOICE_ALLOCATOR_TRACKS)
        return FMT_VOICE_NONE;
    if (!fmt_8bit_engine_state_valid(engine)) {
        fmt_8bit_engine_reset(engine);
        return FMT_VOICE_NONE;
    }

    voice_index = engine->allocator.track_voice[track];
    if (voice_index == FMT_VOICE_NONE)
        return FMT_VOICE_NONE;

    voice = &engine->voice[voice_index];
    if (!fmt_8bit_engine_supported(voice->engine)) {
        (void)fmt_8bit_engine_silence_track(engine, track);
        return FMT_VOICE_NONE;
    }

    fmt_8bit_voice_apply_row(voice, pitch_q7, gain_q15, page_word);
    fmt_8bit_envelope_params(voice->engine, page_word, base, &env_params);
    fmt_8bit_osc_retrigger(voice);
    fmt_envelope_retrigger(&voice->envelope, &env_params);
    return voice_index;
}

uint8_t fmt_8bit_engine_update_row(Fmt8BitEngine *engine,
                                   uint8_t track,
                                   uint16_t pitch_q7,
                                   uint16_t gain_q15,
                                   uint32_t page_word)
{
    uint8_t voice_index;

    if (!engine || track >= FMT_VOICE_ALLOCATOR_TRACKS)
        return FMT_VOICE_NONE;
    if (!fmt_8bit_engine_state_valid(engine)) {
        fmt_8bit_engine_reset(engine);
        return FMT_VOICE_NONE;
    }

    voice_index = engine->allocator.track_voice[track];
    if (voice_index == FMT_VOICE_NONE)
        return FMT_VOICE_NONE;

    fmt_8bit_voice_apply_row(&engine->voice[voice_index],
                             pitch_q7,
                             gain_q15,
                             page_word);
    return voice_index;
}

uint8_t fmt_8bit_engine_release(Fmt8BitEngine *engine,
                                uint8_t track,
                                uint16_t gain_q15,
                                uint32_t page_word)
{
    uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS];
    uint8_t voice_index;
    uint8_t release;

    if (!engine || track >= FMT_VOICE_ALLOCATOR_TRACKS)
        return FMT_VOICE_NONE;
    if (!fmt_8bit_engine_state_valid(engine)) {
        fmt_8bit_engine_reset(engine);
        return FMT_VOICE_NONE;
    }

    voice_index = engine->allocator.track_voice[track];
    if (voice_index == FMT_VOICE_NONE)
        return FMT_VOICE_NONE;

    engine->voice[voice_index].gain_q15 = fmt_8bit_gain_clamp(gain_q15);
    engine->voice[voice_index].page_word = page_word;
    release = fmt_8bit_page_unpack(page_word, 3u);
    fmt_envelope_gate_off(&engine->voice[voice_index].envelope, release);

    fmt_8bit_stage_snapshot(engine, stage);
    if (fmt_voice_allocator_detach(&engine->allocator, track, stage) !=
        voice_index) {
        fmt_8bit_engine_reset(engine);
        return FMT_VOICE_NONE;
    }

    if (engine->voice[voice_index].envelope.stage == FMT_ENVELOPE_IDLE)
        fmt_8bit_voice_clear(&engine->voice[voice_index]);
    return voice_index;
}

static int32_t fmt_8bit_voice_source(Fmt8BitVoice *voice)
{
    uint8_t slot0;

    if (!voice)
        return 0;

    slot0 = fmt_8bit_page_unpack(voice->page_word, 0u);
    if (voice->engine == FMT_8BIT_ENGINE_PULSE)
        return fmt_pulse_render(&voice->osc.pulse,
                                voice->phase_inc,
                                fmt_8bit_pulse_width_from_normalized(slot0));
    if (voice->engine == FMT_8BIT_ENGINE_NOISE)
        return fmt_noise_render(&voice->osc.noise,
                                fmt_noise_clock_inc(voice->phase_inc));
    if (voice->engine == FMT_8BIT_ENGINE_TRIANGLE)
        return fmt_triangle_render(&voice->osc.triangle, voice->phase_inc);
    if (voice->engine == FMT_8BIT_ENGINE_WAVETABLE8)
        return fmt_wavetable8_render(&voice->osc.wavetable8,
                                     voice->phase_inc,
                                     fmt_8bit_wavetable_from_normalized(slot0));
    return 0;
}

int16_t fmt_8bit_engine_render(Fmt8BitEngine *engine)
{
    int32_t mix = 0;
    uint32_t i;

    if (!engine)
        return 0;

    for (i = 0u; i < FMT_VOICE_ALLOCATOR_SLOTS; i++) {
        Fmt8BitVoice *voice = &engine->voice[i];
        int32_t source_q15;
        uint16_t env_q15;
        int32_t x;

        if (voice->envelope.stage == FMT_ENVELOPE_IDLE)
            continue;

        source_q15 = fmt_8bit_voice_source(voice);
        env_q15 = fmt_envelope_render(&voice->envelope);
        x = (source_q15 * (int32_t)env_q15) >> 15;
        x = (x * (int32_t)voice->gain_q15) >> 15;
        mix += x;
    }

    if (mix > 32767)
        mix = 32767;
    else if (mix < -32767)
        mix = -32767;
    return (int16_t)mix;
}

uint8_t fmt_8bit_engine_quiet(const Fmt8BitEngine *engine)
{
    uint32_t i;

    if (!engine)
        return 1u;
    for (i = 0u; i < FMT_VOICE_ALLOCATOR_SLOTS; i++) {
        if (engine->voice[i].envelope.stage != FMT_ENVELOPE_IDLE)
            return 0u;
    }
    return 1u;
}

uint8_t fmt_8bit_engine_track_voice(const Fmt8BitEngine *engine,
                                    uint8_t track)
{
    if (!engine || track >= FMT_VOICE_ALLOCATOR_TRACKS)
        return FMT_VOICE_NONE;
    return engine->allocator.track_voice[track];
}
