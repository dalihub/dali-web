cmake_minimum_required(VERSION 3.18)
project(dali-web-context-boundary LANGUAGES C CXX)
if(NOT EMSCRIPTEN)
  message(FATAL_ERROR "Use emcmake for the boundary probe")
endif()
get_filename_component(DALI_WEB_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
include("${DALI_WEB_ROOT}/cmake/gles.cmake")
set(STATIC ON CACHE BOOL "Static Core" FORCE)
set(ENABLE_PKG_CONFIGURE OFF CACHE BOOL "No source outputs" FORCE)
set(ENABLE_LINK_TEST OFF CACHE BOOL "No mock helpers" FORCE)
set(ENABLE_TIZEN_NAMESPACE OFF CACHE BOOL "Dali namespace" FORCE)
set(DALI_GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/include")
add_compile_options(-pthread)
add_link_options(-pthread)
add_subdirectory("${DALI_WEB_ROOT}/libs/dali-core/build/tizen" core)
# This is a boundary regression probe, not the final production Web closure.
set(adaptor_graphics_dir "${dali_gles_graphics_dir}")
include("${adaptor_graphics_dir}/gles-impl/file.list")
set(cachePath "/diagnostic-only")
configure_file("${DALI_WEB_ROOT}/libs/dali-adaptor/build/tizen/shader-cache-path.in" "${CMAKE_CURRENT_BINARY_DIR}/shader-cache-path.cpp" @ONLY)
add_library(gles-boundary OBJECT
  ${adaptor_graphics_gles_common_src_files}
  ${adaptor_graphics_gles_pending_src_files}
  ${dali_gles_context_source}
  "${adaptor_graphics_dir}/gles/gl-implementation.cpp"
  "${adaptor_graphics_dir}/gles/gl-extensions.cpp"
  "${adaptor_graphics_dir}/gles/gl-extensions-support.cpp"
  "${adaptor_graphics_dir}/common/shader-parser.cpp"
  "${DALI_WEB_ROOT}/libs/dali-adaptor/dali/devel-api/adaptor-framework/environment-variable.cpp"
  "${CMAKE_CURRENT_BINARY_DIR}/shader-cache-path.cpp"
  "${DALI_WEB_ROOT}/tests/probes/gles-context-boundary.cpp")
target_compile_features(gles-boundary PRIVATE cxx_std_17)
target_compile_definitions(gles-boundary PRIVATE DEBUG_ENABLED)
target_include_directories(gles-boundary PRIVATE
  "${DALI_GENERATED_INCLUDE_DIR}"
  "${DALI_WEB_ROOT}/libs/dali-core"
  "${DALI_WEB_ROOT}/libs/dali-adaptor")
