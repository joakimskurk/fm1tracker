from pathlib import Path

p = Path('firmware/src/dinghy.c')
s = p.read_text()

def once(old, new):
    global s
    count = s.count(old)
    assert count == 1, (old[:120], count)
    s = s.replace(old, new, 1)

once(
    '#include "fmt_tracker_event.c"                 /* WP3.7: D033 fixed-state retrig/delay/cut scheduler */\n#include "fmt_pattern_input.c"',
    '#include "fmt_tracker_event.c"                 /* WP3.7: D033 fixed-state retrig/delay/cut scheduler */\n#include "fmt_rt_queue.c"                      /* WP3.8: D034 bounded main-to-audio event ingress */\n#include "fmt_pattern_input.c"')

once(
    'FmtTrackerEffectState fmt_tracker_effect_runtime;\nFmtTrackerRowEventState fmt_tracker_row_event_runtime;\n',
    'FmtTrackerEffectState fmt_tracker_effect_runtime;\nFmtTrackerRowEventState fmt_tracker_row_event_runtime;\nFmtRtQueue fmt_rt_queue_runtime;\nvolatile uint32_t fmt_rt_queue_overflow_seen;\n')

once(
'''static struct {\n    volatile uint32_t inc;                      /* phase step per frame: f * 2^32 / 44100 */\n    volatile int32_t gate;                      /* target level, Q15: 0 = released */\n    int32_t amp;                                /* the level now: glides to gate, no clicks */\n    uint32_t ph, age;\n    uint8_t note;\n} voice[NV];\n''',
'''static struct {\n    volatile uint32_t inc;                      /* phase step per frame: f * 2^32 / 44100 */\n    volatile int32_t gate;                      /* target level, Q15: 0 = released */\n    volatile int32_t amp;                       /* audio-owned; main reads only for UI/quiet */\n    uint32_t ph, age;\n    volatile uint8_t note;                      /* audio-owned; main reads only for LEDs */\n} voice[NV];\nstatic uint32_t audition_ages;\n''')

anchor = '''static void app_audio(int32_t *out, uint32_t n)\n{\n    uint32_t v, i;\n    FmtPlaybackEvent event;\n'''
insert = '''static void audition_release_all(void)\n{\n    uint32_t v;\n    for (v = 0u; v < NV; v++)\n        voice[v].gate = 0;\n}\n\nstatic void audition_note_on(uint8_t note, uint8_t vel)\n{\n    uint32_t v, pick = 0u, best = 0xFFFFFFFFu;\n\n    if (note > 127u || !vel || vel > 127u)\n        return;\n    for (v = 0u; v < NV; v++) {\n        uint32_t score = voice[v].note == note && voice[v].gate ? 0u\n                       : voice[v].gate ? 0x80000000u + voice[v].age\n                       : (uint32_t)voice[v].amp + 1u;\n        if (score < best) {\n            best = score;\n            pick = v;\n        }\n    }\n    voice[pick].note = note;\n    voice[pick].age = ++audition_ages;\n    voice[pick].inc = note_inc(note);\n    voice[pick].gate = (int32_t)((uint32_t)vel * 90u);\n}\n\nstatic void audition_note_off(uint8_t note)\n{\n    uint32_t v;\n    for (v = 0u; v < NV; v++)\n        if (voice[v].note == note)\n            voice[v].gate = 0;\n}\n\nstatic void audition_consume_queue(void)\n{\n    uint32_t i, overflow;\n    FmtRtEvent event;\n\n    overflow = fmt_rt_queue_overflow_seq(&fmt_rt_queue_runtime);\n    if (overflow != fmt_rt_queue_overflow_seen) {\n        audition_release_all();\n        fmt_rt_queue_consumer_discard_all(&fmt_rt_queue_runtime);\n        fmt_rt_queue_overflow_seen = overflow;\n        return;\n    }\n\n    for (i = 0u; i < FMT_RT_QUEUE_CAPACITY; i++) {\n        uint8_t note, vel;\n\n        if (!fmt_rt_queue_dequeue(&fmt_rt_queue_runtime, &event))\n            break;\n        if (event.track >= FMT_TRACKS)\n            continue;\n        note = fmt_rt_event_note(&event);\n        if (note > FMT_NOTE_MAX)\n            continue;\n        if (event.type == FMT_RT_EVENT_NOTE_ON) {\n            vel = fmt_rt_event_velocity(&event);\n            if (vel != 0u && vel <= 127u)\n                audition_note_on(note, vel);\n        } else if (event.type == FMT_RT_EVENT_NOTE_OFF) {\n            audition_note_off(note);\n        }\n    }\n}\n\nstatic void app_audio(int32_t *out, uint32_t n)\n{\n    uint32_t v, i;\n    FmtPlaybackEvent event;\n\n    /* D034: bounded main->audio ingress is consumed only at callback start.\n     * Tracker D020-D033 timing remains on its independently verified path. */\n    audition_consume_queue();\n'''
once(anchor, insert)

