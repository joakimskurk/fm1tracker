/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_envelope.h"
#include "fmt_voice_allocator.h"

_Static_assert(sizeof(FmtVoiceAllocator) == 52u, "D043 allocator must stay 52 B");
_Static_assert(sizeof(FmtVoiceAllocation) == 4u, "D043 result must stay 4 B");

static void stages_idle(uint8_t stage[8])
{
    uint32_t i;
    for (i = 0u; i < 8u; i++)
        stage[i] = FMT_ENVELOPE_IDLE;
}

static void assert_reciprocal(const FmtVoiceAllocator *a)
{
    uint32_t i;
    for (i = 0u; i < 8u; i++) {
        uint8_t v = a->track_voice[i];
        if (v != FMT_VOICE_NONE) {
            assert(v < 8u);
            assert(a->voice_owner[v] == (uint8_t)i);
        }
    }
    for (i = 0u; i < 8u; i++) {
        uint8_t t = a->voice_owner[i];
        if (t != FMT_TRACK_NONE) {
            assert(t < 8u);
            assert(a->track_voice[t] == (uint8_t)i);
        }
    }
}

static void test_layout_reset_and_null(void)
{
    FmtVoiceAllocator a;
    uint8_t stage[8];
    uint32_t i;

    stages_idle(stage);
    memset(&a, 0xA5, sizeof a);
    fmt_voice_allocator_reset(&a);
    assert(a.clock == 0u);
    for (i = 0u; i < 8u; i++) {
        assert(a.age[i] == 0u);
        assert(a.voice_owner[i] == FMT_TRACK_NONE);
        assert(a.track_voice[i] == FMT_VOICE_NONE);
    }
    assert(fmt_voice_allocator_validate(&a, stage) == 1u);
    fmt_voice_allocator_reset((FmtVoiceAllocator *)0);
    assert(fmt_voice_allocator_validate((FmtVoiceAllocator *)0, stage) == 0u);
    assert(fmt_voice_allocator_validate(&a, (const uint8_t *)0) == 0u);
}

static void test_fresh_reuse_and_idle_reap(void)
{
    FmtVoiceAllocator a;
    FmtVoiceAllocation r;
    uint8_t stage[8];
    uint32_t t;
    uint32_t before_age;

    stages_idle(stage);
    fmt_voice_allocator_reset(&a);

    for (t = 0u; t < 8u; t++) {
        r = fmt_voice_allocator_acquire(&a, (uint8_t)t, stage);
        assert(r.voice == (uint8_t)t);
        assert(r.previous_owner == FMT_TRACK_NONE);
        assert(r.kind == FMT_VOICE_ALLOC_FRESH_IDLE);
        assert(r.reset_required == 1u);
        stage[r.voice] = FMT_ENVELOPE_ATTACK;
        assert_reciprocal(&a);
        assert(fmt_voice_allocator_validate(&a, stage) == 1u);
    }
    assert(a.clock == 8u);

    before_age = a.age[3];
    r = fmt_voice_allocator_acquire(&a, 3u, stage);
    assert(r.voice == 3u);
    assert(r.previous_owner == 3u);
    assert(r.kind == FMT_VOICE_ALLOC_REUSE);
    assert(r.reset_required == 0u);
    assert(a.age[3] > before_age);
    assert_reciprocal(&a);

    stage[3] = FMT_ENVELOPE_IDLE;
    assert(fmt_voice_allocator_reap_idle(&a, stage) == 1u);
    assert(a.voice_owner[3] == FMT_TRACK_NONE);
    assert(a.track_voice[3] == FMT_VOICE_NONE);
    assert(a.age[3] == 0u);

    r = fmt_voice_allocator_acquire(&a, 3u, stage);
    assert(r.voice == 3u);
    assert(r.kind == FMT_VOICE_ALLOC_FRESH_IDLE);
    assert(r.reset_required == 1u);
    stage[3] = FMT_ENVELOPE_ATTACK;
    assert_reciprocal(&a);
}

static void allocate_all_active(FmtVoiceAllocator *a, uint8_t stage[8])
{
    uint32_t t;
    stages_idle(stage);
    fmt_voice_allocator_reset(a);
    for (t = 0u; t < 8u; t++) {
        FmtVoiceAllocation r = fmt_voice_allocator_acquire(a, (uint8_t)t, stage);
        assert(r.voice == (uint8_t)t);
        stage[r.voice] = FMT_ENVELOPE_ATTACK;
    }
}

