/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_engine_router.h"

static int fail(const char *what)
{
    fprintf(stderr, "WP5.0 router core failure: %s\n", what);
    return 1;
}

int main(void)
{
    FmtEngineRouter router;
    FmtInstrumentIdentityPublication pub_a;
    FmtInstrumentIdentityPublication pub_b;
    FmtSong song_a;
    FmtSong song_b;
    FmtInstrumentHeader header;
    uint8_t family, engine, version, flags;
    uint32_t i;

    if (sizeof(router) != 8u)
        return fail("router size");
    if (sizeof(pub_a) != 64u)
        return fail("identity publication size");

    fmt_engine_router_reset(&router);
    for (i = 0u; i < FMT_TRACKS; i++) {
        if (fmt_engine_router_family(&router, (uint8_t)i) !=
            FMT_ENGINE_FAMILY_NONE)
            return fail("router reset");
    }
    if (!fmt_engine_router_commit(&router, 0u, FMT_ENGINE_FAMILY_8BIT) ||
        !fmt_engine_router_commit(&router, 1u, FMT_ENGINE_FAMILY_FM) ||
        fmt_engine_router_family(&router, 0u) != FMT_ENGINE_FAMILY_8BIT ||
        fmt_engine_router_family(&router, 1u) != FMT_ENGINE_FAMILY_FM)
        return fail("independent family commit");
    if (fmt_engine_router_commit(&router, 2u, 7u) ||
        fmt_engine_router_family(&router, 2u) != FMT_ENGINE_FAMILY_NONE)
        return fail("unknown family commit");
    fmt_engine_router_clear(&router, 0u);
    if (fmt_engine_router_family(&router, 0u) != FMT_ENGINE_FAMILY_NONE ||
        fmt_engine_router_family(&router, 1u) != FMT_ENGINE_FAMILY_FM)
        return fail("track-local clear");

    fmt_instrument_identity_publication_reset(&pub_a);
    if (fmt_instrument_identity_read(&pub_a, 0u, &family, &engine,
                                     &version, &flags))
        return fail("reset publication resolved");
    if (family != FMT_ENGINE_FAMILY_NONE || engine != 0xFFu ||
        version != 0xFFu || flags != 0xFFu)
        return fail("safe read defaults");

    header.mode = FMT_ENGINE_FAMILY_8BIT;
    header.engine = 4u;
    header.version = 3u;
    header.flags = 0xA5u;
    if (!fmt_instrument_identity_publish(&pub_a, 3u, &header) ||
        !fmt_instrument_identity_read(&pub_a, 3u, &family, &engine,
                                      &version, &flags) ||
        family != FMT_ENGINE_FAMILY_8BIT || engine != 4u ||
        version != 3u || flags != 0xA5u)
        return fail("8-BIT identity round trip");

    header.mode = FMT_ENGINE_FAMILY_FM;
    header.engine = 9u;
    header.version = 1u;
    header.flags = 0x5Au;
    if (!fmt_instrument_identity_publish(&pub_a, 4u, &header) ||
        !fmt_instrument_identity_read(&pub_a, 4u, &family, &engine,
                                      &version, &flags) ||
        family != FMT_ENGINE_FAMILY_FM || engine != 9u ||
        version != 1u || flags != 0x5Au)
        return fail("FM namespace identity round trip");

    header.mode = 7u;
    header.engine = 1u;
    header.version = 0u;
    header.flags = 0u;
    if (!fmt_instrument_identity_publish(&pub_a, 5u, &header) ||
        fmt_instrument_identity_read(&pub_a, 5u, &family, &engine,
                                     &version, &flags) ||
        family != FMT_ENGINE_FAMILY_NONE)
        return fail("unknown family did not fail safe");

    memset(&song_a, 0, sizeof(song_a));
    memset(&song_b, 0, sizeof(song_b));
    song_a.meta.mode = FMT_MODE_8BIT;
    song_b.meta.mode = FMT_MODE_16BIT;
    for (i = 0u; i < FMT_INSTRUMENTS; i++) {
        uint8_t mode = (uint8_t)(i & 1u);
        uint8_t engine_id = (uint8_t)(i + 1u);
        song_a.instruments[i].mode = mode;
        song_a.instruments[i].engine = engine_id;
        song_a.instruments[i].version = (uint8_t)(i >> 1);
        song_a.instruments[i].flags = (uint8_t)(0x80u | i);
        song_b.instruments[i] = song_a.instruments[i];
    }
    fmt_instrument_identity_publish_song(&pub_a, &song_a);
    fmt_instrument_identity_publish_song(&pub_b, &song_b);
    for (i = 0u; i < FMT_INSTRUMENTS; i++) {
        if (pub_a.word[i] != pub_b.word[i])
            return fail("song meta mode affected identity publication");
    }

    if (fmt_instrument_identity_pack(&song_a.instruments[6]) !=
        ((uint32_t)song_a.instruments[6].mode |
         ((uint32_t)song_a.instruments[6].engine << 8) |
         ((uint32_t)song_a.instruments[6].version << 16) |
         ((uint32_t)song_a.instruments[6].flags << 24)))
        return fail("explicit identity packing");

    printf("WP5_0_ROUTER_SIZE_OK=1 bytes=%u\n", (unsigned)sizeof(router));
    printf("WP5_0_IDENTITY_PUBLICATION_OK=1 instruments=%u bytes=%u\n",
           (unsigned)FMT_INSTRUMENTS, (unsigned)sizeof(pub_a));
    printf("WP5_0_ROUTER_CORE_OK=1 tracks=%u families=2 unknown_family=safe\n",
           (unsigned)FMT_TRACKS);
    printf("WP5_0_SONG_MODE_NONGATING_PUBLICATION_OK=1 mode0_mode1_identity_words=identical\n");
    return 0;
}
