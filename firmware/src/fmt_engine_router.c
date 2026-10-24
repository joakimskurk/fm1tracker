/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_engine_router.h"

int fmt_engine_family_known(uint8_t family)
{
    return family == FMT_ENGINE_FAMILY_8BIT ||
           family == FMT_ENGINE_FAMILY_FM;
}

void fmt_engine_router_reset(FmtEngineRouter *router)
{
    uint32_t track;

    if (!router)
        return;
    for (track = 0u; track < FMT_TRACKS; track++)
        router->family[track] = FMT_ENGINE_FAMILY_NONE;
}

uint8_t fmt_engine_router_family(const FmtEngineRouter *router, uint8_t track)
{
    if (!router || track >= FMT_TRACKS)
        return FMT_ENGINE_FAMILY_NONE;
    return router->family[track];
}

int fmt_engine_router_commit(FmtEngineRouter *router,
                             uint8_t track,
                             uint8_t family)
{
    if (!router || track >= FMT_TRACKS || !fmt_engine_family_known(family))
        return 0;
    router->family[track] = family;
    return 1;
}

void fmt_engine_router_clear(FmtEngineRouter *router, uint8_t track)
{
    if (!router || track >= FMT_TRACKS)
        return;
    router->family[track] = FMT_ENGINE_FAMILY_NONE;
}

void fmt_instrument_identity_publication_reset(
    FmtInstrumentIdentityPublication *publication)
{
    uint32_t instrument;

    if (!publication)
        return;
    for (instrument = 0u; instrument < FMT_INSTRUMENTS; instrument++)
        publication->word[instrument] = 0xFFFFFFFFu;
}

uint32_t fmt_instrument_identity_pack(const FmtInstrumentHeader *header)
{
    if (!header)
        return 0xFFFFFFFFu;
    return (uint32_t)header->mode |
           ((uint32_t)header->engine << 8) |
           ((uint32_t)header->version << 16) |
           ((uint32_t)header->flags << 24);
}

int fmt_instrument_identity_publish(FmtInstrumentIdentityPublication *publication,
                                    uint8_t instrument,
                                    const FmtInstrumentHeader *header)
{
    if (!publication || !header || instrument >= FMT_INSTRUMENTS)
        return 0;
    publication->word[instrument] = fmt_instrument_identity_pack(header);
    return 1;
}

void fmt_instrument_identity_publish_song(
    FmtInstrumentIdentityPublication *publication,
    const FmtSong *song)
{
    uint32_t instrument;

    if (!publication)
        return;
    if (!song) {
        fmt_instrument_identity_publication_reset(publication);
        return;
    }
    for (instrument = 0u; instrument < FMT_INSTRUMENTS; instrument++)
        publication->word[instrument] =
            fmt_instrument_identity_pack(&song->instruments[instrument]);
}

int fmt_instrument_identity_read(
    const FmtInstrumentIdentityPublication *publication,
    uint8_t instrument,
    uint8_t *family_out,
    uint8_t *engine_out,
    uint8_t *version_out,
    uint8_t *flags_out)
{
    uint32_t word;
    uint8_t family;

    if (family_out)
        *family_out = FMT_ENGINE_FAMILY_NONE;
    if (engine_out)
        *engine_out = 0xFFu;
    if (version_out)
        *version_out = 0xFFu;
    if (flags_out)
        *flags_out = 0xFFu;
    if (!publication || instrument >= FMT_INSTRUMENTS)
        return 0;

    word = publication->word[instrument];
    family = (uint8_t)word;
    if (!fmt_engine_family_known(family))
        return 0;

    if (family_out)
        *family_out = family;
    if (engine_out)
        *engine_out = (uint8_t)(word >> 8);
    if (version_out)
        *version_out = (uint8_t)(word >> 16);
    if (flags_out)
        *flags_out = (uint8_t)(word >> 24);
    return 1;
}
