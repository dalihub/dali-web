#!/usr/bin/env python3
"""Check the native source closure and reject unsupported Web combinations."""
import json
import os
from pathlib import Path
import subprocess

root = Path(__file__).resolve().parents[1]
run = Path(os.environ['DALI_WEB_RUN_DIR']).resolve()
run.mkdir(parents=True, exist_ok=False)
cmake = os.environ['CMAKE']
records = []
graphics = root / 'libs/dali-adaptor/dali/internal/graphics'
baseline = 'a68e3f00f0c7752561137c179500b004b930285e'
# Read the original lists from Git; never checkout or modify preserved sources.
for name in ['file.list', 'gles-impl/file.list']:
    path = run / 'baseline' / name
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(subprocess.check_output(['git', 'show', baseline + ':libs/dali-adaptor/dali/internal/graphics/' + name], cwd=root))
original = (run / 'baseline/file.list').read_text().replace('INCLUDE( ${adaptor_graphics_dir}/gles-impl/file.list )', 'INCLUDE("' + str(run / 'baseline/gles-impl/file.list') + '")')
(run / 'baseline/file.list').write_text(original)
for backend in ['GLES', 'VULKAN', 'DYNAMIC']:
    for variant in ['baseline', 'current']:
        script = run / (backend + '-' + variant + '.cmake')
        source = run / 'baseline/file.list' if variant == 'baseline' else graphics / 'file.list'
        script.write_text('set(adaptor_graphics_dir "' + str(graphics) + '")\n'
                          'include("' + str(source) + '")\n'
                          'file(WRITE "' + str(script.with_suffix('.json.txt')) + '" "${adaptor_graphics_gles_src_files}\\n${adaptor_graphics_vulkan_src_files}\\n")\n')
        args = [cmake, '-DENABLE_GRAPHICS_BACKEND=' + backend, '-P', str(script)]
        result = subprocess.run(args, capture_output=True, text=True)
        records.append({'argv': args, 'exitCode': result.returncode})
        (script.with_suffix('.log')).write_text(result.stdout + result.stderr)
        assert result.returncode == 0
    before = (run / (backend + '-baseline.json.txt')).read_text().replace('egl-graphics-controller', 'gles-graphics-controller').splitlines()
    after = (run / (backend + '-current.json.txt')).read_text().splitlines()
    assert set(after[0].split(';')) == set(before[0].split(';')) | {str(graphics / 'gles/native-gles-context.cpp'), str(graphics / 'gles/native-gles-rendering.cpp'), str(graphics / 'gles/native-gles-sync.cpp'), str(graphics / 'gles/native-gles-sync-pool.cpp'), str(graphics / 'gles/native-gles-memory.cpp'), str(graphics / 'gles/native-gles-buffer.cpp'), str(graphics / 'gles/native-gles-program-cache.cpp')}
    assert after[1] == before[1], 'Vulkan closure changed'
# Validation runs before package discovery. All these combinations must fail.
for backend, environment, emscripten in [('VULKAN', 'WEB', True), ('DYNAMIC', 'WEB', True), ('GLES', 'WEB', False), ('GLES', 'BAD', True)]:
    args = [cmake, '-DENABLE_GRAPHICS_BACKEND=' + backend, '-DDALI_GLES_ENVIRONMENT=' + environment,
            '-DEMSCRIPTEN=' + ('ON' if emscripten else 'OFF'), '-P', str(graphics / 'gles-environment.cmake')]
    result = subprocess.run(args, capture_output=True, text=True)
    records.append({'argv': args, 'exitCode': result.returncode, 'output': result.stdout + result.stderr})
    assert result.returncode != 0
(run / 'commands.json').write_text(json.dumps(records, indent=2) + '\n')
print('Native GLES/VULKAN/DYNAMIC source sets preserved; invalid combinations rejected')