static void test_detach_free_before_tail_and_oldest_tail(void)
{
    FmtVoiceAllocator a;
    FmtVoiceAllocation r;
    uint8_t stage[8];
    uint8_t v;
    uint32_t tail0_age, tail1_age;

    allocate_all_active(&a, stage);

    stage[0] = FMT_ENVELOPE_RELEASE;
    v = fmt_voice_allocator_detach(&a, 0u, stage);
    assert(v == 0u);
    assert(a.track_voice[0] == FMT_VOICE_NONE);
    assert(a.voice_owner[0] == FMT_TRACK_NONE);
    tail0_age = a.age[0];

    stage[1] = FMT_ENVELOPE_RELEASE;
    v = fmt_voice_allocator_detach(&a, 1u, stage);
    assert(v == 1u);
    tail1_age = a.age[1];
    assert(tail0_age < tail1_age);

    /* A genuinely free IDLE slot always beats release-tail stealing. */
    stage[7] = FMT_ENVELOPE_IDLE;
    r = fmt_voice_allocator_acquire(&a, 0u, stage);
    assert(r.voice == 7u);
    assert(r.kind == FMT_VOICE_ALLOC_FRESH_IDLE);
    assert(r.reset_required == 1u);
    stage[7] = FMT_ENVELOPE_ATTACK;

    /* With no IDLE slot left, track 1 gets the oldest unowned tail: voice 0. */
    r = fmt_voice_allocator_acquire(&a, 1u, stage);
    assert(r.voice == 0u);
    assert(r.kind == FMT_VOICE_ALLOC_STEAL_TAIL);
    assert(r.reset_required == 1u);
    assert(r.previous_owner == FMT_TRACK_NONE);
    stage[0] = FMT_ENVELOPE_ATTACK;
    assert(a.voice_owner[1] == FMT_TRACK_NONE);
    assert(stage[1] == FMT_ENVELOPE_RELEASE);
    assert_reciprocal(&a);
}

static void test_release_tail_tie_and_saturation(void)
{
    FmtVoiceAllocator a;
    FmtVoiceAllocation r;
    uint8_t stage[8];
    uint32_t i;

    fmt_voice_allocator_reset(&a);
    for (i = 0u; i < 8u; i++)
        stage[i] = FMT_ENVELOPE_RELEASE;
    a.clock = UINT32_MAX;
    for (i = 0u; i < 8u; i++)
        a.age[i] = UINT32_MAX;

    assert(fmt_voice_allocator_validate(&a, stage) == 1u);
    r = fmt_voice_allocator_acquire(&a, 0u, stage);
    assert(r.voice == 0u);
    assert(r.kind == FMT_VOICE_ALLOC_STEAL_TAIL);
    assert(a.clock == UINT32_MAX);
    assert(a.age[0] == UINT32_MAX);
    stage[0] = FMT_ENVELOPE_ATTACK;

    r = fmt_voice_allocator_acquire(&a, 0u, stage);
    assert(r.voice == 0u);
    assert(r.kind == FMT_VOICE_ALLOC_REUSE);
    assert(a.clock == UINT32_MAX);
    assert(a.age[0] == UINT32_MAX);

    stage[0] = FMT_ENVELOPE_RELEASE;
    assert(fmt_voice_allocator_detach(&a, 0u, stage) == 0u);
    assert(a.clock == UINT32_MAX);
    assert(a.age[0] == UINT32_MAX);
}

static void test_immediate_idle_detach(void)
{
    FmtVoiceAllocator a;
    FmtVoiceAllocation r;
    uint8_t stage[8];

    stages_idle(stage);
    fmt_voice_allocator_reset(&a);
    r = fmt_voice_allocator_acquire(&a, 0u, stage);
    assert(r.voice == 0u);
    stage[0] = FMT_ENVELOPE_IDLE; /* D042 R=0 after gate-off. */
    assert(fmt_voice_allocator_detach(&a, 0u, stage) == 0u);
    assert(a.track_voice[0] == FMT_VOICE_NONE);
    assert(a.voice_owner[0] == FMT_TRACK_NONE);
    assert(a.age[0] == 0u);
}

