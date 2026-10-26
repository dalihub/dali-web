#!/usr/bin/env python3
"""Native configure attempts and Emscripten syntax checks of extracted native code.
Cross-compilation is not a substitute for a host-native build or runtime test.
"""
import json
import os
from pathlib import Path
import subprocess

root = Path(__file__).resolve().parents[1]
run = Path(os.environ['DALI_WEB_RUN_DIR']).resolve()
build = Path(os.environ['DALI_WEB_BUILD_DIR']).resolve()
include = Path(os.environ['DALI_WEB_GENERATED_INCLUDE_DIR']).resolve()
if run.exists() or build.exists():
    raise SystemExit('Choose new run and build directories')
run.mkdir(parents=True)
build.mkdir(parents=True)
records = []

def execute(name, args):
    with (run / (name + '.log')).open('w') as log:
        code = subprocess.run(args, cwd=root, stdout=log, stderr=subprocess.STDOUT).returncode
    records.append({'name': name, 'argv': args, 'exitCode': code})
    (run / 'commands.json').write_text(json.dumps(records, indent=2) + '\n')
    print(name, code, flush=True)
    return code

for backend in ['GLES', 'VULKAN', 'DYNAMIC']:
    execute('native-configure-' + backend, [os.environ['CMAKE'], '-S', str(root / 'libs/dali-adaptor/build/tizen'),
        '-B', str(build / backend), '-G', 'Ninja', '-DCMAKE_MAKE_PROGRAM=' + os.environ['NINJA'],
        '-DENABLE_GRAPHICS_BACKEND=' + backend, '-DENABLE_PKG_CONFIGURE=OFF'])
for name in ['native-gles-context', 'native-gles-rendering', 'native-gles-sync', 'native-gles-sync-pool', 'egl-graphics', 'native-gles-memory', 'native-gles-buffer', 'gles-graphics-controller', 'gles-graphics-buffer', 'gles2-graphics-memory', 'gles3-graphics-memory', 'native-gles-program-cache', 'gles-graphics-program']:
    folder = 'gles-impl' if name.startswith(('gles-', 'gles2-', 'gles3-')) else 'gles'
    obj = build / (name + '.o')
    args = [os.environ['EMXX'], '-std=c++17', '-pthread', '-DDEBUG_ENABLED', '-I' + str(include),
            '-I' + str(root / 'libs/dali-core'), '-I' + str(root / 'libs/dali-adaptor'),
            '-c', str(root / 'libs/dali-adaptor/dali/internal/graphics' / folder / (name + '.cpp')), '-o', str(obj)]
    if not execute('cross-compile-' + name, args):
        execute('nm-' + name, [os.environ['LLVM_NM'], '-u', '-C', str(obj)])
