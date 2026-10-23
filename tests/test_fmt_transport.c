/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "fmt_transport.h"

static void test_reset_and_null_safety(void)
{
    FmtTransport t = {99u, 999u, 63u, 1u};

    fmt_transport_reset(&t);
    assert(t.phase == 0u);
    assert(t.bpm == FMT_TRANSPORT_DEFAULT_BPM);
    assert(t.step == 0u);
    assert(t.running == 0u);
    assert(sizeof(FmtTransport) == 8u);

    fmt_transport_reset((FmtTransport *)0);
    fmt_transport_start((FmtTransport *)0);
    fmt_transport_stop((FmtTransport *)0);
    assert(fmt_transport_set_bpm((FmtTransport *)0, 120u) == 0);
    assert(fmt_transport_advance((FmtTransport *)0, 256u) == 0u);
    assert(fmt_transport_tick_frame((FmtTransport *)0) == 0u);
}

static void test_stopped_start_stop_resume(void)
{
    FmtTransport t;
    uint32_t phase, step;

    fmt_transport_reset(&t);
    assert(fmt_transport_advance(&t, 100000u) == 0u);
    assert(t.phase == 0u && t.step == 0u);

    fmt_transport_start(&t);
    assert(t.running == 1u);
    assert(fmt_transport_advance(&t, 1000u) == 0u);
    phase = t.phase;
    step = t.step;
    assert(phase == 120000u);

    fmt_transport_stop(&t);
    assert(t.running == 0u);
    assert(fmt_transport_advance(&t, 100000u) == 0u);
    assert(t.phase == phase && t.step == step);

    fmt_transport_start(&t);
    assert(t.running == 1u);
    assert(fmt_transport_advance(&t, 4512u) == 0u);
    assert(t.phase == 661440u);
    assert(fmt_transport_tick_frame(&t) == 1u);
    assert(t.step == 1u);
    assert(t.phase == 60u);

    fmt_transport_stop(&t);
    fmt_transport_reset(&t);
    assert(t.phase == 0u && t.step == 0u && t.running == 0u &&
           t.bpm == FMT_TRANSPORT_DEFAULT_BPM);
}

static void test_120_bpm_fractional_boundary(void)
{
    FmtTransport t;

    fmt_transport_reset(&t);
    fmt_transport_start(&t);

    /* 661500 / 120 = 5512.5 frames: exact remainder accumulation must
     * alternate the two integer boundary distances without drift. */
    assert(fmt_transport_advance(&t, 5512u) == 0u);
    assert(t.step == 0u && t.phase == 661440u);
    assert(fmt_transport_tick_frame(&t) == 1u);
    assert(t.step == 1u && t.phase == 60u);

    assert(fmt_transport_advance(&t, 5511u) == 0u);
    assert(t.phase == 661380u);
    assert(fmt_transport_tick_frame(&t) == 1u);
    assert(t.step == 2u && t.phase == 0u);
}

static void test_tempo_change_preserves_phase(void)
{
    FmtTransport t;
    uint32_t phase;
    uint16_t bpm;

    fmt_transport_reset(&t);
    fmt_transport_start(&t);
    assert(fmt_transport_advance(&t, 1234u) == 0u);
    phase = t.phase;
    bpm = t.bpm;

    assert(fmt_transport_set_bpm(&t, 0u) == 0);
    assert(t.bpm == bpm && t.phase == phase);

    assert(fmt_transport_set_bpm(&t, 137u) == 1);
    assert(t.bpm == 137u);
    assert(t.phase == phase);
    assert(t.running == 1u);
}

static void test_exact_64_step_wrap(void)
{
    FmtTransport t;
    uint32_t boundaries = 0u;

    fmt_transport_reset(&t);
    assert(fmt_transport_set_bpm(&t, 137u) == 1);
    fmt_transport_start(&t);

    while (boundaries < 63u) {
        if (fmt_transport_tick_frame(&t))
            boundaries++;
    }
    assert(t.step == 63u);

    while (!fmt_transport_tick_frame(&t))
        ;
    boundaries++;
    assert(boundaries == 64u);
    assert(t.step == 0u);
    assert(t.phase < FMT_TRANSPORT_PHASE_PER_STEP);
}