once(
    '    /* Preserve Dinghy\'s original manual/audition voice path unchanged. */\n',
    '    /* D034 keeps rendering policy unchanged, but audio now owns every\n     * manual voice mutation as well as the render state. */\n')

old_notes = '''/* ---- notes (main loop) */\nstatic uint32_t ages, last_note = 255;\n\nstatic void play(uint32_t note, uint32_t vel)\n{\n    uint32_t v, pick = 0, best = 0xFFFFFFFFu;\n    if (note > 127u || !vel) return;\n    for (v = 0; v < NV; v++) {\n        uint32_t score = voice[v].note == note && voice[v].gate ? 0u\n                       : voice[v].gate ? 0x80000000u + voice[v].age : (uint32_t)voice[v].amp + 1u;\n        if (score < best) { best = score; pick = v; }\n    }\n    voice[pick].note = (uint8_t)note;\n    voice[pick].age = ++ages;\n    voice[pick].inc = note_inc(note);\n    voice[pick].gate = (int32_t)(vel * 90u);\n    last_note = note;\n}\nstatic void stop(uint32_t note)\n{\n    uint32_t v;\n    for (v = 0; v < NV; v++)\n        if (voice[v].note == note)\n            voice[v].gate = 0;\n}\n'''
new_notes = '''/* ---- notes (main loop) */\nstatic uint32_t last_note = 255u;\n\nstatic void play(uint32_t note, uint32_t vel)\n{\n    FmtRtQueueStatus status;\n    uint8_t track = fmt_pattern_input_runtime.track;\n\n    if (note > 127u || !vel || vel > 127u)\n        return;\n    status = fmt_rt_queue_enqueue_note_on(&fmt_rt_queue_runtime,\n                                          track,\n                                          (uint8_t)note,\n                                          (uint8_t)vel);\n    if (status == FMT_RT_QUEUE_OK)\n        last_note = note;\n}\nstatic void stop(uint32_t note)\n{\n    if (note > 127u)\n        return;\n    (void)fmt_rt_queue_enqueue_note_off(&fmt_rt_queue_runtime,\n                                        fmt_pattern_input_runtime.track,\n                                        (uint8_t)note);\n}\n'''
once(old_notes, new_notes)

once(
'''    /* settings_save() erases flash and waits for silence. A sustained tracker\n     * gate or a release tail therefore still blocks flash just like WP2.4 did. */\n    if (tracker_voice.inc || tracker_voice.amp || tracker_voice.gate)\n        return 0;\n''',
'''    /* D034 includes pending ingress and unresolved overflow recovery in the\n     * silence contract so flash work cannot overtake a queued NOTE_ON/OFF. */\n    if (fmt_rt_queue_depth(&fmt_rt_queue_runtime) != 0u ||\n        fmt_rt_queue_overflow_seq(&fmt_rt_queue_runtime) != fmt_rt_queue_overflow_seen)\n        return 0;\n\n    /* settings_save() erases flash and waits for silence. A sustained tracker\n     * gate or a release tail therefore still blocks flash just like WP2.4 did. */\n    if (tracker_voice.inc || tracker_voice.amp || tracker_voice.gate)\n        return 0;\n''')

once(
'''    fmt_tracker_effect_state_reset(&fmt_tracker_effect_runtime);\n    fmt_tracker_row_event_reset(&fmt_tracker_row_event_runtime);\n    tracker_effect_row_word = fmt_playback_params_default_word();\n''',
'''    fmt_tracker_effect_state_reset(&fmt_tracker_effect_runtime);\n    fmt_tracker_row_event_reset(&fmt_tracker_row_event_runtime);\n    fmt_rt_queue_reset(&fmt_rt_queue_runtime);\n    fmt_rt_queue_overflow_seen = 0u;\n    audition_ages = 0u;\n    tracker_effect_row_word = fmt_playback_params_default_word();\n''')

p.write_text(s)
print('WP3_8_AUDIO_QUEUE_INTEGRATED=1')
