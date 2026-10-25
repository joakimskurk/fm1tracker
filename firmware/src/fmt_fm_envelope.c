/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_fm_envelope.h"

#include "../gen/fmt_fm_envelope_step_q15_16.inc"

static uint16_t fmt_fm_envelope_clamp_q15(uint32_t level_q15)
{
    if (level_q15 > FMT_FM_ENVELOPE_LEVEL_MAX_Q15)
        return (uint16_t)FMT_FM_ENVELOPE_LEVEL_MAX_Q15;
    return (uint16_t)level_q15;
}

static uint32_t fmt_fm_envelope_clamp_level_q15_16(uint32_t level_q15_16)
{
    if (level_q15_16 > FMT_FM_ENVELOPE_LEVEL_MAX_Q15_16)
        return FMT_FM_ENVELOPE_LEVEL_MAX_Q15_16;
    return level_q15_16;
}

uint8_t fmt_fm_envelope_control_sanitize(uint32_t control)
{
    if (control > FMT_FM_ENVELOPE_CONTROL_MAX)
        return (uint8_t)FMT_FM_ENVELOPE_CONTROL_MAX;
    return (uint8_t)control;
}

uint16_t fmt_fm_envelope_sustain_q15(uint32_t control)
{
    uint16_t v = fmt_fm_envelope_control_sanitize(control);
    return (uint16_t)((v << 7) | (v >> 1));
}

uint32_t fmt_fm_envelope_time_step_q15_16(uint32_t control)
{
    return fmt_fm_envelope_step_q15_16[
        fmt_fm_envelope_control_sanitize(control)];
}

void fmt_fm_envelope_reset(FmtFmEnvelopeState *state)
{
    if (!state)
        return;

    state->level_q15_16[0] = 0u;
    state->level_q15_16[1] = 0u;
    state->level_q15_16[2] = 0u;
    state->level_q15_16[3] = 0u;
    state->stage[0] = FMT_FM_ENVELOPE_STAGE_OFF;
    state->stage[1] = FMT_FM_ENVELOPE_STAGE_OFF;
    state->stage[2] = FMT_FM_ENVELOPE_STAGE_OFF;
    state->stage[3] = FMT_FM_ENVELOPE_STAGE_OFF;
}

static void fmt_fm_envelope_retrigger_one(FmtFmEnvelopeState *state,
                                          const FmtFmEnvelopeControls *controls,
                                          uint8_t operator_index)
{
    uint8_t attack = controls->attack[operator_index];
    uint8_t decay = controls->decay[operator_index];

    if (attack != 0u) {
        state->level_q15_16[operator_index] = 0u;
        state->stage[operator_index] = FMT_FM_ENVELOPE_STAGE_ATTACK;
        return;
    }

    if (decay != 0u) {
        state->level_q15_16[operator_index] =
            FMT_FM_ENVELOPE_LEVEL_MAX_Q15_16;
        state->stage[operator_index] = FMT_FM_ENVELOPE_STAGE_DECAY;
        return;
    }

    state->level_q15_16[operator_index] =
        ((uint32_t)fmt_fm_envelope_sustain_q15(
            controls->sustain[operator_index])) << 16;
    state->stage[operator_index] = FMT_FM_ENVELOPE_STAGE_SUSTAIN;
}

void fmt_fm_envelope_retrigger(FmtFmEnvelopeState *state,
                               const FmtFmEnvelopeControls *controls)
{
    if (!state)
        return;
    if (!controls) {
        fmt_fm_envelope_reset(state);
        return;
    }

    fmt_fm_envelope_retrigger_one(state, controls, 0u);
    fmt_fm_envelope_retrigger_one(state, controls, 1u);
    fmt_fm_envelope_retrigger_one(state, controls, 2u);
    fmt_fm_envelope_retrigger_one(state, controls, 3u);
}

static void fmt_fm_envelope_note_off_one(FmtFmEnvelopeState *state,
                                         const FmtFmEnvelopeControls *controls,
                                         uint8_t operator_index)
{
    uint32_t level = fmt_fm_envelope_clamp_level_q15_16(
        state->level_q15_16[operator_index]);
    uint8_t stage = state->stage[operator_index];
    uint8_t release = controls->release[operator_index];

    if (stage > FMT_FM_ENVELOPE_STAGE_RELEASE ||
        stage == FMT_FM_ENVELOPE_STAGE_OFF ||
        level == 0u || release == 0u) {
        state->level_q15_16[operator_index] = 0u;
        state->stage[operator_index] = FMT_FM_ENVELOPE_STAGE_OFF;
        return;
    }

    state->level_q15_16[operator_index] = level;
    state->stage[operator_index] = FMT_FM_ENVELOPE_STAGE_RELEASE;
}

void fmt_fm_envelope_note_off(FmtFmEnvelopeState *state,
                              const FmtFmEnvelopeControls *controls)
{
    if (!state)
        return;
    if (!controls) {
        fmt_fm_envelope_reset(state);
        return;
    }

    fmt_fm_envelope_note_off_one(state, controls, 0u);
    fmt_fm_envelope_note_off_one(state, controls, 1u);
    fmt_fm_envelope_note_off_one(state, controls, 2u);
    fmt_fm_envelope_note_off_one(state, controls, 3u);
}

