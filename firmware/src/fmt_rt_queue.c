/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_rt_queue.h"
#include "../hal/fm1_barrier.h"

static uint32_t fmt_rt_event_word0(const FmtRtEvent *event)
{
    return (uint32_t)event->type |
           ((uint32_t)event->track << 8) |
           ((uint32_t)event->value_a << 16);
}

static void fmt_rt_event_unpack(uint32_t word0,
                                uint32_t word1,
                                FmtRtEvent *event)
{
    event->type = (uint8_t)(word0 & 0xFFu);
    event->track = (uint8_t)((word0 >> 8) & 0xFFu);
    event->value_a = (uint16_t)(word0 >> 16);
    event->value_b = word1;
}

void fmt_rt_queue_reset(FmtRtQueue *queue)
{
    if (!queue)
        return;
    queue->write_seq = 0u;
    queue->read_seq = 0u;
    queue->overflow_seq = 0u;
}

uint32_t fmt_rt_queue_depth(const FmtRtQueue *queue)
{
    uint32_t depth;

    if (!queue)
        return 0u;
    depth = queue->write_seq - queue->read_seq;
    return depth > FMT_RT_QUEUE_CAPACITY ? FMT_RT_QUEUE_CAPACITY : depth;
}

uint32_t fmt_rt_queue_overflow_seq(const FmtRtQueue *queue)
{
    return queue ? queue->overflow_seq : 0u;
}

FmtRtQueueStatus fmt_rt_queue_enqueue(FmtRtQueue *queue,
                                      const FmtRtEvent *event)
{
    uint32_t write_seq, read_seq, slot;

    if (!queue || !event || event->type == FMT_RT_EVENT_NONE)
        return FMT_RT_QUEUE_INVALID;

    write_seq = queue->write_seq;
    read_seq = queue->read_seq;
    if ((uint32_t)(write_seq - read_seq) >= FMT_RT_QUEUE_CAPACITY) {
        queue->overflow_seq = queue->overflow_seq + 1u;
        return FMT_RT_QUEUE_FULL;
    }

    slot = write_seq & FMT_RT_QUEUE_MASK;
    queue->slots[slot][0] = fmt_rt_event_word0(event);
    queue->slots[slot][1] = event->value_b;
    fm1_compiler_barrier();
    queue->write_seq = write_seq + 1u;
    return FMT_RT_QUEUE_OK;
}

int fmt_rt_queue_dequeue(FmtRtQueue *queue, FmtRtEvent *event)
{
    uint32_t read_seq, write_seq, slot, word0, word1;

    if (!queue || !event)
        return 0;

    read_seq = queue->read_seq;
    write_seq = queue->write_seq;
    if (read_seq == write_seq)
        return 0;

    fm1_compiler_barrier();
    slot = read_seq & FMT_RT_QUEUE_MASK;
    word0 = queue->slots[slot][0];
    word1 = queue->slots[slot][1];
    fmt_rt_event_unpack(word0, word1, event);
    fm1_compiler_barrier();
    queue->read_seq = read_seq + 1u;
    return 1;
}

void fmt_rt_queue_consumer_discard_all(FmtRtQueue *queue)
{
    uint32_t write_seq;

    if (!queue)
        return;
    write_seq = queue->write_seq;
    fm1_compiler_barrier();
    queue->read_seq = write_seq;
}

FmtRtQueueStatus fmt_rt_queue_enqueue_note_on(FmtRtQueue *queue,
                                              uint8_t track,
                                              uint8_t note,
                                              uint8_t velocity)
{
    FmtRtEvent event;

    if (!queue || track >= FMT_TRACKS || note > FMT_NOTE_MAX ||
        velocity == 0u || velocity > 127u)
        return FMT_RT_QUEUE_INVALID;

    event.type = FMT_RT_EVENT_NOTE_ON;
    event.track = track;
    event.value_a = (uint16_t)((uint16_t)note |
                               ((uint16_t)velocity << 8));
    event.value_b = 0u;
    return fmt_rt_queue_enqueue(queue, &event);
}

FmtRtQueueStatus fmt_rt_queue_enqueue_note_off(FmtRtQueue *queue,
                                               uint8_t track,
                                               uint8_t note)
{
    FmtRtEvent event;

    if (!queue || track >= FMT_TRACKS || note > FMT_NOTE_MAX)
        return FMT_RT_QUEUE_INVALID;

    event.type = FMT_RT_EVENT_NOTE_OFF;
    event.track = track;
    event.value_a = (uint16_t)note;
    event.value_b = 0u;
    return fmt_rt_queue_enqueue(queue, &event);
}
