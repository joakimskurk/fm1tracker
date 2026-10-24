/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_envelope.h"

#include <stdint.h>

static const uint32_t fmt_envelope_rate_base_q16[16] = {
    48804958u, 45689748u, 42948363u, 41296503u,
    39043966u, 37024451u, 35203576u, 34086002u,
    32536639u, 31122002u, 29825252u, 29019164u,
    27888547u, 26842727u, 25872508u, 25263743u
};

static void fmt_envelope_clear_reserved(FmtEnvelope *env)
{
    env->reserved[0] = 0u;
    env->reserved[1] = 0u;
    env->reserved[2] = 0u;
}

void fmt_envelope_reset(FmtEnvelope *env)
{
    if (!env)
        return;
    env->level_q16 = 0u;
    env->rate_q16 = 0u;
    env->decay_rate_q16 = 0u;
    env->sustain_q16 = 0u;
    env->stage = FMT_ENVELOPE_IDLE;
    fmt_envelope_clear_reserved(env);
}

uint32_t fmt_envelope_rate_q16(uint8_t control)
{
    uint32_t rate;

    if (!control)
        return 0u;

    rate = fmt_envelope_rate_base_q16[control & 15u] >> (control >> 4);
    return rate ? rate : 1u;
}

uint16_t fmt_envelope_sustain_q15(uint8_t sustain)
{
    return (uint16_t)(((uint32_t)sustain * 257u) >> 1);
}

void fmt_envelope_note_on(FmtEnvelope *env, const FmtEnvelopeParams *params)
{
    uint32_t sustain_q15;

    if (!env || !params)
        return;

    sustain_q15 = (uint32_t)fmt_envelope_sustain_q15(params->sustain);
    env->level_q16 = 0u;
    env->sustain_q16 = sustain_q15 << 16;
    env->decay_rate_q16 = fmt_envelope_rate_q16(params->decay);
    env->rate_q16 = 0u;
    fmt_envelope_clear_reserved(env);

    if (params->attack == 0u) {
        env->level_q16 = FMT_ENVELOPE_LEVEL_MAX_Q16;
        if (env->decay_rate_q16 == 0u ||
            env->sustain_q16 == FMT_ENVELOPE_LEVEL_MAX_Q16) {
            env->level_q16 = env->sustain_q16;
            env->stage = FMT_ENVELOPE_SUSTAIN;
        } else {
            env->rate_q16 = env->decay_rate_q16;
            env->stage = FMT_ENVELOPE_DECAY;
        }
        return;
    }

    env->rate_q16 = fmt_envelope_rate_q16(params->attack);
    env->stage = FMT_ENVELOPE_ATTACK;
}

void fmt_envelope_retrigger(FmtEnvelope *env, const FmtEnvelopeParams *params)
{
    fmt_envelope_note_on(env, params);
}

void fmt_envelope_gate_off(FmtEnvelope *env, uint8_t release_control)
{
    uint32_t release_rate;

    if (!env)
        return;

    if (env->stage == FMT_ENVELOPE_IDLE || env->level_q16 == 0u) {
        env->level_q16 = 0u;
        env->rate_q16 = 0u;
        env->stage = FMT_ENVELOPE_IDLE;
        return;
    }

    release_rate = fmt_envelope_rate_q16(release_control);
    if (release_rate == 0u) {
        env->level_q16 = 0u;
        env->rate_q16 = 0u;
        env->stage = FMT_ENVELOPE_IDLE;
        return;
    }

    env->rate_q16 = release_rate;
    env->stage = FMT_ENVELOPE_RELEASE;
}

uint16_t fmt_envelope_render(FmtEnvelope *env)
{
    if (!env)
        return 0u;

    if (env->stage == FMT_ENVELOPE_ATTACK) {
        if (env->level_q16 >= FMT_ENVELOPE_LEVEL_MAX_Q16 - env->rate_q16) {
            env->level_q16 = FMT_ENVELOPE_LEVEL_MAX_Q16;
            if (env->decay_rate_q16 == 0u ||
                env->sustain_q16 == FMT_ENVELOPE_LEVEL_MAX_Q16) {
                env->level_q16 = env->sustain_q16;
                env->rate_q16 = 0u;
                env->stage = FMT_ENVELOPE_SUSTAIN;
            } else {
                env->rate_q16 = env->decay_rate_q16;
                env->stage = FMT_ENVELOPE_DECAY;
            }
        } else {
            env->level_q16 += env->rate_q16;
        }
    } else if (env->stage == FMT_ENVELOPE_DECAY) {
        if (env->rate_q16 == 0u ||
            env->level_q16 <= env->sustain_q16 + env->rate_q16) {
            env->level_q16 = env->sustain_q16;
            env->rate_q16 = 0u;
            env->stage = FMT_ENVELOPE_SUSTAIN;
        } else {
            env->level_q16 -= env->rate_q16;
        }
    } else if (env->stage == FMT_ENVELOPE_RELEASE) {
        if (env->rate_q16 == 0u || env->level_q16 <= env->rate_q16) {
            env->level_q16 = 0u;
            env->rate_q16 = 0u;
            env->stage = FMT_ENVELOPE_IDLE;
        } else {
            env->level_q16 -= env->rate_q16;
        }
    } else if (env->stage == FMT_ENVELOPE_IDLE) {
        env->level_q16 = 0u;
        env->rate_q16 = 0u;
    }

    return (uint16_t)(env->level_q16 >> 16);
}
