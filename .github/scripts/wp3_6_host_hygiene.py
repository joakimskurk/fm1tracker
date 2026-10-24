from pathlib import Path

host = Path("tests/host.c")
s = host.read_text()
old = """static const host_ev_t SCRIPT0[] = {
    {200, 1u << 7, -1, 0, 0, 0, 0, 0, 0},                                /* C4 */
    {700, 1u << 7 | 1u << 11 | 1u << 14, -1, 0, 0, 0, 0, 0, 0},          /* + E4 G4: a chord */
    {1500, 0, -1, 0, 0, 0, 0, 0, 0},
    {1700, 0, -1, 0x90, 81, 120, 0, 0, 0},                               /* MIDI in: A5 */
    {2300, 0, -1, 0x80, 81, 0, 0, 0, 0},
    {2500, 0, BTN_OCT_UP, 0, 0, 0, 0, 0, 0},
    {2700, 1u << 2 | 1u << 9, -1, 0, 0, 0, 0, 0, 0},                     /* G4 + D5, an octave up */
    {3400, 0, -1, 0, 0, 0, 0, 0, 0},
};"""
new = """static const host_ev_t SCRIPT0[] = {
    { .t = 200,  .keys = 1u << 7, .btn = -1 },                           /* C4 */
    { .t = 700,  .keys = 1u << 7 | 1u << 11 | 1u << 14, .btn = -1 },     /* + E4 G4: a chord */
    { .t = 1500, .btn = -1 },
    { .t = 1700, .btn = -1, .st = 0x90, .d1 = 81, .d2 = 120 },           /* MIDI in: A5 */
    { .t = 2300, .btn = -1, .st = 0x80, .d1 = 81 },
    { .t = 2500, .btn = BTN_OCT_UP },
    { .t = 2700, .keys = 1u << 2 | 1u << 9, .btn = -1 },                 /* G4 + D5, an octave up */
    { .t = 3400, .btn = -1 },
};"""
assert s.count(old) == 1
s = s.replace(old, new, 1)
old = "        host_ev_t e = {0, 0, -1, 0, 0, 0, HOST_BTN_PULSE, 0, 0};\n"
new = "        host_ev_t e = { .btn = -1, .btn_action = HOST_BTN_PULSE };\n"
assert s.count(old) == 1
s = s.replace(old, new, 1)
host.write_text(s)

runner = Path("tests/run_wp3_6_effect_audio_host.sh")
s = runner.read_text()
old = "NAMES = ('control', 'arp', 'slide', 'porta', 'vibrato')\n"
new = """NAMES = ('control', 'arp', 'slide', 'porta', 'vibrato')
FROZEN_SHA = {
    'control': '8933b4ae192e314dbaa729b929ebfc0195178842ccd9e89306deffd71a9db1b6',
    'arp': '5e95abbdc6a0a9a65af402b7393b9b4088e968297d239679cd78b3872595a743',
    'slide': '2e205d5d46a6c604fbe6c5976dbf57f252dc4288de70ee69a66b7a6769d4e50e',
    'porta': '2d84235e03ca33d055baf2b2d0d3a430a1b28d3ff4f0faa913d5556f52f1f7e7',
    'vibrato': 'e14673fca74449f31783a44168edbf7146b0a1aa73f492dd9460febb7542c4a4',
}
"""
assert s.count(old) == 1
s = s.replace(old, new, 1)
old = """for name in NAMES:
    path, n, _ = loaded[name]
    print(f'WP3_6_{name.upper()}_WAV_SHA256={sha256(path.read_bytes()).hexdigest()}')
"""
new = """for name in NAMES:
    path, n, _ = loaded[name]
    digest = sha256(path.read_bytes()).hexdigest()
    assert digest == FROZEN_SHA[name], (name, digest)
    print(f'WP3_6_{name.upper()}_WAV_SHA256={digest}')
"""
assert s.count(old) == 1
s = s.replace(old, new, 1)
runner.write_text(s)

print("WP3_6_HOST_HYGIENE_PATCHED=1")
