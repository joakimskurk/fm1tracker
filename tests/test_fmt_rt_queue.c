/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_rt_queue.h"

static FmtRtEvent make_event(uint8_t type,
                             uint8_t track,
                             uint16_t value_a,
                             uint32_t value_b)
{
    FmtRtEvent event;
    event.type = type;
    event.track = track;
    event.value_a = value_a;
    event.value_b = value_b;
    return event;
}

static void assert_event_eq(const FmtRtEvent *a, const FmtRtEvent *b)
{
    assert(a->type == b->type);
    assert(a->track == b->track);
    assert(a->value_a == b->value_a);
    assert(a->value_b == b->value_b);
}

static void test_layout_and_reset(void)
{
    FmtRtQueue queue;

    assert(sizeof(FmtRtEvent) == 8u);
    assert(offsetof(FmtRtEvent, type) == 0u);
    assert(offsetof(FmtRtEvent, track) == 1u);
    assert(offsetof(FmtRtEvent, value_a) == 2u);
    assert(offsetof(FmtRtEvent, value_b) == 4u);
    assert(sizeof(FmtRtQueue) == 268u);
    assert((FMT_RT_QUEUE_CAPACITY & (FMT_RT_QUEUE_CAPACITY - 1u)) == 0u);

    memset(&queue, 0xA5, sizeof queue);
    fmt_rt_queue_reset(&queue);
    assert(queue.write_seq == 0u);
    assert(queue.read_seq == 0u);
    assert(queue.overflow_seq == 0u);
    assert(fmt_rt_queue_depth(&queue) == 0u);
    assert(fmt_rt_queue_overflow_seq(&queue) == 0u);
}

static void test_fifo_and_payload(void)
{
    FmtRtQueue queue;
    FmtRtEvent in[5], out;
    uint32_t i;

    fmt_rt_queue_reset(&queue);
    in[0] = make_event(FMT_RT_EVENT_NOTE_ON, 0u, 0x7F3Cu, 0u);
    in[1] = make_event(FMT_RT_EVENT_NOTE_OFF, 7u, 0x0040u, 0u);
    in[2] = make_event(0x7Eu, 3u, 0xA55Au, 0x12345678u);
    in[3] = make_event(FMT_RT_EVENT_NOTE_ON, 4u, 0x0160u, 0x89ABCDEFu);
    in[4] = make_event(0xFEu, 2u, 0xFFFFu, 0xFFFFFFFFu);

    for (i = 0u; i < 5u; i++) {
        uint32_t read_before = queue.read_seq;
        assert(fmt_rt_queue_enqueue(&queue, &in[i]) == FMT_RT_QUEUE_OK);
        assert(queue.read_seq == read_before);
        assert(fmt_rt_queue_depth(&queue) == i + 1u);
    }

    for (i = 0u; i < 5u; i++) {
        uint32_t write_before = queue.write_seq;
        uint32_t overflow_before = queue.overflow_seq;
        assert(fmt_rt_queue_dequeue(&queue, &out));
        assert_event_eq(&in[i], &out);
        assert(queue.write_seq == write_before);
        assert(queue.overflow_seq == overflow_before);
    }
    assert(!fmt_rt_queue_dequeue(&queue, &out));
    assert(fmt_rt_queue_depth(&queue) == 0u);
}

static void test_full_reject_newest_preserves_fifo(void)
{
    FmtRtQueue queue;
    FmtRtEvent expected[FMT_RT_QUEUE_CAPACITY], extra, out;
    uint32_t i, full_write;

    fmt_rt_queue_reset(&queue);
    for (i = 0u; i < FMT_RT_QUEUE_CAPACITY; i++) {
        expected[i] = make_event((uint8_t)(0x20u + i),
                                 (uint8_t)(i & 7u),
                                 (uint16_t)(0x1000u + i),
                                 0xABC00000u + i);
        assert(fmt_rt_queue_enqueue(&queue, &expected[i]) == FMT_RT_QUEUE_OK);
    }
    assert(fmt_rt_queue_depth(&queue) == FMT_RT_QUEUE_CAPACITY);
    full_write = queue.write_seq;

    extra = make_event(0xF0u, 1u, 0xBEEFu, 0xDEADBEEFu);
    assert(fmt_rt_queue_enqueue(&queue, &extra) == FMT_RT_QUEUE_FULL);
    assert(queue.write_seq == full_write);
    assert(queue.read_seq == 0u);
    assert(queue.overflow_seq == 1u);
    assert(fmt_rt_queue_enqueue(&queue, &extra) == FMT_RT_QUEUE_FULL);
    assert(fmt_rt_queue_enqueue(&queue, &extra) == FMT_RT_QUEUE_FULL);
    assert(queue.write_seq == full_write);
    assert(queue.overflow_seq == 3u);

    for (i = 0u; i < FMT_RT_QUEUE_CAPACITY; i++) {
        assert(fmt_rt_queue_dequeue(&queue, &out));
        assert_event_eq(&expected[i], &out);
    }
    assert(!fmt_rt_queue_dequeue(&queue, &out));
}

