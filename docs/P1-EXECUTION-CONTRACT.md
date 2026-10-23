# P1 웹 실행 계약 설계

2026-10-23 (Asia/Seoul). **D1~D4 설계 결정 확정, P1 설계 완료. P2 미착수.** 사용자가 공통 GLES/환경 분리, 웹 버퍼 보존, 메인 이벤트 + 업데이트·렌더 Worker, GitHub Pages 우선 검증과 실패 시 직접 호스팅 검토를 승인했다. 생산 코드 변경·생산 링크·실제 Pages 실행 성공을 뜻하지 않는다. 아래 미검증 조건은 각 후속 단계의 인수 게이트로 유지한다. D1 결정 ID와 PLAN의 배포 단계 D1은 별개다.

기준 HEAD: `ef6a1f545ff03df69c3297135048f545ee8dd352`. 시작 main clean, 원격 main 일치 확인. [upstreams.lock.json](../upstreams.lock.json), [toolchain.lock.json](../toolchain.lock.json) 유지. Core `e2ce797a41cff49851361210491c8ed38bea46e9`, Adaptor `f0dd9b39d5a51ff0233ea50bf872dd6200e93981`, UI `f6c046d2a15ab7c6bad8d76612e667d66ed1f6d0`. [R2-b](R2-b-RESULTS.md)의 컴파일·mock 결과를 생산 실행 증거로 확장하지 않는다.

## 1. 현재 호출 경로와 변경 경계

아래 경로의 `C/`는 `libs/dali-core/dali/`, `A/`는 `libs/dali-adaptor/dali/`이다. 줄 번호는 위 기준 checkout에서 확인한 탐색 위치이며 함수명이 추적 기준이다. 현재 구현과 제안 계약을 구별한다.

| 단계 | 현재 파일·함수 근거 | 실제 동작 / 웹 경계 |
|---|---|---|
| 이벤트 측 구성 | `A/internal/adaptor/common/adaptor-impl.cpp:175` `Adaptor::Initialize` | TizenPlatformAbstraction, 시스템 callback, graphics factory를 만들고 218행 `Core::New(RenderController, PlatformAbstraction, Graphics::Controller, flags)`. 웹은 Tizen 전체 Adaptor 대신 명시적 host 서비스로 시작하는 안 |
| Core 생성 | `C/internal/common/core-impl.cpp:87` `Core::Core` | 생성 스레드 TLS → notification/animation → RenderManager → UpdateManager → registry/relayout/gesture/shader factory. Core와 Actor 조작 소유자를 동일하게 유지 |
| 초기화 통지 | 같은 파일 186~198 `Initialize`, `ContextCreated`, `ContextDestroyed` | 앞 두 함수는 현재 비어 있다. 호출만으로 GL 준비가 끝났다고 판단할 수 없다. ContextDestroyed는 UpdateManager·RenderManager에 전달 |
| GL phase 1 | `A/internal/graphics/gles/egl-graphics.cpp:35` 생성자, `gles-impl/egl-graphics-controller.cpp:210` `InitializeGLES` | GlImplementation과 GLES::Context/Framebuffer shared-context 등록. GL context 실제 생성과 별개. program binary 기본 활성화도 여기 있음 |
| 스레드 시작 | `A/internal/adaptor/common/combined-update-render-controller.cpp:199` `Initialize`, `Start` | pthread_create, semaphore로 준비 대기. 브라우저 메인에서 이 대기 구조를 그대로 실행하지 않음 |
| GL phase 2 | 같은 파일 641 `UpdateRenderThread`; `A/internal/graphics/gles/egl-graphics.cpp:189` `Initialize/EglInitialize/InitializeGraphicsAPI` | EGL·sync·image helper 생성 → Controller.Initialize → display 초기화 → ConfigureSurface → Core.ContextCreated → surface.InitializeGraphics. Controller.Initialize는 GraphicsSyncAbstraction을 EglSyncImplementation으로 static_cast하므로 대체 sync를 그냥 넘길 수 없음 |
| 표면·공유 컨텍스트 | `egl-graphics.cpp:244` `CreateSurface`, 377 부근 `ConfigureSurface`; `gles/egl-implementation.cpp:286,334,384` `CreateContext/CreateWindowContext` | native window/config, resource context와 이를 share하는 window context. surfaceless가 없으면 surface context 활성화. 웹의 한 WebGL context로 resource/surface 역할을 통합하되 Controller 상태 캐시도 같은 실체를 보도록 해야 함 |
| 이벤트 전달 | `C/internal/common/core-impl.cpp:309` `ProcessEvents/RelayoutAndFlush` | scene 입력, notification, relayout, update queue flush. 재진입은 지연 요청으로 처리. DOM 콜백에서 다른 소유자 Core를 직접 호출하지 않음 |
| 프레임 업데이트 | `combined-update-render-controller.cpp:820,861,910` | texture upload phase 1 → Core.Update(delta,last,next,status,...) → resource context 활성화·upload phase 2. `Core::Update`는 UpdateManager로 위임, notification 필요를 status에 기록 |
| 렌더 | 같은 파일 929~1068 | Core.PreRender → graphics.RenderStart → scene별 Core.PreRenderScene → surface.PreRender → offscreen RenderScene(true) → surface RenderScene(false), 필요 시 ClearScene → graphics.PostRender → Core.PostRender. 화면 렌더 생략과 upload/cleanup 처리를 구별 |
| 제출·present | `gles-impl/egl-graphics-controller.cpp:248,297,324` `SubmitCommandBuffers/PresentRenderTarget/ResolvePresentRenderTarget` | 명령 queue, FLUSH 시 실행. present 명령은 surface.MakeContextCurrent/PostRender 호출. surface PostRender와 프레임 전체 graphics.PostRender는 서로 다른 단계 |
| 생성·업로드·회수 | `gles-impl/egl-graphics-controller.h:525` `Flush`, cpp `ProcessCreateQueues/ProcessTextureUpdateQueue/ProcessCommandQueues/ProcessDiscardQueues` | 생성 → texture update → mipmap → 명령 → 폐기. `WaitIdle()`는 이 Flush만 호출하므로 GPU 완료 보장이 아님 |
| 정상 종료 | `combined-update-render-controller.cpp:1158` 종료부; `egl-graphics.cpp:433` `Shutdown`; `core-impl.cpp:140` 소멸자 | precompiled program 정리 → Core.ContextDestroyed → surface.DestroySurface → graphics.Shutdown(Controller.Shutdown/TerminateGles). Core 소멸은 TLS 제거, SceneDestroyed, UpdateManager 후 RenderManager 해제. 웹은 마지막 GL 회수 때까지 context를 유지하는 별도 순서 검증 필요 |

### 리소스의 수명