uint16_t fmt_fm_envelope_level_q15(const FmtFmEnvelopeState *state,
                                   uint32_t operator_index)
{
    uint32_t level;

    if (!state || operator_index >= FMT_FM_ENVELOPE_OPERATOR_COUNT)
        return 0u;

    level = fmt_fm_envelope_clamp_level_q15_16(
        state->level_q15_16[operator_index]);
    return (uint16_t)(level >> 16);
}

uint16_t fmt_fm_envelope_effective_level_q15(uint32_t static_level_q15,
                                             uint32_t envelope_level_q15)
{
    uint32_t static_level = fmt_fm_envelope_clamp_q15(static_level_q15);
    uint32_t envelope_level = fmt_fm_envelope_clamp_q15(envelope_level_q15);

    if (static_level == 0u || envelope_level == 0u)
        return 0u;
    if (envelope_level == FMT_FM_ENVELOPE_LEVEL_MAX_Q15)
        return (uint16_t)static_level;

    return (uint16_t)((static_level * envelope_level + 16384u) >> 15);
}

void fmt_fm_envelope_map_levels(const FmtFmEnvelopeState *state,
                                const uint16_t static_level_q15[4],
                                uint16_t out_level_q15[4])
{
    if (!out_level_q15)
        return;

    if (!state || !static_level_q15) {
        out_level_q15[0] = 0u;
        out_level_q15[1] = 0u;
        out_level_q15[2] = 0u;
        out_level_q15[3] = 0u;
        return;
    }

    out_level_q15[0] = fmt_fm_envelope_effective_level_q15(
        static_level_q15[0], fmt_fm_envelope_level_q15(state, 0u));
    out_level_q15[1] = fmt_fm_envelope_effective_level_q15(
        static_level_q15[1], fmt_fm_envelope_level_q15(state, 1u));
    out_level_q15[2] = fmt_fm_envelope_effective_level_q15(
        static_level_q15[2], fmt_fm_envelope_level_q15(state, 2u));
    out_level_q15[3] = fmt_fm_envelope_effective_level_q15(
        static_level_q15[3], fmt_fm_envelope_level_q15(state, 3u));
}

static void fmt_fm_envelope_advance_one(FmtFmEnvelopeState *state,
                                        const FmtFmEnvelopeControls *controls,
                                        uint8_t operator_index)
{
    uint32_t level = fmt_fm_envelope_clamp_level_q15_16(
        state->level_q15_16[operator_index]);
    uint32_t target = ((uint32_t)fmt_fm_envelope_sustain_q15(
        controls->sustain[operator_index])) << 16;
    uint32_t step;
    uint8_t stage = state->stage[operator_index];

    if (stage == FMT_FM_ENVELOPE_STAGE_OFF) {
        level = 0u;
    } else if (stage == FMT_FM_ENVELOPE_STAGE_ATTACK) {
        if (controls->attack[operator_index] == 0u) {
            level = FMT_FM_ENVELOPE_LEVEL_MAX_Q15_16;
            stage = FMT_FM_ENVELOPE_STAGE_DECAY;
        } else {
            step = fmt_fm_envelope_step_q15_16[
                controls->attack[operator_index]];
            if (level >= FMT_FM_ENVELOPE_LEVEL_MAX_Q15_16 - step) {
                level = FMT_FM_ENVELOPE_LEVEL_MAX_Q15_16;
                stage = FMT_FM_ENVELOPE_STAGE_DECAY;
            } else {
                level += step;
            }
        }
    } else if (stage == FMT_FM_ENVELOPE_STAGE_DECAY) {
        if (level <= target || controls->decay[operator_index] == 0u) {
            level = target;
            stage = FMT_FM_ENVELOPE_STAGE_SUSTAIN;
        } else {
            step = fmt_fm_envelope_step_q15_16[
                controls->decay[operator_index]];
            if (level - target <= step) {
                level = target;
                stage = FMT_FM_ENVELOPE_STAGE_SUSTAIN;
            } else {
                level -= step;
            }
        }
    } else if (stage == FMT_FM_ENVELOPE_STAGE_SUSTAIN) {
        level = target;
    } else if (stage == FMT_FM_ENVELOPE_STAGE_RELEASE) {
        if (controls->release[operator_index] == 0u || level == 0u) {
            level = 0u;
            stage = FMT_FM_ENVELOPE_STAGE_OFF;
        } else {
            step = fmt_fm_envelope_step_q15_16[
                controls->release[operator_index]];
            if (level <= step) {
                level = 0u;
                stage = FMT_FM_ENVELOPE_STAGE_OFF;
            } else {
                level -= step;
            }
        }
    } else {
        level = 0u;
        stage = FMT_FM_ENVELOPE_STAGE_OFF;
    }

    state->level_q15_16[operator_index] = level;
    state->stage[operator_index] = stage;
}

void fmt_fm_envelope_advance(FmtFmEnvelopeState *state,
                             const FmtFmEnvelopeControls *controls)
{
    if (!state)
        return;
    if (!controls) {
        fmt_fm_envelope_reset(state);
        return;
    }

    fmt_fm_envelope_advance_one(state, controls, 0u);
    fmt_fm_envelope_advance_one(state, controls, 1u);
    fmt_fm_envelope_advance_one(state, controls, 2u);
    fmt_fm_envelope_advance_one(state, controls, 3u);
}
