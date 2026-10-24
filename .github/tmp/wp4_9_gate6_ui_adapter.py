from pathlib import Path
p = Path('tests/run_wp4_9_inherited_regressions.sh')
s = p.read_text()
old = '''run lock_editor_context sh tests/run_lock_editor_context_host.sh
grep -F 'WP3_3_LOCK_EDITOR_CONTEXT_HOST_OK=1' build/wp4_9_gate6_lock_editor_context.log
run lock_delete_context sh tests/run_lock_delete_context_host.sh
grep -F 'WP3_4_LOCK_DELETE_CONTEXT_HOST_OK=1' build/wp4_9_gate6_lock_delete_context.log
run visual sh tests/run_wp3_9_visual_host.sh
grep -F 'WP3_9_REVIEWED_SCREEN_HASHES_OK=1' build/wp4_9_gate6_visual.log
grep -F 'WP3_9_HOME_RETURN_PIXEL_IDENTICAL=1' build/wp4_9_gate6_visual.log
'''
new = '''run ui_context sh tests/run_wp4_9_ui_regressions_host.sh
grep -F 'WP4_9_UI_HOME_RETURN_PIXEL_IDENTICAL=1' build/wp4_9_gate6_ui_context.log
grep -F 'WP4_9_UI_CONTEXT_REGRESSIONS_OK=1 lock_editor_midi=0 lock_delete_midi=0 bounded_visual_frames=1' build/wp4_9_gate6_ui_context.log
'''
assert s.count(old) == 1, s.count(old)
p.write_text(s.replace(old, new, 1))
