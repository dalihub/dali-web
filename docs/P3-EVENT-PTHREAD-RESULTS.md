# P3/P4 app 이벤트 pthread 격리 구현 결과

2026-11-06. 사용자가 권장한 app 이벤트 pthread + window 종료 감독 방식의 구현을 승인했다. **Firefox 로컬 직접 헤더/SW에서 C 조기 검사 행렬을 통과했다. 기본 E는 유지한다.** 이전 [dedicated Worker 시작 실패](P3-EVENT-WORKER-RESULTS.md)의 중단 상태는 이 승인과 별도 C 구현으로 후속 처리했다. C 전체 지원·P3/P4 전체 완료 선언은 아니다. [증거](evidence/p3-event-pthread/README.md).

## 구현

1. `cmake/web`에 기본 OFF `DALI_WEB_TEST_EVENT_PTHREAD`와 독립 `dali-web-event-probe` target을 추가했다. 기본 `dali-web`은 기존 E다. C test TU가 같은 실제 host/Controller/Core를 사용하되 테스트 define으로 소유권·mailbox·계측만 바꾼다. 공통 Core/Adaptor·SDK·public API·lock은 수정하지 않았다.
2. window가 Module/pthread pool과 DOM Canvas를 소유하고 준비된 pool에서 이벤트 pthread를 생성한다. Canvas는 SDK의 공식 pthread attribute로 window→event pthread→render pthread에 전달한다. SDK가 유지하는 offscreen registry를 사용하므로 이전 일반 Worker의 DOM 메서드 요구 분기를 피한다. registry 직접 주입, SDK 수정, `PROXY_TO_PTHREAD`, main proxied GL, 빈 GL/undefined 억제는 사용하지 않았다.
3. 이벤트 소유권은 C에서 `pthread_equal(eventProbeOwner, pthread_self())`, E에서 기존 `emscripten_is_main_runtime_thread()`로 검사한다. C event/render/browser-main ID가 서로 다르고, main/render에는 Core TLS가 없으며 event에서 생성·timer/key signal·해제 시 TLS 상태를 assert한다.
4. main rAF는 timestamp와 pending flag만 atomic mailbox로 전달한다. event owner가 tick/입력/trigger/중단 요청을 소비하고 기존 event→render mailbox와 최대 한 frame 흐름을 유지한다. 테스트 입력은 DOM keydown→정수 key code→Scene.QueueEvent→ProcessEvents→실제 DALi KeyEventSignal이다. timer와 trigger는 event owner에서 Actor 속성을 조작한다. 범용 입력/resize/플랫폼 trigger 구현은 아니다.
5. 정상 종료는 event scene 정리→render 자원/fence shutdown→event Core/TLS 해제→render context 폐기/exit→render tryjoin→event exit→window tryjoin/pool terminate→Canvas 제거 순서다. window는 blocking join을 사용하지 않는다.
6. 강제 종료는 window가 해당 Module의 running/unused Worker 전체에 terminate를 호출하고 Module과 Canvas를 버린다. 멈춘 코드가 allocator lock을 잡고 있을 수 있으므로 이 경로에서는 SDK의 pthread 데이터 free나 C++ 소멸자를 호출하지 않는다. 정상 pool 회수와 구분한다. 예외 시 정리도 강제 경로를 사용한다.
7. fixture는 실행마다 새 Module/공유 메모리/Canvas를 만들고 이전 generation의 상태/픽셀 메시지를 폐기한다. window의 중단 버튼 handler를 통해 정상 중단 또는 강제 종료를 요청한다. 테스트는 synthetic keydown과 programmatic button click을 사용하며 실제 사용자 입력 장치 테스트는 아니다.

## 검증

원본 Emception과 같은 릴리스 계열 Emscripten3.1.24, 고정 profile/lock, EMCC_CORES=6·최대-j6. 모든 build/log는 새 경로다.

