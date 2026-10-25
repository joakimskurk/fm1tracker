/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_param_registry.h"

static uint8_t expected_namespace(uint32_t value)
{
    if (value == FMT_PARAM_NONE)
        return FMT_PARAM_NAMESPACE_NONE;
    if (value >= FMT_PARAM_COMMON_FIRST && value <= FMT_PARAM_COMMON_LAST)
        return FMT_PARAM_NAMESPACE_COMMON;
    if (value >= FMT_PARAM_8BIT_FIRST && value <= FMT_PARAM_8BIT_LAST)
        return FMT_PARAM_NAMESPACE_8BIT;
    if (value >= FMT_PARAM_16BIT_FIRST && value <= FMT_PARAM_16BIT_LAST)
        return FMT_PARAM_NAMESPACE_16BIT;
    if (value >= FMT_PARAM_TRACK_FIRST && value <= FMT_PARAM_TRACK_LAST)
        return FMT_PARAM_NAMESPACE_TRACK;
    if (value >= FMT_PARAM_CHANNEL_FX_FIRST && value <= FMT_PARAM_CHANNEL_FX_LAST)
        return FMT_PARAM_NAMESPACE_CHANNEL_FX;
    if (value >= FMT_PARAM_GLOBAL_FIRST && value <= FMT_PARAM_GLOBAL_LAST)
        return FMT_PARAM_NAMESPACE_GLOBAL;
    if (value >= FMT_PARAM_RESERVED_FIRST && value <= FMT_PARAM_RESERVED_LAST)
        return FMT_PARAM_NAMESPACE_RESERVED;
    return FMT_PARAM_NAMESPACE_INVALID;
}

static int expected_assigned(uint32_t value)
{
    return value == FMT_PARAM_VOICE_LEVEL ||
           value == FMT_PARAM_PITCH_FINE ||
           (value >= FMT_PARAM_BIT8_ATTACK &&
            value <= FMT_PARAM_BIT8_WAVETABLE) ||
           (value >= FMT_PARAM_FM_ALGORITHM &&
            value <= FMT_PARAM_FM_OP4_RELEASE && value != 0x63u) ||
           value == FMT_PARAM_TRACK_LEVEL ||
           value == FMT_PARAM_TRACK_PAN;
}

static void test_scalar_and_exact_ranges(void)
{
    assert(sizeof(FmtParamId) == 1u);
    assert(FMT_PARAM_NONE == 0x00u);
    assert(FMT_PARAM_COMMON_FIRST == 0x01u);
    assert(FMT_PARAM_COMMON_LAST == 0x1Fu);
    assert(FMT_PARAM_8BIT_FIRST == 0x20u);
    assert(FMT_PARAM_8BIT_LAST == 0x5Fu);
    assert(FMT_PARAM_16BIT_FIRST == 0x60u);
    assert(FMT_PARAM_16BIT_LAST == 0x9Fu);
    assert(FMT_PARAM_TRACK_FIRST == 0xA0u);
    assert(FMT_PARAM_TRACK_LAST == 0xBFu);
    assert(FMT_PARAM_CHANNEL_FX_FIRST == 0xC0u);
    assert(FMT_PARAM_CHANNEL_FX_LAST == 0xDFu);
    assert(FMT_PARAM_GLOBAL_FIRST == 0xE0u);
    assert(FMT_PARAM_GLOBAL_LAST == 0xEFu);
    assert(FMT_PARAM_RESERVED_FIRST == 0xF0u);
    assert(FMT_PARAM_RESERVED_LAST == 0xFEu);
    assert(FMT_PARAM_INVALID == 0xFFu);

    assert(FMT_PARAM_BIT8_ATTACK == 0x20u);
    assert(FMT_PARAM_BIT8_WAVETABLE == 0x25u);
    assert(FMT_PARAM_FM_ALGORITHM == 0x60u);
    assert(FMT_PARAM_FM_INDEX == 0x61u);
    assert(FMT_PARAM_FM_FEEDBACK == 0x62u);
    assert(FMT_PARAM_FM_OP1_RATIO == 0x64u);
    assert(FMT_PARAM_FM_OP4_RELEASE == 0x7Bu);
}

