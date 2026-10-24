/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_8bit_params.h"

#include <stddef.h>

static const FmtParamId fmt_8bit_page_ids[4][FMT_8BIT_PAGE_SLOTS] = {
    { FMT_8BIT_PARAM_PULSE_WIDTH, FMT_8BIT_PARAM_DECAY,
      FMT_8BIT_PARAM_SUSTAIN, FMT_8BIT_PARAM_RELEASE },
    { FMT_8BIT_PARAM_ATTACK, FMT_8BIT_PARAM_DECAY,
      FMT_8BIT_PARAM_SUSTAIN, FMT_8BIT_PARAM_RELEASE },
    { FMT_8BIT_PARAM_ATTACK, FMT_8BIT_PARAM_DECAY,
      FMT_8BIT_PARAM_SUSTAIN, FMT_8BIT_PARAM_RELEASE },
    { FMT_8BIT_PARAM_WAVETABLE, FMT_8BIT_PARAM_DECAY,
      FMT_8BIT_PARAM_SUSTAIN, FMT_8BIT_PARAM_RELEASE }
};

void fmt_8bit_param_set_defaults(Fmt8BitParamSet *set)
{
    if (!set)
        return;
    set->attack = FMT_8BIT_DEFAULT_ATTACK;
    set->decay = FMT_8BIT_DEFAULT_DECAY;
    set->sustain = FMT_8BIT_DEFAULT_SUSTAIN;
    set->release = FMT_8BIT_DEFAULT_RELEASE;
    set->pulse_width = FMT_8BIT_DEFAULT_PULSE_WIDTH;
    set->wavetable = FMT_8BIT_DEFAULT_WAVETABLE;
}

int fmt_8bit_engine_supported(uint8_t engine)
{
    return engine >= FMT_8BIT_ENGINE_PULSE &&
           engine <= FMT_8BIT_ENGINE_WAVETABLE8;
}

FmtParamId fmt_8bit_page_param(uint8_t engine, uint8_t slot)
{
    if (!fmt_8bit_engine_supported(engine) || slot >= FMT_8BIT_PAGE_SLOTS)
        return FMT_PARAM_NONE;
    return fmt_8bit_page_ids[engine - FMT_8BIT_ENGINE_PULSE][slot];
}

static int fmt_8bit_value_for_param(FmtParamId id,
                                    const Fmt8BitParamSet *set,
                                    uint8_t *value_out)
{
    if (!set || !value_out)
        return 0;

    switch (id) {
    case FMT_8BIT_PARAM_ATTACK:
        *value_out = set->attack;
        return 1;
    case FMT_8BIT_PARAM_DECAY:
        *value_out = set->decay;
        return 1;
    case FMT_8BIT_PARAM_SUSTAIN:
        *value_out = set->sustain;
        return 1;
    case FMT_8BIT_PARAM_RELEASE:
        *value_out = set->release;
        return 1;
    case FMT_8BIT_PARAM_PULSE_WIDTH:
        *value_out = set->pulse_width;
        return 1;
    case FMT_8BIT_PARAM_WAVETABLE:
        *value_out = set->wavetable;
        return 1;
    default:
        return 0;
    }
}

int fmt_8bit_page_value_from_set(uint8_t engine,
                                 uint8_t slot,
                                 const Fmt8BitParamSet *set,
                                 uint8_t *value_out)
{
    FmtParamId id;

    if (!set || !value_out)
        return 0;
    id = fmt_8bit_page_param(engine, slot);
    if (id == FMT_PARAM_NONE)
        return 0;
    return fmt_8bit_value_for_param(id, set, value_out);
}

uint32_t fmt_8bit_page_pack(uint8_t slot0,
                            uint8_t slot1,
                            uint8_t slot2,
                            uint8_t slot3)
{
    return (uint32_t)slot0 |
           ((uint32_t)slot1 << 8) |
           ((uint32_t)slot2 << 16) |
           ((uint32_t)slot3 << 24);
}

