#!/usr/bin/env bash
set -euo pipefail
script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
python3 - "$script_dir/.." <<'PY'
import json
from pathlib import Path
import subprocess
import sys

root = Path(sys.argv[1]).resolve()
def git(*args):
    return subprocess.check_output(['git', '-C', str(root), *args], text=True).strip()

if Path(git('rev-parse', '--show-toplevel')).resolve() != root:
    sys.exit('Run this script from a clone of dali-web.')
lock = json.loads((root / 'upstreams.lock.json').read_text())
if lock['schemaVersion'] != 1:
    sys.exit('Unsupported upstream lock schema.')
existing = set(git('remote').splitlines())
# Validate every existing remote before making any changes.
for item in lock['upstreams']:
    remote, url = item['remote'], item['url']
    if remote in existing:
        fetch_urls = git('remote', 'get-url', '--all', remote).splitlines()
        push_urls = git('remote', 'get-url', '--push', '--all', remote).splitlines()
        if fetch_urls != [url] or push_urls != [url]:
            sys.exit(f'Remote URL mismatch: {remote}; expected {url}. No remotes changed.')
for item in lock['upstreams']:
    remote, url = item['remote'], item['url']
    if remote not in existing:
        git('remote', 'add', remote, url)
        print(f'Added {remote}: {url}')
    else:
        print(f'Kept {remote}: {url}')
PY
