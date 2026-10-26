# P2-b 생산 WebGL2 링크와 host 구현

2026-10-23 (Asia/Seoul). **실제 Core·생산 GLES Controller·자원·명령·D3 host를 포함하는 기본 WebGL2 strict 링크를 확보했다.** P2의 생산 링크 마일스톤과 P2-b 구현 범위를 완료했다. 브라우저 실행·GPU 보존·픽셀·실제 비동기 종료를 완료했다는 뜻은 아니다. 이 실행 인수는 P3/P4에 남긴다.

시작/현재 HEAD `abf62214b23c651f06536138cb8b6e0c0fddbfb7`. 기존 Core internal·Web 메모리 working tree를 이어서 수정했고 commit/push하지 않았다. GitHub 읽기 조회 원격 main은 `a68e3f00f0c7752561137c179500b004b930285e`. sandbox Git 조회는 DNS 제한으로 실패했으며 연결된 GitHub GET으로 확인했다. upstream·lock·과거 증거·원본 저장소를 갱신하지 않았다.

## 구현

1. 실제 생산 Controller의 CreateBuffer/TryRecycle, Memory factory, create/discard, command 제출과 Core UniformBufferV2를 연결한 CPU 통합 검사를 추가했다. Buffer/Storage/Memory 객체 자체의 nothrow 실패, custom allocator 실패, GL allocation/BufferData 실패, grow/recycle, 매핑 중 교체, Core CPU/GPU uniform 실패, Memory 보유 shutdown을 검사한다. 저장소 단위 테스트를 factory 통과 근거로 사용하지 않았다.
2. **특이 사항:** shutdown 이후 custom allocator 객체를 일반 delete로 해제하던 공통 GLES deleter 결함을 재현했다. allocation callback으로 해제하도록 수정했고 SyncDeleter도 같은 계약을 적용했다. factory는 alloc/free 쌍 누락을 거부한다. native의 생성 재시도 정책은 유지한다. native 실행 회귀는 아래 제한을 따른다.
3. `WebGlImplementation`을 별도 파일로 선택한다. 실제 WebGL2 context 속성과 GLES 3.0 대응·limits를 확인한다. 일반 GLES/WebGL2 호출은 SDK GL로 전달하고 map/advanced blend/binary/native external texture/MSRTT·초기 범위 밖 compressed texture 요청은 실패를 latch한다. 이전 오류가 생기면 뒤따르는 wrapper 호출과 Controller 제출을 차단한다. 지원되지 않는 import를 빈 GL 함수로 해소하지 않는다.
4. shader binary cache를 공통 Program에서 native/Web backend 함수로 분리했다. native 파일·버전 naming·load/save 동작은 native 파일에 유지한다. Web은 cache capability false이고 명시적인 binary cache 호출도 실패한다. source shader compile/link/reflection은 실제 생산 구현이다. Web source closure에 FileLoader·native cache path·Adaptor version 대체 정의를 추가하지 않았다.
5. `web/host`는 실제 RenderController, PlatformAbstraction timer, GraphicsInterface, RenderSurface, sync를 제공한다. browser main이 Core 생성/TLS·Scene/Actor·ProcessEvents·DOM을 소유하고, 별도 pthread가 context·Update·upload·render·fence를 소유한다. 실제 사각형 geometry/source shader/Renderer 생성 경로가 링크에 포함된다. 픽셀 결과는 아직 실행하지 않았다.
6. main/Worker의 release/acquire mailbox로 ready·frame 완료·failure·stop을 연결했다. main은 semaphore/join으로 기다리지 않는다. owner가 WebFailure::Reason을 읽어 main에 게시하고, main은 입력/rAF/timer를 중단한 뒤 event 객체 정리 → Worker의 최종 update/ContextDestroyed/Controller shutdown·fence cancellation → main Core/TLS 해제 → Worker context 폐기·pthread 종료 → main의 비차단 tryjoin 및 pooled Worker 종료 순서로 처리한다. 실패 후 같은 모듈을 다시 시작하지 않으며 명시적인 새 실행은 새 Module/Worker/Canvas를 사용한다.
7. **특이 사항:** 현재 host는 correctness 검증용 mailbox polling(main 8ms/Worker 4ms)을 사용하며 최대 한 frame만 진행한다. 성능 최적화·프레임 지연/숨김/재개 정책의 완료 근거가 아니다. 범용 앱·플레이그라운드 격리·모든 플랫폼 서비스로 확대하지 않았다. JS runtime abort와 전체 Core/STL 전역 OOM은 정상 비동기 종료 보장 밖이다.
8. `cmake/web`와 실제 graphics file.list를 사용하는 정식 최소 생산 target을 추가했다. 전체 Tizen Adaptor WEB profile은 계속 거부한다. Core namespace/invoke-method는 binary directory, native shader/system cache source와 pkg-config 출력도 binary directory로 이동했다. 실제 Adaptor version source는 유지하며 임의 version 상수를 만들지 않았다. SDK 경로는 env/PATH로 선택한다.

