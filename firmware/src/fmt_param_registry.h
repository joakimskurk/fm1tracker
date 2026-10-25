/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_PARAM_REGISTRY_H
#define FM1TRACKER_FMT_PARAM_REGISTRY_H

#include <stdint.h>

/* D027: parameter identity is an exact one-byte semantic domain. */
typedef uint8_t FmtParamId;

#define FMT_PARAM_COMMON_FIRST       0x01u
#define FMT_PARAM_COMMON_LAST        0x1Fu
#define FMT_PARAM_8BIT_FIRST         0x20u
#define FMT_PARAM_8BIT_LAST          0x5Fu
#define FMT_PARAM_16BIT_FIRST        0x60u
#define FMT_PARAM_16BIT_LAST         0x9Fu
#define FMT_PARAM_TRACK_FIRST        0xA0u
#define FMT_PARAM_TRACK_LAST         0xBFu
#define FMT_PARAM_CHANNEL_FX_FIRST   0xC0u
#define FMT_PARAM_CHANNEL_FX_LAST    0xDFu
#define FMT_PARAM_GLOBAL_FIRST       0xE0u
#define FMT_PARAM_GLOBAL_LAST        0xEFu
#define FMT_PARAM_RESERVED_FIRST     0xF0u
#define FMT_PARAM_RESERVED_LAST      0xFEu

/* One authoritative list of IDs actually assigned in the linked runtime.
 * D045 activates the six frozen 8BIT identities. D056 activates the 27 frozen
 * FM identities in the reserved 16BIT namespace while deliberately leaving
 * 0x63 and 0x7C..0x9F unassigned. */
#define FMT_PARAM_REGISTRY(X) \
    X(VOICE_LEVEL,      0x01u, "VOICE LEVEL")  \
    X(PITCH_FINE,       0x02u, "FINE")         \
    X(BIT8_ATTACK,      0x20u, "ATTACK")       \
    X(BIT8_DECAY,       0x21u, "DECAY")        \
    X(BIT8_SUSTAIN,     0x22u, "SUSTAIN")      \
    X(BIT8_RELEASE,     0x23u, "RELEASE")      \
    X(BIT8_PULSE_WIDTH, 0x24u, "WIDTH")        \
    X(BIT8_WAVETABLE,   0x25u, "TABLE")        \
    X(FM_ALGORITHM,     0x60u, "FM ALGORITHM") \
    X(FM_INDEX,         0x61u, "FM INDEX")     \
    X(FM_FEEDBACK,      0x62u, "FM FEEDBACK")  \
    X(FM_OP1_RATIO,     0x64u, "OP1 RATIO")    \
    X(FM_OP2_RATIO,     0x65u, "OP2 RATIO")    \
    X(FM_OP3_RATIO,     0x66u, "OP3 RATIO")    \
    X(FM_OP4_RATIO,     0x67u, "OP4 RATIO")    \
    X(FM_OP1_LEVEL,     0x68u, "OP1 LEVEL")    \
    X(FM_OP2_LEVEL,     0x69u, "OP2 LEVEL")    \
    X(FM_OP3_LEVEL,     0x6Au, "OP3 LEVEL")    \
    X(FM_OP4_LEVEL,     0x6Bu, "OP4 LEVEL")    \
    X(FM_OP1_ATTACK,    0x6Cu, "OP1 ATTACK")   \
    X(FM_OP2_ATTACK,    0x6Du, "OP2 ATTACK")   \
    X(FM_OP3_ATTACK,    0x6Eu, "OP3 ATTACK")   \
    X(FM_OP4_ATTACK,    0x6Fu, "OP4 ATTACK")   \
    X(FM_OP1_DECAY,     0x70u, "OP1 DECAY")    \
    X(FM_OP2_DECAY,     0x71u, "OP2 DECAY")    \
    X(FM_OP3_DECAY,     0x72u, "OP3 DECAY")    \
    X(FM_OP4_DECAY,     0x73u, "OP4 DECAY")    \
    X(FM_OP1_SUSTAIN,   0x74u, "OP1 SUSTAIN")  \
    X(FM_OP2_SUSTAIN,   0x75u, "OP2 SUSTAIN")  \
    X(FM_OP3_SUSTAIN,   0x76u, "OP3 SUSTAIN")  \
    X(FM_OP4_SUSTAIN,   0x77u, "OP4 SUSTAIN")  \
    X(FM_OP1_RELEASE,   0x78u, "OP1 RELEASE")  \
    X(FM_OP2_RELEASE,   0x79u, "OP2 RELEASE")  \
    X(FM_OP3_RELEASE,   0x7Au, "OP3 RELEASE")  \
    X(FM_OP4_RELEASE,   0x7Bu, "OP4 RELEASE")  \
    X(TRACK_LEVEL,      0xA0u, "TRACK LEVEL")  \
    X(TRACK_PAN,        0xA1u, "PAN")

