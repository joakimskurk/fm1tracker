/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_TRACKER_EFFECT_H
#define FM1TRACKER_FMT_TRACKER_EFFECT_H

#include <stdint.h>

#include "fmt_playback.h"

#define FMT_TRACKER_EFFECT_ROWS            FMT_STEPS_PER_PATTERN
#define FMT_TRACKER_EFFECT_TICKS_PER_ROW   6u
#define FMT_TRACKER_EFFECT_TICK_PHASE      110250u

enum {
    FMT_TRACKER_EFFECT_NONE = 0x00,
    FMT_TRACKER_EFFECT_ARPEGGIO = 0x01,
    FMT_TRACKER_EFFECT_SLIDE_UP = 0x02,
    FMT_TRACKER_EFFECT_SLIDE_DOWN = 0x03,
    FMT_TRACKER_EFFECT_PORTAMENTO = 0x04,
    FMT_TRACKER_EFFECT_VIBRATO = 0x05,
    FMT_TRACKER_EFFECT_RETRIG = 0x06,
    FMT_TRACKER_EFFECT_NOTE_DELAY = 0x07,
    FMT_TRACKER_EFFECT_NOTE_CUT = 0x08
};

enum {
    FMT_TRACKER_EFFECT_FLAG_NONE = 0u,
    FMT_TRACKER_EFFECT_FLAG_INVALID_COMMAND = 1u
};

typedef struct {
    uint16_t pitch_q7;
    uint16_t porta_target_q7;
    int16_t transient_q7;
    uint8_t command;
    uint8_t value;
    uint8_t tick;
    uint8_t vibrato_phase;
    uint8_t porta_valid;
    uint8_t pitch_valid;
} FmtTrackerEffectState;

FMT_STATIC_ASSERT(tracker_effect_state_max_16,
                  sizeof(FmtTrackerEffectState) <= 16u);
FMT_STATIC_ASSERT(tracker_effect_tick_divides_row_exactly,
                  FMT_TRANSPORT_PHASE_PER_STEP ==
                  FMT_TRACKER_EFFECT_TICK_PHASE * FMT_TRACKER_EFFECT_TICKS_PER_ROW);
FMT_STATIC_ASSERT(tracker_effect_one_tick_max_per_frame,
                  0xFFFFu < FMT_TRACKER_EFFECT_TICK_PHASE);

/* D032/D033: one aligned atomic publication word per proof-lane row. */
extern volatile uint32_t fmt_tracker_effect_cache[FMT_TRACKER_EFFECT_ROWS];

uint8_t fmt_tracker_effect_command_valid(uint8_t command);
uint32_t fmt_tracker_effect_pack(uint8_t command,
                                 uint8_t value,
                                 uint8_t flags);
uint8_t fmt_tracker_effect_command(uint32_t word);
uint8_t fmt_tracker_effect_value(uint32_t word);
uint8_t fmt_tracker_effect_flags(uint32_t word);
uint32_t fmt_tracker_effect_default_word(void);

void fmt_tracker_effect_cache_reset(void);
void fmt_tracker_effect_cache_rebuild(void);
int fmt_tracker_effect_cache_refresh_step(uint32_t step);
uint32_t fmt_tracker_effect_cache_read(uint32_t step);

void fmt_tracker_effect_state_reset(FmtTrackerEffectState *state);
void fmt_tracker_effect_begin_row(FmtTrackerEffectState *state,
                                  uint8_t event_kind,
                                  uint8_t event_note,
                                  uint32_t effect_word);
/* D033 fixed-state late actions. These preserve the current row command/tick
 * and only apply the note-state transition that would otherwise happen at row
 * dispatch. */
void fmt_tracker_effect_apply_delayed_note(FmtTrackerEffectState *state,
                                           uint8_t note);
void fmt_tracker_effect_apply_cut(FmtTrackerEffectState *state);
uint8_t fmt_tracker_effect_tick_due(const FmtTrackerEffectState *state,
                                    uint32_t transport_phase);
uint8_t fmt_tracker_effect_advance_tick(FmtTrackerEffectState *state);
uint16_t fmt_tracker_effect_final_pitch_q7(const FmtTrackerEffectState *state,
                                           uint8_t fine_pitch);
int16_t fmt_tracker_effect_vibrato_triangle(uint8_t phase);

#endif /* FM1TRACKER_FMT_TRACKER_EFFECT_H */
