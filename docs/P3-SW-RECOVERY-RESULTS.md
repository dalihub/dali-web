# P3 SW bootstrap 복구 결과

2026-11-06. 사용자 “수정 고고” 승인에 따라 [해제 후 재방문 실패](P3-SW-LIFECYCLE-RESULTS.md)를 수정했다. **로컬 Firefox의 bootstrap 복구 단위 완료, P3 전체 진행 중.** [증거](evidence/p3-sw-recovery/README.md).

## 구현

1. `tests/browser/production.js`에서 격리 성공 시 reload flag를 제거한다. 저장소 키는 SW의 절대 URL별로 분리해 다른 경로와 충돌하지 않게 했다. 첫 격리 관측값은 검사 증거용으로 유지한다.
2. 한 시도에서 자동 reload는 최대1회다. reload 후에도 격리되지 않으면 runtime을 시작하지 않고 오류를 표시한다. 등록·ready·controller 대기 전체에5초 timeout을 적용한다. timeout 뒤 늦은 비동기 완료가 listener를 추가하거나 reload를 재개하지 않게 하고 timer/listener를 정리한다. 브라우저 자체의 SW 설치 작업을 취소하는 구현은 아니다.
3. `production.html`에 오류 표시와 ‘다시 시도’ 버튼을 추가했다. 버튼은 해당 시도의 저장 상태를 초기화하고 새 navigation으로 명시적 복구를 시작한다. 자동 무한 재시도나 SW 자동 unregister는 하지 않는다.
4. 기존 SW 원본·기본 E/C host·C++·SDK·lock·컴파일 방향은 유지했다. 수정 대상은 로컬 P3 fixture/bootstrap과 테스트다. 실제 배포용 bootstrap으로 일반화하지 않았다.

## 검증

Firefox157.0.1, 기존 동일3.1.24 `build/p3/event-pthread-cmake-001` E readback 산출물. 새 로그 `logs/p3/sw-recovery-003`, 전체 exit0. 직접 헤더 회귀는 `sw-recovery-headers-001`, exit0.

| 검사 | 실제 결과 |
|---|---|
| 최초·재방문·업데이트·503 업데이트 실패 후 | 기존4회 렌더 회귀 통과 |
| unregister→scope 밖 이동→재방문 | 재등록·격리 복구·실제 렌더, 성공 후 reload flag=null |
| 최초 등록503 | 오류/버튼 표시, runtime 미시작, 자동 reload0. 주입 해제 후 버튼 재시도 렌더 통과 |
| SW가 claim하지 않음 | 5초 timeout·오류/버튼, 자동 reload0. 잘못된 등록 제거 후 재시도 통과 |
| SW 제어6.5초 지연 | 5초 timeout 후 실제 controller 도착, 오류 상태 유지·자동 reload0. 명시적 재시도 통과 |
| 격리 헤더 없는 SW | 자동 reload 정확히1회 뒤 명시 오류, 반복 reload 없음. 잘못된 등록 제거 후 재시도 통과 |
| 직접 COOP/COEP | SW 제어 없이 E 두 프레임/GL error0·ready→stopped |

SW 행렬 정상 실행은 총9회다. 각각 공유 메모리·격리·실제 두 프레임 픽셀·GL error0·ready→stopped를 확인했다. 실패4종은 오류를 기대한 검사이며 정상 렌더 횟수에 포함하지 않는다. 재시도 버튼은 programmatic click이다. 잘못된 SW fixture는 harness가 unregister하고 원본 응답으로 복원한 뒤 버튼을 눌렀다. bootstrap 자체가 잘못된 SW 코드/배포를 자동 수리한다는 증거는 아니다.

기존748개 보존 hash 및 최종 CMake 입력376개 drift0. JS/Python 구문·JSON·문서 링크·diff 검사 통과. 새 C++ 빌드와 native 회귀·C 행렬 재실행은 하지 않았다. 브라우저·driver·loopback 서버는 종료했다. commit/push·배포 없음.

## 특이 사항

1. `sw-recovery-001`은 WebDriver에서 페이지 전역 const를 참조한 검사 오류로 중단했다. 저장소 키를 명시해 수정했다. bootstrap의 해제 후 렌더는 이미 통과했다.
2. `002`는 HTTP cache 때문에 서버 요청 수가 navigation 횟수보다 적어 계수 assertion에서 중단했다. `003` 검사 서버의 HTML에만 no-store를 붙여 실제 요청으로 reload 상한을 검사했다. 이 헤더를 운영/Pages 캐시 정책으로 채택하지 않았다. 두 중간 실패는 보존하고 성공 집계에 포함하지 않는다.
3. 5초 timeout은 이 로컬 fixture의 유한 대기 기준이다. 느린 네트워크의 제품 SLA나 적정 timeout을 검증한 것은 아니다. timeout 후 SW 설치가 계속될 수 있으나 자동 실행은 재개하지 않는다.

## 남은 범위와 다음 작업

기존 복구 승인 대기는 해소됐지만 전체 SW 게이트는 완료하지 않았다. 운영 캐시 정책·HTTP cache와 자산 버전 혼용/rollback·전체 offline/복구·다중 탭·실행 중 update·브라우저 재시작·Chromium·실제 Pages가 남는다. 다음은 network-only 또는 버전별 자산 묶음의 캐시 계약을 정하고 그 정책을 검증하는 작업이다. 실제 Pages 검증·배포는 별도 승인이다.

기본 E와 독립 C 조기 성공을 유지한다. P4 시간/입력/resize·숨김/재개·반복/실패 수명, 추가 pthread·context loss/OOM·장기 누수는 미검증이다. 강제 종료를 정상 소멸·GPU 메모리 즉시 회수로 확대하지 않는다. 원본 Emception+Emscripten3.1.24·서버 없는 브라우저 컴파일·Pages+SW 우선 방향은 유지한다.
