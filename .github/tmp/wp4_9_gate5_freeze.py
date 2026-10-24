from pathlib import Path
p = Path('tests/run_wp4_9_linked_audio_host.sh')
s = p.read_text()

old = """engines = ['pulse','noise','triangle','wavetable']
engine_hash = [sha(x) for x in engines]
assert len(set(engine_hash)) == 4, engine_hash
for name in engines:
    assert first_nonzero(wav_frames(name)) is not None, name
"""
new = """engines = ['pulse','noise','triangle','wavetable']
expected_sha = {
    'pulse': 'd2661eccaf3f55dd31730339b107aa56c136d3dcc43bca2520de160fc99b60a4',
    'noise': 'ae55ce5607e8a0e4a0c0c04b60d2927d4fdc052ef43b186e7b7725a03548e1e0',
    'triangle': 'a338149a5a98bd108b8c72a80eb149029e604a6067cda592e1139a13114f7748',
    'wavetable': '4adf4c4fd64ca9dab4f2ae0562f8b70830924b0d7dabad8f14215de4f69897dd',
    'mixed': '124ccc7e1ad0126cab8b4b51105596c88315b581339add1cdcf8fa978d53713b',
    'ownership': 'f87d6928e1648e5dfbbd157e79dd3c23ca422fb83829cf8ba9adb56348609bf4',
    'delay_control': 'b9a0280cf871425f1de9f4db34957ecd9e0ceab30342af61a10062aeae7a8f1a',
    'delay3': '8cf8a52d0e7910212036ae29b353f1d72b43dd58e3d3e461e33b419f57905973',
    'retrig_control': 'fcfe39f4fc2df8d2338556747eb2c8c43f46b1d868e71ba92d54e42fc37e9827',
    'retrig2': '4616cb08cdb6b4d96147e714dc0717b91c5517ca76702d7af4db96813dd40636',
    'cut_control': 'b9a0280cf871425f1de9f4db34957ecd9e0ceab30342af61a10062aeae7a8f1a',
    'cut3': '719b55cfceaf7840154f74ba8fd186d47211ad2f2e4e549d4f78f9b901bce3f6',
    'precedence_slide': 'e13c25e1f95a25bb29ccb72520efaa252eb8b39ccb7dbc8df336eb42e09aa657',
    'precedence_porta': 'fa70846fb85ea7531cdc3c5be39c71991f77ce92243d266ebcf3666d355bfdd5',
    'precedence_vibrato': '71cb1fdee84c8361e25b80bbc991abe3a50755cbe9b289abfe1e3c0fabf894a4',
    'envelope_snapshot': '64c0eb64fcf9f0d2d153c5939ca63dac1957d401c4394033d621691a03a58ee5',
    'release_slow': '72e7efe314641949c3acc82e84ea0b09f1840f65f9cdea0477352be1358cfc4e',
    'release_fast': '2e0983592a09cfabc10d67d07c92ce8dfdf5f2811bd4134d15679b5825c46533',
    'wavetable_lock': '2ab947a733a78f7a3d3e6717b96f2364d64fad4062b854128bf66466e9e8d7c0',
    'unrelated_lock': 'a9f2be188d70469df028b1b50911ced3244fed961ef2938d845cf736257c1fec',
}
for name, expected in expected_sha.items():
    actual = sha(name)
    assert actual == expected, (name, actual, expected)
engine_hash = [sha(x) for x in engines]
assert len(set(engine_hash)) == 4, engine_hash
for name in engines:
    assert first_nonzero(wav_frames(name)) is not None, name
"""
assert s.count(old) == 1, s.count(old)
s = s.replace(old, new, 1)

old = """assert delay_onset is not None and retrig_diff is not None and cut_diff is not None
"""
new = """assert delay_onset == 2757, delay_onset
assert retrig_diff == 1838, retrig_diff
assert cut_diff == 2757, cut_diff
"""
assert s.count(old) == 1, s.count(old)
s = s.replace(old, new, 1)

s = s.replace("print('WP4_9_GATE5_DISCOVERY_OK=1')", "print('WP4_9_GATE5_CANONICAL_OK=1')", 1)
s = s.replace("echo 'WP4_9_GATE5_HOST_MATRIX_DISCOVERY_OK=1'", "echo 'WP4_9_GATE5_HOST_MATRIX_CANONICAL_OK=1'", 1)
p.write_text(s)
