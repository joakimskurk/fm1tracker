/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_tracker_screen.h"

static uint16_t fmt_tracker_screen_region_bit(uint32_t region)
{
    if (region >= FMT_TRACKER_SCREEN_REGION_COUNT)
        return 0u;
    return (uint16_t)(1u << region);
}

static int fmt_tracker_screen_visible_row(uint8_t step,
                                          uint8_t window_start,
                                          uint8_t *row)
{
    uint8_t offset;

    if (step < window_start)
        return 0;
    offset = (uint8_t)(step - window_start);
    if (offset >= FMT_TRACKER_SCREEN_ROWS)
        return 0;
    if (row)
        *row = offset;
    return 1;
}

static void fmt_tracker_screen_mark_step_dirty(FmtTrackerScreen *screen,
                                               uint8_t step,
                                               uint8_t window_start)
{
    uint8_t row;

    if (screen && fmt_tracker_screen_visible_row(step, window_start, &row))
        screen->dirty |= fmt_tracker_screen_region_bit(FMT_TRACKER_SCREEN_REGION_ROW0 + row);
}

static void fmt_tracker_screen_copy(char **dst, const char *src)
{
    while (*src)
        *(*dst)++ = *src++;
}

static void fmt_tracker_screen_two_digits(char **dst, uint32_t value)
{
    uint32_t tens = 0u;

    while (value >= 10u) {
        value -= 10u;
        tens++;
    }
    *(*dst)++ = (char)('0' + tens);
    *(*dst)++ = (char)('0' + value);
}

static void fmt_tracker_screen_three_digits(char out[4], uint32_t value)
{
    uint32_t hundreds = 0u, tens = 0u;

    while (value >= 100u) {
        value -= 100u;
        hundreds++;
    }
    while (value >= 10u) {
        value -= 10u;
        tens++;
    }
    out[0] = (char)('0' + hundreds);
    out[1] = (char)('0' + tens);
    out[2] = (char)('0' + value);
    out[3] = 0;
}

static void fmt_tracker_screen_u16_dec(char **dst, uint16_t value)
{
    static const uint16_t place[5] = {10000u, 1000u, 100u, 10u, 1u};
    uint32_t i;
    int started = 0;

    for (i = 0u; i < 5u; i++) {
        uint8_t digit = 0u;
        while (value >= place[i]) {
            value = (uint16_t)(value - place[i]);
            digit++;
        }
        if (started || digit || i == 4u) {
            *(*dst)++ = (char)('0' + digit);
            started = 1;
        }
    }
}

void fmt_tracker_screen_reset(FmtTrackerScreen *screen)
{
    uint32_t i;

    if (!screen)
        return;

    screen->dirty = 0u;
    screen->bpm = 0u;
    screen->active = 0u;
    screen->step_rec = 0u;
    screen->track = 0xFFu;
    screen->page = 0xFFu;
    screen->window_start = 0xFFu;
    screen->selected_step = 0xFFu;
    screen->play_step = 0xFFu;
    screen->playing = 0u;

    for (i = 0u; i < FMT_TRACKER_SCREEN_ROWS; i++) {
        screen->row_note[i] = 0u;
        screen->row_instrument[i] = 0u;
        screen->row_volume[i] = 0u;
        screen->row_effect[i] = 0u;
    }
}

uint8_t fmt_tracker_screen_window_start(const FmtPatternInput *input)
{
    uint8_t start;

    if (!input || input->page >= FMT_PATTERN_INPUT_PAGE_COUNT ||
        input->local_step >= FMT_PATTERN_INPUT_STEPS_PER_PAGE)
        return 0u;

    start = (uint8_t)(input->page * FMT_PATTERN_INPUT_STEPS_PER_PAGE);
    if (input->local_step >= FMT_TRACKER_SCREEN_ROWS)
        start = (uint8_t)(start + FMT_TRACKER_SCREEN_ROWS);
    return start;
}

