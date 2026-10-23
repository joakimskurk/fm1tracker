/* SPDX-License-Identifier: GPL-3.0-only */
/* Dinghy: the smallest FM-1 firmware to build your own on. A sine wave on the keys, and that is all.
 * This is the file to change. Everything below it (platform.c) is done: updates, the USB rescue,
 * USB-MIDI, USB audio, keys, knobs, LEDs, the screen, a settings sector.
 *
 *   keys       play sines, 4 voices; the notes go out over USB-MIDI too
 *   OCT- OCT+  the octave (kept in flash)
 *   MASTER     the volume (platform.c)
 *   USB-MIDI   notes in play the sines, with their velocity
 *
 * Rules of the road (the chip has no FPU and no libgcc): integers only, no float, no 64-bit division.
 * tests/host.c runs this file on a computer: the screen as a PNG, the sound as a WAV.
 * What platform.c offers: docs/API.md. The chip's rules: docs/RULES.md. */
#include "platform.c"
#include "fmt_store.c"                         /* WP2.2: one bounds-safe pool-backed editable song */
#include "fmt_transport.c"                     /* WP2.3: exact integer 1/16-note transport timing */
#include "fmt_playback.c"                      /* WP2.4/2.8: exact one-lane row/special dispatch */
#include "fmt_pattern_input.c"                 /* WP2.5/2.6: bounded SEQ step/track cursor addressing */
#include "fmt_step_record.c"                   /* WP2.7/2.8: stopped STEP REC note/special entry */

/* Externally visible so CI can verify the small runtime control states stay in
 * ordinary RAM and never consume editable-song pool space. */
FmtTransport fmt_transport_runtime;
FmtPlayback fmt_playback_runtime;
FmtPatternInput fmt_pattern_input_runtime;
FmtStepRecord fmt_step_record_runtime;

/* app_main seeds the proof pattern before setting this one-way handoff. Audio
 * IRQs exist before app_main(), so the ISR must not read the song store earlier. */
static volatile uint32_t tracker_ready;

/* D024 transport handoff. Main owns play_requested; audio owns stopped_ack and
 * the actual FmtTransport running field. This keeps the ISR transport one-writer
 * while STEP REC waits for a real stop before mutating the shared song store. */
static volatile uint32_t tracker_play_requested;
static volatile uint32_t tracker_transport_stopped;

/* ---- the sound (runs in the audio ISR: keep it short, no flash, no screen) */
#define NV 4u                                   /* manual/audition voices */
#define TRACKER_TARGET_AMP 6000
static struct {
    volatile uint32_t inc;                      /* phase step per frame: f * 2^32 / 44100 */
    volatile int32_t gate;                      /* target level, Q15: 0 = released */
    int32_t amp;                                /* the level now: glides to gate, no clicks */
    uint32_t ph, age;
    uint8_t note;
} voice[NV];

/* Dedicated monophonic proof voice. D025 adds a target gate so OFF can release
 * click-safely while the default WP2.4 note-only render remains sample-identical. */
static struct {
    uint32_t inc;
    uint32_t ph;
    int32_t amp;
    int32_t gate;
} tracker_voice;

static const uint32_t INC_C9[12] = {            /* MIDI 120..131 (C9..B9); lower octaves shift right */
    815363807u, 863847862u, 915214929u, 969636441u, 1027294024u, 1088380105u,
    1153098554u, 1221665363u, 1294309365u, 1371273005u, 1452813141u, 1539201906u};

static uint32_t note_inc(uint32_t note)
{
    return INC_C9[note % 12u] >> (10u - note / 12u);
}

static int32_t sine(uint32_t ph)                /* a full cycle over 2^32 -> -32767..32767 (a refined parabola, ~0.1 %) */
{
    int32_t x = (int32_t)ph >> 16, ax = x < 0 ? -x : x, y, ay;
    y = (x * (32768 - ax)) >> 13;               /* 4x(1-|x|), Q15 */
    ay = y < 0 ? -y : y;
    y += ((((y * ay) >> 15) - y) * 7373) >> 15; /* + 0.225 (y|y| - y) */
    return y > 32767 ? 32767 : y < -32767 ? -32767 : y;
}

static void tracker_apply_event(const FmtPlaybackEvent *event)
{
    if (!event)
        return;

    if (event->kind == FMT_PLAYBACK_EVENT_NOTE && event->note <= FMT_NOTE_MAX) {
        tracker_voice.inc = note_inc(event->note);
        tracker_voice.gate = TRACKER_TARGET_AMP;
    } else if (event->kind == FMT_PLAYBACK_EVENT_TIE) {
        /* TIE sustains an already gated note but may not resurrect one that was
         * explicitly released. Pitch/phase are deliberately untouched. */
        if (tracker_voice.inc && tracker_voice.gate > 0)
            tracker_voice.gate = TRACKER_TARGET_AMP;
    } else if (event->kind == FMT_PLAYBACK_EVENT_OFF) {
        tracker_voice.gate = 0;
    }
}

