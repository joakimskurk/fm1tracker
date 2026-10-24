from pathlib import Path

p = Path('tests/host.c')
s = p.read_text()

old = '#include "fmt_8bit_linked_params.h"\n'
new = '#include "fmt_8bit_linked_params.h"\n#include "fmt_8bit_engine.h"\n'
assert old in s
s = s.replace(old, new, 1)

old = ' *   "T base INSTRUMENT A D S R WIDTH TABLE", "T end"\n'
new = ' *   "T base INSTRUMENT A D S R WIDTH TABLE", "T probe", "T end"\n'
assert old in s
s = s.replace(old, new, 1)

old = '''    HOST_FIX_LOCK = 3u,\n    HOST_FIX_BASE = 4u\n};\n'''
new = '''    HOST_FIX_LOCK = 3u,\n    HOST_FIX_BASE = 4u,\n    HOST_FIX_PROBE = 5u\n};\n'''
assert old in s
s = s.replace(old, new, 1)

old = '''        else if (!strcmp(what, "clear")) { e.fixture_action = HOST_FIX_CLEAR; }\n'''
new = '''        else if (!strcmp(what, "clear")) { e.fixture_action = HOST_FIX_CLEAR; }\n        else if (!strcmp(what, "probe")) { e.fixture_action = HOST_FIX_PROBE; }\n'''
assert old in s
s = s.replace(old, new, 1)

old = '''        } else if (script[step].fixture_action == HOST_FIX_BASE) {\n            Fmt8BitParamSet base;\n'''
new = '''        } else if (script[step].fixture_action == HOST_FIX_PROBE) {\n            uint32_t track, slot;\n            printf("%6u ms  fixture probe tracks", fm1_ms);\n            for (track = 0u; track < FMT_TRACKS; track++) {\n                uint8_t v = fmt_8bit_engine_track_voice(&fmt_8bit_engine_runtime, (uint8_t)track);\n                if (v == FMT_VOICE_NONE)\n                    printf(" t%u=-", track);\n                else\n                    printf(" t%u=v%u/e%u/i%u/s%u", track, v,\n                           fmt_8bit_engine_runtime.voice[v].engine,\n                           fmt_8bit_engine_runtime.voice[v].instrument,\n                           fmt_8bit_engine_runtime.voice[v].envelope.stage);\n            }\n            printf(" slots");\n            for (slot = 0u; slot < FMT_VOICE_ALLOCATOR_SLOTS; slot++)\n                printf(" v%u=o%u/s%u/e%u", slot,\n                       fmt_8bit_engine_runtime.allocator.voice_owner[slot],\n                       fmt_8bit_engine_runtime.voice[slot].envelope.stage,\n                       fmt_8bit_engine_runtime.voice[slot].engine);\n            putchar('\\n');\n        } else if (script[step].fixture_action == HOST_FIX_BASE) {\n            Fmt8BitParamSet base;\n'''
assert old in s
s = s.replace(old, new, 1)

p.write_text(s)