int fmt_tracker_screen_sync(FmtTrackerScreen *screen,
                            const FmtPatternInput *input,
                            const FmtStepRecord *record,
                            const FmtTransport *transport)
{
    uint8_t track, page, window_start, selected_step, play_step, playing, step_rec;
    uint8_t old_selected, old_play, old_playing, old_window;
    uint16_t dirty;
    uint32_t row;
    int full;

    if (!screen)
        return 0;

    if (!input || !transport || !fmt_pattern_input_is_pattern(input) ||
        input->page >= FMT_PATTERN_INPUT_PAGE_COUNT ||
        input->local_step >= FMT_PATTERN_INPUT_STEPS_PER_PAGE ||
        input->selected_track >= FMT_PATTERN_INPUT_TRACK_COUNT) {
        screen->active = 0u;
        screen->dirty = 0u;
        return 0;
    }

    track = input->selected_track;
    page = input->page;
    window_start = fmt_tracker_screen_window_start(input);
    selected_step = fmt_pattern_input_global_step(input);
    play_step = transport->step < FMT_STEPS_PER_PATTERN ? transport->step : 0u;
    playing = (uint8_t)(transport->running && transport->step < FMT_STEPS_PER_PATTERN);
    step_rec = (uint8_t)(record && fmt_step_record_is_active(record));

    old_selected = screen->selected_step;
    old_play = screen->play_step;
    old_playing = screen->playing;
    old_window = screen->window_start;
    dirty = screen->dirty;

    full = !screen->active || screen->track != track || screen->page != page ||
           screen->window_start != window_start || screen->step_rec != step_rec;

    if (full) {
        dirty |= FMT_TRACKER_SCREEN_ALL_DIRTY;
    } else {
        if (old_selected != selected_step) {
            dirty |= fmt_tracker_screen_region_bit(FMT_TRACKER_SCREEN_REGION_HEADER);
            fmt_tracker_screen_mark_step_dirty(screen, old_selected, old_window);
            fmt_tracker_screen_mark_step_dirty(screen, selected_step, window_start);
            dirty |= screen->dirty;
        }

        if (old_playing != playing || old_play != play_step) {
            dirty |= fmt_tracker_screen_region_bit(FMT_TRACKER_SCREEN_REGION_HEADER);
            if (old_playing)
                fmt_tracker_screen_mark_step_dirty(screen, old_play, old_window);
            if (playing)
                fmt_tracker_screen_mark_step_dirty(screen, play_step, window_start);
            dirty |= screen->dirty;
        }

        if (screen->bpm != transport->bpm)
            dirty |= fmt_tracker_screen_region_bit(FMT_TRACKER_SCREEN_REGION_HEADER);
    }

    for (row = 0u; row < FMT_TRACKER_SCREEN_ROWS; row++) {
        const FmtStep *step = fmt_store_step_const(FMT_TRACKER_SCREEN_PATTERN,
                                                   track,
                                                   (uint32_t)window_start + row);
        uint8_t note = step ? step->note : 0xFCu;
        uint8_t instrument = step ? step->instrument : 0xFEu;
        uint8_t volume = step ? step->volume : 0xFEu;
        uint8_t effect = step ? step->effect : 0xFFu;

        if (!full && (screen->row_note[row] != note ||
                      screen->row_instrument[row] != instrument ||
                      screen->row_volume[row] != volume ||
                      screen->row_effect[row] != effect))
            dirty |= fmt_tracker_screen_region_bit(FMT_TRACKER_SCREEN_REGION_ROW0 + row);

        screen->row_note[row] = note;
        screen->row_instrument[row] = instrument;
        screen->row_volume[row] = volume;
        screen->row_effect[row] = effect;
    }

    screen->active = 1u;
    screen->step_rec = step_rec;
    screen->track = track;
    screen->page = page;
    screen->window_start = window_start;
    screen->selected_step = selected_step;
    screen->play_step = play_step;
    screen->playing = playing;
    screen->bpm = transport->bpm;
    screen->dirty = dirty;
    return 1;
}

uint16_t fmt_tracker_screen_dirty(const FmtTrackerScreen *screen)
{
    return screen ? screen->dirty : 0u;
}

