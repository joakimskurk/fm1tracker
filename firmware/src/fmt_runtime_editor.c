/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_runtime_editor.h"

static uint8_t fmt_runtime_editor_clamp_u8_delta(uint8_t value, int32_t delta)
{
    int32_t next;

    if (delta > 255)
        delta = 255;
    else if (delta < -255)
        delta = -255;
    next = (int32_t)value + delta;
    if (next < 0)
        next = 0;
    else if (next > 255)
        next = 255;
    return (uint8_t)next;
}

static uint8_t fmt_runtime_editor_wrap_delta(uint8_t value,
                                             uint8_t count,
                                             int32_t delta)
{
    if (count == 0u)
        return 0u;
    if (delta > 32)
        delta = 32;
    else if (delta < -32)
        delta = -32;
    while (delta > 0) {
        value++;
        if (value >= count)
            value = 0u;
        delta--;
    }
    while (delta < 0) {
        if (value == 0u)
            value = (uint8_t)(count - 1u);
        else
            value--;
        delta++;
    }
    return value;
}

static uint8_t fmt_runtime_editor_engine_index(const FmtInstrumentHeader *header)
{
    if (!header)
        return FMT_RUNTIME_EDITOR_ENGINE_PULSE;
    if (header->mode == FMT_MODE_8BIT &&
        header->engine >= FMT_8BIT_ENGINE_PULSE &&
        header->engine <= FMT_8BIT_ENGINE_WAVETABLE8)
        return (uint8_t)(header->engine - FMT_8BIT_ENGINE_PULSE);
    if (header->mode == FMT_MODE_16BIT &&
        header->engine == FMT_FM_ENGINE_FM4)
        return FMT_RUNTIME_EDITOR_ENGINE_FM4;
    return FMT_RUNTIME_EDITOR_ENGINE_PULSE;
}

static void fmt_runtime_editor_set_engine_header(FmtInstrumentHeader *header,
                                                 uint8_t index)
{
    if (!header)
        return;
    if (index == FMT_RUNTIME_EDITOR_ENGINE_FM4) {
        header->mode = FMT_MODE_16BIT;
        header->engine = FMT_FM_ENGINE_FM4;
    } else {
        header->mode = FMT_MODE_8BIT;
        header->engine = (uint8_t)(FMT_8BIT_ENGINE_PULSE + index);
    }
    header->version = 0u;
    header->flags = 0u;
}

void fmt_runtime_editor_reset(FmtRuntimeEditor *editor)
{
    if (!editor)
        return;
    editor->active = 0u;
    editor->page = 0u;
    editor->dirty = 1u;
    editor->reserved = 0u;
}

void fmt_runtime_editor_enter(FmtRuntimeEditor *editor)
{
    if (!editor)
        return;
    editor->active = 1u;
    editor->dirty = 1u;
}

void fmt_runtime_editor_exit(FmtRuntimeEditor *editor)
{
    if (!editor)
        return;
    editor->active = 0u;
    editor->dirty = 1u;
}

void fmt_runtime_editor_mark_clean(FmtRuntimeEditor *editor)
{
    if (editor)
        editor->dirty = 0u;
}

uint8_t fmt_runtime_editor_instrument(const FmtSong *song,
                                      const FmtPatternInput *input)
{
    if (!song || !input || input->selected_track >= FMT_TRACKS)
        return 0u;
    if (song->tracks[input->selected_track].instrument >= FMT_INSTRUMENTS)
        return 0u;
    return song->tracks[input->selected_track].instrument;
}

const FmtInstrumentHeader *fmt_runtime_editor_header(const FmtSong *song,
                                                     const FmtPatternInput *input)
{
    uint8_t instrument;

    if (!song || !input)
        return (const FmtInstrumentHeader *)0;
    instrument = fmt_runtime_editor_instrument(song, input);
    if (instrument >= FMT_INSTRUMENTS)
        return (const FmtInstrumentHeader *)0;
    return &song->instruments[instrument];
}

