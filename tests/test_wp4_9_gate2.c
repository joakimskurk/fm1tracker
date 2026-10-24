/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "fmt_8bit_engine.h"

#define FULL_GAIN 32767u
#define C4_Q7 ((uint16_t)(60u * 128u))
#define E4_Q7 ((uint16_t)(64u * 128u))
#define G4_Q7 ((uint16_t)(67u * 128u))
#define C5_Q7 ((uint16_t)(72u * 128u))

static Fmt8BitParamSet instant_base(uint8_t release)
{
    Fmt8BitParamSet base;

    fmt_8bit_param_set_defaults(&base);
    base.attack = 0u;
    base.decay = 0u;
    base.sustain = 255u;
    base.release = release;
    return base;
}

static uint32_t page_for(uint8_t engine,
                         uint8_t slot0,
                         uint8_t decay,
                         uint8_t sustain,
                         uint8_t release)
{
    uint32_t word = 0u;

    if (engine == FMT_8BIT_ENGINE_PULSE ||
        engine == FMT_8BIT_ENGINE_WAVETABLE8)
        return fmt_8bit_page_pack(slot0, decay, sustain, release);

    return fmt_8bit_page_pack(slot0, decay, sustain, release);
}

static uint32_t default_instant_page(uint8_t engine, uint8_t release)
{
    uint8_t slot0 = 0u;

    if (engine == FMT_8BIT_ENGINE_PULSE)
        slot0 = 128u;
    else if (engine == FMT_8BIT_ENGINE_WAVETABLE8)
        slot0 = 0u;
    return page_for(engine, slot0, 0u, 255u, release);
}

static void test_layout_and_reset(void)
{
    Fmt8BitEngine engine;
    uint32_t i;

    assert(sizeof(Fmt8BitOscState) == 8u);
    assert(sizeof(Fmt8BitVoice) == 44u);
    assert(sizeof(Fmt8BitEngine) == 404u);

    fmt_8bit_engine_reset(&engine);
    assert(fmt_8bit_engine_quiet(&engine) == 1u);
    for (i = 0u; i < FMT_VOICE_ALLOCATOR_SLOTS; i++) {
        assert(engine.allocator.voice_owner[i] == FMT_TRACK_NONE);
        assert(engine.allocator.track_voice[i] == FMT_VOICE_NONE);
        assert(engine.voice[i].envelope.stage == FMT_ENVELOPE_IDLE);
        assert(engine.voice[i].engine == FMT_8BIT_ENGINE_SAFE);
        assert(engine.voice[i].phase_inc == 0u);
        assert(engine.voice[i].gain_q15 == 0u);
    }
    assert(fmt_8bit_engine_track_voice(&engine, 8u) == FMT_VOICE_NONE);
    assert(fmt_8bit_engine_track_voice((const Fmt8BitEngine *)0, 0u) ==
           FMT_VOICE_NONE);
}

