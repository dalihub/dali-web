#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "$0")/env.sh"
jobs=${DALI_WEB_JOBS:-6}
if [[ ! $jobs =~ ^[1-6]$ ]]; then
  echo 'DALI_WEB_JOBS must be between 1 and 6' >&2
  exit 2
fi
for tool in CMAKE NINJA NODE PYTHON EMCC EMXX EMCMAKE; do
  if [[ -z ${!tool} || ! -x ${!tool} ]]; then
    echo "Missing $tool; see docs/BUILDING.md" >&2
    exit 2
  fi
done
mkdir -p "$DALI_WEB_RUN_DIR"
# Refuse to overwrite evidence from a previous invocation.
(set -o noclobber; : > "$DALI_WEB_RUN_DIR/commands.log") || exit 2
run_step()
{
  local name=$1
  shift
  printf '%q ' "$@" >> "$DALI_WEB_RUN_DIR/commands.log"
  printf '\n' >> "$DALI_WEB_RUN_DIR/commands.log"
  set +e
  "$@" 2>&1 | tee "$DALI_WEB_RUN_DIR/$name.log"
  local codes=("${PIPESTATUS[@]}")
  set -e
  printf '%s exit=%s tee=%s\n' "$name" "${codes[0]}" "${codes[1]}" | tee -a "$DALI_WEB_RUN_DIR/commands.log"
  [[ ${codes[0]} == 0 && ${codes[1]} == 0 ]]
}
run_step emcc-version "$EMCC" --version
run_step node-version "$NODE" --version
run_step cmake-version "$CMAKE" --version
run_step ninja-version "$NINJA" --version
run_step python-version "$PYTHON" --version
run_step configure "$EMCMAKE" "$CMAKE" -S "$DALI_WEB_ROOT/cmake" -B "$DALI_WEB_BUILD_DIR" -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX="$DALI_WEB_BUILD_DIR/install" \
  -DCMAKE_MAKE_PROGRAM="$NINJA" -DCMAKE_C_COMPILER="$EMCC" -DCMAKE_CXX_COMPILER="$EMXX" \
  -DPython3_EXECUTABLE="$PYTHON"
run_step build "$CMAKE" --build "$DALI_WEB_BUILD_DIR" --parallel "$jobs"
run_step linker-test "$NODE" "$DALI_WEB_BUILD_DIR/core/linker-test.js"
run_step core-smoke "$NODE" "$DALI_WEB_BUILD_DIR/core-smoke.js"
