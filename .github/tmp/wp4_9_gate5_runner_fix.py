from pathlib import Path
p = Path('tests/run_wp4_9_linked_audio_host.sh')
s = p.read_text()
old = '''  sha256sum "build/host/wp4_9_$name.wav" | awk -v n="$name" '{print "WP4_9_" toupper(n) "_WAV_SHA256=" $1}' toupper="$name"\n'''
new = '''  hash="$(sha256sum "build/host/wp4_9_$name.wav" | awk '{print $1}')"\n  upper="$(printf '%s' "$name" | tr '[:lower:]' '[:upper:]')"\n  printf 'WP4_9_%s_WAV_SHA256=%s\\n' "$upper" "$hash"\n'''
assert s.count(old) == 1, s.count(old)
p.write_text(s.replace(old, new, 1))
