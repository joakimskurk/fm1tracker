/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_8bit_params.h"
#include "fmt_param_registry.h"

static uint32_t fnv1a_byte(uint32_t hash, uint8_t value)
{
    return (hash ^ value) * 16777619u;
}

static uint32_t fnv1a_word_le(uint32_t hash, uint32_t word)
{
    hash = fnv1a_byte(hash, (uint8_t)word);
    hash = fnv1a_byte(hash, (uint8_t)(word >> 8));
    hash = fnv1a_byte(hash, (uint8_t)(word >> 16));
    hash = fnv1a_byte(hash, (uint8_t)(word >> 24));
    return hash;
}

static uint32_t base_word(uint8_t engine, const Fmt8BitParamSet *base)
{
    uint8_t v[4];
    uint8_t slot;

    for (slot = 0u; slot < 4u; slot++)
        assert(fmt_8bit_page_value_from_set(engine, slot, base, &v[slot]) == 1);
    return fmt_8bit_page_pack(v[0], v[1], v[2], v[3]);
}

static void test_ids_layout_registry_isolation(void)
{
    static const FmtParamId ids[6] = {
        FMT_8BIT_PARAM_ATTACK,
        FMT_8BIT_PARAM_DECAY,
        FMT_8BIT_PARAM_SUSTAIN,
        FMT_8BIT_PARAM_RELEASE,
        FMT_8BIT_PARAM_PULSE_WIDTH,
        FMT_8BIT_PARAM_WAVETABLE
    };
    uint32_t i, j;

    assert(sizeof(Fmt8BitParamSet) == 6u);
    assert(sizeof(Fmt8BitParamLock) == 4u);
    assert(sizeof(Fmt8BitRowParamCache) == 2048u);

    assert(FMT_8BIT_PARAM_ATTACK == 0x20u);
    assert(FMT_8BIT_PARAM_DECAY == 0x21u);
    assert(FMT_8BIT_PARAM_SUSTAIN == 0x22u);
    assert(FMT_8BIT_PARAM_RELEASE == 0x23u);
    assert(FMT_8BIT_PARAM_PULSE_WIDTH == 0x24u);
    assert(FMT_8BIT_PARAM_WAVETABLE == 0x25u);

    for (i = 0u; i < 6u; i++) {
        assert(ids[i] >= FMT_PARAM_8BIT_FIRST && ids[i] <= FMT_PARAM_8BIT_LAST);
        assert(fmt_param_namespace(ids[i]) == FMT_PARAM_NAMESPACE_8BIT);
        /* D044 freezes the IDs but WP4.8 deliberately does not activate the
         * already-linked D027 registry. That migration belongs to WP4.9. */
        assert(fmt_param_lookup(ids[i]) == NULL);
        assert(fmt_param_is_assigned(ids[i]) == 0);
        assert(fmt_param_is_lockable(ids[i]) == 0);
        for (j = i + 1u; j < 6u; j++)
            assert(ids[i] != ids[j]);
    }
}

