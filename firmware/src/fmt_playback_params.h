/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_PLAYBACK_PARAMS_H
#define FM1TRACKER_FMT_PLAYBACK_PARAMS_H

#include <stdint.h>

#include "fmt_lock_store.h"
#include "fmt_playback.h"

#define FMT_PLAYBACK_PARAM_ROWS FMT_STEPS_PER_PATTERN
#define FMT_PLAYBACK_PARAM_EXTRA_TRACKS (FMT_TRACKS - 1u)
#define FMT_PLAYBACK_PARAM_VOICE_DEFAULT 255u
#define FMT_PLAYBACK_PARAM_FINE_DEFAULT 128u
#define FMT_PLAYBACK_PARAM_TRACK_DEFAULT 255u

enum {
    FMT_PLAYBACK_PARAM_FLAG_NONE = 0u,
    FMT_PLAYBACK_PARAM_FLAG_DOMAIN_FALLBACK = 1u,
    FMT_PLAYBACK_PARAM_FLAG_STRUCTURAL_FALLBACK = 2u
};

typedef struct {
    uint16_t pitch_q7;
    uint16_t level_target;
    uint8_t base_note;
    uint8_t active;
} FmtPlaybackParamVoice;

FMT_STATIC_ASSERT(playback_param_voice_size_is_6,
                  sizeof(FmtPlaybackParamVoice) == 6u);

/* D031 track-0 publication remains the exact historical 256 B symbol. D045
 * Gate 1 adds seven separate fixed rows so inherited symbol/API evidence stays
 * meaningful while all eight tracker tracks gain aligned publication words. */
extern volatile uint32_t fmt_playback_param_cache[FMT_PLAYBACK_PARAM_ROWS];
extern volatile uint32_t
    fmt_playback_param_cache_extra[FMT_PLAYBACK_PARAM_EXTRA_TRACKS]
                                  [FMT_PLAYBACK_PARAM_ROWS];

uint32_t fmt_playback_params_pack(uint8_t voice_level,
                                  uint8_t fine_pitch,
                                  uint8_t track_level,
                                  uint8_t flags);
uint32_t fmt_playback_params_default_word(void);
uint8_t fmt_playback_params_voice(uint32_t word);
uint8_t fmt_playback_params_fine(uint32_t word);
uint8_t fmt_playback_params_track(uint32_t word);
uint8_t fmt_playback_params_flags(uint32_t word);

void fmt_playback_params_reset(void);
void fmt_playback_params_rebuild(void);
int fmt_playback_params_refresh_step(uint32_t step);
uint32_t fmt_playback_params_read(uint32_t step);

/* D045 Gate 1 track-aware publication. Legacy helpers above are exact track-0
 * wrappers. Rebuild/refresh are main/editor work; audio performs aligned reads. */
void fmt_playback_params_rebuild_track(uint32_t track);
int fmt_playback_params_refresh_track_step(uint32_t track, uint32_t step);
uint32_t fmt_playback_params_read_track(uint32_t track, uint32_t step);

/* Pure D031 integer mappings. */
uint32_t fmt_playback_scale_u8(uint32_t value, uint8_t scale);
uint16_t fmt_playback_level_target(uint16_t base_target,
                                   uint8_t voice_level,
                                   uint8_t track_level);
uint16_t fmt_playback_pitch_q7(uint8_t note, uint8_t fine_pitch);
uint32_t fmt_playback_interp_inc(uint32_t lo,
                                 uint32_t hi,
                                 uint8_t frac_q7);

void fmt_playback_param_voice_reset(FmtPlaybackParamVoice *voice);
void fmt_playback_param_voice_apply(FmtPlaybackParamVoice *voice,
                                    uint8_t event_kind,
                                    uint8_t event_note,
                                    uint32_t row_word,
                                    uint16_t base_target);

#endif /* FM1TRACKER_FMT_PLAYBACK_PARAMS_H */
