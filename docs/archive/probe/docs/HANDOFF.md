# 다음 context 인수인계: R2-b

갱신일: 2026-10-23. R2-a 완료, R2-b 미착수. R2 전체는 아직 완료하지 않았습니다.

## 먼저 읽을 것

1. `/home/jy/work/dali/AGENTS.md`.
2. 기존 README·STATE·이 HANDOFF·PLAN의 R2-b 범위.
3. [R2-a 결과](R2-a-RESULTS.md), [Tizen 기준](TIZEN-BASELINE.md).
4. dali-web README·upstreams.lock.json·toolchain.lock.json·docs/BUILDING.md·docs/UPSTREAMS.md.

## 현재 기준

- 작업 repo: `/home/jy/work/dali/dali-web`, 원격 https://github.com/dalihub/dali-web, main.
- R2-a commit: `728cf818d4db74890a4c37d01737c8eea3d9a14b`. 실제 Git·원격 상태는 시작 시 재확인.
- lock은 Tizen 기준 그대로. Core·Adaptor는 P0와 다릅니다. Core는 새 빌드·Node 검사에서 50/100 통과했고 Adaptor 새 SHA는 미검증입니다.
- 기록 기준은 계속 `/home/jy/work/dali/webassembly-probe/docs`. R2-b 완료 조건을 충족한 뒤에만 새 docs로 전환합니다.
- 소스/도구: cmake/CMakeLists.txt, scripts/build-core.sh·env.sh, tests/probes/core-smoke.cpp. SDK는 기존 `webassembly-probe/tools/emsdk`, host 도구는 `webassembly-probe/tools/host/usr` 재사용. 개인 경로는 필수가 아니며 환경변수로 지정합니다.
- 산출물: `dali-web/build/core/`. 생성 namespace 헤더는 `generated/include` 아래. Core CMake 선택 옵션 외 upstream 코드 변경 없음.
- R2-a 로그: `webassembly-probe/logs/r2-a/run-001/`. 최초 P0 증거: `webassembly-probe/logs/`의 core-smoke.log·browser-result.json·adaptor-summary.txt·gl-symbols.json. 기존 자료를 덮어쓰지 않습니다.

## 다음 첫 행동과 범위

1. HEAD·status·origin·lock 및 원본 사용자 변경을 재확인하고 별도 R2-b 실행 디렉터리를 만듭니다. upstream 재갱신은 필요하지 않습니다.
2. docs/BUILDING.md대로 환경을 준비합니다. `DALI_WEB_RUN_DIR`를 새 R2-b 디렉터리로 지정하고 `source scripts/env.sh` 후 같은 셸에서 실행합니다. 빌드 산출물이 없거나 달라졌으면 새 Core 빌드를 먼저 수행합니다. 최대 -j6.
3. `python3 tests/browser/check-browser.py`로 Firefox Core 애니메이션을 검증합니다. mock 그래픽 결과이며 픽셀 검사로 판정하지 않습니다. geckodriver·Firefox 버전과 실행 명령·종료 코드를 기록합니다.
4. `scripts/probe-adaptor.sh`로 현재 Adaptor의 6개 번역 단위를 컴파일한 뒤 `python3 scripts/probe-gl-symbols.py`로 링크 탐색합니다. P0의 265개 심볼·기본 7개/FULL_ES3 3개 미해결과 비교합니다. 결과를 미리 동일하다고 가정하지 않습니다. Python 종료 0과 개별 링크 exitCode를 구분합니다.
5. 이관 파일·문서 링크·미이관 자료와 검증 한계를 정리합니다. R2-b 완료 시 새 repo docs로 기록 기준을 전환하고 기존 README·STATE·HANDOFF에는 새 경로를 안내합니다. P0와 R2 로그를 보존하고 commit에는 작은 검토된 증거만 포함합니다.
6. P1 실행 계약 설계 인수인계를 작성한 뒤 멈춥니다. 생산 렌더러 신규 구현·스레드 결정·Pages 배포는 이번 R2-b에 섞지 않습니다.

## 미검증과 남은 파일

브라우저 HTML·Python, Adaptor·GL 스크립트는 이관과 구문 검사만 했습니다. R2-a에서 브라우저·GL·Adaptor 실행은 없습니다. SDK 수동 다운로드 복구 스크립트는 기존 공간에만 보존했고 새 설치는 emsdk 고정 버전 절차를 문서화했습니다. 전체 P0 기록은 아직 이관하지 않았습니다. 현재 네이티브 install 회귀도 미검증입니다.

원본 UI 사용자 파일 `todo-items/README.md`, `todo-items/recycler-api-abi-refactoring.md`는 보존했습니다. 남긴 실행 프로세스는 없습니다. 이전 인수인계는 [007-R2-a](handoffs/007-R2-a.md), 다음 요청문은 [NEXT-CONTEXT](NEXT-CONTEXT.md)입니다.
