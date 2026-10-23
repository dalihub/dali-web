#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "$0")/env.sh"
mkdir -p "$DALI_WEB_BUILD_DIR/adaptor" "$DALI_WEB_RUN_DIR/adaptor"
result=0
for relative_source in \
  dali/internal/graphics/gles/gl-implementation.cpp \
  dali/internal/graphics/gles/egl-implementation.cpp \
  dali/internal/graphics/gles-impl/egl-graphics-controller.cpp \
  dali/internal/graphics/gles-impl/gles2-graphics-memory.cpp \
  dali/internal/graphics/gles-impl/gles3-graphics-memory.cpp \
  dali/internal/adaptor/common/combined-update-render-controller.cpp; do
  unit=$(basename "$relative_source" .cpp)
  if "$EMXX" -std=c++17 -c -DDEBUG_ENABLED \
    -I"$DALI_WEB_BUILD_DIR/generated/include" -I"$DALI_WEB_ROOT/libs/dali-core" -I"$DALI_WEB_ROOT/libs/dali-adaptor" \
    "$DALI_WEB_ROOT/libs/dali-adaptor/$relative_source" \
    -o "$DALI_WEB_BUILD_DIR/adaptor/$unit.o" \
    > "$DALI_WEB_RUN_DIR/adaptor/$unit.log" 2>&1; then
    printf 'COMPILE PASS %s\n' "$relative_source"
  else
    printf 'COMPILE FAIL %s (see logs/adaptor/%s.log)\n' "$relative_source" "$unit"
    result=1
  fi
done
exit "$result"
