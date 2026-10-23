# 다음 context 인수인계: P1

2026-10-23. R2-b 및 R2 완료, P1 미착수. 기록 기준은 `dali-web/docs`입니다.

## 먼저 읽을 것

1. 작업 환경의 상위 AGENTS.md와 repo README.
2. [STATE](STATE.md), 이 문서, [PLAN](PLAN.md)의 P1 범위.
3. [R2-b 결과](R2-b-RESULTS.md), [이관/미이관 자료](MIGRATION.md), [Tizen 기준](TIZEN-BASELINE.md).
4. `upstreams.lock.json`, `toolchain.lock.json`, [BUILDING](BUILDING.md), [UPSTREAMS](UPSTREAMS.md).

## 현재 기준과 재현

작업 repo `/home/jy/work/dali/dali-web`, 원격 https://github.com/dalihub/dali-web, main. R2-a 기반은 `728cf818d4db74890a4c37d01737c8eea3d9a14b`. R2-b 문서·검증 commit은 Git log의 `Validate R2-b probes and migrate project records`입니다. 실제 HEAD·status·origin·원격 SHA를 먼저 확인하고 현재 lock을 유지합니다.

R2-a Core 빌드·Node 통과, R2-b Firefox 50/100·Adaptor 6개 컴파일 통과. GL 탐색 Python 0, 두 개별 링크 1, 265개 심볼 중 기본 7개/FULL_ES3 3개 미해결. P0와 동일한 결과지만 현재 SHA에서 재검증했습니다. 실제 DALi 렌더링 성공은 아닙니다.

Core 산출물은 `build/core/`, 새 전체 로그는 `logs/r2-b/run-001/`. 작은 증거는 `docs/evidence/r2-b/`. 기존 SDK·host 도구는 형제 `webassembly-probe/tools/`에 있으며 선택적으로 환경변수로 재사용합니다. 개인 경로는 필수 조건이 아닙니다. 재현 명령은 BUILDING의 R2-b 절차를 사용하고 새 실행 디렉터리를 지정합니다. Core 빌드가 필요하면 최대 -j6입니다.

## P1 작업 범위와 완료 조건

1. 현재 소스에서 Core 초기화 → GLES Controller → 컨텍스트/표면 → 프레임·리소스 업로드·해제 호출 순서를 추적하고 파일·함수 근거를 기록합니다.
2. 단일 Canvas·WebGL 2 초기 범위와 미지원 EGL pixmap/copy·고급 blend·셰이더 바이너리 정책을 검토합니다. FULL_ES3의 링크 결과를 올바른 메모리 매핑 의미로 가정하지 않습니다. Memory2 업로드와 Memory3의 데이터 보존 의미를 비교합니다.
3. 이벤트·업데이트·렌더 소유자와 스레드 후보, 브라우저 프레임 연결 및 검증 방법을 정리합니다. P0의 단일 스레드 harness를 생산 설계로 자동 채택하지 않습니다.
4. pthreads·SharedArrayBuffer·COOP/COEP 및 Pages 호스팅 제약을 근거로 확인합니다. 실험이 커지면 독립 단위로 나누고 미검증 가정을 명시합니다. Pages 배포는 D1입니다.
5. 실행 계약과 P2 최소 링크 대상·의존성·변경 파일 범위·검증 절차를 문서로 남기고, 설계 선택에 필요한 사용자 결정을 구분합니다. 생산 렌더러 신규 구현은 P2 이후입니다.

원본 UI 사용자 파일 `todo-items/README.md`, `todo-items/recycler-api-abi-refactoring.md`와 P0·R2-a 원본 증거를 보존합니다. 네이티브 build/install·UI 결합·Windows·성능·실제 픽셀은 미검증입니다. 이전 R2-b 시작 인수인계는 [보관 사본](archive/probe/docs/HANDOFF.md)에 있습니다. 남긴 실행 프로세스는 없습니다. P1을 이번 R2-b 작업에서 자동 실행하지 않습니다.
