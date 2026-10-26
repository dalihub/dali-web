#!/usr/bin/env python3
"""Run real Core with test-only failure injection, not a production/GPU link."""
import hashlib
import json
import os
from pathlib import Path
import subprocess

root = Path(__file__).resolve().parents[1]
build = Path(os.environ['DALI_WEB_BUILD_DIR']).resolve()
run = Path(os.environ['DALI_WEB_RUN_DIR']).resolve()
if build.exists() or run.exists():
    raise SystemExit('Choose new build and run directories')
build.mkdir(parents=True)
run.mkdir(parents=True)
records = []


def execute(name, args):
    with (run / (name + '.log')).open('w') as output:
        code = subprocess.run(args, cwd=root, stdout=output, stderr=subprocess.STDOUT).returncode
    records.append({'name': name, 'argv': args, 'cwd': str(root), 'exitCode': code})
    (run / 'commands.json').write_text(json.dumps(records, indent=2) + '\n')
    print(name, code, flush=True)
    if code:
        raise SystemExit(code)


for key in ['EMXX', 'CMAKE', 'NINJA', 'NODE', 'PYTHON']:
    execute('version-' + key, [os.environ[key], '--version'])
jobs = int(os.environ.get('DALI_WEB_JOBS', '6'))
if not 1 <= jobs <= 6:
    raise SystemExit('DALI_WEB_JOBS must be 1..6')
files = list((root / 'libs/dali-core/dali/internal/render').rglob('*'))
files += [root / 'tests/unit/core-buffer-failure.cpp', Path(__file__).resolve(), root / 'cmake/p2-b-core-tests/CMakeLists.txt']
(run / 'source-hashes.json').write_text(json.dumps({str(p.relative_to(root)): hashlib.sha256(p.read_bytes()).hexdigest() for p in files if p.is_file()}, indent=2) + '\n')
execute('configure', [os.environ['EMCMAKE'], os.environ['CMAKE'], '-S', str(root / 'cmake/p2-b-core-tests'), '-B', str(build), '-G', 'Ninja', '-DCMAKE_BUILD_TYPE=Debug', '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON', '-DCMAKE_MAKE_PROGRAM=' + os.environ['NINJA']])
execute('build', [os.environ['CMAKE'], '--build', str(build), '--target', 'core-buffer-failure', 'core-smoke', '-j', str(jobs)])
execute('node-failure', [os.environ['NODE'], str(build / 'core-buffer-failure.js')])
execute('node-smoke', [os.environ['NODE'], str(build / 'probes/core-smoke.js')])
