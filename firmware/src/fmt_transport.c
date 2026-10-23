/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_transport.h"

void fmt_transport_reset(FmtTransport *transport)
{
    if (!transport)
        return;

    transport->phase = 0u;
    transport->bpm = (uint16_t)FMT_TRANSPORT_DEFAULT_BPM;
    transport->step = 0u;
    transport->running = 0u;
}

int fmt_transport_set_bpm(FmtTransport *transport, uint16_t bpm)
{
    if (!transport || bpm == 0u)
        return 0;

    /* Preserve phase: a tempo edit changes the rate from this musical point,
     * rather than jumping to another fraction of the current step. */
    transport->bpm = bpm;
    return 1;
}

void fmt_transport_start(FmtTransport *transport)
{
    if (!transport || transport->bpm == 0u)
        return;
    transport->running = 1u;
}

void fmt_transport_stop(FmtTransport *transport)
{
    if (!transport)
        return;
    transport->running = 0u;
}

uint32_t fmt_transport_advance(FmtTransport *transport, uint32_t frames)
{
    uint32_t advanced = 0u;

    if (!transport || !transport->running || frames == 0u)
        return 0u;

    while (frames) {
        uint32_t chunk = frames > FMT_TRANSPORT_ADVANCE_CHUNK_FRAMES
                       ? FMT_TRANSPORT_ADVANCE_CHUNK_FRAMES : frames;
        uint32_t phase = transport->phase + chunk * (uint32_t)transport->bpm;

        /* No division: retain the exact remainder. With a 256-frame chunk and
         * 16-bit BPM this loop is bounded to at most 26 boundary crossings. */
        while (phase >= FMT_TRANSPORT_PHASE_PER_STEP) {
            phase -= FMT_TRANSPORT_PHASE_PER_STEP;
            transport->step++;
            if (transport->step >= FMT_STEPS_PER_PATTERN)
                transport->step = 0u;
            advanced++;
        }

        transport->phase = phase;
        frames -= chunk;
    }

    return advanced;
}
