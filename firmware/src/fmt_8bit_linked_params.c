/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_8bit_linked_params.h"

#include "fmt_lock_store.h"
#include "fmt_playback_params.h"
#include "fmt_store.h"

Fmt8BitParamSet fmt_8bit_instrument_base[FMT_INSTRUMENTS];
/* D048: this remains the single physical 8 x 64 x 32-bit engine-page
 * publication cache. Future families must reuse/generalize this storage rather
 * than allocate a parallel per-family 2,048-byte row cache. */
volatile Fmt8BitRowParamCache fmt_8bit_row_param_cache
    __attribute__((aligned(4)));

static volatile uint32_t *fmt_8bit_linked_slot(uint32_t track, uint32_t row)
{
    if (track >= FMT_TRACKS || row >= FMT_STEPS_PER_PATTERN)
        return 0;
    return &fmt_8bit_row_param_cache.word[track][row];
}

static uint8_t fmt_8bit_linked_row_context(const FmtSong *song,
                                           uint32_t track,
                                           uint32_t target_row,
                                           uint8_t *instrument_out,
                                           uint8_t *engine_out)
{
    uint8_t current;
    uint32_t row;

    if (instrument_out)
        *instrument_out = FMT_INSTRUMENT_INHERIT;
    if (engine_out)
        *engine_out = FMT_8BIT_ENGINE_SAFE;
    /* D048: song->meta.mode is legacy/default-family metadata only. Row/editor
     * eligibility follows the resolved instrument header family. */
    if (!song || track >= FMT_TRACKS || target_row >= FMT_STEPS_PER_PATTERN)
        return 0u;

    current = song->tracks[track].instrument;
    if (current >= FMT_INSTRUMENTS)
        return 0u;

    for (row = 0u; row <= target_row; row++) {
        const FmtStep *step = fmt_store_step_const(FMT_PLAYBACK_PATTERN,
                                                   track,
                                                   row);
        if (!step)
            return 0u;
        if (step->note <= FMT_NOTE_MAX) {
            if (step->instrument < FMT_INSTRUMENTS) {
                current = step->instrument;
            } else if (step->instrument != FMT_INSTRUMENT_INHERIT) {
                if (row == target_row)
                    return 0u;
            }
        }
    }

    if (current >= FMT_INSTRUMENTS)
        return 0u;
    {
        const FmtInstrumentHeader *header = &song->instruments[current];
        if (header->mode != FMT_MODE_8BIT ||
            !fmt_8bit_engine_supported(header->engine))
            return 0u;
        if (instrument_out)
            *instrument_out = current;
        if (engine_out)
            *engine_out = header->engine;
    }
    return 1u;
}

static uint32_t fmt_8bit_linked_resolve_row(const FmtSong *song,
                                            uint32_t track,
                                            uint32_t row,
                                            uint8_t instrument,
                                            uint8_t engine)
{
    Fmt8BitParamLock locks[FMT_8BIT_MAX_ROW_LOCKS];
    uint8_t count = 0u;
    uint8_t ordinal;
    uint32_t word = 0u;
    FmtLockStatus status;

    (void)song;
    status = fmt_lock_count(FMT_PLAYBACK_PATTERN, track, row, &count);
    if (status != FMT_LOCK_STATUS_OK || count > FMT_8BIT_MAX_ROW_LOCKS)
        return 0u;

    for (ordinal = 0u; ordinal < count; ordinal++) {
        FmtParamId id = (FmtParamId)FMT_PARAM_NONE;
        uint16_t value = 0u;

        status = fmt_lock_at(FMT_PLAYBACK_PATTERN,
                             track,
                             row,
                             ordinal,
                             &id,
                             &value);
        if (status != FMT_LOCK_STATUS_OK)
            return 0u;
        locks[ordinal].value = value;
        locks[ordinal].param_id = id;
        locks[ordinal].reserved = 0u;
    }

    if (fmt_8bit_resolve_page(engine,
                              &fmt_8bit_instrument_base[instrument],
                              locks,
                              count,
                              &word) == FMT_8BIT_RESOLVE_INVALID)
        return 0u;
    return word;
}

void fmt_8bit_linked_params_reset(void)
{
    uint32_t instrument, track, row;

    for (instrument = 0u; instrument < FMT_INSTRUMENTS; instrument++)
        fmt_8bit_param_set_defaults(&fmt_8bit_instrument_base[instrument]);
    for (track = 0u; track < FMT_TRACKS; track++)
        for (row = 0u; row < FMT_STEPS_PER_PATTERN; row++)
            fmt_8bit_row_param_cache.word[track][row] = 0u;
}

