# P2-b 구현 인수인계

최신 상태(2026-10-23): [생산 구현·strict 링크 완료 결과](P2-b-PRODUCTION-RESULTS.md). 아래 내용은 이전 checkpoint 기록이며 현재 승인 대기나 남은 생산 링크 상태를 뜻하지 않는다. 실제 browser/GPU 인수는 P3/P4에 남는다.

현재 상태: [Web 메모리 중간 구현·CPU 검사](P2-b-WEB-MEMORY-RESULTS.md). 단계 4의 생산 Buffer/Controller 통합 검사와 host 비동기 failure/stop 연결이 남았다. 공개 Graphics API·native Memory2/3 유지. 기존 승인 범위에서 계속 가능하며 전체 P2-b·상위 P2는 미완료다. 아래의 단계 4 미구현 설명은 이전 context/sync 시점의 기록이다.

2026-10-23. P2-a manifest/진단 및 source/context에 이어 단계 3 sync 환경 분리의 중간 구현·검증을 남겼다. [sync 결과](P2-b-SYNC-RESULTS.md), [이전 context 결과](P2-b-RESULTS.md). 다음은 단계 4 웹 메모리다. 실제 host·생산 링크·native runtime은 미완료이며 D1~D4와 [P1 계약](P1-EXECUTION-CONTRACT.md)을 유지한다. 아래 순서는 자동 실행 지시가 아니다.

## 적용 순서와 파일 경계

| 순서 | 변경 파일/영역 | 작업과 선행 관계 | 단계별 확인 |
|---|---|---|---|
| 1 | `libs/dali-adaptor/dali/internal/graphics/file.list`, `gles-impl/file.list`, `libs/dali-adaptor/build/tizen/CMakeLists.txt`, 새 `cmake/gles.cmake` | 기존 GLES/Vulkan/DYNAMIC 축 아래 native 기본값을 갖는 GLES 환경 선택 추가. 공통/Native-EGL/Web 목록 명시. generated version·namespace·cache path는 binary directory로 관리. `shader-cache-path.in`은 native용으로 보존 | native 기본 source 선택 불변, 잘못된 Web/Vulkan 조합 configure 거부, source manifest/hash 비교. 전체 Tizen Adaptor를 Web에 링크하지 않음 |
| 2 | `gles-impl/egl-graphics-controller.{h,cpp}`, `egl-graphics-controller-debug.*`, 관련 resource/context 헤더·cpp; 새 `gles-impl/gles-graphics-controller.*`, `gles/native-gles-context.*`, `web/web-gles-context.*` | 공통 Controller 이름·책임 정리. `Initialize`의 EglSyncImplementation cast, `ActivateResourceContext`의 EglGraphics dynamic_cast/GetEglImplementation/GetContext를 native 파일로 이동. 기존 GraphicsInterface/RenderSurfaceInterface 우선 사용. 한 Web context identity와 cache 연결 | 공통 object nm에서 EglGraphics RTTI·EglImplementation 심볼 배제, native context 생성/공유/표면·종료 회귀. Core Graphics::Controller 계약 유지 |
| 3 | `gles-impl/gles-sync-pool.*`, `gles-texture-dependency-checker.*`, `egl-sync-object.*`, `gles-sync-object.*`; 새 `gles/native-gles-sync.*`, `web/web-gles-sync.*` | context 분리 뒤 dependency policy와 환경별 sync 생성/대기 분리. SharedSyncObject의 NativeFence PollFD/CloseFD·DuplicateNativeFenceFD, concrete EGL object는 native에 격리. WebGL2 fence/poll로 필요한 GPU 완료 의미 구현. frame age나 CPU flush를 GPU 완료로 취급하지 않음 | 공통/Web nm에서 NativeFence/FD/EGL 타입 배제, fence 미완료/완료/실패/삭제·종료 중 해제 검사. 미지원 interop 명시 거부, 빈 성공 sync 불가 |
| 4 | `gles-impl/gles-graphics-buffer.*`, Controller MapBufferRange factory; 새 `web/web-buffer-memory.*`, 필요 시 `web/web-buffer-storage.*`; native `gles2/3-graphics-memory.*` | 웹 매핑·업로드와 버퍼 수명 저장소 분리. 필요한 callback/partial 경로에만 CPU shadow, 명시적 구간 업데이트와 shadow 일치. native Memory2/3 유지. Memory4 추가 없음 | bounds/overflow/map-lock offset·크기·allocation 실패·double unmap·grow/recycle. Core 호출부의 실패 처리로 계약 변경이 필요하면 먼저 영향 보고 |
| 5 | `gles/gl-implementation.*`, `gles2/3-implementation.h`, `gl-extensions.*`, `gl-extensions-support.*`, `gles-impl/gles-context.cpp`; 새 Web GL 구현 파일 | native mapping·advanced blend 등 지원 밖 import를 Web wrapper에서 분리. WebGL2 실제 capability 확인, GLES 버전 위장 금지. extension 이름 탐색과 native eglGetProcAddress 의존 구분 | 기본 FULL_ES3=OFF strict link. SDK가 링크 제공하는 binary/GLES3.1+ API도 runtime 지원으로 간주하지 않음. 일반 blend/source shader·지원 밖 요청의 명시 오류 검사 |
| 6 | `gles-impl/gles-graphics-program.cpp`, `common/shader-parser.cpp`, `build/tizen/shader-cache-path.in`, file-loader/system-error/version 관련 생산 구현 | 웹 source shader 경로와 native binary cache 파일 경로를 컴파일 경계에서 분리. runtime bool=false만으로 FileLoader/version/error 심볼이 사라진다고 가정하지 않음. 필요한 utility는 기존 실제 구현을 선별 | Web nm/link에서 native binary cache 의존 제거, native cache load/save/version naming 회귀. 임의 version 상수·빈 FileLoader로 링크 통과 금지 |
| 7 | 새 `web/` host/context/surface, `tests/probes/gles-link-smoke.cpp`, 진단 CMake/script | 실제 host RenderController·PlatformAbstraction·GraphicsInterface·RenderSurface 구현. link-only 진입점을 실제 초기화·자원 생성·command submit·회수 경로로 확장. Core+생산 Controller 연결 유지 | 실제 선택된 생산 경로 strict Wasm link exit 0, map의 GLES/Core 포함 및 mock 미포함 확인. map 실패/부분 object 결과는 성공 아님 |
| 8 | 웹 host callback/timer/trigger/thread ownership, `combined-update-render-controller` 웹 경계 | D3 main 이벤트 + update/upload/render pthread. main ready/stop 대기 비동기화, DOM 데이터 경계. native semaphore/join loop를 main으로 복사하지 않음 | compile/link -pthread 일치, Worker GL 소유권·main 응답·시작/종료. 최초 browser 단위에 D4 로컬 SW/헤더 probe, 실제 Pages 배포는 별도 승인 |

