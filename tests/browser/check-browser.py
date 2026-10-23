"""Run the core execution probe in a disposable headless Firefox session."""
import functools
import http.server
import json
import os
import pathlib
import socket
import subprocess
import threading
import time
import urllib.request

root = pathlib.Path(__file__).resolve().parents[2]
build = pathlib.Path(os.environ.get("DALI_WEB_BUILD_DIR", root / "build/core"))
logs = pathlib.Path(os.environ["DALI_WEB_RUN_DIR"])
logs.mkdir(parents=True, exist_ok=True)
profiles = logs / "browser-profiles"
profiles.mkdir(parents=True, exist_ok=True)
handler = functools.partial(http.server.SimpleHTTPRequestHandler, directory=str(build))
server = http.server.ThreadingHTTPServer(("127.0.0.1", 0), handler)
threading.Thread(target=server.serve_forever, daemon=True).start()
with socket.socket() as temporary:
    temporary.bind(("127.0.0.1", 0))
    driver_port = temporary.getsockname()[1]
endpoint = f"http://127.0.0.1:{driver_port}"


def request(method, path, body=None):
    data = None if body is None else json.dumps(body).encode()
    query = urllib.request.Request(endpoint + path, data=data, method=method,
                                   headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(query, timeout=60) as response:
        return json.load(response)["value"]


session = None
with (logs / "geckodriver.log").open("w") as log:
    driver = subprocess.Popen([os.environ.get("GECKODRIVER", "geckodriver"), "--port", str(driver_port),
                               "--profile-root", str(profiles)], stdout=log, stderr=log)
    try:
        for attempt in range(100):
            try:
                request("GET", "/status")
                break
            except OSError:
                if driver.poll() is not None:
                    raise RuntimeError("geckodriver exited; see logs/geckodriver.log")
                time.sleep(0.1)
        session = request("POST", "/session", {"capabilities": {"alwaysMatch": {
            "browserName": "firefox", "moz:firefoxOptions": {"args": ["-headless"]}
        }}})["sessionId"]
        path = f"/session/{session}"
        request("POST", path + "/url", {"url": f"http://127.0.0.1:{server.server_port}/core-smoke.html"})
        result = None
        for attempt in range(120):
            result = request("POST", path + "/execute/sync", {
                "script": "return window.probeResult || null;", "args": []})
            if result and result["status"] != "running":
                break
            time.sleep(0.5)
        (logs / "browser-result.json").write_text(json.dumps(result, indent=2) + "\n")
        print(json.dumps(result, indent=2))
        if not result or result["status"] != "passed":
            raise RuntimeError("Browser core probe failed or timed out")
    finally:
        if session:
            try:
                request("DELETE", f"/session/{session}")
            except Exception:
                pass
        driver.terminate()
        try:
            driver.wait(timeout=10)
        except subprocess.TimeoutExpired:
            driver.kill()
            driver.wait()
        server.shutdown()
        server.server_close()