static void test_defaults_pages_and_pack(void)
{
    Fmt8BitParamSet set;
    uint32_t word;
    uint8_t slot;

    memset(&set, 0xA5, sizeof set);
    fmt_8bit_param_set_defaults(&set);
    assert(set.attack == 24u);
    assert(set.decay == 0u);
    assert(set.sustain == 255u);
    assert(set.release == 72u);
    assert(set.pulse_width == 128u);
    assert(set.wavetable == 0u);
    fmt_8bit_param_set_defaults(NULL);

    assert(fmt_8bit_engine_supported(FMT_8BIT_ENGINE_SAFE) == 0);
    assert(fmt_8bit_engine_supported(FMT_8BIT_ENGINE_PULSE) == 1);
    assert(fmt_8bit_engine_supported(FMT_8BIT_ENGINE_NOISE) == 1);
    assert(fmt_8bit_engine_supported(FMT_8BIT_ENGINE_TRIANGLE) == 1);
    assert(fmt_8bit_engine_supported(FMT_8BIT_ENGINE_WAVETABLE8) == 1);
    assert(fmt_8bit_engine_supported(5u) == 0);
    assert(fmt_8bit_engine_supported(255u) == 0);

    assert(fmt_8bit_page_param(1u, 0u) == FMT_8BIT_PARAM_PULSE_WIDTH);
    assert(fmt_8bit_page_param(1u, 1u) == FMT_8BIT_PARAM_DECAY);
    assert(fmt_8bit_page_param(1u, 2u) == FMT_8BIT_PARAM_SUSTAIN);
    assert(fmt_8bit_page_param(1u, 3u) == FMT_8BIT_PARAM_RELEASE);
    assert(fmt_8bit_page_param(2u, 0u) == FMT_8BIT_PARAM_ATTACK);
    assert(fmt_8bit_page_param(3u, 0u) == FMT_8BIT_PARAM_ATTACK);
    assert(fmt_8bit_page_param(4u, 0u) == FMT_8BIT_PARAM_WAVETABLE);
    assert(fmt_8bit_page_param(0u, 0u) == FMT_PARAM_NONE);
    assert(fmt_8bit_page_param(5u, 0u) == FMT_PARAM_NONE);
    assert(fmt_8bit_page_param(1u, 4u) == FMT_PARAM_NONE);

    assert(fmt_8bit_default_page_word(1u, &word) == 1 && word == 0x48FF0080u);
    assert(fmt_8bit_default_page_word(2u, &word) == 1 && word == 0x48FF0018u);
    assert(fmt_8bit_default_page_word(3u, &word) == 1 && word == 0x48FF0018u);
    assert(fmt_8bit_default_page_word(4u, &word) == 1 && word == 0x48FF0000u);
    word = 0xDEADBEEFu;
    assert(fmt_8bit_default_page_word(0u, &word) == 0 && word == 0xDEADBEEFu);
    assert(fmt_8bit_default_page_word(1u, NULL) == 0);

    word = fmt_8bit_page_pack(0x12u, 0x34u, 0x56u, 0x78u);
    assert(word == 0x78563412u);
    for (slot = 0u; slot < 4u; slot++)
        assert(fmt_8bit_page_unpack(word, slot) == (uint8_t)(0x12u + slot * 0x22u));
    assert(fmt_8bit_page_unpack(word, 4u) == 0u);
    assert(fmt_8bit_page_unpack(word, 255u) == 0u);
}

static void test_all_normalized_mappings(void)
{
    uint32_t v;
    uint8_t previous = 0u;
    uint32_t selector_count[8] = {0};

    for (v = 0u; v < 256u; v++) {
        uint8_t width = fmt_8bit_pulse_width_from_normalized((uint8_t)v);
        uint8_t table = fmt_8bit_wavetable_from_normalized((uint8_t)v);

        assert(width >= 32u && width <= 224u);
        if (v != 0u)
            assert(width >= previous);
        previous = width;
        assert(width == (uint8_t)(32u + ((v * 193u) >> 8)));
        assert(table == (uint8_t)(v >> 5));
        assert(table < 8u);
        selector_count[table]++;
    }

    assert(fmt_8bit_pulse_width_from_normalized(0u) == 32u);
    assert(fmt_8bit_pulse_width_from_normalized(128u) == 128u);
    assert(fmt_8bit_pulse_width_from_normalized(255u) == 224u);
    for (v = 0u; v < 8u; v++)
        assert(selector_count[v] == 32u);
}

