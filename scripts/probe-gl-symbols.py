"""Link-only inventory of GL/EGL imports; generated declarations are not runnable APIs."""
import json
import os
import shutil
import pathlib
import re
import shlex
import subprocess

root = pathlib.Path(__file__).resolve().parents[1]
build = pathlib.Path(os.environ.get("DALI_WEB_BUILD_DIR", root / "build/core"))
logs = pathlib.Path(os.environ["DALI_WEB_RUN_DIR"])
logs.mkdir(parents=True, exist_ok=True)
nm = os.environ.get("LLVM_NM") or shutil.which("llvm-nm")
compiler = os.environ.get("EMXX") or shutil.which("em++")
if not nm or not compiler:
    raise SystemExit("Source scripts/env.sh and set LLVM_NM/EMXX if needed")
symbols = set()
for filename in ("gl-implementation.o", "egl-implementation.o"):
    output = subprocess.check_output([str(nm), "--undefined-only", str(build / "adaptor" / filename)], text=True)
    symbols.update(re.findall(r"\bU ((?:egl|gl)[A-Z][A-Za-z0-9_]*)$", output, flags=re.MULTILINE))
symbols = sorted(symbols)
source = build / "gl-symbols.cpp"
source.write_text(
    '// Link-only symbol references. Do not execute these placeholder signatures.\n' +
    '\n'.join(f'extern "C" void {symbol}();' for symbol in symbols) +
    '\nusing Entry = void (*)();\nEntry volatile entries[] = {\n' +
    ',\n'.join(symbols) + '\n};\nint main() { return entries[0] == nullptr; }\n'
)
results = {}
for name, extra in (("webgl2", []), ("webgl2-full-es3", ["-sFULL_ES3=1"])):
    command = [str(compiler), str(source), "-sMIN_WEBGL_VERSION=2", "-sMAX_WEBGL_VERSION=2",
               *extra, "-o", str(build / f"gl-symbols-{name}.js")]
    with (logs / "gl-commands.log").open("a") as log:
        log.write(shlex.join(command) + "\n")
    run = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    (logs / f"gl-symbols-{name}.log").write_text(run.stdout)
    with (logs / "gl-commands.log").open("a") as log:
        log.write(f"exitCode={run.returncode} variant={name}\n")
    missing = sorted(set(re.findall(r"undefined symbol: (\w+)", run.stdout)))
    results[name] = {"exitCode": run.returncode, "missingSymbols": missing, "symbolCount": len(symbols)}
results["scope"] = "Direct GL/EGL imports from two compiled translation units only; not full adaptor linkage or runtime support."
(logs / "gl-symbols.json").write_text(json.dumps(results, indent=2) + "\n")
print(json.dumps(results, indent=2))
