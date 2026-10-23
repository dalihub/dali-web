# P2-a 생산 GLES 진단 manifest

2026-10-23. 기준 HEAD `3eb2c89ecad10f810d2ff064f2b4f3d43a42a426`, upstream/toolchain lock 유지. 정확한 입력 목록은 [sources.txt](../cmake/p2-a/sources.txt), [hash](evidence/p2-a/source-hashes.json), 빌드 조합은 [CMake](../cmake/p2-a/CMakeLists.txt)이다. 이 목록은 **분리 전 진단 대상**이며 최종 Web source closure가 아니다.

## 컴파일·링크 집합

1. Core: `libs/dali-core/build/tizen`의 `dali2-core` 전체 정적 라이브러리. 별도 build에서 Debug·`-pthread`로 재빌드. generated namespace/invoke-method는 binary directory. `ENABLE_LINK_TEST=OFF`로 자동 테스트 helper/mock 미포함.
2. GLES: `gles-impl/file.list` 23개 cpp 전부. Controller/debug, EGL sync object, buffer/command/debug/framebuffer, Memory2/3, pipeline/program/reflection/render-pass/render-target/sampler/shader/texture/pipeline-cache/context/sync-pool/GL sync/framebuffer-state-cache/texture-dependency-checker. 제외로 참조를 숨기지 않는다.
3. Wrapper: `gles/gl-implementation.cpp`, `gl-extensions.cpp`, `gl-extensions-support.cpp`. GLES2/3 inline wrapper의 vtable 참조도 포함. EGL extension lookup은 아직 남아 있으며 Web 구현으로 확정한 것이 아니다.
4. Utility: `common/shader-parser.cpp`, `devel-api/adaptor-framework/environment-variable.cpp`. PixelData·debug/trace·locale 등은 먼저 Core 제공 여부와 nm를 확인한다. 전체 Tizen Adaptor를 가져오지 않는다.
5. 생성 utility: `build/tizen/shader-cache-path.in`을 binary directory의 `shader-cache-path.cpp`로 생성. 진단용 cachePath는 `/diagnostic-only`. native CMake의 source tree 생성 동작을 복사하지 않는다. 실행하지 않으며 웹 shader binary 지원을 뜻하지 않는다.
6. Harness: `gles-link-diagnostic.cpp`. KEEPALIVE 진입점에서 실제 GlImplementation/Controller를 생성하고 Core::New에 Controller를 주입한다. host RenderController/PlatformAbstraction은 외부 인자이고 구현하지 않았다. phase-2/context, Core frame, sync-pool native dependency도 별도 KEEPALIVE 진입점으로 참조하며 함수는 실행하지 않는다. Controller vtable이 생산 resource·command 메서드를 참조하며 직접 object 링크로 unused archive 성공을 방지한다.

## 의도적으로 더하지 않은 집합

Native `egl-graphics`, `egl-implementation`, `egl-sync-implementation`, native fence/image, surface/window factories, 전체 Adaptor/thread controller, UI, X11/Wayland/Ecore/TBM, font/image codec은 closure 확장 대상이 아니다. 현재 source의 이들 의존을 오류로 관측하고 P2-b 파일 분리 근거로 쓴다. R2의 `eglCopyBuffers`/`eglCreatePixmapSurface`는 egl-implementation을 포함한 별도 탐색 결과이며 이 manifest에 반드시 나타날 심볼은 아니다.

## 재현

새 build/run 디렉터리만 허용한다. SDK·host 도구 선택은 [BUILDING](BUILDING.md)의 환경변수/PATH 탐색을 따른다. 개인 경로는 필수 조건이 아니다.

```sh
export DALI_WEB_BUILD_DIR="$PWD/build/p2-a/my-new-run"
export DALI_WEB_RUN_DIR="$PWD/logs/p2-a/my-new-run"
source scripts/env.sh
python3 scripts/diagnose-gles.py
```

Core build는 최대 -j6, 진단 번역 단위는 순차 컴파일. `commands.json`에 각 argv/cwd/exitCode, 각 `nm-*.log`에 demangled undefined 참조를 보존한다. `compile_commands.json`은 Core 개별 컴파일 인자도 기록한다. 기본 WebGL2와 FULL_ES3 비교 링크는 모두 `-pthread`, assertions, `ERROR_ON_UNDEFINED_SYMBOLS=1`, linker error-limit=0과 map 요청을 사용한다. 링크 실패와 Python의 결과 수집 성공은 별개다. 컴파일 실패가 있으면 링크는 성공 object의 부분 집합 진단이므로 collection.json을 먼저 확인한다. map이 없으면 생성되지 않은 것으로 기록하며 성공 map을 만들어내지 않는다.

이 harness는 native 결합을 드러내는 링크 전용이다. 실제 host와 Canvas·Worker 생성·렌더·종료 실행은 없다. phase-2/frame 참조는 링크 보존만 위한 것이며 유효한 실행 순서가 아니다. P2-b에서는 이를 실제 host와 자원/명령 사용 경로로 확장하고 map을 확인해야 생산 링크 게이트를 통과할 수 있다.
