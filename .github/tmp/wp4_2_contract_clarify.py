from pathlib import Path

p = Path('docs/WP4_2_PULSE_CONTRACT.md')
s = p.read_text()
old = 'The legal Q7 maximum can interpolate from MIDI 127 toward the internal MIDI-128 increment, so the helper must safely support the internal lookup point `128` even though stored tracker notes remain `0..127`.'
new = old + '\n\nThe public/legal D037 engine-pitch domain still ends at `16383`; MIDI 128 is **not** a legal engine pitch. It exists only as the private upper lookup value needed when interpolating the legal maximum fractional pitch below MIDI 128.'
assert s.count(old) == 1
s = s.replace(old, new, 1)
old = '12. monotonic integer-semitone phase increments over MIDI 0..128;'
new = '12. monotonic integer-semitone phase increments over legal MIDI 0..127, plus the exact maximum-Q7 vector whose interpolation exercises the private MIDI-128 upper lookup;'
assert s.count(old) == 1
s = s.replace(old, new, 1)
p.write_text(s)
print('D038_MIDI128_CLARIFICATION_OK=1 legal_pitch_max=16383 internal_lookup=128')

# Deliberate trigger after the one-time workflow exists.