static void test_attack_source_and_articulation_snapshot(void)
{
    Fmt8BitEngine engine;
    Fmt8BitParamSet base = instant_base(72u);
    uint8_t v;

    base.attack = 9u;

    fmt_8bit_engine_reset(&engine);
    v = fmt_8bit_engine_note(&engine, 0u, FMT_8BIT_ENGINE_PULSE, 0u,
                             C4_Q7, FULL_GAIN,
                             page_for(FMT_8BIT_ENGINE_PULSE,
                                      0u, 0u, 255u, 72u),
                             &base);
    assert(v == 0u);
    assert(engine.voice[v].envelope.stage == FMT_ENVELOPE_ATTACK);

    fmt_8bit_engine_reset(&engine);
    v = fmt_8bit_engine_note(&engine, 0u, FMT_8BIT_ENGINE_WAVETABLE8, 0u,
                             C4_Q7, FULL_GAIN,
                             page_for(FMT_8BIT_ENGINE_WAVETABLE8,
                                      0u, 0u, 255u, 72u),
                             &base);
    assert(v == 0u);
    assert(engine.voice[v].envelope.stage == FMT_ENVELOPE_ATTACK);

    fmt_8bit_engine_reset(&engine);
    v = fmt_8bit_engine_note(&engine, 0u, FMT_8BIT_ENGINE_NOISE, 0u,
                             C4_Q7, FULL_GAIN,
                             page_for(FMT_8BIT_ENGINE_NOISE,
                                      0u, 0u, 255u, 72u),
                             &base);
    assert(v == 0u);
    assert(engine.voice[v].envelope.stage == FMT_ENVELOPE_SUSTAIN);

    fmt_8bit_engine_reset(&engine);
    v = fmt_8bit_engine_note(&engine, 0u, FMT_8BIT_ENGINE_TRIANGLE, 0u,
                             C4_Q7, FULL_GAIN,
                             page_for(FMT_8BIT_ENGINE_TRIANGLE,
                                      9u, 0u, 255u, 72u),
                             &base);
    assert(v == 0u);
    assert(engine.voice[v].envelope.stage == FMT_ENVELOPE_ATTACK);
}

static void test_note_reuse_identity_and_retrigger(void)
{
    Fmt8BitEngine engine;
    Fmt8BitParamSet base = instant_base(72u);
    uint32_t page = default_instant_page(FMT_8BIT_ENGINE_PULSE, 72u);
    uint32_t phase_before;
    uint8_t v;
    uint32_t i;

    fmt_8bit_engine_reset(&engine);
    v = fmt_8bit_engine_note(&engine, 0u, FMT_8BIT_ENGINE_PULSE, 0u,
                             C4_Q7, FULL_GAIN, page, &base);
    assert(v == 0u);
    for (i = 0u; i < 4u; i++)
        (void)fmt_8bit_engine_render(&engine);
    phase_before = engine.voice[v].osc.pulse.phase;
    assert(phase_before != 0u);

    assert(fmt_8bit_engine_note(&engine, 0u, FMT_8BIT_ENGINE_PULSE, 0u,
                                E4_Q7, FULL_GAIN, page, &base) == v);
    assert(engine.voice[v].osc.pulse.phase == phase_before);
    assert(engine.voice[v].pitch_q7 == E4_Q7);
    assert(engine.voice[v].envelope.stage == FMT_ENVELOPE_SUSTAIN);

    assert(fmt_8bit_engine_note(&engine, 0u, FMT_8BIT_ENGINE_PULSE, 1u,
                                G4_Q7, FULL_GAIN, page, &base) == v);
    assert(engine.voice[v].osc.pulse.phase == 0u);
    assert(engine.voice[v].instrument == 1u);

    for (i = 0u; i < 3u; i++)
        (void)fmt_8bit_engine_render(&engine);
    assert(engine.voice[v].osc.pulse.phase != 0u);
    assert(fmt_8bit_engine_retrigger(&engine, 0u, C4_Q7, FULL_GAIN,
                                     page, &base) == v);
    assert(engine.voice[v].osc.pulse.phase == 0u);
    assert(engine.voice[v].envelope.stage == FMT_ENVELOPE_SUSTAIN);

    assert(fmt_8bit_engine_note(&engine, 0u, FMT_8BIT_ENGINE_NOISE, 2u,
                                C4_Q7, FULL_GAIN,
                                default_instant_page(FMT_8BIT_ENGINE_NOISE,
                                                     72u),
                                &base) == v);
    assert(engine.voice[v].engine == FMT_8BIT_ENGINE_NOISE);
    assert(engine.voice[v].instrument == 2u);
    assert(engine.voice[v].osc.noise.phase == 0u);
    assert(engine.voice[v].osc.noise.lfsr == FMT_NOISE_SEED);
}

