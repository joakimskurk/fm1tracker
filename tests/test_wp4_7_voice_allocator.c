/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_envelope.h"
#include "fmt_voice_allocator.h"

typedef char fmt_voice_allocator_must_be_52_bytes[(sizeof(FmtVoiceAllocator) == 52u) ? 1 : -1];
typedef char fmt_voice_allocation_must_be_4_bytes[(sizeof(FmtVoiceAllocation) == 4u) ? 1 : -1];

static void assert_none(FmtVoiceAllocation result)
{
    assert(result.voice == FMT_VOICE_NONE);
    assert(result.previous_owner == FMT_TRACK_NONE);
    assert(result.kind == FMT_VOICE_ALLOC_NONE);
    assert(result.reset_required == 0u);
}

static void assert_reset_state(const FmtVoiceAllocator *allocator)
{
    uint32_t i;

    assert(allocator->clock == 0u);
    for (i = 0u; i < 8u; i++) {
        assert(allocator->age[i] == 0u);
        assert(allocator->voice_owner[i] == FMT_TRACK_NONE);
        assert(allocator->track_voice[i] == FMT_VOICE_NONE);
    }
}

static void test_layout_reset_and_first_allocations(void)
{
    FmtVoiceAllocator allocator;
    uint8_t stage[8] = {0};
    uint32_t track;

    assert(sizeof(FmtVoiceAllocator) == 52u);
    assert(sizeof(FmtVoiceAllocation) == 4u);
    assert(FMT_VOICE_ALLOCATOR_SLOTS == 8u);
    assert(FMT_VOICE_ALLOCATOR_TRACKS == 8u);
    assert(FMT_VOICE_NONE == 0xFFu);
    assert(FMT_TRACK_NONE == 0xFFu);

    memset(&allocator, 0xA5, sizeof allocator);
    fmt_voice_allocator_reset(&allocator);
    assert_reset_state(&allocator);
    assert(fmt_voice_allocator_validate(&allocator, stage) == 1u);

    for (track = 0u; track < 8u; track++) {
        FmtVoiceAllocation result = fmt_voice_allocator_acquire(&allocator, (uint8_t)track, stage);

        assert(result.voice == track);
        assert(result.previous_owner == FMT_TRACK_NONE);
        assert(result.kind == FMT_VOICE_ALLOC_FRESH_IDLE);
        assert(result.reset_required == 1u);
        assert(allocator.voice_owner[track] == track);
        assert(allocator.track_voice[track] == track);
        assert(allocator.age[track] == track + 1u);
        assert(allocator.clock == track + 1u);
        stage[track] = FMT_ENVELOPE_ATTACK;
        assert(fmt_voice_allocator_validate(&allocator, stage) == 1u);
    }

    {
        uint32_t old_age = allocator.age[3];
        FmtVoiceAllocation result = fmt_voice_allocator_acquire(&allocator, 3u, stage);
        assert(result.voice == 3u);
        assert(result.previous_owner == 3u);
        assert(result.kind == FMT_VOICE_ALLOC_REUSE);
        assert(result.reset_required == 0u);
        assert(allocator.age[3] > old_age);
        assert(allocator.clock == 9u);
    }

    fmt_voice_allocator_reset((FmtVoiceAllocator *)0);
    assert(fmt_voice_allocator_validate((const FmtVoiceAllocator *)0, stage) == 0u);
    assert(fmt_voice_allocator_validate(&allocator, (const uint8_t *)0) == 0u);
    assert(fmt_voice_allocator_reap_idle((FmtVoiceAllocator *)0, stage) == FMT_VOICE_REAP_ERROR);
    assert(fmt_voice_allocator_detach((FmtVoiceAllocator *)0, 0u, stage) == FMT_VOICE_NONE);
    assert_none(fmt_voice_allocator_acquire((FmtVoiceAllocator *)0, 0u, stage));
}