1. `C/internal/render/renderers/gpu-buffer.cpp`의 `UpdateDataBuffer`는 CreateBuffer → MapBufferRange → LockRegion → 전체 memcpy → Unlock → UnmapMemory다. callback 경로는 같은 매핑 후 callback이 쓰고 반환한 bytesUpdatedCount를 기록하지만 업로드 범위를 그 값으로 좁히지 않는다.
2. `A/internal/graphics/gles-impl/gles-graphics-buffer.cpp`의 `InitializeResource/InitializeGPUBuffer`는 지연 생성·재활용 뒤 BufferData를 수행한다. CPU uniform buffer와 GPU buffer를 구분한다.
3. `egl-graphics-controller.cpp:1118` `UpdateTextures`는 MEMORY source를 staging에 복사하고 PIXEL_DATA를 queue에 유지한다. `ProcessTextureUpdateQueue`는 format/stride 처리 후 TexImage2D/TexSubImage2D 및 필요 release. BUFFER/TEXTURE source에는 TODO가 남아 있다. 웹 초기는 RGBA8 MEMORY/PixelData만 수용하고 async 로딩 완료 후 소유자가 enqueue한다. 소비 전에 원본을 free하거나 Worker 전송으로 detach하지 않는다.
4. `GLESDeleter`는 일반 삭제를 discard queue에 넘긴다. Shutdown 이후에는 직접 DestroyResource 경로로 바뀐다. GL object보다 Controller/context가 먼저 사라지지 않는지 반복 종료 검사 필요. source snapshot만으로 정상 수명 보장을 선언하지 않는다.
5. `gles-sync-pool.cpp`, `gles-texture-dependency-checker.cpp`, `egl-sync-object.cpp`는 EGL/native fence FD에 직접 연결된다. GL 명령 순서, CPU 큐 동기화, GPU 완료 fence는 다른 계약이다. native fence를 항상 성공하는 빈 구현으로 바꾸지 않는다.

## 2. 제안 실행 계약

초기 지원 범위는 데스크톱·단일 Canvas·WebGL 2·단일 GL 소유자, DALi 기본 geometry와 source shader, 일반 blending, RGBA8 CPU upload다. WebView·영상·카메라·다중 window·native image 공유는 초기 검증 범위 밖이다. 이 단계적 범위가 최종 제품의 영구 제외 목록이나 단일 스레드 확정을 뜻하지 않는다. 지원 확대에는 개별 기능 설계·검증이 필요하다.

| 상태/작업 | 계약 | 실패·검증 |
|---|---|---|
| Constructed | host RenderController/PlatformAbstraction, 실제 GlImplementation·EglGraphicsController를 구성. event owner에서 Core 생성. mock Controller 불가 | host 서비스 누락 시 명시 실패 |
| ContextReady | 소유자에서 Canvas WebGL2 생성/current, 실제 version·확장·limits 확인 후 GL wrapper ContextCreated와 Controller version 설정. resource/surface는 한 실체 | WebGL1로 조용히 downgrade하지 않음. GLES 3.2로 속이지 않음 |
| Running | event queue → ProcessEvents → Update → upload/PreRender → scene offscreen/onscreen render → present → PostRender. 초기 partial damage는 끄고 전체 redraw | 프레임 ID와 phase 순서, GL owner/context ID 기록 |
| Frame request | 브라우저 rAF timestamp를 단조 시간으로 사용. 한 프레임만 실행 중, 추가 요청은 dirty/pending으로 합침. 요청한 rAF는 반환해 브라우저에 제어를 돌려줌 | native SleepUntil/vsync loop를 브라우저 메인으로 복사하지 않음 |
| Clock | 최초 delta=0, 이후 ms를 seconds로 변환. hide/resume 후 기준 시간 재설정하여 숨긴 시간을 한 번에 진행하지 않는 안 | 큰 delta 처리·애니메이션 pause 정책은 P4 검증, 성능 보장 없음 |
| Input/resize | DOM에서 CSS 좌표·DPR·backing size·sequence를 전달, event owner에서 DALi 입력 변환. resize는 프레임 경계에서 scene/viewport/backing size를 함께 적용 | 여러 owner의 scene 직접 변경 금지. 낡은 resize sequence 폐기 |
| Resource visibility | event→update→render 기존 queue의 소유권·순서를 보존. upload는 GL owner만. GPU 완료 필요 시 WebGL2 fence/poll 후보 | WaitIdle/CPU mutex를 GPU 완료 신호로 사용하지 않음 |
| Stop | 새 입력/rAF/upload 차단 → 진행 프레임 완료 확인 → event owner에서 scene/handle 정리·필요 queue 전달 → GL owner에서 resource 회수·ContextDestroyed/Controller shutdown → context 폐기 → Core/host 정리 | 메인 스레드 blocking join 금지. 정상 종료와 context lost를 별도로 검사 |
| ContextLost | 이벤트 수신 후 프레임 중단, generation 무효화, 낡은 GPU handle 재사용 금지. 초기에는 오류 표시와 명시적 재시작 | 자동 복구는 P7. 재시작 시 신규 instance/context, 기존 callback 취소 검증 |

