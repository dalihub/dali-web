#!/usr/bin/env python3
"""Check real source-list selection and isolated native generated paths."""
import json
import os
from pathlib import Path
import subprocess
root = Path(__file__).resolve().parents[1]
run = Path(os.environ['DALI_WEB_RUN_DIR']).resolve()
run.mkdir(parents=True, exist_ok=False)
graphics = root / 'libs/dali-adaptor/dali/internal/graphics'
adaptor = root / 'libs/dali-adaptor/dali/internal/adaptor'
records = []
def execute(name, body):
    script = run / (name + '.cmake')
    script.write_text(body)
    args = [os.environ['CMAKE'], '-P', str(script)]
    result = subprocess.run(args, capture_output=True, text=True)
    records.append({'name': name, 'argv': args, 'exitCode': result.returncode})
    (run / 'commands.json').write_text(json.dumps(records, indent=2) + '\n')
    (run / (name + '.log')).write_text(result.stdout + result.stderr)
    assert result.returncode == 0, result.stderr
execute('web', f'''set(EMSCRIPTEN ON)
set(DALI_GLES_ENVIRONMENT WEB CACHE STRING "" FORCE)
set(ENABLE_GRAPHICS_BACKEND GLES)
set(adaptor_graphics_dir "{graphics}")
include("{graphics}/file.list")
file(WRITE "{run}/web-sources.txt" "${{adaptor_graphics_gles_src_files}}")
''')
sources = (run / 'web-sources.txt').read_text().split(';')
assert len(sources) == len(set(sources))
assert all(Path(p).exists() for p in sources)
assert all('/gles/' not in p and 'gles2-graphics-memory' not in p and 'gles3-graphics-memory' not in p for p in sources)
assert any(p.endswith('web-gl-implementation.cpp') for p in sources)
assert any(p.endswith('web-gles-program-cache.cpp') for p in sources)
# Exercise file.list consumers with two binary trees. Full native configure
# remains a separate check and may fail before reaching generation.
for variant in ['one', 'two']:
    output = run / variant
    output.mkdir()
    execute('native-generated-' + variant, f'''set(DALI_GLES_ENVIRONMENT NATIVE_EGL CACHE STRING "" FORCE)
set(adaptor_graphics_dir "{graphics}")
set(adaptor_adaptor_dir "{adaptor}")
set(DALI_SHADER_CACHE_SOURCE "{output}/shader-cache-path.cpp")
set(DALI_SYSTEM_CACHE_SOURCE "{output}/system-cache-path.cpp")
include("{graphics}/file.list")
include("{adaptor}/file.list")
set(cachePath "/{variant}")
configure_file("{root}/libs/dali-adaptor/build/tizen/shader-cache-path.in" "${{DALI_SHADER_CACHE_SOURCE}}" @ONLY)
configure_file("{root}/libs/dali-adaptor/build/tizen/system-cache-path.in" "${{DALI_SYSTEM_CACHE_SOURCE}}" @ONLY)
file(WRITE "{output}/sources.txt" "${{adaptor_graphics_gles_src_files}}\\n${{adaptor_graphics_vulkan_src_files}}\\n${{adaptor_adaptor_common_src_files}}")
''')
    text = (output / 'sources.txt').read_text()
    assert str(output / 'shader-cache-path.cpp') in text
    assert str(graphics / 'common/shader-cache-path.cpp') not in text
    assert (output / 'system-cache-path.cpp').exists()
assert (run / 'one/shader-cache-path.cpp').read_text() != (run / 'two/shader-cache-path.cpp').read_text()
print('PASS Web closure and independent native generated cache sources')
