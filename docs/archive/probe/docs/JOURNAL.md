# DALi 브라우저 포팅 작업 이력

## 001 초기 기술 검증

일자: 2026-10-22. 상태: P0 완료.

- 별도 작업 공간과 로컬 SDK, Core·Adaptor 복제본 준비.
- Core 소스 변경 없이 WebAssembly 빌드 및 Node·Firefox의 Actor 애니메이션 검사 통과.
- Adaptor 주요 6개 파일 컴파일 통과. GL/EGL 265개 심볼 링크 탐색에서 기본 7개, FULL_ES3 적용 시 3개 미해결.
- 특이 사항: GLES 3.2 헤더 포함은 실제 장애가 아니었음. 버퍼 매핑·셰이더 바이너리·공유 컨텍스트·생산 스레드는 후속 작업 대상.
- 근거와 재현: [최초 결과](../RESULTS.txt), [현재 상태](STATE.md)의 증거 링크.
- 검증 한계: 모의 그래픽 테스트이며 실제 픽셀·생산 Adaptor 전체·Windows는 미검증.

## 002 작업 계획과 인수인계 공간 작성

일자: 2026-10-22. 상태: 문서 작업 완료.

- 사용자 요청: 남은 작업 계획, 효율적인 context 전환 단위, 작업 기록 공간 마련.
- README와 PLAN·STATE·HANDOFF·JOURNAL, 과거 인수인계 보관 디렉터리 생성.
- P1 설계부터 P9 플레이그라운드까지 의존 관계와 증거 기반 완료 조건 정의.
- context 단위: 기본적으로 한 검증 결과가 완성되는 단계. 복잡한 단계는 재현 가능한 하위 결과로 분리.
- 기존 결과 JSON, revision, 복제본의 Git 상태를 확인해 현황 반영. 실행 테스트는 문서 변경 때문에 반복하지 않음.
- 남은 일: P1 이후 전체. 코드 구현·공개 배포·원격 저장소 생성은 이번 작업에 포함하지 않음.

## 003 Public 저장소 생성과 subtree 계획 반영

일자: 2026-10-22. 상태: R0 완료, R1·R2 계획 갱신 완료.

- 사용자 선택에 따라 `dalihub/dali-web` Public 빈 저장소 생성. `gh repo view dalihub/dali-web --json nameWithOwner,url,isPrivate,isEmpty` 종료 코드 0, `isPrivate=false`, `isEmpty=true` 확인.
- 단일 repo에 Core·Adaptor·UI를 subtree로 포함하는 방향 반영. 원본별 포크·submodule·대형 patch 배포 방식 대신 dali-web 자체에 웹 변경을 보관.
- R1 저장소 구성, R2 실험 이관·재검증을 P1 앞에 추가. D1 정적 예제 Pages 배포를 P4 뒤의 별도 작업으로 추가.
- context 이동 순서: R1 → R2 → P1 → 기존 렌더링 단계. 이관 후 기록 기준 전환 조건과 이전 증거 보존 절차 명시.
- 미결 사항: 원본 URL·import SHA·squash 정책, Pages와 스레드 모델의 호환성. 현재 Tizen 기반 SHA와 GitHub 미러를 동일하다고 가정하지 않음.
- 이전 인수인계: [002-P1](handoffs/002-P1.md). README·PLAN·STATE·HANDOFF 동기화.
- 검증: 문서 링크·단계 선행 관계 확인. 문서 변경으로 코드 테스트를 반복하지 않음.
- 이번 변경은 로컬 문서만 해당. 소스 import·이관·추가 원격 쓰기·Pages 배포는 실행하지 않음.

## 004 context 세분화와 성능 검증 계획

일자: 2026-10-22. 상태: 문서 갱신 완료.

- 사용자 요청에 따라 가까운 R1·R2를 각각 a·b로 분리하고 이후 단계는 조정 가능한 분할 후보로 기록.
- 구현과 기본 검증을 한 context에서 완료하며, 오류별로 기계적으로 분리하지 않는 원칙 명시.
- P3 이후 최적화 빌드의 네이티브·웹 비교, 로딩·연속 프레임·컴파일 시간 분리, 프레임 지연 분포와 GPU 가속 조건 기록 계획 추가.
- 현재 렌더링 성능 측정값 없음. 모의 그래픽·Debug 결과를 성능 근거로 사용하지 않음.
- 이전 인수인계는 [003-R1](handoffs/003-R1.md)에 보관. 다음 시작점을 R1-a로 동기화.
- 문서 링크와 하위 단계 상태 확인. 코드·subtree·원격 저장소는 변경하지 않음.