브라우저 implicit present는 콜백 반환 시점에 맡기는 안이다. `eglSwapBuffers` 성공을 실제 display 완료로 취급하지 않는다. OffscreenCanvas explicit commit을 선택하면 생성 속성과 `emscripten_webgl_commit_frame` 호출을 함께 지정하고 implicit 방식과 혼용하지 않는다. [Emscripten html5 API](https://emscripten.org/docs/api_reference/html5.h.html).

## 3. GL/EGL 지원·제외·대체 정책

R2-b의 두 링크는 각각 exit 1이다. 265개는 두 object에서 수집한 직접 import 집합이며 전체 GLES dependency closure가 아니다. FULL_ES3는 아래 매핑 4개를 링크해 줄 뿐 의미 호환성을 보장하지 않는다. [Emscripten GL 지원](https://emscripten.org/docs/porting/multimedia_and_graphics/OpenGL-support.html).

| 기능 | 권장 정책 | 구현 경계·검증 |
|---|---|---|
| eglCopyBuffers / eglCreatePixmapSurface | native pixmap 제외, 함수 export로 성공을 위장하지 않음 | 웹 대상에서 해당 구현 분리/컴파일 제외. 호출 가능한 API는 unsupported 오류. native 소스는 유지 |
| glBlendBarrier / advanced blend | advanced equation 거부, 일반 ADD/SUBTRACT/REVERSE_SUBTRACT 및 확인된 WebGL2 MIN/MAX만 | `gl-implementation.h`, `gles3-implementation.h`, `gles-context.cpp`의 capability·호출 경로 함께 차단. 불가능한 equation 요청은 draw 전 오류. fallback multipass는 별도 설계, 이번 제외 |
| glMapBufferRange / glUnmapBuffer / glFlushMappedBufferRange / glGetBufferPointerv | D2 확정: 웹 전용 CPU staging/shadow + BufferSubData | 별도 웹 메모리 구현과 버퍼 소유 shadow, wrapper에서 native map 참조 분리. 단순 unused 함수 제거에만 기대지 않음 |
| ProgramBinary/GetProgramBinary/ShaderBinary | source compile/link만 지원, binary load/save/precompile cache 끄기 | `InitializeGLES`의 기본 true를 웹 capability false로 제한하고 ProgramImpl의 파일·binary 경로 빌드 분리. 링크되는 실패용 SDK 함수가 있다는 이유로 지원 표시 금지 |
| resource/window context 공유 | 한 WebGL context에서 두 역할 직렬화 | Emscripten libegl의 share 인자 무시·단일 default surface 구조 확인. WebGL object를 별도 context로 전달하지 않음. Controller resource/surface cache 통합/무효화 검사 |
| EGL native fence / EGLImage / external texture | native image/FD interop 제외 | GLES sync pool의 native 분기를 분리. GPU 완료가 필요한 허용 경로에는 WebGL2 sync 기반 의미 있는 구현. native fence 호출 시 오류 |
| buffer age, swap damage, surfaceless, context priority | 초기 미지원, 전체 redraw와 하나의 실제 Canvas context | capability false, context 생성 attribute 제한. 지원되지 않는 EGL attribute를 무조건 통과시키지 않음 |
| 압축 texture·확장·GLES 3.1+ | 초기 범위 밖, 추후 개별 capability 검증 | wrapper vtable 때문에 참조가 남는 API까지 nm/link map에서 확인. 지원되는 것처럼 반환하지 않음 |

SDK 근거는 `upstream/emscripten/src/lib/libegl.js`의 `eglCreateContext`(share 인자를 사용하지 않음), `eglCreateWindowSurface`(default ID), `eglSwapBuffers`(lost 여부 확인 후 성공), `libwebgl2.js:908`의 program binary 오류 경로다. [EGL 공식 설명](https://emscripten.org/docs/porting/multimedia_and_graphics/EGL-Support-in-Emscripten.html)도 부분 구현을 명시한다. 온라인 문서는 개발 버전이므로 이 설계의 세부 판정은 lock SDK 소스에 우선한다.

### D1 확정: 공통 GLES와 실행 환경의 파일 단위 분리

2026-10-23 사용자 승인. 공통 GLES 렌더러를 유지하고 Native/EGL과 Web 환경 구현을 빌드 시 선택한다. 수정량 최소화나 단순 PoC 성공보다 책임 분리·의미 보존·기존 백엔드 회귀 검증을 우선한다. 앞선 HTML5 어댑터 우선안과 제한 EGL 재사용 비교를 이 결정으로 대체한다. HTML5 API는 Web 구현 내부의 컨텍스트 관리 수단이며, 전체 변경을 얇은 어댑터 하나로 끝낼 수 있다고 가정하지 않는다.

```text
Core → Graphics::Controller
       ├─ VulkanGraphicsController
       └─ 공통 GLES Controller·자원·명령 처리
            ├─ Native/EGL 환경 구현 (네이티브 GLES 빌드)
            └─ Web 환경 구현 (웹 GLES 빌드)
```

| 경계 | 공통 책임 | Native/EGL 구현 | Web 구현 |
|---|---|---|---|
| 컨텍스트·표면 | 논리 context 식별·상태 캐시·렌더 대상 수명 | EGL 생성·활성화·공유·surface present | HTML5 API로 Canvas/WebGL2 생성·활성화·제출·해제; 초기 resource/surface 역할은 같은 실제 context |
| 버퍼 업로드 | 갱신 범위·보존 의미·자원 수명 | 기존 native mapping 전략 보존 | D2의 웹 메모리 구현·버퍼 소유 shadow; 명시적 부분 업로드와 보존 의미 유지 |
| 동기화·외부 자원 | 작업 의존성과 완료 요구 | EGL/GL fence·native FD/image | 웹에서 의미를 지킬 수 있는 sync; 미지원 native interop은 명시 거부 |
| GL 호출·기능 | 공통으로 유효한 GLES 명령·shader/resource 처리 | native GL wrapper 및 확장 | WebGL 호환 wrapper·기능 집합; 지원되지 않는 import를 파일 경계에서 분리 |

1. Core의 `Graphics::Controller` 계약을 유지한다. 공통 GLES 코드에서 `EglGraphics`/`EglSyncImplementation`으로의 cast, EGL handle 및 native FD 접근을 환경별 파일로 이동한다. 실제 분리 후 `EglGraphicsController`를 `GlesGraphicsController` 등 역할에 맞게 정리하는 방향이며, 정확한 파일명은 P2 source manifest에서 확정한다.
2. 기존 `GraphicsInterface`·`RenderSurfaceInterface`를 우선 활용하고 부족한 내부 경계만 추가한다. 컨텍스트·업로드·동기화를 한 거대한 platform 클래스에 몰아넣지 않는다. `GraphicsSyncAbstraction`의 blocking wait·native fence 계약을 웹에서 그대로 충족할 수 있다고 가정하지 않는다. 공통 계약 변경이 필요하면 영향과 대안을 먼저 보고한다.
3. 빌드 조합은 공통 GLES source + 환경별 source 한 세트다. CMake target별 source·include·link dependency로 선택한다. 내부 virtual interface/factory 사용과 빌드 시 선택은 양립한다. 매 draw마다 환경을 검사하거나 공통 코드 전체에 `#ifdef`를 퍼뜨리는 방식, GLES Controller 전체 복제는 피한다.
4. GLES/Vulkan/DYNAMIC 기존 선택을 보존한다. GLES 실행 환경 선택은 그 아래의 별도 축이다. 현재 근거는 `A/internal/graphics/file.list:30`의 GLES 소스 묶음과 `libs/dali-adaptor/build/tizen/CMakeLists.txt:453` 이후의 DYNAMIC/GLES/VULKAN 구성이다. 웹에서는 Native/EGL 구현이 선택되지 않고 잘못된 조합은 configure 오류로 처리한다. 새 옵션의 정확한 이름·기본값은 P2에서 정하되 기존 native 기본 동작을 바꾸지 않는다.
5. 환경별 public/internal header가 네이티브 EGL 타입을 공통 GLES 헤더로 노출하지 않게 한다. 상태 캐시는 실제 context identity에 대응한다. 하나의 Web context를 두 개로 위장하지 않는다. EGL 및 native fence 타입은 native 구현에 둔다.
6. 빌드 선택은 가능한 기능의 상한을 정하며 실제 context limits·extensions는 실행 시 확인한다. 필요한 기능의 빈 성공 구현, undefined-symbol 억제, 임의 GLES 버전 위장은 금지한다. unsupported 요청은 명시적으로 거부한다.
7. 정식 구현의 수용 기준은 공통 GLES의 concrete EGL/native 의존 제거, 웹 link closure의 native 의존 배제, 업로드·sync·수명 의미 보존, native GLES/Vulkan 회귀, 실제 DALi 픽셀·반복 시작/종료·context loss 검증이다. P2 링크는 이 중간 마일스톤이며 최종 완료가 아니다.

D1과 별도로 D2 업로드 설계도 아래와 같이 승인됐다. D3와 D4도 아래와 같이 승인됐다. P1 설계 논의는 종료하며 이번에는 생산 구현을 시작하지 않는다.

## 4. Memory3 재확인과 데이터 보존

현재 `gles3-graphics-memory.cpp:70`은 GPU buffer에 **GL_MAP_WRITE_BIT(2)만** 넘긴다. lock SDK `libwebgl.js:4171`은 READ/UNSYNCHRONIZED를 거부하며 WRITE와 INVALIDATE_BUFFER 또는 INVALIDATE_RANGE를 요구한다. 실제 SDK의 access guard를 추출해 Node에서 실행한 [작은 검사](evidence/p1/map-guards.json)는 2→0, 6/10→guard 통과를 확인했다. GL context·실제 malloc/upload를 실행한 검사가 아니다.

1. 현재 Memory3의 GPU 분기는 FULL_ES3 링크 후에도 null을 돌려받는 경로다. Core `GpuBuffer::UpdateDataBuffer`의 memcpy는 ptr null을 검사하지 않는다. 브라우저 crash를 재현했다는 뜻은 아니지만 소스상 실행 차단 사유다.
2. invalidate를 더하는 수정은 원래 내용을 읽거나 일부만 덮어쓰는 의미를 잃는다. SDK는 새 malloc staging을 만들고 Unmap 때 매핑 전체를 BufferSubData한다. 전체 overwrite에 한해 후보이며 기존 데이터 보존의 해결책이 아니다.
3. Memory2 GPU 분기는 `malloc(LockRegion.size)` 후 `BufferSubData(mapInfo.offset,mapInfo.size,ptr)`다. 새 메모리는 초기화되지 않는다. 전체 overwrite는 후보지만 map 크기와 lock 크기가 다르면 경계 문제가 있고 offset 규칙도 명시해야 한다. 현재 CPU 분기는 기존 주소+LockRegion.offset을 반환하며 GPU staging과 다르다.
4. `GpuBuffer::UpdateDataBufferWithCallback`은 전체를 매핑하되 callback이 일부만 쓸 수 있다. 반환된 bytesUpdatedCount가 실제 dirty 범위를 보장하지 않는다. Memory2로 바꿔도 나머지 byte는 보존되지 않는다. `UniformBufferV2::WriteGPU`도 capacity 매핑에 offset 쓰기이므로 모든 사용 byte가 해당 프레임에 초기화된다는 증거가 필요하다.
5. 확정 업로드 계약: 기존 내용 보존이 필요한 웹 GPU buffer별 CPU shadow를 유지하고 최초 상태를 정의한 뒤 수정 byte를 반영한다. 전체 write는 그대로 복사, callback은 기존 shadow를 받아 수정하므로 나머지를 보존한다. map offset/length/lock 범위를 검사하고 필요한 영역을 BufferSubData. 비용은 CPU 메모리와 복사량 증가. P2는 작은 전체 write까지, callback·uniform partial 보존은 P3의 지원 판정 전에 필수 검증한다.
6. full overwrite만 허용해 callback/partial API를 제한하는 대안은 채택하지 않았다. 기존 API의 보존 의미를 유지한다. GPU read mapping은 초기 미지원. shadow가 GPU가 변경한 내용을 반영한다고 가정하지 않는다.
7. **GLES 버전을 2로 위장해서 Memory2를 선택하지 않는다.** `EglGraphicsController::MapBufferRange:1222`의 버전 분기를 upload capability로 분리하고 GL version·shader·UBO 지원은 WebGL2에 맞게 유지한다. native Memory2/3는 유지하고 별도 웹 메모리 구현을 빌드 시 선택한다. Memory4라는 버전 번호 기반 클래스를 추가하지 않는다.

### D2 확정: 웹 메모리 구현과 버퍼 소유 CPU shadow

2026-10-23 사용자 승인. `Memory2`·`Memory3`는 DALi 내부 클래스명이다. 웹용 구현은 예를 들어 `WebBufferMemory`처럼 역할에 맞게 구분한다. 이 이름은 예시이며 파일·클래스 이름은 P2에서 확정한다. 공통 Controller는 기존 메모리 계약을 사용하고 환경별 생성 경로를 선택한다.

1. **소유권:** 보존이 필요한 CPU shadow는 버퍼 또는 버퍼 수명에 종속된 웹 저장소가 소유한다. 임시 매핑 객체가 매번 새 shadow를 만드는 방식은 사용하지 않는다. 매핑 객체는 유효한 구간 접근과 업로드를 담당하며, 매핑 중에는 저장소가 이동·해제되지 않아야 한다.
2. **적용 조건:** 기존 byte를 참조·보존해야 하는 callback/partial mapping 경로에 shadow를 적용한다. 명시적 offset/length와 완전한 새 데이터가 주어지는 부분 갱신은 해당 범위만 BufferSubData하여 GPU의 나머지를 보존할 수 있다. 모든 버퍼에 무조건 사본을 추가하지 않는다. shadow가 존재하면 직접 구간 갱신도 사본에 반영해 이후 callback과 일관성을 유지한다.
3. **callback:** 기존의 유효한 내용을 제공하고 수정하지 않은 byte를 보존한다. 반환된 bytesUpdatedCount를 임의의 dirty 위치 정보로 해석하지 않는다. 정확한 변경 범위를 알 수 없으면 매핑 범위를 보수적으로 업로드한다. 기존 DISCARD/orphan 계약에서 보존을 보장하지 않은 데이터까지 유효하다고 가정하지 않는다.
4. **수명·오류:** 최초/확장 영역의 초기화 정책과 유효 범위, discard/recycle 시 보존 범위를 명시한다. map/lock의 offset·size·정수 overflow·allocation 실패를 검사한다. 실패를 성공 포인터로 위장하지 않고 기존 호출부가 안전하게 처리할 수 있는지 검증한다. 구체적인 오류 전달에 공통 API 변경이 필요하면 영향과 대안을 보고한다.
5. **경계:** GPU가 변경한 데이터는 shadow에 자동 반영되지 않는다. GPU write/readback과 context 복구는 별도 계약이다. CPU shadow를 보유한다고 자동 복구까지 완료한 것으로 취급하지 않는다. D3에 따라 웹 GL·GPU 매핑/업로드와 shadow 갱신은 업데이트·렌더 소유자에서 직렬화한다. 이벤트 측은 데이터/명령을 큐로 전달하며 매핑 포인터를 스레드 간 공유하지 않는다.
6. **비용:** 대상 버퍼의 CPU 메모리·복사량·업로드량을 측정한다. 전체 write·명시적 구간 write·callback·uniform 경로별로 결과를 기록하고, 최적화하더라도 보존 의미를 축소하지 않는다. native 구현에는 웹 shadow 비용을 강제로 추가하지 않는다.

필수 데이터 검사: 알려진 16-byte 초기 패턴 → 중간 4-byte 갱신 후 앞/뒤 보존; callback 0/부분/전체 갱신; nonzero map offset; lock 범위 초과·malloc 실패; buffer grow/recycle; CPU uniform vs GPU uniform; 두 연속 frame; unmap/destructor 중복·종료 중 해제. WebGL2 readback 또는 그 buffer를 사용한 실제 픽셀로 검증한다. 단순 CPU shadow unit test만으로 GPU 업로드 통과 처리하지 않는다.

## 5. D3 확정: 메인 이벤트 + 업데이트·렌더 Worker

현재 생산 기본은 event thread와 combined update/render pthread다. Core queue는 `C/internal/update/queue/update-message-queue.cpp`, `event/common/notification-manager.cpp`, `common/lockless-pointer-ring.h`의 SPSC batch·acquire/release와 mutex pool을 사용한다. 여러 Worker가 producer를 공유하는 설계로 바꾸지 않는다. thread-safe API라는 추측으로 모든 Actor 접근을 허용하지 않는다.

| 후보 | 소유자 / 브라우저 연결 | 장점·비용 | 후보별 검증 |
|---|---|---|---|
| E: 메인 이벤트 + 업데이트·렌더 pthread (채택) | main=DOM/앱·Core event/TLS, render Worker=update/upload/GL·OffscreenCanvas | 기존 이벤트 소유권과 DOM 연동에 자연스러움. 메인 동기 대기 제거 필요, 임의 코드의 페이지 차단은 방지하지 못함 | 비동기 초기화·종료, callback/timer/trigger, queue 순서, context 소유권·resize·resume 검증 |
| A: 메인 단일 실행 | DOM/event/update/render 모두 main, rAF 한 tick | P2/P3 진단은 단순. 긴 C++ 작업이 UI·중단을 막음 | 실제 DALi 프레임 순서·notification·입력 재진입, 긴 frame responsiveness. mock 성공만으로 채택 금지 |
| B: 단일 dedicated Worker | main=DOM/rAF/resize, Worker=Core event+update+render+OffscreenCanvas | SAB 없이 UI와 분리. DALi 내부 실행은 직렬화하는 새 host loop 필요 | Canvas transfer 전 context 생성 금지, 메시지 순서·시간 역전·resize·Worker 종료, notification/event phase 회귀 |
| C: event Worker + update/render pthread | main=DOM/rAF, app pthread=event/Core TLS, render pthread=update/upload/GL·OffscreenCanvas | 기존 DALi 동시성 구조에 가까움. SAB/격리·초기화 barrier·동기화 복잡 | 모든 DALi/host object `-pthread` 재빌드, pool readiness, SPSC stress, 정상 종료 handshake, suspend/resume, sync proxy deadlock 검사 |
| D: pthread + main proxied GL | C의 GL 호출을 main context로 proxy | OffscreenCanvas 대안. proxy 비용·양방향 대기 위험 | GL 호출 순서·proxy 지연·main 응답성과 shutdown; 기본안 아님 |

**생산 기본 E를 사용자 승인으로 채택했다.** 브라우저 메인이 DOM·앱 코드·Core 이벤트를 소유하고 별도 pthread Worker가 업데이트·업로드·렌더를 소유한다. 앞선 C 우선 권장안을 대체한다. C는 플레이그라운드 격리 후보, B는 대안이며 두 실행 구성을 동등하게 지원한다고 확정하지 않는다. 모든 후보의 생산 실행은 미검증이다.

### 소스 조사 근거와 포팅 경계

| 근거 | 확인 내용 | 웹 설계 영향 |
|---|---|---|
| `C/internal/event/common/event-thread-services.cpp:42`, `thread-local-storage.cpp:56` | Core 이벤트 검사는 TLS/종료 상태 기반 | Worker 이벤트 가능성을 배제하지 않지만 전체 지원 증거도 아님. 생성·앱 조작·종료의 owner 일치 필요 |
| `A/internal/adaptor/common/adaptor-impl.cpp:773,1194,1611` | 이벤트 여부는 TLS 플래그. main thread라는 assert 문구도 실제로는 Adaptor TLS 검사 | 문자열상의 main과 OS/browser main을 구분. callback이 잘못된 스레드에서 Core를 호출하면 안 됨 |
| `A/internal/thread/common/thread-settings-impl.cpp:37`, `thread/linux/thread-settings-impl-linux.cpp:52` | UI thread 등록과 getpid 기반 main thread 조회가 별개 | 웹의 browser main/앱 event/render thread 식별 의미를 정의 |
| `A/internal/system/macos/callback-manager-mac.mm:168` | CFRunLoopGetMain에 observer 등록 | 실제 플랫폼 main-loop 결합이 존재. native callback을 Worker에서 그대로 재사용 불가 |
| `A/internal/adaptor/common/combined-update-render-controller.cpp:199,219` | pthread 생성, Start에서 semaphore 대기 | 메인에서는 비동기 ready/stop 통지로 전환. native wait loop를 그대로 실행하지 않음 |

이 조사는 주요 경로의 정적 조사이며 모든 main-thread 의존성을 감사한 결과는 아니다. callback/timer/trigger·TLS·초기화·종료 전체 경로 감사가 구현의 선행 검증이다.

### E 실행 계약

1. main에서 Core/Adaptor event 상태를 만들고 Actor 조작·입력·notification·ProcessEvents를 실행한다. render Worker만 GL context와 버퍼 매핑·shadow 갱신·GPU 업로드를 수행한다. 이벤트와 업데이트 간 기존 SPSC queue 소유권·메모리 순서를 유지한다.
2. DOM 접근과 DALi 이벤트 처리 사이에 웹 host 연결 경계를 둔다. 입력·resize·프레임 timestamp·종료 요청은 명시적 데이터로 전달하며 main 전용 브라우저 API를 Core/공통 이벤트 로직에 넣지 않는다. 현재 E에서는 같은 스레드 연결이 가능하고 후속 C에서는 메시지 전달로 대체할 수 있어야 한다.
3. main rAF가 프레임 timestamp/sequence를 전달한다. event 처리로 flush한 명령을 render owner가 소비하고 최대 한 pending tick으로 합친다. native SleepUntil 루프를 브라우저 main에 복사하지 않는다. notification은 비동기 wakeup 뒤 event owner에서 소비한다.
4. main이 render 준비·종료를 semaphore/condition/join으로 기다리지 않는다. ready/failure/stopped 상태를 비동기 통지하며 준비 전 앱 요청의 보류·실패 정책을 명시한다. main proxy와 render wait의 순환 대기를 금지한다. pause/resume/resize/정상 종료도 같은 규칙을 적용한다.
5. 긴 사용자 초기화/콜백/무한 루프는 main과 편집기를 막을 수 있다. E는 개발자가 제어하는 일반 앱의 기본이며 임의 코드의 중단 가능성을 보장하지 않는다. 별도 render Worker가 이 위험을 해결한다고 주장하지 않는다.

### 플레이그라운드 격리 조기 검증 게이트

P2의 생산 링크와 기본 초기화 경로가 확보된 뒤 P3/P4 구간에서 독립 검증 단위를 마련하고, **P4 종료 전 격리 가능성·장애·변경 범위를 기록한다.** 실행은 해당 단계의 명시적 지시로 진행하며 이번에 Worker 실험을 시작하지 않는다. 검증 실패가 기본 E의 픽셀 성공을 무효화하지는 않지만, 해결 방향 없이 플레이그라운드 구현으로 넘어갈 수는 없다.

1. C 후보에서 Worker를 이벤트 owner로 두고 Core/Adaptor 생성·입력·timer·callback·trigger·정상 종료·재생성을 감사/검증한다. main-thread ID API와 TLS, 플랫폼 loop, 앱 signal이 올바른 owner에서 동작하는지 확인한다.
2. 사용자 초기화 또는 callback 무한 루프를 넣고 편집기 main의 heartbeat·중단 버튼이 살아 있는지 검사한다. event Worker 하나만 종료하는 데 그치지 않고 소속 pthread·pending message·Canvas/context를 포함한 실행 인스턴스 전체 종료와 신규 실행을 검증한다.
3. iframe만으로 별도 thread나 확실한 중단을 보장하지 않는다. Worker 격리도 보안 sandbox 전체를 증명하지 않는다. 코드 컴파일 격리와 자원 제한은 P8/P9의 별도 계약이다.
4. 통과 전에는 C를 지원 구성으로 선언하지 않는다. 실패하면 원인과 대안을 보고하고 D3 확장 결정을 받는다. 이 검증을 P8/P9까지 미루지 않는다.

## 6. pthreads·격리·호스팅

2026-10-23 온라인 문서 확인. pthread는 compile/link 모두 `-pthread`이고 non-threaded Wasm과 별도 산출물이다. `PROXY_TO_PTHREAD`는 event app main을 옮기는 후보이며 모든 DALi thread와 GL 이동을 자동 해결하지 않는다. 준비된 pthread pool 및 main blocking 회피를 검증해야 한다. [Emscripten pthread 문서](https://emscripten.org/docs/porting/pthreads.html).

SAB 경로는 secure context, `crossOriginIsolated === true`, SharedArrayBuffer 사용 가능 여부를 실제 window/worker 양쪽에서 확인한다. 기본 서버안은 `Cross-Origin-Opener-Policy: same-origin`, `Cross-Origin-Embedder-Policy: require-corp`; 외부 리소스는 CORS/CORP 허용 필요. `credentialless`는 대안이며 브라우저·인증 자원 영향을 별도 검증한다. iframe의 Permissions-Policy와 ancestor 격리 조건도 확인한다. [브라우저 API 조건](https://developer.mozilla.org/en-US/docs/Web/API/Window/crossOriginIsolated).

| 호스팅 후보 | 조건·현재 판정 | 검증 방법 |
|---|---|---|
| GitHub Pages 직접 | 정적 사이트와 `/dali-web/` project 경로는 공식 지원. Pages의 사용자 정의 응답 헤더 설정은 확인한 공식 문서에 보장되지 않음. 기본 Pages를 pthread 가능 환경으로 승인할 근거 없음 | D1에서 실제 response headers, crossOriginIsolated/SAB, worker/wasm MIME·하위 경로·404 확인. 이번 배포·해당 URL runtime 검증 없음 |
| 직접 구축한 헤더 제어 호스팅 | Pages 우선 검증 실패 시 전환 검토. 타사 관리형 호스팅 서비스 제외, 서버 구축은 별도 승인. 실행은 isolated origin top-level을 우선 | 격리 헤더 서버에서 최소 pthread+OffscreenCanvas → DALi. Pages에서 top-level 링크로 이동. iframe 사용 시 별도 isolation 검증 |
| Pages + Service Worker 격리 | D4의 우선 검증 경로. 아직 지원 확정 아님. 최초 방문 reload와 SW 제어 범위 필요 | clean profile 최초/두 번째 방문, 업데이트·unregister·offline·cache·실패 반복 reload, Firefox/Chromium 검사 |
| Pages + A/B non-pthread | SAB 요구 없음. B는 OffscreenCanvas 별도 조건 | isolated=false에서도 GL/Worker가 동작하는지 확인, threading 산출물과 혼동 금지 |

[GitHub 공식 Pages 설명](https://docs.github.com/en/pages/getting-started-with-github-pages/what-is-github-pages)은 정적 호스팅·프로젝트 URL을 설명한다. 헤더 제어가 어려운 GitHub Pages에서의 SW 우회는 [coi-serviceworker 원저자 문서](https://github.com/gzuidhof/coi-serviceworker)가 제시한다. 이는 GitHub의 pthread 지원 보장이 아니다. HTML meta tag로 response COOP/COEP를 대체하지 않는다. 사용자 제약에 따라 다른 관리형 호스팅 서비스는 선택지에서 제외한다. 허용 후보는 GitHub Pages와 직접 구축한 호스팅뿐이다.

소규모 후속 검증안: header 없는 localhost와 COOP/COEP localhost 두 모드에 같은 최소 Wasm pthread 카운터+Worker GL context probe를 제공한다. 격리 없는 pthread는 명시 실패, 격리 환경에서 counter와 GL error 확인. 별도 non-pthread B를 비교한다. CPU counter/단순 clear는 DALi 렌더 증거가 아니다. 이 실험은 아직 실행하지 않았으며 커지면 독립 P1 검증 단위로 승인받는다.

### D4 확정: GitHub Pages 우선 검증, 직접 호스팅은 실패 시 검토

1. GitHub Pages + Service Worker 격리를 우선 검증한다. 이는 지원 가능성 검증 방향의 승인이지 배포 완료·runtime 호환성 보장이 아니다. D3 pthread 구조를 유지하며 Pages 제약 때문에 non-pthread로 조용히 변경하지 않는다.
2. 최초 pthread/browser 검증 단위에 작은 호스팅 probe를 함께 준비한다(P2 링크 진단의 선행 조건은 아님). 로컬에서 헤더 없는 응답+SW와 직접 COOP/COEP 응답을 비교한 뒤, 별도 배포 승인 하에 실제 Pages URL에서 검증한다. 로컬 성공만으로 Pages 지원을 선언하지 않는다. 전체 예제 배포 단계까지 최초 검증을 미루지 않는다.
3. 검증 항목은 window/worker의 crossOriginIsolated·SAB, pthread 생성/공유 카운터, Worker WebGL2 context, 최초 방문·재방문·SW 업데이트/해제·캐시·실패 재로딩 방지, Firefox/Chromium, `/dali-web/` 하위 Wasm/Worker/리소스 경로다. 단순 counter/clear 성공과 실제 DALi 렌더 성공을 구분한다.
4. 격리 실패·반복 reload·worker 시작 실패·업데이트 불안정 등 재현 가능한 문제가 남거나 우회 실험이 커지면 중단·보고한다. 승인 후 직접 구축한 정적 파일 서버와 COOP/COEP 설정으로 전환을 검토한다. 자동 서버 구축·외부 서비스 가입은 하지 않는다. 서버 환경·주소·운영은 전환 시 별도 결정한다.
5. HTML/JS/Wasm/Worker/resource URL은 설정 가능한 기준 경로로 관리하고, SW 등록은 웹 bootstrap/deploy 계층에 둔다. C++ 엔진은 호스팅 주소·SW에 의존하지 않는다. 전환 시 기존 SW·캐시·재방문 및 새 origin의 리소스 로딩을 검증한다. 실행 위치는 계속 사용자 브라우저다.

## 7. 사용자 결정과 완료 게이트

| ID | 결정 요청 | 권장 / 대안 | 미결 영향 |
|---|---|---|---|
| D1 확정 | 공통 GLES와 실행 환경 분리 | Native/EGL·Web 구현을 파일 단위로 분리하고 빌드 시 선택. Core 계약·기존 GLES/Vulkan/DYNAMIC 유지 | 설계 승인 완료. 구현·회귀는 P2 이후, 상세 파일·인터페이스는 위 경계 준수 |
| D2 확정 | 업로드 의미·비용 | 별도 웹 메모리 구현 + 보존이 필요한 버퍼 소유 CPU shadow. native Memory2/3 유지, Memory4 추가 없음 | 설계 승인 완료. 보존·수명·오류·메모리 비용 검증은 P2 이후 |
| D3 확정 | 생산 실행 주체와 격리 검증 | E: 메인 이벤트 + 업데이트·렌더 pthread Worker. DOM/이벤트 경계 분리, C는 조기 격리 검증 후보 | 설계 승인 완료. 비동기 대기 전환·runtime 검증 필요, 플레이그라운드 안전 중단 미보장 |
| D4 확정 | 격리 호스팅 우선순위 | GitHub Pages + SW 우선 검증. 실패/과도한 비용 시 직접 호스팅 전환 검토, 타사 관리형 서비스 제외 | 설계 승인 완료. Pages 실제 URL 검증·배포와 직접 서버 구축은 별도 승인/검증 대상 |

게이트 판정: **P1 설계 완료.** 호출 순서·변경 파일 경계·미지원 기능 정책·버퍼 보존·스레드 선택·호스팅 정책·후속 검증 방법을 기록했고 D1~D4 사용자 결정이 모두 확정됐다. P1을 막던 미결 설계 결정은 없다. 다음은 P2-a 최소 생산 GLES 링크 대상·의존성 진단이다.

| P1 완료 조건 | 충족 근거 | 후속 미검증 범위 |
|---|---|---|
| 현재 호출 순서·파일 경계 | 1~3절 소스·함수 추적과 D1 | 실제 초기화/해제·native 회귀 |
| 미지원 기능·메모리 의미 | 3~4절, D2 및 SDK guard 증거 | 실제 upload 보존·오류 경로·픽셀 |
| 실행 주체·동기화 | 5절 D3, main/TLS 조사 | pthread/OffscreenCanvas·비동기 시작/종료·플레이그라운드 격리 |
| 호스팅 조건과 실패 정책 | 6절 D4 | 실제 Pages·SW·격리 및 직접 서버 운영 |
| P2 범위·인수인계 | 8절 및 HANDOFF/NEXT-CONTEXT | 최종 source closure·생산 Wasm 링크 |

설계 완료와 구현 검증 완료를 구별한다. 미검증 결과는 P2/P3/P4/배포 및 플레이그라운드 게이트에서 확인하며 완료로 간주하지 않는다. 설계와 충돌하거나 예상 범위를 크게 벗어나는 결과가 나오면 중단·보고한다.

## 8. P2 최소 링크 인수인계 (결정 후 실행)

### 대상과 의존성

제안 target `gles-link-smoke`는 현재 Core static library + 선택한 생산 GLES 구현 + 최소 web host를 링크한다. 자동 테스트의 TestGraphicsController/TestGlAbstraction·전체 Tizen Adaptor·UI는 연결하지 않는다. 현재 `cmake/CMakeLists.txt`의 core-smoke가 가져오는 linker-test 보조 source 목록을 생산 target에 그대로 복사하지 않는다.

| 집합 | 출발 파일/영역 | 필요 이유·범위 |
|---|---|---|
| Core | `libs/dali-core/build/tizen`, generated namespace/invoke-method headers | R2 CMake 옵션 재사용. E용 pthread는 별도 build dir에서 Core도 재빌드 |
| Controller/렌더 자원 | `A/internal/graphics/gles-impl/file.list`의 cpp 23개를 출발 목록으로 사용 | controller/debug, context/state cache, command-buffer, buffer, Memory2/3, texture/sampler, framebuffer/render target/pass, shader/program/reflection/pipeline/cache, sync/dependency checker. 제외된 native sync/Memory3도 참조를 정리한 뒤 제외; 검증 없이 파일만 삭제하지 않음 |
| GL wrapper | `A/internal/graphics/gles/gl-implementation.cpp`, `gles2/3-implementation.h`, GL extension cache 관련 소스 | wrapper vtable·GLES3 함수 참조, shader prefix, capabilities. 미지원 API import 분리 필요 |
| host 경계 | 새 `A/internal/graphics/web/` 및 최소 `tests/probes/gles-link-smoke.cpp` 후보 | 실제 HTML5 context/GraphicsInterface/RenderSurface, host RenderController·PlatformAbstraction. 이름은 후보이며 public API 추가 아님 |
| sync/플랫폼 보조 | `gles-sync-pool`, `gles-texture-dependency-checker`, `egl-sync-object`, `gles/egl-sync-implementation`, `graphics/common` | C++ cast·RTTI/typeinfo 의존까지 추적해 native 환경 파일로 분리. 공통 GLES와 Web target에 concrete EGL/native fence 의존이 남지 않도록 검증 |
| shader/util | `graphics/common/shader-parser`, environment-variable, locale-numeric-guard, pixel-data, logging/trace, file-loader·program binary path·system-error | shader source compile/reflection에 필요한 실제 utility만. binary off여도 런타임 if만으로 file symbol 참조 제거가 보장되지 않으므로 compile boundary 필요 |
| Emscripten | libc/libc++, GLES headers·WebGL2 JS library; E의 pthread/worker runtime | libEGL native·X11·Wayland·Ecore·TBM·FreeType/HarfBuzz·이미지 codec은 초기 target에 추가하지 않음 |

이 목록은 소스 기반의 최소 **시작 closure**이지 검증 완료한 최종 linker closure가 아니다. Controller vtable·program/texture 의존성 때문에 R2-b 6개 파일만으로 충분하지 않다. P2 첫 산출물은 명시적 source manifest와 실제 undefined-symbol 분류다. 예상 밖으로 플랫폼 전체를 끌어들이면 범위를 늘리지 말고 보고한다.

### 예상 변경 파일

1. `cmake/CMakeLists.txt` 또는 새 `cmake/gles.cmake`, Adaptor `internal/graphics/file.list`, `gles-impl/file.list` 및 `build/tizen/CMakeLists.txt`: 공통/Native-EGL/Web source 집합과 환경별 link dependency 분리, 기존 GLES/Vulkan/DYNAMIC 구성 보존, 잘못된 조합 거부. 독립 target, source 명시, `MIN_WEBGL_VERSION=2`, `MAX_WEBGL_VERSION=2`, assertions, linker map, undefined-symbol 오류 유지. FULL_ES3 기본 off. pthread 선택은 compile/link 전파.
2. `A/internal/graphics/gles-impl/egl-graphics-controller.{h,cpp}` 및 관련 include: 공통 GLES Controller로 역할·이름 정리, concrete EGL cast/type 제거, context/cache와 환경별 계약 연결. native 구현을 별도 파일로 이동하며 Web 구현 추가. 공통 계약을 중복 생성하지 않음.
3. 새 웹 메모리 구현 파일(예: `web-buffer-memory.{h,cpp}`), 버퍼 소유 웹 저장소 및 필요 시 `gles-graphics-buffer.{h,cpp}`: D2의 bounds·수명·보존 계약 구현. 메모리 객체는 map/upload를 담당하고 shadow는 버퍼 수명에 귀속. native `gles2/3-graphics-memory`는 유지하며 웹에서는 선택하지 않는다.
4. `A/internal/graphics/gles/gl-implementation.{h,cpp}`, `gles3-implementation.h`, `gles-impl/gles-context.cpp`, `gles-graphics-program.cpp`: web capability와 금지 경로, source shader 유지.
5. `A/internal/graphics/gles-impl/gles-sync-pool.{h,cpp}`, `gles-texture-dependency-checker.{h,cpp}`, `egl-sync-object.{h,cpp}`: native fence 제외와 허용 GPU sync 의미. native 환경 파일과 Web sync 파일을 분리. 현재 GL sync 경로도 의미·호출 가능성을 확인한 뒤 재사용.
6. 웹 host의 callback/timer/trigger·thread ID·ready/stop 연결과 `combined-update-render-controller`의 웹 실행 경계: E의 main blocking 제거와 GL 소유권 보장. 네이티브 실행은 보존.
7. `tests/probes/gles-link-smoke.cpp`, 새 빌드/검증 script와 `docs/P2-RESULTS.md`: 재현·정확한 scope 기록. 기존 mock probe 보존. Core public API 변경은 현재 제안에 없음.

### P2 검증 절차와 이후 게이트

1. 결정 기록과 실제 Git/lock 확인, 새 `build/p2/<variant>`·`logs/p2/<run>` 사용. tools는 BUILDING의 변수로 선택, 최대 -j6, 기존 output 덮어쓰기 금지.
2. 명시적 source 집합을 object로 컴파일, `llvm-nm -u`·linker map을 보존. 심볼을 Core/utility/GL/EGL/native로 분류. R2 placeholder symbol 선언을 실행 ABI로 재사용하지 않음.
3. 실제 Controller를 생성·주입하고 Core 및 renderer/resource/command 경로를 참조하는 harness를 링크. unused archive만 붙이고 성공이라고 하지 않음. map에서 실제 GLES implementation 포함, mock 미포함 확인. undefined symbol suppression/빈 GL stub 금지.
4. 예상 미해결 7개뿐 아니라 새로운 GLES3.1·EGL·file/trace symbol을 감사. 기본 WebGL2 링크 exit 0과 source manifest/map을 남긴다. FULL_ES3 비교는 별도 variant이며 성공 여부를 구분.
5. native GLES/Vulkan 및 기존 DYNAMIC 구성의 source 선택·빌드 회귀와 웹 native 의존 배제 검사를 계획에 포함한다. 실행 가능한 native 환경에서는 자원·render·종료 회귀도 수행하며 실행하지 못한 항목은 미검증으로 남긴다. P2 최소 인수는 **선택된 생산 GLES path Wasm 링크 성공**이다. context bootstrap 실행은 가능하면 별도 기록하되 DALi 픽셀 성공으로 승격하지 않음. 수정된 native 공통 경로는 native compile 회귀를 수행하거나 미검증으로 남겨 범위 승인을 받음.
6. P3에서 실제 DALi shader/geometry로 배경+사각형 픽셀·GL error·업로드 보존·context 생성/해제 검증. P4에서 시간·입력·resize·프레임 재요청과 반복 start/stop. E의 실제 Worker 검증이 없으면 생산 실행 완료 판정 불가. C 격리 후보는 위 조기 게이트로 따로 검증.
7. 전체 Adaptor, 폰트/UI, Windows, 성능, 자동 context 복구, 배포는 별도 단계. 링크 오류를 풀기 위해 이 범위로 확대해야 하면 중단·보고.

## 9. 이번 실행 증거와 한계

[증거 안내](evidence/p1/README.md), [SDK guard 결과](evidence/p1/map-guards.json). 로컬 전용 `logs/p1/run-001/`에 명령·버전·보존 검사·Git 확인을 남겼다. 생산 빌드와 브라우저 실행은 없으므로 이전 50/100·6개 compile·7/3 unresolved 결과는 R2-b의 결과로 유지한다. SDK guard 검사 통과는 문제 확인의 성공이며 Memory3 사용 가능 판정이 아니다.