int fmt_runtime_editor_select_track(FmtRuntimeEditor *editor,
                                    FmtPatternInput *input,
                                    int32_t delta)
{
    uint8_t next;

    if (!input || delta == 0)
        return 0;
    next = fmt_runtime_editor_wrap_delta(input->selected_track,
                                         FMT_TRACKS,
                                         delta);
    if (next == input->selected_track)
        return 0;
    input->selected_track = next;
    if (editor)
        editor->dirty = 1u;
    return 1;
}

int fmt_runtime_editor_select_instrument(FmtRuntimeEditor *editor,
                                         FmtSong *song,
                                         const FmtPatternInput *input,
                                         int32_t delta)
{
    uint8_t current, next;

    if (!song || !input || input->selected_track >= FMT_TRACKS || delta == 0)
        return 0;
    current = song->tracks[input->selected_track].instrument;
    if (current >= FMT_INSTRUMENTS)
        current = 0u;
    next = fmt_runtime_editor_wrap_delta(current, FMT_INSTRUMENTS, delta);
    if (next == current)
        return 0;
    song->tracks[input->selected_track].instrument = next;
    if (editor) {
        editor->page = 0u;
        editor->dirty = 1u;
    }
    return 1;
}

int fmt_runtime_editor_select_engine(FmtRuntimeEditor *editor,
                                     FmtSong *song,
                                     FmtInstrumentIdentityPublication *publication,
                                     const FmtPatternInput *input,
                                     int32_t delta)
{
    uint8_t instrument, current, next;
    FmtInstrumentHeader *header;

    if (!song || !publication || !input || delta == 0)
        return 0;
    instrument = fmt_runtime_editor_instrument(song, input);
    if (instrument >= FMT_INSTRUMENTS)
        return 0;
    header = &song->instruments[instrument];
    current = fmt_runtime_editor_engine_index(header);
    next = fmt_runtime_editor_wrap_delta(current,
                                         FMT_RUNTIME_EDITOR_ENGINE_COUNT,
                                         delta);
    if (next == current &&
        ((header->mode == FMT_MODE_16BIT && next == FMT_RUNTIME_EDITOR_ENGINE_FM4) ||
         (header->mode == FMT_MODE_8BIT && next < FMT_RUNTIME_EDITOR_ENGINE_FM4 &&
          header->engine == (uint8_t)(FMT_8BIT_ENGINE_PULSE + next))))
        return 0;

    fmt_runtime_editor_set_engine_header(header, next);
    if (!fmt_instrument_identity_publish(publication, instrument, header))
        return 0;
    fmt_8bit_linked_params_rebuild();
    fmt_fm_linked_params_rebuild();
    if (editor) {
        editor->page = 0u;
        editor->dirty = 1u;
    }
    return 1;
}

uint8_t fmt_runtime_editor_page_count(const FmtInstrumentHeader *header)
{
    if (!header)
        return 1u;
    if (header->mode == FMT_MODE_16BIT && header->engine == FMT_FM_ENGINE_FM4)
        return FMT_FM_PAGE_COUNT;
    return 1u;
}

int fmt_runtime_editor_page_delta(FmtRuntimeEditor *editor,
                                  const FmtSong *song,
                                  const FmtPatternInput *input,
                                  int32_t delta)
{
    const FmtInstrumentHeader *header;
    uint8_t count;
    int32_t next;

    if (!editor || !song || !input || delta == 0)
        return 0;
    header = fmt_runtime_editor_header(song, input);
    count = fmt_runtime_editor_page_count(header);
    if (count <= 1u) {
        if (editor->page != 0u) {
            editor->page = 0u;
            editor->dirty = 1u;
            return 1;
        }
        return 0;
    }

    next = (int32_t)editor->page + delta;
    if (next < 0)
        next = 0;
    else if (next >= count)
        next = (int32_t)count - 1;
    if ((uint8_t)next == editor->page)
        return 0;
    editor->page = (uint8_t)next;
    editor->dirty = 1u;
    return 1;
}

