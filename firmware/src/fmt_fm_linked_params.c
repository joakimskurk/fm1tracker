/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_fm_linked_params.h"

#include <stddef.h>

#include "fmt_lock_store.h"
#include "fmt_playback.h"
#include "fmt_playback_params.h"
#include "fmt_store.h"

#define FMT_FM_LINKED_CONTEXT_INVALID   0u
#define FMT_FM_LINKED_CONTEXT_OTHER     1u
#define FMT_FM_LINKED_CONTEXT_FM        2u
#define FMT_FM_LINKED_CONTEXT_FAILSAFE  3u

FmtFmParamSet fmt_fm_instrument_base[FMT_INSTRUMENTS];

static int fmt_fm_linked_param_id(FmtParamId id)
{
    return (id >= FMT_PARAM_FM_ALGORITHM && id <= FMT_PARAM_FM_FEEDBACK) ||
           (id >= FMT_PARAM_FM_OP1_RATIO && id <= FMT_PARAM_FM_OP4_RELEASE);
}

static uint32_t fmt_fm_linked_pack_pair(const FmtFmOverride *a,
                                        const FmtFmOverride *b)
{
    return (uint32_t)a->id |
           ((uint32_t)a->value << 8) |
           ((uint32_t)b->id << 16) |
           ((uint32_t)b->value << 24);
}

static void fmt_fm_linked_unpack_pair(uint32_t word,
                                      FmtFmOverride *a,
                                      FmtFmOverride *b)
{
    a->id = (FmtParamId)(word & 0xFFu);
    a->value = (uint8_t)((word >> 8) & 0xFFu);
    b->id = (FmtParamId)((word >> 16) & 0xFFu);
    b->value = (uint8_t)((word >> 24) & 0xFFu);
}

static uint8_t fmt_fm_linked_row_context(const FmtSong *song,
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
        *engine_out = 0u;
    if (!song || track >= FMT_TRACKS || target_row >= FMT_STEPS_PER_PATTERN)
        return FMT_FM_LINKED_CONTEXT_INVALID;

    current = song->tracks[track].instrument;
    if (current >= FMT_INSTRUMENTS)
        return FMT_FM_LINKED_CONTEXT_INVALID;

    for (row = 0u; row <= target_row; row++) {
        const FmtStep *step = fmt_store_step_const(FMT_PLAYBACK_PATTERN,
                                                   track,
                                                   row);
        if (!step)
            return FMT_FM_LINKED_CONTEXT_INVALID;

        if (step->note <= FMT_NOTE_MAX) {
            if (step->instrument < FMT_INSTRUMENTS) {
                current = step->instrument;
            } else if (step->instrument != FMT_INSTRUMENT_INHERIT &&
                       row == target_row) {
                const FmtInstrumentHeader *inherited =
                    current < FMT_INSTRUMENTS
                    ? &song->instruments[current]
                    : (const FmtInstrumentHeader *)0;
                return inherited && inherited->mode == FMT_MODE_16BIT
                    ? FMT_FM_LINKED_CONTEXT_FAILSAFE
                    : FMT_FM_LINKED_CONTEXT_INVALID;
            }
        }
    }

    if (current >= FMT_INSTRUMENTS)
        return FMT_FM_LINKED_CONTEXT_INVALID;
    {
        const FmtInstrumentHeader *header = &song->instruments[current];

        if (header->mode != FMT_MODE_16BIT)
            return FMT_FM_LINKED_CONTEXT_OTHER;
        if (!fmt_fm_engine_supported(header->engine))
            return FMT_FM_LINKED_CONTEXT_FAILSAFE;
        if (instrument_out)
            *instrument_out = current;
        if (engine_out)
            *engine_out = header->engine;
    }
    return FMT_FM_LINKED_CONTEXT_FM;
}

