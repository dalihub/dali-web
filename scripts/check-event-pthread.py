#!/usr/bin/env python3
"""Build C probe from verified 3.1.24 production objects, without modifying E."""
import hashlib
import json
import os
from pathlib import Path
import subprocess

root = Path(__file__).resolve().parents[1]
build = Path(os.environ['DALI_WEB_BUILD_DIR']).resolve()
run = Path(os.environ['DALI_WEB_RUN_DIR']).resolve()
prior = Path(os.environ['DALI_WEB_PRODUCTION_COMMANDS']).resolve()
sources = Path(os.environ['DALI_WEB_PRODUCTION_HASHES']).resolve()
if build.exists() or run.exists():
    raise SystemExit('Choose new build and log paths')
# Reuse only unchanged renderer/Core inputs; host is deliberately rebuilt.
for name, expected in json.loads(sources.read_text()).items():
    path = Path(name)
    if path in {root / 'web/host/web-host.cpp', root / 'cmake/web/CMakeLists.txt'}:
        continue
    if hashlib.sha256(path.read_bytes()).hexdigest() != expected:
        raise SystemExit('Production input drift: ' + name)
commands = json.loads(prior.read_text())
compile_args = next(c['argv'] for c in commands if c['name'] == 'compile-host-no-probe').copy()
link_args = next(c['argv'] for c in commands if c['name'] == 'link-no-probe').copy()
build.mkdir(parents=True)
run.mkdir(parents=True)
records = []
def execute(name, argv):
    with (run / (name + '.log')).open('w') as output:
        code = subprocess.run(argv, stdout=output, stderr=subprocess.STDOUT).returncode
    records.append({'name': name, 'argv': argv, 'exitCode': code})
    (run / 'commands.json').write_text(json.dumps(records, indent=2) + '\n')
    print(name, code, flush=True)
    if code:
        raise SystemExit(code)
compile_args[0] = os.environ['EMXX']
compile_args[compile_args.index('-o') + 1] = str(build / 'event-pthread.o')
compile_args[-1] = str(root / 'tests/probes/event-pthread.cpp')
link_args[0] = os.environ['EMXX']
for index, arg in enumerate(link_args):
    if arg.endswith('/web-host.o'): link_args[index] = str(build / 'event-pthread.o')
    if arg == '-sPTHREAD_POOL_SIZE=1': link_args[index] = '-sPTHREAD_POOL_SIZE=2'
    if arg.startswith('-Wl,-Map='): link_args[index] = '-Wl,-Map=' + str(run / 'webgl2.map')
link_args[link_args.index('-o') + 1] = str(build / 'dali-web.js')
inputs = {Path(arg) for arg in link_args if arg.endswith(('.o', '.a')) and Path(arg).exists()}
inputs.update([root / 'web/host/web-host.cpp', root / 'web/host/web-event-probe.h', root / 'tests/probes/event-pthread.cpp', root / 'scripts/check-event-pthread.py', sources, prior, root / 'toolchain.emception-3.1.24.lock.json'])
(run / 'inputs.json').write_text(json.dumps({str(p): hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(inputs)}, indent=2) + '\n')
execute('version', [os.environ['EMXX'], '--version'])
execute('compile-c-host', compile_args)
execute('nm-c-host', [os.environ['LLVM_NM'], '-u', '-C', str(build / 'event-pthread.o')])
execute('link-c', link_args)
