/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_TRACKER_EFFECTS_H
#define FM1TRACKER_FMT_TRACKER_EFFECTS_H

#include <stdint.h>

#include "fmt_playback_params.h"

#define FMT_TRACKER_EFFECT_ROWS FMT_STEPS_PER_PATTERN
#define FMT_TRACKER_EFFECT_TICK_PHASE 110250u

enum {
    FMT_TRACKER_EFFECT_NONE = FMT_EFFECT_NONE,
    FMT_TRACKER_EFFECT_ARPEGGIO = 0x01u,
    FMT_TRACKER_EFFECT_SLIDE_UP = 0x02u,
    FMT_TRACKER_EFFECT_SLIDE_DOWN = 0x03u,
    FMT_TRACKER_EFFECT_PORTAMENTO = 0x04u,
    FMT_TRACKER_EFFECT_VIBRATO = 0x05u
};

enum {
    FMT_TRACKER_EFFECT_FLAG_NONE = 0u,
    FMT_TRACKER_EFFECT_FLAG_INVALID_COMMAND = 1u
};

typedef struct {
    uint16_t persistent_pitch_q7;
    uint16_t portamento_target_q7;
    int16_t transient_mod_q7;
    uint8_t command;
    uint8_t value;
    uint8_t tick;
    uint8_t vibrato_phase;
    uint8_t portamento_valid;
    uint8_t fine_value;
} FmtTrackerEffectState;

FMT_STATIC_ASSERT(tracker_effect_state_fits_16,
                  sizeof(FmtTrackerEffectState) <= 16u);

/* D032: one naturally aligned atomic publication word per proof-lane row. */
extern volatile uint32_t fmt_tracker_effect_cache[FMT_TRACKER_EFFECT_ROWS];

uint32_t fmt_tracker_effect_pack(uint8_t command,
                                 uint8_t value,
                                 uint8_t flags);
uint8_t fmt_tracker_effect_command(uint32_t word);
uint8_t fmt_tracker_effect_value(uint32_t word);
uint8_t fmt_tracker_effect_flags(uint32_t word);
uint32_t fmt_tracker_effect_default_word(void);
int fmt_tracker_effect_command_valid(uint8_t command);

void fmt_tracker_effect_cache_reset(void);
void fmt_tracker_effect_cache_rebuild(void);
int fmt_tracker_effect_cache_refresh_step(uint32_t step);
uint32_t fmt_tracker_effect_cache_read(uint32_t step);

void fmt_tracker_effect_state_reset(FmtTrackerEffectState *state);
void fmt_tracker_effect_apply_row(FmtTrackerEffectState *state,
                                  uint8_t event_kind,
                                  uint8_t event_note,
                                  uint32_t effect_word,
                                  uint8_t fine_value,
                                  uint8_t was_active);

/* Call only after the current frame's D020 transport advance and only when no
 * row boundary occurred. Returns 1 iff one of the five internal D032 tick
 * thresholds was crossed and state was advanced. */
int fmt_tracker_effect_advance(FmtTrackerEffectState *state,
                               uint32_t transport_phase,
                               uint8_t note_active);

uint16_t fmt_tracker_effect_final_pitch_q7(const FmtTrackerEffectState *state);
int16_t fmt_tracker_effect_triangle64(uint8_t phase);

#endif /* FM1TRACKER_FMT_TRACKER_EFFECTS_H */