static void test_consumer_discard(void)
{
    FmtRtQueue queue;
    FmtRtEvent event = make_event(0x55u, 2u, 0x1234u, 0x55667788u);
    uint32_t i, write_before;

    fmt_rt_queue_reset(&queue);
    for (i = 0u; i < 9u; i++)
        assert(fmt_rt_queue_enqueue(&queue, &event) == FMT_RT_QUEUE_OK);
    write_before = queue.write_seq;
    fmt_rt_queue_consumer_discard_all(&queue);
    assert(queue.write_seq == write_before);
    assert(queue.read_seq == write_before);
    assert(fmt_rt_queue_depth(&queue) == 0u);
    assert(queue.overflow_seq == 0u);
}

static void test_sequence_wrap(void)
{
    FmtRtQueue queue;
    FmtRtEvent in[3], out;
    uint32_t i;

    fmt_rt_queue_reset(&queue);
    queue.write_seq = UINT32_MAX - 1u;
    queue.read_seq = UINT32_MAX - 1u;

    in[0] = make_event(0x31u, 0u, 1u, 11u);
    in[1] = make_event(0x32u, 1u, 2u, 22u);
    in[2] = make_event(0x33u, 2u, 3u, 33u);
    for (i = 0u; i < 3u; i++)
        assert(fmt_rt_queue_enqueue(&queue, &in[i]) == FMT_RT_QUEUE_OK);

    assert(queue.write_seq == 1u);
    assert(fmt_rt_queue_depth(&queue) == 3u);
    for (i = 0u; i < 3u; i++) {
        assert(fmt_rt_queue_dequeue(&queue, &out));
        assert_event_eq(&in[i], &out);
    }
    assert(queue.read_seq == 1u);
    assert(fmt_rt_queue_depth(&queue) == 0u);
}

static void test_note_helpers_and_malformed_rejection(void)
{
    FmtRtQueue queue;
    FmtRtEvent out, none = make_event(FMT_RT_EVENT_NONE, 0u, 0u, 0u);
    uint32_t write_before, overflow_before;

    fmt_rt_queue_reset(&queue);
    assert(fmt_rt_queue_enqueue_note_on(&queue, 7u, 127u, 127u) == FMT_RT_QUEUE_OK);
    assert(fmt_rt_queue_enqueue_note_off(&queue, 0u, 0u) == FMT_RT_QUEUE_OK);

    assert(fmt_rt_queue_dequeue(&queue, &out));
    assert(out.type == FMT_RT_EVENT_NOTE_ON);
    assert(out.track == 7u);
    assert(fmt_rt_event_note(&out) == 127u);
    assert(fmt_rt_event_velocity(&out) == 127u);
    assert(out.value_b == 0u);

    assert(fmt_rt_queue_dequeue(&queue, &out));
    assert(out.type == FMT_RT_EVENT_NOTE_OFF);
    assert(out.track == 0u);
    assert(fmt_rt_event_note(&out) == 0u);
    assert(fmt_rt_event_velocity(&out) == 0u);

    write_before = queue.write_seq;
    overflow_before = queue.overflow_seq;
    assert(fmt_rt_queue_enqueue(&queue, &none) == FMT_RT_QUEUE_INVALID);
    assert(fmt_rt_queue_enqueue_note_on(&queue, 8u, 60u, 100u) == FMT_RT_QUEUE_INVALID);
    assert(fmt_rt_queue_enqueue_note_on(&queue, 0u, 128u, 100u) == FMT_RT_QUEUE_INVALID);
    assert(fmt_rt_queue_enqueue_note_on(&queue, 0u, 60u, 0u) == FMT_RT_QUEUE_INVALID);
    assert(fmt_rt_queue_enqueue_note_on(&queue, 0u, 60u, 128u) == FMT_RT_QUEUE_INVALID);
    assert(fmt_rt_queue_enqueue_note_off(&queue, 8u, 60u) == FMT_RT_QUEUE_INVALID);
    assert(fmt_rt_queue_enqueue_note_off(&queue, 0u, 128u) == FMT_RT_QUEUE_INVALID);
    assert(queue.write_seq == write_before);
    assert(queue.overflow_seq == overflow_before);
    assert(fmt_rt_queue_depth(&queue) == 0u);
}

int main(void)
{
    test_layout_and_reset();
    test_fifo_and_payload();
    test_full_reject_newest_preserves_fifo();
    test_consumer_discard();
    test_sequence_wrap();
    test_note_helpers_and_malformed_rejection();

    printf("FMT_RT_QUEUE_TESTS_OK=1\n");
    printf("event_bytes=%u queue_bytes=%u capacity=%u usable=%u overflow=reject_newest wrap=1 discard_o1=1 malformed_reject=1\n",
           (unsigned)sizeof(FmtRtEvent),
           (unsigned)sizeof(FmtRtQueue),
           (unsigned)FMT_RT_QUEUE_CAPACITY,
           (unsigned)FMT_RT_QUEUE_CAPACITY);
    return 0;
}