int fmt_runtime_editor_env_page(FmtRuntimeEditor *editor,
                                const FmtSong *song,
                                const FmtPatternInput *input)
{
    const FmtInstrumentHeader *header;
    uint8_t next;

    if (!editor || !song || !input)
        return 0;
    header = fmt_runtime_editor_header(song, input);
    if (!header || header->mode != FMT_MODE_16BIT ||
        header->engine != FMT_FM_ENGINE_FM4)
        return 0;

    if (editor->page < FMT_FM_PAGE_ATTACK ||
        editor->page >= FMT_FM_PAGE_RELEASE)
        next = FMT_FM_PAGE_ATTACK;
    else
        next = (uint8_t)(editor->page + 1u);
    if (next == editor->page)
        return 0;
    editor->page = next;
    editor->dirty = 1u;
    return 1;
}

static int fmt_runtime_editor_set_8bit_value(Fmt8BitParamSet *base,
                                             FmtParamId id,
                                             uint8_t value)
{
    if (!base)
        return 0;
    switch (id) {
    case FMT_8BIT_PARAM_ATTACK:
        base->attack = value;
        return 1;
    case FMT_8BIT_PARAM_DECAY:
        base->decay = value;
        return 1;
    case FMT_8BIT_PARAM_SUSTAIN:
        base->sustain = value;
        return 1;
    case FMT_8BIT_PARAM_RELEASE:
        base->release = value;
        return 1;
    case FMT_8BIT_PARAM_PULSE_WIDTH:
        base->pulse_width = value;
        return 1;
    case FMT_8BIT_PARAM_WAVETABLE:
        base->wavetable = value;
        return 1;
    default:
        return 0;
    }
}

int fmt_runtime_editor_apply_knob(FmtRuntimeEditor *editor,
                                  FmtSong *song,
                                  const FmtPatternInput *input,
                                  uint32_t knob,
                                  int32_t delta)
{
    uint8_t instrument;
    FmtInstrumentHeader *header;

    if (!editor || !song || !input || knob >= 4u || delta == 0)
        return 0;
    instrument = fmt_runtime_editor_instrument(song, input);
    if (instrument >= FMT_INSTRUMENTS)
        return 0;
    header = &song->instruments[instrument];

    if (header->mode == FMT_MODE_8BIT && fmt_8bit_engine_supported(header->engine)) {
        const Fmt8BitParamSet *current = fmt_8bit_linked_params_base(instrument);
        Fmt8BitParamSet next;
        FmtParamId id;
        uint8_t value;

        if (!current)
            return 0;
        next = *current;
        id = fmt_8bit_page_param(header->engine, (uint8_t)knob);
        if (id == FMT_PARAM_NONE ||
            !fmt_8bit_page_value_from_set(header->engine,
                                          (uint8_t)knob,
                                          current,
                                          &value))
            return 0;
        value = fmt_runtime_editor_clamp_u8_delta(value, delta);
        if (!fmt_runtime_editor_set_8bit_value(&next, id, value) ||
            !fmt_8bit_linked_params_set_base(instrument, &next))
            return 0;
        fmt_8bit_linked_params_rebuild();
        editor->dirty = 1u;
        return 1;
    }

    if (header->mode == FMT_MODE_16BIT && header->engine == FMT_FM_ENGINE_FM4) {
        const FmtFmParamSet *current = fmt_fm_linked_params_base(instrument);
        FmtFmParamSet next;
        FmtParamId id;
        uint8_t value;

        if (!current || editor->page >= FMT_FM_PAGE_COUNT)
            return 0;
        next = *current;
        id = fmt_fm_page_param(editor->page, (uint8_t)knob);
        if (id == FMT_PARAM_NONE || !fmt_fm_param_value(current, id, &value))
            return 0;
        value = fmt_runtime_editor_clamp_u8_delta(value, delta);
        if (!fmt_fm_param_set_value(&next, id, value) ||
            !fmt_fm_linked_params_set_base(instrument, &next))
            return 0;
        editor->dirty = 1u;
        return 1;
    }
    return 0;
}