static void test_idle_sync_and_reap(void)
{
    FmtVoiceAllocator allocator;
    uint8_t stage[8] = {0};
    FmtVoiceAllocation result;

    fmt_voice_allocator_reset(&allocator);
    result = fmt_voice_allocator_acquire(&allocator, 0u, stage);
    assert(result.voice == 0u);
    stage[0] = FMT_ENVELOPE_ATTACK;

    /* Envelope reached IDLE before housekeeping: next acquire must clear stale owner first. */
    stage[0] = FMT_ENVELOPE_IDLE;
    result = fmt_voice_allocator_acquire(&allocator, 1u, stage);
    assert(result.voice == 0u);
    assert(result.kind == FMT_VOICE_ALLOC_FRESH_IDLE);
    assert(allocator.track_voice[0] == FMT_VOICE_NONE);
    assert(allocator.track_voice[1] == 0u);
    assert(allocator.voice_owner[0] == 1u);
    stage[0] = FMT_ENVELOPE_ATTACK;

    /* Reap can clear several independently completed voices in one bounded pass. */
    result = fmt_voice_allocator_acquire(&allocator, 2u, stage);
    assert(result.voice == 1u);
    stage[1] = FMT_ENVELOPE_SUSTAIN;
    stage[0] = FMT_ENVELOPE_IDLE;
    stage[1] = FMT_ENVELOPE_IDLE;
    assert(fmt_voice_allocator_reap_idle(&allocator, stage) == 2u);
    assert(allocator.track_voice[1] == FMT_VOICE_NONE);
    assert(allocator.track_voice[2] == FMT_VOICE_NONE);
    assert(allocator.voice_owner[0] == FMT_TRACK_NONE);
    assert(allocator.voice_owner[1] == FMT_TRACK_NONE);
    assert(allocator.age[0] == 0u);
    assert(allocator.age[1] == 0u);
}

static void test_detach_release_and_free_before_tail(void)
{
    FmtVoiceAllocator allocator;
    uint8_t stage[8] = {0};
    FmtVoiceAllocation result;
    uint32_t release_age;

    fmt_voice_allocator_reset(&allocator);
    result = fmt_voice_allocator_acquire(&allocator, 0u, stage);
    assert(result.voice == 0u);
    stage[0] = FMT_ENVELOPE_ATTACK;

    /* Detach is legal only after D042 has entered RELEASE or IDLE. */
    assert(fmt_voice_allocator_detach(&allocator, 0u, stage) == FMT_VOICE_NONE);
    assert(allocator.track_voice[0] == 0u);

    stage[0] = FMT_ENVELOPE_RELEASE;
    assert(fmt_voice_allocator_detach(&allocator, 0u, stage) == 0u);
    assert(allocator.track_voice[0] == FMT_VOICE_NONE);
    assert(allocator.voice_owner[0] == FMT_TRACK_NONE);
    assert(stage[0] == FMT_ENVELOPE_RELEASE);
    release_age = allocator.age[0];
    assert(release_age != 0u);

    /* A real free IDLE slot wins before a release tail is stolen. */
    result = fmt_voice_allocator_acquire(&allocator, 1u, stage);
    assert(result.voice == 1u);
    assert(result.kind == FMT_VOICE_ALLOC_FRESH_IDLE);
    assert(result.reset_required == 1u);
    assert(allocator.age[0] == release_age);

    /* Zero-time release can be detached as a genuinely free IDLE slot. */
    stage[1] = FMT_ENVELOPE_IDLE;
    assert(fmt_voice_allocator_detach(&allocator, 1u, stage) == 1u);
    assert(allocator.voice_owner[1] == FMT_TRACK_NONE);
    assert(allocator.track_voice[1] == FMT_VOICE_NONE);
    assert(allocator.age[1] == 0u);
}

static void make_two_tails_and_six_owned(FmtVoiceAllocator *allocator, uint8_t stage[8])
{
    FmtVoiceAllocation result;
    uint8_t track;

    fmt_voice_allocator_reset(allocator);
    memset(stage, FMT_ENVELOPE_IDLE, 8u);

    result = fmt_voice_allocator_acquire(allocator, 1u, stage);
    assert(result.voice == 0u);
    stage[0] = FMT_ENVELOPE_RELEASE;
    assert(fmt_voice_allocator_detach(allocator, 1u, stage) == 0u);

    result = fmt_voice_allocator_acquire(allocator, 2u, stage);
    assert(result.voice == 1u);
    stage[1] = FMT_ENVELOPE_RELEASE;
    assert(fmt_voice_allocator_detach(allocator, 2u, stage) == 1u);

    for (track = 1u; track <= 6u; track++) {
        result = fmt_voice_allocator_acquire(allocator, track, stage);
        assert(result.voice == (uint8_t)(track + 1u));
        stage[result.voice] = FMT_ENVELOPE_ATTACK;
    }
}

