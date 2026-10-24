/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt_8bit_linked_params.h"
#include "fmt_lock_store.h"
#include "fmt_param_registry.h"
#include "fmt_playback_params.h"
#include "fmt_store.h"

static FmtSong *song(void)
{
    FmtSong *s = fmt_store_song();
    assert(s != NULL);
    return s;
}

static void reset_fixture(void)
{
    uint32_t i;
    FmtSong *s;

    fmt_store_reset();
    fmt_lock_store_reset();
    fmt_playback_params_reset();
    fmt_8bit_linked_params_reset();

    s = song();
    s->meta.mode = FMT_MODE_8BIT;
    for (i = 0u; i < 4u; i++) {
        s->instruments[i].mode = FMT_MODE_8BIT;
        s->instruments[i].engine = (uint8_t)(i + 1u);
        s->tracks[i].instrument = (uint8_t)i;
    }
    s->instruments[4].mode = FMT_MODE_8BIT;
    s->instruments[4].engine = FMT_8BIT_ENGINE_SAFE;
}

static void set_note(uint32_t track, uint32_t row, uint8_t note, uint8_t instrument)
{
    FmtStep *st = fmt_store_step(0u, track, row);
    assert(st != NULL);
    st->note = note;
    st->instrument = instrument;
}

static uint32_t word_from_base(uint8_t engine, const Fmt8BitParamSet *base)
{
    uint8_t v[4];
    uint32_t slot;

    for (slot = 0u; slot < 4u; slot++)
        assert(fmt_8bit_page_value_from_set(engine, (uint8_t)slot, base, &v[slot]));
    return fmt_8bit_page_pack(v[0], v[1], v[2], v[3]);
}

static void test_registry_activation(void)
{
    static const FmtParamId ids[6] = {
        FMT_PARAM_BIT8_ATTACK,
        FMT_PARAM_BIT8_DECAY,
        FMT_PARAM_BIT8_SUSTAIN,
        FMT_PARAM_BIT8_RELEASE,
        FMT_PARAM_BIT8_PULSE_WIDTH,
        FMT_PARAM_BIT8_WAVETABLE
    };
    static const char *labels[6] = {
        "ATTACK", "DECAY", "SUSTAIN", "RELEASE", "WIDTH", "TABLE"
    };
    uint32_t i;

    assert(FMT_PARAM_ASSIGNED_COUNT == 10u);
    assert(fmt_param_registry_count() == 10u);
    for (i = 0u; i < 6u; i++) {
        const FmtParamInfo *info = fmt_param_lookup(ids[i]);
        assert(info != NULL);
        assert(info->id == ids[i]);
        assert(info->flags == FMT_PARAM_FLAG_LOCKABLE);
        assert(strcmp(info->label, labels[i]) == 0);
        assert(fmt_param_namespace(ids[i]) == FMT_PARAM_NAMESPACE_8BIT);
        assert(fmt_param_is_assigned(ids[i]));
        assert(fmt_param_is_lockable(ids[i]));
    }
    assert(!fmt_param_is_assigned((FmtParamId)0x26u));
    assert(!fmt_param_is_lockable((FmtParamId)0x26u));
    assert(fmt_param_namespace((FmtParamId)0x26u) == FMT_PARAM_NAMESPACE_8BIT);
}

static void test_layout_reset_and_defaults(void)
{
    uint32_t instrument, track, row;

    reset_fixture();
    assert(sizeof(fmt_8bit_instrument_base) == 96u);
    assert(sizeof(fmt_8bit_row_param_cache) == 2048u);
    assert(((uintptr_t)&fmt_8bit_row_param_cache & 3u) == 0u);

    for (instrument = 0u; instrument < FMT_INSTRUMENTS; instrument++) {
        const Fmt8BitParamSet *base = fmt_8bit_linked_params_base(instrument);
        assert(base != NULL);
        assert(base->attack == FMT_8BIT_DEFAULT_ATTACK);
        assert(base->decay == FMT_8BIT_DEFAULT_DECAY);
        assert(base->sustain == FMT_8BIT_DEFAULT_SUSTAIN);
        assert(base->release == FMT_8BIT_DEFAULT_RELEASE);
        assert(base->pulse_width == FMT_8BIT_DEFAULT_PULSE_WIDTH);
        assert(base->wavetable == FMT_8BIT_DEFAULT_WAVETABLE);
    }
    assert(fmt_8bit_linked_params_base(FMT_INSTRUMENTS) == NULL);

    for (track = 0u; track < FMT_TRACKS; track++)
        for (row = 0u; row < FMT_STEPS_PER_PATTERN; row++)
            assert(fmt_8bit_linked_params_read(track, row) == 0u);
    assert(fmt_8bit_linked_params_read(FMT_TRACKS, 0u) == 0u);
    assert(fmt_8bit_linked_params_read(0u, FMT_STEPS_PER_PATTERN) == 0u);
}

