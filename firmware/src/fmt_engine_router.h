/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_ENGINE_ROUTER_H
#define FM1TRACKER_FMT_ENGINE_ROUTER_H

#include <stdint.h>

#include "fmt_model.h"

/* D048 keeps the historical numeric mode values as engine-family identities.
 * FMT_ENGINE_FAMILY_NONE is runtime-only and is never a valid instrument family. */
#define FMT_ENGINE_FAMILY_8BIT ((uint8_t)FMT_MODE_8BIT)
#define FMT_ENGINE_FAMILY_FM   ((uint8_t)FMT_MODE_16BIT)
#define FMT_ENGINE_FAMILY_NONE 0xFFu

/* Exactly one family byte per logical tracker track. Family-local engines keep
 * their own bounded voice handles/allocators; the common router never duplicates
 * those handles. Audio owns this state. */
typedef struct {
    uint8_t family[FMT_TRACKS];
} FmtEngineRouter;

/* Main/startup publishes the existing four-byte instrument headers as aligned
 * 32-bit words. Audio reads only these fixed words and never needs song metadata
 * to decide an instrument family/engine. */
typedef struct {
    volatile uint32_t word[FMT_INSTRUMENTS];
} FmtInstrumentIdentityPublication;

#define FMT_ENGINE_ROUTER_STATIC_ASSERT(name, expr) \
    typedef char fmt_engine_router_static_assert_##name[(expr) ? 1 : -1]

FMT_ENGINE_ROUTER_STATIC_ASSERT(router_is_8_bytes,
                                sizeof(FmtEngineRouter) == 8u);
FMT_ENGINE_ROUTER_STATIC_ASSERT(identity_publication_is_64_bytes,
                                sizeof(FmtInstrumentIdentityPublication) == 64u);
FMT_ENGINE_ROUTER_STATIC_ASSERT(instrument_header_stays_4_bytes,
                                sizeof(FmtInstrumentHeader) == 4u);
FMT_ENGINE_ROUTER_STATIC_ASSERT(family_8bit_stays_zero,
                                FMT_ENGINE_FAMILY_8BIT == 0u);
FMT_ENGINE_ROUTER_STATIC_ASSERT(family_fm_stays_one,
                                FMT_ENGINE_FAMILY_FM == 1u);

int fmt_engine_family_known(uint8_t family);

void fmt_engine_router_reset(FmtEngineRouter *router);
uint8_t fmt_engine_router_family(const FmtEngineRouter *router, uint8_t track);
int fmt_engine_router_commit(FmtEngineRouter *router,
                             uint8_t track,
                             uint8_t family);
void fmt_engine_router_clear(FmtEngineRouter *router, uint8_t track);

void fmt_instrument_identity_publication_reset(
    FmtInstrumentIdentityPublication *publication);
uint32_t fmt_instrument_identity_pack(const FmtInstrumentHeader *header);
int fmt_instrument_identity_publish(FmtInstrumentIdentityPublication *publication,
                                    uint8_t instrument,
                                    const FmtInstrumentHeader *header);
void fmt_instrument_identity_publish_song(
    FmtInstrumentIdentityPublication *publication,
    const FmtSong *song);
int fmt_instrument_identity_read(
    const FmtInstrumentIdentityPublication *publication,
    uint8_t instrument,
    uint8_t *family_out,
    uint8_t *engine_out,
    uint8_t *version_out,
    uint8_t *flags_out);

#endif /* FM1TRACKER_FMT_ENGINE_ROUTER_H */