int fmt_tracker_screen_take_region(FmtTrackerScreen *screen, uint8_t *region)
{
    uint32_t i;

    if (!screen || !region || !screen->dirty)
        return 0;

    for (i = 0u; i < FMT_TRACKER_SCREEN_REGION_COUNT; i++) {
        uint16_t bit = fmt_tracker_screen_region_bit(i);
        if (screen->dirty & bit) {
            screen->dirty = (uint16_t)(screen->dirty & (uint16_t)~bit);
            *region = (uint8_t)i;
            return 1;
        }
    }
    return 0;
}

int fmt_tracker_screen_region_to_row(uint32_t region, uint8_t *row)
{
    if (row)
        *row = 0u;
    if (region < FMT_TRACKER_SCREEN_REGION_ROW0 ||
        region >= FMT_TRACKER_SCREEN_REGION_COUNT)
        return 0;
    if (row)
        *row = (uint8_t)(region - FMT_TRACKER_SCREEN_REGION_ROW0);
    return 1;
}

void fmt_tracker_screen_markers(const FmtTrackerScreen *screen,
                                uint32_t visible_row,
                                char out[3])
{
    uint8_t step;

    if (!out)
        return;
    out[0] = ' ';
    out[1] = ' ';
    out[2] = 0;

    if (!screen || !screen->active || visible_row >= FMT_TRACKER_SCREEN_ROWS)
        return;

    step = (uint8_t)(screen->window_start + visible_row);
    if (screen->selected_step == step)
        out[0] = '>';
    if (screen->playing && screen->play_step == step)
        out[1] = '*';
}

void fmt_tracker_screen_note_text(uint32_t note, char out[5])
{
    static const char letter[12] = {'C','C','D','D','E','F','F','G','G','A','A','B'};
    static const uint8_t sharp[12] = {0u,1u,0u,1u,0u,0u,1u,0u,1u,0u,1u,0u};
    uint32_t semitone;
    int32_t octave = -1;

    if (!out)
        return;

    if (note == FMT_NOTE_EMPTY) {
        out[0] = '-'; out[1] = '-'; out[2] = '-'; out[3] = ' '; out[4] = 0;
        return;
    }
    if (note == FMT_NOTE_TIE) {
        out[0] = '='; out[1] = '='; out[2] = '='; out[3] = ' '; out[4] = 0;
        return;
    }
    if (note == FMT_NOTE_OFF) {
        out[0] = 'O'; out[1] = 'F'; out[2] = 'F'; out[3] = ' '; out[4] = 0;
        return;
    }
    if (note > FMT_NOTE_MAX) {
        out[0] = '?'; out[1] = '?'; out[2] = '?'; out[3] = ' '; out[4] = 0;
        return;
    }

    semitone = note;
    while (semitone >= 12u) {
        semitone -= 12u;
        octave++;
    }

    out[0] = letter[semitone];
    out[1] = sharp[semitone] ? '#' : '-';
    if (octave < 0) {
        out[2] = '-';
        out[3] = '1';
    } else {
        out[2] = (char)('0' + octave);
        out[3] = ' ';
    }
    out[4] = 0;
}

void fmt_tracker_screen_instrument_text(uint32_t instrument, char out[3])
{
    char *p;

    if (!out)
        return;
    if (instrument == FMT_INSTRUMENT_INHERIT) {
        out[0] = '.'; out[1] = '.'; out[2] = 0;
        return;
    }
    if (instrument >= FMT_INSTRUMENTS) {
        out[0] = '?'; out[1] = '?'; out[2] = 0;
        return;
    }
    p = out;
    fmt_tracker_screen_two_digits(&p, instrument + 1u);
    *p = 0;
}

void fmt_tracker_screen_volume_text(uint32_t volume, char out[4])
{
    if (!out)
        return;
    if (volume == FMT_VOLUME_INHERIT) {
        out[0] = '.'; out[1] = '.'; out[2] = '.'; out[3] = 0;
        return;
    }
    if (volume > 254u) {
        out[0] = '?'; out[1] = '?'; out[2] = '?'; out[3] = 0;
        return;
    }
    fmt_tracker_screen_three_digits(out, volume);
}

