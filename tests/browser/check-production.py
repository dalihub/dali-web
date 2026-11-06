#!/usr/bin/env python3
"""Run a local production host with direct or SW isolation headers."""
import argparse
from collections import Counter
import functools
import hashlib
import http.server
import json
import os
from pathlib import Path
import shutil
import socket
import subprocess
import threading
import time
import urllib.request

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('mode', choices=['headers', 'sw'])
parser.add_argument('--pixels', action='store_true', help='Require two instrumented real frames')
parser.add_argument('--memory', action='store_true')
parser.add_argument('--failure', choices=['shader', 'reflection'])
parser.add_argument('--event-worker', action='store_true', help='Diagnostic C startup gate; no runtime shims')
parser.add_argument('--event-pthread', action='store_true', help='C ownership, hang/stop and recreation matrix')
args = parser.parse_args()
if args.event_pthread and (args.event_worker or args.pixels or args.memory or args.failure):
    parser.error('Event pthread is a separate matrix')
if args.event_worker and (args.pixels or args.memory or args.failure):
    parser.error('Event Worker startup is a separate diagnostic gate')
if args.failure and (args.pixels or args.memory):
    parser.error('Failure checks cannot require successful pixels/memory')
root = Path(__file__).resolve().parents[2]
logs = Path(os.environ['DALI_WEB_RUN_DIR']).resolve()
build = Path(os.environ['DALI_WEB_BUILD_DIR']).resolve()
logs.mkdir(parents=True, exist_ok=False)
profiles = logs / 'profiles'
profiles.mkdir()
assets = logs / 'assets'
fixture = assets / 'probe'
fixture.mkdir(parents=True)
for name in ['production.html', 'production.js', 'production-sw.js']:
    shutil.copy2(root / 'tests/browser' / name, fixture / name)
if args.event_worker:
    for name in ['event-worker.html', 'event-worker.js', 'event-worker-runtime.js']:
        shutil.copy2(root / 'tests/browser' / name, fixture / name)
if args.event_pthread:
    for name in ['event-pthread.html', 'event-pthread.js']:
        shutil.copy2(root / 'tests/browser' / name, fixture / name)
(fixture / 'runtime').symlink_to(build, target_is_directory=True)
(logs / 'input-hashes.json').write_text(json.dumps({
    str(path): hashlib.sha256(path.read_bytes()).hexdigest()
    for path in [*fixture.glob('*.*'), *build.glob('dali-web.*')]
    if path.is_file()
}, indent=2) + '\n')


class Handler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        if args.mode == 'headers':
            self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
            self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        self.send_header('Content-Security-Policy', "default-src 'none'; script-src 'self' 'unsafe-eval' blob:; connect-src 'self'; worker-src 'self' blob:; img-src 'none'; frame-src 'self'; form-action 'none'")
        super().end_headers()

    def log_message(self, fmt, *values):
        with (logs / 'http.log').open('a') as output:
            output.write(fmt % values + '\n')


with socket.socket() as temporary:
    temporary.bind(('127.0.0.1', 0))
    port = temporary.getsockname()[1]
endpoint = f'http://127.0.0.1:{port}'


def request(method, path, data=None):
    query = urllib.request.Request(endpoint + path,
        data=None if data is None else json.dumps(data).encode(), method=method,
        headers={'Content-Type': 'application/json'})
    with urllib.request.urlopen(query, timeout=40) as response:
        return json.load(response)['value']


server = http.server.ThreadingHTTPServer(('127.0.0.1', 0), functools.partial(Handler, directory=str(assets)))
threading.Thread(target=server.serve_forever, daemon=True).start()
session_id = None
result = None
try:
    with (logs / 'geckodriver.log').open('w') as output:
        driver = subprocess.Popen([os.environ.get('GECKODRIVER', 'geckodriver'), '--port', str(port), '--profile-root', str(profiles)], stdout=output, stderr=output)
        try:
            for attempt in range(100):
                try:
                    request('GET', '/status')
                    break
                except OSError:
                    if driver.poll() is not None:
                        raise RuntimeError('geckodriver exited')
                    time.sleep(0.1)
            session = request('POST', '/session', {'capabilities': {'alwaysMatch': {
                'browserName': 'firefox', 'moz:firefoxOptions': {'args': ['-headless'], 'prefs': {
                    'network.proxy.type': 1, 'network.proxy.http': '127.0.0.1', 'network.proxy.http_port': 9,
                    'network.proxy.ssl': '127.0.0.1', 'network.proxy.ssl_port': 9,
                    'network.proxy.no_proxies_on': 'localhost,127.0.0.1', 'network.proxy.failover_direct': False
                }}}}})
            session_id = session['sessionId']
            base = '/session/' + session_id
            (logs / 'browser-capabilities.json').write_text(json.dumps(session, indent=2))
            request('POST', base + '/timeouts', {'pageLoad': 30000, 'script': 30000})
            failure = {'shader': 1, 'reflection': 2}.get(args.failure, 0)
            trace = os.environ.get('DALI_WEB_GL_TRACE', '0')
            page = 'event-pthread.html' if args.event_pthread else ('event-worker.html' if args.event_worker else 'production.html')
            request('POST', base + '/url', {'url': f'http://127.0.0.1:{server.server_port}/probe/{page}?sw={int(args.mode == "sw")}&trace={trace}&failure={failure}'})
            for attempt in range(30):
                result = request('POST', base + '/execute/sync', {'script': 'return window.productionResult || null;', 'args': []})
                if result and result.get('done'):
                    break
                time.sleep(1)
            (logs / 'runtime-result.json').write_text(json.dumps(result, indent=2) + '\n')
            print(json.dumps(result), flush=True)
        finally:
            if session_id:
                try:
                    request('DELETE', '/session/' + session_id)
                except Exception:
                    pass
            driver.terminate()
            try:
                driver.wait(timeout=10)
            except subprocess.TimeoutExpired:
                driver.kill()
                driver.wait()