uint8_t fmt_runtime_editor_value(const FmtRuntimeEditor *editor,
                                 const FmtSong *song,
                                 const FmtPatternInput *input,
                                 uint32_t knob,
                                 uint8_t *value_out)
{
    uint8_t instrument;
    const FmtInstrumentHeader *header;

    if (!editor || !song || !input || !value_out || knob >= 4u)
        return 0u;
    instrument = fmt_runtime_editor_instrument(song, input);
    header = fmt_runtime_editor_header(song, input);
    if (!header || instrument >= FMT_INSTRUMENTS)
        return 0u;

    if (header->mode == FMT_MODE_8BIT && fmt_8bit_engine_supported(header->engine)) {
        const Fmt8BitParamSet *base = fmt_8bit_linked_params_base(instrument);
        if (!base)
            return 0u;
        return (uint8_t)fmt_8bit_page_value_from_set(header->engine,
                                                      (uint8_t)knob,
                                                      base,
                                                      value_out);
    }
    if (header->mode == FMT_MODE_16BIT && header->engine == FMT_FM_ENGINE_FM4) {
        const FmtFmParamSet *base = fmt_fm_linked_params_base(instrument);
        if (!base || editor->page >= FMT_FM_PAGE_COUNT)
            return 0u;
        return (uint8_t)fmt_fm_page_value_from_set(editor->page,
                                                   (uint8_t)knob,
                                                   base,
                                                   value_out);
    }
    return 0u;
}

const char *fmt_runtime_editor_engine_name(const FmtInstrumentHeader *header)
{
    uint8_t index = fmt_runtime_editor_engine_index(header);
    static const char *const names[FMT_RUNTIME_EDITOR_ENGINE_COUNT] = {
        "PULSE", "NOISE", "TRIANGLE", "WAVETABLE8", "FM4"
    };
    return names[index];
}

const char *fmt_runtime_editor_page_name(const FmtRuntimeEditor *editor,
                                         const FmtInstrumentHeader *header)
{
    static const char *const fm_pages[FMT_FM_PAGE_COUNT] = {
        "CORE", "RATIO", "LEVEL", "ATTACK", "DECAY", "SUSTAIN", "RELEASE"
    };

    if (!editor || !header)
        return "VOICE";
    if (header->mode == FMT_MODE_16BIT && header->engine == FMT_FM_ENGINE_FM4 &&
        editor->page < FMT_FM_PAGE_COUNT)
        return fm_pages[editor->page];
    return "VOICE";
}

const char *fmt_runtime_editor_slot_name(const FmtRuntimeEditor *editor,
                                         const FmtInstrumentHeader *header,
                                         uint32_t knob)
{
    static const char *const bit8_env[4] = { "ATT", "DEC", "SUS", "REL" };
    static const char *const pulse[4] = { "PW", "DEC", "SUS", "REL" };
    static const char *const wavetable[4] = { "WAV", "DEC", "SUS", "REL" };
    static const char *const fm_core[4] = { "ALG", "IDX", "FB", "-" };
    static const char *const fm_ratio[4] = { "R1", "R2", "R3", "R4" };
    static const char *const fm_level[4] = { "L1", "L2", "L3", "L4" };
    static const char *const fm_attack[4] = { "A1", "A2", "A3", "A4" };
    static const char *const fm_decay[4] = { "D1", "D2", "D3", "D4" };
    static const char *const fm_sustain[4] = { "S1", "S2", "S3", "S4" };
    static const char *const fm_release[4] = { "R1", "R2", "R3", "R4" };
    const char *const *page = bit8_env;

    if (!header || knob >= 4u)
        return "-";
    if (header->mode == FMT_MODE_8BIT) {
        if (header->engine == FMT_8BIT_ENGINE_PULSE)
            page = pulse;
        else if (header->engine == FMT_8BIT_ENGINE_WAVETABLE8)
            page = wavetable;
        return page[knob];
    }
    if (header->mode != FMT_MODE_16BIT || header->engine != FMT_FM_ENGINE_FM4 || !editor)
        return "-";

    switch (editor->page) {
    case FMT_FM_PAGE_CORE: page = fm_core; break;
    case FMT_FM_PAGE_RATIO: page = fm_ratio; break;
    case FMT_FM_PAGE_LEVEL: page = fm_level; break;
    case FMT_FM_PAGE_ATTACK: page = fm_attack; break;
    case FMT_FM_PAGE_DECAY: page = fm_decay; break;
    case FMT_FM_PAGE_SUSTAIN: page = fm_sustain; break;
    case FMT_FM_PAGE_RELEASE: page = fm_release; break;
    default: return "-";
    }
    return page[knob];
}
