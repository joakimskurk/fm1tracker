/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_engine_pitch.h"

/* Exact Phase 3 proof-lane C9 table. Keep these integers frozen under D038. */
static const uint32_t fmt_engine_pitch_inc_c9[12] = {
    815363807u, 863847862u, 915214929u, 969636441u,
    1027294024u, 1088380105u, 1153098554u, 1221665363u,
    1294309365u, 1371273005u, 1452813141u, 1539201906u
};

static uint32_t fmt_engine_pitch_note_inc(uint32_t note)
{
    return fmt_engine_pitch_inc_c9[note % 12u] >> (10u - note / 12u);
}

static uint32_t fmt_engine_pitch_interp(uint32_t lo,
                                        uint32_t hi,
                                        uint8_t frac_q7)
{
    uint32_t diff, q, r;

    if (frac_q7 == 0u || hi <= lo)
        return lo;

    diff = hi - lo;
    q = diff >> 7;
    r = diff & 127u;
    return lo + q * (uint32_t)frac_q7 +
           ((r * (uint32_t)frac_q7 + 64u) >> 7);
}

uint32_t fmt_engine_pitch_inc_q7(uint16_t pitch_q7)
{
    uint32_t pitch = pitch_q7;
    uint32_t note, lo;
    uint8_t frac;

    if (pitch > FMT_ENGINE_PITCH_MAX_Q7)
        pitch = FMT_ENGINE_PITCH_MAX_Q7;

    note = pitch >> 7;
    frac = (uint8_t)(pitch & 127u);
    lo = fmt_engine_pitch_note_inc(note);

    if (!frac)
        return lo;

    return fmt_engine_pitch_interp(lo,
                                   fmt_engine_pitch_note_inc(note + 1u),
                                   frac);
}