static void test_malformed_and_no_mutation(void)
{
    FmtVoiceAllocator a, before;
    FmtVoiceAllocation r;
    uint8_t stage[8];

    stages_idle(stage);
    fmt_voice_allocator_reset(&a);
    before = a;
    r = fmt_voice_allocator_acquire(&a, 8u, stage);
    assert(r.kind == FMT_VOICE_ALLOC_NONE && r.voice == FMT_VOICE_NONE);
    assert(memcmp(&a, &before, sizeof a) == 0);

    stage[0] = 5u;
    before = a;
    r = fmt_voice_allocator_acquire(&a, 0u, stage);
    assert(r.kind == FMT_VOICE_ALLOC_NONE);
    assert(memcmp(&a, &before, sizeof a) == 0);
    stage[0] = FMT_ENVELOPE_IDLE;

    stage[0] = FMT_ENVELOPE_ATTACK; /* unowned active voice is malformed. */
    before = a;
    assert(fmt_voice_allocator_validate(&a, stage) == 0u);
    r = fmt_voice_allocator_acquire(&a, 0u, stage);
    assert(r.kind == FMT_VOICE_ALLOC_NONE);
    assert(memcmp(&a, &before, sizeof a) == 0);

    stages_idle(stage);
    fmt_voice_allocator_reset(&a);
    a.track_voice[0] = 0u; /* reverse owner missing */
    before = a;
    assert(fmt_voice_allocator_validate(&a, stage) == 0u);
    r = fmt_voice_allocator_acquire(&a, 0u, stage);
    assert(r.kind == FMT_VOICE_ALLOC_NONE);
    assert(memcmp(&a, &before, sizeof a) == 0);

    allocate_all_active(&a, stage);
    before = a;
    assert(fmt_voice_allocator_detach(&a, 0u, stage) == FMT_VOICE_NONE);
    assert(memcmp(&a, &before, sizeof a) == 0);

    /* Impossible full state for an unowned requester: corrupt reciprocal map. */
    a.track_voice[0] = FMT_VOICE_NONE;
    before = a;
    r = fmt_voice_allocator_acquire(&a, 0u, stage);
    assert(r.kind == FMT_VOICE_ALLOC_NONE);
    assert(memcmp(&a, &before, sizeof a) == 0);
}

static void test_all_256_unowned_idle_release_masks(void)
{
    uint32_t mask;

    for (mask = 0u; mask < 256u; mask++) {
        FmtVoiceAllocator a;
        FmtVoiceAllocation r;
        uint8_t stage[8];
        uint32_t i;
        uint8_t expected = FMT_VOICE_NONE;

        fmt_voice_allocator_reset(&a);
        a.clock = 8u;
        for (i = 0u; i < 8u; i++) {
            if (mask & (1u << i)) {
                stage[i] = FMT_ENVELOPE_RELEASE;
                a.age[i] = i + 1u;
            } else {
                stage[i] = FMT_ENVELOPE_IDLE;
                a.age[i] = 0u;
                if (expected == FMT_VOICE_NONE)
                    expected = (uint8_t)i;
            }
        }

        assert(fmt_voice_allocator_validate(&a, stage) == 1u);
        r = fmt_voice_allocator_acquire(&a, 0u, stage);
        if (mask != 255u) {
            assert(r.kind == FMT_VOICE_ALLOC_FRESH_IDLE);
            assert(r.voice == expected);
        } else {
            assert(r.kind == FMT_VOICE_ALLOC_STEAL_TAIL);
            assert(r.voice == 0u);
        }
        assert(r.reset_required == 1u);
        assert_reciprocal(&a);
    }
}

static uint32_t fnv_byte(uint32_t h, uint8_t value)
{
    h ^= value;
    return h * 16777619u;
}

static uint32_t fnv_u32(uint32_t h, uint32_t value)
{
    h = fnv_byte(h, (uint8_t)value);
    h = fnv_byte(h, (uint8_t)(value >> 8));
    h = fnv_byte(h, (uint8_t)(value >> 16));
    h = fnv_byte(h, (uint8_t)(value >> 24));
    return h;
}