uint8_t fmt_8bit_page_unpack(uint32_t word, uint8_t slot)
{
    if (slot >= FMT_8BIT_PAGE_SLOTS)
        return 0u;
    return (uint8_t)(word >> ((uint32_t)slot * 8u));
}

static int fmt_8bit_page_word_from_set(uint8_t engine,
                                       const Fmt8BitParamSet *set,
                                       uint32_t *word_out)
{
    uint8_t value[4];
    uint8_t slot;

    if (!set || !word_out || !fmt_8bit_engine_supported(engine))
        return 0;
    for (slot = 0u; slot < FMT_8BIT_PAGE_SLOTS; slot++) {
        if (!fmt_8bit_page_value_from_set(engine, slot, set, &value[slot]))
            return 0;
    }
    *word_out = fmt_8bit_page_pack(value[0], value[1], value[2], value[3]);
    return 1;
}

int fmt_8bit_default_page_word(uint8_t engine, uint32_t *word_out)
{
    Fmt8BitParamSet defaults;

    if (!word_out)
        return 0;
    fmt_8bit_param_set_defaults(&defaults);
    return fmt_8bit_page_word_from_set(engine, &defaults, word_out);
}

uint8_t fmt_8bit_pulse_width_from_normalized(uint8_t value)
{
    return (uint8_t)(32u + (((uint32_t)value * 193u) >> 8));
}

uint8_t fmt_8bit_wavetable_from_normalized(uint8_t value)
{
    return (uint8_t)(value >> 5);
}

static int fmt_8bit_slot_for_param(uint8_t engine,
                                   FmtParamId param_id,
                                   uint8_t *slot_out)
{
    uint8_t slot;

    if (!slot_out || !fmt_8bit_engine_supported(engine))
        return 0;
    for (slot = 0u; slot < FMT_8BIT_PAGE_SLOTS; slot++) {
        if (fmt_8bit_page_param(engine, slot) == param_id) {
            *slot_out = slot;
            return 1;
        }
    }
    return 0;
}

uint8_t fmt_8bit_resolve_page(uint8_t engine,
                              const Fmt8BitParamSet *base,
                              const Fmt8BitParamLock *locks,
                              uint8_t lock_count,
                              uint32_t *word_out)
{
    uint32_t base_word;
    uint32_t resolved_word;
    uint8_t status = FMT_8BIT_RESOLVE_OK;
    uint8_t seen_slots = 0u;
    uint8_t i;

    if (!base || !word_out)
        return FMT_8BIT_RESOLVE_INVALID;
    if (!fmt_8bit_page_word_from_set(engine, base, &base_word))
        return FMT_8BIT_RESOLVE_ENGINE_FALLBACK;
    if (lock_count > FMT_8BIT_MAX_ROW_LOCKS) {
        *word_out = base_word;
        return FMT_8BIT_RESOLVE_STRUCTURAL_FALLBACK;
    }
    if (lock_count != 0u && !locks)
        return FMT_8BIT_RESOLVE_INVALID;

    resolved_word = base_word;
    for (i = 0u; i < lock_count; i++) {
        uint8_t slot;
        uint8_t slot_mask;
        uint32_t shift;

        if (!fmt_8bit_slot_for_param(engine, locks[i].param_id, &slot))
            continue;
        slot_mask = (uint8_t)(1u << slot);
        if ((seen_slots & slot_mask) != 0u) {
            *word_out = base_word;
            return FMT_8BIT_RESOLVE_STRUCTURAL_FALLBACK;
        }
        seen_slots = (uint8_t)(seen_slots | slot_mask);

        if (locks[i].value > 255u) {
            status = FMT_8BIT_RESOLVE_DOMAIN_FALLBACK;
            continue;
        }

        shift = (uint32_t)slot * 8u;
        resolved_word &= ~(0xFFu << shift);
        resolved_word |= ((uint32_t)(uint8_t)locks[i].value << shift);
    }

    *word_out = resolved_word;
    return status;
}
