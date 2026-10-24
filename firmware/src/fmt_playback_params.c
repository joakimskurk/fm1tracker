/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_playback_params.h"

volatile uint32_t fmt_playback_param_cache[FMT_PLAYBACK_PARAM_ROWS]
    __attribute__((aligned(4)));
volatile uint32_t
    fmt_playback_param_cache_extra[FMT_PLAYBACK_PARAM_EXTRA_TRACKS]
                                  [FMT_PLAYBACK_PARAM_ROWS]
    __attribute__((aligned(4)));

FMT_STATIC_ASSERT(playback_param_cache_is_256,
                  sizeof(fmt_playback_param_cache) == 256u);
FMT_STATIC_ASSERT(playback_param_extra_cache_is_1792,
                  sizeof(fmt_playback_param_cache_extra) == 1792u);

static volatile uint32_t *fmt_playback_params_slot(uint32_t track,
                                                   uint32_t step)
{
    if (track >= FMT_TRACKS || step >= FMT_PLAYBACK_PARAM_ROWS)
        return 0;
    if (track == FMT_PLAYBACK_TRACK)
        return &fmt_playback_param_cache[step];
    return &fmt_playback_param_cache_extra[track - 1u][step];
}

uint32_t fmt_playback_params_pack(uint8_t voice_level,
                                  uint8_t fine_pitch,
                                  uint8_t track_level,
                                  uint8_t flags)
{
    return (uint32_t)voice_level |
           ((uint32_t)fine_pitch << 8) |
           ((uint32_t)track_level << 16) |
           ((uint32_t)flags << 24);
}

uint32_t fmt_playback_params_default_word(void)
{
    return fmt_playback_params_pack(FMT_PLAYBACK_PARAM_VOICE_DEFAULT,
                                    FMT_PLAYBACK_PARAM_FINE_DEFAULT,
                                    FMT_PLAYBACK_PARAM_TRACK_DEFAULT,
                                    FMT_PLAYBACK_PARAM_FLAG_NONE);
}

uint8_t fmt_playback_params_voice(uint32_t word)
{
    return (uint8_t)(word & 0xFFu);
}

uint8_t fmt_playback_params_fine(uint32_t word)
{
    return (uint8_t)((word >> 8) & 0xFFu);
}

uint8_t fmt_playback_params_track(uint32_t word)
{
    return (uint8_t)((word >> 16) & 0xFFu);
}

uint8_t fmt_playback_params_flags(uint32_t word)
{
    return (uint8_t)(word >> 24);
}

void fmt_playback_params_reset(void)
{
    uint32_t track, step;
    uint32_t word = fmt_playback_params_default_word();

    for (track = 0u; track < FMT_TRACKS; track++) {
        for (step = 0u; step < FMT_PLAYBACK_PARAM_ROWS; step++) {
            volatile uint32_t *slot = fmt_playback_params_slot(track, step);
            *slot = word;
        }
    }
}

int fmt_playback_params_refresh_track_step(uint32_t track, uint32_t step)
{
    volatile uint32_t *slot;
    uint8_t count = 0u, ordinal;
    uint8_t voice = FMT_PLAYBACK_PARAM_VOICE_DEFAULT;
    uint8_t fine = FMT_PLAYBACK_PARAM_FINE_DEFAULT;
    uint8_t track_level = FMT_PLAYBACK_PARAM_TRACK_DEFAULT;
    uint8_t flags = FMT_PLAYBACK_PARAM_FLAG_NONE;
    FmtLockStatus status;

    slot = fmt_playback_params_slot(track, step);
    if (!slot)
        return 0;

    status = fmt_lock_count(FMT_PLAYBACK_PATTERN,
                            track,
                            step,
                            &count);
    if (status != FMT_LOCK_STATUS_OK) {
        *slot = fmt_playback_params_pack(
            voice, fine, track_level,
            FMT_PLAYBACK_PARAM_FLAG_STRUCTURAL_FALLBACK);
        return 1;
    }

    for (ordinal = 0u; ordinal < count && ordinal < FMT_LOCKS_PER_STEP; ordinal++) {
        FmtParamId param_id = (FmtParamId)FMT_PARAM_INVALID;
        uint16_t value = 0u;

        status = fmt_lock_at(FMT_PLAYBACK_PATTERN,
                             track,
                             step,
                             ordinal,
                             &param_id,
                             &value);
        if (status != FMT_LOCK_STATUS_OK) {
            *slot = fmt_playback_params_pack(
                FMT_PLAYBACK_PARAM_VOICE_DEFAULT,
                FMT_PLAYBACK_PARAM_FINE_DEFAULT,
                FMT_PLAYBACK_PARAM_TRACK_DEFAULT,
                FMT_PLAYBACK_PARAM_FLAG_STRUCTURAL_FALLBACK);
            return 1;
        }

        if (param_id == (FmtParamId)FMT_PARAM_TRACK_PAN)
            continue;

        if (param_id != (FmtParamId)FMT_PARAM_VOICE_LEVEL &&
            param_id != (FmtParamId)FMT_PARAM_PITCH_FINE &&
            param_id != (FmtParamId)FMT_PARAM_TRACK_LEVEL)
            continue;

        if (value > 255u) {
            flags |= FMT_PLAYBACK_PARAM_FLAG_DOMAIN_FALLBACK;
            continue;
        }

        if (param_id == (FmtParamId)FMT_PARAM_VOICE_LEVEL)
            voice = (uint8_t)value;
        else if (param_id == (FmtParamId)FMT_PARAM_PITCH_FINE)
            fine = (uint8_t)value;
        else
            track_level = (uint8_t)value;
    }

    *slot = fmt_playback_params_pack(voice, fine, track_level, flags);
    return 1;
}

