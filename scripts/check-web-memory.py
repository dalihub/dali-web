#!/usr/bin/env python3
"""Test production Web memory/storage with injected GL; not GPU evidence."""
import json
import os
from pathlib import Path
import subprocess

root = Path(__file__).resolve().parents[1]
run = Path(os.environ['DALI_WEB_RUN_DIR']).resolve()
build = Path(os.environ['DALI_WEB_BUILD_DIR']).resolve()
core = Path(os.environ['DALI_WEB_CORE_BUILD_DIR']).resolve()
if run.exists() or build.exists():
    raise SystemExit('Choose new run and build directories')
run.mkdir(parents=True)
build.mkdir(parents=True)
records = []

def execute(name, args):
    with (run / (name + '.log')).open('w') as output:
        code = subprocess.run(args, cwd=root, stdout=output, stderr=subprocess.STDOUT).returncode
    records.append({'name': name, 'argv': args, 'exitCode': code})
    (run / 'commands.json').write_text(json.dumps(records, indent=2) + '\n')
    print(name, code, flush=True)
    if code:
        raise SystemExit(code)

wrap = ['glBindBuffer', 'glBufferSubData', 'glGetError', 'glFenceSync', 'glDeleteSync', 'glClientWaitSync', 'glFlush',
        'emscripten_webgl_get_current_context', 'emscripten_is_webgl_context_lost']
execute('compile-link', [os.environ['EMXX'], '-std=c++17', '-pthread', '-DDEBUG_ENABLED',
    '-I' + str(core / 'generated/include'), '-I' + str(root / 'libs/dali-core'),
    '-I' + str(root / 'libs/dali-adaptor'),
    str(root / 'tests/unit/web-buffer-memory.cpp'),
    str(root / 'libs/dali-adaptor/dali/internal/graphics/web/web-gles-fence.cpp'),
    *[str(root / 'libs/dali-adaptor/dali/internal/graphics/web' / (name + '.cpp')) for name in ['web-buffer-memory', 'web-buffer-storage', 'web-gles-failure']],
    str(core / 'core/libdali2-core.a'), '-sABORTING_MALLOC=0', '-sPTHREAD_POOL_SIZE=1', '-sENVIRONMENT=node',
    '-sMIN_WEBGL_VERSION=2', '-sMAX_WEBGL_VERSION=2', '-sASSERTIONS=1',
    '-sERROR_ON_UNDEFINED_SYMBOLS=1', *['-Wl,--wrap=' + name for name in wrap],
    '-o', str(build / 'web-memory-unit.js')])
for mode in ['preserve', 'cpu', 'bounds', 'overflow', 'map-bounds', 'duplicate-map', 'generations', 'allocation', 'upload', 'context', 'fence', 'stop']:
    execute('node-' + mode, [os.environ['NODE'], str(build / 'web-memory-unit.js'), mode])