static void test_live_row_update_does_not_retime(void)
{
    Fmt8BitEngine engine;
    Fmt8BitParamSet base = instant_base(72u);
    uint32_t page0 = default_instant_page(FMT_8BIT_ENGINE_PULSE, 72u);
    uint32_t page1 = page_for(FMT_8BIT_ENGINE_PULSE, 200u, 91u, 3u, 33u);
    uint32_t phase_before;
    uint32_t level_before;
    uint32_t sustain_before;
    uint8_t stage_before;
    uint8_t v;

    fmt_8bit_engine_reset(&engine);
    v = fmt_8bit_engine_note(&engine, 0u, FMT_8BIT_ENGINE_PULSE, 0u,
                             C4_Q7, FULL_GAIN, page0, &base);
    assert(v == 0u);
    (void)fmt_8bit_engine_render(&engine);

    phase_before = engine.voice[v].osc.pulse.phase;
    level_before = engine.voice[v].envelope.level_q16;
    sustain_before = engine.voice[v].envelope.sustain_q16;
    stage_before = engine.voice[v].envelope.stage;

    assert(fmt_8bit_engine_update_row(&engine, 0u, G4_Q7, 12345u,
                                      page1) == v);
    assert(engine.voice[v].osc.pulse.phase == phase_before);
    assert(engine.voice[v].envelope.level_q16 == level_before);
    assert(engine.voice[v].envelope.sustain_q16 == sustain_before);
    assert(engine.voice[v].envelope.stage == stage_before);
    assert(engine.voice[v].pitch_q7 == G4_Q7);
    assert(engine.voice[v].gain_q15 == 12345u);
    assert(engine.voice[v].page_word == page1);
}

static void test_release_tail_detach_steal_and_quiet(void)
{
    Fmt8BitEngine engine;
    Fmt8BitParamSet base = instant_base(1u);
    uint32_t page = default_instant_page(FMT_8BIT_ENGINE_PULSE, 1u);
    uint32_t track;
    uint32_t frames;
    uint8_t v;

    fmt_8bit_engine_reset(&engine);
    for (track = 0u; track < 8u; track++) {
        v = fmt_8bit_engine_note(&engine, (uint8_t)track,
                                 FMT_8BIT_ENGINE_PULSE,
                                 (uint8_t)track,
                                 (uint16_t)(C4_Q7 + track * 128u),
                                 FULL_GAIN, page, &base);
        assert(v == (uint8_t)track);
    }

    assert(fmt_8bit_engine_release(&engine, 0u, FULL_GAIN, page) == 0u);
    assert(fmt_8bit_engine_track_voice(&engine, 0u) == FMT_VOICE_NONE);
    assert(engine.allocator.voice_owner[0] == FMT_TRACK_NONE);
    assert(engine.voice[0].envelope.stage == FMT_ENVELOPE_RELEASE);
    assert(fmt_8bit_engine_quiet(&engine) == 0u);

    v = fmt_8bit_engine_note(&engine, 0u, FMT_8BIT_ENGINE_PULSE, 0u,
                             C5_Q7, FULL_GAIN, page, &base);
    assert(v == 0u);
    assert(engine.allocator.voice_owner[0] == 0u);
    assert(engine.voice[0].osc.pulse.phase == 0u);
    assert(engine.voice[0].envelope.stage == FMT_ENVELOPE_SUSTAIN);

    fmt_8bit_engine_reset(&engine);
    assert(fmt_8bit_engine_note(&engine, 0u, FMT_8BIT_ENGINE_TRIANGLE, 0u,
                                C4_Q7, FULL_GAIN,
                                default_instant_page(
                                    FMT_8BIT_ENGINE_TRIANGLE, 1u),
                                &base) == 0u);
    assert(fmt_8bit_engine_release(
               &engine, 0u, FULL_GAIN,
               default_instant_page(FMT_8BIT_ENGINE_TRIANGLE, 1u)) == 0u);
    assert(fmt_8bit_engine_quiet(&engine) == 0u);
    for (frames = 0u; frames < 128u && !fmt_8bit_engine_quiet(&engine);
         frames++)
        (void)fmt_8bit_engine_render(&engine);
    assert(frames != 0u && frames < 128u);
    assert(fmt_8bit_engine_quiet(&engine) == 1u);
    assert(fmt_8bit_engine_track_voice(&engine, 0u) == FMT_VOICE_NONE);

    base.release = 0u;
    fmt_8bit_engine_reset(&engine);
    assert(fmt_8bit_engine_note(&engine, 0u, FMT_8BIT_ENGINE_PULSE, 0u,
                                C4_Q7, FULL_GAIN,
                                default_instant_page(
                                    FMT_8BIT_ENGINE_PULSE, 0u),
                                &base) == 0u);
    assert(fmt_8bit_engine_release(
               &engine, 0u, FULL_GAIN,
               default_instant_page(FMT_8BIT_ENGINE_PULSE, 0u)) == 0u);
    assert(fmt_8bit_engine_quiet(&engine) == 1u);
    assert(engine.voice[0].engine == FMT_8BIT_ENGINE_SAFE);
}

