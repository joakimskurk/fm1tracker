/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_voice_allocator.h"
#include "fmt_envelope.h"

static FmtVoiceAllocation fmt_voice_alloc_none(void)
{
    FmtVoiceAllocation result;

    result.voice = FMT_VOICE_NONE;
    result.previous_owner = FMT_TRACK_NONE;
    result.kind = FMT_VOICE_ALLOC_NONE;
    result.reset_required = 0u;
    return result;
}

static uint8_t fmt_voice_stage_valid(uint8_t stage)
{
    return stage <= FMT_ENVELOPE_RELEASE;
}

static uint8_t fmt_voice_mapping_valid(const FmtVoiceAllocator *allocator)
{
    uint32_t i;

    if (!allocator)
        return 0u;

    for (i = 0u; i < FMT_VOICE_ALLOCATOR_SLOTS; i++) {
        uint8_t owner = allocator->voice_owner[i];

        if (owner == FMT_TRACK_NONE)
            continue;
        if (owner >= FMT_VOICE_ALLOCATOR_TRACKS)
            return 0u;
        if (allocator->track_voice[owner] != (uint8_t)i)
            return 0u;
    }

    for (i = 0u; i < FMT_VOICE_ALLOCATOR_TRACKS; i++) {
        uint8_t voice = allocator->track_voice[i];

        if (voice == FMT_VOICE_NONE)
            continue;
        if (voice >= FMT_VOICE_ALLOCATOR_SLOTS)
            return 0u;
        if (allocator->voice_owner[voice] != (uint8_t)i)
            return 0u;
    }

    return 1u;
}

static uint8_t fmt_voice_stage_view_valid(const FmtVoiceAllocator *allocator,
                                          const uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS])
{
    uint32_t i;

    if (!allocator || !stage)
        return 0u;

    for (i = 0u; i < FMT_VOICE_ALLOCATOR_SLOTS; i++) {
        uint8_t s = stage[i];

        if (!fmt_voice_stage_valid(s))
            return 0u;
        if (allocator->voice_owner[i] == FMT_TRACK_NONE &&
            s != FMT_ENVELOPE_IDLE &&
            s != FMT_ENVELOPE_RELEASE)
            return 0u;
    }

    return 1u;
}

static void fmt_voice_touch(FmtVoiceAllocator *allocator, uint8_t voice)
{
    if (allocator->clock != UINT32_MAX)
        allocator->clock++;
    allocator->age[voice] = allocator->clock;
}

static uint8_t fmt_voice_reap_idle_unchecked(FmtVoiceAllocator *allocator,
                                             const uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS])
{
    uint32_t i;
    uint8_t count = 0u;

    for (i = 0u; i < FMT_VOICE_ALLOCATOR_SLOTS; i++) {
        uint8_t owner;

        if (stage[i] != FMT_ENVELOPE_IDLE)
            continue;

        owner = allocator->voice_owner[i];
        if (owner != FMT_TRACK_NONE) {
            allocator->track_voice[owner] = FMT_VOICE_NONE;
            allocator->voice_owner[i] = FMT_TRACK_NONE;
            count++;
        }
        allocator->age[i] = 0u;
    }

    return count;
}

void fmt_voice_allocator_reset(FmtVoiceAllocator *allocator)
{
    uint32_t i;

    if (!allocator)
        return;

    allocator->clock = 0u;
    for (i = 0u; i < FMT_VOICE_ALLOCATOR_SLOTS; i++) {
        allocator->age[i] = 0u;
        allocator->voice_owner[i] = FMT_TRACK_NONE;
    }
    for (i = 0u; i < FMT_VOICE_ALLOCATOR_TRACKS; i++)
        allocator->track_voice[i] = FMT_VOICE_NONE;
}

uint8_t fmt_voice_allocator_validate(const FmtVoiceAllocator *allocator,
                                     const uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS])
{
    if (!fmt_voice_mapping_valid(allocator))
        return 0u;
    return fmt_voice_stage_view_valid(allocator, stage);
}

