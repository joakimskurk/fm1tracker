/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_model.h"

#define WP5_0_FAMILY_8BIT 0u
#define WP5_0_FAMILY_STUB 1u
#define WP5_0_FAMILY_NONE 0xFFu
#define WP5_0_STUB_FAMILIES 2u

typedef struct {
    uint8_t family[FMT_TRACKS];
} Wp5RouterModel;

typedef struct {
    uint32_t word[FMT_INSTRUMENTS];
} Wp5IdentityPublicationModel;

typedef struct {
    uint32_t word[FMT_TRACKS][FMT_STEPS_PER_PATTERN];
} Wp5SharedPageCacheModel;

typedef struct {
    uint8_t owned[WP5_0_STUB_FAMILIES][FMT_TRACKS];
    uint8_t fail_note[WP5_0_STUB_FAMILIES];
    uint32_t note_count[WP5_0_STUB_FAMILIES];
    uint32_t hard_silence_count[WP5_0_STUB_FAMILIES];
    uint32_t release_count[WP5_0_STUB_FAMILIES];
    uint32_t tie_count[WP5_0_STUB_FAMILIES];
    uint32_t retrig_count[WP5_0_STUB_FAMILIES];
    uint32_t active_cross_track_steals;
    uint8_t trace[64];
    uint8_t trace_count;
} Wp5StubFamilies;

#define WP5_STATIC_ASSERT(name, expr) \
    typedef char wp5_static_assert_##name[(expr) ? 1 : -1]

WP5_STATIC_ASSERT(router_is_8_bytes, sizeof(Wp5RouterModel) == 8u);
WP5_STATIC_ASSERT(identity_publication_is_64_bytes,
                  sizeof(Wp5IdentityPublicationModel) == 64u);
WP5_STATIC_ASSERT(shared_page_cache_is_2048_bytes,
                  sizeof(Wp5SharedPageCacheModel) == 2048u);

enum {
    TRACE_NOTE_8BIT = 0x10,
    TRACE_NOTE_STUB = 0x11,
    TRACE_HARD_8BIT = 0x20,
    TRACE_HARD_STUB = 0x21,
    TRACE_RELEASE_8BIT = 0x30,
    TRACE_RELEASE_STUB = 0x31,
    TRACE_TIE_8BIT = 0x40,
    TRACE_TIE_STUB = 0x41,
    TRACE_RETRIG_8BIT = 0x50,
    TRACE_RETRIG_STUB = 0x51
};

static void fail(const char *what)
{
    fprintf(stderr, "WP5.0 D048 router contract failure: %s\n", what);
}

static int trace_push(Wp5StubFamilies *stub, uint8_t value)
{
    if (!stub || stub->trace_count >= sizeof(stub->trace))
        return 0;
    stub->trace[stub->trace_count++] = value;
    return 1;
}

static uint8_t trace_note(uint8_t family)
{
    return family == WP5_0_FAMILY_8BIT ? TRACE_NOTE_8BIT : TRACE_NOTE_STUB;
}

static uint8_t trace_hard(uint8_t family)
{
    return family == WP5_0_FAMILY_8BIT ? TRACE_HARD_8BIT : TRACE_HARD_STUB;
}

static uint8_t trace_release(uint8_t family)
{
    return family == WP5_0_FAMILY_8BIT ? TRACE_RELEASE_8BIT : TRACE_RELEASE_STUB;
}

static uint8_t trace_tie(uint8_t family)
{
    return family == WP5_0_FAMILY_8BIT ? TRACE_TIE_8BIT : TRACE_TIE_STUB;
}

static uint8_t trace_retrig(uint8_t family)
{
    return family == WP5_0_FAMILY_8BIT ? TRACE_RETRIG_8BIT : TRACE_RETRIG_STUB;
}

static int stub_family_valid(uint8_t family)
{
    return family < WP5_0_STUB_FAMILIES;
}

static int stub_note(Wp5StubFamilies *stub, uint8_t family, uint8_t track)
{
    if (!stub || !stub_family_valid(family) || track >= FMT_TRACKS)
        return 0;
    stub->note_count[family]++;
    if (!trace_push(stub, trace_note(family)))
        return 0;
    if (stub->fail_note[family])
        return 0;
    if (stub->owned[family][track])
        return 1;
    stub->owned[family][track] = 1u;
    return 1;
}

static void stub_hard_silence(Wp5StubFamilies *stub,
                              uint8_t family,
                              uint8_t track)
{
    if (!stub || !stub_family_valid(family) || track >= FMT_TRACKS)
        return;
    stub->hard_silence_count[family]++;
    (void)trace_push(stub, trace_hard(family));
    stub->owned[family][track] = 0u;
}

static void stub_release(Wp5StubFamilies *stub, uint8_t family, uint8_t track)
{
    if (!stub || !stub_family_valid(family) || track >= FMT_TRACKS)
        return;
    stub->release_count[family]++;
    (void)trace_push(stub, trace_release(family));
    stub->owned[family][track] = 0u;
}