enum {
    FMT_PARAM_NONE = 0x00u,
#define FMT_PARAM_ENUM(symbol, value, label) FMT_PARAM_##symbol = (value),
    FMT_PARAM_REGISTRY(FMT_PARAM_ENUM)
#undef FMT_PARAM_ENUM
    FMT_PARAM_INVALID = 0xFFu,
    FMT_PARAM_ASSIGNED_COUNT = 37u
};

enum {
    FMT_PARAM_NAMESPACE_NONE = 0u,
    FMT_PARAM_NAMESPACE_COMMON = 1u,
    FMT_PARAM_NAMESPACE_8BIT = 2u,
    FMT_PARAM_NAMESPACE_16BIT = 3u,
    FMT_PARAM_NAMESPACE_TRACK = 4u,
    FMT_PARAM_NAMESPACE_CHANNEL_FX = 5u,
    FMT_PARAM_NAMESPACE_GLOBAL = 6u,
    FMT_PARAM_NAMESPACE_RESERVED = 7u,
    FMT_PARAM_NAMESPACE_INVALID = 8u
};

enum {
    FMT_PARAM_FLAG_LOCKABLE = 1u
};

typedef struct {
    FmtParamId id;
    uint8_t flags;
    const char *label;
} FmtParamInfo;

#define FMT_PARAM_STATIC_ASSERT(name, expr) typedef char fmt_param_static_assert_##name[(expr) ? 1 : -1]
FMT_PARAM_STATIC_ASSERT(id_is_one_byte, sizeof(FmtParamId) == 1u);
FMT_PARAM_STATIC_ASSERT(none_is_zero, FMT_PARAM_NONE == 0x00u);
FMT_PARAM_STATIC_ASSERT(invalid_is_ff, FMT_PARAM_INVALID == 0xFFu);
FMT_PARAM_STATIC_ASSERT(common_before_8bit, FMT_PARAM_COMMON_LAST < FMT_PARAM_8BIT_FIRST);
FMT_PARAM_STATIC_ASSERT(bit8_before_bit16, FMT_PARAM_8BIT_LAST < FMT_PARAM_16BIT_FIRST);
FMT_PARAM_STATIC_ASSERT(bit16_before_track, FMT_PARAM_16BIT_LAST < FMT_PARAM_TRACK_FIRST);
FMT_PARAM_STATIC_ASSERT(track_before_fx, FMT_PARAM_TRACK_LAST < FMT_PARAM_CHANNEL_FX_FIRST);
FMT_PARAM_STATIC_ASSERT(fx_before_global, FMT_PARAM_CHANNEL_FX_LAST < FMT_PARAM_GLOBAL_FIRST);
FMT_PARAM_STATIC_ASSERT(global_before_reserved, FMT_PARAM_GLOBAL_LAST < FMT_PARAM_RESERVED_FIRST);
FMT_PARAM_STATIC_ASSERT(reserved_before_invalid, FMT_PARAM_RESERVED_LAST < FMT_PARAM_INVALID);
FMT_PARAM_STATIC_ASSERT(bit8_attack_id, FMT_PARAM_BIT8_ATTACK == 0x20u);
FMT_PARAM_STATIC_ASSERT(bit8_table_id, FMT_PARAM_BIT8_WAVETABLE == 0x25u);
FMT_PARAM_STATIC_ASSERT(fm_algorithm_id, FMT_PARAM_FM_ALGORITHM == 0x60u);
FMT_PARAM_STATIC_ASSERT(fm_feedback_id, FMT_PARAM_FM_FEEDBACK == 0x62u);
FMT_PARAM_STATIC_ASSERT(fm_op1_ratio_id, FMT_PARAM_FM_OP1_RATIO == 0x64u);
FMT_PARAM_STATIC_ASSERT(fm_op4_release_id, FMT_PARAM_FM_OP4_RELEASE == 0x7Bu);

/* Namespace classification describes numeric ownership, not assignment. An ID
 * can belong to a future namespace while fmt_param_lookup() still returns NULL. */
uint8_t fmt_param_namespace(FmtParamId id);

/* Assigned-registry access is deterministic, bounded and read-only. */
uint32_t fmt_param_registry_count(void);
const FmtParamInfo *fmt_param_registry_at(uint32_t index);
const FmtParamInfo *fmt_param_lookup(FmtParamId id);
int fmt_param_is_assigned(FmtParamId id);
int fmt_param_is_lockable(FmtParamId id);

#endif /* FM1TRACKER_FMT_PARAM_REGISTRY_H */