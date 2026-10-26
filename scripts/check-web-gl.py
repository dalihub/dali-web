#!/usr/bin/env python3
"""CPU wrapper/capability checks, not production/GPU evidence."""
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
    if code: raise SystemExit(code)
wrap = ['glGetError', 'glGetIntegerv', 'glBlendEquation', 'glDrawArrays', 'emscripten_webgl_get_current_context',
        'emscripten_is_webgl_context_lost', 'emscripten_webgl_get_context_attributes']
execute('compile-link', [os.environ['EMXX'], '-std=c++17', '-pthread', '-DDEBUG_ENABLED',
    '-I' + str(core / 'generated/include'), '-I' + str(root / 'libs/dali-core'), '-I' + str(root / 'libs/dali-adaptor'),
    str(root / 'tests/unit/web-gl-capabilities.cpp'),
    *[str(root / 'libs/dali-adaptor/dali/internal/graphics/web' / (name + '.cpp')) for name in
      ['web-gl-implementation', 'web-gles-failure', 'web-gles-fence', 'web-gles-program-cache']],
    str(core / 'core/libdali2-core.a'), '-sABORTING_MALLOC=0', '-sPTHREAD_POOL_SIZE=1', '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1',
    '-sMIN_WEBGL_VERSION=2', '-sMAX_WEBGL_VERSION=2', '-sFULL_ES3=0', '-sASSERTIONS=1', '-sERROR_ON_UNDEFINED_SYMBOLS=1',
    *['-Wl,--wrap=' + name for name in wrap], '-o', str(build / 'web-gl-test.js')])
for mode in ['blend', 'map', 'binary', 'cache', 'error']:
    execute('node-' + mode, [os.environ['NODE'], str(build / 'web-gl-test.js'), mode])
