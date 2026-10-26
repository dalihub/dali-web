#!/usr/bin/env bash
# Source explicitly. This selects an existing SDK; it never installs or rewrites it.
dali_web_profile_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
dali_web_sdk=${DALI_WEB_EMSDK_3124:-$dali_web_profile_root/tools/emsdk-3.1.24}
if [[ ! -f $dali_web_sdk/upstream/emscripten/emcc ]]; then
  echo 'Set DALI_WEB_EMSDK_3124 to an existing 3.1.24 SDK root' >&2
  return 1
fi
export EM_CONFIG=${EM_CONFIG:-$dali_web_sdk/.emscripten}
export EM_CACHE=${EM_CACHE:-$dali_web_sdk/cache}
if [[ ! -f $EM_CONFIG || ! -d $EM_CACHE ]]; then
  echo 'Prepare EM_CONFIG and EM_CACHE before selecting the 3.1.24 profile' >&2
  return 1
fi
export DALI_WEB_EMSDK=""
export EMCC="$dali_web_sdk/upstream/emscripten/emcc"
export EMXX="$dali_web_sdk/upstream/emscripten/em++"
export EMCMAKE="$dali_web_sdk/upstream/emscripten/emcmake"
export LLVM_NM="$dali_web_sdk/upstream/bin/llvm-nm"
export PATH="$dali_web_sdk/upstream/emscripten:$PATH"
export EMCC_CORES=6
export DALI_WEB_JOBS=${DALI_WEB_JOBS:-6}
export DALI_WEB_TOOLCHAIN_LOCK="$dali_web_profile_root/toolchain.emception-3.1.24.lock.json"
source "$dali_web_profile_root/scripts/env.sh" || return
dali_web_compiler_version=$("$EMCC" --version) || return
if [[ $dali_web_compiler_version != *"3.1.24"* || $dali_web_compiler_version != *"68a9f990429e0bcfb63b1cde68bad792554350a5"* ]]; then
  echo 'Compiler does not match toolchain.emception-3.1.24.lock.json' >&2
  return 1
fi