int fmt_8bit_linked_params_rebuild_track(uint32_t track)
{
    const FmtSong *song = fmt_store_song_const();
    uint8_t current;
    uint32_t row;

    if (!song || track >= FMT_TRACKS)
        return 0;

    current = song->tracks[track].instrument;
    for (row = 0u; row < FMT_STEPS_PER_PATTERN; row++) {
        const FmtStep *step = fmt_store_step_const(FMT_PLAYBACK_PATTERN,
                                                   track,
                                                   row);
        uint8_t malformed_note = 0u;
        uint8_t engine = FMT_8BIT_ENGINE_SAFE;
        uint32_t word = 0u;

        if (!step) {
            fmt_8bit_row_param_cache.word[track][row] = 0u;
            continue;
        }

        if (step->note <= FMT_NOTE_MAX) {
            if (step->instrument < FMT_INSTRUMENTS)
                current = step->instrument;
            else if (step->instrument != FMT_INSTRUMENT_INHERIT)
                malformed_note = 1u;
        }

        if (!malformed_note && current < FMT_INSTRUMENTS) {
            const FmtInstrumentHeader *header = &song->instruments[current];
            if (header->mode == FMT_MODE_8BIT &&
                fmt_8bit_engine_supported(header->engine)) {
                engine = header->engine;
                word = fmt_8bit_linked_resolve_row(song,
                                                   track,
                                                   row,
                                                   current,
                                                   engine);
            }
        }
        fmt_8bit_row_param_cache.word[track][row] = word;
    }
    return 1;
}

void fmt_8bit_linked_params_rebuild(void)
{
    uint32_t track;

    for (track = 0u; track < FMT_TRACKS; track++)
        (void)fmt_8bit_linked_params_rebuild_track(track);
}

int fmt_8bit_linked_params_refresh_track_row(uint32_t track, uint32_t row)
{
    const FmtSong *song = fmt_store_song_const();
    volatile uint32_t *slot = fmt_8bit_linked_slot(track, row);
    uint8_t instrument = FMT_INSTRUMENT_INHERIT;
    uint8_t engine = FMT_8BIT_ENGINE_SAFE;

    if (!song || !slot)
        return 0;

    if (!fmt_8bit_linked_row_context(song,
                                     track,
                                     row,
                                     &instrument,
                                     &engine)) {
        *slot = 0u;
        return 1;
    }

    *slot = fmt_8bit_linked_resolve_row(song,
                                        track,
                                        row,
                                        instrument,
                                        engine);
    return 1;
}

uint32_t fmt_8bit_linked_params_read(uint32_t track, uint32_t row)
{
    volatile uint32_t *slot = fmt_8bit_linked_slot(track, row);
    return slot ? *slot : 0u;
}

int fmt_8bit_linked_params_set_base(uint32_t instrument,
                                    const Fmt8BitParamSet *base)
{
    if (!base || instrument >= FMT_INSTRUMENTS)
        return 0;
    fmt_8bit_instrument_base[instrument] = *base;
    fmt_8bit_linked_params_rebuild();
    return 1;
}

const Fmt8BitParamSet *fmt_8bit_linked_params_base(uint32_t instrument)
{
    if (instrument >= FMT_INSTRUMENTS)
        return (const Fmt8BitParamSet *)0;
    return &fmt_8bit_instrument_base[instrument];
}

static int fmt_8bit_linked_editor_mapping(const FmtLockEditor *editor,
                                          uint32_t knob,
                                          FmtParamId *param_id,
                                          uint16_t *reference)
{
    const FmtSong *song = fmt_store_song_const();
    uint8_t instrument = FMT_INSTRUMENT_INHERIT;
    uint8_t engine = FMT_8BIT_ENGINE_SAFE;
    uint8_t value = 0u;
    FmtParamId id;

    if (!editor || !editor->active || knob >= FMT_8BIT_PAGE_SLOTS ||
        !song ||
        !fmt_8bit_linked_row_context(song,
                                     editor->track,
                                     editor->step,
                                     &instrument,
                                     &engine))
        return 0;

    id = fmt_8bit_page_param(engine, (uint8_t)knob);
    if (id == FMT_PARAM_NONE || instrument >= FMT_INSTRUMENTS ||
        !fmt_8bit_page_value_from_set(engine,
                                      (uint8_t)knob,
                                      &fmt_8bit_instrument_base[instrument],
                                      &value))
        return 0;

    if (param_id)
        *param_id = id;
    if (reference)
        *reference = value;
    return 1;
}

static void fmt_8bit_linked_refresh_editor_row(const FmtLockEditor *editor)
{
    if (!editor || !editor->active)
        return;
    (void)fmt_playback_params_refresh_track_step(editor->track, editor->step);
    (void)fmt_8bit_linked_params_refresh_track_row(editor->track, editor->step);
}

int fmt_8bit_linked_editor_apply_delta(FmtLockEditor *editor,
                                       uint32_t knob,
                                       int32_t delta)
{
    FmtParamId id = (FmtParamId)FMT_PARAM_NONE;
    uint16_t reference = 0u;
    int handled;

    if (delta == 0 ||
        !fmt_8bit_linked_editor_mapping(editor, knob, &id, &reference))
        return 0;

    handled = fmt_lock_editor_apply_param_delta(editor,
                                                id,
                                                reference,
                                                delta);
    if (handled && editor->last_status == (uint8_t)FMT_LOCK_EDITOR_STATUS_OK)
        fmt_8bit_linked_refresh_editor_row(editor);
    return handled;
}

int fmt_8bit_linked_editor_confirm_delete(FmtLockEditor *editor)
{
    int handled = fmt_lock_editor_confirm_delete(editor);

    if (handled && editor &&
        editor->last_status == (uint8_t)FMT_LOCK_EDITOR_STATUS_DELETED)
        fmt_8bit_linked_refresh_editor_row(editor);
    return handled;
}