static void test_malformed_note_fails_to_track_silence(void)
{
    Fmt8BitEngine engine;
    Fmt8BitParamSet base = instant_base(72u);
    uint32_t page = default_instant_page(FMT_8BIT_ENGINE_PULSE, 72u);

    fmt_8bit_engine_reset(&engine);
    assert(fmt_8bit_engine_note(&engine, 3u, FMT_8BIT_ENGINE_PULSE, 3u,
                                C4_Q7, FULL_GAIN, page, &base) == 0u);
    assert(fmt_8bit_engine_track_voice(&engine, 3u) == 0u);

    assert(fmt_8bit_engine_note(&engine, 3u, FMT_8BIT_ENGINE_SAFE, 3u,
                                C4_Q7, FULL_GAIN, 0u, &base) ==
           FMT_VOICE_NONE);
    assert(fmt_8bit_engine_track_voice(&engine, 3u) == FMT_VOICE_NONE);
    assert(engine.voice[0].envelope.stage == FMT_ENVELOPE_IDLE);
    assert(engine.voice[0].engine == FMT_8BIT_ENGINE_SAFE);

    assert(fmt_8bit_engine_note(&engine, 4u, FMT_8BIT_ENGINE_PULSE, 16u,
                                C4_Q7, FULL_GAIN, page, &base) ==
           FMT_VOICE_NONE);
    assert(fmt_8bit_engine_track_voice(&engine, 4u) == FMT_VOICE_NONE);
}

static void populate_eight_pulses(Fmt8BitEngine *engine,
                                  const Fmt8BitParamSet *base)
{
    uint32_t track;
    uint32_t page = default_instant_page(FMT_8BIT_ENGINE_PULSE, 72u);

    fmt_8bit_engine_reset(engine);
    for (track = 0u; track < 8u; track++) {
        assert(fmt_8bit_engine_note(engine, (uint8_t)track,
                                    FMT_8BIT_ENGINE_PULSE,
                                    (uint8_t)track,
                                    C4_Q7,
                                    FULL_GAIN,
                                    page,
                                    base) == (uint8_t)track);
    }
}

static void test_eight_voice_mix_clamp(void)
{
    Fmt8BitEngine engine;
    Fmt8BitParamSet base = instant_base(72u);
    uint32_t i;

    populate_eight_pulses(&engine, &base);
    assert(fmt_8bit_engine_render(&engine) == 32767);

    populate_eight_pulses(&engine, &base);
    for (i = 0u; i < 8u; i++)
        engine.voice[i].osc.pulse.phase = 0xF0000000u;
    assert(fmt_8bit_engine_render(&engine) == -32767);
}

static uint32_t fnv_byte(uint32_t h, uint8_t value)
{
    h ^= value;
    return h * 16777619u;
}

