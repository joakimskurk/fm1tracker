/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FMT_VOICE_ALLOCATOR_H
#define FMT_VOICE_ALLOCATOR_H

#include <stdint.h>

#define FMT_VOICE_ALLOCATOR_SLOTS 8u
#define FMT_VOICE_ALLOCATOR_TRACKS 8u
#define FMT_VOICE_NONE 0xFFu
#define FMT_TRACK_NONE 0xFFu
#define FMT_VOICE_REAP_ERROR 0xFFu

enum {
    FMT_VOICE_ALLOC_NONE = 0,
    FMT_VOICE_ALLOC_REUSE = 1,
    FMT_VOICE_ALLOC_FRESH_IDLE = 2,
    FMT_VOICE_ALLOC_STEAL_TAIL = 3
};

typedef struct {
    uint32_t age[FMT_VOICE_ALLOCATOR_SLOTS];
    uint32_t clock;
    uint8_t voice_owner[FMT_VOICE_ALLOCATOR_SLOTS];
    uint8_t track_voice[FMT_VOICE_ALLOCATOR_TRACKS];
} FmtVoiceAllocator;

typedef struct {
    uint8_t voice;
    uint8_t previous_owner;
    uint8_t kind;
    uint8_t reset_required;
} FmtVoiceAllocation;

void fmt_voice_allocator_reset(FmtVoiceAllocator *allocator);
uint8_t fmt_voice_allocator_validate(const FmtVoiceAllocator *allocator,
                                     const uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS]);
uint8_t fmt_voice_allocator_reap_idle(FmtVoiceAllocator *allocator,
                                      const uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS]);
FmtVoiceAllocation fmt_voice_allocator_acquire(FmtVoiceAllocator *allocator,
                                               uint8_t track,
                                               const uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS]);
uint8_t fmt_voice_allocator_detach(FmtVoiceAllocator *allocator,
                                   uint8_t track,
                                   const uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS]);

#endif
