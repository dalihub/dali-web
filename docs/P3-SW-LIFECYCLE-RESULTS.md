# P3 SW 수명주기 후속 검사

후속: 사용자 승인으로 [bootstrap 복구 단위](P3-SW-RECOVERY-RESULTS.md)를 완료했다. 아래는 수정 전 실패 기록이며 현재 승인 대기가 아니다. 운영 캐시 등 전체 SW 게이트는 남는다.

2026-11-06. **SW 해제 후 같은 탭 재진입 실패를 재현해 P1 D4에 따라 중단했다. P3는 진행 중이며 SW 게이트는 미통과다.** [증거](evidence/p3-sw-lifecycle/README.md).

## 기준과 검사 범위

시작 HEAD·실제 origin/main은 `99d21f3992c20d60dc0396eda23e43402d2f4842`, 작업 트리는 clean이었다. sandbox DNS 실패 후 읽기 전용 원격 조회로 확인했다. 과거 미커밋·C 승인 대기 기록보다 이 상태를 우선했다. 상위 AGENTS와 NEXT-CONTEXT/HANDOFF의 필수 문서·lock을 확인했다.

새 `tests/browser/check-sw-lifecycle.py`가 기존 `production.html/js/sw.js`를 새 로그의 `/probe/`에 복사하고 Firefox157.0.1에서 순차 검사한다. 기본 E의 기존 동일3.1.24 `build/p3/event-pthread-cmake-001` 산출물을 사용했다. 업데이트는 served SW 복사본에 주석만 추가한다. 원본 bootstrap/SW·C++·공통/native·SDK·lock은 수정하지 않았다. 로컬 loopback 정적 파일 제공만 사용하며 외부 요청은 프록시/CSP로 차단한다. 운영 서버나 컴파일 서버를 도입하지 않았다.

## 실제 결과

| 항목 | 관측·판정 |
|---|---|
| 최초 방문 | isolation=false→SW/reload→true. sharedMemory/controlled=true, 실제2프레임·GL error0·ready→stopped |
| 같은 탭 재방문 | scope 밖 페이지를 거쳐 돌아와 격리·공유 메모리·실제2프레임·정상 종료 유지 |
| SW 업데이트 | register.update로 byte 변경 감지, controller identity 변경·activated. 재진입 실제2프레임·정상 종료 |
| 업데이트 실패 | SW script 응답503 주입, update Promise 오류·기존 controller 유지·activated. 오류 주입 해제 뒤 재진입 실제2프레임·정상 종료 |
| 리소스 실패 | 없는 Wasm 요청의404 보존, 연결 끊김은 fetch NetworkError. 오류를200 성공으로 변환하지 않음 |
| 캐시 관측 | 전후 CacheStorage keys=[]; Worker 응답200/text/javascript, COOP same-origin·COEP require-corp. 로컬 응답 Cache-Control 없음, registration.updateViaCache=imports |
| SW 해제 | unregister=true·등록 수0. 현재 문서는 여전히 controlled/isolated=true. scope 밖으로 이동하면 false/false |
| 해제 뒤 재방문 | `Error: SW reload did not isolate`, 이벤트/픽셀 없음. 같은 탭 추가 navigation에서도 동일 실패. 전체 exit1 |

네 번의 정상 실행은 각각 실제 두 프레임의 배경 `[13,25,51,255]`, 사각형 `[229,76,25,255]`와 GL error0을 검사했다. 실패 검사 이후 기존 정상 E 결과를 무효화하지 않는다. C 행렬은 재실행하지 않았으며 기존 독립 C 조기 검사 성공은 유지한다.

## 특이 사항·중단 이유

`tests/browser/production.js`는 최초 비격리 방문에서 `sessionStorage.reloaded=1`을 기록한다. 격리 성공 뒤 이를 지우지 않는다. SW를 해제하고 해당 문서를 떠난 뒤 같은 탭으로 재진입하면 저장된 flag가 남아, register 호출 전에 오류를 던진다. 무한 reload는 막지만 새 등록 시도까지 막는 시험용 bootstrap 수명 문제다. SW/브라우저 자체의 격리 불가능이나 Pages 실패를 증명한 것은 아니다.

[P1 D4](P1-EXECUTION-CONTRACT.md)의 “격리 실패·반복 reload·worker 시작 실패·업데이트 불안정 등 재현 가능한 문제가 남거나 우회 실험이 커지면 중단·보고한다” 조건에 해당한다. 증거 기록 외 복구 구현·정책 교체·후속 행렬 확대는 하지 않았다. 직접 호스팅으로 전환하지 않았다.

`sw-lifecycle-001`은 sandbox socket 생성 권한 실패로 브라우저 실행 전 종료했다. 새002에서 승인된 로컬 검사를 실행했다. 이 권한 문제와 실제 SW 게이트 실패를 구별한다. 모든 검사 프로세스는 종료했다. 기존748개 보존 hash와 최종 CMake 입력376개 drift0, 독립 결과 검산 통과. 새 빌드/native 회귀는 코드 변경이 없어 수행하지 않았다. commit/push·배포 없음.

## 캐시 정책 판정과 미검증

현재 SW는 network fetch 응답에 격리 헤더만 붙이며 CacheStorage 저장/삭제·오프라인 fallback·버전별 자산 묶음을 구현하지 않는다. **CacheStorage가 비었다고 HTTP cache까지 없다고 판단하지 않는다.** 로컬 SimpleHTTP 서버의 헤더는 실제 Pages/CDN 캐시 정책이 아니다. 주석-only SW 업데이트 통과는 JS/Wasm/pthread Worker/컴파일러 자산의 동시 버전 업데이트나 혼용 방지를 증명하지 않는다. 운영 캐시 정책 검증은 완료하지 못했다.

남은 항목은 등록404/거부·ready/controller timeout, 전체 오프라인 navigation·복구, cache quota/eviction·오염·자산 버전 혼용/rollback, 실행 중 업데이트·여러 탭·브라우저 재시작 재방문, Chromium/타 OS다. 실제 Pages URL·배포는 별도 승인 대상이다. P4 입력/resize·숨김/재개·반복 수명, context loss/OOM·추가 pthread·장기 누수도 그대로 남는다. 강제 종료를 정상 소멸·GPU 메모리 즉시 회수로 확대하지 않는다. 이번에는 브라우저 컴파일을 재실행하지 않았다.

## 다음 제안 — 승인 전 미구현

1. Pages+SW 우선 방향을 유지하며 bootstrap 복구부터 수정: 격리 성공 후 retry flag 정리, 등록/제어 대기 timeout과 오류 표시, 실패 후 명시적 재시도. 한 번의 복구 시도 안에서 reload 횟수 상한을 유지하고 전역 sessionStorage key 충돌을 피한다.
2. 수정 후 기존 재현을 통과시키고 실패·해제·여러 탭 행렬을 확대한다. 운영 캐시는 별도 선택: 초기 network-only로 오프라인 미지원과 HTTP 재검증 조건을 명시하거나, 버전 고정 자산 묶음과 manifest 무결성·이전 버전 회수 계약을 설계한다. 임의로 cache-first를 도입하지 않는다.
3. 위 로컬 게이트 이후에만 별도 승인으로 실제 Pages를 검증한다. 원본 Emception+Emscripten3.1.24·서버 없는 브라우저 컴파일·기본 E 및 독립 C 구성을 유지한다.
