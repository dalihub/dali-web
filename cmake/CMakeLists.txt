cmake_minimum_required(VERSION 3.18)
project(dali-web-core-probe LANGUAGES C CXX)

if(NOT EMSCRIPTEN)
  message(FATAL_ERROR "Configure this probe with emcmake")
endif()

set(STATIC ON CACHE BOOL "Build static DALi" FORCE)
set(ENABLE_PKG_CONFIGURE OFF CACHE BOOL "Avoid source-tree pkg-config output" FORCE)
set(ENABLE_LINK_TEST ON CACHE BOOL "Build upstream core linker test" FORCE)
get_filename_component(DALI_WEB_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(DALI_GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/include")
set(ENABLE_TIZEN_NAMESPACE OFF CACHE BOOL "Use the Dali namespace for this probe" FORCE)
add_subdirectory("${DALI_WEB_ROOT}/libs/dali-core/build/tizen" core)

get_target_property(probe_test_sources linker-test SOURCES)
list(REMOVE_ITEM probe_test_sources linker-test.cpp)
add_executable(core-smoke "${DALI_WEB_ROOT}/tests/probes/core-smoke.cpp" ${probe_test_sources})
target_compile_features(core-smoke PRIVATE cxx_std_17)
target_include_directories(core-smoke PRIVATE
  "${DALI_WEB_ROOT}/libs/dali-core/automated-tests/src/dali/dali-test-suite-utils")
target_link_libraries(core-smoke PRIVATE dali2-core)
target_link_options(core-smoke PRIVATE -sALLOW_MEMORY_GROWTH=1 -sASSERTIONS=1 -sEXIT_RUNTIME=1)
configure_file("${DALI_WEB_ROOT}/tests/browser/core-smoke.html" core-smoke.html COPYONLY)
