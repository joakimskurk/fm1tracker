/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_8BIT_PARAMS_H
#define FM1TRACKER_FMT_8BIT_PARAMS_H

#include <stdint.h>

#include "fmt_param_registry.h"

#define FMT_8BIT_ENGINE_SAFE       0u
#define FMT_8BIT_ENGINE_PULSE      1u
#define FMT_8BIT_ENGINE_NOISE      2u
#define FMT_8BIT_ENGINE_TRIANGLE   3u
#define FMT_8BIT_ENGINE_WAVETABLE8 4u

#define FMT_8BIT_PAGE_SLOTS 4u
#define FMT_8BIT_PARAM_TRACKS 8u
#define FMT_8BIT_PARAM_ROWS 64u
#define FMT_8BIT_MAX_ROW_LOCKS 4u

/* D044 semantic identities now alias the one linked D027 registry source of
 * truth activated by D045 Gate 3. Numeric identity remains exactly 0x20..0x25. */
#define FMT_8BIT_PARAM_ATTACK      ((FmtParamId)FMT_PARAM_BIT8_ATTACK)
#define FMT_8BIT_PARAM_DECAY       ((FmtParamId)FMT_PARAM_BIT8_DECAY)
#define FMT_8BIT_PARAM_SUSTAIN     ((FmtParamId)FMT_PARAM_BIT8_SUSTAIN)
#define FMT_8BIT_PARAM_RELEASE     ((FmtParamId)FMT_PARAM_BIT8_RELEASE)
#define FMT_8BIT_PARAM_PULSE_WIDTH ((FmtParamId)FMT_PARAM_BIT8_PULSE_WIDTH)
#define FMT_8BIT_PARAM_WAVETABLE   ((FmtParamId)FMT_PARAM_BIT8_WAVETABLE)

#define FMT_8BIT_DEFAULT_ATTACK      24u
#define FMT_8BIT_DEFAULT_DECAY       0u
#define FMT_8BIT_DEFAULT_SUSTAIN     255u
#define FMT_8BIT_DEFAULT_RELEASE     72u
#define FMT_8BIT_DEFAULT_PULSE_WIDTH 128u
#define FMT_8BIT_DEFAULT_WAVETABLE   0u

enum {
    FMT_8BIT_RESOLVE_OK = 0u,
    FMT_8BIT_RESOLVE_DOMAIN_FALLBACK = 1u,
    FMT_8BIT_RESOLVE_STRUCTURAL_FALLBACK = 2u,
    FMT_8BIT_RESOLVE_ENGINE_FALLBACK = 3u,
    FMT_8BIT_RESOLVE_INVALID = 4u
};

typedef struct {
    uint8_t attack;
    uint8_t decay;
    uint8_t sustain;
    uint8_t release;
    uint8_t pulse_width;
    uint8_t wavetable;
} Fmt8BitParamSet;

/* Bounded input snapshot equivalent to one already-validated D028 record. */
typedef struct {
    uint16_t value;
    FmtParamId param_id;
    uint8_t reserved;
} Fmt8BitParamLock;

/* D037/D044 publication shape. D045 Gate 3 allocates the one linked aligned
 * instance in fmt_8bit_linked_params.c; the type stays exact 2048 B. */
typedef struct {
    uint32_t word[FMT_8BIT_PARAM_TRACKS][FMT_8BIT_PARAM_ROWS];
} Fmt8BitRowParamCache;

#define FMT_8BIT_STATIC_ASSERT(name, expr) \
    typedef char fmt_8bit_static_assert_##name[(expr) ? 1 : -1]
FMT_8BIT_STATIC_ASSERT(param_set_size_is_6, sizeof(Fmt8BitParamSet) == 6u);
FMT_8BIT_STATIC_ASSERT(param_lock_size_is_4, sizeof(Fmt8BitParamLock) == 4u);
FMT_8BIT_STATIC_ASSERT(row_cache_size_is_2048, sizeof(Fmt8BitRowParamCache) == 2048u);
FMT_8BIT_STATIC_ASSERT(attack_id_is_20, FMT_8BIT_PARAM_ATTACK == 0x20u);
FMT_8BIT_STATIC_ASSERT(table_id_is_25, FMT_8BIT_PARAM_WAVETABLE == 0x25u);

void fmt_8bit_param_set_defaults(Fmt8BitParamSet *set);
int fmt_8bit_engine_supported(uint8_t engine);
FmtParamId fmt_8bit_page_param(uint8_t engine, uint8_t slot);
int fmt_8bit_page_value_from_set(uint8_t engine,
                                 uint8_t slot,
                                 const Fmt8BitParamSet *set,
                                 uint8_t *value_out);
uint32_t fmt_8bit_page_pack(uint8_t slot0,
                            uint8_t slot1,
                            uint8_t slot2,
                            uint8_t slot3);
uint8_t fmt_8bit_page_unpack(uint32_t word, uint8_t slot);
int fmt_8bit_default_page_word(uint8_t engine, uint32_t *word_out);
uint8_t fmt_8bit_pulse_width_from_normalized(uint8_t value);
uint8_t fmt_8bit_wavetable_from_normalized(uint8_t value);

/* Resolve one engine page from an already-selected semantic base set and at
 * most four bounded row-lock snapshots. No D028 traversal occurs here. */
uint8_t fmt_8bit_resolve_page(uint8_t engine,
                              const Fmt8BitParamSet *base,
                              const Fmt8BitParamLock *locks,
                              uint8_t lock_count,
                              uint32_t *word_out);

#endif /* FM1TRACKER_FMT_8BIT_PARAMS_H */