#!/usr/bin/env python3
"""Collect strict production-object diagnostics; exit 0 means collection only."""
import json
import os
from pathlib import Path
import shlex
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
if execute('configure', [os.environ['EMCMAKE'], os.environ['CMAKE'], '-S', str(root / 'cmake/p2-a'), '-B', str(build), '-G', 'Ninja', '-DCMAKE_BUILD_TYPE=Debug', '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON', '-DCMAKE_MAKE_PROGRAM=' + os.environ['NINJA']]):
    raise SystemExit(1)
jobs = int(os.environ.get('DALI_WEB_JOBS', '6'))
if not 1 <= jobs <= 6:
    raise SystemExit('DALI_WEB_JOBS must be 1..6')
if execute('core-build', [os.environ['CMAKE'], '--build', str(build), '--target', 'dali2-core', '-j', str(jobs)]):
    raise SystemExit(1)
objects = []
failed = []
for entry in json.loads((build / 'compile_commands.json').read_text()):
    if 'gles-diagnostic.dir' not in entry['command']:
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
# Direct objects prevent unused-archive success; KEEPALIVE roots the real Core
# construction and the production Controller/GL wrapper vtables.
for variant, extra in [('webgl2', []), ('full-es3', ['-sFULL_ES3=1'])]:
    execute('link-' + variant, [os.environ['EMXX'], '-pthread', '--no-entry', *objects, archive,
        '-sMIN_WEBGL_VERSION=2', '-sMAX_WEBGL_VERSION=2', '-sASSERTIONS=1',
        '-sERROR_ON_UNDEFINED_SYMBOLS=1', '-Wl,--error-limit=0',
        '-Wl,-Map=' + str(run / (variant + '.map')), *extra,
        '-o', str(build / (variant + '.js'))])
(run / 'collection.json').write_text(json.dumps({'compileFailures': failed, 'objects': objects,
    'productionLinkComplete': False, 'runtimeTested': False}, indent=2) + '\n')
