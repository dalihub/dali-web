#!/usr/bin/env python3
"""Inspect the existing SW lifecycle; stop at a reproducible D4 failure."""
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
(assets / 'outside.html').write_text('<!doctype html><title>Outside SW scope</title>')
(fixture / 'runtime').symlink_to(build, target_is_directory=True)
(logs / 'input-hashes.json').write_text(json.dumps({
    str(path): hashlib.sha256(path.read_bytes()).hexdigest()
    for path in [*fixture.glob('*.*'), *build.glob('dali-web.*')]
    if path.is_file()
}, indent=2) + '\n')


class Handler(http.server.SimpleHTTPRequestHandler):
    fail_sw = False
    sw_variant = None
    page_requests = 0

    def do_GET(self):
        path = self.path.split('?')[0]
        if path == '/probe/production.html':
            Handler.page_requests += 1
        if path == '/probe/production-sw.js' and self.sw_variant:
            source = "self.addEventListener('install', e => e.waitUntil(self.skipWaiting()));"
            if self.sw_variant == 'no-headers':
                source += "self.addEventListener('activate', e => e.waitUntil(self.clients.claim()));"
            if self.sw_variant == 'delayed-control':
                source = (root / 'tests/browser/production-sw.js').read_text().replace(
                    'event.waitUntil(self.clients.claim())',
                    'event.waitUntil(new Promise(resolve => setTimeout(resolve, 6500)).then(() => self.clients.claim()))')
            payload = source.encode()
            self.send_response(200)
            self.send_header('Content-Type', 'text/javascript')
            self.send_header('Cache-Control', 'no-store')
            self.send_header('Content-Length', str(len(payload)))
            self.end_headers()
            self.wfile.write(payload)
            return
        if self.path.split('?')[0] == '/probe/production-sw.js' and self.fail_sw:
            self.send_error(503, 'Injected SW update failure')
            return
        if self.path == '/probe/disconnect':
            self.connection.shutdown(socket.SHUT_RDWR)
            self.connection.close()
            return
        super().do_GET()

    def end_headers(self):
        # Count real document requests rather than HTTP-cache hits in reload assertions.
        # This is a test-server policy, not a proposed Pages cache policy.
        if self.path.split('?')[0].endswith('.html'):
            self.send_header('Cache-Control', 'no-store')
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
            origin = f'http://127.0.0.1:{server.server_port}'
            url = origin + '/probe/production.html?sw=1'
            results = []

            def js(source):
                return request('POST', base + '/execute/sync', {'script': source, 'args': []})

            def async_js(source):
                return request('POST', base + '/execute/async', {'script':
                    'const done = arguments[arguments.length - 1]; (async () => {' + source +
                    '})().then(done, error => done({error: String(error)}));', 'args': []})

            def save(name, value):
                results.append({'name': name, 'value': value})
                (logs / 'runtime-result.json').write_text(json.dumps(results, indent=2) + '\n')
                print(name + ': ' + json.dumps(value), flush=True)
                return value

            def navigate(target):
                request('POST', base + '/url', {'url': target})

            def production(name):
                navigate(url)
                return wait_result(name)

            def wait_result(name):
                value = None
                for attempt in range(150):
                    value = js('return window.productionResult || null;')
                    if value and value.get('done'):
                        break
                    time.sleep(0.1)
                return save(name, value)

            def require_render(value):
                assert value and value.get('done') and not value['errors'], value
                assert value['isolation'] and value['controlled'] and value['sharedMemory']
                assert [event['state'] for event in value['events']] == ['ready', 'stopped']
                assert len(value['pixels']) == 2
                expected = [13, 26, 51, 255, 230, 77, 26, 255]
                assert all(frame['error'] == 0 and len(frame['pixels']) == 8 and
                    all(abs(a - b) <= 1 for a, b in zip(frame['pixels'], expected))
                    for frame in value['pixels'])

            first = production('first-visit')
            require_render(first)
            assert first['firstIsolation'] == 'false'
            navigate(origin + '/outside.html')
            require_render(production('revisit-same-tab'))
            save('cache-and-errors', async_js("""
                const keysBefore = await caches.keys();
                const missing = await fetch('./missing-resource.wasm');
                let disconnected;
                try { await fetch('./disconnect'); disconnected = 'unexpected-success'; }
                catch (error) { disconnected = String(error); }
                const response = await fetch('./runtime/dali-web.worker.js');
                return {keysBefore, keysAfter: await caches.keys(), missingStatus: missing.status,
                    disconnected, workerStatus: response.status,
                    workerMime: response.headers.get('Content-Type'),
                    cacheControl: response.headers.get('Cache-Control'),
                    coop: response.headers.get('Cross-Origin-Opener-Policy'),
                    coep: response.headers.get('Cross-Origin-Embedder-Policy')};
            """))
            # Append a comment only to the served copy to trigger a real byte update.
            sw_copy = fixture / 'production-sw.js'
            sw_copy.write_text(sw_copy.read_text() + '\n// Lifecycle test revision 2\n')
            save('update', async_js("""
                const registration = await navigator.serviceWorker.getRegistration();
                const previous = navigator.serviceWorker.controller;
                await registration.update();
                const start = performance.now();
                while (navigator.serviceWorker.controller === previous && performance.now() - start < 10000)
                    await new Promise(resolve => setTimeout(resolve, 50));
                return {changed: navigator.serviceWorker.controller !== previous,
                    state: navigator.serviceWorker.controller?.state, isolation: crossOriginIsolated,
                    updateViaCache: registration.updateViaCache};
            """))
            assert results[-1]['value'].get('changed'), results[-1]
            require_render(production('after-update'))
            Handler.fail_sw = True
            save('failed-update', async_js("""
                const registration = await navigator.serviceWorker.getRegistration();
                const previous = navigator.serviceWorker.controller;
                let error = null;
                try { await registration.update(); } catch (caught) { error = String(caught); }
                return {error, sameController: navigator.serviceWorker.controller === previous,
                    state: navigator.serviceWorker.controller?.state, isolation: crossOriginIsolated};
            """))
            Handler.fail_sw = False
            require_render(production('after-failed-update'))
            save('unregister', async_js("""
                const registration = await navigator.serviceWorker.getRegistration();
                const removed = await registration.unregister();
                return {removed, controlled: !!navigator.serviceWorker.controller,
                    isolation: crossOriginIsolated,
                    registrations: (await navigator.serviceWorker.getRegistrations()).length,
                    reloadFlag: sessionStorage.getItem('dali-sw:' + new URL('./production-sw.js', location.href).href + ':reloaded')};
            """))
            navigate(origin + '/outside.html')
            save('outside-after-unregister', js("return {controlled: !!navigator.serviceWorker.controller, isolation: crossOriginIsolated};"))
            require_render(production('revisit-after-unregister'))
            assert js("return sessionStorage.getItem('dali-sw:' + new URL('./production-sw.js', location.href).href + ':reloaded');") is None

            for mode in ['registration-503', 'no-control', 'delayed-control', 'no-headers']:
                async_js("const r = await navigator.serviceWorker.getRegistration(); return r ? r.unregister() : false;")
                navigate(origin + '/outside.html')
                Handler.fail_sw = mode == 'registration-503'
                Handler.sw_variant = mode if mode != 'registration-503' else None
                before = Handler.page_requests
                failed = production(mode)
                assert failed and failed['done'] and failed['errors'] and not failed['events'], failed
                if mode in ['no-control', 'delayed-control']:
                    assert failed['errors'] == ['Error: SW registration/control timed out']
                if mode == 'no-headers':
                    assert failed['errors'] == ['Error: SW reload did not isolate']
                count = Handler.page_requests
                save(mode + '-request-count', {'before': before, 'after': count})
                assert count - before == (2 if mode == 'no-headers' else 1)
                # A late ready/controller event must not trigger another reload after failure.
                time.sleep(2 if mode == 'delayed-control' else 1)
                if mode == 'delayed-control':
                    late = save('late-controller', js("return {controlled: !!navigator.serviceWorker.controller, done: window.productionResult.done};"))
                    assert late == {'controlled': True, 'done': True}
                assert Handler.page_requests == count
                ui = save(mode + '-ui', js("""return {
                    retryVisible: !document.querySelector('#isolation-retry').hidden,
                    errorVisible: !document.querySelector('#isolation-error').hidden,
                    message: document.querySelector('#isolation-error').textContent,
                    runtimeAbsent: !window.productionModule
                };"""))
                assert ui['retryVisible'] and ui['errorVisible'] and ui['runtimeAbsent']
                save(mode + '-navigation-count', {'automaticReloads': count - before - 1})
                Handler.fail_sw = False
                Handler.sw_variant = None
                # Remove the intentionally broken test registration, as an operator repair.
                async_js("const r = await navigator.serviceWorker.getRegistration(); return r ? r.unregister() : false;")
                js("document.querySelector('#isolation-retry').click(); return true;")
                # Wait for navigation to replace the failed document before observing completion.
                for attempt in range(100):
                    if Handler.page_requests > count:
                        break
                    time.sleep(0.05)
                require_render(wait_result(mode + '-retry'))
                assert js("return sessionStorage.getItem('dali-sw:' + new URL('./production-sw.js', location.href).href + ':reloaded');") is None

            verdict = {'passed': True, 'scope': 'local E bootstrap recovery and lifecycle subset',
                'recoveryModes': ['unregister-revisit', 'registration-503', 'no-control', 'delayed-control', 'no-headers'],
                'cachePolicyValidated': False}
            (logs / 'verdict.json').write_text(json.dumps(verdict, indent=2) + '\n')
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

raise SystemExit(0 if verdict['passed'] else 1)
