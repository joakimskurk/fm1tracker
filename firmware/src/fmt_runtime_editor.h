/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_RUNTIME_EDITOR_H
#define FM1TRACKER_FMT_RUNTIME_EDITOR_H

#include <stdint.h>

#include "fmt_8bit_linked_params.h"
#include "fmt_engine_router.h"
#include "fmt_fm_linked_params.h"
#include "fmt_pattern_input.h"

typedef struct {
    uint8_t active;
    uint8_t page;
    uint8_t dirty;
    uint8_t reserved;
} FmtRuntimeEditor;

#define FMT_RUNTIME_EDITOR_ENGINE_COUNT 5u

enum {
    FMT_RUNTIME_EDITOR_ENGINE_PULSE = 0u,
    FMT_RUNTIME_EDITOR_ENGINE_NOISE = 1u,
    FMT_RUNTIME_EDITOR_ENGINE_TRIANGLE = 2u,
    FMT_RUNTIME_EDITOR_ENGINE_WAVETABLE8 = 3u,
    FMT_RUNTIME_EDITOR_ENGINE_FM4 = 4u
};

void fmt_runtime_editor_reset(FmtRuntimeEditor *editor);
void fmt_runtime_editor_enter(FmtRuntimeEditor *editor);
void fmt_runtime_editor_exit(FmtRuntimeEditor *editor);
void fmt_runtime_editor_mark_clean(FmtRuntimeEditor *editor);

int fmt_runtime_editor_select_track(FmtRuntimeEditor *editor,
                                    FmtPatternInput *input,
                                    int32_t delta);
int fmt_runtime_editor_select_instrument(FmtRuntimeEditor *editor,
                                         FmtSong *song,
                                         const FmtPatternInput *input,
                                         int32_t delta);
int fmt_runtime_editor_select_engine(FmtRuntimeEditor *editor,
                                     FmtSong *song,
                                     FmtInstrumentIdentityPublication *publication,
                                     const FmtPatternInput *input,
                                     int32_t delta);
int fmt_runtime_editor_page_delta(FmtRuntimeEditor *editor,
                                  const FmtSong *song,
                                  const FmtPatternInput *input,
                                  int32_t delta);
int fmt_runtime_editor_env_page(FmtRuntimeEditor *editor,
                                const FmtSong *song,
                                const FmtPatternInput *input);
int fmt_runtime_editor_apply_knob(FmtRuntimeEditor *editor,
                                  FmtSong *song,
                                  const FmtPatternInput *input,
                                  uint32_t knob,
                                  int32_t delta);

uint8_t fmt_runtime_editor_instrument(const FmtSong *song,
                                      const FmtPatternInput *input);
const FmtInstrumentHeader *fmt_runtime_editor_header(const FmtSong *song,
                                                     const FmtPatternInput *input);
uint8_t fmt_runtime_editor_page_count(const FmtInstrumentHeader *header);
uint8_t fmt_runtime_editor_value(const FmtRuntimeEditor *editor,
                                 const FmtSong *song,
                                 const FmtPatternInput *input,
                                 uint32_t knob,
                                 uint8_t *value_out);

const char *fmt_runtime_editor_engine_name(const FmtInstrumentHeader *header);
const char *fmt_runtime_editor_page_name(const FmtRuntimeEditor *editor,
                                         const FmtInstrumentHeader *header);
const char *fmt_runtime_editor_slot_name(const FmtRuntimeEditor *editor,
                                         const FmtInstrumentHeader *header,
                                         uint32_t knob);

#endif /* FM1TRACKER_FMT_RUNTIME_EDITOR_H */