static void test_oldest_release_tail_steal(void)
{
    FmtVoiceAllocator allocator;
    uint8_t stage[8];
    FmtVoiceAllocation result;
    uint32_t old0, old1;
    uint8_t owner_copy[8];

    make_two_tails_and_six_owned(&allocator, stage);
    old0 = allocator.age[0];
    old1 = allocator.age[1];
    assert(old0 < old1);
    memcpy(owner_copy, allocator.voice_owner, sizeof owner_copy);

    result = fmt_voice_allocator_acquire(&allocator, 0u, stage);
    assert(result.voice == 0u);
    assert(result.previous_owner == FMT_TRACK_NONE);
    assert(result.kind == FMT_VOICE_ALLOC_STEAL_TAIL);
    assert(result.reset_required == 1u);
    assert(allocator.voice_owner[0] == 0u);
    assert(allocator.track_voice[0] == 0u);
    assert(allocator.voice_owner[1] == FMT_TRACK_NONE);

    /* No other track-owned primary voice was changed. */
    for (uint32_t i = 2u; i < 8u; i++)
        assert(allocator.voice_owner[i] == owner_copy[i]);
}

static void test_all_256_idle_release_masks(void)
{
    uint32_t mask;

    for (mask = 0u; mask < 256u; mask++) {
        FmtVoiceAllocator allocator;
        uint8_t stage[8];
        FmtVoiceAllocation result;
        uint32_t i;
        uint8_t expected = FMT_VOICE_NONE;

        fmt_voice_allocator_reset(&allocator);
        allocator.clock = 8u;
        for (i = 0u; i < 8u; i++) {
            if (mask & (1u << i)) {
                stage[i] = FMT_ENVELOPE_RELEASE;
                allocator.age[i] = i + 1u;
            } else {
                stage[i] = FMT_ENVELOPE_IDLE;
                allocator.age[i] = 0u;
                if (expected == FMT_VOICE_NONE)
                    expected = (uint8_t)i;
            }
        }

        result = fmt_voice_allocator_acquire(&allocator, 0u, stage);
        if (mask != 255u) {
            assert(result.voice == expected);
            assert(result.kind == FMT_VOICE_ALLOC_FRESH_IDLE);
        } else {
            assert(result.voice == 0u);
            assert(result.kind == FMT_VOICE_ALLOC_STEAL_TAIL);
        }
        assert(result.reset_required == 1u);
    }
}

static void test_saturating_clock_and_tie_break(void)
{
    FmtVoiceAllocator allocator;
    uint8_t stage[8];
    FmtVoiceAllocation result;
    uint32_t i;

    fmt_voice_allocator_reset(&allocator);
    allocator.clock = UINT32_MAX;
    for (i = 0u; i < 8u; i++) {
        allocator.age[i] = UINT32_MAX;
        allocator.voice_owner[i] = FMT_TRACK_NONE;
        allocator.track_voice[i] = FMT_VOICE_NONE;
        stage[i] = FMT_ENVELOPE_RELEASE;
    }

    result = fmt_voice_allocator_acquire(&allocator, 0u, stage);
    assert(result.voice == 0u);
    assert(result.kind == FMT_VOICE_ALLOC_STEAL_TAIL);
    assert(allocator.clock == UINT32_MAX);
    assert(allocator.age[0] == UINT32_MAX);

    /* Reuse at saturation stays saturated and cannot wrap to zero. */
    stage[0] = FMT_ENVELOPE_SUSTAIN;
    result = fmt_voice_allocator_acquire(&allocator, 0u, stage);
    assert(result.voice == 0u);
    assert(result.kind == FMT_VOICE_ALLOC_REUSE);
    assert(allocator.clock == UINT32_MAX);
    assert(allocator.age[0] == UINT32_MAX);
}