static void stub_tie(Wp5StubFamilies *stub, uint8_t family, uint8_t track)
{
    if (!stub || !stub_family_valid(family) || track >= FMT_TRACKS)
        return;
    stub->tie_count[family]++;
    (void)trace_push(stub, trace_tie(family));
}

static void stub_retrig(Wp5StubFamilies *stub, uint8_t family, uint8_t track)
{
    if (!stub || !stub_family_valid(family) || track >= FMT_TRACKS)
        return;
    stub->retrig_count[family]++;
    (void)trace_push(stub, trace_retrig(family));
}

static void router_reset(Wp5RouterModel *router)
{
    uint32_t track;
    if (!router)
        return;
    for (track = 0u; track < FMT_TRACKS; track++)
        router->family[track] = WP5_0_FAMILY_NONE;
}

static int router_note(Wp5RouterModel *router,
                       Wp5StubFamilies *stub,
                       uint8_t track,
                       uint8_t requested_family)
{
    uint8_t old_family;

    if (!router || !stub || track >= FMT_TRACKS)
        return 0;
    old_family = router->family[track];

    if (!stub_family_valid(requested_family)) {
        if (stub_family_valid(old_family))
            stub_hard_silence(stub, old_family, track);
        router->family[track] = WP5_0_FAMILY_NONE;
        return 0;
    }

    if (stub_family_valid(old_family) && old_family != requested_family) {
        stub_hard_silence(stub, old_family, track);
        router->family[track] = WP5_0_FAMILY_NONE;
    }

    if (!stub_note(stub, requested_family, track)) {
        router->family[track] = WP5_0_FAMILY_NONE;
        return 0;
    }

    router->family[track] = requested_family;
    return 1;
}

static void router_off_or_cut(Wp5RouterModel *router,
                              Wp5StubFamilies *stub,
                              uint8_t track)
{
    uint8_t family;
    if (!router || !stub || track >= FMT_TRACKS)
        return;
    family = router->family[track];
    if (stub_family_valid(family))
        stub_release(stub, family, track);
    router->family[track] = WP5_0_FAMILY_NONE;
}

static void router_tie(Wp5RouterModel *router,
                       Wp5StubFamilies *stub,
                       uint8_t track)
{
    uint8_t family;
    if (!router || !stub || track >= FMT_TRACKS)
        return;
    family = router->family[track];
    if (stub_family_valid(family))
        stub_tie(stub, family, track);
}

static void router_retrig(Wp5RouterModel *router,
                          Wp5StubFamilies *stub,
                          uint8_t track)
{
    uint8_t family;
    if (!router || !stub || track >= FMT_TRACKS)
        return;
    family = router->family[track];
    if (stub_family_valid(family))
        stub_retrig(stub, family, track);
}

static int all_other_routes_equal(const Wp5RouterModel *router,
                                  const uint8_t before[FMT_TRACKS],
                                  uint8_t except_track)
{
    uint32_t track;
    for (track = 0u; track < FMT_TRACKS; track++) {
        if (track != except_track && router->family[track] != before[track])
            return 0;
    }
    return 1;
}

