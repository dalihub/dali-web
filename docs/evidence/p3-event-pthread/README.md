# app 이벤트 pthread C 조기 검사 증거

2026-11-06. [구현·검증·특이 사항·제한](../../P3-EVENT-PTHREAD-RESULTS.md). [재현](../../BUILDING.md#p3p4-독립-c-이벤트-pthread-검사).

- `headers/sw-result`: 최종 CMake 산출물의 실제 브라우저 결과. 각각 정상(0)→초기화 loop(1)→timer callback loop(2)→새 정상(0). 오류 없음, 새 메모리/Canvas, 늦은 메시지 차단, 모든 run passed.
- `headers/sw-verdict`: 각각 exit0에 해당하는 판정. `headers/sw-inputs`: 최종 fixture/runtime SHA256.
- `e-pixels-*`: 새 전체 CMake 기본 E의 실제2프레임/GL error0·ready→stopped.
- `e-production-*`: 계측 없는 E compile/nm/link 및 browser 성공. 이 실행은 자체 픽셀 검사를 하지 않는다.
- `reuse-build-commands`: 중간 최종004 host compile/nm/strict 링크. 기존 renderer/Core 재사용 최적화 경로이며 최종 C 인수는 새 전체 CMake 빌드다.
- `cmake-commands`, `cmake-artifacts`: 실제 전체 configure/build 명령·종료, 로그와 E/C 산출물 hash. 큰 로그·map·SDK는 로컬에만 둔다.
- `sources/closure-commands`: GLES/VULKAN/DYNAMIC 집합·Web closure·독립 generated 경로 회귀. 예상된 잘못된 조합 거부 exit1과 실제 실패를 구분한다.
- `build-inputs`: 전체 compile entry와 host/test/CMake 등의 최종 source hash snapshot. 전체 SDK/header dependency lock을 대신하지 않는다.
- `validation`: 기존748개 보존, 이전499개 중 승인된 host/CMake 변경2개만 존재, 재사용 빌드 입력 drift0, 개별 판정과 제한.

`state` 배열은 `web/host/web-event-probe.h`의 ProbeField 순서다:

| index | 의미 |
|---|---|
| 0 | mode: 정상0/init loop1/timer callback loop2 |
| 1,2 | event/render pthread ID |
| 3,4 | Core TLS 생성/정상 제거 |
| 5,6 | timer/trigger 실행 |
| 7,8 | event/render pump 카운터 |
| 9,10 | 무한 루프 진입/spin 카운터 |
| 11,12 | 정상 context Close 완료/event pthread exit 준비 |
| 13,14 | main의 stop/trigger 요청 |
| 15,16 | main rAF timestamp/pending |
| 17,18 | key 요청/실제 DALi signal의 keyCode |
| 19 | browser main pthread ID |

서로 다른 Module은 별도 공유 메모리이므로 같은 숫자 thread ID를 가질 수 있다. 한 Module 안의 main/event/render ID가 서로 다름을 검사한다. 모듈 간 숫자 ID 재사용을 Worker/Module 재사용으로 해석하지 않는다.

무한 루프는 진입 flag만으로 판정하지 않는다.600ms 동안 spin/render counter 변화와 main heartbeat(최종29~30회)를 함께 확인했다. 종료 요청 뒤150ms를 기다리고 추가350ms 동안 전체 shared counter 불변을 확인했다. 정상 실행은 TLS 제거·context Close·pthread reap도 통과해야 한다. 강제 종료에서 해당 flags가0인 것은 예상된 결과이며 정상 소멸을 주장하지 않는다. 지연 메시지 차단은 종료된 receiver에 주입한 별도 검사다.

최종 로컬 경로:

1. `build/p3/event-pthread-cmake-001`, `logs/p3/event-pthread-cmake-001` — 새 전체371단계. C runtime은 build의 `event-probe/`.
2. `logs/p3/event-pthread-final-{headers,sw}-001` — 최종 C 행렬.
3. `logs/p3/event-pthread-e-regression-001` — 기본 E 픽셀 회귀.
4. `build/p3/event-pthread-e-production-002`, `logs/p3/event-pthread-e-production-002`, `logs/p3/event-pthread-e-production-browser-002` — 계측 없는 E.
5. `logs/p3/event-pthread-{sources,closure}-001` — source 회귀.

중간 실패는 결과 문서에 구별해 보존했다. 전체 프로필·산출물은 추적하지 않는다. 검증은 로컬 Firefox,2개 pthread, synthetic 입력/중단에 한정하며 Chromium·GPU 메모리 회수/장기 누수·추가 동적 pthread·실제 Pages 배포는 미검증이다.