static void test_malformed_inputs_fail_without_mutation(void)
{
    FmtVoiceAllocator allocator, before;
    uint8_t stage[8] = {0};
    FmtVoiceAllocation result;

    fmt_voice_allocator_reset(&allocator);
    before = allocator;
    assert_none(fmt_voice_allocator_acquire(&allocator, 8u, stage));
    assert(memcmp(&allocator, &before, sizeof allocator) == 0);

    stage[0] = 5u;
    before = allocator;
    assert_none(fmt_voice_allocator_acquire(&allocator, 0u, stage));
    assert(memcmp(&allocator, &before, sizeof allocator) == 0);
    assert(fmt_voice_allocator_reap_idle(&allocator, stage) == FMT_VOICE_REAP_ERROR);
    assert(memcmp(&allocator, &before, sizeof allocator) == 0);

    stage[0] = FMT_ENVELOPE_ATTACK; /* unowned ATTACK is forbidden */
    before = allocator;
    assert_none(fmt_voice_allocator_acquire(&allocator, 0u, stage));
    assert(memcmp(&allocator, &before, sizeof allocator) == 0);

    memset(stage, FMT_ENVELOPE_IDLE, sizeof stage);
    allocator.voice_owner[0] = 0u;
    allocator.track_voice[0] = FMT_VOICE_NONE; /* broken bidirectional map */
    before = allocator;
    result = fmt_voice_allocator_acquire(&allocator, 0u, stage);
    assert_none(result);
    assert(memcmp(&allocator, &before, sizeof allocator) == 0);
}

static void test_guards_and_replay_basics(void)
{
    struct {
        uint32_t before;
        FmtVoiceAllocator allocator;
        uint32_t after;
    } guarded;
    uint8_t stage[8] = {0};
    uint32_t i;

    guarded.before = 0x13579BDFu;
    guarded.after = 0x2468ACE0u;
    fmt_voice_allocator_reset(&guarded.allocator);

    for (i = 0u; i < 8u; i++) {
        FmtVoiceAllocation result = fmt_voice_allocator_acquire(&guarded.allocator, (uint8_t)i, stage);
        assert(result.voice == i);
        stage[i] = FMT_ENVELOPE_SUSTAIN;
    }
    for (i = 0u; i < 8u; i += 2u) {
        stage[i] = FMT_ENVELOPE_RELEASE;
        assert(fmt_voice_allocator_detach(&guarded.allocator, (uint8_t)i, stage) == i);
    }

    assert(guarded.before == 0x13579BDFu);
    assert(guarded.after == 0x2468ACE0u);
    assert(fmt_voice_allocator_validate(&guarded.allocator, stage) == 1u);
}

static uint32_t fnv_byte(uint32_t hash, uint8_t value)
{
    hash ^= value;
    return hash * 16777619u;
}

static uint32_t fnv_u32(uint32_t hash, uint32_t value)
{
    hash = fnv_byte(hash, (uint8_t)value);
    hash = fnv_byte(hash, (uint8_t)(value >> 8));
    hash = fnv_byte(hash, (uint8_t)(value >> 16));
    hash = fnv_byte(hash, (uint8_t)(value >> 24));
    return hash;
}

static uint32_t fold_state(uint32_t hash,
                           const FmtVoiceAllocator *allocator,
                           const uint8_t stage[8])
{
    uint32_t i;

    hash = fnv_u32(hash, allocator->clock);
    for (i = 0u; i < 8u; i++)
        hash = fnv_u32(hash, allocator->age[i]);
    for (i = 0u; i < 8u; i++)
        hash = fnv_byte(hash, allocator->voice_owner[i]);
    for (i = 0u; i < 8u; i++)
        hash = fnv_byte(hash, allocator->track_voice[i]);
    for (i = 0u; i < 8u; i++)
        hash = fnv_byte(hash, stage[i]);
    return hash;
}