static void test_exact_assigned_registry(void)
{
    static const FmtParamId expected_ids[FMT_PARAM_ASSIGNED_COUNT] = {
        FMT_PARAM_VOICE_LEVEL, FMT_PARAM_PITCH_FINE,
        FMT_PARAM_BIT8_ATTACK, FMT_PARAM_BIT8_DECAY,
        FMT_PARAM_BIT8_SUSTAIN, FMT_PARAM_BIT8_RELEASE,
        FMT_PARAM_BIT8_PULSE_WIDTH, FMT_PARAM_BIT8_WAVETABLE,
        FMT_PARAM_FM_ALGORITHM, FMT_PARAM_FM_INDEX, FMT_PARAM_FM_FEEDBACK,
        FMT_PARAM_FM_OP1_RATIO, FMT_PARAM_FM_OP2_RATIO,
        FMT_PARAM_FM_OP3_RATIO, FMT_PARAM_FM_OP4_RATIO,
        FMT_PARAM_FM_OP1_LEVEL, FMT_PARAM_FM_OP2_LEVEL,
        FMT_PARAM_FM_OP3_LEVEL, FMT_PARAM_FM_OP4_LEVEL,
        FMT_PARAM_FM_OP1_ATTACK, FMT_PARAM_FM_OP2_ATTACK,
        FMT_PARAM_FM_OP3_ATTACK, FMT_PARAM_FM_OP4_ATTACK,
        FMT_PARAM_FM_OP1_DECAY, FMT_PARAM_FM_OP2_DECAY,
        FMT_PARAM_FM_OP3_DECAY, FMT_PARAM_FM_OP4_DECAY,
        FMT_PARAM_FM_OP1_SUSTAIN, FMT_PARAM_FM_OP2_SUSTAIN,
        FMT_PARAM_FM_OP3_SUSTAIN, FMT_PARAM_FM_OP4_SUSTAIN,
        FMT_PARAM_FM_OP1_RELEASE, FMT_PARAM_FM_OP2_RELEASE,
        FMT_PARAM_FM_OP3_RELEASE, FMT_PARAM_FM_OP4_RELEASE,
        FMT_PARAM_TRACK_LEVEL, FMT_PARAM_TRACK_PAN
    };
    static const char *expected_labels[FMT_PARAM_ASSIGNED_COUNT] = {
        "VOICE LEVEL", "FINE",
        "ATTACK", "DECAY", "SUSTAIN", "RELEASE", "WIDTH", "TABLE",
        "FM ALGORITHM", "FM INDEX", "FM FEEDBACK",
        "OP1 RATIO", "OP2 RATIO", "OP3 RATIO", "OP4 RATIO",
        "OP1 LEVEL", "OP2 LEVEL", "OP3 LEVEL", "OP4 LEVEL",
        "OP1 ATTACK", "OP2 ATTACK", "OP3 ATTACK", "OP4 ATTACK",
        "OP1 DECAY", "OP2 DECAY", "OP3 DECAY", "OP4 DECAY",
        "OP1 SUSTAIN", "OP2 SUSTAIN", "OP3 SUSTAIN", "OP4 SUSTAIN",
        "OP1 RELEASE", "OP2 RELEASE", "OP3 RELEASE", "OP4 RELEASE",
        "TRACK LEVEL", "PAN"
    };
    uint32_t i, j;

    assert(FMT_PARAM_ASSIGNED_COUNT == 37u);
    assert(fmt_param_registry_count() == FMT_PARAM_ASSIGNED_COUNT);

    for (i = 0u; i < FMT_PARAM_ASSIGNED_COUNT; i++) {
        const FmtParamInfo *info = fmt_param_registry_at(i);
        assert(info != NULL);
        assert(info->id == expected_ids[i]);
        assert(info->flags == FMT_PARAM_FLAG_LOCKABLE);
        assert(strcmp(info->label, expected_labels[i]) == 0);
        assert(fmt_param_lookup(info->id) == info);
        assert(fmt_param_is_assigned(info->id));
        assert(fmt_param_is_lockable(info->id));
        if (i > 0u)
            assert(expected_ids[i - 1u] < expected_ids[i]);
        for (j = i + 1u; j < FMT_PARAM_ASSIGNED_COUNT; j++)
            assert(expected_ids[i] != expected_ids[j]);
    }

    assert(fmt_param_registry_at(FMT_PARAM_ASSIGNED_COUNT) == NULL);
    assert(fmt_param_registry_at(UINT32_MAX) == NULL);
}