int main(void)
{
    Wp5RouterModel router;
    Wp5IdentityPublicationModel identities;
    Wp5SharedPageCacheModel pages;
    Wp5StubFamilies stub;
    uint8_t before[FMT_TRACKS];
    uint8_t trace_start;
    uint32_t note_before;
    uint32_t track;

    memset(&identities, 0, sizeof(identities));
    memset(&pages, 0, sizeof(pages));
    memset(&stub, 0, sizeof(stub));
    router_reset(&router);

    if (sizeof(router) != 8u || sizeof(identities) != 64u ||
        sizeof(pages) != 2048u) {
        fail("frozen shape changed");
        return 2;
    }

    if (!router_note(&router, &stub, 0u, WP5_0_FAMILY_8BIT) ||
        !router_note(&router, &stub, 1u, WP5_0_FAMILY_STUB) ||
        router.family[0] != WP5_0_FAMILY_8BIT ||
        router.family[1] != WP5_0_FAMILY_STUB) {
        fail("different families did not coexist across tracks");
        return 3;
    }

    memcpy(before, router.family, sizeof(before));
    trace_start = stub.trace_count;
    if (!router_note(&router, &stub, 0u, WP5_0_FAMILY_STUB) ||
        router.family[0] != WP5_0_FAMILY_STUB ||
        stub.owned[WP5_0_FAMILY_8BIT][0] != 0u ||
        stub.owned[WP5_0_FAMILY_STUB][0] != 1u ||
        stub.trace_count != (uint8_t)(trace_start + 2u) ||
        stub.trace[trace_start] != TRACE_HARD_8BIT ||
        stub.trace[trace_start + 1u] != TRACE_NOTE_STUB ||
        !all_other_routes_equal(&router, before, 0u)) {
        fail("cross-family switch ordering/ownership changed");
        return 4;
    }

    stub.fail_note[WP5_0_FAMILY_STUB] = 1u;
    memcpy(before, router.family, sizeof(before));
    if (router_note(&router, &stub, 2u, WP5_0_FAMILY_STUB) ||
        router.family[2] != WP5_0_FAMILY_NONE ||
        !all_other_routes_equal(&router, before, 2u)) {
        fail("failed dispatch did not leave route NONE");
        return 5;
    }

    if (!router_note(&router, &stub, 3u, WP5_0_FAMILY_8BIT)) {
        fail("setup for failed cross-family dispatch failed");
        return 6;
    }
    memcpy(before, router.family, sizeof(before));
    trace_start = stub.trace_count;
    if (router_note(&router, &stub, 3u, WP5_0_FAMILY_STUB) ||
        router.family[3] != WP5_0_FAMILY_NONE ||
        stub.owned[WP5_0_FAMILY_8BIT][3] != 0u ||
        stub.trace_count != (uint8_t)(trace_start + 2u) ||
        stub.trace[trace_start] != TRACE_HARD_8BIT ||
        stub.trace[trace_start + 1u] != TRACE_NOTE_STUB ||
        !all_other_routes_equal(&router, before, 3u)) {
        fail("failed cross-family replacement retained old primary");
        return 7;
    }
    stub.fail_note[WP5_0_FAMILY_STUB] = 0u;

    if (!router_note(&router, &stub, 4u, WP5_0_FAMILY_8BIT)) {
        fail("TIE/RETRIG setup note failed");
        return 8;
    }
    note_before = stub.note_count[WP5_0_FAMILY_8BIT] +
                  stub.note_count[WP5_0_FAMILY_STUB];
    memcpy(before, router.family, sizeof(before));
    router_tie(&router, &stub, 4u);
    router_retrig(&router, &stub, 4u);
    if (memcmp(before, router.family, sizeof(before)) != 0 ||
        note_before != stub.note_count[WP5_0_FAMILY_8BIT] +
                       stub.note_count[WP5_0_FAMILY_STUB] ||
        stub.tie_count[WP5_0_FAMILY_8BIT] != 1u ||
        stub.retrig_count[WP5_0_FAMILY_8BIT] != 1u) {
        fail("TIE/RETRIG allocated or changed family");
        return 9;
    }

    if (!router_note(&router, &stub, 5u, WP5_0_FAMILY_STUB)) {
        fail("OFF/CUT setup note failed");
        return 10;
    }
    memcpy(before, router.family, sizeof(before));
    router_off_or_cut(&router, &stub, 5u);
    if (router.family[5] != WP5_0_FAMILY_NONE ||
        stub.owned[WP5_0_FAMILY_STUB][5] != 0u ||
        !all_other_routes_equal(&router, before, 5u)) {
        fail("OFF/CUT did not clear only the selected route");
        return 11;
    }

    if (!router_note(&router, &stub, 6u, WP5_0_FAMILY_8BIT)) {
        fail("malformed-family setup note failed");
        return 12;
    }
    memcpy(before, router.family, sizeof(before));
    if (router_note(&router, &stub, 6u, 7u) ||
        router.family[6] != WP5_0_FAMILY_NONE ||
        stub.owned[WP5_0_FAMILY_8BIT][6] != 0u ||
        !all_other_routes_equal(&router, before, 6u)) {
        fail("unsupported family did not fail toward silence");
        return 13;
    }

    for (track = 0u; track < FMT_TRACKS; track++) {
        uint8_t family = router.family[track];
        if (family != WP5_0_FAMILY_NONE &&
            (!stub_family_valid(family) || !stub.owned[family][track])) {
            fail("router route has no corresponding family-local owner");
            return 14;
        }
    }

    printf("WP5_0_PREMIGRATION_ROUTER_MODEL_SIZE_OK=1 bytes=%u\n",
           (unsigned)sizeof(router));
    printf("WP5_0_PREMIGRATION_IDENTITY_MODEL_OK=1 instruments=%u bytes=%u\n",
           (unsigned)FMT_INSTRUMENTS,
           (unsigned)sizeof(identities));
    printf("WP5_0_PREMIGRATION_SHARED_CACHE_MODEL_OK=1 rows=%u bytes=%u copies=1\n",
           (unsigned)(FMT_TRACKS * FMT_STEPS_PER_PATTERN),
           (unsigned)sizeof(pages));
    printf("WP5_0_PREMIGRATION_MIXED_ROUTE_MODEL_OK=1 tracks=%u active_cross_track_steals=%u\n",
           (unsigned)FMT_TRACKS,
           (unsigned)stub.active_cross_track_steals);
    printf("WP5_0_PREMIGRATION_SWITCH_ORDER_MODEL_OK=1 old_silence=before_new_note commit_on_success=1 failure_route=none\n");
    printf("WP5_0_PREMIGRATION_NONNOTE_MODEL_OK=1 tie_alloc=0 retrig_alloc=0 off_clears_route=1\n");
    return 0;
}