static void install_distinct_bases(void)
{
    Fmt8BitParamSet *b;

    b = &fmt_8bit_instrument_base[0];
    b->attack = 11u; b->decay = 22u; b->sustain = 33u;
    b->release = 44u; b->pulse_width = 55u; b->wavetable = 66u;

    b = &fmt_8bit_instrument_base[1];
    b->attack = 12u; b->decay = 23u; b->sustain = 34u;
    b->release = 45u; b->pulse_width = 56u; b->wavetable = 67u;

    b = &fmt_8bit_instrument_base[2];
    b->attack = 13u; b->decay = 24u; b->sustain = 35u;
    b->release = 46u; b->pulse_width = 57u; b->wavetable = 68u;

    b = &fmt_8bit_instrument_base[3];
    b->attack = 14u; b->decay = 25u; b->sustain = 36u;
    b->release = 47u; b->pulse_width = 58u; b->wavetable = 69u;
}

static void test_note_only_inheritance_and_cache(void)
{
    FmtSong *s;
    uint32_t pulse, noise, tri, wave;

    reset_fixture();
    install_distinct_bases();
    s = song();
    s->tracks[0].instrument = 0u;

    set_note(0u, 0u, 60u, 0u);
    set_note(0u, 2u, 61u, 1u);
    set_note(0u, 3u, FMT_NOTE_TIE, 2u);
    set_note(0u, 4u, FMT_NOTE_OFF, 3u);
    set_note(0u, 5u, 62u, 16u); /* malformed explicit instrument */
    set_note(0u, 7u, 63u, 2u);
    set_note(0u, 9u, 64u, 3u);
    set_note(0u, 11u, 65u, 4u); /* valid index, unsupported SAFE engine */
    set_note(0u, 13u, 66u, 0u);

    assert(fmt_lock_set(0u, 0u, 0u, FMT_PARAM_BIT8_PULSE_WIDTH, 200u, NULL) ==
           FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(0u, 0u, 0u, FMT_PARAM_VOICE_LEVEL, 123u, NULL) ==
           FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(0u, 0u, 2u, FMT_PARAM_BIT8_ATTACK, 201u, NULL) ==
           FMT_LOCK_STATUS_OK);
    assert(fmt_lock_set(0u, 0u, 2u, FMT_PARAM_BIT8_WAVETABLE, 202u, NULL) ==
           FMT_LOCK_STATUS_OK); /* ignored on NOISE page */

    assert(fmt_8bit_linked_params_rebuild_track(0u));

    pulse = word_from_base(FMT_8BIT_ENGINE_PULSE, &fmt_8bit_instrument_base[0]);
    noise = word_from_base(FMT_8BIT_ENGINE_NOISE, &fmt_8bit_instrument_base[1]);
    tri = word_from_base(FMT_8BIT_ENGINE_TRIANGLE, &fmt_8bit_instrument_base[2]);
    wave = word_from_base(FMT_8BIT_ENGINE_WAVETABLE8, &fmt_8bit_instrument_base[3]);

    assert(fmt_8bit_page_unpack(fmt_8bit_linked_params_read(0u, 0u), 0u) == 200u);
    assert(fmt_8bit_page_unpack(fmt_8bit_linked_params_read(0u, 0u), 1u) == 22u);
    assert(fmt_8bit_linked_params_read(0u, 1u) == pulse);

    assert(fmt_8bit_page_unpack(fmt_8bit_linked_params_read(0u, 2u), 0u) == 201u);
    assert(fmt_8bit_page_unpack(fmt_8bit_linked_params_read(0u, 2u), 1u) == 23u);
    assert(fmt_8bit_linked_params_read(0u, 3u) == noise);
    assert(fmt_8bit_linked_params_read(0u, 4u) == noise);
    assert(fmt_8bit_linked_params_read(0u, 5u) == 0u);
    assert(fmt_8bit_linked_params_read(0u, 6u) == noise);
    assert(fmt_8bit_linked_params_read(0u, 7u) == tri);
    assert(fmt_8bit_linked_params_read(0u, 8u) == tri);
    assert(fmt_8bit_linked_params_read(0u, 9u) == wave);
    assert(fmt_8bit_linked_params_read(0u, 10u) == wave);
    assert(fmt_8bit_linked_params_read(0u, 11u) == 0u);
    assert(fmt_8bit_linked_params_read(0u, 12u) == 0u);
    assert(fmt_8bit_linked_params_read(0u, 13u) == pulse);

    s->meta.mode = FMT_MODE_16BIT;
    assert(fmt_8bit_linked_params_rebuild_track(0u));
    assert(fmt_8bit_linked_params_read(0u, 0u) == 0u);
    assert(fmt_8bit_linked_params_read(0u, 13u) == 0u);
}