## 005 R1-a 원본·기준 commit·subtree 정책 확정

일자: 2026-10-22. 상태: R1-a 완료, R1-b 미착수.

- 사용자 지정 범위에 따라 원본 선정·P0 비교·이력 정책·R1-b 인수인계만 수행. import·코드 이관·commit·push 없음.
- [R1-a 결정](R1-a-UPSTREAMS.md)에 공개 URL·full SHA·tree·태그와 R1-b lock·파일 목록·remote 복원 계약 기록. README·PLAN·STATE·HANDOFF 동기화.
- Core·Adaptor는 공개 `dali_2.5.43` peeled commit 선정. P0와 SHA는 다르지만 tree 일치, `git diff --exit-code` 0, ancestor 검사 0. UI는 공개 `v2.5.43.11572` 선정, P0 UI 검증 없음.
- 최초·후속 모두 `--squash` 확정. 포팅 이력과 원본 갱신 분리, 일반 clone 이력 부담 제한이 이유. 상세 과거 이력은 upstream에서 조회하며 lock·subtree 메타데이터 보존. non-squash와 계정 의존 Tizen SSH 원본은 기각.
- 검증: `git ls-remote` 네 repo 모두 종료 0, dali-web refs 없음; GitHub get_repo public 확인; 세 선정 commit API 조회 성공. 증거 (로컬 보존 자료: `webassembly-probe/logs/r1-a/local-verification.json`; [위치 안내](../../../MIGRATION.md)), 원격 검사 (로컬 보존 자료: `webassembly-probe/logs/r1-a/remote-checks.txt`; [위치 안내](../../../MIGRATION.md)). 문서 링크·SHA·상태 일관성 검사 수행.
- 특이 사항: P0 SHA의 GitHub API 조회는 각각 422. 동일 tree의 공개 commit으로 해결. 초기 sandbox DNS 오류(128)는 네트워크 허용 재조회로 해결. UI 최신 devel은 2.5.44 계열이라 채택하지 않음. 로컬 UI 사용자 변경은 보존.
- 생성물: `logs/r1-a/`, 결정 문서, [이전 인수인계](handoffs/004-R1-a.md). UI 비교용 임시 bare repo `/tmp/dali-r1a-ui-Tiv4hU`만 추가, 원본 repo와 P0 코드·로그는 변경하지 않음. 남긴 실행 프로세스 없음.
- 검증 한계: 빌드·Node·Firefox 재실행 없음. tree 동등성은 UI 결합 빌드나 실제 렌더링 검증이 아님.
- 다음 행동: R1-b 실행 지시 후 대상 경로·remote 상태 재확인. subtree 자체의 commit 생성과 사용자 허용 범위를 확인하고 진행. R2-b 전 기록 기준은 기존 공간 유지.

## 006 R1-b 저장소 뼈대·subtree·검증·push

일자: 2026-10-22. 상태: R1-b 및 상위 R1 완료.

- 사용자 허가 범위에서 새 dali-web main 생성, R1-a 고정 URL·full SHA로 Core·Adaptor·UI 전체 tree squash import, lock·remote 스크립트·라이선스 목록·루트 생성물 제외 규칙 구성.
- R1-a 결정·당시 HANDOFF를 새 repo의 역사적 스냅샷으로 포함. 기록 기준은 기존 docs 유지. [이전 인수인계](handoffs/005-R1-b.md) 보관 후 R2-a HANDOFF·다음 프롬프트 작성.
- 별도 로컬 일반 clone 소스·tree·lock·라이선스·ignore·commit trailer 검증 PASS. remote 반복 실행 및 URL 충돌 시 무변경 확인. 원본 5개 repo HEAD·status 및 사용자 파일·기존 P0 로그 보존 확인.
- 8개 생성 commit의 trailer 정확히 1회, subtree 메타데이터 보존. 최종 `888ff1423ca205bbaf65c493ee8d5771257f9a6a`를 origin/main으로 force 없이 push, 원격 SHA 검증 완료.
- 특이 사항: sandbox DNS는 네트워크 허용으로 해결. subtree 내부 commit-tree는 일반 hook을 거치지 않아 임시 실행 복사본과 메시지 wrapper로 생성 전 trailer 삽입·검증. 설치 파일·upstream 소스는 변경하지 않음.
- 상세 명령·commit·검증 결과·증거: [R1-b 결과](R1-b-RESULTS.md), `logs/r1-b/`. 검증 clone `/tmp/dali-r1b-verify-clone` 및 임시 wrapper 보존, 남긴 실행 프로세스 없음.
- 실험 이관·빌드·Node·브라우저 실행·Pages 배포 없음. 다음은 R2-a이며 이번 작업은 여기서 중단.

