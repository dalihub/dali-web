#!/usr/bin/env bash
# Source this file; explicit overrides win over SDK/PATH discovery.
dali_web_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
if [[ -n ${DALI_WEB_HOST_TOOLS:-} ]]; then
  export PATH="$DALI_WEB_HOST_TOOLS/bin:$PATH"
  export LD_LIBRARY_PATH="$DALI_WEB_HOST_TOOLS/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
fi
if [[ -n ${DALI_WEB_EMSDK:-} ]]; then
  source "$DALI_WEB_EMSDK/emsdk_env.sh" || return
elif ! command -v emcmake >/dev/null && [[ -f "$dali_web_root/tools/emsdk/emsdk_env.sh" ]]; then
  source "$dali_web_root/tools/emsdk/emsdk_env.sh" || return
fi
export DALI_WEB_ROOT="$dali_web_root"
export DALI_WEB_BUILD_DIR=${DALI_WEB_BUILD_DIR:-$dali_web_root/build/core}
export DALI_WEB_RUN_DIR=${DALI_WEB_RUN_DIR:-$dali_web_root/logs/r2-a/$(date -u +%Y%m%dT%H%M%SZ)-$$}
export CMAKE=${CMAKE:-$(command -v cmake || true)}
export NINJA=${NINJA:-$(command -v ninja || true)}
export NODE=${NODE:-$(command -v node || true)}
export PYTHON=${PYTHON:-$(command -v python3 || true)}
export EMCC=${EMCC:-$(command -v emcc || true)}
export EMXX=${EMXX:-$(command -v em++ || true)}
export EMCMAKE=${EMCMAKE:-$(command -v emcmake || true)}
export LLVM_NM=${LLVM_NM:-$(command -v llvm-nm || true)}
if [[ -z $LLVM_NM && -n $EMCC ]]; then
  export LLVM_NM=$(cd "$(dirname "$EMCC")/../bin" && pwd)/llvm-nm
fi
