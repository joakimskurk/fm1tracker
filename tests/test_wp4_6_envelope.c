/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_envelope.h"

static const uint32_t expected_rate_base_q16[16] = {
    48804958u, 45689748u, 42948363u, 41296503u,
    39043966u, 37024451u, 35203576u, 34086002u,
    32536639u, 31122002u, 29825252u, 29019164u,
    27888547u, 26842727u, 25872508u, 25263743u
};

static uint32_t expected_rate(uint8_t control)
{
    uint32_t rate;
    if (!control)
        return 0u;
    rate = expected_rate_base_q16[control & 15u] >> (control >> 4);
    return rate ? rate : 1u;
}

static void test_layout_reset_and_domains(void)
{
    FmtEnvelope env;
    uint32_t c;
    uint32_t previous_nonzero = UINT32_MAX;
    uint16_t previous_sustain = 0u;

    assert(sizeof(FmtEnvelopeParams) == 4u);
    assert(sizeof(FmtEnvelope) == 20u);
    assert(FMT_ENVELOPE_IDLE == 0u && FMT_ENVELOPE_ATTACK == 1u);
    assert(FMT_ENVELOPE_DECAY == 2u && FMT_ENVELOPE_SUSTAIN == 3u);
    assert(FMT_ENVELOPE_RELEASE == 4u);
    assert(FMT_ENVELOPE_LEVEL_MAX_Q16 == 0x7FFF0000u);

    memset(&env, 0xA5, sizeof env);
    fmt_envelope_reset(&env);
    assert(env.level_q16 == 0u && env.rate_q16 == 0u);
    assert(env.decay_rate_q16 == 0u && env.sustain_q16 == 0u);
    assert(env.stage == FMT_ENVELOPE_IDLE);
    assert(env.reserved[0] == 0u && env.reserved[1] == 0u && env.reserved[2] == 0u);

    for (c = 0u; c <= 255u; c++) {
        uint32_t actual_rate = fmt_envelope_rate_q16((uint8_t)c);
        uint16_t actual_sustain = fmt_envelope_sustain_q15((uint8_t)c);
        assert(actual_rate == expected_rate((uint8_t)c));
        assert(actual_sustain == (uint16_t)((c * 257u) >> 1));
        if (c == 0u) {
            assert(actual_rate == 0u);
        } else {
            assert(actual_rate > 0u);
            assert(actual_rate <= previous_nonzero);
            previous_nonzero = actual_rate;
            assert(actual_sustain >= previous_sustain);
        }
        previous_sustain = actual_sustain;
    }

    assert(fmt_envelope_rate_q16(1u) == 45689748u);
    assert(fmt_envelope_rate_q16(15u) == 25263743u);
    assert(fmt_envelope_rate_q16(16u) == 24402479u);
    assert(fmt_envelope_rate_q16(24u) == 16268319u);
    assert(fmt_envelope_rate_q16(64u) == 3050309u);
    assert(fmt_envelope_rate_q16(96u) == 762577u);
    assert(fmt_envelope_rate_q16(112u) == 381288u);
    assert(fmt_envelope_rate_q16(255u) == 770u);

    assert(fmt_envelope_sustain_q15(0u) == 0u);
    assert(fmt_envelope_sustain_q15(1u) == 128u);
    assert(fmt_envelope_sustain_q15(64u) == 8224u);
    assert(fmt_envelope_sustain_q15(128u) == 16448u);
    assert(fmt_envelope_sustain_q15(160u) == 20560u);
    assert(fmt_envelope_sustain_q15(192u) == 24672u);
    assert(fmt_envelope_sustain_q15(254u) == 32639u);
    assert(fmt_envelope_sustain_q15(255u) == 32767u);

    fmt_envelope_reset((FmtEnvelope *)0);
    fmt_envelope_note_on((FmtEnvelope *)0, (const FmtEnvelopeParams *)0);
    fmt_envelope_retrigger((FmtEnvelope *)0, (const FmtEnvelopeParams *)0);
    fmt_envelope_gate_off((FmtEnvelope *)0, 1u);
    assert(fmt_envelope_render((FmtEnvelope *)0) == 0u);
}