공개 Graphics Controller/Memory 시그니처·vtable 변경 없음. native Memory2/3 유지, Memory4 없음. 기존 Core bool은 CPU 준비 결과이며 void Unlock/Unmap 이후 GL 업로드 성공 또는 GPU 완료를 뜻하지 않는다. `-sABORTING_MALLOC=0`은 최종 host 링크에도 명시한다.

## 검증과 증거

[생산 링크 증거](evidence/p2-b-production/README.md), [첫 Controller 통합 checkpoint](evidence/p2-b-controller-memory/README.md). 명령·버전·각 compile/nm/link/Node 종료 코드와 입력 hash를 보존한다. 기본 WebGL2는 FULL_ES3=0, undefined 오류 활성화이며 생산 링크에는 테스트 GL/Controller·linker wrap을 넣지 않는다.

| 검사 | 결과 | 판정 범위 |
|---|---|---|
| production-003 | 새 pthread Core 333단계·생산/host 35개 compile/nm 및 기본 strict 링크 각각 0 | 실제 host 포함, 브라우저 실행 아님 |
| controller-007 | 생산 33개 compile/nm·테스트 링크·Node18모드 각각 0 | 실제 Buffer/Controller/Core uniform + GL 주입 CPU 검사 |
| gl-unit-002 | compile/link·Node5모드 각각 0 | 일반 blend/draw 호출, 미지원 map/blend/binary/cache 및 GL 오류 후 draw 차단 |
| production-sources-001 | GLES/VULKAN/DYNAMIC source 회귀 0 | native cache 추출 추가, Vulkan 선택 목록 보존 |
| closure-002 | Web 정식 source 목록·독립 native generated 경로 검사 0 | 전체 native configure/build 성공 근거 아님 |
| production-native-002 | native configure 3개 각각 1 | host C/C++ compiler 부재 |
| 같은 native 검사 | 추출/공통 13개 Emscripten compile/nm 각각 0 | native build/install/runtime 아님 |

최종 host의 idle 이벤트 처리까지 반영한 production-003 결과는 증거의 `result.json`과 명령을 기준으로 한다. production-001/002와 이전 실패 산출물은 덮어쓰지 않았다. 전체 linker map은 약 21MB로 로컬 로그에 보존하고 공개 증거에는 hash·선택 경로 발췌를 둔다.

초기 controller-001/002는 테스트 include/API 오류, 003은 주입 GetIntegerv 미설정, 004는 allocator 해제 결함 재현이다. 005는 12모드, 006/007은 확대된 18모드 통과다. wrapper-001의 MSRTT 미지원 함수 컴파일 오류는 명시적 거부로 수정했다. native-001의 추출 cache GL 정의 include 누락과 closure-001의 CMake cache 옵션 설정 오류도 보존하고 수정 후 통과했다.

## 미검증 범위와 다음 게이트

1. 실제 browser의 main/Worker·OffscreenCanvas·비동기 failure/stop/재생성, GPU 업로드 보존·uniform·shader·픽셀/readback·context loss는 실행하지 않았다. host 코드와 strict 링크를 이 실행 증거로 확대하지 않는다. Node 테스트의 GL 응답 배열은 GPU가 아니다.
2. native build/install/runtime, 자원·create 재시도·custom allocator 종료·shader cache load/save의 실제 GL/파일 실행은 compiler 부재로 미검증이다. 영향은 공통 factory/deleter/queue·Program cache 경계·generated source 선택과 기존 Core internal render 실패 처리다. Vulkan 본체는 수정하지 않았으나 공통 Core 변경과 generated cache 경로는 해당 backend에도 회귀 대상이다.
3. 다음은 P3 실제 DALi background/사각형 픽셀과 GL 오류, 16-byte/중간4-byte·callback0/부분/전체·nonzero offset·CPU/GPU uniform·두 frame·grow/recycle·수명 보존 검사다. 최초 pthread/browser에서 로컬 헤더/SW 비교를 함께 수행한다. 실제 Pages 검증은 별도 승인이다. GitHub Pages+SW 우선, 실패/과도한 비용 시 직접 호스팅 검토, 다른 관리형 호스팅 금지.
4. P3/P4의 이벤트 Worker 플레이그라운드 격리 조기 게이트, P4 종료 전 무한 루프 응답성·전체 pthread/Canvas 종료·재생성 검증을 유지한다. main 이벤트 모델 E가 임의 사용자 코드의 격리/중단을 보장하지 않는다.

이번에는 서버 구축·브라우저 실행·Pages 배포·upstream 갱신·SDK 설치·lock 갱신을 하지 않았다. 빌드는 최대 -j6이고 SDK·산출물·프로필·대용량 로그는 commit 대상이 아니다.
