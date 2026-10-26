#!/usr/bin/env python3
"""CPU integration only: production Controller/Buffer/Core with injected GL."""
import hashlib
import json
import os
from pathlib import Path
import shlex
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
inputs = set()
def execute(name, args):
    with (run / (name + '.log')).open('w') as output:
        code = subprocess.run(args, cwd=root, stdout=output, stderr=subprocess.STDOUT).returncode
    records.append({'name': name, 'argv': args, 'exitCode': code})
    (run / 'commands.json').write_text(json.dumps(records, indent=2) + '\n')
    print(name, code, flush=True)
    if code:
        raise SystemExit(code)

for key in ['EMXX', 'NODE', 'LLVM_NM']:
    execute('version-' + key, [os.environ[key], '--version'])
objects = []
# Recompile every selected production TU; the old build supplies Core and its
# recorded source selection, never old Adaptor objects.
for entry in json.loads((core / 'compile_commands.json').read_text()):
    if not any(name in entry['command'] for name in ['gles-boundary.dir', 'dali-web-renderer.dir']):
        continue
    source = Path(entry['file'])
    if source.name in ['gles-context-boundary.cpp', 'gl-implementation.cpp', 'gl-extensions.cpp', 'gl-extensions-support.cpp', 'web-gl-implementation.cpp', 'gles-web-boundary.cpp']:
        continue
    args = shlex.split(entry['command'])
    obj = build / (source.stem + '.o')
    args[args.index('-o') + 1] = str(obj)
    inputs.add(source)
    execute('compile-' + source.stem, args)
    execute('nm-' + source.stem, [os.environ['LLVM_NM'], '-u', '-C', str(obj)])
    objects.append(str(obj))
utils = root / 'libs/dali-adaptor/automated-tests/src/dali-adaptor/dali-test-suite-utils'
sources = [root / 'tests/unit/web-buffer-controller.cpp']
inputs.update(sources)
inputs.update((root / 'tests/unit').glob('web-*.h'))
(run / 'source-hashes.json').write_text(json.dumps({str(p): hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(inputs)}, indent=2) + '\n')
wrap = ['glGenBuffers', 'glDeleteBuffers', 'glBindBuffer', 'glBufferData', 'glBufferSubData', 'glGetError',
        'emscripten_webgl_get_current_context', 'emscripten_is_webgl_context_lost']
execute('compile-link-test', [os.environ['EMXX'], '-std=c++17', '-pthread', '-DDEBUG_ENABLED',
    '-I' + str(core / 'generated/include'), '-I' + str(root / 'libs/dali-core'),
    '-I' + str(root / 'libs/dali-adaptor'), '-I' + str(utils),
    *map(str, sources), *objects, str(core / 'core/libdali2-core.a'),
    '-sABORTING_MALLOC=0', '-sPTHREAD_POOL_SIZE=1', '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1',
    '-sMIN_WEBGL_VERSION=2', '-sMAX_WEBGL_VERSION=2', '-sASSERTIONS=1',
    '-sERROR_ON_UNDEFINED_SYMBOLS=1', '-Wl,-Map=' + str(run / 'test.map'),
    *['-Wl,--wrap=' + name for name in wrap], '-o', str(build / 'controller-test.js')])
for mode in ['factory', 'discard', 'factory-allocation', 'buffer-object', 'storage-object', 'memory-object',
             'gl-allocation', 'gl-data', 'uniform-cpu', 'uniform-gpu', 'shutdown', 'shutdown-allocator', 'invalid-allocator', 'mapped-recycle', 'recycle-failure', 'grow-failure', 'uniform-cpu-failure', 'uniform-gpu-failure']:
    execute('node-' + mode, [os.environ['NODE'], str(build / 'controller-test.js'), mode])