uint8_t fmt_voice_allocator_reap_idle(FmtVoiceAllocator *allocator,
                                      const uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS])
{
    if (!fmt_voice_allocator_validate(allocator, stage))
        return FMT_VOICE_REAP_ERROR;
    return fmt_voice_reap_idle_unchecked(allocator, stage);
}

FmtVoiceAllocation fmt_voice_allocator_acquire(FmtVoiceAllocator *allocator,
                                               uint8_t track,
                                               const uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS])
{
    FmtVoiceAllocation result = fmt_voice_alloc_none();
    uint32_t i;
    uint8_t voice;
    uint8_t candidate = FMT_VOICE_NONE;
    uint32_t candidate_age = UINT32_MAX;

    if (!allocator || !stage || track >= FMT_VOICE_ALLOCATOR_TRACKS)
        return result;
    if (!fmt_voice_allocator_validate(allocator, stage))
        return result;

    (void)fmt_voice_reap_idle_unchecked(allocator, stage);

    voice = allocator->track_voice[track];
    if (voice != FMT_VOICE_NONE) {
        fmt_voice_touch(allocator, voice);
        result.voice = voice;
        result.previous_owner = track;
        result.kind = FMT_VOICE_ALLOC_REUSE;
        result.reset_required = 0u;
        return result;
    }

    for (i = 0u; i < FMT_VOICE_ALLOCATOR_SLOTS; i++) {
        if (stage[i] == FMT_ENVELOPE_IDLE) {
            voice = (uint8_t)i;
            allocator->voice_owner[voice] = track;
            allocator->track_voice[track] = voice;
            fmt_voice_touch(allocator, voice);
            result.voice = voice;
            result.previous_owner = FMT_TRACK_NONE;
            result.kind = FMT_VOICE_ALLOC_FRESH_IDLE;
            result.reset_required = 1u;
            return result;
        }
    }

    for (i = 0u; i < FMT_VOICE_ALLOCATOR_SLOTS; i++) {
        if (allocator->voice_owner[i] != FMT_TRACK_NONE ||
            stage[i] != FMT_ENVELOPE_RELEASE)
            continue;
        if (candidate == FMT_VOICE_NONE ||
            allocator->age[i] < candidate_age ||
            (allocator->age[i] == candidate_age && i < candidate)) {
            candidate = (uint8_t)i;
            candidate_age = allocator->age[i];
        }
    }

    if (candidate == FMT_VOICE_NONE)
        return result;

    allocator->voice_owner[candidate] = track;
    allocator->track_voice[track] = candidate;
    fmt_voice_touch(allocator, candidate);
    result.voice = candidate;
    result.previous_owner = FMT_TRACK_NONE;
    result.kind = FMT_VOICE_ALLOC_STEAL_TAIL;
    result.reset_required = 1u;
    return result;
}

uint8_t fmt_voice_allocator_detach(FmtVoiceAllocator *allocator,
                                   uint8_t track,
                                   const uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS])
{
    uint8_t voice;

    if (!allocator || !stage || track >= FMT_VOICE_ALLOCATOR_TRACKS)
        return FMT_VOICE_NONE;
    if (!fmt_voice_allocator_validate(allocator, stage))
        return FMT_VOICE_NONE;

    voice = allocator->track_voice[track];
    if (voice == FMT_VOICE_NONE)
        return FMT_VOICE_NONE;
    if (stage[voice] != FMT_ENVELOPE_RELEASE &&
        stage[voice] != FMT_ENVELOPE_IDLE)
        return FMT_VOICE_NONE;

    allocator->track_voice[track] = FMT_VOICE_NONE;
    allocator->voice_owner[voice] = FMT_TRACK_NONE;

    if (stage[voice] == FMT_ENVELOPE_IDLE)
        allocator->age[voice] = 0u;
    else
        fmt_voice_touch(allocator, voice);

    return voice;
}