static void tracker_render_frame(int32_t *out)
{
    uint32_t ph, inc;
    int32_t amp, gate, s;

    inc = tracker_voice.inc;
    amp = tracker_voice.amp;
    gate = tracker_voice.gate;
    if (!inc && !amp && !gate)
        return;
    if (!inc)
        return;

    ph = tracker_voice.ph + inc;
    if (amp < gate) {
        amp += 48;
        if (amp > gate)
            amp = gate;
    } else if (amp > gate) {
        amp -= 6;
        if (amp < gate)
            amp = gate;
    }
    s = (sine(ph) * amp) >> 15;
    out[0] += s;
    out[1] += s;
    tracker_voice.ph = ph;
    tracker_voice.amp = amp;
    if (!amp && !gate)
        tracker_voice.inc = 0u;
}

static void app_audio(int32_t *out, uint32_t n)
{
    uint32_t v, i;
    FmtPlaybackEvent event;

    /* Main publishes only the desired run state. Audio owns all writes to the
     * transport runtime, then publishes a stopped acknowledgement for editor
     * safety. Default requested=1 preserves WP2.4's automatic proof playback. */
    if (tracker_ready) {
        if (!tracker_play_requested) {
            if (fmt_transport_runtime.running)
                fmt_transport_stop(&fmt_transport_runtime);
            tracker_transport_stopped = 1u;
        } else {
            tracker_transport_stopped = 0u;
            if (!fmt_playback_runtime.started) {
                if (fmt_playback_start(&fmt_playback_runtime, &fmt_transport_runtime, &event))
                    tracker_apply_event(&event);
            } else if (!fmt_transport_runtime.running) {
                fmt_transport_start(&fmt_transport_runtime);
            }
        }
    }

    /* One proof lane must observe sub-block row boundaries exactly. Render the
     * current row first, then tick elapsed time; a crossed row applies to the
     * next frame, even when the boundary lands inside a normal 256-frame block.
     * When transport is stopped the proof tone may sustain, but no row/store
     * read occurs until audio has resumed the transport. */
    if (fmt_playback_runtime.started) {
        for (i = 0u; i < n; i++) {
            tracker_render_frame(out + 2u * i);
            if (fmt_playback_end_frame(&fmt_playback_runtime, &fmt_transport_runtime, &event))
                tracker_apply_event(&event);
        }
    }

    /* Preserve Dinghy's original manual/audition voice path unchanged. */
    for (v = 0; v < NV; v++) {
        uint32_t ph = voice[v].ph, step = voice[v].inc;
        int32_t amp = voice[v].amp, g = voice[v].gate;
        if (!amp && !g)
            continue;
        for (i = 0; i < n; i++) {
            int32_t s;
            if (amp < g) { amp += 48; if (amp > g) amp = g; }
            else if (amp > g) { amp -= 6; if (amp < g) amp = g; }
            ph += step;
            s = (sine(ph) * amp) >> 15;
            out[2u * i] += s;
            out[2u * i + 1u] += s;
        }
        voice[v].ph = ph;
        voice[v].amp = amp;
    }
}

/* ---- notes (main loop) */
static uint32_t ages, last_note = 255;

static void play(uint32_t note, uint32_t vel)
{
    uint32_t v, pick = 0, best = 0xFFFFFFFFu;
    if (note > 127u || !vel) return;
    for (v = 0; v < NV; v++) {
        uint32_t score = voice[v].note == note && voice[v].gate ? 0u
                       : voice[v].gate ? 0x80000000u + voice[v].age : (uint32_t)voice[v].amp + 1u;
        if (score < best) { best = score; pick = v; }
    }
    voice[pick].note = (uint8_t)note;
    voice[pick].age = ++ages;
    voice[pick].inc = note_inc(note);
    voice[pick].gate = (int32_t)(vel * 90u);
    last_note = note;
}
static void stop(uint32_t note)
{
    uint32_t v;
    for (v = 0; v < NV; v++)
        if (voice[v].note == note)
            voice[v].gate = 0;
}
static int quiet(void)
{
    uint32_t v;

    /* settings_save() erases flash and waits for silence. A sustained tracker
     * gate or a release tail therefore still blocks flash just like WP2.4 did. */
    if (tracker_voice.inc || tracker_voice.amp || tracker_voice.gate)
        return 0;

    for (v = 0; v < NV; v++)
        if (voice[v].gate || voice[v].amp) return 0;
    return 1;
}

