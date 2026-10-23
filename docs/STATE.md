# DALi 브라우저 포팅 현재 상태

갱신일: 2026-10-23 (Asia/Seoul). P0·R0·R1·R2-a·R2-b 및 상위 R2 완료. **현재 기록 기준은 dali-web/docs**, 다음 단계 P1은 미착수입니다. [R2-b 결과](R2-b-RESULTS.md), [이관·증거 위치](MIGRATION.md), [계획](PLAN.md).

작업 repo는 `dali-web`, 원격은 https://github.com/dalihub/dali-web, branch main입니다. R2-a 기준 commit은 `728cf818d4db74890a4c37d01737c8eea3d9a14b`입니다. R2-b commit은 Git 이력에서 `Validate R2-b probes and migrate project records` 제목으로 식별합니다. 시작할 때 실제 HEAD·status·원격을 다시 확인합니다.

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
5. P1에서 실행 계약·스레드/호스팅 조건·지원 기능·P2 파일 범위를 결정해야 합니다. 이번 작업은 설계 결정을 내리지 않았습니다.

[HANDOFF](HANDOFF.md)의 P1 범위에서 사용자 실행 지시 후 이어갑니다. [다음 프롬프트](NEXT-CONTEXT.md). 계획 항목은 자동 실행 지시나 작업큐가 아닙니다.