static void test_bulk_matches_per_frame(void)
{
    static const uint16_t bpms[] = {31u, 120u, 137u, 255u, 997u, 65535u};
    static const uint32_t frames[] = {1u, 255u, 256u, 257u, 5512u, 12345u, 100000u};
    uint32_t b, f;

    for (b = 0; b < (uint32_t)(sizeof(bpms) / sizeof(bpms[0])); b++) {
        for (f = 0; f < (uint32_t)(sizeof(frames) / sizeof(frames[0])); f++) {
            FmtTransport bulk, tick;
            uint32_t bulk_edges, tick_edges = 0u, i;

            fmt_transport_reset(&bulk);
            fmt_transport_reset(&tick);
            assert(fmt_transport_set_bpm(&bulk, bpms[b]) == 1);
            assert(fmt_transport_set_bpm(&tick, bpms[b]) == 1);
            fmt_transport_start(&bulk);
            fmt_transport_start(&tick);

            bulk_edges = fmt_transport_advance(&bulk, frames[f]);
            for (i = 0; i < frames[f]; i++)
                tick_edges += fmt_transport_tick_frame(&tick);

            assert(bulk_edges == tick_edges);
            assert(bulk.phase == tick.phase);
            assert(bulk.step == tick.step);
            assert(bulk.bpm == tick.bpm);
            assert(bulk.running == tick.running);
        }
    }
}

static void check_long_run(uint16_t bpm, uint32_t frames)
{
    FmtTransport t;
    uint64_t exact_phase;
    uint32_t expected_edges, expected_phase;
    uint32_t actual_edges;

    fmt_transport_reset(&t);
    assert(fmt_transport_set_bpm(&t, bpm) == 1);
    fmt_transport_start(&t);

    actual_edges = fmt_transport_advance(&t, frames);
    exact_phase = (uint64_t)frames * (uint64_t)bpm;
    expected_edges = (uint32_t)(exact_phase / (uint64_t)FMT_TRANSPORT_PHASE_PER_STEP);
    expected_phase = (uint32_t)(exact_phase % (uint64_t)FMT_TRANSPORT_PHASE_PER_STEP);

    assert(actual_edges == expected_edges);
    assert(t.phase == expected_phase);
    assert(t.step == (uint8_t)(expected_edges % FMT_STEPS_PER_PATTERN));
    assert(t.phase < FMT_TRANSPORT_PHASE_PER_STEP);
}

static void test_long_run_no_drift(void)
{
    const uint32_t one_hour = FMT_TRANSPORT_SAMPLE_RATE * 60u * 60u;

    /* Independent host-only 64-bit reference. Firmware transport code itself
     * contains no 64-bit arithmetic or division. */
    check_long_run(31u, one_hour);
    check_long_run(60u, one_hour);
    check_long_run(120u, one_hour);
    check_long_run(137u, one_hour);
    check_long_run(255u, one_hour);
    check_long_run(997u, one_hour);

    /* Also exercise the full uint16 BPM range for one minute. */
    check_long_run(65535u, FMT_TRANSPORT_SAMPLE_RATE * 60u);
}

int main(void)
{
    test_reset_and_null_safety();
    test_stopped_start_stop_resume();
    test_120_bpm_fractional_boundary();
    test_tempo_change_preserves_phase();
    test_exact_64_step_wrap();
    test_bulk_matches_per_frame();
    test_long_run_no_drift();

    printf("FMT_TRANSPORT_TESTS_OK=1\n");
    printf("phase_per_step=%u default_bpm=%u state_bytes=%zu long_run_hours=6 max_bpm=65535\n",
           (unsigned)FMT_TRANSPORT_PHASE_PER_STEP,
           (unsigned)FMT_TRANSPORT_DEFAULT_BPM,
           sizeof(FmtTransport));
    return 0;
}