static void test_zero_time_and_event_semantics(void)
{
    FmtEnvelope env, retrig, unchanged;
    FmtEnvelopeParams params = {0u, 0u, 160u, 0u};
    uint32_t i;
    uint32_t before_release;

    fmt_envelope_reset(&env);
    fmt_envelope_note_on(&env, &params);
    assert(env.stage == FMT_ENVELOPE_SUSTAIN);
    assert(env.level_q16 == ((uint32_t)20560u << 16));
    assert(fmt_envelope_render(&env) == 20560u);
    fmt_envelope_gate_off(&env, 0u);
    assert(env.stage == FMT_ENVELOPE_IDLE && env.level_q16 == 0u);

    params.decay = 1u;
    fmt_envelope_note_on(&env, &params);
    assert(env.stage == FMT_ENVELOPE_DECAY);
    assert(env.level_q16 == FMT_ENVELOPE_LEVEL_MAX_Q16);

    params.sustain = 255u;
    fmt_envelope_note_on(&env, &params);
    assert(env.stage == FMT_ENVELOPE_SUSTAIN);
    assert(env.level_q16 == FMT_ENVELOPE_LEVEL_MAX_Q16);

    params.attack = 64u;
    params.decay = 96u;
    params.sustain = 160u;
    params.release = 112u;
    fmt_envelope_note_on(&env, &params);
    for (i = 0u; i < 123u; i++)
        (void)fmt_envelope_render(&env);
    assert(env.level_q16 > 0u);
    unchanged = env;
    assert(memcmp(&env, &unchanged, sizeof env) == 0);

    fmt_envelope_note_on(&env, &params);
    fmt_envelope_reset(&retrig);
    fmt_envelope_retrigger(&retrig, &params);
    assert(memcmp(&env, &retrig, sizeof env) == 0);
    assert(env.level_q16 == 0u && env.stage == FMT_ENVELOPE_ATTACK);

    for (i = 0u; i < 200u; i++)
        (void)fmt_envelope_render(&env);
    before_release = env.level_q16;
    fmt_envelope_gate_off(&env, params.release);
    assert(env.stage == FMT_ENVELOPE_RELEASE);
    assert(env.level_q16 == before_release);
    (void)fmt_envelope_render(&env);
    assert(env.level_q16 < before_release);
}

static void test_stage_monotonicity_and_default_timing(void)
{
    FmtEnvelope env;
    FmtEnvelopeParams params = {64u, 96u, 160u, 112u};
    uint16_t previous = 0u;
    uint32_t i;
    uint32_t attack_to_decay = 0u;
    uint32_t decay_to_sustain = 0u;
    uint32_t release_to_idle = 0u;

    fmt_envelope_reset(&env);
    fmt_envelope_note_on(&env, &params);
    for (i = 1u; i <= 4096u; i++) {
        uint8_t before = env.stage;
        uint16_t current = fmt_envelope_render(&env);
        if (before == FMT_ENVELOPE_ATTACK)
            assert(current >= previous);
        else if (before == FMT_ENVELOPE_DECAY)
            assert(current <= previous);
        if (!attack_to_decay && env.stage == FMT_ENVELOPE_DECAY)
            attack_to_decay = i;
        if (!decay_to_sustain && env.stage == FMT_ENVELOPE_SUSTAIN)
            decay_to_sustain = i;
        previous = current;
    }
    assert(attack_to_decay == 705u);
    assert(decay_to_sustain == 1755u);
    assert(env.stage == FMT_ENVELOPE_SUSTAIN);
    assert(env.level_q16 == ((uint32_t)20560u << 16));

    fmt_envelope_gate_off(&env, params.release);
    previous = 20560u;
    for (i = 1u; i <= 4096u; i++) {
        uint16_t current = fmt_envelope_render(&env);
        assert(current <= previous);
        if (!release_to_idle && env.stage == FMT_ENVELOPE_IDLE)
            release_to_idle = i;
        previous = current;
    }
    assert(release_to_idle == 3534u);
    assert(env.level_q16 == 0u && env.stage == FMT_ENVELOPE_IDLE);

    params.attack = FMT_ENVELOPE_DEFAULT_ATTACK;
    params.decay = FMT_ENVELOPE_DEFAULT_DECAY;
    params.sustain = FMT_ENVELOPE_DEFAULT_SUSTAIN;
    params.release = FMT_ENVELOPE_DEFAULT_RELEASE;
    fmt_envelope_reset(&env);
    fmt_envelope_note_on(&env, &params);
    i = 0u;
    while (env.stage != FMT_ENVELOPE_SUSTAIN) {
        (void)fmt_envelope_render(&env);
        assert(++i < 1000u);
    }
    assert(i == 133u);
    assert(env.level_q16 == FMT_ENVELOPE_LEVEL_MAX_Q16);
    fmt_envelope_gate_off(&env, params.release);
    i = 0u;
    while (env.stage != FMT_ENVELOPE_IDLE) {
        (void)fmt_envelope_render(&env);
        assert(++i < 2000u);
    }
    assert(i == 1057u);
}