/* ---- the screen: the name, the last note big, the keyboard with what sounds, the octave */
#define C_ACC RGB(180, 140, 255)
#define C_KEY RGB(210, 210, 210)
#define C_OFF RGB(90, 90, 90)
#define KEY0 53u
static int black(uint32_t note) { return (0x54Au >> (note % 12u)) & 1u; }

static void draw_note(void)
{
    static const char *const NAME[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    char s[8] = "-";
    if (last_note < 128u) {
        const char *p = NAME[last_note % 12u];
        uint32_t i = 0;
        while (*p) s[i++] = *p++;
        if (last_note >= 12u) s[i++] = (char)('0' + last_note / 12u - 1u);
        s[i] = 0;
    }
    draw_text_box(0, 82, 240, &FONT_L, s, C_WHITE, 1);
}

static void draw_keys(uint32_t lit)
{
    uint32_t k, w = 0;
    cv_begin(240, 56, C_BLACK);
    for (k = 0; k < 27u; k++)
        if (!black(KEY0 + k)) {
            cv_rect(8 + (int32_t)w * 14, 0, 13, 56, lit >> k & 1u ? C_ACC : C_KEY);
            w++;
        }
    for (k = 0, w = 0; k < 27u; k++)
        if (black(KEY0 + k))
            cv_rect(8 + (int32_t)w * 14 - 5, 0, 9, 34, lit >> k & 1u ? C_ACC : C_BLACK);
        else
            w++;
    cv_blit(0, 140);
}

static void draw_status(int32_t oct)
{
    char s[12] = "OCTAVE +0";
    s[7] = oct < 0 ? '-' : '+';
    s[8] = (char)('0' + (oct < 0 ? -oct : oct));
    draw_text_box(0, 212, 240, &FONT_S, s, C_OFF, 1);
}

static void app_main(void)
{
    struct { int8_t oct; } set = {0};
    uint32_t prev = 0, k, t_change = 0, dirty = 0, lit_drawn = ~0u, note_drawn = ~0u;
    int32_t oct_drawn = 99;
    uint8_t held[27] = {0};

    fmt_store_reset();
    fmt_playback_seed_proof_pattern();
    fmt_transport_reset(&fmt_transport_runtime);
    fmt_playback_reset(&fmt_playback_runtime);
    fmt_pattern_input_reset(&fmt_pattern_input_runtime);
    fmt_step_record_reset(&fmt_step_record_runtime);
    tracker_play_requested = 1u;
    tracker_transport_stopped = 1u;
    tracker_ready = 1u;

    settings_load(&set, sizeof set);
    if (set.oct < -2 || set.oct > 2) set.oct = 0;

    lcd_fill(0, 0, 240, 240, C_BLACK);
    draw_text_box(0, 18, 240, &FONT_L, APP_NAME, C_ACC, 1);
    draw_text_box(0, 52, 240, &FONT_S, "a sine to build on  v" APP_VER, C_OFF, 1);

    for (;;) {
        uint32_t keys, down, up, rel, b, lit = 0, base, write_key;
        uint8_t st, d1, d2, lowest_key;
        int pattern_mode, step_rec_mode;
        platform_poll();

        b = fm1_input_edges(&rel);

        if (b & (1u << BTN_HOME)) {
            fmt_step_record_exit(&fmt_step_record_runtime);
            fmt_pattern_input_enter_performance(&fmt_pattern_input_runtime);
        } else {
            if (b & (1u << BTN_SEQ))
                fmt_pattern_input_enter_pattern(&fmt_pattern_input_runtime);

            if (b & (1u << BTN_PLAY)) {
                if (fmt_step_record_is_active(&fmt_step_record_runtime)) {
                    fmt_step_record_exit(&fmt_step_record_runtime);
                    tracker_play_requested = 1u;
                } else {
                    tracker_play_requested = tracker_play_requested ? 0u : 1u;
                }
            }

            if (b & (1u << BTN_REC)) {
                if (fmt_step_record_is_active(&fmt_step_record_runtime)) {
                    fmt_step_record_exit(&fmt_step_record_runtime);
                } else if (!tracker_play_requested && tracker_transport_stopped) {
                    (void)fmt_step_record_enter(&fmt_step_record_runtime,
                                                &fmt_pattern_input_runtime,
                                                &fmt_transport_runtime);
                }
            }
        }

        pattern_mode = fmt_pattern_input_is_pattern(&fmt_pattern_input_runtime);
        step_rec_mode = fmt_step_record_is_active(&fmt_step_record_runtime);

        /* D025's pending FX state lives in FmtStepRecord so modifier ordering,
         * cancellation and writes are covered by the same editor unit tests. */
        if (step_rec_mode)
            (void)fmt_step_record_special_input(&fmt_step_record_runtime,
                                                &fmt_pattern_input_runtime,
                                                &fmt_transport_runtime,
                                                (b & (1u << BTN_FX)) != 0u,
                                                (rel & (1u << BTN_FX)) != 0u,
                                                (b & (1u << BTN_SEL)) != 0u);

        if (pattern_mode && !step_rec_mode) {
            if (b & (1u << BTN_OCT_DOWN))
                (void)fmt_pattern_input_page_prev(&fmt_pattern_input_runtime);
            if (b & (1u << BTN_OCT_UP))
                (void)fmt_pattern_input_page_next(&fmt_pattern_input_runtime);
        } else {
            if (b & (1u << BTN_OCT_DOWN) && set.oct > -2) { set.oct--; dirty = 1; t_change = fm1_ms; }
            if (b & (1u << BTN_OCT_UP) && set.oct < 2) { set.oct++; dirty = 1; t_change = fm1_ms; }
        }

        base = (uint32_t)(KEY0 + 12 * set.oct);
        keys = fm1_in.notes;
        down = keys & ~prev;
        up = prev & ~keys;
        prev = keys;

        write_key = FMT_PATTERN_INPUT_KEY_COUNT;
        if (step_rec_mode && !fmt_step_record_runtime.gesture_latched &&
            fmt_step_record_lowest_key(down, &lowest_key))
            write_key = lowest_key;

        for (k = 0; k < 27u; k++) {
            if ((up >> k & 1u) && held[k]) {
                stop(held[k]);
                midi_send(0x80, held[k], 0);
                held[k] = 0u;
            }
            if (down >> k & 1u) {
                if (step_rec_mode) {
                    held[k] = (uint8_t)(base + k);
                    play(held[k], 100);
                    midi_send(0x90, held[k], 100);
                } else if (pattern_mode) {
                    if (!fmt_pattern_input_select_key(&fmt_pattern_input_runtime, k))
                        (void)fmt_pattern_input_select_track_key(&fmt_pattern_input_runtime, k);
                } else {
                    held[k] = (uint8_t)(base + k);
                    play(held[k], 100);
                    midi_send(0x90, held[k], 100);
                }
            }
        }

        if (step_rec_mode && write_key < FMT_PATTERN_INPUT_KEY_COUNT)
            (void)fmt_step_record_write_note(&fmt_step_record_runtime,
                                             &fmt_pattern_input_runtime,
                                             &fmt_transport_runtime,
                                             base + write_key);
        if (step_rec_mode && keys == 0u)
            fmt_step_record_release_gesture(&fmt_step_record_runtime);

        while (midi_get(&st, &d1, &d2)) {
            if ((st & 0xF0u) == 0x90u && d2) play(d1, d2);
            else if ((st & 0xF0u) == 0x80u || (st & 0xF0u) == 0x90u) stop(d1);
        }

        base = (uint32_t)(KEY0 + 12 * set.oct);
        for (k = 0; k < NV; k++)
            if (voice[k].gate && voice[k].note >= base && voice[k].note < base + 27u)
                lit |= 1u << (voice[k].note - base);
        for (k = 0; k < 27u; k++)
            fm1_led_key(KEY_LED(k), lit >> k & 1u);

        fm1_led_key(BTN_HOME, !pattern_mode);
        fm1_led_key(BTN_SEQ, pattern_mode);
        fm1_led_key(BTN_PLAY, tracker_play_requested != 0u);
        fm1_led_key(BTN_REC, step_rec_mode);
        fm1_led_key(BTN_FX, step_rec_mode && fmt_step_record_runtime.fx_pending);
        fm1_led_key(BTN_OCT_DOWN, pattern_mode && !step_rec_mode ? fmt_pattern_input_runtime.page > 0u : set.oct < 0);
        fm1_led_key(BTN_OCT_UP, pattern_mode && !step_rec_mode ? fmt_pattern_input_runtime.page + 1u < FMT_PATTERN_INPUT_PAGE_COUNT : set.oct > 0);

        if (lit != lit_drawn) { lit_drawn = lit; draw_keys(lit); }
        if (last_note != note_drawn) { note_drawn = last_note; draw_note(); }
        if (set.oct != oct_drawn) { oct_drawn = set.oct; draw_status(set.oct); }

        if (dirty && quiet() && fm1_ms - t_change > 2000u) {
            settings_save(&set, sizeof set);
            dirty = 0;
        }
    }
}
