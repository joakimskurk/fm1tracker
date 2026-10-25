/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FMT_FM_ENVELOPE_H
#define FMT_FM_ENVELOPE_H

#include <stdint.h>

#define FMT_FM_ENVELOPE_OPERATOR_COUNT 4u
#define FMT_FM_ENVELOPE_CONTROL_MAX 255u
#define FMT_FM_ENVELOPE_LEVEL_MAX_Q15 32767u
#define FMT_FM_ENVELOPE_LEVEL_MAX_Q15_16 0x7fff0000u

#define FMT_FM_ENVELOPE_STAGE_OFF 0u
#define FMT_FM_ENVELOPE_STAGE_ATTACK 1u
#define FMT_FM_ENVELOPE_STAGE_DECAY 2u
#define FMT_FM_ENVELOPE_STAGE_SUSTAIN 3u
#define FMT_FM_ENVELOPE_STAGE_RELEASE 4u

typedef struct {
    uint32_t level_q15_16[FMT_FM_ENVELOPE_OPERATOR_COUNT];
    uint8_t stage[FMT_FM_ENVELOPE_OPERATOR_COUNT];
} FmtFmEnvelopeState;

typedef struct {
    uint8_t attack[FMT_FM_ENVELOPE_OPERATOR_COUNT];
    uint8_t decay[FMT_FM_ENVELOPE_OPERATOR_COUNT];
    uint8_t sustain[FMT_FM_ENVELOPE_OPERATOR_COUNT];
    uint8_t release[FMT_FM_ENVELOPE_OPERATOR_COUNT];
} FmtFmEnvelopeControls;

typedef char FmtFmEnvelopeState_size_must_be_20[
    sizeof(FmtFmEnvelopeState) == 20u ? 1 : -1];
typedef char FmtFmEnvelopeControls_size_must_be_16[
    sizeof(FmtFmEnvelopeControls) == 16u ? 1 : -1];

uint8_t fmt_fm_envelope_control_sanitize(uint32_t control);
uint16_t fmt_fm_envelope_sustain_q15(uint32_t control);
uint32_t fmt_fm_envelope_time_step_q15_16(uint32_t control);

void fmt_fm_envelope_reset(FmtFmEnvelopeState *state);
void fmt_fm_envelope_retrigger(FmtFmEnvelopeState *state,
                               const FmtFmEnvelopeControls *controls);
void fmt_fm_envelope_note_off(FmtFmEnvelopeState *state,
                              const FmtFmEnvelopeControls *controls);
void fmt_fm_envelope_advance(FmtFmEnvelopeState *state,
                             const FmtFmEnvelopeControls *controls);

uint16_t fmt_fm_envelope_level_q15(const FmtFmEnvelopeState *state,
                                   uint32_t operator_index);
uint16_t fmt_fm_envelope_effective_level_q15(uint32_t static_level_q15,
                                             uint32_t envelope_level_q15);
void fmt_fm_envelope_map_levels(const FmtFmEnvelopeState *state,
                                const uint16_t static_level_q15[4],
                                uint16_t out_level_q15[4]);

#endif
