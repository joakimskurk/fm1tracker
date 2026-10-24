/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_playback_params.h"
#include "fmt_store.h"

extern FmtLock fmt_lock_store_storage[FMT_LOCK_POOL_MAX];

static FmtSong song_before;
static FmtLock locks_before[FMT_LOCK_POOL_MAX];

static void reset_all(void)
{
    fmt_store_reset();
    fmt_lock_store_reset();
    fmt_playback_params_reset();
}

static void set_lock(uint32_t step, FmtParamId param, uint16_t value)
{
    uint8_t replaced = 0u;
    assert(fmt_lock_set(FMT_PLAYBACK_PATTERN,
                        FMT_PLAYBACK_TRACK,
                        step,
                        param,
                        value,
                        &replaced) == FMT_LOCK_STATUS_OK);
}

static void test_layout_defaults_and_pack(void)
{
    uint32_t step;
    uint32_t word;

    reset_all();
    assert(sizeof(fmt_playback_param_cache) == 256u);
    assert(((uintptr_t)&fmt_playback_param_cache[0] & 3u) == 0u);
    assert(sizeof(FmtPlaybackParamVoice) == 6u);

    word = fmt_playback_params_default_word();
    assert(word == 0x00FF80FFu);
    assert(fmt_playback_params_voice(word) == 255u);
    assert(fmt_playback_params_fine(word) == 128u);
    assert(fmt_playback_params_track(word) == 255u);
    assert(fmt_playback_params_flags(word) == 0u);

    for (step = 0u; step < FMT_PLAYBACK_PARAM_ROWS; step++)
        assert(fmt_playback_params_read(step) == word);
    assert(fmt_playback_params_read(FMT_PLAYBACK_PARAM_ROWS) == word);
    assert(!fmt_playback_params_refresh_step(FMT_PLAYBACK_PARAM_ROWS));
    assert(!fmt_playback_params_refresh_step(UINT32_MAX));
}

static void test_all_rows_and_accepted_params(void)
{
    uint32_t step, word;

    reset_all();
    for (step = 0u; step < FMT_PLAYBACK_PARAM_ROWS; step++)
        set_lock(step, (FmtParamId)FMT_PARAM_VOICE_LEVEL, (uint16_t)step);

    fmt_playback_params_rebuild();
    for (step = 0u; step < FMT_PLAYBACK_PARAM_ROWS; step++) {
        word = fmt_playback_params_read(step);
        assert(fmt_playback_params_voice(word) == step);
        assert(fmt_playback_params_fine(word) == 128u);
        assert(fmt_playback_params_track(word) == 255u);
        assert(fmt_playback_params_flags(word) == 0u);
    }

    reset_all();
    set_lock(7u, (FmtParamId)FMT_PARAM_TRACK_PAN, 0u);
    set_lock(7u, (FmtParamId)FMT_PARAM_TRACK_LEVEL, 128u);
    set_lock(7u, (FmtParamId)FMT_PARAM_VOICE_LEVEL, 64u);
    set_lock(7u, (FmtParamId)FMT_PARAM_PITCH_FINE, 192u);
    assert(fmt_playback_params_refresh_step(7u));
    word = fmt_playback_params_read(7u);
    assert(fmt_playback_params_voice(word) == 64u);
    assert(fmt_playback_params_fine(word) == 192u);
    assert(fmt_playback_params_track(word) == 128u);
    assert(fmt_playback_params_flags(word) == 0u);
}

static void test_domain_and_structural_fallback(void)
{
    uint32_t word;
    uint16_t head;

    reset_all();
    set_lock(10u, (FmtParamId)FMT_PARAM_VOICE_LEVEL, 500u);
    set_lock(10u, (FmtParamId)FMT_PARAM_PITCH_FINE, 64u);
    set_lock(10u, (FmtParamId)FMT_PARAM_TRACK_LEVEL, 128u);
    assert(fmt_playback_params_refresh_step(10u));
    word = fmt_playback_params_read(10u);
    assert(fmt_playback_params_voice(word) == 255u);
    assert(fmt_playback_params_fine(word) == 64u);
    assert(fmt_playback_params_track(word) == 128u);
    assert(fmt_playback_params_flags(word) == FMT_PLAYBACK_PARAM_FLAG_DOMAIN_FALLBACK);

    reset_all();
    set_lock(11u, (FmtParamId)FMT_PARAM_VOICE_LEVEL, 77u);
    head = fmt_store_step(FMT_PLAYBACK_PATTERN, FMT_PLAYBACK_TRACK, 11u)->lock_head;
    assert(head < FMT_LOCK_POOL_MAX);
    fmt_lock_store_storage[head].owner = 1u;
    assert(fmt_playback_params_refresh_step(11u));
    word = fmt_playback_params_read(11u);
    assert(fmt_playback_params_voice(word) == 255u);
    assert(fmt_playback_params_fine(word) == 128u);
    assert(fmt_playback_params_track(word) == 255u);
    assert(fmt_playback_params_flags(word) == FMT_PLAYBACK_PARAM_FLAG_STRUCTURAL_FALLBACK);
}

static void test_resolver_never_mutates_song_or_locks(void)
{
    reset_all();
    set_lock(3u, (FmtParamId)FMT_PARAM_VOICE_LEVEL, 100u);
    set_lock(3u, (FmtParamId)FMT_PARAM_PITCH_FINE, 140u);
    set_lock(9u, (FmtParamId)FMT_PARAM_TRACK_PAN, 200u);

    memcpy(&song_before, fmt_store_song_const(), sizeof song_before);
    memcpy(locks_before, fmt_lock_store_storage, sizeof locks_before);

    fmt_playback_params_rebuild();
    assert(memcmp(&song_before, fmt_store_song_const(), sizeof song_before) == 0);
    assert(memcmp(locks_before, fmt_lock_store_storage, sizeof locks_before) == 0);

    assert(fmt_playback_params_refresh_step(3u));
    assert(memcmp(&song_before, fmt_store_song_const(), sizeof song_before) == 0);
    assert(memcmp(locks_before, fmt_lock_store_storage, sizeof locks_before) == 0);
}

