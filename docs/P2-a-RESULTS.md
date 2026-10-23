# P2-a 생산 GLES 링크·의존성 진단 결과

2026-10-23. **P2-a 진단 완료, 상위 P2 미완료, P2-b 미착수.** 생산 소스 변경 없이 현재 source closure를 컴파일하고 strict 링크 실패를 재현했다. [manifest](P2-a-MANIFEST.md), [P2-b 변경 순서·회귀 계획](P2-b-HANDOFF.md), [작은 증거](evidence/p2-a/README.md).

## 기준·실행 결과

시작 HEAD와 원격 main은 `3eb2c89ecad10f810d2ff064f2b4f3d43a42a426`, clean main. origin은 `https://github.com/dalihub/dali-web.git`. upstream/lock 갱신 없음. 최초 원격 조회는 sandbox DNS 오류 exit 128, 호스트 권한 읽기 조회 exit 0으로 일치 확인. 원본/P0/SDK와 과거 증거를 입력으로 수정하지 않았다.

| 검사 | 종료 코드 | 결과/한계 |
|---|---|---|
| 각 도구 버전 | 각각 0 | Emscripten 6.0.13, LLVM 24.0.0git, Node 24.19.0, CMake 3.22.1, Ninja 1.10.1, Python 3.10.12. 실제 문자열은 versions.txt |
| diagnostic-001 configure | 1 | native CMake 생성물 shader-cache-path.cpp를 실제 source로 잘못 포함. 최초 실패 보존 |
| diagnostic-002 configure/Core build | 각각 0 | shader-cache-path.in을 binary directory에 생성. Core 333단계, pthread Debug 정적 빌드 |
| diagnostic-002 30개 compile/nm | 각각 0 | 초기 harness: Controller/Core 생성·wrapper vtable만 유지 |
| diagnostic-002 기본/FULL_ES3 링크 | 각각 1 | 미해결 10/6개. 아직 참조하지 않은 초기화·native dependency 경로가 GC로 제거됨 |
| diagnostic-003 configure/Core build | 각각 0 | 새 build/log, Core 333단계. 이전 결과 덮어쓰기 없음 |
| diagnostic-003 30개 compile 및 각 nm | 각각 0 | GLES 23 + wrapper 3 + utility 2 + generated cache path 1 + harness 1 |
| diagnostic-003 기본 WebGL2 strict 링크 | 1 | 고유 미해결 16개 |
| diagnostic-003 FULL_ES3 strict 링크 | 1 | 고유 미해결 12개, 매핑 관련 4개만 감소 |
| Core defined-symbol nm | 0 | object undefined 중 Core가 제공하는 심볼을 대조. 최종 링크 완료 증거는 아님 |
| 진단 Python (002/003) | 각각 0 | 실패 포함 결과 수집 성공. 생산 링크 성공 아님 |

최종 harness는 KEEPALIVE로 실제 Controller/Core 생성, phase-2 초기화·resource context 활성화, Core Update/PreRender/RenderScene/PostRender 및 sync-pool native dependency 경로를 추가 참조했다. 자원·command 메서드는 생산 Controller vtable로 연결된다. mock/placeholder ABI/빈 GL·EGL·sync 구현은 없다. 전체 object를 직접 링크하며 undefined 오류를 유지했다. 실패 시 linker map은 생성되지 않았고 최종 실행 가능한 JS/Wasm도 확보하지 못했다.

## nm/linker 의존성 분류

