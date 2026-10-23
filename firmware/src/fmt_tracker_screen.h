/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_FMT_TRACKER_SCREEN_H
#define FM1TRACKER_FMT_TRACKER_SCREEN_H

#include <stdint.h>

#include "fmt_pattern_input.h"
#include "fmt_step_record.h"
#include "fmt_store.h"
#include "fmt_transport.h"

#define FMT_TRACKER_SCREEN_PATTERN              0u
#define FMT_TRACKER_SCREEN_ROWS                 8u
#define FMT_TRACKER_SCREEN_REGION_HEADER        0u
#define FMT_TRACKER_SCREEN_REGION_FOOTER        1u
#define FMT_TRACKER_SCREEN_REGION_ROW0          2u
#define FMT_TRACKER_SCREEN_REGION_COUNT        10u
#define FMT_TRACKER_SCREEN_MAX_REGIONS_PER_POLL 2u
#define FMT_TRACKER_SCREEN_ALL_DIRTY ((uint16_t)((1u << FMT_TRACKER_SCREEN_REGION_COUNT) - 1u))

/* D026 presentation cache. It is deliberately small ordinary-RAM state and
 * stores only values that can affect the current eight-row visual surface. */
typedef struct {
    uint16_t dirty;
    uint16_t bpm;
    uint8_t active;
    uint8_t step_rec;
    uint8_t track;
    uint8_t page;
    uint8_t window_start;
    uint8_t selected_step;
    uint8_t play_step;
    uint8_t playing;
    uint8_t row_note[FMT_TRACKER_SCREEN_ROWS];
    uint8_t row_instrument[FMT_TRACKER_SCREEN_ROWS];
    uint8_t row_volume[FMT_TRACKER_SCREEN_ROWS];
    uint8_t row_effect[FMT_TRACKER_SCREEN_ROWS];
} FmtTrackerScreen;

FMT_STATIC_ASSERT(tracker_screen_state_fits_64_bytes,
                  sizeof(FmtTrackerScreen) <= 64u);

void fmt_tracker_screen_reset(FmtTrackerScreen *screen);

/* D026 viewport: one fixed eight-row half of the current 16-step page, owned
 * by the edit cursor. Invalid/null input safely returns row 0. */
uint8_t fmt_tracker_screen_window_start(const FmtPatternInput *input);

/* Synchronize the presentation cache from the existing read-only editor,
 * transport and pattern store state. Returns 1 while tracker context is active.
 * Dirty bits accumulate until the renderer consumes them. */
int fmt_tracker_screen_sync(FmtTrackerScreen *screen,
                            const FmtPatternInput *input,
                            const FmtStepRecord *record,
                            const FmtTransport *transport);

uint16_t fmt_tracker_screen_dirty(const FmtTrackerScreen *screen);

/* Pop one pending region in deterministic priority order: header, footer,
 * then visible rows top-to-bottom. The main loop calls this no more than the
 * D026 MAX_REGIONS_PER_POLL limit. */
int fmt_tracker_screen_take_region(FmtTrackerScreen *screen, uint8_t *region);

/* Region id -> visible row 0..7. Header/footer/invalid ids are rejected. */
int fmt_tracker_screen_region_to_row(uint32_t region, uint8_t *row);

/* Independent cursor/playhead marker cells. out is always NUL terminated and
 * contains exactly two marker characters: '>'/space then '*'/space. */
void fmt_tracker_screen_markers(const FmtTrackerScreen *screen,
                                uint32_t visible_row,
                                char out[3]);

/* Deterministic fixed-width D026 text helpers. */
void fmt_tracker_screen_note_text(uint32_t note, char out[5]);
void fmt_tracker_screen_instrument_text(uint32_t instrument, char out[3]);
void fmt_tracker_screen_volume_text(uint32_t volume, char out[4]);
void fmt_tracker_screen_effect_text(uint32_t effect, char out[4]);
void fmt_tracker_screen_row_text(const FmtTrackerScreen *screen,
                                 uint32_t visible_row,
                                 char out[32]);
void fmt_tracker_screen_header_line1(const FmtTrackerScreen *screen, char out[32]);
void fmt_tracker_screen_header_line2(const FmtTrackerScreen *screen, char out[32]);
void fmt_tracker_screen_header_line3(char out[32]);
void fmt_tracker_screen_footer(const FmtTrackerScreen *screen, char out[32]);

#endif /* FM1TRACKER_FMT_TRACKER_SCREEN_H */