static uint32_t run_long_sequence(FmtEnvelope *out,
                                  uint32_t *attack_to_decay,
                                  uint32_t *decay_to_sustain,
                                  uint32_t *release_to_idle)
{
    FmtEnvelope env;
    FmtEnvelopeParams params = {64u, 96u, 160u, 112u};
    uint32_t hash = 2166136261u;
    uint32_t i;

    *attack_to_decay = 0u;
    *decay_to_sustain = 0u;
    *release_to_idle = 0u;
    fmt_envelope_reset(&env);
    fmt_envelope_note_on(&env, &params);
    for (i = 1u; i <= 4096u; i++) {
        uint16_t level = fmt_envelope_render(&env);
        if (!*attack_to_decay && env.stage == FMT_ENVELOPE_DECAY)
            *attack_to_decay = i;
        if (!*decay_to_sustain && env.stage == FMT_ENVELOPE_SUSTAIN)
            *decay_to_sustain = i;
        hash ^= (uint32_t)level;
        hash *= 16777619u;
    }
    fmt_envelope_gate_off(&env, params.release);
    for (i = 1u; i <= 4096u; i++) {
        uint16_t level = fmt_envelope_render(&env);
        if (!*release_to_idle && env.stage == FMT_ENVELOPE_IDLE)
            *release_to_idle = i;
        hash ^= (uint32_t)level;
        hash *= 16777619u;
    }
    hash ^= env.level_q16;
    hash *= 16777619u;
    hash ^= (uint32_t)env.stage;
    hash *= 16777619u;
    if (out)
        *out = env;
    return hash;
}

static void test_guards_replay_and_long_hash(void)
{
    struct {
        uint32_t before;
        FmtEnvelope env;
        uint32_t after;
    } guarded;
    FmtEnvelope a, b;
    FmtEnvelopeParams params = {64u, 0u, 255u, 112u};
    uint32_t aa, ad, ar, ba, bd, br;
    uint32_t i;
    uint32_t start_level;
    uint32_t release_frames = 0u;
    uint32_t ha, hb;

    guarded.before = 0x13579BDFu;
    guarded.after = 0x2468ACE0u;
    fmt_envelope_reset(&guarded.env);
    fmt_envelope_note_on(&guarded.env, &params);
    for (i = 0u; i < 353u; i++)
        (void)fmt_envelope_render(&guarded.env);
    start_level = guarded.env.level_q16;
    assert(start_level > 0u && start_level < FMT_ENVELOPE_LEVEL_MAX_Q16);
    fmt_envelope_gate_off(&guarded.env, params.release);
    while (guarded.env.stage != FMT_ENVELOPE_IDLE) {
        uint32_t before = guarded.env.level_q16;
        (void)fmt_envelope_render(&guarded.env);
        assert(guarded.env.level_q16 <= before);
        assert(++release_frames < 5633u);
    }
    assert(guarded.before == 0x13579BDFu && guarded.after == 0x2468ACE0u);

    ha = run_long_sequence(&a, &aa, &ad, &ar);
    hb = run_long_sequence(&b, &ba, &bd, &br);
    assert(ha == 0x40A44A2Eu && hb == ha);
    assert(memcmp(&a, &b, sizeof a) == 0);
    assert(aa == 705u && ad == 1755u && ar == 3534u);
    assert(ba == aa && bd == ad && br == ar);
    assert(a.level_q16 == 0u && a.stage == FMT_ENVELOPE_IDLE);
}

int main(void)
{
    test_layout_reset_and_domains();
    test_zero_time_and_event_semantics();
    test_stage_monotonicity_and_default_timing();
    test_guards_replay_and_long_hash();

    puts("WP4_6_ENVELOPE_TESTS_OK=1");
    puts("params_bytes=4 state_bytes=20 topology=ADSR rates=256 sustain_values=256");
    puts("attack_to_decay=705 decay_to_sustain=1755 release_to_idle=3534 default_attack=133 default_release=1057");
    puts("long_hash=40a44a2e final_level=0 final_stage=IDLE tie_empty=no_call note_retrigger=hard_restart");
    return 0;
}
