/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FMT_FM_ENGINE_H
#define FMT_FM_ENGINE_H

#include <stdint.h>

#include "fmt_fm_algorithm.h"
#include "fmt_fm_controls.h"
#include "fmt_fm_envelope.h"
#include "fmt_fm_feedback.h"
#include "fmt_voice_allocator.h"

#define FMT_FM_ENGINE_FM4 0u
#define FMT_FM_ENGINE_SLOTS 8u
#define FMT_FM_ENGINE_TRACKS 8u
#define FMT_FM_ENGINE_INSTRUMENTS 16u
#define FMT_FM_ENGINE_GAIN_MAX_Q15 32767u

typedef struct {
    uint8_t ratio_id[4];
    uint8_t level_control[4];
    FmtFmEnvelopeControls envelope;
    uint8_t algorithm;
    uint8_t index_control;
    uint8_t feedback_control;
    uint8_t reserved;
} FmtFmPatch;

typedef struct {
    FmtFmAlgorithmState algorithm_state;
    FmtFmEnvelopeState envelope_state;
    FmtFmFeedbackState feedback_state;
    uint16_t alignment_pad;
    uint32_t phase_increment[4];
    uint16_t static_level_q15[4];
    FmtFmEnvelopeControls envelope_controls;
    uint16_t mod_depth_q14;
    uint16_t gain_q15;
    uint8_t ratio_id[4];
    uint8_t feedback_control;
    uint8_t algorithm;
    uint8_t instrument;
    uint8_t reserved;
} FmtFmVoice;

typedef struct {
    FmtVoiceAllocator allocator;
    FmtFmVoice voice[FMT_FM_ENGINE_SLOTS];
} FmtFmEngine;

#define FMT_FM_ENGINE_STATIC_ASSERT(name, expr) \
    typedef char fmt_fm_engine_static_assert_##name[(expr) ? 1 : -1]
FMT_FM_ENGINE_STATIC_ASSERT(patch_is_28_bytes, sizeof(FmtFmPatch) == 28u);
FMT_FM_ENGINE_STATIC_ASSERT(voice_is_92_bytes, sizeof(FmtFmVoice) == 92u);
FMT_FM_ENGINE_STATIC_ASSERT(allocator_is_52_bytes,
                            sizeof(FmtVoiceAllocator) == 52u);
FMT_FM_ENGINE_STATIC_ASSERT(engine_is_788_bytes, sizeof(FmtFmEngine) == 788u);

uint8_t fmt_fm_engine_supported(uint8_t engine_id);
const FmtFmPatch *fmt_fm_engine_default_patch(void);
void fmt_fm_engine_reset(FmtFmEngine *engine);

uint8_t fmt_fm_engine_note(FmtFmEngine *engine,
                           uint8_t track,
                           uint8_t engine_id,
                           uint8_t instrument,
                           uint16_t pitch_q7,
                           uint16_t gain_q15,
                           const FmtFmPatch *patch);

uint8_t fmt_fm_engine_update_row(FmtFmEngine *engine,
                                 uint8_t track,
                                 uint8_t instrument,
                                 uint16_t pitch_q7,
                                 uint16_t gain_q15,
                                 const FmtFmPatch *patch);

uint8_t fmt_fm_engine_retrigger(FmtFmEngine *engine,
                                uint8_t track,
                                uint8_t instrument,
                                uint16_t pitch_q7,
                                uint16_t gain_q15,
                                const FmtFmPatch *patch);

uint8_t fmt_fm_engine_release(FmtFmEngine *engine,
                              uint8_t track,
                              uint16_t gain_q15,
                              const FmtFmPatch *patch);

uint8_t fmt_fm_engine_silence_track(FmtFmEngine *engine, uint8_t track);
int16_t fmt_fm_engine_render(FmtFmEngine *engine);
uint8_t fmt_fm_engine_quiet(const FmtFmEngine *engine);
uint8_t fmt_fm_engine_track_voice(const FmtFmEngine *engine, uint8_t track);

#endif
