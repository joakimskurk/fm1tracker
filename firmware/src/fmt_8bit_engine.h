/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_8BIT_ENGINE_H
#define FM1TRACKER_FMT_8BIT_ENGINE_H

#include <stdint.h>

#include "fmt_8bit_params.h"
#include "fmt_envelope.h"
#include "fmt_osc_noise.h"
#include "fmt_osc_pulse.h"
#include "fmt_osc_triangle.h"
#include "fmt_osc_wavetable8.h"
#include "fmt_voice_allocator.h"

/* D045 Gate 2: pure fixed-state linked 8-BIT engine. This module deliberately
 * knows nothing about song/editor/cache ownership and is not linked from
 * dinghy.c until Gate 4. */
typedef union {
    FmtPulseOsc pulse;
    FmtNoiseOsc noise;
    FmtTriangleOsc triangle;
    FmtWavetable8Osc wavetable8;
} Fmt8BitOscState;

typedef struct {
    FmtEnvelope envelope;
    Fmt8BitOscState osc;
    uint32_t phase_inc;
    uint32_t page_word;
    uint16_t pitch_q7;
    uint16_t gain_q15;
    uint8_t engine;
    uint8_t instrument;
    uint8_t reserved[2];
} Fmt8BitVoice;

typedef struct {
    FmtVoiceAllocator allocator;
    Fmt8BitVoice voice[FMT_VOICE_ALLOCATOR_SLOTS];
} Fmt8BitEngine;

#define FMT_8BIT_ENGINE_STATIC_ASSERT(name, expr) \
    typedef char fmt_8bit_engine_static_assert_##name[(expr) ? 1 : -1]
FMT_8BIT_ENGINE_STATIC_ASSERT(osc_state_is_8_bytes,
                              sizeof(Fmt8BitOscState) == 8u);
FMT_8BIT_ENGINE_STATIC_ASSERT(voice_is_44_bytes,
                              sizeof(Fmt8BitVoice) == 44u);
FMT_8BIT_ENGINE_STATIC_ASSERT(engine_is_404_bytes,
                              sizeof(Fmt8BitEngine) == 404u);

void fmt_8bit_engine_reset(Fmt8BitEngine *engine);

/* NOTE allocates/reuses one D043 primary voice. Inputs are already resolved at
 * an event boundary; no store/cache traversal occurs here. Returns voice slot
 * 0..7 or FMT_VOICE_NONE. Unsupported/malformed NOTE fails toward silence for
 * the affected track and releases its previous primary ownership. */
uint8_t fmt_8bit_engine_note(Fmt8BitEngine *engine,
                             uint8_t track,
                             uint8_t engine_id,
                             uint8_t instrument,
                             uint16_t pitch_q7,
                             uint16_t gain_q15,
                             uint32_t page_word,
                             const Fmt8BitParamSet *base);

/* RETRIG never allocates. It resets oscillator state and hard-restarts D042 on
 * the currently owned voice using the current row snapshots. */
uint8_t fmt_8bit_engine_retrigger(Fmt8BitEngine *engine,
                                  uint8_t track,
                                  uint16_t pitch_q7,
                                  uint16_t gain_q15,
                                  uint32_t page_word,
                                  const Fmt8BitParamSet *base);

/* TIE/EMPTY row update: live pitch/gain/page changes only; no envelope or
 * oscillator reset. Returns the owned voice or FMT_VOICE_NONE. */
uint8_t fmt_8bit_engine_update_row(Fmt8BitEngine *engine,
                                   uint8_t track,
                                   uint16_t pitch_q7,
                                   uint16_t gain_q15,
                                   uint32_t page_word);

/* OFF/CUT share D042 release semantics. The current row page/gain are snapped,
 * release is started, and D043 ownership detaches in the same event. */
uint8_t fmt_8bit_engine_release(Fmt8BitEngine *engine,
                                uint8_t track,
                                uint16_t gain_q15,
                                uint32_t page_word);

/* Explicit safe-silence helper used for malformed/unsupported NOTE handling. */
uint8_t fmt_8bit_engine_silence_track(Fmt8BitEngine *engine, uint8_t track);

/* Render exactly eight fixed slots in ascending order and clamp mono Q15. */
int16_t fmt_8bit_engine_render(Fmt8BitEngine *engine);

/* Quiet only when every D042 envelope is IDLE, including release tails. */
uint8_t fmt_8bit_engine_quiet(const Fmt8BitEngine *engine);

/* Bounded owner lookup for integration/tests. */
uint8_t fmt_8bit_engine_track_voice(const Fmt8BitEngine *engine,
                                    uint8_t track);

#endif /* FM1TRACKER_FMT_8BIT_ENGINE_H */
