# P3/P4 이벤트 Worker 격리 조기 검사

후속: 사용자가 app 이벤트 pthread + window 종료 감독 방식의 구현을 승인했고 [C 조기 검사 행렬](P3-EVENT-PTHREAD-RESULTS.md)을 완료했다. 아래 중단/미검증은 그 이전 dedicated Worker 후보의 기록이다.

2026-11-06. **기본 E 재실행 통과. C의 무수정 모듈 이동 후보는 Canvas 전달 단계에서 실패하여 중단했다. 격리 게이트는 미통과이며 P3는 진행 중이다.** [작은 증거](evidence/p3-event-worker/README.md).

## 기준과 구현 범위

시작 HEAD와 실제 `git ls-remote origin refs/heads/main`은 모두 `c98c04796b0b5ebe3c0029a6e37260bd13d6ffb6`, 작업 트리는 clean이었다. origin은 `https://github.com/dalihub/dali-web.git`이다. 첫 sandbox 원격 조회의 DNS 실패는 확장된 읽기 실행으로 해결했다. 과거 미커밋 기록보다 이 상태를 우선한다.

1. 생산 E, Core/Adaptor, SDK/lock, 공개 API는 수정하지 않았다. `tests/browser/check-production.py --event-worker`와 별도 fixture 3개를 추가했다. 실패를 성공으로 변환하지 않으며 C 시작 실패는 exit 1이다.
2. 기존 검증된 3.1.24 `build/p3/production-memory-final-001`의 계측 없는 JS/Wasm/pthread Worker를 그대로 사용했다. 재빌드·객체 혼용 없이 입력 hash를 기록했다. 새 로그/프로필만 만들었으며 과거 산출물은 덮어쓰지 않았다.
3. 이번 C 시작 후보는 window에서 Canvas를 dedicated Worker로 전달하고, 그 Worker를 **Wasm main-runtime/event owner**로 삼아 기존 host가 render pthread를 만들도록 하는 방식이다. P1 표의 app-pthread 방식(`PROXY_TO_PTHREAD`) 자체를 구현한 것은 아니다. C 전체의 불가능성을 판정하지 않는다.
4. DOM 접근은 window에만 두고 worker에 OffscreenCanvas와 시작/중단 메시지를 전달했다. SDK 내부 `GL.offscreenCanvases` 주입, 가짜 DOM 메서드, GL proxy/fallback, SDK 수정은 하지 않았다.

## 실행 결과와 개별 판정

Firefox 157.0.1, loopback `/probe/`, 직접 COOP/COEP. 다음은 서로 다른 판정이다.

| 항목 | 근거와 결과 | 판정 |
|---|---|---|
| E 기준 실행 | `event-worker-e-baseline-001`, 오류 없음, sharedMemory/isolation=true, ready→stopped, exit 0 | 기존 E 시작/정상 종료 통지 재확인. 새 픽셀·누수 검사 아님 |
| C 초기 fixture | `event-worker-c-startup-001`, nested pthread loader의 URL.createObjectURL TypeError, exit 1 | fixture의 `mainScriptUrlOrBlob` 누락. 명시 URL을 넣고 재실행, SDK 수정 없음 |
| C 시작 | `event-worker-c-startup-002`, window/worker isolation=true, sharedMemory=true, failure→stopped, exit 1 | `render pthread/Canvas transfer failed`. ready 없음, Core 생성 전 실패 |
| Core/TLS·이벤트 소유권 | 아래 정적 감사만 수행. Core 생성·Actor·입력·timer callback·trigger·signal 실행에 도달하지 못함 | 런타임 미검증 |
| 무한 루프 중 main 응답성 | 시작 중 heartbeat 3회는 관측했지만 사용자 초기화/콜백 무한 루프는 넣지 않음 | 미검증. 짧은 시작 응답성을 무한 루프 격리로 확대하지 않음 |
| 전체 pthread/Canvas 종료 | 실패 host가 stopped를 통지, fixture가 event Worker terminate와 DOM Canvas 제거 요청. 브라우저/driver/서버는 실행 후 종료 | 소속 pthread 실행 중단·pending message 무효화·GL 자원 회수의 독립 계측 없음. 미검증 |
| 새 실행 인스턴스 | 같은 페이지에서 새로운 Module/Worker/Canvas의 정상 프레임을 확인하지 못함 | 미검증. 별도 Firefox 프로필 실행을 재생성 증거로 사용하지 않음 |
| 보존 | 과거 lock/산출물 748개, 최종 생산 입력 499개, 사용한 runtime 3개 hash drift 0 | 기존 기준 보존 |

## 특이 사항: Canvas 전달 실패 원인과 중단

실제 Worker 관측은 `OffscreenCanvas=true`, `transferControlToOffscreen=undefined`, `document=undefined`, `requestAnimationFrame=function`이었다. 브라우저의 OffscreenCanvas 미지원이 아니다.

고정 SDK `src/library_pthread.js`의 `__pthread_create_js`는 `GL.offscreenCanvases[name]`에 등록된 Canvas를 우선 전달한다. 그 entry가 없고 `!ENVIRONMENT_IS_PTHREAD`이면 `Module.canvas`를 DOM Canvas처럼 처리하면서 `transferControlToOffscreen()`을 요구한다. 일반 dedicated Worker는 Wasm main runtime이므로 이 분기에 들어가지만, 이미 전송된 OffscreenCanvas에는 해당 DOM 메서드가 없다. SDK의 “browser does not support OffscreenCanvas” 진단은 이 경우 원인을 정확히 설명하지 않는다. 이 실패는 실제 출력과 고정 SDK 소스 분기로 확인했다.