## 007 R1-b 후속: Tizen 최신 tip으로 고정 기준 변경

일자: 2026-10-22. 상태: 완료.

- 사용자 요청에 따라 세 원본의 Tizen 공식 `refs/heads/tizen` tip을 조회·fetch한 뒤 전체 tree squash merge. 기존 GitHub 릴리스 이력은 유지.
- lock·upstream URL·remote·라이선스 목록·기준 문서 갱신. Core·Adaptor의 P0 tree 동등성은 false로 변경. 새 SHA의 실행 검증은 R2에 남김.
- 별도 일반 clone 검증 PASS, 생성 commit trailer 검사 및 원본/P0 자료 보존 확인. 최종 `888e8e7e1409c1637f6cff8f1024e77c34b43728`를 force 없이 push하고 원격 SHA 확인.
- 특이 사항: HTTPS 403·일시 DNS 실패 후 기존 SSH 연결로 성공. 개인 계정명은 lock에서 제외, 접근 설정 필요성 문서화.
- 상세 결과 [Tizen 기준](TIZEN-BASELINE.md), 증거 `logs/r1-b-tizen/`. 기존 HANDOFF는 [006-R2-a-before-tizen](handoffs/006-R2-a-before-tizen.md)에 보관.
- 실험 이관·빌드·Node·브라우저 실행 없음. R2-a 시작 전 멈춤. 남긴 실행 프로세스 없음.

## 008 R2-a 빌드·실험 경로 이관과 현재 Core 검증

일자: 2026-10-23. 상태: R2-a 완료, R2-b 미착수.

- 기존 실험의 의존성을 확인하고 새 repo cmake·scripts·tests에 이관. src/dali-*를 libs/dali-*로 전환, toolchain.lock.json·BUILDING.md에 버전·탐색·오버라이드 기록.
- 새 고정 Core SHA + 생성 헤더 경로 옵션으로 375단계 Debug Wasm 빌드, Node linker-test 및 애니메이션 50/100 모두 종료 0. 최대 --parallel 6. 전체 명령·exit·tee 결과는 logs/r2-a/run-001/commands.log.
- 특이 사항: upstream Core가 source에 namespace 헤더를 생성하므로 선택적 DALI_GENERATED_INCLUDE_DIR 추가. 웹 build/generated/include로 분리, 네이티브 기본 경로 유지. API·ABI 변경 없음, 네이티브 build/install 미검증.
- 원본/P0 HEAD·status·diff와 기존 로그·UI 사용자 파일 hash 불변, 구문·ignore·diff 검사 통과. upstream lock 갱신 없음. SDK는 기존 설치 재사용.
- main commit `728cf818d4db74890a4c37d01737c8eea3d9a14b`, AI co-author trailer 정확히 1회 확인. force 없이 push, 원격 SHA 일치 확인. 최초 sandbox DNS 실패(128), 네트워크 허용 재시도 성공(0).
- 상세 파일 목록·명령·검증 한계: [R2-a 결과](R2-a-RESULTS.md). 이전 HANDOFF는 [007-R2-a](handoffs/007-R2-a.md)에 보존. STATE·PLAN·README·현재 HANDOFF·NEXT-CONTEXT 갱신.
- 브라우저·Adaptor·GL은 코드 이관/구문 확인만 수행, 실행 없음. 기록 기준은 기존 docs 유지. 생산 렌더러·스레드·배포는 진행하지 않음.
- 생성물: dali-web/build/core, 기존 logs/r2-a/run-001. SDK·산출물·프로필·대용량 로그 commit 없음. 남긴 실행 프로세스 없음. R2-a 완료 후 중단, 다음은 R2-b.

## 후속 작업 기록 형식

다음 번호의 항목을 추가하고 아래 정보를 남깁니다.

- 일자, 단계와 하위 목표, 상태.
- 변경 파일과 revision 또는 보존한 diff 경로.
- 선택한 방안과 이유, 중요한 실패 시도와 재현 방법.
- 검증 명령, 종료 코드, 로그·이미지 등 증거 경로, 미검증 범위.
- 특이 사항과 합의한 설계에 미친 영향.
- 남은 일과 다음 한 가지 행동.
- 남긴 프로세스·생성물·환경 변경, 해당 HANDOFF 보관 경로.