finally:
    server.shutdown()
    server.server_close()

passed = bool(result and result.get('done') and not result.get('errors')
    and result.get('isolation') and result.get('sharedMemory')
    and [event['state'] for event in result.get('events', [])] == ['ready', 'stopped'])
if args.mode == 'sw':
    passed = passed and result.get('controlled') and result.get('firstIsolation') == 'false'
if args.pixels or args.memory:
    frames = result.get('pixels', []) if result else []
    expected = [13, 26, 51, 255, 230, 77, 26, 255]
    passed = passed and len(frames) == 2 and all(
        frame['frame'] == index + 1 and frame['error'] == 0 and len(frame['pixels']) == 8
        and all(abs(actual - target) <= 1 for actual, target in zip(frame['pixels'], ([13, 26, 51, 255, 51, 153, 204, 255] if args.memory and index == 1 else expected)))
        for index, frame in enumerate(frames))
if args.memory:
    checks = result.get('memory', []) if result else []
    required = Counter({'callback-no-write': 1, 'callback-0': 1, 'callback-4': 1,
        'callback-16': 1, 'offset-unlock-false-unmap': 1, 'gpu-uniform-readback': 4,
        'cpu-uniform-gpu-query': 3, 'before-detach': 2, 'replacement-zero': 2,
        'recycle-zero': 2, 'grow-zero': 2, 'grow-upload': 2})
    passed = passed and bool(checks) and all(check['passed'] and check['error'] == 0
        and check['actual'] == check['expected'] for check in checks)
    for frame in [1, 2]:
        expected_names = required.copy()
        expected_names['core-initial-16' if frame == 1 else 'next-frame-preserved'] = 1
        passed = passed and Counter(check['name'] for check in checks
            if check['frame'] == frame and check['actual']) == expected_names
        names = {check['name'] for check in checks if check['frame'] == frame}
        passed = passed and {'detached-shadow-not-freed', 'last-reference-frees-shadow',
            'allocator-all-references-released', 'mapped-recycle-new-object',
            'unmapped-recycle-reuse', 'custom-allocator-new-object'} <= names
if args.failure:
    events = [event['state'] for event in result.get('events', [])] if result else []
    errors = result.get('errors', []) if result else []
    passed = bool(result and result.get('done') and result.get('isolation') and result.get('sharedMemory')
        and events == ['ready', 'failure', 'stopped']
        and result['events'][1]['reason'] == 'GL operation failed'
        and not result.get('pixels') and not result.get('memory')
        and not any(word in '\n'.join(errors).lower() for word in ['unreachable', 'out of bounds', 'runtimeerror', 'abort(', 'uncaught', 'typeerror', 'referenceerror', 'rangeerror']))
if args.event_pthread:
    runs = result.get('runs', []) if result else []
    passed = bool(result and result.get('done') and not result.get('errors')
        and result.get('isolation') and len(runs) == 4
        and [r['mode'] for r in runs] == [0, 1, 2, 0]
        and all(r.get('passed') for r in runs)
        and result.get('freshMemory') and result.get('staleMessageRejected'))
    if args.mode == 'sw':
        passed = passed and result.get('controlled') and result.get('firstIsolation') == 'false'
(logs / 'verdict.json').write_text(json.dumps({'passed': bool(passed), 'eventWorkerStartupOnly': args.event_worker, 'eventPthreadMatrix': args.event_pthread, 'pixelsRequired': args.pixels or args.memory, 'memoryRequired': args.memory, 'expectedFailure': args.failure, 'mode': args.mode}, indent=2) + '\n')
raise SystemExit(0 if passed else 1)