static uint32_t gate2_replay_hash(void)
{
    Fmt8BitEngine engine;
    Fmt8BitParamSet base = instant_base(2u);
    uint32_t h = 2166136261u;
    uint32_t frame;
    uint32_t page[4];

    page[0] = default_instant_page(FMT_8BIT_ENGINE_PULSE, 2u);
    page[1] = default_instant_page(FMT_8BIT_ENGINE_NOISE, 2u);
    page[2] = default_instant_page(FMT_8BIT_ENGINE_TRIANGLE, 2u);
    page[3] = default_instant_page(FMT_8BIT_ENGINE_WAVETABLE8, 2u);

    fmt_8bit_engine_reset(&engine);
    assert(fmt_8bit_engine_note(&engine, 0u, FMT_8BIT_ENGINE_PULSE, 0u,
                                C4_Q7, FULL_GAIN, page[0], &base) == 0u);
    assert(fmt_8bit_engine_note(&engine, 1u, FMT_8BIT_ENGINE_NOISE, 1u,
                                E4_Q7, 28000u, page[1], &base) == 1u);
    assert(fmt_8bit_engine_note(&engine, 2u, FMT_8BIT_ENGINE_TRIANGLE, 2u,
                                G4_Q7, 24000u, page[2], &base) == 2u);
    assert(fmt_8bit_engine_note(&engine, 3u, FMT_8BIT_ENGINE_WAVETABLE8, 3u,
                                C5_Q7, 20000u, page[3], &base) == 3u);

    for (frame = 0u; frame < 256u; frame++) {
        int16_t sample;
        uint16_t bits;

        if (frame == 32u) {
            page[0] = page_for(FMT_8BIT_ENGINE_PULSE, 220u,
                               0u, 255u, 2u);
            assert(fmt_8bit_engine_update_row(&engine, 0u, C4_Q7,
                                              FULL_GAIN, page[0]) == 0u);
        } else if (frame == 64u) {
            assert(fmt_8bit_engine_retrigger(&engine, 1u, E4_Q7, 28000u,
                                             page[1], &base) == 1u);
        } else if (frame == 96u) {
            assert(fmt_8bit_engine_release(&engine, 2u, 24000u,
                                           page[2]) == 2u);
        } else if (frame == 128u) {
            page[3] = page_for(FMT_8BIT_ENGINE_WAVETABLE8, 192u,
                               0u, 255u, 2u);
            assert(fmt_8bit_engine_note(&engine, 3u,
                                        FMT_8BIT_ENGINE_WAVETABLE8, 3u,
                                        C5_Q7, 20000u, page[3], &base) == 3u);
        }

        sample = fmt_8bit_engine_render(&engine);
        bits = (uint16_t)sample;
        h = fnv_byte(h, (uint8_t)bits);
        h = fnv_byte(h, (uint8_t)(bits >> 8));
    }

    for (frame = 0u; frame < 8u; frame++) {
        h = fnv_byte(h, engine.allocator.voice_owner[frame]);
        h = fnv_byte(h, engine.voice[frame].envelope.stage);
        h = fnv_byte(h, engine.voice[frame].engine);
    }
    return h;
}

int main(void)
{
    uint32_t replay_hash;

    test_layout_and_reset();
    test_attack_source_and_articulation_snapshot();
    test_note_reuse_identity_and_retrigger();
    test_live_row_update_does_not_retime();
    test_release_tail_detach_steal_and_quiet();
    test_malformed_note_fails_to_track_silence();
    test_eight_voice_mix_clamp();

    replay_hash = gate2_replay_hash();
    printf("WP4_9_GATE2_REPLAY_FNV1A32=%08x\n", replay_hash);
    printf("WP4_9_GATE2_TESTS_OK=1\n");
    printf("osc_state_bytes=8 voice_bytes=44 engine_bytes=404 slots=8 mix_clamp=32767 identity_reset=1 phase_reuse=1 retrig_reset=1 release_tail=1 malformed_silence=1\n");
    return 0;
}