static void test_resolver_semantics(void)
{
    Fmt8BitParamSet base = {7u, 11u, 13u, 17u, 19u, 23u};
    Fmt8BitParamSet saved_base = base;
    Fmt8BitParamLock locks[4];
    Fmt8BitParamLock saved_locks[4];
    uint32_t word, expected, sentinel;
    uint8_t status;

    memset(locks, 0, sizeof locks);
    expected = base_word(1u, &base);
    word = 0u;
    assert(fmt_8bit_resolve_page(1u, &base, NULL, 0u, &word) == FMT_8BIT_RESOLVE_OK);
    assert(word == expected);

    locks[0].param_id = FMT_8BIT_PARAM_PULSE_WIDTH;
    locks[0].value = 200u;
    locks[1].param_id = FMT_8BIT_PARAM_DECAY;
    locks[1].value = 201u;
    locks[2].param_id = FMT_8BIT_PARAM_SUSTAIN;
    locks[2].value = 202u;
    locks[3].param_id = FMT_8BIT_PARAM_RELEASE;
    locks[3].value = 203u;
    memcpy(saved_locks, locks, sizeof locks);
    status = fmt_8bit_resolve_page(1u, &base, locks, 4u, &word);
    assert(status == FMT_8BIT_RESOLVE_OK);
    assert(word == fmt_8bit_page_pack(200u, 201u, 202u, 203u));
    assert(memcmp(&base, &saved_base, sizeof base) == 0);
    assert(memcmp(locks, saved_locks, sizeof locks) == 0);

    /* Common and another engine's semantic ID are ignored. */
    locks[0].param_id = FMT_PARAM_VOICE_LEVEL;
    locks[0].value = 1u;
    locks[1].param_id = FMT_PARAM_PITCH_FINE;
    locks[1].value = 2u;
    locks[2].param_id = FMT_8BIT_PARAM_ATTACK;
    locks[2].value = 3u;
    status = fmt_8bit_resolve_page(1u, &base, locks, 3u, &word);
    assert(status == FMT_8BIT_RESOLVE_OK);
    assert(word == expected);

    /* >255 applies only as a domain fallback; other valid overrides survive. */
    locks[0].param_id = FMT_8BIT_PARAM_PULSE_WIDTH;
    locks[0].value = 300u;
    locks[1].param_id = FMT_8BIT_PARAM_RELEASE;
    locks[1].value = 99u;
    status = fmt_8bit_resolve_page(1u, &base, locks, 2u, &word);
    assert(status == FMT_8BIT_RESOLVE_DOMAIN_FALLBACK);
    assert(fmt_8bit_page_unpack(word, 0u) == base.pulse_width);
    assert(fmt_8bit_page_unpack(word, 3u) == 99u);

    /* Duplicate applicable IDs discard every tentative override. */
    locks[0].param_id = FMT_8BIT_PARAM_DECAY;
    locks[0].value = 44u;
    locks[1].param_id = FMT_8BIT_PARAM_RELEASE;
    locks[1].value = 55u;
    locks[2].param_id = FMT_8BIT_PARAM_DECAY;
    locks[2].value = 66u;
    status = fmt_8bit_resolve_page(1u, &base, locks, 3u, &word);
    assert(status == FMT_8BIT_RESOLVE_STRUCTURAL_FALLBACK);
    assert(word == expected);

    word = 0u;
    status = fmt_8bit_resolve_page(1u, &base, locks, 5u, &word);
    assert(status == FMT_8BIT_RESOLVE_STRUCTURAL_FALLBACK);
    assert(word == expected);

    sentinel = 0xA5A5A5A5u;
    word = sentinel;
    assert(fmt_8bit_resolve_page(0u, &base, NULL, 0u, &word) == FMT_8BIT_RESOLVE_ENGINE_FALLBACK);
    assert(word == sentinel);
    assert(fmt_8bit_resolve_page(5u, &base, NULL, 0u, &word) == FMT_8BIT_RESOLVE_ENGINE_FALLBACK);
    assert(word == sentinel);
    assert(fmt_8bit_resolve_page(1u, NULL, NULL, 0u, &word) == FMT_8BIT_RESOLVE_INVALID);
    assert(word == sentinel);
    assert(fmt_8bit_resolve_page(1u, &base, NULL, 1u, &word) == FMT_8BIT_RESOLVE_INVALID);
    assert(word == sentinel);
    assert(fmt_8bit_resolve_page(1u, &base, NULL, 0u, NULL) == FMT_8BIT_RESOLVE_INVALID);
}

static void test_exhaustive_engine_slot_values(void)
{
    Fmt8BitParamSet base = {7u, 11u, 13u, 17u, 19u, 23u};
    uint32_t engine, slot, value;

    for (engine = 1u; engine <= 4u; engine++) {
        uint32_t unlocked = base_word((uint8_t)engine, &base);
        for (slot = 0u; slot < 4u; slot++) {
            FmtParamId id = fmt_8bit_page_param((uint8_t)engine, (uint8_t)slot);
            for (value = 0u; value < 256u; value++) {
                Fmt8BitParamLock lock;
                uint32_t word = 0u;
                uint32_t expected = unlocked;
                uint32_t shift = slot * 8u;

                lock.value = (uint16_t)value;
                lock.param_id = id;
                lock.reserved = 0xA5u;
                expected &= ~(0xFFu << shift);
                expected |= value << shift;
                assert(fmt_8bit_resolve_page((uint8_t)engine, &base,
                                             &lock, 1u, &word) == FMT_8BIT_RESOLVE_OK);
                assert(word == expected);
            }
        }
    }
}

