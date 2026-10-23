# DALi 브라우저 포팅 현재 상태

갱신일: 2026-10-23 (Asia/Seoul). P0·R0·R1·R2-a·R2-b 및 상위 R2 완료. **현재 기록 기준은 dali-web/docs**, **P1 설계 초안 작성·결정 대기, P1 미완료/P2 미착수**입니다. [실행 계약](P1-EXECUTION-CONTRACT.md)의 D1 아키텍처는 확정했으며 D2~D4를 순서대로 논의합니다. [R2-b 결과](R2-b-RESULTS.md), [이관·증거 위치](MIGRATION.md), [계획](PLAN.md).

작업 repo는 `dali-web`, 원격은 https://github.com/dalihub/dali-web, branch main입니다. R2-a 기준 commit은 `728cf818d4db74890a4c37d01737c8eea3d9a14b`입니다. R2-b commit은 `ef6a1f545ff03df69c3297135048f545ee8dd352`이며 P1 시작 시 clean main과 원격 일치를 재확인했습니다. 시작할 때 실제 HEAD·status·원격을 다시 확인합니다.

## 현재 고정 소스 검증

| 항목 | 결과 | 범위 |
|---|---|---|
| Core Wasm·Node (R2-a) | 빌드 375단계·linker-test·애니메이션 50/100, 모두 종료 0 | 현재 lock Core + 생성 헤더 경로 CMake 옵션 |
| Firefox (R2-b) | 종료 0, 중간 50·최종 100 | R2-a 산출물, 모의 그래픽·단일 스레드 |
| Adaptor (R2-b) | 6개 번역 단위 각각 종료 0 | 현재 lock 소스, 전체 라이브러리 아님 |
| GL 탐색 (R2-b) | Python 종료 0, 두 링크 각각 종료 1 | 265개 심볼, 기본 7개/FULL_ES3 3개 미해결, P0와 동일 |
| 실제 DALi 픽셀·생산 렌더러·UI 결합 | 미검증 | P2/P3/P6 이후 |

[upstream lock](../upstreams.lock.json)과 [도구 lock](../toolchain.lock.json)은 변경하지 않았습니다. Core·Adaptor P0 tree는 현재와 다르며, P0는 역사적 비교 기준입니다. 브라우저는 Firefox 157.0.1·geckodriver 0.37.1입니다. 실제 도구 버전은 [증거](evidence/r2-b/versions.json)를 따릅니다.

## 보존·특이 사항·미결

1. 원본 Core·Adaptor·UI, P0 복제본의 HEAD·status·diff와 기존 로그·UI 사용자 파일 hash를 전후 비교해 보존했습니다. SDK는 기존 설치를 재사용했습니다.
2. sandbox 소켓·snap·DNS 제한은 호스트 실행으로 해결했습니다. 최초 실패도 새 `logs/r2-b/run-001/`에 보존합니다. 기존 P0·R2-a 로그는 이전 공간 그대로입니다.
3. GL 두 링크는 여전히 실패합니다. FULL_ES3 링크 지원과 버퍼 보존 의미는 별개이며 생산 Adaptor 전체·스레드·컨텍스트·고급 blend·셰이더 바이너리·UI·성능·Windows·네이티브 install은 미검증입니다.
4. SDK·산출물·프로필·대용량 로그는 commit 제외. 남긴 브라우저·서버·빌드 프로세스 없음. Pages 배포 없음.
5. P1 초안에 현재 소스 호출 순서·웹 변경 경계·지원 정책·P2 source closure 후보를 기록했습니다. D1은 공통 GLES + Native/EGL·Web 환경별 파일을 빌드 시 선택하는 정식 포팅 구조로 사용자 승인됐습니다. Core 계약과 기존 GLES/Vulkan/DYNAMIC을 유지합니다. D2 shadow upload 의미, D3 thread owner, D4 격리 호스팅은 미결이며 다음 논의는 D2입니다.
6. P1 특이 사항: Memory3의 WRITE-only 플래그는 lock SDK guard에서 0으로 거부됩니다. Memory2도 partial callback의 기존 byte를 자동 보존하지 않습니다. Controller/sync pool은 concrete EGL/native fence에 의존합니다. 생산 코드 수정 없이 설계에 반영했습니다.
7. P1 검사는 SDK access guard 추출 실행(Node 24.19.0)과 소스·보존·문서 검사입니다. 실제 GL·pthread·SAB·OffscreenCanvas·호스팅 검증은 하지 않았습니다. [증거](evidence/p1/README.md).

[HANDOFF](HANDOFF.md)의 P1 결정 게이트부터 이어갑니다. D2~D4 미결 상태에서는 P2 구현 진행 불가입니다. [다음 프롬프트](NEXT-CONTEXT.md). 계획 항목은 자동 실행 지시나 작업큐가 아닙니다.
