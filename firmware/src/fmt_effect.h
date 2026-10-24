/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_EFFECT_H
#define FM1TRACKER_FMT_EFFECT_H

#include <stdint.h>

#include "fmt_playback.h"

#define FMT_EFFECT_ROWS            FMT_STEPS_PER_PATTERN
#define FMT_EFFECT_TICKS_PER_ROW   6u
#define FMT_EFFECT_PHASE_PER_TICK  (FMT_TRANSPORT_PHASE_PER_STEP / FMT_EFFECT_TICKS_PER_ROW)

enum {
    FMT_FX_NONE = 0x00u,
    FMT_FX_ARPEGGIO = 0x01u,
    FMT_FX_SLIDE_UP = 0x02u,
    FMT_FX_SLIDE_DOWN = 0x03u,
    FMT_FX_PORTAMENTO = 0x04u,
    FMT_FX_VIBRATO = 0x05u
};

enum {
    FMT_EFFECT_FLAG_NONE = 0u,
    FMT_EFFECT_FLAG_INVALID_COMMAND = 1u
};

typedef struct {
    uint16_t persistent_pitch_q7;
    uint16_t porta_target_q7;
    int16_t transient_q7;
    uint8_t command;
    uint8_t value;
    uint8_t tick;
    uint8_t vibrato_phase;
    uint8_t porta_valid;
    uint8_t note_active;
} FmtEffectState;

FMT_STATIC_ASSERT(effect_state_size_at_most_16, sizeof(FmtEffectState) <= 16u);
FMT_STATIC_ASSERT(effect_phase_divides_row_exactly,
                  FMT_EFFECT_PHASE_PER_TICK * FMT_EFFECT_TICKS_PER_ROW == FMT_TRANSPORT_PHASE_PER_STEP);
FMT_STATIC_ASSERT(effect_max_bpm_crosses_at_most_one_tick,
                  0xFFFFu < FMT_EFFECT_PHASE_PER_TICK);

/* D032: one aligned atomic publication word per pattern-0/track-0 proof row. */
extern volatile uint32_t fmt_effect_cache[FMT_EFFECT_ROWS];

uint8_t fmt_effect_command_valid(uint8_t command);
uint32_t fmt_effect_pack(uint8_t command, uint8_t value, uint8_t flags);
uint32_t fmt_effect_default_word(void);
uint8_t fmt_effect_command(uint32_t word);
uint8_t fmt_effect_value(uint32_t word);
uint8_t fmt_effect_flags(uint32_t word);

void fmt_effect_cache_reset(void);
void fmt_effect_cache_rebuild(void);
int fmt_effect_cache_refresh_step(uint32_t step);
uint32_t fmt_effect_cache_read(uint32_t step);

uint8_t fmt_effect_tick_for_phase(uint32_t phase);
int16_t fmt_effect_vibrato_triangle(uint8_t phase);

void fmt_effect_state_reset(FmtEffectState *state);
void fmt_effect_state_begin_row(FmtEffectState *state,
                                uint8_t event_kind,
                                uint8_t event_note,
                                uint32_t effect_word);
uint8_t fmt_effect_state_advance_phase(FmtEffectState *state, uint32_t phase);
uint16_t fmt_effect_state_final_pitch_q7(const FmtEffectState *state,
                                         uint8_t fine_pitch);

#endif /* FM1TRACKER_FMT_EFFECT_H */