static uint32_t run_long_sequence(FmtVoiceAllocator *final_allocator, uint8_t final_stage[8])
{
    FmtVoiceAllocator allocator;
    uint8_t stage[8] = {0};
    uint32_t hash = 2166136261u;
    uint32_t random = 0xC0FFEE11u;
    uint32_t step;

    fmt_voice_allocator_reset(&allocator);

    for (step = 0u; step < 4096u; step++) {
        uint8_t track, op, voice, w;

        random = random * 1664525u + 1013904223u;
        track = (uint8_t)((random >> 16) & 7u);
        op = (uint8_t)((random >> 29) & 3u);
        hash = fnv_byte(hash, op);
        hash = fnv_byte(hash, track);

        if (op == 0u || op == 3u) {
            FmtVoiceAllocation result = fmt_voice_allocator_acquire(&allocator, track, stage);
            hash = fnv_byte(hash, result.voice);
            hash = fnv_byte(hash, result.previous_owner);
            hash = fnv_byte(hash, result.kind);
            hash = fnv_byte(hash, result.reset_required);
            if (result.voice != FMT_VOICE_NONE)
                stage[result.voice] = FMT_ENVELOPE_ATTACK;
        } else if (op == 1u) {
            voice = allocator.track_voice[track];
            if (voice != FMT_VOICE_NONE)
                stage[voice] = FMT_ENVELOPE_RELEASE;
            hash = fnv_byte(hash, fmt_voice_allocator_detach(&allocator, track, stage));
        } else {
            voice = (uint8_t)((random >> 8) & 7u);
            if (stage[voice] == FMT_ENVELOPE_RELEASE &&
                allocator.voice_owner[voice] == FMT_TRACK_NONE)
                stage[voice] = FMT_ENVELOPE_IDLE;
            w = (uint8_t)((random >> 5) & 7u);
            if (allocator.voice_owner[w] != FMT_TRACK_NONE &&
                (stage[w] == FMT_ENVELOPE_ATTACK || stage[w] == FMT_ENVELOPE_DECAY))
                stage[w] = FMT_ENVELOPE_SUSTAIN;
            hash = fnv_byte(hash, fmt_voice_allocator_reap_idle(&allocator, stage));
        }

        w = (uint8_t)((random >> 2) & 7u);
        if (allocator.voice_owner[w] != FMT_TRACK_NONE &&
            stage[w] == FMT_ENVELOPE_ATTACK &&
            ((random >> 12) & 1u))
            stage[w] = FMT_ENVELOPE_SUSTAIN;

        assert(fmt_voice_allocator_validate(&allocator, stage) == 1u);
        hash = fold_state(hash, &allocator, stage);
    }

    if (final_allocator)
        *final_allocator = allocator;
    if (final_stage)
        memcpy(final_stage, stage, 8u);
    return hash;
}

static void test_long_sequence_and_replay(void)
{
    FmtVoiceAllocator a, b;
    uint8_t stage_a[8], stage_b[8];
    uint32_t hash_a = run_long_sequence(&a, stage_a);
    uint32_t hash_b = run_long_sequence(&b, stage_b);

    assert(hash_a == 0xC6BAFF38u);
    assert(hash_b == hash_a);
    assert(memcmp(&a, &b, sizeof a) == 0);
    assert(memcmp(stage_a, stage_b, sizeof stage_a) == 0);
    assert(a.clock == 2658u);
    assert(a.voice_owner[0] == 7u);
    assert(a.voice_owner[1] == 1u);
    assert(a.voice_owner[2] == FMT_TRACK_NONE);
    assert(a.voice_owner[3] == 3u);
    assert(a.voice_owner[4] == 2u);
    assert(a.voice_owner[5] == 4u);
    assert(a.voice_owner[6] == 6u);
    assert(a.voice_owner[7] == 5u);
    assert(stage_a[2] == FMT_ENVELOPE_RELEASE);
}

int main(void)
{
    test_layout_reset_and_first_allocations();
    test_idle_sync_and_reap();
    test_detach_release_and_free_before_tail();
    test_oldest_release_tail_steal();
    test_all_256_idle_release_masks();
    test_saturating_clock_and_tie_break();
    test_malformed_inputs_fail_without_mutation();
    test_guards_and_replay_basics();
    test_long_sequence_and_replay();

    puts("WP4_7_ALLOCATOR_TESTS_OK=1");
    puts("allocator_bytes=52 result_bytes=4 slots=8 masks=256 active_cross_track_steal=0 release_tail_steal=1");
    puts("selection=reuse,lowest_idle,oldest_release_tail tie=lower_slot clock=saturating_u32");
    puts("long_hash_fnv1a32=c6baff38 events=4096 final_clock=2658");
    return 0;
}
