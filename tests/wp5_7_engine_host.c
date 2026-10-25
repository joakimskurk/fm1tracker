/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_fm_engine.h"

static void test_d050_seam_equivalence(void)
{
    static const uint32_t inc[4] = {
        25480118u, 50960236u, 36030479u, 203840944u
    };
    static const uint16_t level[4] = { 32767u, 12345u, 23456u, 30000u };
    uint32_t algorithm;

    for (algorithm = 0u; algorithm < FMT_FM_ALGORITHM_COUNT; algorithm++) {
        FmtFmAlgorithmState legacy;
        FmtFmAlgorithmState split;
        uint32_t frame;

        fmt_fm_algorithm_reset(&legacy);
        fmt_fm_algorithm_reset(&split);
        for (frame = 0u; frame < 4096u; frame++) {
            int32_t a;
            int32_t b;
            int32_t op4;

            a = fmt_fm_algorithm_render(&legacy,
                                        (uint8_t)algorithm,
                                        inc,
                                        level,
                                        4112u);
            op4 = fmt_fm_operator_render(&split.op[3], inc[3], 0, level[3]);
            b = fmt_fm_algorithm_render_from_op4(&split,
                                                 (uint8_t)algorithm,
                                                 inc,
                                                 level,
                                                 4112u,
                                                 op4);
            assert(a == b);
            assert(memcmp(&legacy, &split, sizeof(legacy)) == 0);
        }
    }
    puts("WP5_7_D050_SEAM_EQUIVALENT_OK=1 algorithms=4 frames_each=4096 op4_advance=once");
}

static void test_engine_shape_and_ownership(void)
{
    FmtFmEngine engine;
    const FmtFmPatch *patch = fmt_fm_engine_default_patch();
    uint32_t track;
    int nonzero = 0;

    assert(sizeof(FmtFmPatch) == 28u);
    assert(sizeof(FmtFmVoice) == 92u);
    assert(sizeof(FmtVoiceAllocator) == 52u);
    assert(sizeof(FmtFmEngine) == 788u);
    assert(patch != 0);
    assert(fmt_fm_engine_supported(FMT_FM_ENGINE_FM4));
    assert(!fmt_fm_engine_supported(1u));

    fmt_fm_engine_reset(&engine);
    assert(fmt_fm_engine_quiet(&engine));

    for (track = 0u; track < 8u; track++) {
        uint8_t voice = fmt_fm_engine_note(&engine,
                                           (uint8_t)track,
                                           FMT_FM_ENGINE_FM4,
                                           (uint8_t)track,
                                           (uint16_t)(7680u + track * 128u),
                                           32767u,
                                           patch);
        assert(voice == track);
        assert(fmt_fm_engine_track_voice(&engine, (uint8_t)track) == voice);
    }

    for (track = 0u; track < 128u; track++) {
        if (fmt_fm_engine_render(&engine) != 0)
            nonzero = 1;
    }
    assert(nonzero);
    assert(!fmt_fm_engine_quiet(&engine));

    assert(fmt_fm_engine_update_row(&engine, 0u, 0u, 8000u, 30000u, patch) == 0u);
    assert(fmt_fm_engine_track_voice(&engine, 0u) == 0u);
    assert(fmt_fm_engine_retrigger(&engine, 0u, 0u, 8000u, 30000u, patch) == 0u);
    assert(fmt_fm_engine_track_voice(&engine, 0u) == 0u);

    assert(fmt_fm_engine_release(&engine, 0u, 30000u, patch) == 0u);
    assert(fmt_fm_engine_track_voice(&engine, 0u) == FMT_VOICE_NONE);
    assert(engine.allocator.voice_owner[0] == FMT_TRACK_NONE);

    /* All other seven slots remain active, so track 0 must reuse the only
     * legal detached release tail rather than steal an active primary. */
    assert(fmt_fm_engine_note(&engine,
                              0u,
                              FMT_FM_ENGINE_FM4,
                              0u,
                              8000u,
                              30000u,
                              patch) == 0u);
    for (track = 1u; track < 8u; track++)
        assert(fmt_fm_engine_track_voice(&engine, (uint8_t)track) == track);

    assert(fmt_fm_engine_silence_track(&engine, 1u) == 1u);
    assert(fmt_fm_engine_track_voice(&engine, 1u) == FMT_VOICE_NONE);
    for (track = 2u; track < 8u; track++)
        assert(fmt_fm_engine_track_voice(&engine, (uint8_t)track) == track);

    /* Malformed engine identity fails toward silence for the affected track. */
    assert(fmt_fm_engine_note(&engine, 2u, 99u, 2u, 8000u, 32767u, patch) == FMT_VOICE_NONE);
    assert(fmt_fm_engine_track_voice(&engine, 2u) == FMT_VOICE_NONE);
    for (track = 3u; track < 8u; track++)
        assert(fmt_fm_engine_track_voice(&engine, (uint8_t)track) == track);

    puts("WP5_7_FM_ENGINE_OWNERSHIP_OK=1 slots=8 active_cross_track_steals=0 release_tail=reused malformed=safe_silence");
}

static void test_retrigger_and_release_state(void)
{
    FmtFmEngine engine;
    const FmtFmPatch *patch = fmt_fm_engine_default_patch();
    uint8_t voice;
    uint32_t i;

    fmt_fm_engine_reset(&engine);
    voice = fmt_fm_engine_note(&engine, 3u, 0u, 3u, 7680u, 32767u, patch);
    assert(voice != FMT_VOICE_NONE);
    for (i = 0u; i < 32u; i++)
        (void)fmt_fm_engine_render(&engine);
    assert(engine.voice[voice].algorithm_state.op[0].phase != 0u ||
           engine.voice[voice].algorithm_state.op[1].phase != 0u);

    assert(fmt_fm_engine_retrigger(&engine, 3u, 3u, 7680u, 32767u, patch) == voice);
    assert(engine.voice[voice].algorithm_state.op[0].phase == 0u);
    assert(engine.voice[voice].algorithm_state.op[1].phase == 0u);
    assert(engine.voice[voice].algorithm_state.op[2].phase == 0u);
    assert(engine.voice[voice].algorithm_state.op[3].phase == 0u);
    assert(engine.voice[voice].feedback_state.previous_op4_q15 == 0);

    assert(fmt_fm_engine_release(&engine, 3u, 32767u, patch) == voice);
    assert(fmt_fm_engine_track_voice(&engine, 3u) == FMT_VOICE_NONE);
    assert(engine.allocator.voice_owner[voice] == FMT_TRACK_NONE);

    puts("WP5_7_FM_ENGINE_EVENT_OK=1 note=hard_retrigger retrig=hard_reset off_cut=detached_release");
}

int main(void)
{
    test_d050_seam_equivalence();
    test_engine_shape_and_ownership();
    test_retrigger_and_release_state();
    puts("WP5_7_FM_ENGINE_HOST_OK=1 d050=compatible engine=788 voice=92 slots=8");
    return 0;
}
