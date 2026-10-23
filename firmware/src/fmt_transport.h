/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_TRANSPORT_H
#define FM1TRACKER_FMT_TRANSPORT_H

#include <stdint.h>

#include "fmt_model.h"

/* D020: exact V0.1 1/16-note timing in the 44.1 kHz frame domain.
 * One step lasts 44100 * 15 / BPM frames, so each rendered frame adds BPM
 * units to phase and a step boundary occurs at this fixed threshold. */
#define FMT_TRANSPORT_SAMPLE_RATE       44100u
#define FMT_TRANSPORT_PHASE_PER_STEP   661500u
#define FMT_TRANSPORT_DEFAULT_BPM          120u
#define FMT_TRANSPORT_ADVANCE_CHUNK_FRAMES 256u

typedef struct {
    uint32_t phase;
    uint16_t bpm;
    uint8_t step;
    uint8_t running;
} FmtTransport;

FMT_STATIC_ASSERT(transport_size_is_8, sizeof(FmtTransport) == 8u);
FMT_STATIC_ASSERT(transport_bpm_fits_one_boundary_per_frame,
                  0xFFFFu < FMT_TRANSPORT_PHASE_PER_STEP);

void fmt_transport_reset(FmtTransport *transport);
int fmt_transport_set_bpm(FmtTransport *transport, uint16_t bpm);
void fmt_transport_start(FmtTransport *transport);
void fmt_transport_stop(FmtTransport *transport);

/* Advance exactly one 44.1 kHz frame. Returns 1 iff this frame crosses a
 * 1/16-note boundary and the current step advances. This is timing only;
 * WP2.4 owns audible row dispatch. */
static inline uint8_t fmt_transport_tick_frame(FmtTransport *transport)
{
    uint32_t phase;

    if (!transport || !transport->running)
        return 0u;

    phase = transport->phase + (uint32_t)transport->bpm;
    if (phase < FMT_TRANSPORT_PHASE_PER_STEP) {
        transport->phase = phase;
        return 0u;
    }

    transport->phase = phase - FMT_TRANSPORT_PHASE_PER_STEP;
    transport->step++;
    if (transport->step >= FMT_STEPS_PER_PATTERN)
        transport->step = 0u;
    return 1u;
}

/* Bulk timing/state advance. Returns the number of crossed step boundaries.
 * It is mathematically equivalent to tick_frame() but processes at most 256
 * frames per chunk so every intermediate product remains safely 32-bit and
 * the real-time work stays explicitly bounded for the normal audio block. */
uint32_t fmt_transport_advance(FmtTransport *transport, uint32_t frames);

#endif /* FM1TRACKER_FMT_TRANSPORT_H */