static void test_guards_and_replay_hash(void)
{
    struct {
        uint32_t pre;
        Fmt8BitParamSet base;
        uint32_t post;
    } guarded_base;
    struct {
        uint32_t pre;
        Fmt8BitParamLock lock[4];
        uint32_t post;
    } guarded_locks;
    uint32_t hash = 2166136261u;
    uint32_t engine, value;

    guarded_base.pre = 0x11223344u;
    guarded_base.base.attack = 1u;
    guarded_base.base.decay = 2u;
    guarded_base.base.sustain = 3u;
    guarded_base.base.release = 4u;
    guarded_base.base.pulse_width = 5u;
    guarded_base.base.wavetable = 6u;
    guarded_base.post = 0x55667788u;
    memset(&guarded_locks.lock, 0, sizeof guarded_locks.lock);
    guarded_locks.pre = 0x89ABCDEFu;
    guarded_locks.post = 0x10203040u;
    guarded_locks.lock[0].param_id = FMT_8BIT_PARAM_RELEASE;
    guarded_locks.lock[0].value = 99u;
    {
        uint32_t word = 0u;
        assert(fmt_8bit_resolve_page(2u, &guarded_base.base,
                                     guarded_locks.lock, 1u, &word) == FMT_8BIT_RESOLVE_OK);
    }
    assert(guarded_base.pre == 0x11223344u && guarded_base.post == 0x55667788u);
    assert(guarded_locks.pre == 0x89ABCDEFu && guarded_locks.post == 0x10203040u);

    for (engine = 1u; engine <= 4u; engine++) {
        for (value = 0u; value < 256u; value++) {
            Fmt8BitParamSet base;
            Fmt8BitParamLock locks[3];
            uint32_t word = 0u;
            uint8_t status;

            base.attack = (uint8_t)(value * 3u + 7u);
            base.decay = (uint8_t)(value * 5u + 11u);
            base.sustain = (uint8_t)(value * 7u + 13u);
            base.release = (uint8_t)(value * 11u + 17u);
            base.pulse_width = (uint8_t)(value * 13u + 19u);
            base.wavetable = (uint8_t)(value * 17u + 23u);

            locks[0].param_id = fmt_8bit_page_param((uint8_t)engine, 0u);
            locks[0].value = (uint16_t)(uint8_t)(value * 29u + 31u);
            locks[0].reserved = 0u;
            locks[1].param_id = fmt_8bit_page_param((uint8_t)engine, 3u);
            locks[1].value = (uint16_t)(uint8_t)(value * 37u + 41u);
            locks[1].reserved = 0u;
            locks[2].param_id = FMT_PARAM_VOICE_LEVEL;
            locks[2].value = (uint16_t)value;
            locks[2].reserved = 0u;

            status = fmt_8bit_resolve_page((uint8_t)engine, &base, locks, 3u, &word);
            assert(status == FMT_8BIT_RESOLVE_OK);
            hash = fnv1a_byte(hash, (uint8_t)engine);
            hash = fnv1a_byte(hash, (uint8_t)value);
            hash = fnv1a_word_le(hash, word);
            hash = fnv1a_byte(hash, status);
        }
    }

    assert(hash == 0x635311C5u);
    printf("WP4_8_REPLAY_FNV1A32=%08x\n", hash);
}

int main(void)
{
    test_ids_layout_registry_isolation();
    test_defaults_pages_and_pack();
    test_all_normalized_mappings();
    test_resolver_semantics();
    test_exhaustive_engine_slot_values();
    test_guards_and_replay_hash();

    puts("WP4_8_D044_TESTS_OK=1");
    puts("ids=6 base_bytes=6 lock_snapshot_bytes=4 cache_type_bytes=2048 page_slots=4");
    puts("width_vectors=256 table_vectors=256 exhaustive_engine_slot_values=4096 max_lock_inspections=4");
    return 0;
}
