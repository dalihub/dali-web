cmake_minimum_required(VERSION 3.18)
project(dali-web-production LANGUAGES C CXX)
if(NOT EMSCRIPTEN)
  message(FATAL_ERROR "Use emcmake with the locked Emscripten SDK")
endif()
get_filename_component(DALI_WEB_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(DALI_GLES_ENVIRONMENT WEB CACHE STRING "WebGL2 environment" FORCE)
set(ENABLE_GRAPHICS_BACKEND GLES)
set(STATIC ON CACHE BOOL "Static Core" FORCE)
set(ENABLE_PKG_CONFIGURE OFF CACHE BOOL "Keep source tree clean" FORCE)
set(ENABLE_LINK_TEST OFF CACHE BOOL "Production excludes mock helpers" FORCE)
set(ENABLE_TIZEN_NAMESPACE OFF CACHE BOOL "Dali namespace" FORCE)
set(DALI_GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/include")
add_compile_options(-pthread)
add_link_options(-pthread)
add_subdirectory("${DALI_WEB_ROOT}/libs/dali-core/build/tizen" core)
set(adaptor_graphics_dir "${DALI_WEB_ROOT}/libs/dali-adaptor/dali/internal/graphics")
include("${adaptor_graphics_dir}/file.list")
add_library(dali-web-renderer OBJECT ${adaptor_graphics_gles_src_files}
  "${DALI_WEB_ROOT}/libs/dali-adaptor/dali/devel-api/adaptor-framework/environment-variable.cpp")
target_compile_features(dali-web-renderer PRIVATE cxx_std_17)
target_compile_definitions(dali-web-renderer PRIVATE DEBUG_ENABLED)
target_include_directories(dali-web-renderer PRIVATE "${DALI_GENERATED_INCLUDE_DIR}"
  "${DALI_WEB_ROOT}/libs/dali-core" "${DALI_WEB_ROOT}/libs/dali-adaptor")
add_executable(dali-web "${DALI_WEB_ROOT}/web/host/web-host.cpp" $<TARGET_OBJECTS:dali-web-renderer>)
target_compile_features(dali-web PRIVATE cxx_std_17)
target_compile_definitions(dali-web PRIVATE DEBUG_ENABLED)
option(DALI_WEB_TEST_READBACK "Read two real frames for the P3 browser test" OFF)
option(DALI_WEB_TEST_MEMORY "Production GPU memory probe" OFF)
if(DALI_WEB_TEST_MEMORY)
  target_compile_definitions(dali-web PRIVATE DALI_WEB_TEST_MEMORY DALI_WEB_TEST_READBACK)
endif()
if(DALI_WEB_TEST_READBACK)
  target_compile_definitions(dali-web PRIVATE DALI_WEB_TEST_READBACK)
endif()
target_include_directories(dali-web PRIVATE "${DALI_GENERATED_INCLUDE_DIR}"
  "${DALI_WEB_ROOT}/libs/dali-core" "${DALI_WEB_ROOT}/libs/dali-adaptor")
target_link_libraries(dali-web PRIVATE dali2-core)
target_link_options(dali-web PRIVATE --no-entry -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2
  -sFULL_ES3=0 -sERROR_ON_UNDEFINED_SYMBOLS=1 -sABORTING_MALLOC=0 -sASSERTIONS=1
  -sPTHREAD_POOL_SIZE=1 -sOFFSCREENCANVAS_SUPPORT=1 -sALLOW_BLOCKING_ON_MAIN_THREAD=0
  -sMODULARIZE=1 -sEXPORT_NAME=createDaliWeb -sENVIRONMENT=web,worker
  "-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/dali-web.map")

configure_file("${DALI_WEB_ROOT}/web/index.html" "${CMAKE_CURRENT_BINARY_DIR}/index.html" COPYONLY)
configure_file("${DALI_WEB_ROOT}/web/bootstrap.js" "${CMAKE_CURRENT_BINARY_DIR}/bootstrap.js" COPYONLY)

option(DALI_WEB_TEST_EVENT_PTHREAD "Build the independent C ownership/isolation probe" OFF)
if(DALI_WEB_TEST_EVENT_PTHREAD)
  add_executable(dali-web-event-probe "${DALI_WEB_ROOT}/tests/probes/event-pthread.cpp" $<TARGET_OBJECTS:dali-web-renderer>)
  target_compile_features(dali-web-event-probe PRIVATE cxx_std_17)
  target_compile_definitions(dali-web-event-probe PRIVATE DEBUG_ENABLED)
  target_include_directories(dali-web-event-probe PRIVATE "${DALI_GENERATED_INCLUDE_DIR}"
    "${DALI_WEB_ROOT}/libs/dali-core" "${DALI_WEB_ROOT}/libs/dali-adaptor")
  target_link_libraries(dali-web-event-probe PRIVATE dali2-core)
  target_link_options(dali-web-event-probe PRIVATE --no-entry -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2
    -sFULL_ES3=0 -sERROR_ON_UNDEFINED_SYMBOLS=1 -sABORTING_MALLOC=0 -sASSERTIONS=1
    -sPTHREAD_POOL_SIZE=2 -sOFFSCREENCANVAS_SUPPORT=1 -sALLOW_BLOCKING_ON_MAIN_THREAD=0
    -sMODULARIZE=1 -sEXPORT_NAME=createDaliWeb -sENVIRONMENT=web,worker)
  set_target_properties(dali-web-event-probe PROPERTIES OUTPUT_NAME dali-web
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/event-probe")
endif()
