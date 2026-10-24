/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_RT_QUEUE_H
#define FM1TRACKER_FMT_RT_QUEUE_H

#include <stddef.h>
#include <stdint.h>

#include "fmt_model.h"

#define FMT_RT_QUEUE_CAPACITY 32u
#define FMT_RT_QUEUE_MASK     (FMT_RT_QUEUE_CAPACITY - 1u)

enum {
    FMT_RT_EVENT_NONE = 0x00u,
    FMT_RT_EVENT_NOTE_ON = 0x01u,
    FMT_RT_EVENT_NOTE_OFF = 0x02u
};

typedef struct {
    uint8_t type;
    uint8_t track;
    uint16_t value_a;
    uint32_t value_b;
} FmtRtEvent;

typedef struct {
    volatile uint32_t slots[FMT_RT_QUEUE_CAPACITY][2];
    volatile uint32_t write_seq;
    volatile uint32_t read_seq;
    volatile uint32_t overflow_seq;
} FmtRtQueue;

typedef enum {
    FMT_RT_QUEUE_OK = 0,
    FMT_RT_QUEUE_FULL = 1,
    FMT_RT_QUEUE_INVALID = 2
} FmtRtQueueStatus;

FMT_STATIC_ASSERT(rt_event_size_is_8, sizeof(FmtRtEvent) == 8u);
FMT_STATIC_ASSERT(rt_event_type_offset_is_0, offsetof(FmtRtEvent, type) == 0u);
FMT_STATIC_ASSERT(rt_event_track_offset_is_1, offsetof(FmtRtEvent, track) == 1u);
FMT_STATIC_ASSERT(rt_event_value_a_offset_is_2, offsetof(FmtRtEvent, value_a) == 2u);
FMT_STATIC_ASSERT(rt_event_value_b_offset_is_4, offsetof(FmtRtEvent, value_b) == 4u);
FMT_STATIC_ASSERT(rt_queue_capacity_power_of_two,
                  (FMT_RT_QUEUE_CAPACITY & (FMT_RT_QUEUE_CAPACITY - 1u)) == 0u);
FMT_STATIC_ASSERT(rt_queue_size_is_268, sizeof(FmtRtQueue) == 268u);

void fmt_rt_queue_reset(FmtRtQueue *queue);
uint32_t fmt_rt_queue_depth(const FmtRtQueue *queue);
uint32_t fmt_rt_queue_overflow_seq(const FmtRtQueue *queue);

FmtRtQueueStatus fmt_rt_queue_enqueue(FmtRtQueue *queue,
                                      const FmtRtEvent *event);
int fmt_rt_queue_dequeue(FmtRtQueue *queue, FmtRtEvent *event);

/* Consumer-only O(1) recovery primitive. Existing slot payloads are irrelevant
 * once read_seq catches the producer's published write_seq. */
void fmt_rt_queue_consumer_discard_all(FmtRtQueue *queue);

/* D034 NOTE helpers validate current note semantics before publication. */
FmtRtQueueStatus fmt_rt_queue_enqueue_note_on(FmtRtQueue *queue,
                                              uint8_t track,
                                              uint8_t note,
                                              uint8_t velocity);
FmtRtQueueStatus fmt_rt_queue_enqueue_note_off(FmtRtQueue *queue,
                                               uint8_t track,
                                               uint8_t note);

static inline uint8_t fmt_rt_event_note(const FmtRtEvent *event)
{
    return event ? (uint8_t)(event->value_a & 0xFFu) : 0u;
}

static inline uint8_t fmt_rt_event_velocity(const FmtRtEvent *event)
{
    return event ? (uint8_t)(event->value_a >> 8) : 0u;
}

#endif /* FM1TRACKER_FMT_RT_QUEUE_H */