static int fmt_fm_linked_collect_row(uint32_t track,
                                     uint32_t row,
                                     FmtFmOverride out[FMT_FM_MAX_ROW_OVERRIDES],
                                     uint8_t *count_out)
{
    uint8_t stored_count = 0u;
    uint8_t valid_count = 0u;
    uint8_t ordinal;
    FmtParamId previous = (FmtParamId)FMT_PARAM_NONE;
    uint8_t have_previous = 0u;
    FmtLockStatus status;

    if (!out || !count_out)
        return 0;
    *count_out = 0u;

    status = fmt_lock_count(FMT_PLAYBACK_PATTERN, track, row, &stored_count);
    if (status != FMT_LOCK_STATUS_OK || stored_count > FMT_FM_MAX_ROW_OVERRIDES)
        return 0;

    for (ordinal = 0u; ordinal < stored_count; ordinal++) {
        FmtParamId id = (FmtParamId)FMT_PARAM_NONE;
        uint16_t value = 0u;

        status = fmt_lock_at(FMT_PLAYBACK_PATTERN,
                             track,
                             row,
                             ordinal,
                             &id,
                             &value);
        if (status != FMT_LOCK_STATUS_OK)
            return 0;

        /* D028 promises strict ascending semantic IDs. A duplicate or reversal
         * therefore means malformed store state and invalidates the row payload. */
        if (have_previous && id <= previous)
            return 0;
        previous = id;
        have_previous = 1u;

        /* Wrong-family IDs remain valid user data in D028 but have no FM effect.
         * FM values are normalized bytes: raw uint16 values >255 are filtered,
         * never truncated into a different legal value. */
        if (!fmt_fm_linked_param_id(id) || value > 255u)
            continue;

        if (valid_count >= FMT_FM_MAX_ROW_OVERRIDES)
            return 0;
        out[valid_count].id = id;
        out[valid_count].value = (uint8_t)value;
        valid_count++;
    }

    *count_out = valid_count;
    return 1;
}

void fmt_fm_linked_params_reset(void)
{
    uint32_t instrument;

    for (instrument = 0u; instrument < FMT_INSTRUMENTS; instrument++)
        fmt_fm_param_set_defaults(&fmt_fm_instrument_base[instrument]);
}

int fmt_fm_linked_params_set_base(uint32_t instrument,
                                  const FmtFmParamSet *base)
{
    if (!base || instrument >= FMT_INSTRUMENTS)
        return 0;
    fmt_fm_instrument_base[instrument] = *base;
    return 1;
}

const FmtFmParamSet *fmt_fm_linked_params_base(uint32_t instrument)
{
    if (instrument >= FMT_INSTRUMENTS)
        return (const FmtFmParamSet *)0;
    return &fmt_fm_instrument_base[instrument];
}

int fmt_fm_linked_params_publish(uint32_t track,
                                 uint32_t row,
                                 const FmtFmOverride *overrides,
                                 uint8_t count)
{
    FmtFmOverride sorted[FMT_FM_MAX_ROW_OVERRIDES];
    uint32_t low, high;
    uint8_t i, j;
    uint8_t active, inactive;

    if (track >= FMT_TRACKS || row >= FMT_STEPS_PER_PATTERN ||
        count > FMT_FM_MAX_ROW_OVERRIDES ||
        (count != 0u && !overrides))
        return 0;

    for (i = 0u; i < FMT_FM_MAX_ROW_OVERRIDES; i++) {
        sorted[i].id = (FmtParamId)FMT_PARAM_NONE;
        sorted[i].value = 0u;
    }

    for (i = 0u; i < count; i++) {
        if (!fmt_fm_linked_param_id(overrides[i].id))
            return 0;
        sorted[i] = overrides[i];
    }

    for (i = 1u; i < count; i++) {
        FmtFmOverride key = sorted[i];
        j = i;
        while (j > 0u && sorted[j - 1u].id > key.id) {
            sorted[j] = sorted[j - 1u];
            j--;
        }
        sorted[j] = key;
    }

    for (i = 1u; i < count; i++) {
        if (sorted[i - 1u].id == sorted[i].id)
            return 0;
    }

    low = fmt_fm_linked_pack_pair(&sorted[0], &sorted[1]);
    high = fmt_fm_linked_pack_pair(&sorted[2], &sorted[3]);
    active = (uint8_t)(fmt_engine_row_param_cache.active_bank[track][row] & 1u);
    inactive = (uint8_t)(active ^ 1u);

    if (inactive == 0u) {
        fmt_engine_row_param_cache.legacy.word[track][row] = low;
        fmt_engine_row_param_cache.bank0_high[track][row] = high;
    } else {
        fmt_engine_row_param_cache.bank1_low[track][row] = low;
        fmt_engine_row_param_cache.bank1_high[track][row] = high;
    }
    fmt_engine_row_param_cache.active_bank[track][row] = inactive;
    return 1;
}