static void test_integer_mappings(void)
{
    assert(fmt_playback_scale_u8(6000u, 255u) == 6000u);
    assert(fmt_playback_scale_u8(6000u, 0u) == 0u);
    assert(fmt_playback_scale_u8(6000u, 128u) == 3012u);
    assert(fmt_playback_level_target(6000u, 255u, 255u) == 6000u);
    assert(fmt_playback_level_target(6000u, 0u, 255u) == 0u);
    assert(fmt_playback_level_target(6000u, 255u, 0u) == 0u);
    assert(fmt_playback_level_target(6000u, 128u, 128u) == 1512u);

    assert(fmt_playback_pitch_q7(60u, 0u) == 7552u);
    assert(fmt_playback_pitch_q7(60u, 128u) == 7680u);
    assert(fmt_playback_pitch_q7(60u, 255u) == 7807u);
    assert(fmt_playback_pitch_q7(0u, 0u) == 0u);
    assert(fmt_playback_pitch_q7(127u, 255u) == 16383u);

    assert(fmt_playback_interp_inc(1000u, 1128u, 0u) == 1000u);
    assert(fmt_playback_interp_inc(1000u, 1128u, 64u) == 1064u);
    assert(fmt_playback_interp_inc(1000u, 1128u, 127u) == 1127u);
    assert(fmt_playback_interp_inc(1000u, 1000u, 64u) == 1000u);
    assert(fmt_playback_interp_inc(2000u, 1000u, 64u) == 2000u);
}

static void test_row_local_note_tie_empty_off_semantics(void)
{
    FmtPlaybackParamVoice voice;
    uint32_t defaults = fmt_playback_params_default_word();
    uint32_t mod = fmt_playback_params_pack(128u, 192u, 128u, 0u);
    uint32_t mute = fmt_playback_params_pack(0u, 128u, 255u, 0u);
    uint16_t pitch_before_off;

    fmt_playback_param_voice_reset(&voice);
    assert(!voice.active && voice.level_target == 0u);

    fmt_playback_param_voice_apply(&voice,
                                   FMT_PLAYBACK_EVENT_NOTE,
                                   60u,
                                   defaults,
                                   6000u);
    assert(voice.active == 1u);
    assert(voice.base_note == 60u);
    assert(voice.pitch_q7 == 7680u);
    assert(voice.level_target == 6000u);

    fmt_playback_param_voice_apply(&voice,
                                   FMT_PLAYBACK_EVENT_TIE,
                                   FMT_NOTE_TIE,
                                   mod,
                                   6000u);
    assert(voice.active == 1u);
    assert(voice.base_note == 60u);
    assert(voice.pitch_q7 == 7744u);
    assert(voice.level_target == 1512u);

    fmt_playback_param_voice_apply(&voice,
                                   FMT_PLAYBACK_EVENT_NONE,
                                   FMT_NOTE_EMPTY,
                                   mute,
                                   6000u);
    assert(voice.active == 1u);
    assert(voice.pitch_q7 == 7680u);
    assert(voice.level_target == 0u);

    fmt_playback_param_voice_apply(&voice,
                                   FMT_PLAYBACK_EVENT_NONE,
                                   FMT_NOTE_EMPTY,
                                   defaults,
                                   6000u);
    assert(voice.active == 1u);
    assert(voice.level_target == 6000u);

    pitch_before_off = voice.pitch_q7;
    fmt_playback_param_voice_apply(&voice,
                                   FMT_PLAYBACK_EVENT_OFF,
                                   FMT_NOTE_OFF,
                                   mod,
                                   6000u);
    assert(voice.active == 0u);
    assert(voice.level_target == 0u);
    assert(voice.pitch_q7 == pitch_before_off);

    fmt_playback_param_voice_apply(&voice,
                                   FMT_PLAYBACK_EVENT_TIE,
                                   FMT_NOTE_TIE,
                                   defaults,
                                   6000u);
    assert(voice.active == 0u && voice.level_target == 0u);
    fmt_playback_param_voice_apply(&voice,
                                   FMT_PLAYBACK_EVENT_NONE,
                                   FMT_NOTE_EMPTY,
                                   defaults,
                                   6000u);
    assert(voice.active == 0u && voice.level_target == 0u);

    fmt_playback_param_voice_apply(&voice,
                                   FMT_PLAYBACK_EVENT_NOTE,
                                   64u,
                                   mod,
                                   6000u);
    assert(voice.active == 1u);
    assert(voice.base_note == 64u);
    assert(voice.pitch_q7 == 8256u);
    assert(voice.level_target == 1512u);
}

int main(void)
{
    test_layout_defaults_and_pack();
    test_all_rows_and_accepted_params();
    test_domain_and_structural_fallback();
    test_resolver_never_mutates_song_or_locks();
    test_integer_mappings();
    test_row_local_note_tie_empty_off_semantics();

    printf("FMT_PLAYBACK_PARAMS_TESTS_OK=1\n");
    printf("cache_bytes=%zu rows=%u word_bytes=4 defaults=255,128,255 active_params=voice,fine,track pan=storage_only\n",
           sizeof(fmt_playback_param_cache), (unsigned)FMT_PLAYBACK_PARAM_ROWS);
    printf("row_local=1 domain_fallback=1 structural_fallback=1 no_store_mutation=1 voice_state_bytes=%zu\n",
           sizeof(FmtPlaybackParamVoice));
    return 0;
}