host의 `Start()`가 pthread 생성 실패로 반환하여 `CreateScene()`/`Core::New()`에 도달하지 못한다. `stopped`는 시작 실패 처리 결과이며 render context를 생성하고 정상 회수한 증거가 아니다.

[P1 실행 계약](P1-EXECUTION-CONTRACT.md)의 조기 게이트는 “통과 전에는 C를 지원 구성으로 선언하지 않는다. 실패하면 원인과 대안을 보고하고 D3 확장 결정을 받는다”라고 규정한다. 이 실패 이후 SDK 내부 우회·GL proxy·host 스레드 계약 변경으로 진행하지 않았다. 직접 헤더가 이미 격리된 상태에서 발생한 실패이므로 SW/호스팅 전환으로 해결됐다고 가정하지 않으며 SW 추가 실행도 중단했다.

## 소유권 정적 감사

1. Core `internal/common/core-impl.cpp`의 생성자는 `CreateThreadLocalStorage()`를 호출하고 소멸자는 현재 스레드 TLS를 제거한다. `internal/event/common/thread-local-storage.cpp`의 저장소와 shutdown flag는 `thread_local`이다. `event-thread-services.cpp::IsEventThread()`는 TLS Created/IsShuttingDown을 확인한다. 따라서 생성·Actor 조작·ProcessEvents·해제를 같은 event owner에서 수행해야 한다. 이는 후보 가능성의 근거이며 Worker 실행 성공의 증거는 아니다.
2. `web/host/web-host.cpp`의 Start/Stop/Platform timer는 `emscripten_is_main_runtime_thread()`를 assert한다. SDK `system/lib/pthread/emscripten_thread_state.S`는 이 조건을 `!ENVIRONMENT_IS_PTHREAD`로 설명하며 browser-main 조건과 구분한다. dedicated runtime Worker와 app pthread는 이 assert에 대해 서로 다르다. `PROXY_TO_PTHREAD` 옵션만 추가하면 기존 owner 계약이 자동으로 성립한다고 볼 수 없다.
3. 기존 host는 ready mailbox 뒤 Core/Scene 생성, event owner의 rAF/ProcessEvents, render pthread의 Update/GL, ResourcesStopped 뒤 Core 삭제, render context 폐기와 tryjoin 순서다. Platform timer callback도 해당 runtime의 timeout에서 실행한다. 이번 C 실패는 이 흐름보다 앞이다. DOM 입력/resize와 trigger·앱 signal, 숨김/재개까지 모두 검증한 host는 아니다.
4. 전체 native Adaptor는 링크하지 않았다. `adaptor-impl.cpp`의 별도 TLS `gIsEventThread`, ThreadSettings의 UI thread 등록과 Linux `getpid()` 기반 main ID, 플랫폼 loop/callback은 별도 감사 대상이다. 최소 host의 결과를 전체 Adaptor Worker 지원으로 확대하지 않는다.

## 다음 결정과 후속 검증

아래는 제안이며 구현 승인이나 작업큐 등록이 아니다.

1. 권장 검토 방향은 **독립 C 테스트 target에서 app event pthread를 사용하고 window가 실행 수명과 모든 pthread 종료를 감독하는 방식**이다. event-owner assert/타이머, main rAF의 데이터 전달, Canvas 전달, 정상 종료와 강제 중단을 먼저 설계한다. 기본 E 및 공통/public 계약을 유지한다. `PROXY_TO_PTHREAD`는 검토할 수 있지만 자동 해결책이나 검증된 구현은 아니다.
2. 대안은 dedicated runtime Worker의 Canvas를 고정 SDK의 offscreen registry/공유 크기 정보와 연결하는 별도 adapter다. SDK 내부 결합과 nested pthread 전체 강제 종료의 관측·제어 부담이 있어 바로 구현하지 않는다. SDK 교체·가짜 DOM/빈 GL·main proxied GL로 조용히 전환하지 않는다.
3. 사용자가 D3 확장 범위를 결정한 뒤 정상 프레임·event-owner/TLS 생성/해제·입력/timer/callback/trigger를 먼저 확인한다. 이어 초기화와 callback 각각의 무한 루프에서 main heartbeat/실제 중단 동작을 계측한다. 모든 소속 pthread의 shared counter 정지·pending message generation 차단·Canvas 제거/정상 context 폐기, 새 Module/Worker/Canvas 정상 픽셀까지 별도 판정한다. 강제 종료는 C++ 소멸자 정상 실행과 구분한다.

P3 진행 중, P4 전체·P8/P9 완료 아님. SW 재방문/업데이트/해제/캐시 정책, Chromium, context loss/OOM, 전체 누수와 native runtime은 미검증이다. 공통/native 코드 변경이 없어 native build/runtime은 재실행하지 않았다. 기존 native 전체 빌드의 설치 Core 헤더 부재 한계는 유지한다. 서버 없는 Emception 3.1.24 방향, GitHub Pages+SW 우선과 배포 별도 승인 조건을 유지한다. commit/push·upstream 갱신·운영 서버·배포 없음.