int fmt_fm_linked_params_read(uint32_t track,
                              uint32_t row,
                              FmtFmRowOverrides *out)
{
    uint32_t low, high;
    uint8_t active;

    if (!out || track >= FMT_TRACKS || row >= FMT_STEPS_PER_PATTERN)
        return 0;

    active = (uint8_t)(fmt_engine_row_param_cache.active_bank[track][row] & 1u);
    if (active == 0u) {
        low = fmt_engine_row_param_cache.legacy.word[track][row];
        high = fmt_engine_row_param_cache.bank0_high[track][row];
    } else {
        low = fmt_engine_row_param_cache.bank1_low[track][row];
        high = fmt_engine_row_param_cache.bank1_high[track][row];
    }

    fmt_fm_linked_unpack_pair(low, &out->slot[0], &out->slot[1]);
    fmt_fm_linked_unpack_pair(high, &out->slot[2], &out->slot[3]);
    return 1;
}

int fmt_fm_linked_params_refresh_track_row(uint32_t track, uint32_t row)
{
    const FmtSong *song = fmt_store_song_const();
    FmtFmOverride overrides[FMT_FM_MAX_ROW_OVERRIDES];
    uint8_t count = 0u;
    uint8_t instrument = FMT_INSTRUMENT_INHERIT;
    uint8_t engine = 0u;
    uint8_t context;

    if (!song || track >= FMT_TRACKS || row >= FMT_STEPS_PER_PATTERN)
        return 0;

    context = fmt_fm_linked_row_context(song,
                                        track,
                                        row,
                                        &instrument,
                                        &engine);
    (void)instrument;
    (void)engine;

    /* A valid other-family row owns the shared low word through its own view.
     * Do not touch any bank or selector here. */
    if (context == FMT_FM_LINKED_CONTEXT_OTHER ||
        context == FMT_FM_LINKED_CONTEXT_INVALID)
        return 1;

    if (context == FMT_FM_LINKED_CONTEXT_FAILSAFE)
        return fmt_fm_linked_params_publish(track, row, (const FmtFmOverride *)0, 0u);

    if (!fmt_fm_linked_collect_row(track, row, overrides, &count))
        return fmt_fm_linked_params_publish(track, row, (const FmtFmOverride *)0, 0u);

    return fmt_fm_linked_params_publish(track, row, overrides, count);
}

int fmt_fm_linked_params_rebuild_track(uint32_t track)
{
    uint32_t row;

    if (track >= FMT_TRACKS || !fmt_store_song_const())
        return 0;
    for (row = 0u; row < FMT_STEPS_PER_PATTERN; row++) {
        if (!fmt_fm_linked_params_refresh_track_row(track, row))
            return 0;
    }
    return 1;
}

void fmt_fm_linked_params_rebuild(void)
{
    uint32_t track;

    for (track = 0u; track < FMT_TRACKS; track++)
        (void)fmt_fm_linked_params_rebuild_track(track);
}