int fmt_playback_params_refresh_step(uint32_t step)
{
    return fmt_playback_params_refresh_track_step(FMT_PLAYBACK_TRACK, step);
}

void fmt_playback_params_rebuild_track(uint32_t track)
{
    uint32_t step;

    if (track >= FMT_TRACKS)
        return;
    for (step = 0u; step < FMT_PLAYBACK_PARAM_ROWS; step++)
        (void)fmt_playback_params_refresh_track_step(track, step);
}

void fmt_playback_params_rebuild(void)
{
    uint32_t track;

    for (track = 0u; track < FMT_TRACKS; track++)
        fmt_playback_params_rebuild_track(track);
}

uint32_t fmt_playback_params_read_track(uint32_t track, uint32_t step)
{
    volatile uint32_t *slot = fmt_playback_params_slot(track, step);

    if (!slot)
        return fmt_playback_params_default_word();
    return *slot;
}

uint32_t fmt_playback_params_read(uint32_t step)
{
    return fmt_playback_params_read_track(FMT_PLAYBACK_TRACK, step);
}

uint32_t fmt_playback_scale_u8(uint32_t value, uint8_t scale)
{
    return (value * (uint32_t)scale + 127u) / 255u;
}

uint16_t fmt_playback_level_target(uint16_t base_target,
                                   uint8_t voice_level,
                                   uint8_t track_level)
{
    uint32_t level = fmt_playback_scale_u8(base_target, voice_level);
    level = fmt_playback_scale_u8(level, track_level);
    return (uint16_t)level;
}

uint16_t fmt_playback_pitch_q7(uint8_t note, uint8_t fine_pitch)
{
    int32_t pitch = (int32_t)note * 128 + ((int32_t)fine_pitch - 128);

    if (pitch < 0)
        pitch = 0;
    if (pitch > 16383)
        pitch = 16383;
    return (uint16_t)pitch;
}

uint32_t fmt_playback_interp_inc(uint32_t lo,
                                 uint32_t hi,
                                 uint8_t frac_q7)
{
    uint32_t diff, q, r;

    if (frac_q7 == 0u || hi <= lo)
        return lo;
    if (frac_q7 > 127u)
        frac_q7 = 127u;

    diff = hi - lo;
    q = diff >> 7;
    r = diff & 127u;
    return lo + q * frac_q7 + ((r * frac_q7 + 64u) >> 7);
}

void fmt_playback_param_voice_reset(FmtPlaybackParamVoice *voice)
{
    if (!voice)
        return;
    voice->pitch_q7 = 0u;
    voice->level_target = 0u;
    voice->base_note = 0u;
    voice->active = 0u;
}

void fmt_playback_param_voice_apply(FmtPlaybackParamVoice *voice,
                                    uint8_t event_kind,
                                    uint8_t event_note,
                                    uint32_t row_word,
                                    uint16_t base_target)
{
    uint8_t fine, voice_level, track_level;

    if (!voice)
        return;

    if (event_kind == FMT_PLAYBACK_EVENT_OFF) {
        voice->active = 0u;
        voice->level_target = 0u;
        return;
    }

    if (event_kind == FMT_PLAYBACK_EVENT_NOTE) {
        if (event_note > FMT_NOTE_MAX)
            return;
        voice->base_note = event_note;
        voice->active = 1u;
    } else if (event_kind != FMT_PLAYBACK_EVENT_TIE &&
               event_kind != FMT_PLAYBACK_EVENT_NONE) {
        return;
    }

    if (!voice->active) {
        voice->level_target = 0u;
        return;
    }

    voice_level = fmt_playback_params_voice(row_word);
    fine = fmt_playback_params_fine(row_word);
    track_level = fmt_playback_params_track(row_word);
    voice->pitch_q7 = fmt_playback_pitch_q7(voice->base_note, fine);
    voice->level_target = fmt_playback_level_target(base_target,
                                                    voice_level,
                                                    track_level);
}