void fmt_tracker_screen_effect_text(uint32_t effect, char out[4])
{
    if (!out)
        return;
    if (effect == FMT_EFFECT_NONE) {
        out[0] = '-'; out[1] = '-'; out[2] = '-'; out[3] = 0;
    } else {
        out[0] = '?'; out[1] = '?'; out[2] = '?'; out[3] = 0;
    }
}

void fmt_tracker_screen_row_text(const FmtTrackerScreen *screen,
                                 uint32_t visible_row,
                                 char out[32])
{
    char note[5], instrument[3], volume[4], effect[4];
    char *p;

    if (!out)
        return;
    out[0] = 0;
    if (!screen || !screen->active || visible_row >= FMT_TRACKER_SCREEN_ROWS)
        return;

    fmt_tracker_screen_note_text(screen->row_note[visible_row], note);
    fmt_tracker_screen_instrument_text(screen->row_instrument[visible_row], instrument);
    fmt_tracker_screen_volume_text(screen->row_volume[visible_row], volume);
    fmt_tracker_screen_effect_text(screen->row_effect[visible_row], effect);

    p = out;
    fmt_tracker_screen_two_digits(&p, (uint32_t)screen->window_start + visible_row + 1u);
    *p++ = ' ';
    fmt_tracker_screen_copy(&p, note);
    *p++ = ' ';
    fmt_tracker_screen_copy(&p, instrument);
    *p++ = ' ';
    fmt_tracker_screen_copy(&p, volume);
    *p++ = ' ';
    fmt_tracker_screen_copy(&p, effect);
    *p = 0;
}

void fmt_tracker_screen_header_line1(const FmtTrackerScreen *screen, char out[32])
{
    char *p;

    if (!out)
        return;
    out[0] = 0;
    if (!screen || !screen->active)
        return;

    p = out;
    fmt_tracker_screen_copy(&p, screen->step_rec ? "STEP REC " : "SEQ ");
    fmt_tracker_screen_copy(&p, "CUR");
    fmt_tracker_screen_two_digits(&p, (uint32_t)screen->selected_step + 1u);
    *p++ = ' ';
    if (screen->playing) {
        fmt_tracker_screen_copy(&p, "PLY");
        fmt_tracker_screen_two_digits(&p, (uint32_t)screen->play_step + 1u);
    } else {
        fmt_tracker_screen_copy(&p, "STOP");
    }
    *p++ = ' ';
    fmt_tracker_screen_u16_dec(&p, screen->bpm);
    *p = 0;
}

void fmt_tracker_screen_header_line2(const FmtTrackerScreen *screen, char out[32])
{
    char *p;

    if (!out)
        return;
    out[0] = 0;
    if (!screen || !screen->active)
        return;

    p = out;
    fmt_tracker_screen_copy(&p, "P01 T");
    *p++ = (char)('1' + screen->track);
    fmt_tracker_screen_copy(&p, " PG");
    *p++ = (char)('1' + screen->page);
    fmt_tracker_screen_copy(&p, "/4 ");
    fmt_tracker_screen_two_digits(&p, (uint32_t)screen->window_start + 1u);
    *p++ = '-';
    fmt_tracker_screen_two_digits(&p, (uint32_t)screen->window_start + FMT_TRACKER_SCREEN_ROWS);
    *p = 0;
}

void fmt_tracker_screen_header_line3(char out[32])
{
    char *p;

    if (!out)
        return;
    p = out;
    fmt_tracker_screen_copy(&p, "ST NOTE IN VOL FX");
    *p = 0;
}

void fmt_tracker_screen_footer(const FmtTrackerScreen *screen, char out[32])
{
    char *p;

    if (!out)
        return;
    out[0] = 0;
    if (!screen || !screen->active)
        return;
    p = out;
    fmt_tracker_screen_copy(&p, screen->step_rec ?
                            "FX RST  SEL TIE  FX+SEL OFF" :
                            "KEYS STEP/TRK  OCT PAGE");
    *p = 0;
}