static void test_single_row_refresh_and_base_rebuild(void)
{
    Fmt8BitParamSet changed;
    uint32_t before_other;

    reset_fixture();
    install_distinct_bases();
    set_note(0u, 0u, 60u, 0u);
    set_note(0u, 2u, 61u, 1u);
    fmt_8bit_linked_params_rebuild();

    before_other = fmt_8bit_linked_params_read(0u, 2u);
    assert(fmt_lock_set(0u, 0u, 1u, FMT_PARAM_BIT8_PULSE_WIDTH, 77u, NULL) ==
           FMT_LOCK_STATUS_OK);
    fmt_8bit_row_param_cache.word[0][2] = before_other;
    assert(fmt_8bit_linked_params_refresh_track_row(0u, 1u));
    assert(fmt_8bit_page_unpack(fmt_8bit_linked_params_read(0u, 1u), 0u) == 77u);
    assert(fmt_8bit_linked_params_read(0u, 2u) == before_other);
    assert(!fmt_8bit_linked_params_refresh_track_row(FMT_TRACKS, 0u));
    assert(!fmt_8bit_linked_params_refresh_track_row(0u, FMT_STEPS_PER_PATTERN));

    changed = fmt_8bit_instrument_base[0];
    changed.pulse_width = 88u;
    assert(fmt_8bit_linked_params_set_base(0u, &changed));
    assert(fmt_8bit_page_unpack(fmt_8bit_linked_params_read(0u, 0u), 0u) == 88u);
    assert(fmt_8bit_page_unpack(fmt_8bit_linked_params_read(0u, 1u), 0u) == 77u);
    assert(fmt_8bit_linked_params_read(0u, 2u) == before_other);
    assert(!fmt_8bit_linked_params_set_base(FMT_INSTRUMENTS, &changed));
    assert(!fmt_8bit_linked_params_set_base(0u, NULL));
}

static void prepare_editor(FmtLockEditor *editor, uint8_t track, uint8_t row)
{
    fmt_lock_editor_reset(editor);
    editor->active = 1u;
    editor->held_key = 0u;
    editor->track = track;
    editor->step = row;
}

static uint16_t plus_one_clamped(uint8_t value)
{
    return value == 255u ? 255u : (uint16_t)(value + 1u);
}

