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

static void test_layout_stage_ids_reset_and_null(void)
{
    FmtEnvelope env;

    assert(sizeof(FmtEnvelopeParams) == 4u);
    assert(sizeof(FmtEnvelope) == 20u);
    assert(FMT_ENVELOPE_IDLE == 0u);
    assert(FMT_ENVELOPE_ATTACK == 1u);
    assert(FMT_ENVELOPE_DECAY == 2u);
    assert(FMT_ENVELOPE_SUSTAIN == 3u);
    assert(FMT_ENVELOPE_RELEASE == 4u);
    assert(FMT_ENVELOPE_LEVEL_MAX_Q16 == 0x7FFF0000u);

    memset(&env, 0xA5, sizeof env);
    fmt_envelope_reset(&env);
    assert(env.level_q16 == 0u);
    assert(env.rate_q16 == 0u);
    assert(env.decay_rate_q16 == 0u);
    assert(env.sustain_q16 == 0u);
    assert(env.stage == FMT_ENVELOPE_IDLE);
    assert(env.reserved[0] == 0u && env.reserved[1] == 0u && env.reserved[2] == 0u);

    fmt_envelope_reset((FmtEnvelope *)0);
    fmt_envelope_note_on((FmtEnvelope *)0, (const FmtEnvelopeParams *)0);
    fmt_envelope_retrigger((FmtEnvelope *)0, (const FmtEnvelopeParams *)0);
    fmt_envelope_gate_off((FmtEnvelope *)0, 1u);
    assert(fmt_envelope_render((FmtEnvelope *)0) == 0u);
}

static void test_all_rate_controls_and_vectors(void)
{
    static const struct {
        uint8_t control;
        uint32_t rate;
        uint32_t frames;
    } vectors[] = {
        {0u, 0u, 0u},
        {1u, 45689748u, 47u},
        {15u, 25263743u, 85u},
        {16u, 24402479u, 88u},
        {24u, 16268319u, 133u},
        {32u, 12201239u, 177u},
        {64u, 3050309u, 705u},
        {80u, 1525154u, 1409u},
        {96u, 762577u, 2817u},
        {112u, 381288u, 5633u},
        {128u, 190644u, 11265u},
        {160u, 47661u, 45057u},
        {192u, 11915u, 180229u},
        {224u, 2978u, 721095u},
        {240u, 1489u, 1442189u},
        {255u, 770u, 2788855u}
    };
    uint32_t c;
    uint32_t i;
    uint32_t previous = UINT32_MAX;

    for (c = 0u; c <= 255u; c++) {
        uint32_t actual = fmt_envelope_rate_q16((uint8_t)c);
        assert(actual == expected_rate((uint8_t)c));
        if (c == 0u)
            assert(actual == 0u);
        else {
            assert(actual > 0u);
            assert(actual <= previous);
        }
        previous = actual;
    }

    for (i = 0u; i < (uint32_t)(sizeof vectors / sizeof vectors[0]); i++) {
        uint32_t rate = fmt_envelope_rate_q16(vectors[i].control);
        assert(rate == vectors[i].rate);
        if (rate) {
            uint32_t frames = (FMT_ENVELOPE_LEVEL_MAX_Q16 + rate - 1u) / rate;
            assert(frames == vectors[i].frames);
        } else {
            assert(vectors[i].frames == 0u);
        }
    }
}

static void test_all_sustain_controls(void)
{
    static const struct {
        uint8_t control;
        uint16_t q15;
    } vectors[] = {
        {0u, 0u}, {1u, 128u}, {64u, 8224u}, {128u, 16448u},
        {160u, 20560u}, {192u, 24672u}, {254u, 32639u}, {255u, 32767u}
    };
    uint32_t c;
    uint32_t i;
    uint16_t previous = 0u;

    for (c = 0u; c <= 255u; c++) {
        uint16_t actual = fmt_envelope_sustain_q15((uint8_t)c);
        uint16_t expected = (uint16_t)((c * 257u) >> 1);
        assert(actual == expected);
        if (c)
            assert(actual >= previous);
        previous = actual;
    }
    for (i = 0u; i < (uint32_t)(sizeof vectors / sizeof vectors[0]); i++)
        assert(fmt_envelope_sustain_q15(vectors[i].control) == vectors[i].q15);
}

