# R2 이관 및 증거 위치

2026-10-23 R2-b 완료 후 현재 기록 기준은 이 repo의 `docs/STATE.md`, `HANDOFF.md`, `PLAN.md`, `JOURNAL.md`입니다. 이전 공간을 삭제하지 않으며 두 공간을 동시에 최신 문서로 관리하지 않습니다.

## 코드와 문서

| 기존 webassembly-probe 경로 | dali-web 경로 | 점검 |
|---|---|---|
| CMakeLists.txt | cmake/CMakeLists.txt | libs 소스·binary 생성 헤더 경로, R2-a 빌드 통과 |
| core-smoke.cpp | tests/probes/core-smoke.cpp | byte 동일 |
| core-smoke.html | tests/browser/core-smoke.html | byte 동일, build 사본도 동일 |
| build-core.sh·env.sh | scripts/동일 이름 | 도구 탐색·외부 SDK·1~6 jobs, R2-a 검증 |
| check-browser.py | tests/browser/check-browser.py | 현재 Core 산출물로 Firefox 재검증 |
| probe-adaptor.sh | scripts/probe-adaptor.sh | 현재 소스 6개 컴파일, 명령·exit 기록 보강 |
| probe-gl-symbols.py | scripts/probe-gl-symbols.py | 현재 object에서 265개 심볼 탐색, 명령·exit 기록 보강 |
| docs/PLAN·STATE·HANDOFF·JOURNAL·NEXT-CONTEXT | docs/동일 이름.md | 현재 상태·P1 시작점으로 전환 |
| README·RESULTS·전체 기존 docs/handoffs | docs/archive/probe/ | 전환 직전 이력 보존, 상대 링크 재매핑 |
| 기존 logs/gl-symbols.json | docs/evidence/r2-b/p0-gl-symbols.json | 작은 P0 비교 증거, 원본 보존 |
| 새 R2-b 결과·버전·명령 | docs/evidence/r2-b/ | 작은 검토용 증거, 명령의 개인 경로는 변수 표기로 정규화 |

[원본/보관 사본 SHA256 목록](archive/probe-manifest.json)으로 전환 전 문서와 사본을 구별합니다. archive는 역사적 상태이며 현재 사실을 선언하지 않습니다. 새 archive의 문서 링크는 사본끼리 연결하고, repo 밖 증거는 로컬 보존 경로와 이 안내로 바꿨습니다. 기존 `docs/r1-a/`의 두 원문 스냅샷은 수정하지 않았습니다. 그 원문 외부 링크는 [기존 스냅샷 안내](r1-a/README.md)의 매핑을 따릅니다.

## 이관하지 않은 자료

| 자료 | 보존 위치와 이유 |
|---|---|
| P0 소스 복제본·생성 헤더·산출물 | `webassembly-probe/src/`, `build/`; 새 SHA 소스와 섞지 않음 |
| SDK·추출형 host 도구·다운로드 패키지 | `webassembly-probe/tools/`; 외부 도구로 선택적 재사용, commit 제외 |
| download-sdk.py·__pycache__ | 기존 공간 루트; 일회성 SDK 다운로드 복구 도구/캐시, 일반 설치 필수 아님 |
| P0·R1·R2-a 원본 전체 로그 | `webassembly-probe/logs/`, 특히 `logs/r2-a/run-001/`; 원본 그대로 로컬 보존 |
| R2-b 전체 로그·프로필 | `dali-web/logs/r2-b/run-001/`; 실패/성공 명령·브라우저·컴파일·링크·보존 검사, ignored |
| R1 검증 clone·임시 도구 | 과거 문서에 기록된 `/tmp` 위치; 후속 필수 입력 아님 |
| 원본 repo 사용자 변경 | 원래 위치; 새 repo에 복사하지 않음 |

기존 공간 경로는 이 작업 환경에서 repo의 형제 디렉터리입니다. 일반 clone에 이 로컬 증거·상위 AGENTS.md는 포함되지 않습니다. 일반 clone의 재현에는 [BUILDING](BUILDING.md)의 SDK 준비와 repo 소스만 필요합니다. 공개 증거는 작은 사본에 한정되며 전체 과거 로그 다운로드를 제공하지 않습니다.