static uint32_t hash_snapshot(uint32_t h,
                              const FmtVoiceAllocator *a,
                              const uint8_t stage[8],
                              const FmtVoiceAllocation *r)
{
    uint32_t i;
    if (r) {
        h = fnv_byte(h, r->voice);
        h = fnv_byte(h, r->previous_owner);
        h = fnv_byte(h, r->kind);
        h = fnv_byte(h, r->reset_required);
    }
    for (i = 0u; i < 8u; i++)
        h = fnv_u32(h, a->age[i]);
    h = fnv_u32(h, a->clock);
    for (i = 0u; i < 8u; i++)
        h = fnv_byte(h, a->voice_owner[i]);
    for (i = 0u; i < 8u; i++)
        h = fnv_byte(h, a->track_voice[i]);
    for (i = 0u; i < 8u; i++)
        h = fnv_byte(h, stage[i]);
    return h;
}

static void test_long_replay_hash_and_guards(void)
{
    struct {
        uint32_t before;
        FmtVoiceAllocator a;
        uint32_t after;
    } guarded;
    uint8_t stage[8];
    uint32_t h = 2166136261u;
    uint32_t i, t;

    guarded.before = 0x13579BDFu;
    guarded.after = 0x2468ACE0u;
    stages_idle(stage);
    fmt_voice_allocator_reset(&guarded.a);
    for (t = 0u; t < 8u; t++) {
        FmtVoiceAllocation r = fmt_voice_allocator_acquire(&guarded.a, (uint8_t)t, stage);
        assert(r.voice != FMT_VOICE_NONE);
        stage[r.voice] = FMT_ENVELOPE_ATTACK;
    }

    for (i = 0u; i < 4096u; i++) {
        FmtVoiceAllocation r;
        uint8_t track = (uint8_t)((i >> 2) & 7u);
        uint8_t op = (uint8_t)(i & 3u);

        r.voice = FMT_VOICE_NONE;
        r.previous_owner = FMT_TRACK_NONE;
        r.kind = FMT_VOICE_ALLOC_NONE;
        r.reset_required = 0u;

        if (op == 0u) {
            uint8_t v = guarded.a.track_voice[track];
            if (v != FMT_VOICE_NONE) {
                stage[v] = FMT_ENVELOPE_RELEASE;
                r.voice = fmt_voice_allocator_detach(&guarded.a, track, stage);
            }
        } else if (op == 1u || op == 2u) {
            r = fmt_voice_allocator_acquire(&guarded.a, track, stage);
            if (r.voice != FMT_VOICE_NONE && r.reset_required)
                stage[r.voice] = FMT_ENVELOPE_ATTACK;
        } else {
            uint8_t v = guarded.a.track_voice[track];
            if (v != FMT_VOICE_NONE)
                stage[v] = FMT_ENVELOPE_IDLE;
            r.voice = fmt_voice_allocator_reap_idle(&guarded.a, stage);
            assert(r.voice != FMT_VOICE_REAP_ERROR);
        }

        assert(fmt_voice_allocator_validate(&guarded.a, stage) == 1u);
        assert_reciprocal(&guarded.a);
        h = hash_snapshot(h, &guarded.a, stage, &r);
    }

    assert(h == 0xC1138B39u);
    assert(guarded.a.clock == 2064u);
    assert(guarded.before == 0x13579BDFu);
    assert(guarded.after == 0x2468ACE0u);
}

int main(void)
{
    test_layout_reset_and_null();
    test_fresh_reuse_and_idle_reap();
    test_detach_free_before_tail_and_oldest_tail();
    test_release_tail_tie_and_saturation();
    test_immediate_idle_detach();
    test_malformed_and_no_mutation();
    test_all_256_unowned_idle_release_masks();
    test_long_replay_hash_and_guards();

    puts("WP4_7_ALLOCATOR_TESTS_OK=1");
    puts("allocator_bytes=52 result_bytes=4 slots=8 masks=256 active_cross_track_steal=0 release_tail_steal=1");
    puts("clock=saturating oldest_tail=age_then_low_slot free_priority=idle_before_release no_voice=fail_safe");
    puts("long_hash=c1138b39 long_ops=4096 final_clock=2064");
    return 0;
}