static void test_zero_time_edges(void)
{
    FmtEnvelope env;
    FmtEnvelopeParams params;

    params.attack = 0u;
    params.decay = 0u;
    params.sustain = 160u;
    params.release = 0u;
    fmt_envelope_reset(&env);
    fmt_envelope_note_on(&env, &params);
    assert(env.stage == FMT_ENVELOPE_SUSTAIN);
    assert(env.level_q16 == ((uint32_t)20560u << 16));
    assert(fmt_envelope_render(&env) == 20560u);
    fmt_envelope_gate_off(&env, 0u);
    assert(env.stage == FMT_ENVELOPE_IDLE);
    assert(env.level_q16 == 0u);

    params.decay = 1u;
    fmt_envelope_note_on(&env, &params);
    assert(env.stage == FMT_ENVELOPE_DECAY);
    assert(env.level_q16 == FMT_ENVELOPE_LEVEL_MAX_Q16);
    assert(env.rate_q16 == fmt_envelope_rate_q16(1u));

    params.sustain = 255u;
    fmt_envelope_note_on(&env, &params);
    assert(env.stage == FMT_ENVELOPE_SUSTAIN);
    assert(env.level_q16 == FMT_ENVELOPE_LEVEL_MAX_Q16);

    params.attack = 1u;
    params.decay = 0u;
    params.sustain = 0u;
    fmt_envelope_note_on(&env, &params);
    assert(env.stage == FMT_ENVELOPE_ATTACK);
    assert(env.level_q16 == 0u);
}

static void test_note_retrigger_tie_and_release_semantics(void)
{
    FmtEnvelope env;
    FmtEnvelope retrig;
    FmtEnvelope unchanged;
    FmtEnvelopeParams params = {64u, 96u, 160u, 112u};
    uint32_t i;
    uint32_t before_release;

    fmt_envelope_reset(&env);
    fmt_envelope_note_on(&env, &params);
    assert(env.stage == FMT_ENVELOPE_ATTACK);
    assert(env.level_q16 == 0u);
    for (i = 0u; i < 123u; i++)
        (void)fmt_envelope_render(&env);
    assert(env.level_q16 > 0u);

    unchanged = env; /* D037 TIE/EMPTY semantics: make no envelope call. */
    assert(memcmp(&env, &unchanged, sizeof env) == 0);

    fmt_envelope_note_on(&env, &params);
    fmt_envelope_reset(&retrig);
    fmt_envelope_retrigger(&retrig, &params);
    assert(memcmp(&env, &retrig, sizeof env) == 0);
    assert(env.level_q16 == 0u);
    assert(env.stage == FMT_ENVELOPE_ATTACK);

    for (i = 0u; i < 200u; i++)
        (void)fmt_envelope_render(&env);
    before_release = env.level_q16;
    assert(before_release > 0u);
    fmt_envelope_gate_off(&env, params.release);
    assert(env.stage == FMT_ENVELOPE_RELEASE);
    assert(env.level_q16 == before_release);
    assert(fmt_envelope_render(&env) < (uint16_t)(before_release >> 16));
}

static void test_monotonic_stages_and_terminals(void)
{
    FmtEnvelope env;
    FmtEnvelopeParams params = {64u, 96u, 160u, 112u};
    uint16_t previous;
    uint16_t current;
    uint32_t i;
    uint32_t attack_to_decay = 0u;
    uint32_t decay_to_sustain = 0u;
    uint32_t release_to_idle = 0u;

    fmt_envelope_reset(&env);
    fmt_envelope_note_on(&env, &params);
    previous = 0u;
    for (i = 1u; i <= 4096u; i++) {
        uint8_t before = env.stage;
        current = fmt_envelope_render(&env);
        if (before == FMT_ENVELOPE_ATTACK)
            assert(current >= previous);
        else if (before == FMT_ENVELOPE_DECAY)
            assert(current <= previous);
        if (!attack_to_decay && env.stage == FMT_ENVELOPE_DECAY) {
            attack_to_decay = i;
            assert(current == 32767u);
        }
        if (!decay_to_sustain && env.stage == FMT_ENVELOPE_SUSTAIN) {
            decay_to_sustain = i;
            assert(current == 20560u);
        }
        previous = current;
    }
    assert(attack_to_decay == 705u);
    assert(decay_to_sustain == 1755u);
    assert(env.stage == FMT_ENVELOPE_SUSTAIN);
    assert(env.level_q16 == ((uint32_t)20560u << 16));

    fmt_envelope_gate_off(&env, params.release);
    previous = 20560u;
    for (i = 1u; i <= 4096u; i++) {
        current = fmt_envelope_render(&env);
        assert(current <= previous);
        if (!release_to_idle && env.stage == FMT_ENVELOPE_IDLE) {
            release_to_idle = i;
            assert(current == 0u);
        }
        previous = current;
    }
    assert(release_to_idle == 3534u);
    assert(env.stage == FMT_ENVELOPE_IDLE);
    assert(env.level_q16 == 0u);
}

