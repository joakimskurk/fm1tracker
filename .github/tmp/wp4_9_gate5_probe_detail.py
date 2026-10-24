from pathlib import Path

p = Path('tests/host.c')
s = p.read_text()
old = '''            for (slot = 0u; slot < FMT_VOICE_ALLOCATOR_SLOTS; slot++)
                printf(" v%u=o%u/s%u/e%u", slot,
                       fmt_8bit_engine_runtime.allocator.voice_owner[slot],
                       fmt_8bit_engine_runtime.voice[slot].envelope.stage,
                       fmt_8bit_engine_runtime.voice[slot].engine);
            putchar('\\n');
'''
new = '''            for (slot = 0u; slot < FMT_VOICE_ALLOCATOR_SLOTS; slot++)
                printf(" v%u=o%u/s%u/e%u", slot,
                       fmt_8bit_engine_runtime.allocator.voice_owner[slot],
                       fmt_8bit_engine_runtime.voice[slot].envelope.stage,
                       fmt_8bit_engine_runtime.voice[slot].engine);
            printf(" transport=step%u/phase%u", fmt_transport_runtime.step,
                   fmt_transport_runtime.phase);
            putchar('\\n');
            for (slot = 0u; slot < FMT_VOICE_ALLOCATOR_SLOTS; slot++) {
                const Fmt8BitVoice *v = &fmt_8bit_engine_runtime.voice[slot];
                uint32_t owner = fmt_8bit_engine_runtime.allocator.voice_owner[slot];
                uint32_t phase = v->osc.pulse.phase;
                uint32_t lfsr = v->engine == FMT_8BIT_ENGINE_NOISE
                              ? v->osc.noise.lfsr : 0u;
                printf("%6u ms  fixture voice v%u owner%u eng%u inst%u stage%u pitch%u gain%u "
                       "page%08x level%u rate%u decay%u sustain%u phase%08x lfsr%u\\n",
                       fm1_ms, slot, owner, v->engine, v->instrument,
                       v->envelope.stage, v->pitch_q7, v->gain_q15,
                       (unsigned)v->page_word, (unsigned)v->envelope.level_q16,
                       (unsigned)v->envelope.rate_q16,
                       (unsigned)v->envelope.decay_rate_q16,
                       (unsigned)v->envelope.sustain_q16,
                       (unsigned)phase, (unsigned)lfsr);
            }
            for (track = 0u; track < FMT_TRACKS; track++) {
                FmtTrackerEffectState *effect = tracker_effect_state(track);
                FmtTrackerRowEventState *row_event = tracker_row_event_state(track);
                printf("%6u ms  fixture state t%u tick%u cmd%u value%u pitch%u transient%d "
                       "pending%u pnote%u common%08x page%08x currenti%u notei%u\\n",
                       fm1_ms, track,
                       effect ? effect->tick : 0u,
                       effect ? effect->command : 0u,
                       effect ? effect->value : 0u,
                       effect ? effect->pitch_q7 : 0u,
                       effect ? effect->transient_q7 : 0,
                       row_event ? row_event->pending_valid : 0u,
                       row_event ? row_event->pending_note : 0u,
                       (unsigned)tracker_common_row_word[track],
                       (unsigned)tracker_page_row_word[track],
                       tracker_current_instrument[track],
                       tracker_note_instrument[track]);
            }
'''
assert s.count(old) == 1, s.count(old)
s = s.replace(old, new, 1)
p.write_text(s)