static void test_page_aware_editor_all_engines(void)
{
    uint32_t track, knob;

    reset_fixture();
    for (track = 0u; track < 4u; track++)
        set_note(track, 0u, 60u, FMT_INSTRUMENT_INHERIT);
    fmt_playback_params_rebuild();
    fmt_8bit_linked_params_rebuild();

    for (track = 0u; track < 4u; track++) {
        FmtLockEditor editor;
        uint8_t engine = (uint8_t)(track + 1u);
        uint8_t count = 0u;

        prepare_editor(&editor, (uint8_t)track, 0u);
        for (knob = 0u; knob < 4u; knob++) {
            FmtParamId id = fmt_8bit_page_param(engine, (uint8_t)knob);
            uint8_t base_value = 0u;
            uint16_t stored = 0u;

            assert(id != FMT_PARAM_NONE);
            assert(fmt_8bit_page_value_from_set(engine,
                                                 (uint8_t)knob,
                                                 &fmt_8bit_instrument_base[track],
                                                 &base_value));
            assert(fmt_8bit_linked_editor_apply_delta(&editor, knob, 1));
            assert(editor.last_status == (uint8_t)FMT_LOCK_EDITOR_STATUS_OK);
            assert(editor.last_param == id);
            assert(fmt_lock_get(0u, track, 0u, id, &stored) == FMT_LOCK_STATUS_OK);
            assert(stored == plus_one_clamped(base_value));
        }
        assert(fmt_lock_count(0u, track, 0u, &count) == FMT_LOCK_STATUS_OK);
        assert(count == 4u);
        assert(fmt_playback_params_read_track(track, 0u) == fmt_playback_params_default_word());
    }
}

static void test_editor_refresh_delete_and_footer(void)
{
    FmtLockEditor editor;
    uint16_t value = 0u;
    uint32_t base_word;
    char footer[32];

    reset_fixture();
    set_note(0u, 0u, 60u, 0u);
    fmt_playback_params_rebuild();
    fmt_8bit_linked_params_rebuild();
    base_word = word_from_base(FMT_8BIT_ENGINE_PULSE, &fmt_8bit_instrument_base[0]);

    prepare_editor(&editor, 0u, 0u);
    fmt_playback_param_cache[0] = 0u;
    fmt_8bit_row_param_cache.word[0][0] = 0xDEADBEEFu;

    assert(fmt_8bit_linked_editor_apply_delta(&editor, 0u, 5));
    assert(fmt_lock_get(0u, 0u, 0u, FMT_PARAM_BIT8_PULSE_WIDTH, &value) ==
           FMT_LOCK_STATUS_OK);
    assert(value == (uint16_t)(FMT_8BIT_DEFAULT_PULSE_WIDTH + 5u));
    assert(fmt_playback_params_read_track(0u, 0u) == fmt_playback_params_default_word());
    assert(fmt_8bit_page_unpack(fmt_8bit_linked_params_read(0u, 0u), 0u) ==
           (uint8_t)(FMT_8BIT_DEFAULT_PULSE_WIDTH + 5u));

    fmt_lock_editor_footer(&editor, footer);
    assert(strstr(footer, "K1 WIDTH") != NULL);

    assert(fmt_lock_editor_arm_delete(&editor));
    fmt_playback_param_cache[0] = 0u;
    fmt_8bit_row_param_cache.word[0][0] = 0xDEADBEEFu;
    assert(fmt_8bit_linked_editor_confirm_delete(&editor));
    assert(editor.last_status == (uint8_t)FMT_LOCK_EDITOR_STATUS_DELETED);
    assert(fmt_lock_get(0u, 0u, 0u, FMT_PARAM_BIT8_PULSE_WIDTH, &value) ==
           FMT_LOCK_STATUS_NOT_FOUND);
    assert(fmt_playback_params_read_track(0u, 0u) == fmt_playback_params_default_word());
    assert(fmt_8bit_linked_params_read(0u, 0u) == base_word);

    fmt_lock_editor_footer(&editor, footer);
    assert(strstr(footer, "DELETED K1 WIDTH") != NULL);
}

int main(void)
{
    test_registry_activation();
    test_layout_reset_and_defaults();
    test_note_only_inheritance_and_cache();
    test_single_row_refresh_and_base_rebuild();
    test_page_aware_editor_all_engines();
    test_editor_refresh_delete_and_footer();

    printf("WP4_9_GATE3_TESTS_OK=1\n");
    printf("registry_ids=10 bit8_ids=6 base_bank_bytes=%u row_cache_bytes=%u tracks=8 rows=64 page_slots=4 inheritance=note_only editor=page_aware refresh=d031+d044 d032_refresh=0 dinghy_migration=0\n",
           (unsigned)sizeof(fmt_8bit_instrument_base),
           (unsigned)sizeof(fmt_8bit_row_param_cache));
    return 0;
}