static void test_all_256_values(void)
{
    uint32_t value;
    uint32_t assigned = 0u;

    for (value = 0u; value <= 0xFFu; value++) {
        FmtParamId id = (FmtParamId)value;
        const FmtParamInfo *info = fmt_param_lookup(id);
        uint8_t ns = fmt_param_namespace(id);

        assert(ns == expected_namespace(value));
        if (expected_assigned(value)) {
            assert(info != NULL);
            assert(info->id == id);
            assert(fmt_param_is_assigned(id));
            assert(fmt_param_is_lockable(id));
            assigned++;
        } else {
            assert(info == NULL);
            assert(!fmt_param_is_assigned(id));
            assert(!fmt_param_is_lockable(id));
        }
    }
    assert(assigned == FMT_PARAM_ASSIGNED_COUNT);
}

static void test_unassigned_namespace_safety(void)
{
    assert(fmt_param_namespace(0x26u) == FMT_PARAM_NAMESPACE_8BIT);
    assert(!fmt_param_is_assigned(0x26u));
    assert(fmt_param_namespace(0x60u) == FMT_PARAM_NAMESPACE_16BIT);
    assert(fmt_param_is_assigned(0x60u));
    assert(fmt_param_namespace(0x63u) == FMT_PARAM_NAMESPACE_16BIT);
    assert(!fmt_param_is_assigned(0x63u));
    assert(fmt_param_namespace(0x7Bu) == FMT_PARAM_NAMESPACE_16BIT);
    assert(fmt_param_is_assigned(0x7Bu));
    assert(fmt_param_namespace(0x7Cu) == FMT_PARAM_NAMESPACE_16BIT);
    assert(!fmt_param_is_assigned(0x7Cu));
    assert(fmt_param_namespace(0x9Fu) == FMT_PARAM_NAMESPACE_16BIT);
    assert(!fmt_param_is_assigned(0x9Fu));
    assert(fmt_param_namespace(FMT_PARAM_NONE) == FMT_PARAM_NAMESPACE_NONE);
    assert(!fmt_param_is_assigned(FMT_PARAM_NONE));
    assert(fmt_param_namespace(FMT_PARAM_INVALID) == FMT_PARAM_NAMESPACE_INVALID);
    assert(!fmt_param_is_assigned(FMT_PARAM_INVALID));
}

int main(void)
{
    test_scalar_and_exact_ranges();
    test_exact_assigned_registry();
    test_all_256_values();
    test_unassigned_namespace_safety();

    printf("FMT_PARAM_REGISTRY_TESTS_OK=1\n");
    printf("id_bytes=%u assigned=%u exhaustive_values=256 d044=0x20_0x25 d056=27_hole_0x63 none=0x%02X invalid=0x%02X\n",
           (unsigned)sizeof(FmtParamId),
           (unsigned)FMT_PARAM_ASSIGNED_COUNT,
           (unsigned)FMT_PARAM_NONE,
           (unsigned)FMT_PARAM_INVALID);
    return 0;
}