int fmt_fm_linked_params_compose(uint32_t instrument,
                                 const FmtFmRowOverrides *overrides,
                                 FmtFmPatch *patch_out)
{
    FmtFmParamSet resolved;
    uint32_t i;

    if (!overrides || !patch_out || instrument >= FMT_INSTRUMENTS)
        return 0;

    resolved = fmt_fm_instrument_base[instrument];
    for (i = 0u; i < FMT_FM_MAX_ROW_OVERRIDES; i++) {
        FmtParamId id = overrides->slot[i].id;
        if (id == FMT_PARAM_NONE)
            continue;
        if (!fmt_fm_linked_param_id(id) ||
            !fmt_fm_param_set_value(&resolved, id, overrides->slot[i].value))
            return 0;
    }

    fmt_fm_params_to_patch(&resolved, patch_out);
    return 1;
}

int fmt_fm_linked_editor_is_fm(const FmtLockEditor *editor)
{
    const FmtSong *song = fmt_store_song_const();

    if (!editor || !editor->active || !song)
        return 0;
    return fmt_fm_linked_row_context(song,
                                     editor->track,
                                     editor->step,
                                     (uint8_t *)0,
                                     (uint8_t *)0) == FMT_FM_LINKED_CONTEXT_FM;
}

static int fmt_fm_linked_editor_mapping(const FmtLockEditor *editor,
                                        uint8_t page,
                                        uint32_t knob,
                                        FmtParamId *param_id,
                                        uint16_t *reference)
{
    const FmtSong *song = fmt_store_song_const();
    uint8_t instrument = FMT_INSTRUMENT_INHERIT;
    uint8_t engine = 0u;
    uint8_t value = 0u;
    FmtParamId id;

    if (!editor || !editor->active || page >= FMT_FM_PAGE_COUNT ||
        knob >= FMT_FM_PAGE_SLOTS || !song ||
        fmt_fm_linked_row_context(song,
                                  editor->track,
                                  editor->step,
                                  &instrument,
                                  &engine) != FMT_FM_LINKED_CONTEXT_FM)
        return 0;
    (void)engine;

    id = fmt_fm_page_param(page, (uint8_t)knob);
    if (id == FMT_PARAM_NONE || instrument >= FMT_INSTRUMENTS ||
        !fmt_fm_page_value_from_set(page,
                                    (uint8_t)knob,
                                    &fmt_fm_instrument_base[instrument],
                                    &value))
        return 0;

    if (param_id)
        *param_id = id;
    if (reference)
        *reference = value;
    return 1;
}

static void fmt_fm_linked_refresh_editor_row(const FmtLockEditor *editor)
{
    if (!editor || !editor->active)
        return;
    (void)fmt_playback_params_refresh_track_step(editor->track, editor->step);
    (void)fmt_fm_linked_params_refresh_track_row(editor->track, editor->step);
}

int fmt_fm_linked_editor_apply_delta(FmtLockEditor *editor,
                                     uint8_t page,
                                     uint32_t knob,
                                     int32_t delta)
{
    FmtParamId id = (FmtParamId)FMT_PARAM_NONE;
    uint16_t reference = 0u;
    int handled;

    if (delta == 0 ||
        !fmt_fm_linked_editor_mapping(editor,
                                      page,
                                      knob,
                                      &id,
                                      &reference))
        return 0;

    handled = fmt_lock_editor_apply_param_delta(editor,
                                                id,
                                                reference,
                                                delta);
    if (handled && editor->last_status == (uint8_t)FMT_LOCK_EDITOR_STATUS_OK)
        fmt_fm_linked_refresh_editor_row(editor);
    return handled;
}

int fmt_fm_linked_editor_confirm_delete(FmtLockEditor *editor)
{
    int handled = fmt_lock_editor_confirm_delete(editor);

    if (handled && editor &&
        editor->last_status == (uint8_t)FMT_LOCK_EDITOR_STATUS_DELETED)
        fmt_fm_linked_refresh_editor_row(editor);
    return handled;
}
