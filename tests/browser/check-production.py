#!/usr/bin/env python3
"""Run a local production host with direct or SW isolation headers."""
import argparse
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
args = parser.parse_args()
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
            trace = os.environ.get('DALI_WEB_GL_TRACE', '0')
            request('POST', base + '/url', {'url': f'http://127.0.0.1:{server.server_port}/probe/production.html?sw={int(args.mode == "sw")}&trace={trace}'})
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
if args.pixels:
    frames = result.get('pixels', []) if result else []
    expected = [13, 26, 51, 255, 230, 77, 26, 255]
    passed = passed and len(frames) == 2 and all(
        frame['frame'] == index + 1 and frame['error'] == 0 and len(frame['pixels']) == 8
        and all(abs(actual - target) <= 1 for actual, target in zip(frame['pixels'], expected))
        for index, frame in enumerate(frames))
(logs / 'verdict.json').write_text(json.dumps({'passed': bool(passed), 'pixelsRequired': args.pixels, 'mode': args.mode}, indent=2) + '\n')
raise SystemExit(0 if passed else 1)
