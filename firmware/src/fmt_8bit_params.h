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

/* D044 semantic identities. WP4.9 activates these exact numeric IDs in the
 * linked D027 registry; WP4.8 deliberately keeps that registry unchanged. */
#define FMT_8BIT_PARAM_ATTACK      ((FmtParamId)0x20u)
#define FMT_8BIT_PARAM_DECAY       ((FmtParamId)0x21u)
#define FMT_8BIT_PARAM_SUSTAIN     ((FmtParamId)0x22u)
#define FMT_8BIT_PARAM_RELEASE     ((FmtParamId)0x23u)
#define FMT_8BIT_PARAM_PULSE_WIDTH ((FmtParamId)0x24u)
#define FMT_8BIT_PARAM_WAVETABLE   ((FmtParamId)0x25u)

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

/* D037/D044 future publication shape. WP4.8 defines the type only and does not
 * allocate a global instance, preserving the isolated zero-linked-delta gate. */
typedef struct {
    uint32_t word[FMT_8BIT_PARAM_TRACKS][FMT_8BIT_PARAM_ROWS];
} Fmt8BitRowParamCache;

#define FMT_8BIT_STATIC_ASSERT(name, expr) \
    typedef char fmt_8bit_static_assert_##name[(expr) ? 1 : -1]
FMT_8BIT_STATIC_ASSERT(param_set_size_is_6, sizeof(Fmt8BitParamSet) == 6u);
FMT_8BIT_STATIC_ASSERT(param_lock_size_is_4, sizeof(Fmt8BitParamLock) == 4u);
FMT_8BIT_STATIC_ASSERT(row_cache_size_is_2048, sizeof(Fmt8BitRowParamCache) == 2048u);

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