위 표는 전체 목표다. Controller/context/native rendering 분리와 sync 환경 파일 분리를 구현했다. WebFence는 zero-timeout poll, pending/complete/failed/cancelled 상태, owner 삭제·다른 스레드 해제 요청·종료 cancellation을 구현했다. native cast/FD는 native 파일로 이동했고 native 정책을 보존했다. Web은 native interop/cross-context GPU wait를 거부하며 shared fence를 frame 단위로 재사용하지 않는다. 실제 GPU 동작은 미검증이다.

단계 4 웹 메모리 → 5 wrapper/capability → 6 cache → 7/8 실제 host가 남았다. host는 `WebFence::HasFailure()`를 GL owner에서 읽어 비동기 오류/stop으로 전달하고, 마지막 제출 후 fence shutdown → context 폐기 순서를 보장해야 한다. 실패 후에는 새 Worker/실행 인스턴스를 사용한다. Core/GraphicsSyncAbstraction 계약을 바꿔야 하면 먼저 중단·보고한다.

`DALI_GLES_ENVIRONMENT=NATIVE_EGL` 기본값과 WEB production configure 거부를 유지한다. `cmake/p2-b`는 이제 Web sync를 선택하지만 여전히 native memory/wrapper/cache를 포함하는 부분 경계 진단이다. 정식 source closure와 native 생성 경로 전체 정리는 미완료다.

## 회귀·완료 게이트

1. P2-b: 새 build/log 경로에서 Core·생산 GLES·실제 host를 컴파일하고 strict 링크. 기본 WebGL2의 exit 0이 목표이며 FULL_ES3 비교 성공을 대신 사용하지 않는다. nm/map에서 concrete EGL/native/FD 배제, source manifest와 링크에 실제 포함된 자원/명령/Controller 확인. 과거 R2 mock은 보존하되 생산 근거로 쓰지 않는다.
2. 공통 수정마다 native GLES/Vulkan/DYNAMIC configure/source 선택 및 가능한 native compile 회귀. 환경 의존성 부족으로 못 돌리면 명령·장애와 미검증 파일 범위를 적고 완료 판정 전에 보고. native GL 자원·표면·sync·program cache·shutdown 실행 회귀는 가능한 환경에서 수행하고 미실행이면 구분.
3. P3: 실제 DALi shader/geometry 사각형·배경 픽셀/GL error, 16-byte 패턴의 중간 4-byte 갱신 전후 readback/픽셀, callback 0/부분/전체, nonzero map offset, CPU/GPU uniform, 두 frame, grow/recycle·수명 검사. shadow CPU 단위 검사만으로 GPU 보존 통과 처리 금지.
4. P3/P4: Worker·context 생성/해제, main blocking 제거, 비동기 오류/ready/stop, 입력·resize·애니메이션·반복 실행. context loss는 명시 중단/재시작, 자동 복구는 별도.
5. 최초 pthread/browser 검증에서 헤더 없는 로컬+SW와 직접 COOP/COEP 비교. 별도 승인 후 Pages 실제 URL의 window/worker isolation·SAB·Worker GL·최초/재방문·업데이트/해제·하위 경로 검증. 실패/과도한 비용이면 직접 호스팅 검토. 다른 관리형 호스팅 금지.
6. P3/P4에 이벤트 Worker 플레이그라운드 격리 후보 검증, P4 종료 전 무한 루프 중 main 응답과 전체 pthread/Canvas 종료·재생성 결과 기록. 기본 E 성공을 임의 코드 격리 성공으로 확대하지 않음.

P2-a 진단은 P2-b 착수 근거를 제공한다. **상위 P2는 생산 링크 성공 전까지 미완료**이며 실제 픽셀/생산 실행은 이후 별도 게이트다.
