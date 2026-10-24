/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FMT_ENGINE_PITCH_H
#define FMT_ENGINE_PITCH_H

#include <stdint.h>

#define FMT_ENGINE_PITCH_MAX_Q7 16383u

/* D038: canonical Phase 4 Q7 semitone pitch -> 32-bit phase increment.
 * Legal public input is 0..16383. Malformed higher values clamp to the legal
 * maximum. The private implementation may use MIDI 128 only as the upper
 * interpolation lookup for the legal maximum fractional pitch. */
uint32_t fmt_engine_pitch_inc_q7(uint16_t pitch_q7);

#endif