| 분류 | 실제 근거 | P2-b 처리 |
|---|---|---|
| Core | Controller object의 PixelData buffer/release·Debug, wrapper의 TimeChecker, sync-pool의 Mutex; harness의 Core::New 및 frame 호출. `nm-boundaries.json`의 coreDefinedMatches | 현재 Core static archive 사용. Core public 계약 유지, 실제 resource/command map 검증은 링크 성공 후 |
| 공통 GLES | Controller가 Buffer/CommandBuffer/Texture/Framebuffer/Program/Pipeline/Sync/Memory2/3 참조 | 23개 전체를 출발 closure로 유지, 환경 분리 후 최종 manifest 확정 |
| GL | wrapper vtable에서 `glUnmapBuffer`, `glGetBufferPointerv`, `glMapBufferRange`, `glFlushMappedBufferRange`, `glBlendBarrier` 미해결 | 매핑은 D2 웹 메모리, advanced blend는 미지원 명시. FULL_ES3는 앞 4개 링크 제공만 확인 |
| EGL concrete 타입·RTTI | `typeinfo for EglGraphics`, `EglGraphics::GetEglImplementation() const`, `EglImplementation::GetContext() const` 미해결 | Controller ActivateResourceContext의 dynamic_cast를 native 파일로 이동. 함수 호출뿐 아니라 RTTI도 제거 필요 |
| Native fence | `EglSyncObject::DuplicateNativeFenceFD()`, `NativeFence::PollFD(int)`, `NativeFence::CloseFD(int)` 미해결 | SyncPool native 분리, Web sync 의미 구현. 전체 native fence를 Web에 추가하지 않음 |
| Utility / binary cache | `FileLoader::ReadFile(...)`, `ADAPTOR_MAJOR/MINOR/MICRO_VERSION`, `TizenPlatform::PrintSystemError(...)` 미해결 | Program binary 경로 컴파일 분리. version은 기존 `public-api/dali-adaptor-version.cpp`, error는 `internal/system/generic/system-error-print-generic.cpp`에 실제 정의가 있으나 이번에는 추가 링크하지 않음 |
| EGL API lookup | gl-extensions object nm의 `eglGetProcAddress` | SDK 링크에서 해당 이름 오류가 없다는 사실만 확인. native extension/API 호환성 보장 아님 |
| C/C++ runtime | malloc/free, STL, mutex/TLS, exceptions/RTTI 관련 undefined | SDK libc/libc++/pthread에 맡김. 모든 nm undefined를 누락 구현으로 계산하지 않음 |

`FileLoader` 생산 facade는 generic 구현 외에 download 관련 include/경로도 갖는다. 단일 심볼 해소를 위해 전체 imaging/network stack을 가져오지 않았다. generated cache path를 제공해도 file/version/error 의존이 남는다는 사실을 확인했다. 웹에서 program binary를 끄는 runtime flag만으로 링크 경계가 해결되지 않는다.

R2의 265개 직접 GL/EGL 탐색과 이번 집합은 다르다. 이번에는 `egl-implementation.cpp`를 넣지 않아 `eglCopyBuffers`/`eglCreatePixmapSurface`가 진단에 없으며, 이를 해결했다고 해석하지 않는다. GLES3.1+의 추가 미해결 이름은 이번 linker 출력에 없지만 SDK 함수 존재만으로 지원을 승인하지 않는다. concrete sync의 virtual call/inline cast는 이름이 linker 오류에 없어도 소스 의존이다.

## 특이 사항·한계·판정

1. shader-cache-path는 native CMake 생성물이므로 진단 build 경로에만 생성했다. native source 생성 경로 변경은 하지 않았다. 최초 오류와 수정 후 결과를 별도로 보존했다.
2. 최초 harness의 GC가 native 오류를 가릴 수 있음을 실제 10/6 → 16/12 차이로 확인했다. 최종 harness에 초기화·프레임·native sync 참조를 추가했지만 이는 **링크 전용 경로**다. host 서비스·Canvas·frame loop·정상 종료·실제 함수 실행을 구현하거나 검증하지 않았다.
3. compile/link 모두 pthread 옵션이지만 Worker 실행, SAB, OffscreenCanvas, main 비동기 ready/stop, Pages/SW는 미검증. FULL_ES3의 링크 지원과 Memory3 WRITE-only 거부·보존 문제는 별개다.
4. native GLES/Vulkan/DYNAMIC build/install·실행, 전체 Adaptor/UI, 브라우저 픽셀·shader·GPU 보존·성능·Windows·context loss는 미검증. 이번 생산 소스 diff는 없으며 native 회귀 계획은 P2-b에 명시했다.
5. 진단은 합의한 범위 안에서 종료했다. **P2-b 진행 가능**: 확인한 concrete 의존을 파일 단위로 분리할 근거가 있다. 구현 중 공통 계약 변경·플랫폼 전체 포팅·실험 확대가 필요하면 먼저 멈추고 보고한다. 생산 링크가 실패했으므로 상위 P2 완료는 선언하지 않는다.

전체 로그/산출물은 `logs/p2-a/{run-001,diagnostic-001,diagnostic-002,diagnostic-003}`와 `build/p2-a/run-00{1,2,3}`에 보존한다. 작은 공개 증거에는 개인 SDK 경로를 환경변수 표기로 정규화했다. SDK·산출물·대용량 로그는 commit 대상이 아니다. 서버·브라우저·배포를 실행하지 않았고 빌드 프로세스는 종료했다.