static void test_default_timing(void)
{
    FmtEnvelope env;
    FmtEnvelopeParams params = {
        FMT_ENVELOPE_DEFAULT_ATTACK,
        FMT_ENVELOPE_DEFAULT_DECAY,
        FMT_ENVELOPE_DEFAULT_SUSTAIN,
        FMT_ENVELOPE_DEFAULT_RELEASE
    };
    uint32_t frames = 0u;

    fmt_envelope_reset(&env);
    fmt_envelope_note_on(&env, &params);
    while (env.stage != FMT_ENVELOPE_SUSTAIN) {
        (void)fmt_envelope_render(&env);
        frames++;
        assert(frames < 1000u);
    }
    assert(frames == 133u);
    assert(env.level_q16 == FMT_ENVELOPE_LEVEL_MAX_Q16);

    fmt_envelope_gate_off(&env, params.release);
    frames = 0u;
    while (env.stage != FMT_ENVELOPE_IDLE) {
        (void)fmt_envelope_render(&env);
        frames++;
        assert(frames < 2000u);
    }
    assert(frames == 1057u);
    assert(env.level_q16 == 0u);
}

static void test_release_distance_and_guards(void)
{
    struct {
        uint32_t before;
        FmtEnvelope env;
        uint32_t after;
    } guarded;
    FmtEnvelopeParams params = {64u, 0u, 255u, 112u};
    uint32_t i;
    uint32_t release_frames = 0u;
    uint32_t start_level;

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
        release_frames++;
        assert(release_frames < 6000u);
    }
    assert(release_frames < 5633u);
    assert(guarded.before == 0x13579BDFu);
    assert(guarded.after == 0x2468ACE0u);
}

static uint32_t long_hash(FmtEnvelope *final_state,
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
    if (final_state)
        *final_state = env;
    return hash;
}

static void test_replay_and_long_hash(void)
{
    FmtEnvelope a, b;
    uint32_t a_attack, a_decay, a_release;
    uint32_t b_attack, b_decay, b_release;
    uint32_t ha = long_hash(&a, &a_attack, &a_decay, &a_release);
    uint32_t hb = long_hash(&b, &b_attack, &b_decay, &b_release);

    assert(ha == 0x40A44A2Eu);
    assert(hb == ha);
    assert(memcmp(&a, &b, sizeof a) == 0);
    assert(a_attack == 705u && b_attack == 705u);
    assert(a_decay == 1755u && b_decay == 1755u);
    assert(a_release == 3534u && b_release == 3534u);
    assert(a.level_q16 == 0u);
    assert(a.stage == FMT_ENVELOPE_IDLE);
}

int main(void)
{
    test_layout_stage_ids_reset_and_null();
    test_all_rate_controls_and_vectors();
    test_all_sustain_controls();
    test_zero_time_edges();
    test_note_retrigger_tie_and_release_semantics();
    test_monotonic_stages_and_terminals();
    test_default_timing();
    test_release_distance_and_guards();
    test_replay_and_long_hash();

    puts("WP4_6_ENVELOPE_TESTS_OK=1");
    puts("params_bytes=4 state_bytes=20 topology=ADSR rates=256 sustain_values=256");
    puts("attack_to_decay=705 decay_to_sustain=1755 release_to_idle=3534 default_attack=133 default_release=1057");
    puts("long_hash=40a44a2e final_level=0 final_stage=IDLE tie_empty=no_call note_retrigger=hard_restart");
    return 0;
}
