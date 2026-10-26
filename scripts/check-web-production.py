#!/usr/bin/env python3
"""Build the production WebGL2 host and record each command exit status."""
import json
import hashlib
import os
from pathlib import Path
import shlex
import shutil
import subprocess

root = Path(__file__).resolve().parents[1]
build = Path(os.environ['DALI_WEB_BUILD_DIR']).resolve()
run = Path(os.environ['DALI_WEB_RUN_DIR']).resolve()
# Never reuse outputs: failed runs must remain inspectable.
if build.exists() or run.exists():
    raise SystemExit('Choose new build AND run directories')
build.mkdir(parents=True)
run.mkdir(parents=True)
records = []

def execute(name, args, cwd=root):
    with (run / (name + '.log')).open('w') as log:
        code = subprocess.run(args, cwd=cwd, stdout=log, stderr=subprocess.STDOUT).returncode
    records.append({'name': name, 'argv': args, 'cwd': str(cwd), 'exitCode': code})
    (run / 'commands.json').write_text(json.dumps(records, indent=2) + '\n')
    print(name, code, flush=True)
    return code

for key, flag in [('EMXX', '--version'), ('LLVM_NM', '--version'), ('CMAKE', '--version'), ('NINJA', '--version'), ('NODE', '--version'), ('PYTHON', '--version')]:
    execute('version-' + key, [os.environ[key], flag])
if execute('configure', [os.environ['EMCMAKE'], os.environ['CMAKE'], '-S', str(root / 'cmake/web'), '-B', str(build), '-G', 'Ninja', '-DDALI_GLES_ENVIRONMENT=' + os.environ.get('DALI_GLES_ENVIRONMENT', 'WEB'), '-DCMAKE_BUILD_TYPE=Debug', '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON', '-DCMAKE_MAKE_PROGRAM=' + os.environ['NINJA']]):
    raise SystemExit(1)
jobs = int(os.environ.get('DALI_WEB_JOBS', '6'))
if not 1 <= jobs <= 6:
    raise SystemExit('DALI_WEB_JOBS must be 1..6')
if execute('core-build', [os.environ['CMAKE'], '--build', str(build), '--target', 'dali2-core', '-j', str(jobs)]):
    raise SystemExit(1)
inputs = {Path(entry['file']) for entry in json.loads((build / 'compile_commands.json').read_text())}
inputs.update(root / path for path in ['cmake/gles.cmake', 'cmake/web/CMakeLists.txt', 'upstreams.lock.json', 'toolchain.lock.json'])
if os.environ.get('DALI_WEB_TOOLCHAIN_LOCK'):
    inputs.add(Path(os.environ['DALI_WEB_TOOLCHAIN_LOCK']).resolve())
inputs.update((root / 'libs/dali-adaptor/dali/internal/graphics').rglob('*.h'))
inputs.update((root / 'web/host').glob('*.h'))
inputs.update(root / path for path in ['web/index.html', 'web/bootstrap.js', 'libs/dali-adaptor/dali/internal/graphics/file.list', 'libs/dali-adaptor/dali/internal/graphics/gles-impl/file.list', 'libs/dali-adaptor/dali/internal/graphics/gles-environment.cmake'])
(run / 'source-hashes.json').write_text(json.dumps({str(path): hashlib.sha256(path.read_bytes()).hexdigest() for path in sorted(inputs)}, indent=2) + '\n')
objects = []
failed = []
for entry in json.loads((build / 'compile_commands.json').read_text()):
    if not any(name in entry['command'] for name in ['dali-web-renderer.dir', 'dali-web.dir']):
        continue
    args = shlex.split(entry['command'])
    obj = Path(entry['directory']) / args[args.index('-o') + 1]
    obj.parent.mkdir(parents=True, exist_ok=True)
    name = Path(entry['file']).stem
    if execute('compile-' + name, args, Path(entry['directory'])):
        failed.append(entry['file'])
    else:
        objects.append(str(obj))
        execute('nm-' + name, [os.environ['LLVM_NM'], '-u', '-C', str(obj)])
archive = str(build / 'core/libdali2-core.a')
execute('nm-core', [os.environ['LLVM_NM'], '-u', '-C', archive])
# Direct objects retain the actual Controller/resource/command and host paths.
code = execute('link-webgl2', [os.environ['EMXX'], '-pthread', '--no-entry', *objects, archive,
    '-sMIN_WEBGL_VERSION=2', '-sMAX_WEBGL_VERSION=2', '-sFULL_ES3=0', '-sASSERTIONS=1',
    '-sERROR_ON_UNDEFINED_SYMBOLS=1', '-sABORTING_MALLOC=0', '-Wl,--error-limit=0',
    '-sPTHREAD_POOL_SIZE=1', '-sOFFSCREENCANVAS_SUPPORT=1', '-sALLOW_BLOCKING_ON_MAIN_THREAD=0',
    '-sMODULARIZE=1', '-sEXPORT_NAME=createDaliWeb', '-sENVIRONMENT=web,worker',
    '-Wl,-Map=' + str(run / 'webgl2.map'), '-o', str(build / 'dali-web.js')])
(run / 'collection.json').write_text(json.dumps({'compileFailures': failed, 'objects': objects,
    'productionLinkComplete': code == 0 and not failed, 'runtimeTested': False}, indent=2) + '\n')
if code or failed:
    raise SystemExit(1)

for name in ['index.html', 'bootstrap.js']:
    shutil.copy2(root / 'web' / name, build / name)