| 검사 | 결과·근거 |
|---|---|
| 별도 C probe 초기 빌드 | 동일3.1.24의 검증된 renderer/Core를 재사용, host compile/nm 및 strict 링크0. 최종 재사용 단위 `event-pthread-004` |
| 새 전체 CMake 빌드 | `event-pthread-cmake-001`, 새 Core·renderer·E/C 두 target 총371단계, configure/build0. 기본 WebGL2/FULL_ES3=0, undefined 오류 유지 |
| 최종 C 직접 헤더/SW | `event-pthread-final-{headers,sw}-001`, 각각 exit0·오류 목록 없음. 같은 페이지에서 정상→초기화 무한 루프→timer callback 무한 루프→새 정상 실행의4회 행렬 |
| Core/TLS·소유권 | main/event/render 서로 다른 thread ID. event TLS 생성, timer/입력 signal에서 owner/TLS assert, 정상 종료에서 TLS 제거. 정상/콜백 실행의 key signal 실제 keyCode65 |
| 실제 렌더 | 정상2회와 callback 전1회, 각2프레임. 배경 `[13,25,51,255]`, 사각형 `[229,76,25,255]`, GL error0. 새 인스턴스에서도 일치 |
| 초기화 무한 루프 | Core/scene 구성 후 ready 전 C++ 무한 루프. shared spin counter와 render tick이 계속 변하는600ms 동안 main heartbeat 지속. ready/정상 stopped를 요구하지 않음 |
| callback 무한 루프 | 정상 픽셀·key signal·trigger 후 실제 Platform timer callback에서 C++ 무한 루프. main heartbeat 및 render tick 지속 |
| 전체 실행 중단 | 각 실행에서 등록된2개 Worker 모두 terminate 요청, Canvas DOM 제거.150ms 대기 뒤350ms 동안 event/render/spin 및 mailbox 카운터 전체 불변. 단순 stopped 통지와 별도 검증 |
| 재생성·낡은 메시지 | 새 shared memory와 Canvas identity 확인. 두 강제 종료 뒤 새 정상 실행의 픽셀/TLS 해제 확인. 종료된 receiver에 지연 메시지를 주입해 상태 변경 차단 확인 |
| SW | 첫 방문 isolation=false, 제어/reload 뒤 true. 같은 C 행렬 통과. 업데이트/캐시 정책 검사는 아님 |
| E 픽셀 회귀 | 새 전체 빌드 `event-pthread-e-regression-001`, 실제 두 프레임·GL error0·ready→stopped |
| 계측 없는 E | `event-pthread-e-production-002` host compile/nm/strict 링크0, `event-pthread-e-production-browser-002` 오류 없이 ready→stopped |
| source 회귀 | native GLES/VULKAN/DYNAMIC 집합 및 Web closure·독립 generated source 검사0. native 전체 build/runtime 재실행은 하지 않음 |

본 행렬에서 전체 실행 종료는 **해당2개 pthread의 실행 정지, main 일정 중단, Canvas 제거 및 새 실행 가능성**을 의미한다. 정상 context 폐기 호출은 별도 flag로 확인했다. 강제 종료는 Core/TLS 소멸자·GL 삭제 호출이 실행됐다는 뜻이 아니며, 브라우저 내부 GPU 메모리 즉시 회수·누수 없음·GC 시점까지 증명하지 않는다. Worker가 실행 중이던 점은 spin/render counter 변화로, 중단 후 정지는 별도350ms 관측으로 확인했다.

## 특이 사항과 중간 검사

1. 이전 dedicated Worker의 Canvas 실패를 SDK 수정 없이 해소했다. 승인한 app pthread 방식으로 실행 주체를 분리했으며 E를 C로 교체하지 않았다.
2. 첫 C fixture 실행은 EM_JS import를 Module export로 직접 부르는 테스트 오류로 실패했다. KEEPALIVE C wrapper를 추가해 수정했다. `event-pthread-001/002`와 `event-pthread-headers-001`은 중간 자료다. `003`은 입력 signal 보강 전 성공, `004`는 보강 후 성공이며 최종 인수는 새 전체 CMake 산출물을 사용한다.
3. 계측 없는 E 검사 보조 명령의 nm 실행 파일을 잘못 EMXX로 지정해 `e-production-001`에서 nm1, 산출물 없는 browser-001도1이었다. 올바른 LLVM_NM으로 새002 경로에서 compile/nm/link/browser 모두0. 실패 결과는 보존했고 성공 집계에 넣지 않는다.
4. main은 종료된 Module의 allocator를 호출하지 않는다. 이는 강제 종료 경로의 설계 선택이다. 실제 allocator 잠금 중 hang, 추가 동적 pthread, GPU 드라이버 hang, main runtime의 동기 proxy 중 무한 루프까지 검증한 것은 아니다.

## 상태와 다음 작업

이번 승인 범위인 **독립 C 후보의 Core/TLS·이벤트 소유권, 두 무한 루프 중 main 응답성, 소속 pthread 종료와 새 인스턴스 재생성 조기 검사**는 로컬 Firefox 직접 헤더/SW에서 완료했다. 별도 C 테스트 구성으로 유지하며 범용 플레이그라운드 지원 선언은 하지 않는다. P3는 진행 중이다.

1. SW 재방문·업데이트·해제·실패·캐시 정책 게이트가 남는다. GitHub Pages+SW 우선, 실제 Pages 검증·배포는 별도 승인이다.
2. P4 프레임/숨김·재개, 포인터 입력/resize, 실패·반복 수명 검증은 별도다. C supervisor를 실제 앱 인터페이스로 일반화하는 작업은 이번 테스트 구현에 포함하지 않는다.
3. Chromium/타 OS, 추가 pthread·SPSC stress, 전체 native Adaptor 서비스/TLS·main ID·platform loop, context loss/OOM·장기 누수·컴파일 격리·자원 제한은 미검증이다. native source 집합은 유지하며 native 전체 build/install/runtime의 이전 Core 설치 헤더 부재 한계도 유지한다.
4. 원본/과거 SDK·lock·산출물·실패 증거 보존. 서버 없는 Emception3.1.24 컴파일 방향 유지. commit/push·upstream 갱신·운영 서버·배포 없음.
