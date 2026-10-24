from pathlib import Path

for name in ('firmware/src/fmt_playback_params.c', 'firmware/src/fmt_tracker_effect.c'):
    p = Path(name)
    s = p.read_text()
    old = 'return (volatile uint32_t *)0;'
    assert s.count(old) == 1, (name, s.count(old))
    s = s.replace(old, 'return 0;', 1)
    p.write_text(s)
