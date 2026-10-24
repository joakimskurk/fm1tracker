/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FMT_ENVELOPE_H
#define FMT_ENVELOPE_H

#include <stdint.h>

#define FMT_ENVELOPE_IDLE 0u
#define FMT_ENVELOPE_ATTACK 1u
#define FMT_ENVELOPE_DECAY 2u
#define FMT_ENVELOPE_SUSTAIN 3u
#define FMT_ENVELOPE_RELEASE 4u

#define FMT_ENVELOPE_LEVEL_MAX_Q16 0x7FFF0000u

#define FMT_ENVELOPE_DEFAULT_ATTACK 24u
#define FMT_ENVELOPE_DEFAULT_DECAY 0u
#define FMT_ENVELOPE_DEFAULT_SUSTAIN 255u
#define FMT_ENVELOPE_DEFAULT_RELEASE 72u

typedef struct {
    uint8_t attack;
    uint8_t decay;
    uint8_t sustain;
    uint8_t release;
} FmtEnvelopeParams;

typedef struct {
    uint32_t level_q16;
    uint32_t rate_q16;
    uint32_t decay_rate_q16;
    uint32_t sustain_q16;
    uint8_t stage;
    uint8_t reserved[3];
} FmtEnvelope;

/* D042 common linear ADSR primitive. */
void fmt_envelope_reset(FmtEnvelope *env);
uint32_t fmt_envelope_rate_q16(uint8_t control);
uint16_t fmt_envelope_sustain_q15(uint8_t sustain);
void fmt_envelope_note_on(FmtEnvelope *env, const FmtEnvelopeParams *params);
void fmt_envelope_retrigger(FmtEnvelope *env, const FmtEnvelopeParams *params);
void fmt_envelope_gate_off(FmtEnvelope *env, uint8_t release_control);
uint16_t fmt_envelope_render(FmtEnvelope *env);

#endif
